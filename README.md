# Distributed Hash Table with BTree Storage

## Overview

This project implements a primitive InterPlanetary File System (IPFS) using a Ring Distributed Hash Table (DHT) and B-trees for storing data. It demonstrates concepts of distributed systems, hashing, and efficient data storage and retrieval.

## Features

- **Ring DHT**: Implements a circular DHT where nodes are organized in a ring structure.
- **Finger Tables**: Each node maintains a finger table to quickly locate other nodes in the DHT.
- **BTree Storage**: Data is stored in B-trees associated with each node, enabling efficient storage and retrieval.
- **SHA-1 Hashing**: Uses SHA-1 hashing to generate unique IDs for files and nodes.
- **Quadratic Probing**: Handles collisions using quadratic probing for ID assignment.
- **Routing**: Efficient routing and lookup using the finger table.
