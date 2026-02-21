# Replay Protection

## Overview

BitcoinBT (BTCBT) is a height-based hard fork of Bitcoin,
activating independent consensus rules at block height 903,845.

Prior to block 903,845, BitcoinBT shares identical transaction history
with Bitcoin.

After block 903,845, consensus rules diverge permanently.

Replay behavior depends on post-fork rule divergence and
transaction construction.

---

## Pre-Fork State

- Shared blockchain history through block 903,844
- Identical UTXO state at height 903,844
- Transactions valid on Bitcoin are equally valid on BitcoinBT prior to fork

No replay conditions exist before fork activation.

---

## Post-Fork Consensus Divergence

Beginning at block 903,845:

- Block interval changes (5 minutes)
- Difficulty adjustment switches to ASERT
- Block subsidy rules change
- Maximum block size rule changes
- One-time special subsidy allocation occurs

Due to consensus changes, blocks on each chain are validated differently.

---

## Replay Risk Analysis

Replay risk refers to the possibility that a transaction
broadcast on one chain is also valid and accepted on the other chain.

Replay risk depends on:

- Script compatibility
- Signature rules
- Transaction format consistency
- Network policy enforcement
- Chain height differences

Because BitcoinBT retains Bitcoin's base script system,
transaction structure remains compatible in principle.

However, diverging difficulty targets and block validation rules
reduce practical replay symmetry over time.

---

## Default Replay Policy

BitcoinBT does not introduce a mandatory new SIGHASH flag.

BitcoinBT does not modify transaction serialization format.

BitcoinBT does not add network-level replay protection opcodes.

Replay behavior is governed by standard consensus divergence
after fork activation.

---

## Practical Replay Mitigation

Participants may mitigate replay risk by:

- Waiting for sufficient post-fork confirmations
- Using chain-specific dust separation techniques
- Splitting UTXOs across networks
- Broadcasting transactions only after chain separation stabilizes

Exchanges should implement confirmation policies
appropriate for network hash rate and observed reorganization depth.

---

## Chain Separation Factors

Chain separation increases over time due to:

- Independent block production
- Independent difficulty adjustments
- Independent subsidy issuance
- Divergent block timestamps

As chain state diverges, transaction replay feasibility decreases.

---

## Exchange Considerations

Exchanges integrating BitcoinBT should:

- Require adequate confirmation depth
- Monitor network hash rate stability
- Verify replay isolation before enabling deposits/withdrawals
- Conduct independent node validation

Replay behavior should be evaluated operationally
during integration testing.

---

## Full Node Enforcement

Nodes validate:

- Block height
- Block difficulty
- Block subsidy correctness
- Block size rules
- ASERT-derived targets

Nodes that do not enforce BitcoinBT consensus rules
will reject post-fork blocks.

---

## Summary

Replay protection in BitcoinBT relies on:

- Height-based consensus divergence
- Difficulty algorithm separation
- Subsidy rule divergence
- Independent block production

BitcoinBT and Bitcoin operate as fully independent networks
after block height 903,845.

Replay behavior must be evaluated based on transaction timing
and chain state divergence.
