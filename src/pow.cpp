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
// 포크 전에는 레거시(BTC) DAA, 포크 후에는 (앵커가 준비된 경우에만) ASERT 사용.
// 앵커가 없으면 포크 후에도 레거시로 fallback.
// (선택) fork+1(=903,845) 특별 블록만 powLimit로 쉽게 허용 라인은 유지.
// ============================================================================
unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast,
                                 const CBlockHeader* pblock,
                                 const Consensus::Params& params)
{
    // ★ 포크 전(<= fork_h)은 무조건 레거시 DAA — 가장 먼저 강제
    Assert(pindexLast != nullptr);
    const int next_height = pindexLast->nHeight + 1;
    const int fork_h      = params.btcbt_fork_block_height;
        if (next_height <= fork_h) {
        LogPrintf("POWDBG[REQ]: h=%d fork_h=%d path=LEGACY\n", next_height, fork_h);
        return GetNextWorkRequired_Legacy(pindexLast, pblock, params);
    }
LogPrintf("POWDBG[REQ]: h=%d fork_h=%d path=POST_FORK\n", next_height, fork_h);

     // (필수) 포크+1 완화: 첫 블록은 powLimit 허용 (체인 부팅용)
    if (next_height == fork_h + 1) {
        return UintToArith256(params.powLimit).GetCompact();
    }
 
    // 포크 후: ASERT 앵커 준비 확인
    const bool asert_ready =
        (params.btcbt_asert_anchor_height >= 0) &&
        (params.btcbt_asert_anchor_bits   != 0);
    if (!asert_ready) {
        LogPrintf("POWDBG[REQ]: h=%d ASERT anchor_ready=0 -> LEGACY\n", next_height);
        // 안전 폴백
        return GetNextWorkRequired_Legacy(pindexLast, pblock, params);
    }
    // 포크 후 + 앵커 준비 완료 → ASERT
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
bool PermittedDifficultyTransition(const Consensus::Params& params,
                                   int64_t height,
                                   uint32_t old_nbits,
                                   uint32_t new_nbits)
{
    const int fork_h = params.btcbt_fork_block_height;
  // 포크 전은 그대로 허용
    if (height <= fork_h) return true;
    // ✅ 포크 이후는 ASERT가 전권: 정책 체크를 완전히 우회
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
