# FlashDB ⚡
A high-performance, thread-safe, in-memory key-value store with LRU eviction and persistence, written in C++.

## 🚀 Features
- **In-Memory Storage**: Sub-millisecond read/write latency using `std::unordered_map`.
- **Thread Safety**: Concurrent client handling using **Fine-Grained Locking** (`std::mutex`).
- **Memory Management**: Custom **LRU (Least Recently Used)** eviction policy to manage memory under load.
- **Persistence**: Snapshot-based durability (`SAVE` command) to survive server restarts.
- **Custom Protocol**: TCP-based communication protocol built on raw Linux sockets.

## 🛠️ Tech Stack
- **Language**: C++ (STL)
- **Networking**: Linux Sockets (TCP/IP)
- **Concurrency**: `std::thread`, `std::mutex`, `std::lock_guard`
- **I/O**: File streams for snapshotting

## 🔧 Architecture
FlashDB uses a **Client-Server architecture**.
1. **Networking Layer**: A multi-threaded TCP server listens on Port 8080. Each client connection spawns a detached thread.
2. **Storage Engine**: Data is stored in a `Hash Map` for O(1) access and a `Doubly Linked List` for O(1) LRU eviction.
3. **Locking Mechanism**: A global mutex ensures thread safety during `SET` and `DEL` operations, preventing race conditions.

## 💻 Usage

### 1. Build
g++ server.cpp -o flashdb -pthread

### 2. RunBash./flashdb
### 3. Connect (Client)Use netcat to connect to the server:Bashnc localhost 8080
### 4. CommandsCommandDescriptionExampleSET <key> <value>Stores a key-value pairSET user:1 ShreyGET <key>Retrieves a valueGET user:1DEL <key>Deletes a keyDEL user:1SAVEPersists data to diskSAVE