# BitcoinBT (BTCBT)
## Technical Specification for Exchange Review

---

## 1. Network Definition

BitcoinBT (BTCBT) is a SHA-256 Proof-of-Work blockchain implemented as a deterministic consensus fork inheriting the Bitcoin blockchain history up to block height **903,844**.

### Fork Structure

- **Historical Inheritance:** Blocks 0 – 903,844  
- **Consensus Activation Height:** 903,845  
- **Genesis Block:** Bitcoin genesis block (inherited, unchanged)  
- **Branching Mechanism:** Block-height-based deterministic switch  

Consensus activation is determined strictly by block height.  
No external signaling, governance voting, or centralized activation logic exists.

---

## 2. Post-Fork Consensus Parameters

The following parameters apply to blocks with height ≥ 903,845.

| Parameter | Value |
|------------|--------|
| Proof-of-Work | SHA-256 (double SHA-256) |
| Target Block Interval | 300 seconds |
| Difficulty Adjustment | ASERT |
| ASERT Half-Life | 172,800 seconds (2 days) |
| Consensus Max Block Size | 32 MB (32,000,000 bytes) |
| Halving Interval | 210,000 blocks |
| Maximum Supply (Production Mainnet) | 21,000,000 BTCBT |

All parameters above are enforced at the consensus level.

The special reward described below is included within this maximum supply cap.

---

## 3. Supply and Emission Structure

### 3.1 Maximum Supply

**Production Mainnet Maximum Supply:**  
21,000,000 BTCBT  

Issuance exceeding this cap constitutes a consensus violation and results in block invalidation.

---

### 3.2 Special Reward

A one-time special reward of **630,000 BTCBT** is issued at block height:

**903,850**

This reward:

- Is included within the 21,000,000 BTCBT maximum supply cap  
- Does not create additional inflation beyond the cap  
- Is enforced strictly at the consensus level  
- Does not alter the geometric subsidy reduction schedule beyond the defined maximum supply  

---

### 3.3 Block Subsidy Mechanism

Block subsidy is:

- Determined exclusively by block height  
- Halved every 210,000 blocks  
- Validated during block verification  

Changes to subsidy rules require consensus software updates and network adoption.

---

## 4. Difficulty Adjustment (ASERT)

BitcoinBT applies ASERT difficulty adjustment per block.

### Properties

- Difficulty recalculated for every block  
- No periodic retarget window  
- No minimum-difficulty exception rule  
- Continuous time-deviation-based correction  

### ASERT Configuration

- Half-life: 172,800 seconds  
- Reference anchor: Fork boundary block  

Difficulty recalculation cannot be disabled at runtime.

---

## 5. Block Structure

BitcoinBT preserves the inherited Bitcoin block structure.

### Block Header (80 bytes)

- nVersion (4 bytes)  
- hashPrevBlock (32 bytes)  
- hashMerkleRoot (32 bytes)  
- nTime (4 bytes)  
- nBits (4 bytes)  
- nNonce (4 bytes)  

### Validity Condition

BlockHash ≤ Target

Target is derived from `nBits` using compact encoding.

Blocks exceeding **32,000,000 bytes** are rejected at the consensus layer.

---

## 6. Signature and Script Rules

BitcoinBT:

- Does NOT introduce SIGHASH ForkID  
- Preserves Bitcoin-compatible signature validation  
- Preserves the UTXO model  
- Preserves the script execution model  

Transaction validation logic remains structurally equivalent to inherited rules.

---

## 7. Chain Selection Rule

Chain selection is determined exclusively by:

> Greatest accumulated proof-of-work

No centralized checkpointing logic is introduced beyond inherited architecture.

---

## 8. Network Architecture

BitcoinBT operates as a permissionless P2P network.

Each node independently:

- Validates transactions  
- Validates blocks  
- Propagates valid data  
- Rejects consensus-invalid blocks  

There is no centralized authority capable of overriding consensus rules.

---

## 9. Deterministic Validation

For identical input data and identical software version:

Nodes produce identical validation results.

Consensus parameters are defined as constants and height-based conditions within source code.  
Core consensus logic cannot be modified at runtime.

---

## 10. Development Base

- Codebase Origin: Bitcoin Core v26  

### Modification Scope

- Chain parameters  
- ASERT difficulty logic  
- Target block interval  
- Consensus max block size  
- Height-based branching logic  

Transaction verification structure and validation architecture remain unchanged.

---

## 11. Current Phase

The project is currently in a controlled validation and implementation verification phase.

### Internally Verified

- Post-fork consensus branching  
- Per-block ASERT difficulty adjustment  
- Block reward calculation path  
- Block size cap enforcement  
- Multi-node synchronization  

Large-scale external network validation is outside the scope of this document.

Test environments may operate with temporary or experimental parameters.  
Such environments do not modify or redefine the Production mainnet consensus specification described here.

---

## 12. Scope Limitation

This specification:

- Does not provide investment advice  
- Does not provide valuation guidance  
- Does not provide legal advice  
- Describes consensus parameters only  

The final authority for consensus rules is:

> The publicly released source code and the corresponding production mainnet release tag.
