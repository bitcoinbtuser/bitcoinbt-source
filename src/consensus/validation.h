// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CONSENSUS_VALIDATION_H
#define BITCOIN_CONSENSUS_VALIDATION_H

#include <string>
#include <version.h>
#include <consensus/consensus.h>
#include <primitives/transaction.h>
#include <primitives/block.h>

/** Index marker for when no witness commitment is present in a coinbase transaction. */
static constexpr int NO_WITNESS_COMMITMENT{-1};

/** Minimum size of a witness commitment structure. Defined in BIP 141. **/
static constexpr size_t MINIMUM_WITNESS_COMMITMENT{38};

/** A "reason" why a transaction was invalid, suitable for determining whether the
  * provider of the transaction should be banned/ignored/disconnected/etc.
  */
enum class TxValidationResult {
    TX_RESULT_UNSET = 0,     //!< initial value. Tx has not yet been rejected
    TX_CONSENSUS,            //!< invalid by consensus rules
    /**
     * Invalid by a change to consensus rules more recent than SegWit.
     * Currently unused as there are no such consensus rule changes, and any download
     * sources realistically need to support SegWit in order to provide useful data,
     * so differentiating between always-invalid and invalid-by-pre-SegWit-soft-fork
     * is uninteresting.
     */
    TX_RECENT_CONSENSUS_CHANGE,
    TX_INPUTS_NOT_STANDARD,   //!< inputs (covered by txid) failed policy rules
    TX_NOT_STANDARD,          //!< otherwise didn't meet our local policy rules
    TX_MISSING_INPUTS,        //!< transaction was missing some of its inputs
    TX_PREMATURE_SPEND,       //!< transaction spends a coinbase too early, or violates locktime/sequence locks
    /**
     * Transaction might have a witness prior to SegWit
     * activation, or witness may have been malleated (which includes
     * non-standard witnesses).
     */
    TX_WITNESS_MUTATED,
    /**
     * Transaction is missing a witness.
     */
    TX_WITNESS_STRIPPED,
    /**
     * Tx already in mempool or conflicts with a tx in the chain
     * (if it conflicts with another tx in mempool, we use MEMPOOL_POLICY as it failed to reach the RBF threshold)
     * Currently this is only used if the transaction already exists in the mempool or on chain.
     */
    TX_CONFLICT,
    TX_MEMPOOL_POLICY,        //!< violated mempool's fee/size/descendant/RBF/etc limits
    TX_NO_MEMPOOL,            //!< this node does not have a mempool so can't validate the transaction
};

/** A "reason" why a block was invalid, suitable for determining whether the
  * provider of the block should be banned/ignored/disconnected/etc.
  * These are much more granular than the rejection codes, which may be more
  * useful for some other use-cases.
  */
enum class BlockValidationResult {
    BLOCK_VALID,
    BLOCK_HEADER,
    BLOCK_MUTATED,
    BLOCK_MISSING_PREV,
    BLOCK_INVALID_HEADER,
    BLOCK_INVALID_PREV,
    BLOCK_CONSENSUS,
    BLOCK_NONFINAL,
    BLOCK_TIME_FUTURE,
    BLOCK_CHECKPOINT,
    BLOCK_TRANSACTIONS,
    BLOCK_VALIDATION_FAILED,

    // BTCBT 확장
    BLOCK_SERIALIZATION,
    BLOCK_WEIGHT,

    // net_processing.cpp에서 사용되는 항목들
    BLOCK_RESULT_UNSET,
    BLOCK_HEADER_LOW_WORK,
    BLOCK_CACHED_INVALID,
    BLOCK_RECENT_CONSENSUS_CHANGE
};


/** Template for capturing information about block/transaction validation. This is instantiated
 *  by TxValidationState and BlockValidationState for validation information on transactions
 *  and blocks respectively. */
template <typename Result>
class ValidationState
{
private:
    enum class ModeState {
        M_VALID,   //!< everything ok
        M_INVALID, //!< network rule violation (DoS value may be set)
        M_ERROR,   //!< run-time error
    } m_mode{ModeState::M_VALID};
    Result m_result{};
    std::string m_reject_reason;
    std::string m_debug_message;

public:
    bool Invalid(Result result,
                 const std::string& reject_reason = "",
                 const std::string& debug_message = "")
    {
        m_result = result;
        m_reject_reason = reject_reason;
        m_debug_message = debug_message;
        if (m_mode != ModeState::M_ERROR) m_mode = ModeState::M_INVALID;
        return false;
    }
    bool Error(const std::string& reject_reason)
    {
        if (m_mode == ModeState::M_VALID)
            m_reject_reason = reject_reason;
        m_mode = ModeState::M_ERROR;
        return false;
    }
    bool IsValid() const { return m_mode == ModeState::M_VALID; }
    bool IsInvalid() const { return m_mode == ModeState::M_INVALID; }
    bool IsError() const { return m_mode == ModeState::M_ERROR; }
    Result GetResult() const { return m_result; }
    std::string GetRejectReason() const { return m_reject_reason; }
    std::string GetDebugMessage() const { return m_debug_message; }
    std::string ToString() const
    {
        if (IsValid()) {
            return "Valid";
        }

        if (!m_debug_message.empty()) {
            return m_reject_reason + ", " + m_debug_message;
        }

        return m_reject_reason;
    }
};

class TxValidationState : public ValidationState<TxValidationResult> {};
class BlockValidationState : public ValidationState<BlockValidationResult> {};

// These implement the weight = (stripped_size * 4) + witness_size formula,
// using only serialization with and without witness data. As witness_size
// is equal to total_size - stripped_size, this formula is identical to:
// weight = (stripped_size * 3) + total_size.
static inline int32_t GetTransactionWeight(const CTransaction& tx)
{
    return ::GetSerializeSize(tx, PROTOCOL_VERSION | SERIALIZE_TRANSACTION_NO_WITNESS) * (WITNESS_SCALE_FACTOR - 1) + ::GetSerializeSize(tx, PROTOCOL_VERSION);
}
static inline int64_t GetBlockWeight(const CBlock& block)
{
    return ::GetSerializeSize(block, PROTOCOL_VERSION | SERIALIZE_TRANSACTION_NO_WITNESS) * (WITNESS_SCALE_FACTOR - 1) + ::GetSerializeSize(block, PROTOCOL_VERSION);
}
static inline int64_t GetTransactionInputWeight(const CTxIn& txin)
{
    // scriptWitness size is added here because witnesses and txins are split up in segwit serialization.
    return ::GetSerializeSize(txin, PROTOCOL_VERSION | SERIALIZE_TRANSACTION_NO_WITNESS) * (WITNESS_SCALE_FACTOR - 1) + ::GetSerializeSize(txin, PROTOCOL_VERSION) + ::GetSerializeSize(txin.scriptWitness.stack, PROTOCOL_VERSION);
}

/** Compute at which vout of the block's coinbase transaction the witness commitment occurs, or -1 if not found */
inline int GetWitnessCommitmentIndex(const CBlock& block)
{
    int commitpos = NO_WITNESS_COMMITMENT;
    if (!block.vtx.empty()) {
        for (size_t o = 0; o < block.vtx[0]->vout.size(); o++) {
            const CTxOut& vout = block.vtx[0]->vout[o];
            if (vout.scriptPubKey.size() >= MINIMUM_WITNESS_COMMITMENT &&
                vout.scriptPubKey[0] == OP_RETURN &&
                vout.scriptPubKey[1] == 0x24 &&
                vout.scriptPubKey[2] == 0xaa &&
                vout.scriptPubKey[3] == 0x21 &&
                vout.scriptPubKey[4] == 0xa9 &&
                vout.scriptPubKey[5] == 0xed) {
                commitpos = o;
            }
        }
    }
    return commitpos;
}

#endif // BITCOIN_CONSENSUS_VALIDATION_H
