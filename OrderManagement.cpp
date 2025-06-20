#include "OrderManagement.h"
#include <thread>

OrderManagement::OrderManagement(int sH, int sM, int eH, int eM, int maxPerSec)
    : startHour(sH), startMinute(sM), endHour(eH), endMinute(eM), maxOrdersPerSecond(maxPerSec) {
    currentSecondStart = std::chrono::steady_clock::now();
}

bool OrderManagement::isWithinTimeWindow() {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    int currentMinutes = now->tm_hour * 60 + now->tm_min;
    int startMinutes = startHour * 60 + startMinute;
    int endMinutes = endHour * 60 + endMinute;
    return currentMinutes >= startMinutes && currentMinutes < endMinutes;
}

void OrderManagement::send(const OrderRequest& request) {
    std::cout << "Sent OrderID: " << request.m_orderId << std::endl;
    orderSentTimes[request.m_orderId] = std::chrono::steady_clock::now();
    sentCountThisSecond++;
}

void OrderManagement::sendLogon() {
    std::cout << "Logon Sent" << std::endl;
    isLoggedOn = true;
}

void OrderManagement::sendLogout() {
    std::cout << "Logout Sent" << std::endl;
    isLoggedOn = false;
}

void OrderManagement::onData(OrderRequest&& request) {
    std::lock_guard<std::mutex> lock(mtx);

    if (!isWithinTimeWindow()) {
        std::cout << "OrderID: " << request.m_orderId << " rejected (outside trading window).\n";
        return;
    }

    if (!isLoggedOn) sendLogon();

    auto now = std::chrono::steady_clock::now();
    if (now - currentSecondStart >= std::chrono::seconds(1)) {
        currentSecondStart = now;
        sentCountThisSecond = 0;
    }

    switch (request.m_requestType) {
        case RequestType::New:
            if (sentCountThisSecond < maxOrdersPerSecond) {
                send(request);
            } else {
                orderQueue.push(request.m_orderId);
                pendingOrders[request.m_orderId] = request;
            }
            break;

        case RequestType::Modify:
            if (pendingOrders.find(request.m_orderId) != pendingOrders.end()) {
                pendingOrders[request.m_orderId].m_price = request.m_price;
                pendingOrders[request.m_orderId].m_qty = request.m_qty;
                std::cout << "Modified OrderID: " << request.m_orderId << std::endl;
            }
            break;

        case RequestType::Cancel:
            if (pendingOrders.find(request.m_orderId) != pendingOrders.end()) {
                pendingOrders.erase(request.m_orderId);
                std::cout << "Cancelled OrderID: " << request.m_orderId << std::endl;
            }
            break;

        default:
            std::cout << "Unknown request type for OrderID: " << request.m_orderId << std::endl;
            break;
    }
}

void OrderManagement::tick() {
    std::lock_guard<std::mutex> lock(mtx);

    auto now = std::chrono::steady_clock::now();
    if (now - currentSecondStart >= std::chrono::seconds(1)) {
        currentSecondStart = now;
        sentCountThisSecond = 0;
    }

    while (!orderQueue.empty() && sentCountThisSecond < maxOrdersPerSecond) {
        uint64_t orderId = orderQueue.front();
        orderQueue.pop();
        if (pendingOrders.find(orderId) != pendingOrders.end()) {
            send(pendingOrders[orderId]);
            pendingOrders.erase(orderId);
        }
    }

    if (!isWithinTimeWindow() && isLoggedOn) {
        sendLogout();
    }
}

void OrderManagement::onData(OrderResponse&& response) {
    std::lock_guard<std::mutex> lock(mtx);

    auto now = std::chrono::steady_clock::now();
    if (orderSentTimes.find(response.m_orderId) != orderSentTimes.end()) {
        auto sentTime = orderSentTimes[response.m_orderId];
        auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(now - sentTime).count();

        std::ofstream log("latency_log.txt", std::ios::app);
        log << "OrderID: " << response.m_orderId
            << ", Response: " << responseTypeToStr(response.m_responseType)
            << ", Latency(ms): " << latency << "\n";

        orderSentTimes.erase(response.m_orderId);
    }
}

std::string OrderManagement::responseTypeToStr(ResponseType type) {
    switch (type) {
        case ResponseType::Accept: return "ACCEPT";
        case ResponseType::Reject: return "REJECT";
        default: return "UNKNOWN";
    }
}