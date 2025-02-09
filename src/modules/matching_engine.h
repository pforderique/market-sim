#pragma once
#include <chrono>
#include <queue>
#include "../../datamodel/datamodel.h"

class MatchingEngine 
{
public:
    MatchingEngine();
    ~MatchingEngine();

    datamodel::AddOrderResponse add_order(const datamodel::AddOrderRequest& request);

private:
    enum Status {
        PENDING,
        PARTIALLY_FILLED,
        FILLED,
        CANCELLED
    };

    // Internal order struct
    struct Order : public datamodel::AddOrderResponse {
        int remaining_qty;
        Status status;
    };

    struct CompareAsk {
        bool operator()(const Order& lhs, const Order& rhs) const {
            if (lhs.price == rhs.price) {
                return lhs.timestamp > rhs.timestamp;
            }
            return lhs.price > rhs.price;
        }
    };

    struct CompareBid {
        bool operator()(const Order& lhs, const Order& rhs) const {
            if (lhs.price == rhs.price) {
                return lhs.timestamp > rhs.timestamp;
            }
            return lhs.price < rhs.price;
        }
    };

    // Order book
    std::priority_queue<Order, std::vector<Order>, CompareAsk> ask_orders;
    std::priority_queue<Order, std::vector<Order>, CompareBid> bid_orders;
};