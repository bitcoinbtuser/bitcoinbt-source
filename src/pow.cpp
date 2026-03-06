// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <primitives/block.h>
#include <uint256.h>
#include <util/check.h>
#include <logging.h>
#include <util/time.h>
#include <algorithm> // for min/max if needed

// === 내부 함수 선언 ===
static unsigned int GetNextWorkRequired_Legacy(const CBlockIndex* pindexLast, const CBlockHeader* pblock, const Consensus::Params& params);
static unsigned int GetNextWorkRequired_ASERT (const CBlockIndex* pindexLast, const CBlockHeader* pblock, const Consensus::Params& params);

// ============================================================================
// BCHN ASERTi3-2d core (정식 고정소수점 Q16 구현)
// next_target = ref_target * 2^((timeDiff - T*(heightDiff+1)) / halfLife)
// - timeDiff   : (prev_time - anchor_time)
// - heightDiff : (prev_height - anchor_height)
// - (heightDiff + 1)로 "다음 블록"을 반영 (정식 스펙)
// ============================================================================
static arith_uint256 CalculateASERT_BCHN(const arith_uint256& refTarget,
                                        const int64_t targetSpacing,
                                        const int64_t timeDiff,
                                        const int64_t heightDiff,
                                        const arith_uint256& powLimit,
                                        const int64_t halfLife)
{
    Assert(refTarget > 0 && refTarget <= powLimit);
    // Need some leading zero bits in powLimit to handle overflow safely (BCHN 관례)
    Assert((powLimit >> 224) == 0);
    Assert(heightDiff >= 0);
    Assert(halfLife > 0);
    Assert(targetSpacing > 0);

    // exponent_fp = ((timeDiff - T*(heightDiff+1)) * 65536) / halfLife
    // (BCHN은 arithmetic shift(부호 유지) 가정)
    static_assert(int64_t(-1) >> 1 == int64_t(-1), "ASERT requires arithmetic right shift");

const __int128 num = (__int128)(timeDiff - targetSpacing * (heightDiff + 1)) * (__int128)65536;
const int64_t exponent_fp = (int64_t)(num / (__int128)halfLife);

int64_t shifts = exponent_fp >> 16;            // integer part
const uint16_t frac = uint16_t(exponent_fp);   // fractional part [0..65535]
Assert(exponent_fp == (shifts * 65536) + frac);

    // factor = 2^(frac/65536) in Q16, BCHN polynomial approximation
    // 2^x ~= 1 + 0.695502049*x + 0.2262698*x^2 + 0.0782318*x^3  (0<=x<1)
const uint64_t f = (uint64_t)frac;
const __int128 f2 = (__int128)f * (__int128)f;
const __int128 f3 = f2 * (__int128)f;

const uint32_t factor = 65536 + (uint32_t)(((
    + (__int128)195766423245049ULL * (__int128)f
    + (__int128)971821376ULL       * f2
    + (__int128)5127ULL            * f3
    + ((__int128)1 << 47)
) >> 48));

    arith_uint256 nextTarget = refTarget * factor;

    // Apply 2^(integer part) / 65536
    shifts -= 16;
    if (shifts <= 0) {
        nextTarget >>= -shifts;
    } else {
        const arith_uint256 shifted = nextTarget << shifts;
        if ((shifted >> shifts) != nextTarget) {
            nextTarget = powLimit;
        } else {
            nextTarget = shifted;
        }
    }

    if (nextTarget == 0) {
        nextTarget = arith_uint256(1);
    } else if (nextTarget > powLimit) {
        nextTarget = powLimit;
    }

    return nextTarget;
}

// ============================================================================
// 포크 전에는 레거시(BTC) DAA, 포크 후에는 (앵커가 준비된 경우에만) ASERT 사용.
// 앵커가 없으면 포크 후에도 레거시로 fallback.
// (선택) fork+1(=903,845) 특별 블록만 powLimit로 쉽게 허용 라인은 유지.
// ============================================================================
unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast,
                                 const CBlockHeader* pblock,
                                 const Consensus::Params& params)
{
    Assert(pindexLast != nullptr);

    const int next_height = pindexLast->nHeight + 1;
    const int fork_h      = params.btcbt_fork_block_height;

    // ★ 포크 전(<= fork_h)은 무조건 레거시 DAA
    if (next_height <= fork_h) {
        LogPrintf("POWDBG[REQ]: h=%d fork_h=%d path=LEGACY\n", next_height, fork_h);
        unsigned int ret = GetNextWorkRequired_Legacy(pindexLast, pblock, params);
        if (ret == 0) {
            ret = UintToArith256(params.powLimit).GetCompact();
            LogPrintf("POWDBG[CLAMP]: legacy returned 0 at h=%d -> powLimit=%08x\n", next_height, ret);
        }
        return ret;
    }

    LogPrintf("POWDBG[REQ]: h=%d fork_h=%d path=POST_FORK\n", next_height, fork_h);

    // (필수) 포크+1 완화: 첫 블록은 powLimit 허용 (체인 부팅용)
    if (next_height == fork_h + 1) {
        unsigned int ret = UintToArith256(params.powLimit).GetCompact();
        if (ret == 0) {
            ret = 0x1d00ffff; // 절대 0이 되지 않는 안전 값
            LogPrintf("POWDBG[CLAMP]: fork+1 powLimit compact=0 -> fallback=%08x\n", ret);
        }
        return ret;
    }

    // warmup 종료 높이: 앵커 높이와 동기화
    const int warmup_end = std::max(params.btcbt_asert_anchor_height, fork_h + 1);
    if (next_height <= warmup_end) {
        unsigned int ret = UintToArith256(params.powLimit).GetCompact();
        if (ret == 0) {
            ret = 0x1d00ffff;
            LogPrintf("POWDBG[CLAMP]: post_fork warmup powLimit compact=0 -> fallback=%08x\n", ret);
        }
        LogPrintf("POWDBG[WARMUP]: h=%d <= warmup_end=%d -> powLimit=%08x\n", next_height, warmup_end, ret);
        return ret;
    }

    // 포크 후: ASERT 앵커 준비 확인
    const bool asert_ready =
        (params.btcbt_asert_anchor_height >= 0) &&
        (params.btcbt_asert_anchor_bits   != 0);

    if (!asert_ready) {
        LogPrintf("POWDBG[REQ]: h=%d ASERT anchor_ready=0 -> LEGACY\n", next_height);
        unsigned int ret = GetNextWorkRequired_Legacy(pindexLast, pblock, params);
                if (ret == 0) {
            ret = UintToArith256(params.powLimit).GetCompact();
            LogPrintf("POWDBG[CLAMP]: legacy fallback returned 0 at h=%d -> powLimit=%08x\n", next_height, ret);
        }
        return ret;
    }

    // ===== BTCBT 합의 고정: 첫 ASERT 블록(=anchor+1) bits 튐 방지 =====
    const int anchor_h = std::max(params.btcbt_asert_anchor_height, fork_h + 1);
    unsigned int anchor_bits = params.btcbt_asert_anchor_bits;
    if (anchor_bits == 0) anchor_bits = UintToArith256(params.powLimit).GetCompact();

    if (next_height == anchor_h + 1) {
        LogPrintf("POWDBG[FIX]: h=%d == anchor_h+1=%d -> fixed_bits=%08x\n",
                  next_height, anchor_h + 1, (unsigned)anchor_bits);
        return anchor_bits;
    }

    // 포크 후 + 앵커 준비 완료 → ASERT
    unsigned int ret = GetNextWorkRequired_ASERT(pindexLast, pblock, params);
    if (ret == 0) {
        arith_uint256 min_target = arith_uint256(1);
        ret = min_target.GetCompact();
        LogPrintf("POWDBG[CLAMP]: ASERT returned 0 at h=%d -> min_target=%08x\n", next_height, ret);
    }
    return ret;
}

// ============================================================================
// (비트코인 레거시) 다음 작업 난이도 계산
//   nFirstBlockTime: 윈도우 시작 블록의 타임스탬프
// ============================================================================
unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    Assert(pindexLast != nullptr);

    if (params.fPowNoRetargeting) {
        return pindexLast->nBits;
    }

    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;

    int64_t nTargetTimespan = params.nPowTargetTimespan;
    if (nTargetTimespan <= 0) {
        return pindexLast->nBits;
    }

    const int64_t nMinTimespan = nTargetTimespan / 4;
    const int64_t nMaxTimespan = nTargetTimespan * 4;
    if (nActualTimespan < nMinTimespan) nActualTimespan = nMinTimespan;
    if (nActualTimespan > nMaxTimespan) nActualTimespan = nMaxTimespan;

    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

    const arith_uint256 powLimit = UintToArith256(params.powLimit);
    if (bnNew > powLimit) bnNew = powLimit;

    return bnNew.GetCompact();
}

// ============================================================================
// 레거시 경로
// ============================================================================
static unsigned int GetNextWorkRequired_Legacy(const CBlockIndex* pindexLast, const CBlockHeader* pblock, const Consensus::Params& params)
{
    Assert(pindexLast != nullptr);
    const int next_height = pindexLast->nHeight + 1;

    if (params.fPowNoRetargeting) {
        return pindexLast->nBits;
    }

    if ((next_height % params.DifficultyAdjustmentInterval()) != 0) {
        if (params.fPowAllowMinDifficultyBlocks) {
            if (pblock && pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing * 2) {
                return UintToArith256(params.powLimit).GetCompact();
            }
            const CBlockIndex* pindex = pindexLast;
            while (pindex->pprev &&
                   (pindex->nHeight % params.DifficultyAdjustmentInterval()) != 0 &&
                   pindex->nBits == UintToArith256(params.powLimit).GetCompact()) {
                pindex = pindex->pprev;
            }
            return pindex->nBits;
        }
        return pindexLast->nBits;
    }

    const CBlockIndex* pindexFirst = pindexLast;
    for (int i = 0; i < params.DifficultyAdjustmentInterval() - 1; ++i) {
        pindexFirst = pindexFirst->pprev;
        Assert(pindexFirst != nullptr);
    }

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

// ============================================================================
// ASERT 경로 (BCHN 방식으로 결정론적 계산)
// - 후보 블록 시간(pblock/now) 사용 금지
// - prev(pindexLast) 기준 timeDiff/heightDiff 계산
// ============================================================================
static unsigned int GetNextWorkRequired_ASERT(const CBlockIndex* pindexLast, const CBlockHeader* /*pblock*/, const Consensus::Params& params)
{
    Assert(pindexLast != nullptr);

    const int fork_h = params.btcbt_fork_block_height;

    const int anchor_h = std::max(params.btcbt_asert_anchor_height, fork_h + 1);

    unsigned int anchor_bits = params.btcbt_asert_anchor_bits;
    if (anchor_bits == 0) {
        anchor_bits = UintToArith256(params.powLimit).GetCompact();
    }

    const CBlockIndex* anchor = pindexLast->GetAncestor(anchor_h);
    if (!anchor || anchor->nHeight != anchor_h) {
        return anchor_bits; // 앵커 탐색 실패 시 anchor_bits로 단일화
    }

    // ===== BTCBT 합의 고정: 첫 ASERT 계산(=anchor 다음 블록)에선 변화 0 =====
    // prev가 anchor 자체면(next block이 anchor+1) 무조건 anchor_bits 반환
    if (pindexLast->nHeight == anchor_h) {
        LogPrintf("POWDBG[FIX]: ASERT first-after-anchor prev_h=%d anchor_h=%d -> fixed_bits=%08x\n",
                  (int)pindexLast->nHeight, (int)anchor_h, (unsigned)anchor_bits);
        return anchor_bits;
    }

    int64_t T = (params.btcbt_block_interval > 0) ? params.btcbt_block_interval : params.nPowTargetSpacing;
    if (T <= 0) return anchor_bits;

    const arith_uint256 powLimit = UintToArith256(params.powLimit);
    arith_uint256 target_ref;
target_ref.SetCompact(anchor_bits);

const int64_t half_life = (params.btcbt_asert_half_life > 0) ? params.btcbt_asert_half_life : 172800;

// ✅ BCHN 정식: time_delta = eval_time - anchor_parent_time
// ⚠ BTCBT 포크 직후(앵커=903845)에는 903844(비트코인 과거 시간)를 부모로 쓰면 time_diff가 비정상적으로 커져
// 항상 powLimit로 clamp되어 bits가 1d00ffff에 고정될 수 있음.
// 따라서 포크+1 앵커인 경우, 가상 부모 시간을 (anchor_time - T)로 고정한다.
const int64_t anchor_time = anchor->GetBlockTime();

int64_t anchor_parent_time = 0;
if (anchor->nHeight == fork_h + 1) {
    anchor_parent_time = anchor_time - T;
} else {
    if (anchor->pprev == nullptr) return anchor_bits;
    anchor_parent_time = anchor->pprev->GetBlockTime();
}

const int64_t time_diff   = pindexLast->GetBlockTime() - anchor_parent_time;
const int64_t height_diff = pindexLast->nHeight       - anchor->nHeight;

LogPrintf("POWDBG[ASERT]: prev_h=%d anchor_h=%d anchor_parent_time=%lld prev_time=%lld time_diff=%lld height_diff=%lld T=%lld half_life=%lld anchor_bits=%08x\n",
          (int)pindexLast->nHeight, (int)anchor->nHeight,
          (long long)anchor_parent_time, (long long)pindexLast->GetBlockTime(),
          (long long)time_diff, (long long)height_diff,
          (long long)T, (long long)half_life, (unsigned)anchor_bits);

const arith_uint256 nextTarget = CalculateASERT_BCHN(
    target_ref,
    T,
    time_diff,
    height_diff,
    powLimit,
    half_life
);

return nextTarget.GetCompact();
}

// ============================================================================
// 난이도 전이 허용 검사 (정책)
// ============================================================================
bool PermittedDifficultyTransition(const Consensus::Params& params,
                                   int64_t height,
                                   uint32_t /*old_nbits*/,
                                   uint32_t /*new_nbits*/)
{
    const int fork_h = params.btcbt_fork_block_height;
    if (height <= fork_h) return true;
    // 포크 이후는 ASERT가 전권: 정책 체크 우회
    return true;
}

// ============================================================================
// PoW 검증
// ============================================================================
bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit)) {
        return false;
    }
    if (UintToArith256(hash) > bnTarget) {
        return false;
    }
    return true;
}