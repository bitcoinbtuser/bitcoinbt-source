// Copyright (c) 2016-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_VERSIONBITS_H
#define BITCOIN_VERSIONBITS_H

#include <chain.h>
#include <consensus/params.h>
#include <sync.h>
#include <node/blockstorage.h>
#include <map>
#include <vector>
#include <limits>

class ChainstateManager;

/** What block version to use for new blocks (pre-versionbits) */
static const int32_t VERSIONBITS_LAST_OLD_BLOCK_VERSION = 4;
/** What bits to set in version for versionbits blocks */
static const int32_t VERSIONBITS_TOP_BITS = 0x20000000UL;
/** What bitmask determines whether versionbits is in use */
static const int32_t VERSIONBITS_TOP_MASK = 0xE0000000UL;
/** Total bits available for versionbits */
static const int32_t VERSIONBITS_NUM_BITS = 29;

/**
 * BIP 9 defines a finite-state-machine to deploy a softfork in multiple stages.
 * State transitions happen during retarget period if conditions are met.
 * In case of reorg, transitions can go backward. Without transition, state is
 * inherited between periods. All blocks of a period share the same state.
 */
enum class ThresholdState {
    DEFINED,    //!< First state that each softfork starts out as. The genesis block is by definition in this state for each deployment.
    STARTED,    //!< For blocks past the starttime.
    LOCKED_IN,  //!< For at least one retarget period after the first retarget period with STARTED blocks of which at least threshold have the associated bit set in nVersion, until min_activation_height is reached.
    ACTIVE,     //!< For all blocks after the LOCKED_IN retarget period (final state)
    FAILED      //!< For all blocks once the first retarget period after the timeout time is hit, if LOCKED_IN wasn't already reached (final state)
};

/** A map that gives the state for blocks whose height is a multiple of Period().
 * The map is indexed by the block's parent, so all keys will either be nullptr
 * or a block with (height + 1) % Period() == 0.
 */
typedef std::map<const CBlockIndex*, ThresholdState> ThresholdConditionCache;

/** Display status of an in-progress BIP9 softfork */
struct BIP9Stats {
    int period = 0;       //!< Length of blocks in the BIP9 signalling period
    int threshold = 0;    //!< Required number of signalling blocks to activate
    int elapsed = 0;      //!< Number of blocks elapsed since period start
    int count = 0;        //!< Signalling blocks count
    bool possible = false; //!< Whether activation is still possible
};

/**
 * Abstract class that implements BIP9-style threshold logic.
 */
class AbstractThresholdConditionChecker {
protected:
    virtual bool Condition(const CBlockIndex* pindex, const Consensus::Params& params) const = 0;
    virtual int64_t BeginTime(const Consensus::Params& params) const = 0;
    virtual int64_t EndTime(const Consensus::Params& params) const = 0;
    virtual int MinActivationHeight(const Consensus::Params& params) const { return 0; }
    virtual int Period(const Consensus::Params& params) const = 0;
    virtual int Threshold(const Consensus::Params& params) const = 0;

public:
    ThresholdState GetStateFor(const CBlockIndex* pindexPrev, const Consensus::Params& params, ThresholdConditionCache& cache) const;
    BIP9Stats GetStateStatisticsFor(const CBlockIndex* pindex, const Consensus::Params& params, std::vector<bool>* signalling_blocks = nullptr) const;
    int GetStateSinceHeightFor(const CBlockIndex* pindexPrev, const Consensus::Params& params, ThresholdConditionCache& cache) const;
};

/**
 * BIP9 allows multiple softforks to be deployed in parallel. We cache
 * per-period state for each of them.
 */
class VersionBitsCache {
private:
    Mutex m_mutex;
    ThresholdConditionCache m_caches[Consensus::MAX_VERSION_BITS_DEPLOYMENTS] GUARDED_BY(m_mutex);

public:
    ThresholdState State(const CBlockIndex* pindexPrev, const Consensus::Params& params, Consensus::DeploymentPos pos) EXCLUSIVE_LOCKS_REQUIRED(!m_mutex);
    int StateSinceHeight(const CBlockIndex* pindexPrev, const Consensus::Params& params, Consensus::DeploymentPos pos) EXCLUSIVE_LOCKS_REQUIRED(!m_mutex);
    int32_t ComputeBlockVersion(const CBlockIndex* pindexPrev, const Consensus::Params& params) EXCLUSIVE_LOCKS_REQUIRED(!m_mutex);
    static BIP9Stats Statistics(const CBlockIndex* pindex, const Consensus::Params& params, Consensus::DeploymentPos pos, std::vector<bool>* signalling_blocks = nullptr);
    static uint32_t Mask(const Consensus::Params& params, Consensus::DeploymentPos pos);
    void Clear() EXCLUSIVE_LOCKS_REQUIRED(!m_mutex);
};

/**
 * BTCBT-specific WarningBitsConditionChecker: used to detect warning conditions
 * from unknown version bits.
 */
class WarningBitsConditionChecker : public AbstractThresholdConditionChecker {
private:
    const ChainstateManager& m_chainman;
    int bit;

protected:
    bool Condition(const CBlockIndex* pindex, const Consensus::Params& params) const override {
        return ((pindex->nVersion & VERSIONBITS_TOP_MASK) == VERSIONBITS_TOP_BITS) &&
               ((pindex->nVersion >> bit) & 1) != 0;
    }

    int64_t BeginTime(const Consensus::Params& params) const override { return 0; }
    int64_t EndTime(const Consensus::Params& params) const override { return std::numeric_limits<int64_t>::max(); }
    int Period(const Consensus::Params& params) const override { return params.nMinerConfirmationWindow; }
    int Threshold(const Consensus::Params& params) const override { return params.nRuleChangeActivationThreshold; }

public:
    WarningBitsConditionChecker(const ChainstateManager& chainman_, int bitIn)
        : m_chainman(chainman_), bit(bitIn) {}
};

#endif // BITCOIN_VERSIONBITS_H
