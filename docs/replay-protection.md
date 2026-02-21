# Replay Protection

## Overview

BitcoinBT (BTCBT) is a height-based hard fork of Bitcoin,
activating independent consensus rules at block height 903,845.

Prior to block 903,845, BitcoinBT shares identical transaction history
with Bitcoin.

After block 903,845, consensus rules diverge permanently.
A one-time special subsidy is issued at block height 903,850,
further separating chain state.

Replay behavior depends on post-fork rule divergence
and transaction construction.

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
- Block subsidy rules diverge
- Maximum block size rule changes

At block 903,850:

- A one-time special subsidy of 630,000 BTCBT is issued

Due to these consensus changes,
blocks on each chain are validated differently.

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
- Divergence in UTXO state after special issuance

BitcoinBT retains Bitcoin's base script system,
so transaction structure remains compatible in principle.

However, independent difficulty targets,
subsidy divergence at 903,850,
and ongoing independent block production
reduce replay symmetry over time.

---

## Default Replay Policy

BitcoinBT does not introduce a mandatory new SIGHASH flag.

BitcoinBT does not modify transaction serialization format.

BitcoinBT does not add network-level replay protection opcodes.

Replay isolation relies on structural consensus divergence.

---

## Practical Replay Mitigation

Participants may mitigate replay risk by:

- Waiting for sufficient confirmations after block 903,845
- Ensuring transactions occur after block 903,850 divergence
- Splitting UTXOs on each network
- Broadcasting transactions only after chain separation stabilizes

Exchanges should implement confirmation policies
based on observed hash rate and reorganization depth.

---

## Chain Separation Factors

Chain separation increases due to:

- Independent block production after 903,845
- Independent ASERT difficulty recalculation
- Monetary divergence at block 903,850
- Divergent timestamps and chain work accumulation

As chain state diverges,
cross-chain replay feasibility decreases.

---

## Exchange Considerations

Exchanges integrating BitcoinBT should:

- Apply conservative confirmation requirements
- Monitor hash rate stability
- Verify replay isolation during testing
- Operate independent BitcoinBT full nodes

Replay behavior must be evaluated before enabling deposits or withdrawals.

---

## Full Node Enforcement

BitcoinBT nodes validate:

- Block height
- ASERT-derived difficulty
- Block subsidy correctness
- Special subsidy correctness at 903,850
- Block size compliance

Nodes enforcing Bitcoin rules will reject BitcoinBT blocks after 903,845.
BitcoinBT nodes will reject Bitcoin blocks beyond fork height.

---

## Summary

Replay protection in BitcoinBT relies on:

- Height-based consensus divergence at 903,845
- Special subsidy divergence at 903,850
- Independent difficulty recalculation
- Independent block production

BitcoinBT and Bitcoin operate as fully independent networks
after fork activation.

Replay feasibility decreases as chain state diverges.
