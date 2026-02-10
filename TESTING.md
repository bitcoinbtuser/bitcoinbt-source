# BitcoinBT Testing Guide

This document describes how to participate in the **BitcoinBT (BTCBT)** public testing phase.

BitcoinBT is currently under active development and testing.
All testing feedback helps improve the stability and security of the future mainnet.

---

## Testing Scope

Testers are encouraged to participate in the following areas:

- Running a full node
- Syncing and validating the blockchain
- Mining and stratum connectivity testing
- Wallet creation and basic transactions
- Performance and stability observation
- Bug and issue reporting

---

## Network Overview

- Fork point: Bitcoin block height **903,844**
- BitcoinBT activation: **block 903,845**
- Proof-of-Work: **SHA-256d**
- Block interval: **5 minutes**

---

## Running a Full Node

### Build from source (recommended)

    ./autogen.sh
    ./configure
    make

### Run the node

    ./src/bitcoind -chain=btcbt

Allow some time for the initial sync to complete.

---

## Mining & Stratum Testing

A public test stratum is available.

- **Stratum URL**

    stratum+tcp://bitcoinbt.xyz:3333

- **Algorithm**: `sha256d`

### Example (cpuminer)

    ./cpuminer -a sha256d \
      -o stratum+tcp://bitcoinbt.xyz:3333 \
      -u <YOUR_BTCBT_ADDRESS> \
      -p x

ASIC miners using standard SHA-256d stratum protocols are also supported.

---

## Wallet Testing

Testers are encouraged to perform the following actions:

- Create a new wallet
- Generate receiving addresses
- Receive mining rewards
- Send basic transactions
- Observe transaction confirmations

Wallet behavior should be consistent with Bitcoin Core–based systems.

---

## Explorer & Monitoring

A public explorer is available for verification:

- Explorer: https://explorer.bitcoinbt.xyz

Testers may use the explorer to:

- Verify block production
- Track transactions
- Confirm mining rewards

---

## Reporting Issues

All bugs and issues should be reported via **GitHub Issues**.

When reporting an issue, please include:

- Node version or commit hash
- Operating system and environment
- Steps to reproduce the issue
- Logs or screenshots (if applicable)

For security-related issues, **do NOT open a public issue**.
Refer to `SECURITY.md`.

---

## What Not to Test

During the public testing phase:

- Do not assume network permanence
- Do not treat test funds as having real-world value
- Breaking changes may occur without notice

---

## Feedback

Constructive feedback from testers is highly appreciated.

Your participation directly contributes to:

- Network stability
- Consensus safety
- Long-term sustainability of BitcoinBT

Thank you for testing BitcoinBT.
