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
int GetAdaptiveMaxBlockWeight(size_t mempool_tx_count)
{
    const int min_weight = 4000000;     // 4MB
    const int max_weight = 32000000;    // 32MB

    if (mempool_tx_count < 1000) return min_weight;
    if (mempool_tx_count > 100000) return max_weight;

    return min_weight + ((mempool_tx_count * (max_weight - min_weight)) / 100000);
}

using node::g_node;

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

static BlockAssembler::Options ClampOptions(BlockAssembler::Options options)
{
    options.nBlockMaxWeight = std::clamp<size_t>(options.nBlockMaxWeight, 4000, DEFAULT_BLOCK_MAX_WEIGHT);
    return options;
}

BlockAssembler::BlockAssembler(Chainstate& chainstate, const CTxMemPool* mempool, const Options& options)
    : chainparams{chainstate.m_chainman.GetParams()},
      m_mempool{mempool},
      m_chainstate{chainstate},
      m_options{ClampOptions(options)} {}

void ApplyArgsManOptions(const ArgsManager& args, BlockAssembler::Options& options)
{
    options.nBlockMaxWeight = args.GetIntArg("-blockmaxweight", options.nBlockMaxWeight);
    if (const auto blockmintxfee{args.GetArg("-blockmintxfee")}) {
        if (const auto parsed{ParseMoney(*blockmintxfee)})
            options.blockMinFeeRate = CFeeRate{*parsed};
    }
}

static BlockAssembler::Options ConfiguredOptions(const CTxMemPool* mempool)
{
    BlockAssembler::Options options;
    ApplyArgsManOptions(gArgs, options);

    int mempool_tx_count = 0;
    if (mempool) {
        mempool_tx_count = mempool->size();
    }

    options.nBlockMaxWeight = GetAdaptiveMaxBlockWeight(mempool_tx_count);
    return ClampOptions(options);
}

BlockAssembler::BlockAssembler(Chainstate& chainstate, const CTxMemPool* mempool)
    : BlockAssembler(chainstate, mempool, ConfiguredOptions(mempool)) {}

void BlockAssembler::resetBlock()
{
    inBlock.clear();
    nBlockWeight = 4000;
    nBlockSigOpsCost = 400;
    nBlockTx = 0;
    nFees = 0;
}


std::unique_ptr<CBlockTemplate> BlockAssembler::CreateNewBlock(const CScript& scriptPubKeyIn)
{
    const auto time_start{SteadyClock::now()};
    resetBlock();

    pblocktemplate = std::make_unique<CBlockTemplate>();
    if (!pblocktemplate) return nullptr;

    CBlock* const pblock = &pblocktemplate->block;
    pblock->vtx.emplace_back();
    pblocktemplate->vTxFees.push_back(-1);
    pblocktemplate->vTxSigOpsCost.push_back(-1);

    LOCK(::cs_main);
    CBlockIndex* pindexPrev = m_chainstate.m_chain.Tip();
    assert(pindexPrev != nullptr);
    nHeight = pindexPrev->nHeight + 1;

    const auto& consensus = chainparams.GetConsensus();
    pblock->nVersion = m_chainstate.m_chainman.m_versionbitscache.ComputeBlockVersion(pindexPrev, consensus);
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
    if (nBlockSigOpsCost + packageSigOpsCost >= MAX_BLOCK_SIGOPS_COST) return false;
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
