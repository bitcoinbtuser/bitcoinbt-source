BitcoinBT integration/staging tree
==================================

https://bitcoinbt.xyz

For an immediately usable, binary version of the BitcoinBT software, visit:  
➡️ https://github.com/bitcoinbtuser/bitcoinbt-source/releases

What is BitcoinBT?
------------------

**BitcoinBT (BTCBT)** is a full-node implementation based on **Bitcoin Core v26**, and a **legitimate, consensus-compatible hard fork of Bitcoin**. The chain splits at **block height 903,844** (July 3, 2025), inheriting all prior Bitcoin history up to that point.

BitcoinBT introduces technical enhancements focused on scalability and efficiency—while preserving full compatibility with existing global infrastructure such as mining pools, explorers, and wallets.

### ✅ Key Features

- ⏱ **5-minute block intervals**
- 🎁 **Special premined reward: 2,000,000 BTCBT**
- 📦 **Adaptive Block Size**  
  (4MB ~ 32MB, automatically adjusted based on mempool size)
- 🔐 **Taproot + Schnorr + MuSig2**  
  (next-gen multi-signature support)
- ⚡ **SegWit enabled / Lightning Network compatible**
- 📉 **ASERT Difficulty Adjustment Algorithm**  
  (used by Bitcoin Cash and other next-gen chains)
- 🧱 **Preserves original Bitcoin consensus**  
  ❌ No CashTokens  
  ❌ No AI  
  ❌ No non-standard opcode forks
- 🌍 **Globally compatible with mining pools, wallets, and explorers**  
  (SHA-256d PoW standard maintained)

> BitcoinBT delivers real-world improvements without sacrificing decentralization or compatibility.

Documentation
-------------

See the [doc folder](/doc) for technical guides, configuration details, and developer documentation.

License
-------

Released under the terms of the **MIT license**.  
See [COPYING](COPYING) or https://opensource.org/licenses/MIT.

Development Process
-------------------

The `main` branch is the actively maintained development line.  
Stable releases are tagged and published after full testing.

All protocol rules, wallet behavior, networking, and consensus logic live in this repository.

- 📄 [CONTRIBUTING.md](CONTRIBUTING.md) — Contribution process
- 🛠 [doc/developer-notes.md](doc/developer-notes.md) — Developer reference

Testing
-------

### 🔁 Automated Testing

- Build and run tests with:
  ```bash
  make check
