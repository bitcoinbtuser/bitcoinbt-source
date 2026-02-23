BitcoinBT (BTCBT)
Technical Specification for Exchange Review
________________________________________
1. Network Definition
BitcoinBT (BTCBT) is a SHA-256 Proof-of-Work blockchain implemented as a deterministic consensus fork inheriting the Bitcoin blockchain history up to block height 903,844.
•	Historical inheritance: Blocks 0 ~ 903,844
•	Consensus activation height: 903,845
•	Genesis block: Bitcoin genesis block (inherited, unchanged)
•	Consensus branching method: Block-height-based deterministic switch
No external signaling, governance vote, or centralized activation logic exists.
Consensus activation is determined strictly by block height.
________________________________________
2. Consensus Parameter Summary
The following consensus parameters apply to post-fork blocks (height ≥ 903,845):
Parameter	Value
Proof-of-Work	SHA-256 (double SHA-256)
Target Block Interval	300 seconds
Difficulty Adjustment	ASERT
ASERT Half-life	172,800 seconds (2 days)
Max Block Size (Consensus)	32 MB (32,000,000 bytes)
Halving Interval	210,000 blocks
Maximum Supply (Production Mainnet)	21,000,000 BTCBT
All parameters above are enforced at consensus level.
________________________________________
3. Supply and Emission Structure
3.1 Maximum Supply
The Production mainnet maximum supply is:
21,000,000 BTCBT
Issuance exceeding this cap is considered a consensus violation and results in block invalidation.
3.2 Special Reward
A one-time special reward of 630,000 BTCBT is issued at block height:
903,850
This reward is included within the 21,000,000 BTCBT maximum supply cap.
It is not an inflation extension beyond the cap.
3.3 Block Subsidy Mechanism
Block subsidy:
•	Determined solely by block height
•	Enforced during block validation
•	Halves every 210,000 blocks
Subsidy changes require consensus software modification and network adoption.
________________________________________
4. Difficulty Adjustment (ASERT)
BitcoinBT uses the ASERT difficulty adjustment algorithm applied per block.
Properties:
•	Difficulty is recalculated for every new block.
•	No periodic retarget window.
•	No minimum-difficulty exception rule.
•	Adjustment is continuous and time-deviation-based.
ASERT parameters:
•	Half-life: 172,800 seconds
•	Reference block: fork boundary anchor
Difficulty recalculation cannot be disabled at runtime.
________________________________________
5. Block Structure
BitcoinBT preserves the inherited block structure:
Block Header (80 bytes):
•	nVersion (4 bytes)
•	hashPrevBlock (32 bytes)
•	hashMerkleRoot (32 bytes)
•	nTime (4 bytes)
•	nBits (4 bytes)
•	nNonce (4 bytes)
Block validity condition:
BlockHash ≤ Target
Target is derived from nBits using compact encoding.
Blocks exceeding:
32,000,000 bytes
are rejected at consensus level.
________________________________________
6. Signature and Script Rules
BitcoinBT:
•	Does NOT introduce SIGHASH ForkID.
•	Maintains Bitcoin-compatible signature validation rules.
•	Preserves UTXO model.
•	Preserves script execution model.
Transaction validation logic remains structurally equivalent to inherited rules.
________________________________________
7. Chain Selection Rule
Chain selection is based exclusively on:
Greatest accumulated proof-of-work
No centralized checkpointing logic is applied beyond inherited architecture.
________________________________________
8. Network Architecture
BitcoinBT operates as a permissionless P2P network.
Nodes independently:
•	Validate transactions
•	Validate blocks
•	Propagate valid data
•	Reject consensus-invalid blocks
There is no centralized authority capable of overriding consensus rules.
________________________________________
9. Determinism
For identical input data and identical software version:
Nodes produce identical validation results.
Consensus parameters are defined as constants and height-based conditions in source code.
No runtime modification of core consensus parameters exists.
________________________________________
10. Development Base
•	Codebase origin: Bitcoin Core v26
•	Modification scope:
o	Chain parameters
o	ASERT difficulty logic
o	Block interval target
o	Consensus max block size
o	Height-based branching logic
Transaction verification structure and fundamental validation architecture remain unchanged.
________________________________________
11. Current Phase
The project is currently in controlled validation and implementation verification phase.
Verified internally:
•	Post-fork consensus branching
•	ASERT per-block difficulty adjustment
•	Block reward calculation path
•	Block size cap enforcement
•	Multi-node synchronization
Large-scale external network validation is outside the scope of this document.
________________________________________
12. Scope Limitation
This specification:
•	Does not provide investment advice.
•	Does not provide valuation guidance.
•	Does not provide legal advice.
•	Describes consensus parameters only.
Final authority for consensus rules is the publicly released source code and production mainnet deployment.

