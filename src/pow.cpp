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

#include <algorithm> // for min/max if needed

// === 내부 함수 선언 ===
static unsigned int GetNextWorkRequired_Legacy(const CBlockIndex* pindexLast, const CBlockHeader* pblock, const Consensus::Params& params);
static unsigned int GetNextWorkRequired_ASERT (const CBlockIndex* pindexLast, const CBlockHeader* pblock, const Consensus::Params& params);

// ============================================================================
// 포크 전에는 레거시, 포크 후에는 (앵커가 준비된 경우에만) ASERT 사용
// 앵커가 없으면 포크 후에도 레거시로 fallback
// ============================================================================
unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader* pblock, const Consensus::Params& params)
{
    Assert(pindexLast != nullptr);
    const int next_height = pindexLast->nHeight + 1;

    // 새 필드 우선, 미설정이면 구 필드 사용
   const int fork_h = params.btcbt_fork_block_height;

// 포크가 "설정되지 않았으면" 항상 레거시
const bool fork_configured = (fork_h > 0);
if (!fork_configured) {
    return GetNextWorkRequired_Legacy(pindexLast, pblock, params);
}

const bool fork_active = (next_height > fork_h);
// 앵커도 포크가 설정된 네트워크에서만 의미가 있음
const bool asert_ready = fork_configured &&
                         (params.btcbt_asert_anchor_height >= 0) &&
                         (params.btcbt_asert_anchor_bits != 0);
// 1) 포크 전 → 레거시
if (!fork_active) {
    return GetNextWorkRequired_Legacy(pindexLast, pblock, params);
}
// 2) 포크 후이나 앵커 미설정 → 레거시 fallback
if (!asert_ready) {
    return GetNextWorkRequired_Legacy(pindexLast, pblock, params);
}

    // 3) 포크 후 + 앵커 설정 → ASERT
    return GetNextWorkRequired_ASERT(pindexLast, pblock, params);
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

    // 실제 경과 시간
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;

    // 클램프: /4 ~ *4
    int64_t nTargetTimespan = params.nPowTargetTimespan;
if (nTargetTimespan <= 0) {
    // 비정상 파라미터 방어: 바로 이전 난이도 유지
    return pindexLast->nBits;
}

    const int64_t nMinTimespan = nTargetTimespan / 4;
    const int64_t nMaxTimespan = nTargetTimespan * 4;
    if (nActualTimespan < nMinTimespan) nActualTimespan = nMinTimespan;
    if (nActualTimespan > nMaxTimespan) nActualTimespan = nMaxTimespan;

    // 새 타깃 계산
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

    const arith_uint256 powLimit = UintToArith256(params.powLimit);
    if (bnNew > powLimit) bnNew = powLimit;

    return bnNew.GetCompact();
}

// ============================================================================
// 레거시 경로: 포크 전 또는 포크 후라 해도 앵커 미설정 시 사용
//   - DifficultyAdjustmentInterval 경계에서만 retarget
//   - 테스트넷 특례(허용 시) 최소 난이도 처리
// ============================================================================
static unsigned int GetNextWorkRequired_Legacy(const CBlockIndex* pindexLast, const CBlockHeader* pblock, const Consensus::Params& params)
{
    Assert(pindexLast != nullptr);
    const int next_height = pindexLast->nHeight + 1;

    if (params.fPowNoRetargeting) {
        return pindexLast->nBits;
    }

    // 윈도우 중간: retarget 없음
    if ((next_height % params.DifficultyAdjustmentInterval()) != 0) {
        // testnet 특례
        if (params.fPowAllowMinDifficultyBlocks) {
            // If the new block's timestamp is more than 2 * target spacing
            // then allow mining of a min-difficulty block.
            if (pblock && pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing * 2) {
                return UintToArith256(params.powLimit).GetCompact();
            }
            // Otherwise, return the last non-special-min-difficulty block
            const CBlockIndex* pindex = pindexLast;
            while (pindex->pprev && (pindex->nHeight % params.DifficultyAdjustmentInterval()) != 0 && pindex->nBits == UintToArith256(params.powLimit).GetCompact()) {
                pindex = pindex->pprev;
            }
            return pindex->nBits;
        }
        return pindexLast->nBits;
    }

    // 윈도우 시작 블록 찾기 (interval 길이만큼 과거로)
    const CBlockIndex* pindexFirst = pindexLast;
    for (int i = 0; i < params.DifficultyAdjustmentInterval() - 1; ++i) {
        pindexFirst = pindexFirst->pprev;
        Assert(pindexFirst != nullptr);
    }

    // 레거시 타깃 계산
    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

// ============================================================================
// ASERT 경로: 포크 후 + 앵커 높이/비트가 준비된 경우
//   - 앵커는 "높이(anchor_h)로 탐색", 타깃 기준은 "앵커 bits" 사용
//   - 레포에 정식 ASERT 구현이 있다면 이 자리에서 그 함수를 호출하도록 교체하세요.
// ============================================================================
static unsigned int GetNextWorkRequired_ASERT(const CBlockIndex* pindexLast, const CBlockHeader* pblock, const Consensus::Params& params)
{
    Assert(pindexLast != nullptr);

    const int anchor_h = params.btcbt_asert_anchor_height;     // -1이면 래퍼에서 이미 차단됨
const unsigned int anchor_bits = params.btcbt_asert_anchor_bits; // 0이면 래퍼에서 차단

    // 높이로 앵커 블록 찾기
    const CBlockIndex* anchor = pindexLast;
    while (anchor && anchor->nHeight > anchor_h) {
        anchor = anchor->pprev;
    }
    if (!anchor || anchor->nHeight != anchor_h) {
        // 안전장치: 앵커를 못 찾으면 레거시로
        return GetNextWorkRequired_Legacy(pindexLast, pblock, params);
    }

    // 기준 블록 간격 (BTCBT가 따로 지정했다면 그것을 사용)
    // 기준 블록 간격 (하드 가드)
int64_t T = (params.btcbt_block_interval > 0) ? params.btcbt_block_interval
                                              : params.nPowTargetSpacing;
if (T <= 0) {
    // 안전장치: 분모 0 회피 + 레거시 경로로 폴백
    return GetNextWorkRequired_Legacy(pindexLast, pblock, params);
}


    const arith_uint256 powLimit = UintToArith256(params.powLimit);

    // 기준 타깃: "앵커 bits" (요청사항)
    arith_uint256 target_ref; target_ref.SetCompact(anchor_bits);

    // 시간/높이 오프셋
    const int64_t time_diff   = pindexLast->GetBlockTime() - anchor->GetBlockTime();
    const int64_t height_diff = pindexLast->nHeight      - anchor->nHeight;
    const int64_t ideal_time  = height_diff * T;
    const int64_t offset      = time_diff - ideal_time;

    // 간략 버전의 지수 근사 (정식 ASERT로 교체 권장)
    arith_uint256 target = target_ref;

    // exponent 고정소수점(16.16) 근사
const int64_t exponent = ((offset * 65536) / T);

    // 2^floor(exponent/65536)
    if (exponent >= 0) {
        target <<= (exponent / 65536);
    } else {
        target >>= ((-exponent) / 65536);
    }

    // 잔여분(선형 근사): target *= (1 + frac)
    const int64_t frac = exponent % 65536;
    if (frac != 0) {
        // 정수 산술 근사: target *= (10000 + frac*10000/65536) / 10000
        target *= 10000 + ((frac * 10000) / 65536);
        target /= 10000;
    }

    if (target > powLimit) target = powLimit;
    return target.GetCompact();
}

// ============================================================================
// 난이도 전이 허용 검사 (정책)
//   - 새 forkHeight 우선 사용
// ============================================================================
bool PermittedDifficultyTransition(const Consensus::Params& params, int64_t height, uint32_t old_nbits, uint32_t new_nbits)
{
    if (params.fPowAllowMinDifficultyBlocks) return true;

const int fork_h = params.btcbt_fork_block_height;const bool fork_configured = (fork_h > 0);
const bool is_fork = fork_configured && (height > fork_h);

const int64_t interval = is_fork ? (params.btcbt_block_interval > 0 ? 2016 : params.DifficultyAdjustmentInterval())
                                 : params.DifficultyAdjustmentInterval();
    if (height % interval == 0) {
        int64_t timespan = is_fork && params.btcbt_block_interval > 0
                 ? (params.btcbt_block_interval * 2016)
                 : params.nPowTargetTimespan;
if (timespan <= 0) {
    // 분모 0 방지: 관대하게 "조정 불가"로 처리(전이 불허)
    return false;
}

        const int64_t min_timespan = timespan / 4;
        const int64_t max_timespan = timespan * 4;

        const arith_uint256 pow_limit = UintToArith256(params.powLimit);
        arith_uint256 observed_new_target; observed_new_target.SetCompact(new_nbits);

        arith_uint256 largest_target; largest_target.SetCompact(old_nbits);
        largest_target *= max_timespan;
        largest_target /= timespan;
        if (largest_target > pow_limit) largest_target = pow_limit;
        if (observed_new_target > largest_target) return false;

        arith_uint256 smallest_target; smallest_target.SetCompact(old_nbits);
        smallest_target *= min_timespan;
        smallest_target /= timespan;
        if (smallest_target > pow_limit) smallest_target = pow_limit;
        if (observed_new_target < smallest_target) return false;
    } else if (old_nbits != new_nbits) {
        return false;
    }
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
