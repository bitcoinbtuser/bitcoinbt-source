# Technical Specification

## Overview

BitcoinBT (BTCBT) is an independent Bitcoin hard fork based on Bitcoin Core v26.

BitcoinBT inherits the Bitcoin blockchain history up to block height 903,844.
Independent consensus rules activate at block height 903,845.

After the fork height, BitcoinBT operates as a fully independent network.

---

## Chain Inheritance

- Genesis block: Bitcoin Genesis (block 0)
- Shared chain history: block 0 to 903,844
- Divergence height: 903,845
- Independent chain ID: BTCBT

Blocks after height 903,845 are validated under BitcoinBT consensus rules only.

---

## Consensus Changes

The following consensus modifications are applied starting at fork activation:

- Target block interval: 5 minutes
- Difficulty adjustment algorithm: ASERT
- Maximum block size: up to 32 MB
- Halving interval: 210,000 blocks (~2 years)
- One-time special subsidy allocation at fork block

These parameters are enforced at the protocol level.

---

## Fork Activation Block

- Fork block height: 903,845
- Activation rule: height-based consensus switch

At this block:

- BitcoinBT consensus rules become active
- Special one-time subsidy allocation is triggered
- ASERT difficulty rules apply

All validating nodes must enforce these rules.

---

## Difficulty Adjustment

Algorithm: ASERT (Absolutely Scheduled Exponentially Weighted Target)

Characteristics:

- Smooth and per-block difficulty adjustment
- Eliminates legacy retarget window
- Stabilizes block intervals under hash rate volatility
- Anchor-based deterministic adjustment

ASERT parameters are embedded in consensus validation logic.

---

## Block Parameters

- Target block time: 300 seconds
- Max block size: up to 32 MB
- Block reward logic: consensus-defined subsidy calculation
- Coinbase validation: enforced per protocol rules

All block validity rules are implemented in validation and mining logic.

---

## Monetary Rules

- Block interval: 5 minutes
- Halving interval: 210,000 blocks
- Special allocation: 630,000 BTCBT at fork activation
- No hidden minting
- No administrative supply modification

All issuance logic is deterministic and auditable.

---

## Mining

- Algorithm: SHA-256 (compatible with Bitcoin mining hardware)
- Independent hash power required
- No merged mining assumed by default

Mining RPC and validation follow Bitcoin Core v26 structure with consensus modifications.

---

## Replay Considerations

BitcoinBT operates under independent consensus after fork height.
Transactions valid on Bitcoin are not automatically valid on BitcoinBT post-fork.

Replay behavior depends on script validation differences and consensus divergence.

---

## Node Software

Based on:
- Bitcoin Core v26

Modified components include:
- Consensus parameters
- Difficulty adjustment logic
- Block subsidy calculation
- Fork activation enforcement

Source code:
https://github.com/bitcoinbtuser/bitcoinbt-source

---

## Validation Enforcement

All rules described above are:

- Enforced by consensus validation
- Deterministic
- Verifiable by full node execution

Nodes that do not enforce these rules will reject BitcoinBT blocks after fork height.

---

## Summary

BitcoinBT implements:

- Direct Bitcoin chain inheritance
- Height-based fork activation
- 5-minute block interval
- ASERT difficulty adjustment
- 32 MB maximum block size
- Deterministic halving schedule
- Consensus-defined one-time subsidy allocation

All parameters are transparent and enforced at the protocol level.
