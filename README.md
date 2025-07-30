BitcoinBT integration/staging tree
==================================

https://bitcoinbt.xyz

For an immediately usable, binary version of the BitcoinBT software, see:  
➡️ https://github.com/bitcoinbtuser/bitcoinbt-source/releases

What is BitcoinBT?
------------------

**BitcoinBT (BTCBT)** is a full-node implementation based on **Bitcoin Core v26**, designed as a **legitimate hard fork of Bitcoin** at block height **903,844**. It connects to the BitcoinBT peer-to-peer network to validate blocks and transactions and includes an optional wallet and GUI interface.

BitcoinBT improves scalability and programmability while preserving compatibility with Bitcoin infrastructure.

### ✅ Key Features

- ⏱ **5-minute block intervals**
- 🎁 **Special genesis reward: 2,000,000 BTCBT**
- 📦 **Adaptive Block Size (4MB → 32MB, mempool-based)**
- 🔐 **Schnorr signatures + Taproot + MuSig2 (multi-signature)**
- ⚡ **SegWit-enabled and Lightning Network compatible**
- 🧱 **ASERT difficulty adjustment algorithm**
- 🔒 **Strict Bitcoin consensus: No CashTokens, No AI, No experimental forks**
- 🌍 **Global mining pool compatibility (SHA-256d standard PoW)**

Further information is available in the [doc folder](/doc).

License
-------

BitcoinBT is released under the terms of the MIT license.  
See [COPYING](COPYING) or https://opensource.org/licenses/MIT for more details.

Development Process
-------------------

The `main` branch is the actively maintained development branch.  
Official releases are tagged from it after stability and regression testing.

All changes—including consensus rules, wallet behavior, and network logic—are maintained in this unified repository.

- 📄 Contribution guidelines: [CONTRIBUTING.md](CONTRIBUTING.md)
- 🛠 Developer notes: [doc/developer-notes.md](doc/developer-notes.md)

Testing
-------

Testing is a core part of BitcoinBT’s development.

### 🔁 Automated Testing

- Use `make check` to run unit and integration tests.
- See [src/test/README.md](src/test/README.md) for full instructions.

### 🧪 Manual QA

- Large or risky changes must be tested by independent reviewers.
- Pull requests should include a test plan if not self-evident.

Translations
------------

BitcoinBT supports translations via [Transifex](https://www.transifex.com/bitcoin/bitcoin/).  
**⚠️ Note:** Do not submit translations via GitHub pull request—translation updates are synced separately.

---

BitcoinBT is the next evolution of Bitcoin hard forks, balancing innovation and global compatibility.
