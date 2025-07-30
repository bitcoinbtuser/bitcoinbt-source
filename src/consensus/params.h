// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license.

#ifndef BITCOIN_CONSENSUS_PARAMS_H
#define BITCOIN_CONSENSUS_PARAMS_H

#include <uint256.h>

#include <chrono>
#include <limits>
#include <map>
#include <vector>

namespace Consensus {

/**
 * A buried deployment is one where the height of the activation has been hardcoded into
 * the client implementation long after the consensus change has activated. See BIP 90.
 */
enum BuriedDeployment : int16_t {
    DEPLOYMENT_HEIGHTINCB = std::numeric_limits<int16_t>::min(),
    DEPLOYMENT_CLTV,
    DEPLOYMENT_DERSIG,
    DEPLOYMENT_CSV,
    DEPLOYMENT_SEGWIT,
};
constexpr bool ValidDeployment(BuriedDeployment dep) { return dep <= DEPLOYMENT_SEGWIT; }

/**
 * VersionBits deployments (BIP9-style).
 */
enum DeploymentPos : uint16_t {
    DEPLOYMENT_TESTDUMMY,
    DEPLOYMENT_TAPROOT,
    // NOTE: Add new deployments here and update deploymentinfo.cpp
    MAX_VERSION_BITS_DEPLOYMENTS
};
constexpr bool ValidDeployment(DeploymentPos dep) { return dep < MAX_VERSION_BITS_DEPLOYMENTS; }

/**
 * Struct for each individual consensus rule change using BIP9.
 */
struct BIP9Deployment {
    int bit{28};
    int64_t nStartTime{NEVER_ACTIVE};
    int64_t nTimeout{NEVER_ACTIVE};
    int min_activation_height{0};

    static constexpr int64_t NO_TIMEOUT = std::numeric_limits<int64_t>::max();
    static constexpr int64_t ALWAYS_ACTIVE = -1;
    static constexpr int64_t NEVER_ACTIVE = -2;
};

/**
 * Parameters that influence chain consensus.
 */
struct Params {
    uint256 hashGenesisBlock;
    int nSubsidyHalvingInterval;
    std::map<uint256, uint32_t> script_flag_exceptions;

    int BIP34Height;
    uint256 BIP34Hash;
    int BIP65Height;
    int BIP66Height;
    int CSVHeight;
    int SegwitHeight;
    int MinBIP9WarningHeight;

    uint32_t nRuleChangeActivationThreshold;
    uint32_t nMinerConfirmationWindow;
    BIP9Deployment vDeployments[MAX_VERSION_BITS_DEPLOYMENTS];

    // Proof of work parameters
    uint256 powLimit;
    bool fPowAllowMinDifficultyBlocks;
    bool fPowNoRetargeting;
    int64_t nPowTargetSpacing;
    int64_t nPowTargetTimespan;
    std::chrono::seconds PowTargetSpacing() const { return std::chrono::seconds{nPowTargetSpacing}; }
    int64_t DifficultyAdjustmentInterval() const { return nPowTargetTimespan / nPowTargetSpacing; }

    uint256 nMinimumChainWork;
    uint256 defaultAssumeValid;

    // BTCBT 하드포크 전용 파라미터
    int btcbt_fork_block_height{0};
    uint256 btcbt_fork_block_hash;
       int btcbt_block_interval{0};
    int btcbt_halving_interval{0};
    int btcbt_max_block_size{0};
    int btcbt_max_block_sigops_cost{0};

    // ASERT 난이도 조정 기준 블록 (anchor)
    int btcbt_asert_anchor_height{0};
    uint256 btcbt_asert_anchor_hash;

    // Signet (if used)
    bool signet_blocks{false};
    std::vector<uint8_t> signet_challenge;


    /**
     * Returns activation height for buried deployments.
     */
    int DeploymentHeight(BuriedDeployment dep) const {
        switch (dep) {
            case DEPLOYMENT_HEIGHTINCB: return BIP34Height;
            case DEPLOYMENT_CLTV: return BIP65Height;
            case DEPLOYMENT_DERSIG: return BIP66Height;
            case DEPLOYMENT_CSV: return CSVHeight;
            case DEPLOYMENT_SEGWIT: return SegwitHeight;
        }
        return std::numeric_limits<int>::max();
    }
};

} // namespace Consensus

#endif // BITCOIN_CONSENSUS_PARAMS_H
