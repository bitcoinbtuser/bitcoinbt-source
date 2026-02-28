// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CONSENSUS_CONSENSUS_H
#define BITCOIN_CONSENSUS_CONSENSUS_H

#include <cstdlib>
#include <stdint.h>

/** The maximum allowed size for a serialized block, in bytes (only for buffer size limits) */
static const unsigned int MAX_BLOCK_SERIALIZED_SIZE = 32000000; // BTCBT: allow up to 32MB blocks to be received/handled

/**
 * The maximum allowed block weight (BIP141).
 * NOTE: This constant is used where the block height is unknown (e.g., non-contextual checks),
 *       to preserve pre-fork Bitcoin validation behavior.
 *       BTCBT post-fork block limits are enforced contextually (with height) via consensus parameters.
 */
static const unsigned int MAX_BLOCK_WEIGHT = 4000000;

/** The maximum allowed number of signature check operations in a block (network rule). */
static const int64_t MAX_BLOCK_SIGOPS_COST = 80000;

/** Coinbase transaction outputs can only be spent after this number of new blocks (network rule). */
static const int COINBASE_MATURITY = 100;

static const int WITNESS_SCALE_FACTOR = 4;

static const size_t MIN_TRANSACTION_WEIGHT = WITNESS_SCALE_FACTOR * 60; // 60 is the lower bound for the size of a valid serialized CTransaction
static const size_t MIN_SERIALIZABLE_TRANSACTION_WEIGHT = WITNESS_SCALE_FACTOR * 10; // 10 is the lower bound for the size of a serialized CTransaction

/** Flags for nSequence and nLockTime locks */
/** Interpret sequence numbers as relative lock-time constraints. */
static constexpr unsigned int LOCKTIME_VERIFY_SEQUENCE = (1 << 0);

#endif // BITCOIN_CONSENSUS_CONSENSUS_H
