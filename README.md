# Order Management System – Assignment

## 👤 Author
**Jaswanth Chilakalapudi**

## 📌 Overview

This project implements a basic **Order Management System** that:
- Accepts order requests from upstream systems.
- Sends orders to exchange within a configurable time window.
- Enforces throttling (X orders/sec).
- Queues excess orders for future sending.
- Supports Modify/Cancel operations on queued orders.
- Logs response latency from the exchange.

The system is implemented in **C++** without using any third-party libraries.

---

##  Assumptions

1. The **upstream system** and **exchange communication** (TCP/shared memory) is abstracted away. We simulate order flow via function calls.
2. The **system clock is assumed to be set to IST**, and logon/logoff are based on system local time.
3. Only orders that are **still in the queue** can be modified or canceled.
4. Modify only updates `price` and `qty`, and retains the original position in the queue.
5. If Modify/Cancel is received for an order not in queue (already sent), it is ignored.
6. The `tick()` function simulates time-based flushing every second.
7. The system is expected to run 24x7, but sends orders only within the configured window.

---

##  Design Decisions

- Used `std::queue<uint64_t>` to maintain **FIFO ordering** of queued order IDs.
- Used `std::unordered_map<uint64_t, OrderRequest>` for **O(1)** lookup for Modify/Cancel logic.
- Thread safety is ensured using `std::mutex`.
- The latency log is written to `latency_log.txt` in append mode.
- The system uses a **manual tick** to simulate 1-second intervals for sending queued orders.

---

##  Components

- `OrderManagement.h`: Class declaration and core data structures.
- `OrderManagement.cpp`: Core logic of the order management engine.
- `main.cpp`: Test driver to simulate sample order flow.

---

##  How to Run

```bash

g++ -std=c++17 OrderManagement.cpp main.cpp -o order_mgmt
```
```bash
./order_mgmt
```

