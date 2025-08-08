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

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader* pblock, const Consensus::Params& params)
{
    const int next_height = pindexLast->nHeight + 1;

    // BTCBT 하드포크 이후 난이도 조정
    if (next_height >= params.btcbt_fork_block_height) {
        int btcbt_adjust_interval = (params.btcbt_block_interval > 0) ? 2016 : params.DifficultyAdjustmentInterval();

        if (next_height % btcbt_adjust_interval != 0) {
            return pindexLast->nBits;
        }

        const CBlockIndex* pindexFirst = pindexLast;
        for (int i = 0; i < btcbt_adjust_interval - 1; ++i) {
            pindexFirst = pindexFirst->pprev;
            assert(pindexFirst);
        }

        return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
    }

    // 기본 비트코인 난이도 조정
    if ((next_height % params.DifficultyAdjustmentInterval()) != 0) {
        return pindexLast->nBits;
    }

    const CBlockIndex* pindexFirst = pindexLast;
    for (int i = 0; i < params.DifficultyAdjustmentInterval() - 1; ++i) {
        pindexFirst = pindexFirst->pprev;
        assert(pindexFirst);
    }

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);

    // BTCBT ASERT 활성 조건
    if (pindexLast->nHeight + 1 >= params.btcbt_fork_block_height) {
        const int64_t target_block_time = params.btcbt_block_interval;
        const arith_uint256 powLimit = UintToArith256(params.powLimit);

        const CBlockIndex* pindex_ref = pindexLast;

        // ✅ 보완: 정확한 anchor 블록 도달까지 이동
        while (pindex_ref->pprev &&
               pindex_ref->nHeight > params.btcbt_asert_anchor_height &&
               pindex_ref->GetBlockHash() != params.btcbt_asert_anchor_hash) {
            pindex_ref = pindex_ref->pprev;
        }

        // ⛔ 실패 시 fallback (안전장치)
        // ⛔ 앵커를 못 찾으면: 직전 난이도 유지 + 경고 로그 (보안상 더 안전)
if (!pindex_ref || pindex_ref->GetBlockHash() != params.btcbt_asert_anchor_hash) {
    LogPrintf("[ASERT] anchor not found or mismatched (height=%d). Keep last nBits=%08x\n",
              pindexLast->nHeight, pindexLast->nBits);
    return pindexLast->nBits;
}

        int64_t time_diff = pindexLast->GetBlockTime() - pindex_ref->GetBlockTime();
        int64_t height_diff = pindexLast->nHeight - pindex_ref->nHeight;
        int64_t ideal_time_diff = height_diff * target_block_time;
        int64_t time_offset = time_diff - ideal_time_diff;

        arith_uint256 target;
        target.SetCompact(pindex_ref->nBits);

        int64_t exponent = (time_offset * 65536) / target_block_time;

        target *= arith_uint256(1) << (exponent / 65536);
        target *= 10000 + ((exponent % 65536) * 10000) / 65536;
        target /= 10000;

        if (target > powLimit) target = powLimit;

        return target.GetCompact();
    }

    // 기존 비트코인 방식
    int64_t nTargetTimespan = params.nPowTargetTimespan;
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;

    const int64_t nMinTimespan = nTargetTimespan / 4;
    const int64_t nMaxTimespan = nTargetTimespan * 4;
    nActualTimespan = std::clamp(nActualTimespan, nMinTimespan, nMaxTimespan);

    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

    if (bnNew > UintToArith256(params.powLimit)) {
        bnNew = UintToArith256(params.powLimit);
    }

    return bnNew.GetCompact();
}

bool PermittedDifficultyTransition(const Consensus::Params& params, int64_t height, uint32_t old_nbits, uint32_t new_nbits)
{
    if (params.fPowAllowMinDifficultyBlocks) return true;

    bool is_fork = (height >= params.btcbt_fork_block_height);
    int64_t interval = is_fork ? (params.btcbt_block_interval > 0 ? 2016 : params.DifficultyAdjustmentInterval())
                               : params.DifficultyAdjustmentInterval();

    if (height % interval == 0) {
        int64_t timespan = is_fork && params.btcbt_block_interval > 0 ? (params.btcbt_block_interval * 2016) : params.nPowTargetTimespan;

        const int64_t min_timespan = timespan / 4;
        const int64_t max_timespan = timespan * 4;

        const arith_uint256 pow_limit = UintToArith256(params.powLimit);
        arith_uint256 observed_new_target;
        observed_new_target.SetCompact(new_nbits);

        arith_uint256 largest_target;
        largest_target.SetCompact(old_nbits);
        largest_target *= max_timespan;
        largest_target /= timespan;
        if (largest_target > pow_limit) {
            largest_target = pow_limit;
        }

        if (observed_new_target > largest_target) return false;

        arith_uint256 smallest_target;
        smallest_target.SetCompact(old_nbits);
        smallest_target *= min_timespan;
        smallest_target /= timespan;
        if (smallest_target > pow_limit) {
            smallest_target = pow_limit;
        }

        if (observed_new_target < smallest_target) return false;
    } else if (old_nbits != new_nbits) {
        return false;
    }
    return true;
}

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
