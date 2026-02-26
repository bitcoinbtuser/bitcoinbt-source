Consensus Structure Summary
BitcoinBT (BTCBT)
________________________________________
1. Network Overview
BitcoinBT (BTCBT) is a consensus fork chain that inherits the Bitcoin blockchain in full up to block height 903,844. Adjusted consensus conditions apply to blocks after the fork point.
•	Inherited range: Blocks 0 – 903,844
•	Consensus activation height: 903,845
•	Proof-of-Work: SHA-256 (double SHA-256)
•	Target block interval: 300 seconds
•	Halving interval: 210,000 blocks
•	Maximum supply cap: 21,000,000 BTCBT
Pre-fork blocks follow inherited validation rules, while post-fork blocks are validated under the updated consensus conditions.
________________________________________
2. Consensus Structure
BitcoinBT applies consensus transition strictly based on block height.
No signaling mechanism or external approval process is used.
The primary consensus conditions applied after the fork include:
•	Per-block difficulty adjustment
•	32 MB consensus maximum block size
•	300-second target block interval
•	Accumulated-work-based chain selection
All nodes validate blocks under identical consensus rules. Chain selection is determined by the chain with the greatest accumulated Proof-of-Work.
________________________________________
3. Emission Model
The total supply cap on the Production Mainnet is defined as 21,000,000 BTCBT.
A special reward of 630,000 BTCBT is issued once at block height 903,850. This amount is included within the total supply cap.
Block subsidies are calculated based on block height and are reduced by half every 210,000 blocks. Under this structure, total supply increases progressively and converges toward the defined maximum.
________________________________________
4. Network Operation Model
BitcoinBT operates as an open peer-to-peer (P2P) network.
Nodes independently validate blocks and transactions, propagate valid data, and maintain synchronized chain state.
Block production occurs through competitive Proof-of-Work hashing. Valid blocks are connected to the chain after full network validation.
________________________________________
5. Document Scope
This document provides a summary overview of BitcoinBT’s consensus structure and network model for whitepaper submission purposes.
It does not constitute investment solicitation, financial guarantee, price projection, or legal advice. The authoritative reference for consensus rules is the publicly released software executed on the Production Mainnet.

