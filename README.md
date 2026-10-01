# BitcoinBT (BTCBT)

BitcoinBT (BTCBT) is an independent SHA-256 Proof-of-Work blockchain forked from Bitcoin at block height **903,844**.

The BitcoinBT network is currently operating as a live public mainnet with active mining, block production, public nodes, wallet support, explorer infrastructure, and mining pool services.

---

## Development Status

**A new phase of technical development and review began on October 1, 2026.**

BitcoinBT is beginning a new phase of development covering both the **BitcoinBT network and the official BitcoinBT mining pool**.

The purpose of this development phase is to review, improve, and further develop the technical infrastructure of the BitcoinBT ecosystem.

### Mining Pool — Development Goals

Current development areas under consideration include:

* Stratum compatibility improvements
* Broader compatibility with SHA-256 ASIC miners
* Mining job generation and distribution improvements
* Difficulty and share handling review
* Pool stability and performance improvements
* Miner connection and job-processing improvements

### BitcoinBT Network — Development Goals

Current development areas under consideration include:

* BitcoinBT Core improvements
* Network and node infrastructure improvements
* RPC and infrastructure improvements
* Mining-related improvements
* Performance and compatibility improvements
* Other technical improvements identified during development

> **Important:** These are current development goals and plans, not finalized specifications or guaranteed features. The actual scope and implementation may change depending on technical testing, compatibility, security considerations, and development results.

Existing mainnet and mining services will continue to operate during the development process whenever possible.

Significant changes will be tested and reviewed before deployment to the production environment.

Further development updates will be published as work progresses.

---

## Network Information

| Parameter             | Value                  |
| --------------------- | ---------------------- |
| Name                  | BitcoinBT              |
| Ticker                | BTCBT                  |
| Consensus             | SHA-256 Proof-of-Work  |
| Fork Height           | Bitcoin Block #903,844 |
| First BTCBT Block     | #903,845               |
| Block Time            | 5 Minutes              |
| Difficulty Adjustment | ASERT                  |
| Maximum Supply        | 21,000,000 BTCBT       |
| Maximum Block Size    | 32 MB                  |
| P2P Port              | 8333                   |
| RPC Port              | 8332                   |

---

## Features

* Bitcoin Core v26 Based
* Independent Public Mainnet
* SHA-256 ASIC Mining
* SegWit Support
* Taproot Support
* Schnorr Signatures
* ASERT Difficulty Adjustment
* Public Mining Pool Infrastructure
* Open Source Development

---

## Current Network Status

* Mainnet: Active
* Mining: Active
* Block Production: Active
* Explorer: Online
* Mining Pool: Online
* Wallet Support: Available

---

## Official Resources

### Website

https://bitcoinbt.xyz

### Block Explorer

https://explorer.bitcoinbt.xyz

### Mining Pool

https://pool.bitcoinbt.xyz

### GitHub Repository

https://github.com/bitcoinbtuser/bitcoinbt-source

### GitHub Discussions

https://github.com/bitcoinbtuser/bitcoinbt-source/discussions

### Telegram

https://t.me/Crypto_BTCBT

### X (Twitter)

https://x.com/BTCBT_BitcoinBT

### Email

[info@bitcoinbt.xyz](mailto:info@bitcoinbt.xyz)

---

## Windows Wallet

Official Windows wallet releases:

https://github.com/bitcoinbtuser/bitcoinbt-source/releases

SHA256 checksums are published with each release.

Users are encouraged to independently verify downloaded binaries before use.

---

## Build From Source

### Linux

```bash
./autogen.sh
./configure
make -j$(nproc)
```

### Run Node

```bash
./src/bitcoinbtd \
-chain=btcbt \
-datadir=$HOME/.bitcoinbt-mainnet
```

---

## Documentation

Project documentation and historical records are available in the `/docs` directory.

Important documents include:

* Mainnet Declaration
* Fork Validation Reports
* Network History
* Development Notes

---

## Security

Please review:

* SECURITY.md
* CONTRIBUTING.md
* CODE_OF_CONDUCT.md

before reporting issues or submitting pull requests.

Security vulnerabilities should be reported privately to:

[info@bitcoinbt.xyz](mailto:info@bitcoinbt.xyz)

---

## Source Code

BitcoinBT is open source.

Developers are encouraged to inspect, review, audit, compile, and contribute to the project through GitHub.

Consensus-related modifications should be reviewed carefully before deployment.

---

## License

BitcoinBT incorporates portions of Bitcoin Core released under the MIT License.

This project is distributed under the MIT License.

See the LICENSE file for details.
