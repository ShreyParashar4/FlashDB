# ⚡ FlashDB
High-performance in-memory key-value store built in **C++17** with **LRU eviction**, **multithreading**, and **snapshot persistence**.

![Language](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
![Type](https://img.shields.io/badge/type-In--Memory%20Database-green.svg)
![License](https://img.shields.io/badge/license-MIT-orange.svg)

FlashDB mimics the core of Redis using a custom architecture, raw POSIX sockets, and fine-grained locking. The project demonstrates systems-level C++ programming, networking, and memory-safe design.

---

## 🏗️ Architecture

FlashDB uses a thread-per-client model and a shared LRU cache protected by mutex locks.


---

## 🚀 Features

- **O(1) GET/SET/DEL** using `unordered_map` + doubly-linked list.  
- **Thread-safe** via fine-grained `std::mutex` locking.  
- **LRU eviction** when capacity is reached.  
- **Persistence** using snapshot (`SAVE`) to disk.  
- **Custom TCP protocol** built on raw Linux sockets.  

---

## 📊 Time Complexity

| Operation | Complexity | Description |
|----------|------------|-------------|
| SET      | O(1)       | Insert/update key |
| GET      | O(1)       | Fetch + refresh LRU |
| DEL      | O(1)       | Remove key |
| Evict    | O(1)       | Remove LRU key |

---

## 🛠️ Tech Stack

- **Language:** C++17  
- **Sockets:** `<sys/socket.h>`, `<netinet/in.h>`  
- **Concurrency:** `std::thread`, `std::mutex`  
- **Storage:** `std::fstream`  

---

## 💻 Build & Run

### Build
```bash
g++ server.cpp -o flashdb -pthread
```

### Run
```bash
./flashdb
```


### Connect (Client)
```bash
nc localhost 8080
```

📜 Commands
Command	Description	Example
SET <key> <value>	Store or update. Evicts LRU if full.	SET name Shrey
GET <key>	Retrieve and update LRU position.	GET name
DEL <key>	Delete key.	DEL name
SAVE	Write snapshot to flashdb.data.	SAVE