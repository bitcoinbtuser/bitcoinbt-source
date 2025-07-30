BitcoinBT integration/staging tree
==================================

https://bitcoinbt.xyz

For an immediately usable, binary version of the BitcoinBT software, see  
https://github.com/bitcoinbtuser/bitcoinbt-source/releases

What is BitcoinBT?
------------------

**BitcoinBT (BTCBT)** is a Bitcoin Core v26-based full node implementation that connects to the BitcoinBT peer-to-peer network to download and fully
validate blocks and transactions. It also includes a wallet and an optional graphical user interface.

BitcoinBT is a **legitimate hard fork of Bitcoin**, with enhancements to improve scalability, efficiency, and on-chain programmability.  

Key features include:

- ⏱ **5-minute block intervals**
- 🎁 **Genesis reward: 2,000,000 BTCBT**
- 📦 **Adaptive Block Size (up to 32MB)**
- 🔐 **Schnorr + Taproot + MuSig2**
- 🧠 **CashTokens for advanced smart contracts**
- ⛓️ **SegWit-enabled and Lightning Network compatible**

Further information is available in the [doc folder](/doc).

License
-------

BitcoinBT is released under the terms of the MIT license.  
See [COPYING](COPYING) or https://opensource.org/licenses/MIT for more information.

Development Process
-------------------

The `main` branch is actively developed and regularly updated.  
Stable versions of BitcoinBT are tagged from these builds to indicate official releases.

All development, including GUI (if applicable), is maintained in this repository.  
If contributing new features, bug fixes, or protocol improvements, please fork from the `main` branch.

See [CONTRIBUTING.md](CONTRIBUTING.md) for the contribution process, and refer to [doc/developer-notes.md](doc/developer-notes.md) for developer guidelines.

Testing
-------

Code review and testing are critical to the stability and security of BitcoinBT.  
We welcome help reviewing other contributors' pull requests and submitting thorough test coverage.

### Automated Testing

Developers are encouraged to write [unit tests](/src/test/README.md) for all new code.  
Tests can be compiled and run using:

```bash
make check
