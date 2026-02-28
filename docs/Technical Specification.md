# BitcoinBT (BTCBT)
## Technical Specification Summary  

---

## 1. Network Definition and Fork Structure

BitcoinBT (BTCBT) is a SHA-256d (double SHA-256) Proof-of-Work blockchain implemented as a deterministic consensus fork of Bitcoin.

BTCBT fully inherits Bitcoin’s blockchain history up to block height **903,844** and activates modified consensus rules starting at block **903,845**.

### 1.1 Fork Parameters

- **Fork Height (H_f):** 903,844  
- **Activation Height (H_a):** 903,845  
- **Genesis Block:** Bitcoin genesis block (unchanged)  
- **Consensus Switch:** Determined strictly by block height  
- **Chain Selection Rule:** Maximum accumulated work (`nChainWork`)

No governance vote, miner signaling, runtime parameter, or centralized trigger is involved in activation.

### 1.2 Deterministic Branch Function

\[
R(h) =
\begin{cases}
R_{BTC} & \text{if } h \le 903{,}844 \\
R_{BTCBT} & \text{if } h \ge 903{,}845
\end{cases}
\]

The fork boundary is a constant defined in chain parameters and cannot be altered.

BTCBT is not a snapshot-based replication.  
State continuity is achieved through full consensus re-validation from the inherited chain history.

---

## 2. Consensus Preservation and Modifications

### 2.1 Consensus Elements Preserved from Bitcoin

The following consensus structures remain identical to Bitcoin Core:

- 80-byte block header format  
- SHA-256d Proof-of-Work algorithm  
- UTXO-based state transition model  
- Script execution semantics  
- SegWit and Taproot validation paths  
- Chain selection via accumulated ChainWork  
- MedianTimePast (MTP) timestamp constraints  

BitcoinBT does not redesign the fundamental Bitcoin consensus architecture.

---

### 2.2 Modified Consensus Parameters

| Parameter | Bitcoin | BitcoinBT |
|-----------|----------|------------|
| Difficulty Adjustment | 2016-block retarget | ASERT |
| ASERT Half-life | — | 172,800 seconds |
| Target Block Interval | 600 seconds | 300 seconds |
| Max Serialized Block Size | 1 MB | 32 MB |
| Redistribution Logic | None | 630,000 BTCBT structured redistribution |

---

## 3. Difficulty Adjustment Algorithm (ASERT)

### 3.1 Anchor Definition

- **Anchor Height:**  
  \[
  h_0 = 903{,}845
  \]
- **Anchor Target:** \(T_0\)  
- **Anchor Timestamp:** \(t_0\)

---

### 3.2 Time Error

\[
\Delta t = (t_h - t_0) - 300(h - h_0)
\]

---

### 3.3 Target Formula

\[
Target(h) = T_0 \cdot 2^{\left(\frac{\Delta t}{172{,}800}\right)}
\]

Properties:

- Continuous per-block adjustment  
- No retarget window  
- Immediate response to hashrate variation  
- Upper bounded by `powLimit`

---

### 3.4 Header Consistency Condition

\[
Expand(nBits_h) = Target(h)
\]

If this condition fails, the block is rejected.

---

## 4. Monetary Policy and Supply Cap

### 4.1 Base Monetary Parameters

- Initial Subsidy:  
  \[
  S_0 = 50 \text{ BTCBT}
  \]
- Halving Interval:  
  \[
  I = 210{,}000 \text{ blocks}
  \]

---

### 4.2 Subsidy Function

\[
BaseSubsidy(h) = S_0 \gg \left\lfloor \frac{h}{I} \right\rfloor
\]

Subsidy is computed using integer arithmetic only.

---

### 4.3 Total Supply Cap

\[
TotalSupply \le 21{,}000{,}000 \text{ BTCBT}
\]

Supply converges mathematically as a geometric series.

---

## 5. 630,000 BTCBT Redistribution Structure

This mechanism modifies issuance timing without increasing long-term supply.

---

### 5.1 Redistribution Block

\[
H_r = 903{,}850
\]

\[
Subsidy(H_r) = BaseSubsidy(H_r) + 630{,}000
\]

---

### 5.2 Offset Schedule

For the subsequent 630,000 blocks:

\[
Subsidy(h) = BaseSubsidy(h) - 1
\]

Negative subsidy is not permitted.

---

### 5.3 Net Supply Invariance

\[
+630{,}000 - 630{,}000 = 0
\]

\[
TotalSupply \le 21{,}000{,}000
\]

The supply cap is unchanged.

---

## 6. Chain Selection and Security Model

### 6.1 Work per Block

\[
Work(h) =
\left\lfloor
\frac{2^{256}}{Target(h) + 1}
\right\rfloor
\]

---

### 6.2 Accumulated ChainWork

\[
ChainWork(h) = \sum_{i=0}^{h} Work(i)
\]

---

### 6.3 Active Chain Rule

\[
ActiveChain = \arg\max ChainWork
\]

Chain selection is determined by cumulative computational work, not block count.

---

## 7. Consensus Invariants

For every block height \(h\):

### 7.1 Proof-of-Work Validity

\[
Hash_{SHA256d}(Header_h) \le Target(h)
\]

---

### 7.2 Supply Cap Enforcement

\[
TotalSupply(h) \le 21{,}000{,}000
\]

---

### 7.3 ChainWork Monotonicity

\[
ChainWork(h) > ChainWork(h-1)
\]

---

### 7.4 Timestamp Constraints

\[
MTP(h-1) < Timestamp(h) \le NetworkTime + 7200
\]

---

### 7.5 Deterministic Consensus

\[
Consensus = F(Block_h, State_{h-1}, Constants)
\]

Consensus does not depend on external signaling, policy rules, or majority voting.

---

## 8. Summary

BitcoinBT preserves Bitcoin’s consensus security model while introducing:

- ASERT-based per-block difficulty adjustment  
- 5-minute block interval  
- 32MB maximum serialized block size  
- Structured reward redistribution without supply inflation  

The system remains fully deterministic, supply-capped, and work-secured.
