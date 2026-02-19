# BitcoinBT (BTCBT)

BitcoinBT (BTCBT) is an independent Bitcoin hard fork project
based on Bitcoin Core v26.

This repository contains the reference implementation of the
BitcoinBT node software, provided for public testing and
technical verification.

---

## Project Status

Public Test Environment (Pre-Mainnet)

BitcoinBT is currently operating in a public testing phase.
The network is live for validating consensus rules,
block production, mining behavior, and network stability.

Some features or pages may be incomplete or unavailable.
This is expected during active testing.

---

## Network Definition

- Inherits Bitcoin blockchain history up to block 903,844
- Fork activation height: 903,845
- Independent consensus rules apply after the fork
- Independent mining required (no automatic BTC holder allocation)

BitcoinBT does not modify or affect the Bitcoin (BTC) network.

---

## Key Consensus Parameters (Test Phase)

- Block interval: 5 minutes
- Difficulty adjustment: ASERT
- Maximum block size: up to 32 MB
- Halving interval: 210,000 blocks (~2 years)
- Genesis block: Bitcoin Genesis (block 0)

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

- Block height and block hash consistency
- Block interval behavior
- Difficulty adjustment behavior
- Mining reward issuance
- Node restart and reindex behavior
- Network propagation stability

---

## Mining (Test Only)

Algorithm: SHA-256

Stratum endpoint (test pool):

stratum+tcp://bitcoinbt.xyz:3333

Mining during this phase is for testing purposes only.

---

## Public Explorer

A public block explorer is provided for transparency
and real-time verification of on-chain behavior.

Explorer content may change without notice.

---

## Security & Contact

Please do not open public issues for consensus or security bugs.

Contact:  
info@bitcoinbt.xyz

---

## Window

https://github.com/bitcoinbtuser/bitcoinbt-source/releases/tag/v0.1.0-win64


---

## Source Code

https://github.com/bitcoinbtuser/bitcoinbt-source

Issues and pull requests are welcome for testing and review.

---

## License

This project is released under the MIT License.  
See the LICENSE file for details.
