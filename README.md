
# ⚡ VoltKV

A high-performance, crash-resilient, transactional in-memory key-value storage engine built from scratch using clean, modular C++17. 

VoltKV mimics production-grade cache data stores like Redis, achieving predictable **$O(1)$ time complexity** for foundational CRUD operations. It implements atomic multi-command transaction flows, hardware-bound expiration policies, and a local cold-storage persistence pipeline.

---

## 🏗️ System Architecture & Modularity

VoltKV enforces a strict **Separation of Concerns (SoC)** design pattern, keeping data representation, parsing mechanics, and storage execution decoupled across dedicated interfaces.

```text
VoltKV/
├── src/
│   ├── parser/      # Lexical Analysis, Tokenization & Whitespace Sanitation
│   │   ├── parser.h
│   │   └── parser.cpp
│   ├── engines/     # Hash Matrices, Monotonic Clocks & File Streams
│   │   ├── engine.h
│   │   └── engine.cpp
│   └── main.cpp     # Controller Runtime Loop & Interactive Shell UI
├── Makefile         # Incremental Dependency Tracker & Compiler Automation
└── README.md        # System Documentation

```

---

## 🚀 Advanced Engineering Implementations

### 1. High-Precision Passive TTL Eviction ("Lazy Deletion")

Instead of employing expensive background thread sweeps that introduce thread contention, race conditions, and CPU thrashing, memory recovery is handled **passively on read/write boundaries**.
Keys with expiration values are timestamped with deadlines relative to `std::chrono::high_resolution_clock`. When an access command hits the key, a runtime assertion evaluates:

$$\text{currentTime} > \text{expiryTime}$$

If crossed, the entry is instantaneously evicted from the heap map on the fly before returning a safe `(nil)` response, preserving performance paths.

### 2. Isolated Transactional Atomicity (ACID-lite)

VoltKV ensures multi-operation sequence integrity via an encapsulated command-queuing engine (`MULTI`, `EXEC`, `DISCARD`). When a transaction block is initiated, incoming string sequences are bypassed into a 2D vector queue matrix rather than hitting RAM storage. Upon invoking `EXEC`, the engine processes the queue sequentially in a single tight block, eliminating partial-state data fragmentation.

### 3. Crash-Resilient Cold Storage Pipeline

To transition volatile memory arrays into persistent local storage, VoltKV operates a flat-file serialization system using custom pipe-delimiters (`|`) via `std::fstream`. The engine automatically scans for a local `dump.kv` state machine binary at startup to auto-hydrate and reconstitute data states across hardware reboot boundaries.

---

## 💻 Supported Protocol Commands

| Target Command | Syntax Examples | Return Typing / Outputs | Purpose & Internal Mechanics |
| --- | --- | --- | --- |
| **SET** | `SET user "Alice"` | `OK` | Inserts/Updates a key into the $O(1)$ map. Preserves quoted whitespace payloads. |
| **GET** | `GET user` | `"Alice"` or `(nil)` | Evaluates existence and lifetime before pulling memory addresses. |
| **DEL** | `DEL user` | `OK` | Unmaps data and instantly frees underlying heap storage blocks. |
| **EXISTS** | `EXISTS user` | `(integer) 1` or `(integer) 0` | Intercepts keyspace to affirm presence without executing dynamic data loads. |
| **KEYS** | `KEYS` | `1) "user"` | Traverses the hash space, filtering out expired metadata to map active rosters. |
| **EXPIRE** | `EXPIRE user 60` | `OK` | Enforces a monotonic timeout constraint (in seconds) to transient entries. |
| **TTL** | `TTL user` | `(integer) 54` or `(integer) -1` | Outputs exact seconds remaining before lazy deletion triggers eviction. |
| **SAVE** | `SAVE` | `OK` | Flushes active internal runtime records out to persistent disk layers. |
| **MULTI** | `MULTI` | `OK` | Locks interface routing blocks and initializes a separate execution queue. |
| **EXEC** | `EXEC` | *Multi-line Array* | Commits and sequentially fires queued payloads; returns all tracking feedback. |
| **DISCARD** | `DISCARD` | `OK` | Clears all pending vector queues entirely, reverting state flags cleanly to normal. |

---

## 🛠️ Compilation & Quick Start

### Prerequisites

* A modern C++ compiler supporting C++17 features (`g++` / MinGW).
* `make` system utility script engine installed.

### Build Sequence

1. Clone the project and step into the workspace root directory:
```bash
git clone https://github.com/nisha-bugalia/voltkv
cd VoltKV

```


2. Fire the incremental compilation automated build workflow:
```bash
mingw32-make

```


3. Boot up the live transactional interactive console tool:
```bash
.\voltkv.exe

```



---

## 📊 Sample Interactive Runtime Trace

```text
Welcome to the VoltKV In-Memory-Database engine [Version 1.0]
Loading data from disk...
Successfully restored 2 keys from disk.

VoltKV> SET tracking_id "VT-9901"
OK
VoltKV> EXPIRE tracking_id 30
OK
VoltKV> TTL tracking_id
(integer) 27
VoltKV> MULTI
OK
VoltKV> SET status "Committed"
QUEUED
VoltKV> GET status
QUEUED
VoltKV> EXEC
1) OK
2) "Committed"
VoltKV> SAVE
OK
VoltKV> EXIT
Shutting down VoltKV... Goodbye!

```
