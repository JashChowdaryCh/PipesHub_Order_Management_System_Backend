#pragma once
#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <chrono>
#include <ctime>
#include <string>
#include <mutex>

struct Logon {
    std::string username;
    std::string password;
};

struct Logout {
    std::string username;
};

enum class RequestType {
    Unknown = 0,
    New = 1,
    Modify = 2,
    Cancel = 3
};

enum class ResponseType {
    Unknown = 0,
    Accept = 1,
    Reject = 2
};

struct OrderRequest {
    int m_symbolId;
    double m_price;
    uint64_t m_qty;
    char m_side; // 'B' or 'S'
    uint64_t m_orderId;
    RequestType m_requestType = RequestType::New;
};

struct OrderResponse {
    uint64_t m_orderId;
    ResponseType m_responseType;
};

class OrderManagement {
public:
    OrderManagement(int sH, int sM, int eH, int eM, int maxPerSec);

    void onData(OrderRequest&& request);
    void onData(OrderResponse&& response);

    void tick(); 

private:
    void send(const OrderRequest& request);
    void sendLogon();
    void sendLogout();
    bool isWithinTimeWindow();
    std::string responseTypeToStr(ResponseType type);

    int startHour, startMinute;
    int endHour, endMinute;
    int maxOrdersPerSecond;

    std::queue<uint64_t> orderQueue;
    std::unordered_map<uint64_t, OrderRequest> pendingOrders;
    std::unordered_map<uint64_t, std::chrono::steady_clock::time_point> orderSentTimes;

    std::chrono::steady_clock::time_point currentSecondStart;
    int sentCountThisSecond = 0;
    bool isLoggedOn = false;

    std::mutex mtx; 
};