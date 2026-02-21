# Security Model

## Overview

BitcoinBT (BTCBT) inherits the core security architecture of Bitcoin,
including Proof-of-Work consensus and SHA-256 mining.

The network shares chain history through block 903,844.
At block height 903,845, independent consensus rules activate.
A one-time special subsidy is issued at block height 903,850.

After block 903,845, BitcoinBT operates as an independent PoW network.

---

## Proof-of-Work

- Algorithm: SHA-256
- Mining model: competitive Proof-of-Work
- Block validity determined by hash target compliance

BitcoinBT does not modify the core PoW hashing function.
Block validity requires meeting the network-defined ASERT-adjusted difficulty target.

Network security depends on accumulated hash power.

---

## Hash Power Assumptions

BitcoinBT assumes:

- Independent hash power participation
- Economic incentives for miners
- Market-driven competition

Security increases proportionally with total active hash rate,
similar to Bitcoin.

---

## 51% Attack Considerations

Like all PoW networks, BitcoinBT is subject to majority hash power assumptions.

If an entity controls more than 50% of network hash rate,
they could:

- Reorganize recent blocks
- Delay confirmation finality
- Attempt double-spend attacks

Mitigation factors include:

- Distributed mining participation
- Exchange confirmation policies
- Public monitoring of hash rate
- Economic incentives aligned with honest mining

Security strengthens as hash rate grows.

---

## ASERT Difficulty Adjustment

BitcoinBT replaces legacy retarget windows with:

ASERT (Absolutely Scheduled Exponentially Weighted Target)

Security characteristics:

- Per-block difficulty adjustment
- Rapid response to sudden hash rate changes
- Deterministic anchor-based recalculation
- Reduced risk of prolonged stagnation

ASERT improves stability under volatile mining conditions.

---

## 5-Minute Block Interval Impact

Block interval is reduced to 5 minutes.

Security implications:

- Faster average confirmation times
- More frequent block creation
- Slightly increased orphan probability in low bandwidth environments

Mitigation:

- Modern networking stack (Bitcoin Core v26 base)
- Standard relay policies
- Efficient block propagation

---

## Block Size and Validation

Maximum block size: up to 32 MB

Security considerations:

- Increased validation cost
- Higher bandwidth requirements
- Larger propagation payload

Mitigation:

- Standard mempool policy enforcement
- Hardware assumptions aligned with modern infrastructure
- Full node validation remains mandatory

---

## Monetary Integrity

BitcoinBT enforces:

- Height-based fork activation at 903,845
- One-time special issuance at 903,850
- Deterministic block subsidy schedule
- 210,000 block halving interval
- No administrative minting authority

All issuance rules are consensus-defined and validated by nodes.

---

## Replay and Chain Isolation

After fork activation (903,845):

- Difficulty adjustment diverges
- Block timing diverges
- Monetary rules diverge
- Block size rules diverge

At block 903,850, the special subsidy further differentiates
BitcoinBT chain state from Bitcoin.

Nodes enforcing Bitcoin rules reject BitcoinBT blocks after 903,845.
Nodes enforcing BitcoinBT rules reject Bitcoin blocks beyond that height.

This ensures permanent structural separation.

---

## Node Software Integrity

Security relies on:

- Open-source validation logic
- Deterministic rule enforcement
- Publicly auditable consensus parameters
- Transparent repository maintenance

Consensus-critical logic is fully visible in source code.

---

## Operational Security Considerations

Participants should:

- Apply appropriate confirmation policies
- Monitor hash rate stability
- Observe reorganization depth
- Maintain up-to-date node software

Network robustness increases with distributed participation.

---

## Summary

BitcoinBT security is based on:

- SHA-256 Proof-of-Work
- Independent consensus activation at 903,845
- One-time subsidy issuance at 903,850
- ASERT per-block difficulty adjustment
- Deterministic and publicly auditable rules

Security strengthens proportionally with network participation and hash rate.
