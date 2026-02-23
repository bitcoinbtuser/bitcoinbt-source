# BitcoinBT (BTCBT)

BitcoinBT (BTCBT) is an independent Bitcoin hard fork project
based on Bitcoin Core v26.

This repository contains the reference implementation of the
BitcoinBT node software, provided for public testing,
technical validation, and consensus verification.

---

## Project Status

Public Test Environment (Pre-Mainnet)

BitcoinBT is currently operating in a public testing phase.
The network is live for validating consensus rules,
block production, mining behavior, issuance logic,
and network stability.

Consensus parameters described in this document
represent the intended mainnet specification,
unless explicitly marked as test-specific.

Some components or tools may be incomplete during active testing.

---

## Network Definition

- Inherits Bitcoin blockchain history from block 0 through 903,844
- Fork activation height: 903,845
- Independent consensus rules apply after activation
- Independent mining required (no automatic BTC holder allocation)

BitcoinBT operates independently and does not modify
or affect the Bitcoin (BTC) network.

---

## Monetary Policy

Total supply cap: **21,000,000 BTCBT**

Block interval: **300 seconds**

Halving interval: **210,000 blocks (~2 years)**

One-time special subsidy: **630,000 BTCBT**

The 630,000 BTCBT allocation is included within the
21,000,000 BTCBT total supply cap and does not create
additional inflation beyond the defined maximum supply.

The special subsidy is enforced at a defined
post-fork block height according to the consensus rules.

All issuance logic is implemented and enforced
at the consensus level and is publicly verifiable
in this repository.

There is no hidden premine.

---

## Key Consensus Parameters (Test Phase)

- Fork activation height: 903,845
- Target block interval: 300 seconds
- Difficulty adjustment algorithm: ASERT
- ASERT half-life: 172,800 seconds (2 days)
- Consensus maximum block size: 32 MB
- Halving interval: 210,000 blocks
- Total supply cap: 21,000,000 BTCBT

Consensus validation is deterministic.
Nodes running the same software will produce identical validation results for identical inputs.

---

## Build (Linux)

./autogen.sh  
./configure  
make  

---

## Run Node (Example)

./src/bitcoind -chain=btcbt -datadir=~/.bitcoinbt

---

## What to Test

- Block height and hash consistency
- Block interval behavior
- ASERT difficulty adjustment behavior
- Mining reward issuance correctness
- Node restart and reindex behavior
- Network propagation stability

---

## Mining (Test Only)

Algorithm: SHA-256

Stratum endpoint (test pool):

stratum+tcp://bitcoinbt.xyz:3333

Mining during this phase is for consensus validation purposes only.

---

## Public Explorer

A public block explorer is provided for transparency
and real-time verification of on-chain behavior.

Explorer content may change during active testing.

---

## Security & Contact

Do not disclose security or consensus-related issues publicly.

Contact:  
info@bitcoinbt.xyz

---

## Windows Release

https://github.com/bitcoinbtuser/bitcoinbt-source/releases/tag/v0.1.0-win64

---

## Source Code

https://github.com/bitcoinbtuser/bitcoinbt-source

Pull requests and technical review contributions are welcome.

---

## License

This project is released under the MIT License.  
See the LICENSE file for details.
