// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license.

#include <validation.h>
#include <node/chainstate.h>
#include <node/miner.h>
#include <versionbits.h>
#include <node/context.h>

#include <chain.h>
#include <chainparams.h>
#include <coins.h>
#include <common/args.h>
#include <consensus/amount.h>
#include <consensus/consensus.h>
#include <consensus/merkle.h>
#include <consensus/tx_verify.h>
#include <consensus/validation.h>
#include <deploymentstatus.h>
#include <logging.h>
#include <policy/feerate.h>
#include <policy/policy.h>
#include <pow.h>
#include <primitives/transaction.h>
#include <timedata.h>
#include <util/moneystr.h>

#include <algorithm>
#include <utility>

// 👉 아래에 함수 정의 삽입
// Adaptive Block Size: 동적 블록 크기 계산 함수
int GetAdaptiveMaxBlockWeight(size_t mempool_tx_count, int cur_height, const Consensus::Params& consensus)
{
    // ✅ regtest 우회: 항상 MAX_BLOCK_WEIGHT 반환 (4,000,000)
    const CChainParams& params = Params();
    if (params.GetChainType() == ChainType::REGTEST) {
        return MAX_BLOCK_WEIGHT;
    }

    // ── 합의 한도 내에서 동작 ──
    const int hard_cap = std::min<int>(static_cast<int>(MAX_BLOCK_WEIGHT),
                                       static_cast<int>(consensus.btcbt_max_block_size));

    const bool post_fork = (cur_height >= consensus.btcbt_fork_block_height);

    // 하한/상한을 합의 한도 내에서 결정
    const int min_weight = std::min<int>(hard_cap, post_fork ? 8'000'000 : 4'000'000);
    const int max_weight = std::min<int>(hard_cap, 32'000'000);

    // ✅ 가드: mempool=0 또는 매우 적을 때 분모 0/불안정 방지
    if (mempool_tx_count == 0)       return min_weight;
    if (mempool_tx_count <= 1'000)   return min_weight;
    if (mempool_tx_count >= 100'000) return max_weight;

    // ✅ 64bit 계산 후 최종 int로 clamp
    const int64_t span = (int64_t)max_weight - (int64_t)min_weight;
    const int64_t add  = ((int64_t)mempool_tx_count * span) / 100'000;
    const int64_t res  = (int64_t)min_weight + add;

    return (int)std::clamp<int64_t>(res, min_weight, max_weight);
}
using node::g_node;
// 찾기용 앵커(끝): GetAdaptiveMaxBlockWeight REPLACE END
namespace node {

int64_t UpdateTime(CBlockHeader* pblock, const Consensus::Params& consensusParams, const CBlockIndex* pindexPrev)
{
    int64_t nOldTime = pblock->nTime;
    int64_t nNewTime = std::max<int64_t>(pindexPrev->GetMedianTimePast() + 1, TicksSinceEpoch<std::chrono::seconds>(GetAdjustedTime()));
    if (nOldTime < nNewTime) pblock->nTime = nNewTime;
    if (consensusParams.fPowAllowMinDifficultyBlocks)
        pblock->nBits = GetNextWorkRequired(pindexPrev, pblock, consensusParams);
    return nNewTime - nOldTime;
}

void RegenerateCommitments(CBlock& block, ChainstateManager& chainman)
{
    CMutableTransaction tx{*block.vtx.at(0)};
    tx.vout.erase(tx.vout.begin() + GetWitnessCommitmentIndex(block));
    block.vtx.at(0) = MakeTransactionRef(tx);
    const CBlockIndex* prev_block = WITH_LOCK(::cs_main, return chainman.m_blockman.LookupBlockIndex(block.hashPrevBlock));
    chainman.GenerateCoinbaseCommitment(block, prev_block);
    block.hashMerkleRoot = BlockMerkleRoot(block);
}

// 찾기용 앵커(시작): BlockAssembler-ctor REPLACE START
static BlockAssembler::Options ClampOptions(BlockAssembler::Options options)
{
    // 후속 패치: 합의 한도 내에서 상한을 계산
    const Consensus::Params& consensus = Params().GetConsensus();
    const size_t hard_cap = static_cast<size_t>(std::min<int>(
        static_cast<int>(MAX_BLOCK_WEIGHT),
        static_cast<int>(consensus.btcbt_max_block_size)));
    options.nBlockMaxWeight = std::clamp<size_t>(options.nBlockMaxWeight, static_cast<size_t>(4000), hard_cap);
    return options;
}

BlockAssembler::BlockAssembler(Chainstate& chainstate, const CTxMemPool* mempool, const Options& options)
    : chainparams{chainstate.m_chainman.GetParams()},
      m_mempool{mempool},
      m_chainstate{chainstate},
      // ✅ m_options은 const이므로, 초기화 리스트에서 람다로 계산해 주입
      m_options{[&]() {
          Options o = ClampOptions(options);
          const CChainParams& p = chainstate.m_chainman.GetParams();
          if (p.GetChainType() == ChainType::REGTEST) {
              o.nBlockMaxWeight = MAX_BLOCK_WEIGHT; // 4,000,000
          }
          return o;
      }()}
{
    // (본문에서는 m_options에 대입하지 않음: const)
}
// 찾기용 앵커(끝): BlockAssembler-ctor REPLACE END

// 찾기용 앵커(시작): ApplyArgsManOptions LOG FIX REPLACE START
void ApplyArgsManOptions(const ArgsManager& args, BlockAssembler::Options& options)
{
    // REGTEST: -blockmaxweight 무시하고 고정 한도
    const CChainParams& params = Params();
    if (params.GetChainType() == ChainType::REGTEST) {
        options.nBlockMaxWeight = MAX_BLOCK_WEIGHT;
    } else {
        // 그 외 체인: 사용자 인자 허용하되 4000 ~ MAX_BLOCK_WEIGHT 범위로 clamp
        const int user_w = args.GetIntArg("-blockmaxweight", static_cast<int>(options.nBlockMaxWeight));
        options.nBlockMaxWeight = std::clamp<size_t>(
            static_cast<size_t>(user_w), 4000, static_cast<size_t>(MAX_BLOCK_WEIGHT));
    }
    // 👇 로그: v26 호환을 위해 LogPrintf 사용, size_t는 %zu
    LogPrintf("ApplyArgsManOptions: nBlockMaxWeight=%zu\n", options.nBlockMaxWeight);

    // (기존) -blockmintxfee 처리 유지
    if (const auto blockmintxfee{args.GetArg("-blockmintxfee")}) {
        if (const auto parsed{ParseMoney(*blockmintxfee)}) {
            options.blockMinFeeRate = CFeeRate{*parsed};
        }
    }
}
// 찾기용 앵커(끝): ApplyArgsManOptions LOG FIX REPLACE END


// 찾기용 앵커(시작): ConfiguredOptions LOG FIX REPLACE START
static BlockAssembler::Options ConfiguredOptions(const CTxMemPool* mempool, const Chainstate& chainstate)
{
    BlockAssembler::Options options;
    ApplyArgsManOptions(gArgs, options);

    const CChainParams& params = chainstate.m_chainman.GetParams();
    const auto& consensus = params.GetConsensus();
    const bool is_regtest = (params.GetChainType() == ChainType::REGTEST);

    const int mp = mempool ? static_cast<int>(mempool->size()) : 0;
    const CBlockIndex* tip = chainstate.m_chain.Tip();
    const int cur_height = tip ? (tip->nHeight + 1) : 0;

    // ✅ REGTEST: 항상 고정 한도 반환
    if (is_regtest) {
        options.nBlockMaxWeight = MAX_BLOCK_WEIGHT;
        // 👇 로그
        LogPrintf("ConfiguredOptions[regtest]: mp=%d, height=%d, nBlockMaxWeight=%zu\n",
                  mp, cur_height, static_cast<size_t>(options.nBlockMaxWeight));
        return ClampOptions(options);
    }

    // ✅ mempool=0: 초기 조립 경로 안정화 — 기본 한도 사용
    if (mp == 0) {
        options.nBlockMaxWeight = DEFAULT_BLOCK_MAX_WEIGHT;
        // 👇 로그
        LogPrintf("ConfiguredOptions[mp=0]: height=%d, nBlockMaxWeight=%zu\n",
                  cur_height, static_cast<size_t>(options.nBlockMaxWeight));
        return ClampOptions(options);
    }

    // ✅ Adaptive 계산 + 합의 한도 내 clamp
    const int hard_cap = std::min<int>(static_cast<int>(MAX_BLOCK_WEIGHT),
                                       static_cast<int>(consensus.btcbt_max_block_size));
    const int target = GetAdaptiveMaxBlockWeight(static_cast<size_t>(mp), cur_height, consensus);
    options.nBlockMaxWeight = std::clamp<int>(target, 4000, hard_cap);

    // 👇 로그
    LogPrintf("ConfiguredOptions: mp=%d, height=%d, nBlockMaxWeight=%zu (hard_cap=%d)\n",
              mp, cur_height, static_cast<size_t>(options.nBlockMaxWeight), hard_cap);

    return ClampOptions(options);
}
// 찾기용 앵커(끝): ConfiguredOptions LOG FIX REPLACE END


BlockAssembler::BlockAssembler(Chainstate& chainstate, const CTxMemPool* mempool)
    : BlockAssembler(chainstate, mempool, ConfiguredOptions(mempool, chainstate)) {}
// 찾기용 앵커(끝): ConfiguredOptions REPLACE END

void BlockAssembler::resetBlock()
{
    inBlock.clear();
    nBlockWeight = 4000;
    nBlockSigOpsCost = 400;
    nBlockTx = 0;
    nFees = 0;
}


// 찾기용 앵커(시작): CreateNewBlock GUARD INSERT START
std::unique_ptr<CBlockTemplate> BlockAssembler::CreateNewBlock(const CScript& scriptPubKeyIn)
{
    const auto time_start{SteadyClock::now()};
    resetBlock();

    pblocktemplate = std::make_unique<CBlockTemplate>();
    if (!pblocktemplate) return nullptr;

    CBlock* const pblock = &pblocktemplate->block;
    pblock->vtx.emplace_back();
 // 찾기용 앵커(시작): CreateNewBlock MEMPOOL0 GUARD REPLACE START
    pblocktemplate->vTxFees.push_back(-1);
    pblocktemplate->vTxSigOpsCost.push_back(-1);

    LOCK(::cs_main);
    CBlockIndex* pindexPrev = m_chainstate.m_chain.Tip();
    assert(pindexPrev != nullptr);
    nHeight = pindexPrev->nHeight + 1;

   // 찾기용 앵커(시작): CreateNewBlock MEMPOOL0 GUARD REPLACE START
    // ✅ mempool==0 → coinbase-only 템플릿을 즉시 반환(하위 경로 FPE 차단)
    if (m_mempool == nullptr || m_mempool->size() == 0) {
        LogPrintf("CreateNewBlock: mempool=0 at height=%d → returning coinbase-only block\n", nHeight);

        // 블록 헤더 기본 설정
        const auto& consensus_local = chainparams.GetConsensus();
        pblock->nVersion = m_chainstate.m_chainman.m_versionbitscache.ComputeBlockVersion(pindexPrev, consensus_local);
        pblock->nTime = static_cast<uint32_t>(pindexPrev->GetMedianTimePast() + 1);
        UpdateTime(pblock, consensus_local, pindexPrev);
        // 정규화된 시간 기준으로 난이도 계산
        pblock->nBits = GetNextWorkRequired(pindexPrev, pblock, consensus_local);
        pblock->nNonce   = 0;

        // coinbase 생성(수수료=0)
        CMutableTransaction coinbase_tx;
        coinbase_tx.vin.resize(1);
        coinbase_tx.vin[0].prevout.SetNull();
        coinbase_tx.vin[0].scriptSig = (CScript() << nHeight << OP_0); // BIP34 높이 + 일관성
        coinbase_tx.vin[0].scriptWitness.stack.resize(1);
        coinbase_tx.vin[0].scriptWitness.stack[0] = std::vector<unsigned char>(32, 0);
        coinbase_tx.vout.resize(1);
        coinbase_tx.vout[0].nValue = GetBlockSubsidy(nHeight, consensus_local);
        coinbase_tx.vout[0].scriptPubKey = scriptPubKeyIn;

        // coinbase 배치
        pblock->vtx[0] = MakeTransactionRef(std::move(coinbase_tx));

        // 커밋먼트/머클 루트
        pblocktemplate->vchCoinbaseCommitment =
            m_chainstate.m_chainman.GenerateCoinbaseCommitment(*pblock, pindexPrev);
        pblock->hashMerkleRoot = BlockMerkleRoot(*pblock);

        // ✅ 추가 마무리: 헤더/템플릿 값 정리
        pblock->hashPrevBlock = pindexPrev->GetBlockHash();                   // (1) prev hash
        pblocktemplate->vTxFees[0] = 0;                                       // (2) 수수료 슬롯
        pblocktemplate->vTxSigOpsCost[0] = WITNESS_SCALE_FACTOR *
                                           GetLegacySigOpCount(*pblock->vtx[0]); // (3) SigOps 기록

        // 최종 템플릿 반환
        return std::move(pblocktemplate);
    }
// 찾기용 앵커(끝): CreateNewBlock MEMPOOL0 GUARD REPLACE END



    // ✅ (선택) 마지막 가드: 옵션 덮어쓰기 전 안전화(값은 계산·로그만, const m_options는 수정하지 않음)
    // 찾기용 앵커(시작): CreateNewBlock LOG FIX REPLACE START
    {
        const CChainParams& params = chainparams;
        const auto& consensus = params.GetConsensus();
        const bool is_regtest = (params.GetChainType() == ChainType::REGTEST);
        const int mp = (m_mempool ? static_cast<int>(m_mempool->size()) : 0);

        // const m_options를 수정할 수 없으므로, '의도한 값'을 계산해서 현재 값과 비교만 수행
        size_t desired_weight = 0;
        if (is_regtest) {
            desired_weight = static_cast<size_t>(MAX_BLOCK_WEIGHT);
        } else if (mp == 0) {
            desired_weight = static_cast<size_t>(DEFAULT_BLOCK_MAX_WEIGHT);
        } else {
            const int hard_cap = std::min<int>(static_cast<int>(MAX_BLOCK_WEIGHT),
                                               static_cast<int>(consensus.btcbt_max_block_size));
            const int target = GetAdaptiveMaxBlockWeight(static_cast<size_t>(mp), nHeight, consensus);
            desired_weight = static_cast<size_t>(std::clamp<int>(target, 4000, hard_cap));
        }
        // 로그: 현재 값과 의도한 값 비교(필요 시 추적)
        LogPrintf("CreateNewBlock: mp=%d, height=%d, nBlockMaxWeight(cur)=%zu, desired=%zu\n",
                  mp, nHeight, static_cast<size_t>(m_options.nBlockMaxWeight), desired_weight);
    }
    // 찾기용 앵커(끝): CreateNewBlock LOG FIX REPLACE END

    const auto& consensus = chainparams.GetConsensus();
    pblock->nVersion = m_chainstate.m_chainman.m_versionbitscache.ComputeBlockVersion(pindexPrev, consensus);
// 찾기용 앵커(끝): CreateNewBlock MEMPOOL0 GUARD REPLACE END

    if (chainparams.MineBlocksOnDemand()) {
        pblock->nVersion = gArgs.GetIntArg("-blockversion", pblock->nVersion);
    }

    pblock->nTime = TicksSinceEpoch<std::chrono::seconds>(GetAdjustedTime());
    m_lock_time_cutoff = pindexPrev->GetMedianTimePast();

    int nPackagesSelected = 0;
    int nDescendantsUpdated = 0;
    if (m_mempool) {
        LOCK(m_mempool->cs);
        addPackageTxs(*m_mempool, nPackagesSelected, nDescendantsUpdated);
    }

    m_last_block_num_txs = nBlockTx;
    m_last_block_weight = nBlockWeight;

    // Coinbase + 보상 분기
    CMutableTransaction coinbaseTx;
    coinbaseTx.vin.resize(1);
    coinbaseTx.vin[0].prevout.SetNull();
    coinbaseTx.vout.resize(1);
    coinbaseTx.vout[0].scriptPubKey = scriptPubKeyIn;

// BTCBT: GetBlockSubsidy()는 포크 후 첫 블록에 2,000,000 BTCBT를 반환함
  CAmount coinbase_reward = GetBlockSubsidy(nHeight, consensus);


    coinbaseTx.vout[0].nValue = nFees + coinbase_reward;
    coinbaseTx.vin[0].scriptSig = CScript() << nHeight << OP_0;
    pblock->vtx[0] = MakeTransactionRef(std::move(coinbaseTx));
    pblocktemplate->vchCoinbaseCommitment = m_chainstate.m_chainman.GenerateCoinbaseCommitment(*pblock, pindexPrev);
    pblocktemplate->vTxFees[0] = -nFees;

    LogPrintf("CreateNewBlock(): block weight: %u txs: %u fees: %ld sigops %d\n",
              GetBlockWeight(*pblock), nBlockTx, nFees, nBlockSigOpsCost);

    pblock->hashPrevBlock = pindexPrev->GetBlockHash();
    UpdateTime(pblock, consensus, pindexPrev);
     pblock->nBits = GetNextWorkRequired(pindexPrev, pblock, consensus);
    if (pblock->nBits == 0) {
        const unsigned int clamp = UintToArith256(consensus.powLimit).GetCompact();
        pblock->nBits = clamp ? clamp : 0x1d00ffff;
        LogPrintf("POWDBG[CLAMP2]: template nBits==0 at height=%d -> set=%08x\n", nHeight, pblock->nBits);
    }
    pblock->nNonce = 0;
    pblocktemplate->vTxSigOpsCost[0] = WITNESS_SCALE_FACTOR * GetLegacySigOpCount(*pblock->vtx[0]);

    BlockValidationState state;
    if (m_options.test_block_validity && !TestBlockValidity(state, chainparams, m_chainstate, *pblock, pindexPrev,
                                                            GetAdjustedTime, false, false)) {
        throw std::runtime_error(strprintf("%s: TestBlockValidity failed: %s", __func__, state.ToString()));
    }

    const auto time_2{SteadyClock::now()};
    LogPrint(BCLog::BENCH, "CreateNewBlock() packages: %.2fms (%d packages, %d updated descendants), validity: %.2fms (total %.2fms)\n",
             Ticks<MillisecondsDouble>(time_2 - time_start), nPackagesSelected, nDescendantsUpdated,
             Ticks<MillisecondsDouble>(time_2 - time_2),
             Ticks<MillisecondsDouble>(time_2 - time_start));

    return std::move(pblocktemplate);
}

void BlockAssembler::onlyUnconfirmed(CTxMemPool::setEntries& testSet)
{
    for (auto it = testSet.begin(); it != testSet.end(); ) {
        if (inBlock.count(*it)) {
            testSet.erase(it++);
        } else {
            ++it;
        }
    }
}

bool BlockAssembler::TestPackage(uint64_t packageSize, int64_t packageSigOpsCost) const
{
    if (nBlockWeight + WITNESS_SCALE_FACTOR * packageSize >= m_options.nBlockMaxWeight) return false;

    const auto& consensus = m_chainstate.m_chainman.GetParams().GetConsensus();
    const int cur_height = nHeight;
    const int64_t fork_sigops = 200000;
    const int64_t sigops_limit = (cur_height >= consensus.btcbt_fork_block_height)
                                 ? fork_sigops
                                 : MAX_BLOCK_SIGOPS_COST;

    const int64_t cur_sigops = static_cast<int64_t>(nBlockSigOpsCost);
    if (cur_sigops + packageSigOpsCost >= sigops_limit) return false;
    return true;
}

bool BlockAssembler::TestPackageTransactions(const CTxMemPool::setEntries& package) const
{
    for (CTxMemPool::txiter it : package) {
        if (!IsFinalTx(it->GetTx(), nHeight, m_lock_time_cutoff)) return false;
    }
    return true;
}

void BlockAssembler::AddToBlock(CTxMemPool::txiter iter)
{
    pblocktemplate->block.vtx.emplace_back(iter->GetSharedTx());
    pblocktemplate->vTxFees.push_back(iter->GetFee());
    pblocktemplate->vTxSigOpsCost.push_back(iter->GetSigOpCost());
    nBlockWeight += iter->GetTxWeight();
    ++nBlockTx;
    nBlockSigOpsCost += iter->GetSigOpCost();
    nFees += iter->GetFee();
    inBlock.insert(iter);

    if (gArgs.GetBoolArg("-printpriority", DEFAULT_PRINTPRIORITY)) {
        LogPrintf("fee rate %s txid %s\n",
                  CFeeRate(iter->GetModifiedFee(), iter->GetTxSize()).ToString(),
                  iter->GetTx().GetHash().ToString());
    }
}
static int UpdatePackagesForAdded(const CTxMemPool& mempool,
                                  const CTxMemPool::setEntries& alreadyAdded,
                                  indexed_modified_transaction_set& mapModifiedTx)
    EXCLUSIVE_LOCKS_REQUIRED(mempool.cs)
{
    AssertLockHeld(mempool.cs);
    int nDescendantsUpdated = 0;
    for (CTxMemPool::txiter it : alreadyAdded) {
        CTxMemPool::setEntries descendants;
        mempool.CalculateDescendants(it, descendants);
        for (CTxMemPool::txiter desc : descendants) {
            if (alreadyAdded.count(desc)) continue;
            ++nDescendantsUpdated;
            modtxiter mit = mapModifiedTx.find(desc);
            if (mit == mapModifiedTx.end()) {
                CTxMemPoolModifiedEntry modEntry(desc);
                mit = mapModifiedTx.insert(modEntry).first;
            }
            mapModifiedTx.modify(mit, update_for_parent_inclusion(it));
        }
    }
    return nDescendantsUpdated;
}

void BlockAssembler::SortForBlock(const CTxMemPool::setEntries& package,
                                  std::vector<CTxMemPool::txiter>& sortedEntries)
{
    sortedEntries.clear();
    sortedEntries.insert(sortedEntries.begin(), package.begin(), package.end());
    std::sort(sortedEntries.begin(), sortedEntries.end(), CompareTxIterByAncestorCount());
}

void BlockAssembler::addPackageTxs(const CTxMemPool& mempool, int& nPackagesSelected, int& nDescendantsUpdated)
{
    AssertLockHeld(mempool.cs);

    indexed_modified_transaction_set mapModifiedTx;
    CTxMemPool::setEntries failedTx;

    auto mi = mempool.mapTx.get<ancestor_score>().begin();
    CTxMemPool::txiter iter;

    const int64_t MAX_CONSECUTIVE_FAILURES = 1000;
    int64_t nConsecutiveFailed = 0;

    while (mi != mempool.mapTx.get<ancestor_score>().end() || !mapModifiedTx.empty()) {
        if (mi != mempool.mapTx.get<ancestor_score>().end()) {
            auto it = mempool.mapTx.project<0>(mi);
            if (mapModifiedTx.count(it) || inBlock.count(it) || failedTx.count(it)) {
                ++mi;
                continue;
            }
        }

        bool fUsingModified = false;
        modtxscoreiter modit = mapModifiedTx.get<ancestor_score>().begin();
        if (mi == mempool.mapTx.get<ancestor_score>().end()) {
            iter = modit->iter;
            fUsingModified = true;
        } else {
            iter = mempool.mapTx.project<0>(mi);
            if (modit != mapModifiedTx.get<ancestor_score>().end() &&
                CompareTxMemPoolEntryByAncestorFee()(*modit, CTxMemPoolModifiedEntry(iter))) {
                iter = modit->iter;
                fUsingModified = true;
            } else {
                ++mi;
            }
        }

        assert(!inBlock.count(iter));
        uint64_t packageSize = iter->GetSizeWithAncestors();
        CAmount packageFees = iter->GetModFeesWithAncestors();
        int64_t packageSigOpsCost = iter->GetSigOpCostWithAncestors();

        if (fUsingModified) {
            packageSize = modit->nSizeWithAncestors;
            packageFees = modit->nModFeesWithAncestors;
            packageSigOpsCost = modit->nSigOpCostWithAncestors;
        }

        if (packageFees < m_options.blockMinFeeRate.GetFee(packageSize)) return;

        if (!TestPackage(packageSize, packageSigOpsCost)) {
            if (fUsingModified) {
                mapModifiedTx.get<ancestor_score>().erase(modit);
                failedTx.insert(iter);
            }
            ++nConsecutiveFailed;
            if (nConsecutiveFailed > MAX_CONSECUTIVE_FAILURES &&
                nBlockWeight > m_options.nBlockMaxWeight - 4000) {
                break;
            }
            continue;
        }

        auto ancestors = mempool.AssumeCalculateMemPoolAncestors(__func__, *iter,
                            CTxMemPool::Limits::NoLimits(), false);
        onlyUnconfirmed(ancestors);
        ancestors.insert(iter);

        if (!TestPackageTransactions(ancestors)) {
            if (fUsingModified) {
                mapModifiedTx.get<ancestor_score>().erase(modit);
                failedTx.insert(iter);
            }
            continue;
        }

        nConsecutiveFailed = 0;
        std::vector<CTxMemPool::txiter> sortedEntries;
        SortForBlock(ancestors, sortedEntries);

        for (CTxMemPool::txiter sortedIt : sortedEntries) {
            AddToBlock(sortedIt);
            mapModifiedTx.erase(sortedIt);
        }

        ++nPackagesSelected;
        nDescendantsUpdated += UpdatePackagesForAdded(mempool, ancestors, mapModifiedTx);
    }
}

} // namespace node
