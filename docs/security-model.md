# Security Model

## Overview

BitcoinBT (BTCBT) inherits the core security architecture of Bitcoin,
including Proof-of-Work consensus and SHA-256 mining.

After block height 903,845, BitcoinBT operates under independent
consensus rules while maintaining PoW-based chain security.

---

## Proof-of-Work

- Algorithm: SHA-256
- Mining model: competitive PoW
- Block validity determined by hash target compliance

BitcoinBT does not modify the core Proof-of-Work structure.
Block validity requires meeting the network-defined difficulty target.

Security therefore depends on accumulated hash power.

---

## Hash Power Assumptions

BitcoinBT assumes:

- Independent hash power participation
- Economic incentives for miners
- Market-driven mining competition

As with Bitcoin, network security increases proportionally
with total active hash rate.

---

## 51% Attack Considerations

Like all PoW-based networks, BitcoinBT is subject to
majority hash power attack assumptions.

If an attacker controls >50% of total network hash rate,
they could:

- Reorganize recent blocks
- Delay confirmation finality
- Attempt double-spend attacks

Mitigations include:

- Distributed mining participation
- Public block monitoring
- Exchange confirmation policies
- Transparent hash rate reporting

Security strengthens as network hash power grows.

---

## ASERT Difficulty Adjustment

BitcoinBT replaces legacy difficulty retargeting
with ASERT (Absolutely Scheduled Exponentially Weighted Target).

Security characteristics of ASERT:

- Per-block difficulty adjustment
- Rapid response to hash rate volatility
- Reduced risk of timestamp manipulation compared to window-based retargets
- Predictable and anchor-based recalculation

ASERT prevents long stagnation periods caused by sudden hash rate drops.

---

## 5-Minute Block Interval Impact

BitcoinBT reduces block interval from 10 minutes to 5 minutes.

Security implications:

- Faster confirmation times
- More frequent difficulty recalculation events
- Slightly increased orphan probability under low propagation efficiency

Mitigation:

- Propagation efficiency improvements
- Modern node networking stack (Bitcoin Core v26 base)

---

## Block Size and Validation

Maximum block size: up to 32 MB

Security implications:

- Increased validation workload
- Higher bandwidth requirement
- Larger block propagation payload

Mitigations:

- Modern hardware baseline assumption
- Network bandwidth planning
- Standard mempool policy enforcement

Block validation rules remain consensus-enforced.

---

## Consensus Enforcement

Security is guaranteed by:

- Deterministic validation rules
- Open-source client enforcement
- Full node verification
- Rejection of invalid blocks

No centralized authority can override consensus rules.

---

## Monetary Integrity

BitcoinBT ensures:

- No administrative inflation controls
- No discretionary minting
- Height-based activation logic
- Consensus-defined special issuance

All supply rules are enforced during block validation.

---

## Replay and Chain Isolation

After fork activation:

- Difficulty rules diverge
- Block targets diverge
- Subsidy logic diverges
- Block validation logic diverges

Nodes enforcing Bitcoin rules will reject BitcoinBT blocks,
and vice versa.

This ensures structural separation of chains.

---

## Node Software Integrity

Security relies on:

- Open-source reviewability
- Deterministic builds
- Community verification
- Transparent repository

Code changes affecting consensus are publicly visible.

---

## Operational Security Considerations

Network participants should consider:

- Standard exchange confirmation requirements
- Monitoring reorganization depth
- Observing network hash rate stability
- Maintaining up-to-date node versions

Security posture strengthens with distributed participation.

---

## Summary

BitcoinBT security is based on:

- SHA-256 Proof-of-Work
- Deterministic consensus rules
- ASERT difficulty stability
- Independent chain separation
- Public auditability

Network security increases as hash rate and participation grow.
