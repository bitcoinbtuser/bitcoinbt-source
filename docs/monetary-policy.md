## Monetary Policy

Overview

BitcoinBT (BTCBT) follows a consensus-enforced issuance model activated at the fork height.
All monetary parameters are defined at the protocol level and are publicly auditable in the source code.

BitcoinBT inherits the Bitcoin blockchain history up to block height 903,844.
At block height 903,845, independent consensus rules become active.

---

## Fork Activation

Inherited chain: Bitcoin (BTC)
Last shared block: 903,844
Fork activation block: 903,845

All monetary changes take effect starting at block 903,845.

---

## Block Interval

Target block time: 5 minutes
Consensus-enforced
Independent difficulty adjustment using ASERT

With a 5-minute block interval, the annual block production differs from Bitcoin (10-minute interval).

---

## Halving Schedule

Halving interval: 210,000 blocks
Estimated halving period: approximately 2 years
Enforced by consensus rules

The halving logic is implemented in the block subsidy calculation within the node software.

---

## Special One-Time Subsidy Allocation

Amount: 630,000 BTCBT
Activation block: 903,845
Type: One-time consensus-defined issuance

This allocation is:

Defined directly in the consensus rules
Triggered only at fork activation height
Transparent and verifiable in the public repository
Not hidden in any off-chain mechanism

The special allocation is implemented at the protocol level and enforced by all validating nodes.

---

## Ongoing Block Rewards

After the fork activation block:

Standard block subsidy applies
Reward schedule follows halving rules
Rewards are issued only through valid mined blocks

No coins are created outside consensus-defined issuance.

---

## No Hidden Premine

BitcoinBT does not include:

Stealth minting mechanisms
Off-chain allocations
Administrative inflation controls

All issuance rules are enforceable and verifiable through node validation.

---

## Auditability

All monetary parameters can be verified by:

Reviewing consensus rules in source code
Running a full node
Inspecting block rewards through the public explorer
Verifying block subsidy logic at fork height

This ensures deterministic and reproducible issuance.

---

## Supply Transparency

All minted BTCBT:

Are recorded on-chain
Follow consensus-defined issuance
Are independently verifiable by any full node operator

There are no discretionary or manually triggered supply mechanisms.

---

## Conclusion

BitcoinBT monetary issuance is:

Consensus-defined
Deterministic
Publicly auditable
Activated at a clearly defined fork height

All parameters are implemented in open-source code and are enforceable by network validation rules.
