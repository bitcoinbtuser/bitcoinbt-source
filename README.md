# BitcoinBT (BTCBT)

BitcoinBT (BTCBT) is an independent Bitcoin hard fork project
based on Bitcoin Core v26.

This repository contains the reference implementation of the
BitcoinBT node software for public testing,
technical validation, and deterministic consensus verification.

BitcoinBT operates independently and does not modify
or affect the Bitcoin (BTC) network.

---

## Project Status

**Public Test Phase (Pre-Mainnet Validation)**

BitcoinBT is currently operating in a public validation environment.

The live test network is used to verify:

- Consensus rule enforcement  
- Block production behavior  
- Mining performance  
- Reward issuance logic  
- ASERT difficulty adjustment  
- Network propagation stability  

Consensus parameters described in this repository
represent the intended mainnet specification,
unless explicitly marked as test-specific.

Mainnet finalization has not yet been declared.

---

## Deterministic Fork Structure

BitcoinBT inherits Bitcoin blockchain history from:

Block 0 through 903,844

Fork activation height:

**903,845**

Formal rule transition:

For block height \( h \):

-Bitcoin rules apply when h <= 903,844
-BitcoinBT rules apply when h >= 903,845

The fork boundary is constant and defined at the consensus level.

There is no governance voting,
no miner signaling threshold,
no runtime override,
and no operator discretion involved in activation.

Consensus transition depends exclusively on block height.

---

## Monetary Policy

Total Supply Cap:

**21,000,000 BTCBT**

Block Interval:

**300 seconds**

Halving Interval:

**210,000 blocks (~2 years)**

---

### Special Subsidy

One-time allocation:

**630,000 BTCBT**

This allocation is included within the
21,000,000 BTCBT total supply cap.

It does not create additional inflation.

Offset structure:

+630,000 − (630,000 × 1 reward reduction) = 0

The long-term total supply remains capped at:

21,000,000 BTCBT

There is no hidden premine.

All issuance logic is enforced at the consensus level
and is publicly verifiable in the source code.

---

## Key Consensus Parameters (Validation Phase)

- Fork activation height: 903,845  
- Target block interval: 300 seconds  
- Difficulty adjustment algorithm: ASERT  
- ASERT half-life: 172,800 seconds (2 days)  
- Maximum consensus block size: 32 MB  
- Halving interval: 210,000 blocks  
- Supply cap: 21,000,000 BTCBT  

Consensus validation is deterministic.

Nodes running identical software will produce
identical validation results for identical input data.

---

## Build (Linux)

```bash
./autogen.sh
./configure
make  
```
---

## Run Node (Example)

./src/bitcoind -chain=btcbt -datadir=~/.bitcoinbt

---

## What to Test

Participants in the public validation phase are encouraged to verify the following behaviors and invariants:

- Block height progression and hash continuity
- Deterministic validation across multiple independently running nodes
- Block interval behavior (target approximately 300 seconds)
- Timestamp handling consistency
- ASERT difficulty adjustment response under varying hashrate conditions
- Mining reward issuance correctness at standard blocks
- Special subsidy enforcement at the designated activation height
- Halving interval correctness (210,000 blocks)
- Node restart stability
- Full reindex behavior consistency
- Chain reorganization handling
- Network propagation latency and peer connectivity stability
- Consistency of block template generation (getblocktemplate)
- Valid block submission behavior (submitblock)

Validation results should be reproducible when running identical software under identical conditions.

---

## Mining (Test Only)

Algorithm: SHA-256

Stratum endpoint (test pool):

stratum+tcp://bitcoinbt.xyz:3333

Mining during this validation phase is intended exclusively for:

- Consensus rule validation
- ASERT difficulty algorithm verification
- Reward issuance testing
- Block propagation testing
- Fork boundary validation

No economic guarantees are implied during the test phase.

Mining during testing does not represent final mainnet economics.

---

## Public Explorer

A public block explorer is provided for transparency and real-time verification of on-chain behavior.

Explorer functionality may include:

- Block height and hash lookup
- Transaction inspection
- Difficulty tracking
- Supply progression monitoring
- Reward verification

Explorer content and structure may change during active validation.

---

## Security & Contact

Do not disclose security or consensus-related vulnerabilities publicly.

Responsible disclosure is required.

Contact:

info@bitcoinbt.xyz

When reporting issues, include:

- Reproduction steps
- Log excerpts (if applicable)
- Software version
- Operating environment details

---

## Windows Release

Precompiled Windows binaries are available at:

https://github.com/bitcoinbtuser/bitcoinbt-source/releases/tag/v0.1.0-win64

Verify checksums before execution.

---

## Source Code

Repository:

https://github.com/bitcoinbtuser/bitcoinbt-source

The repository includes:

- Full node source code
- Consensus parameter definitions
- Difficulty algorithm implementation
- Build configuration scripts

Pull requests and technical review contributions are welcome.

Consensus-related changes should be reviewed carefully before merge.

---

## License

This project is released under the MIT License.

See the LICENSE file for full details.

The project incorporates portions of Bitcoin Core under the MIT license in compliance with its original licensing terms.
