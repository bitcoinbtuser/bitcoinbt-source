# BitcoinBT (BTCBT)
## Network, Monetary & Treasury Roadmap  


---

# Definitions

- $S_{max}$ : Maximum total supply  
- $S_{special}$ : Special reward allocation  
- $I$ : Epoch interval (210,000 blocks)  
- $Sale_y$ : Executed treasury amount in year $y$  
- $C_{init}$ : Initial cumulative cap (Phase I)  
- $C_{year}$ : Annual cap (Post-2031)

---

# 1. Consensus and Supply Invariance Structure

## 1.1 Deterministic Fork Definition

BitcoinBT (BTCBT) is based on Bitcoin Core v26.  
It inherits the Bitcoin blockchain history up to block height **903,844**.

From block height **903,845**, BTCBT consensus rules apply.

Formal transition function:

$$
R(h) =
\begin{cases}
R_{BTC}, & h \leq 903{,}844 \\
R_{BTCBT}, & h \geq 903{,}845
\end{cases}
$$

The transition depends exclusively on block height.  
It cannot be altered by governance voting, operator discretion, external signaling, or runtime configuration.

---

## 1.2 Total Supply Cap

$$
S_{max} = 21{,}000{,}000
$$

The supply cap is enforced at the consensus level.

There exists no mechanism for:

- Additional issuance  
- Supply expansion  
- Cap alteration outside predefined rules  

---

## 1.3 Special Reward and Offset Mechanism

$$
S_{special} = 630{,}000
$$

This allocation is included within $S_{max}$.  
It does not introduce net inflation.

Base reward adjustment:

$$
R_{adjusted} = 50 - 1 = 49
$$

Applied over:

$$
630{,}000 \text{ blocks}
$$

Offset verification:

$$
+630{,}000 - (630{,}000 \times 1) = 0
$$

### Long-Term Supply Identity

$$
S_{total} = S_{base} - Offset + S_{special}
$$

Where:

$$
Offset = 630{,}000
$$

$$
S_{special} = 630{,}000
$$

Therefore:

$$
S_{total} = S_{max} = 21{,}000{,}000
$$

Total supply remains unchanged.

---

# 2. 49 Reward Structure and Epoch Alignment

Epoch interval:

$$
I = 210{,}000
$$

Issuance under reward 50:

$$
50 \times 210{,}000 = 10{,}500{,}000
$$

Issuance under reward 49:

$$
49 \times 210{,}000 = 10{,}290{,}000
$$

Difference:

$$
10{,}500{,}000 - 10{,}290{,}000 = 210{,}000
$$

The 49-reward structure reduces issuance by exactly **210,000 per epoch**.

The Phase I treasury cap numerically equals this structural issuance reduction.

---

# 3. Phase I — 2026–2027 Listing and Expansion

## 3.1 Initial Cumulative Cap

$$
C_{init} = 210{,}000
$$

Constraint:

$$
\sum_{y=2026}^{2027} Sale_y \le 210{,}000
$$

---

## 3.2 Execution Conditions

Execution may occur only when all conditions are satisfied.

### Market Stability
- Adequate average volume  
- Sufficient order book depth  
- No material price distortion  

### Operational Necessity
- Exchange listing maintenance  
- Security audits  
- Infrastructure costs  

### Network Stability
- No abnormal hashrate decline  
- No consensus instability  
- Not immediately after major protocol updates  

If conditions are not satisfied:

$$
Sale_y = 0
$$

---

## 3.3 Execution Principles

- No bulk dumping  
- Tranche-based execution  
- Internal approval required  
- Public disclosure permitted  
- No unilateral discretionary execution structure  

---

## 3.4 Global Mining Pool Expansion (2026–2027)

- Integration with global SHA-256 pools  
- Full Stratum compatibility  
- `getblocktemplate` validation  
- `submitblock` validation  
- Multi-pool interoperability testing  
- Avoid single-pool dependency  

---

# 4. Phase II — 2028–2030 Mandatory Lock-Up

$$
\forall y \in [2028,2030], \quad Sale_y = 0
$$

This period constitutes a structurally non-supply interval.

---

# 5. Phase III — Post-2031 Long-Term Framework

Remaining treasury:

$$
630{,}000 - 210{,}000 = 420{,}000
$$

## Annual Cap

$$
C_{year} = 20{,}000
$$

Constraint:

$$
Sale_y \le 20{,}000 \quad (y \ge 2031)
$$

## No Carry-Over

$$
CarryOver = 0
$$

Unused allocation does not accumulate into subsequent years.

## Maximum Theoretical Horizon

$$
420{,}000 \div 20{,}000 = 21 \text{ years}
$$

---

# 6. Non-Modification Clauses

No plans exist to modify:

- $C_{init} = 210{,}000$  
- $C_{year} = 20{,}000$  
- 2028–2030 lock-up  
- $S_{max} = 21{,}000{,}000$  

Any structural change requires:

- Official disclosure  
- Announcement prior to effect  
- Reasonable grace period  

---

# 7. Risk Containment Matrix

| Risk | Control Mechanism |
|------|-------------------|
| Unlimited selling | Hard caps |
| Automatic execution misinterpretation | Explicitly denied |
| Carry-over acceleration | Prohibited |
| Cap overflow | Formula-constrained |
| Lock-up violation | Structural zero window |
| Supply inflation | Offset equation |
| Hashrate concentration | Global pool strategy |

---

# 8. Disclosure and Transparency Framework

## 8.1 Disclosure Principle

Execution under Phase I and III follows a prior-or-concurrent disclosure principle via:

- Official website  
- Official GitHub repository  
- Official announcements  

---

## 8.2 Cap-Change Procedure

The following cannot be modified solely by internal discretion:

- 210,000 cumulative cap  
- 20,000 annual cap  
- 2028–2030 lock-up  

Structural changes require formal disclosure before implementation.

---

## 8.3 On-Chain Verifiability

Treasury wallet addresses will be disclosed after mainnet stabilization.

After disclosure:

- On-chain movements are publicly verifiable  
- Executed amounts can be reconciled against defined caps  

---

## 8.4 Immutable Supply Principle

$$
S_{max} = 21{,}000{,}000
$$

The total supply cap is enforced at the consensus level and cannot be altered by operational policy or internal discretion.

---

# Execution Disclaimer

All defined caps represent maximum allowable limits.  
They do not constitute an obligation or automatic execution plan.

No predetermined selling schedule exists.  
All execution decisions remain condition-based.

---

# Legal Notice

This document defines structural monetary limits.  
It does not constitute financial advice or investment solicitation.
