# BitcoinBT (BTCBT) – Frequently Asked Questions

This FAQ addresses common technical questions regarding the
**BitcoinBT (BTCBT)** public testing phase.

BitcoinBT is currently under active development and testing.
All answers below reflect the current testing status only.

---

## Q1. Is BitcoinBT mainnet or testnet?

BitcoinBT is currently in a **public testing phase**.

The network follows mainnet-like consensus rules and parameters,
but no guarantees of permanence or production readiness are implied.
Behavior may change as testing progresses.

---

## Q2. Is BitcoinBT a real Bitcoin hard fork?

Yes.

BitcoinBT is a Bitcoin hard fork based on **Bitcoin Core v26**.
It inherits the Bitcoin genesis block and all Bitcoin block history
up to block height **903,844**, applying new consensus rules from
block **903,845** onward.

---

## Q3. Do BTC holders receive BTCBT?

No.

BTC ownership is **not relevant**.
There are **no airdrops, claims, or automatic distributions**.

---

## Q4. What is being tested in this phase?

The current public test focuses on:

- Full node operation and synchronization
- Block validation and network behavior
- SHA-256d mining compatibility
- Wallet creation and basic transactions
- Stability across different environments

---

## Q5. Where can I download the software?

Windows binaries are available here:

https://github.com/bitcoinbtuser/bitcoinbt-source/releases

Linux and macOS users may build from source.

---

## Q6. Is mining supported?

Yes.

BitcoinBT uses **SHA-256d** Proof-of-Work.
A public test stratum is available for mining tests.

Mining is optional and intended for compatibility and stability testing.

---

## Q7. Are test coins worth anything?

No.

All coins on the network during this phase
have **no real-world value** and should not be treated as assets.

---

## Q8. Where is the documentation?

Current documentation is available in the repository:

- `README.md` – Project overview
- `TESTING.md` – Node, mining, and wallet testing guide
- `SECURITY.md` – Security reporting policy
- `docs/FAQ.md` – This document

Additional documentation may be expanded as testing continues.

---

## Q9. How can I report bugs or issues?

All technical issues should be reported via GitHub Issues:

https://github.com/bitcoinbtuser/bitcoinbt-source/issues

Security-related issues should follow the instructions in `SECURITY.md`.

---

## Q10. Is there a roadmap or whitepaper?

No formal roadmap or whitepaper is published at this stage.

BitcoinBT is currently focused on **technical validation and testing**.
Design decisions are reflected directly in the source code.

---

This FAQ may be updated as the project progresses.
