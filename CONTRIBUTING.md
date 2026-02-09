# Contributing to BitcoinBT

Thank you for your interest in contributing to **BitcoinBT (BTCBT)**.

BitcoinBT is a Bitcoin Core v26–based implementation and a legitimate hard fork of Bitcoin.
The project is currently in a **public testing phase**, and contributions are welcome under the guidelines below.

---

## Project Status

BitcoinBT is under active development and testing.

- The chain has forked from Bitcoin at **block height 903,844**
- The `main` branch represents the current integration / staging tree
- Consensus-related code is handled **conservatively**

Not all pull requests may be accepted, especially those that impact consensus rules.

---

## What Contributions Are Welcome

We welcome contributions in the following areas:

- Bug fixes (non-consensus critical)
- Build system improvements
- Documentation improvements
- Test coverage and testing tools
- Performance optimizations that do not alter consensus
- Wallet / RPC / GUI improvements
- Platform compatibility fixes (Linux, Windows, macOS)

---

## Consensus & Protocol Changes (Important)

Changes that affect **consensus, mining, difficulty adjustment, block validation,
transaction rules, or network behavior** are subject to **strict review**.

Please note:

- Consensus changes **require discussion before implementation**
- Large or protocol-impacting changes may be declined even if technically correct
- The goal is long-term network stability and compatibility

If you are unsure whether your change affects consensus, **assume that it does and ask first**.

---

## Pull Request Process

1. **Fork the repository**
2. Create a feature branch from `main`
3. Make small, focused commits with clear messages
4. Open a Pull Request against the `main` branch
5. Clearly describe:
   - What the change does
   - Why it is needed
   - Whether it affects consensus (yes/no)

All commits must be submitted via Pull Requests.
Direct pushes to `main` are restricted.

---

## Coding Style

- Follow existing Bitcoin Core coding conventions
- Keep changes minimal and readable
- Avoid unrelated refactors in the same PR
- Comment complex logic where appropriate

---

## Testing Expectations

- Code should compile successfully
- New features should include tests where applicable
- Consensus-related changes must be tested thoroughly

Testing instructions can be found in the documentation or will be requested during review if needed.

---

## Reporting Issues & Discussions

- Use GitHub Issues for bugs and feature requests
- Clearly label whether an issue is:
  - Bug
  - Improvement
  - Question
- Avoid posting unverified security vulnerabilities publicly

---

## Security Issues

If you discover a security vulnerability:

- **Do NOT open a public issue**
- Contact the maintainers privately (see `SECURITY.md` if present)

---

## License

By contributing to BitcoinBT, you agree that your contributions will be licensed under
the **MIT License**, the same license as the rest of the project.

---

Thank you for helping improve BitcoinBT.
