# Order Management System – Assignment

## 👤 Author
**Jaswanth Chilakalapudi**

---

## 📌 Assumptions Made
- The order window time is in **IST (local system time)**.
- Incoming orders are received through an already-implemented upstream system.
- Only `tick()` is expected to be called once every second externally.
- Order queue is thread-safe via `std::mutex`.
- No external libraries or frameworks are used.
- Order responses always arrive after the order has been sent.
- Order `Modify` and `Cancel` are only applicable to queued (not yet sent) orders.

---

## 🧠 Design Decisions

### ✅ Time Window Handling
- Logon is sent automatically at the start of the trading window.
- Logout is sent at the end of the window.
- All orders outside this window are rejected.

### ✅ Order Throttling
- Only `N` orders per second are allowed to be sent (`maxOrdersPerSecond`).
- Additional orders are queued using FIFO order.
- `tick()` checks and sends pending orders every second.

### ✅ Modify & Cancel Handling
- If a `Modify` request is received for an existing queued order, it updates the price and quantity.
- If a `Cancel` request is received, the order is removed from the queue.

### ✅ Latency Logging
- Each order’s round-trip latency is calculated in milliseconds and appended to `latency_log.txt`.

---

## 🔧 How to Compile & Run

### Compilation (Linux/macOS/Windows with g++):
```bash
g++ -std=c++17 main.cpp OrderManagement.cpp -o order_mgmt
```

```bash
./order_mgmt
```
