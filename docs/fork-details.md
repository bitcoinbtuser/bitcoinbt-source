# Fork Details

## Overview

BitcoinBT (BTCBT) is a direct height-based hard fork of Bitcoin.

The network inherits the full Bitcoin blockchain history
from block 0 (Genesis) through block height 903,844.

At block height 903,845, BitcoinBT activates independent
consensus rules and diverges permanently from the Bitcoin chain.

A one-time special subsidy is issued at block height 903,850.

---

## Shared History

- Genesis block: Bitcoin Genesis (block 0)
- Shared blocks: 0 → 903,844
- Shared transaction history up to fork height
- Identical UTXO state at height 903,844 (prior to fork activation)

Up to block 903,844, BitcoinBT follows unmodified Bitcoin Core v26 validation logic.

---

## Divergence Point

- Fork activation height: 903,845
- Special subsidy issuance height: 903,850
- Activation method: block height switch
- Rule enforcement: consensus-level modification

Starting at height 903,845:

- New block interval: 5 minutes
- ASERT difficulty adjustment replaces legacy retarget
- Modified block subsidy logic applies
- Modified maximum block size rules apply

At height 903,850:

- A one-time issuance of 630,000 BTCBT is executed
  through consensus-defined block subsidy logic

Nodes validating Bitcoin rules will reject BTCBT blocks
after block height 903,845.

---

## Consensus Separation

BitcoinBT modifies:

- Difficulty adjustment algorithm
- Block timing target
- Block subsidy schedule
- Maximum block size limit

Because these are consensus-critical parameters,
Bitcoin and BitcoinBT chains are permanently incompatible
after block 903,845.

There is no soft-fork compatibility mode.

---

## Replay Characteristics

BitcoinBT and Bitcoin share the same transaction history
only up to block 903,844.

After block 903,845:

- Consensus rules diverge
- Difficulty targets diverge
- Block validation rules diverge

After block 903,850:

- Monetary state diverges further due to special issuance

Due to consensus divergence,
post-fork blocks are not mutually valid across networks.

Replay behavior depends on transaction construction
and chain state divergence.

---

## Chain Identity

- Independent network
- Independent block production
- Independent difficulty adjustment
- Independent block validation

BitcoinBT does not modify, interrupt,
or otherwise affect the Bitcoin (BTC) main network.

---

## Node Enforcement

All fork behavior is:

- Enforced at consensus level
- Deterministic
- Automatically activated at height 903,845

Nodes running unmodified Bitcoin software
will reject BitcoinBT blocks after block 903,845.

Nodes running BitcoinBT software
will reject Bitcoin blocks beyond fork height.

---

## Technical Implementation Areas

Fork-related logic is implemented in:

- Chain parameter configuration
- Consensus rule definitions
- Difficulty adjustment logic (ASERT)
- Block subsidy calculation (including special issuance at 903,850)
- Validation and block acceptance logic

All logic is publicly auditable in the source repository.

---

## Summary

BitcoinBT represents a direct chain inheritance followed by
a deterministic consensus-level divergence at block 903,845.

Special monetary divergence occurs at block 903,850.

The fork is:

- Height-activated
- Consensus-enforced
- Permanent
- Publicly auditable

BitcoinBT and Bitcoin operate as fully independent networks
after the fork activation block.
