#include "OrderManagement.h"
#include <thread>
#include <chrono>

int main() {
    OrderManagement oms(10, 0, 13, 0, 3); 

    
    oms.onData(OrderRequest{1, 100.0, 10, 'B', 101, RequestType::New});
    oms.onData(OrderRequest{2, 200.0, 20, 'S', 102, RequestType::New});
    oms.onData(OrderRequest{3, 300.0, 30, 'B', 103, RequestType::New});
    oms.onData(OrderRequest{4, 400.0, 40, 'S', 104, RequestType::New});

   
    oms.onData(OrderRequest{4, 450.0, 50, 'S', 104, RequestType::Modify});

 
    oms.onData(OrderRequest{103, 0.0, 0, 'B', 103, RequestType::Cancel});

    
    for (int i = 0; i < 5; ++i) {
        oms.tick();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    
    oms.onData(OrderResponse{101, ResponseType::Accept});
    oms.onData(OrderResponse{102, ResponseType::Reject});
    oms.onData(OrderResponse{104, ResponseType::Accept});

    return 0;
}