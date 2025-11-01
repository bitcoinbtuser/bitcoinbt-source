// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_KERNEL_CHAINPARAMS_H
#define BITCOIN_KERNEL_CHAINPARAMS_H

#include <consensus/params.h>
#include <kernel/messagestartchars.h>
#include <primitives/block.h>
#include <uint256.h>
#include <util/chaintype.h>
#include <util/hash_type.h>
#include <util/vector.h>

#include <cstdint>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

typedef std::map<int, uint256> MapCheckpoints;

// --- v27 호환: 체크포인트가 비어있을 수 있으므로 가드 추가 ---
struct CCheckpointData {
    MapCheckpoints mapCheckpoints;

    int GetHeight() const {
        if (mapCheckpoints.empty()) return 0;
        const auto it = mapCheckpoints.crbegin();
        return it->first; // height
    }
};

// --- v27 스타일: BaseHash<uint256> 래퍼 + 기본 생성자 제공 ---
struct AssumeutxoHash : public BaseHash<uint256> {
    AssumeutxoHash() : BaseHash(uint256()) {}                 // 기본 생성자 (0 해시)
    explicit AssumeutxoHash(const uint256& hash) : BaseHash(hash) {}
};

// --- v27 스타일: assumeutxo 메타데이터 컨테이너 ---
struct AssumeutxoData {
    int height{0};
    AssumeutxoHash hash_serialized{}; // 기본 0으로 초기화
    uint64_t nChainTx{0};             // 스냅샷 기준 누적 tx 수 (LoadBlockIndex 보조)
    uint256 blockhash{};
};

// 체인 트랜잭션 통계(IBD 진행률 추정용)
struct ChainTxData {
    int64_t nTime;    //!< 마지막 tx 수 기준의 UNIX 타임스탬프
    int64_t nTxCount; //!< 해당 시점까지 총 tx 수
    double  dTxRate;  //!< 그 이후 추정 TPS
};

// CChainParams: 네트워크별 조정 가능한 모든 파라미터
class CChainParams
{
public:
    enum Base58Type {
        PUBKEY_ADDRESS,
        SCRIPT_ADDRESS,
        SECRET_KEY,
        EXT_PUBLIC_KEY,
        EXT_SECRET_KEY,

        MAX_BASE58_TYPES
    };

    const Consensus::Params& GetConsensus() const { return consensus; }
    const MessageStartChars& MessageStart() const { return pchMessageStart; }
    const MessageStartChars& DiskMagic()  const { return pchDiskMagic; } // BTCBT 전용(blk.dat 매직)
    uint16_t GetDefaultPort() const { return nDefaultPort; }

    const CBlock& GenesisBlock() const { return genesis; }
    bool DefaultConsistencyChecks() const { return fDefaultConsistencyChecks; }
    bool IsTestChain() const { return m_chain_type != ChainType::MAIN; }
    bool IsMockableChain() const { return m_is_mockable_chain; }
    uint64_t PruneAfterHeight() const { return nPruneAfterHeight; }
    uint64_t AssumedBlockchainSize() const { return m_assumed_blockchain_size; }
    uint64_t AssumedChainStateSize() const { return m_assumed_chain_state_size; }
    bool MineBlocksOnDemand() const { return consensus.fPowNoRetargeting; }
    std::string GetChainTypeString() const { return ChainTypeToString(m_chain_type); }
    ChainType GetChainType() const { return m_chain_type; }
    const std::vector<std::string>& DNSSeeds() const { return vSeeds; }
    const std::vector<unsigned char>& Base58Prefix(Base58Type type) const { return base58Prefixes[type]; }
    const std::string& Bech32HRP() const { return bech32_hrp; }
    const std::vector<uint8_t>& FixedSeeds() const { return vFixedSeeds; }
    const CCheckpointData& Checkpoints() const { return checkpointData; }

    // assumeutxo 조회
    std::optional<AssumeutxoData> AssumeutxoForHeight(int height) const
    {
        return FindFirst(m_assumeutxo_data, [&](const auto& d) { return d.height == height; });
    }
    std::optional<AssumeutxoData> AssumeutxoForBlockhash(const uint256& blockhash) const
    {
        return FindFirst(m_assumeutxo_data, [&](const auto& d) { return d.blockhash == blockhash; });
    }

    const ChainTxData& TxData() const { return chainTxData; }

    // SigNet 옵션 (v27)
    struct SigNetOptions {
        std::optional<std::vector<uint8_t>> challenge{};
        std::optional<std::vector<std::string>> seeds{};
    };

    // 버전비트 파라미터 (v27)
    struct VersionBitsParameters {
        int64_t start_time;
        int64_t timeout;
        int min_activation_height;
    };

    // Regtest 옵션 (v27)
    struct RegTestOptions {
        std::unordered_map<Consensus::DeploymentPos, VersionBitsParameters> version_bits_parameters{};
        std::unordered_map<Consensus::BuriedDeployment, int> activation_heights{};
        bool fastprune{false};
    };

    static std::unique_ptr<const CChainParams> RegTest(const RegTestOptions& options);
    static std::unique_ptr<const CChainParams> SigNet(const SigNetOptions& options);
    static std::unique_ptr<const CChainParams> Main();
    static std::unique_ptr<const CChainParams> TestNet();
    static std::unique_ptr<const CChainParams> BTCBT(); // BTCBT 전용 팩토리

protected:
    CChainParams() {}

    Consensus::Params consensus;
    MessageStartChars pchMessageStart;
    MessageStartChars pchDiskMagic; // 디스크 블록 파일 매직 (P2P와 분리 가능)

    uint16_t nDefaultPort{0};
    uint64_t nPruneAfterHeight{0};
    uint64_t m_assumed_blockchain_size{0};
    uint64_t m_assumed_chain_state_size{0};
    std::vector<std::string> vSeeds;
    std::vector<unsigned char> base58Prefixes[MAX_BASE58_TYPES];
    std::string bech32_hrp;
    ChainType m_chain_type{ChainType::MAIN};
    CBlock genesis;
    std::vector<uint8_t> vFixedSeeds;
    bool fDefaultConsistencyChecks{false};
    bool m_is_mockable_chain{false};
    CCheckpointData checkpointData;
    std::vector<AssumeutxoData> m_assumeutxo_data;
    ChainTxData chainTxData{0,0,0.0};
};

#endif // BITCOIN_KERNEL_CHAINPARAMS_H
