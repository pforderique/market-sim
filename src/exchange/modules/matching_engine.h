#pragma once
#include <chrono>
#include <functional>
#include <queue>
#include "../../../datamodel/datamodel.h"

struct IMatchingEngine
{
public:
    virtual datamodel::AddOrderResponse add_order(const datamodel::AddOrderRequest& request) = 0;
    virtual ~IMatchingEngine() {};
};

class MatchingEngine : public IMatchingEngine
{
public:
    using OnTransactionCallback = std::function<void(const datamodel::Transaction&)>;

    MatchingEngine(OnTransactionCallback on_transaction = nullptr): on_transaction(on_transaction) {
        printf("MatchingEngine created.\n");
    }
    ~MatchingEngine() {
        printf("MatchingEngine destroyed.\n");
    }

    datamodel::AddOrderResponse add_order(const datamodel::AddOrderRequest& request) override;

private:
    OnTransactionCallback on_transaction;

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

        Order(const datamodel::AddOrderResponse& response)
            : datamodel::AddOrderResponse(response), remaining_qty(response.qty), status(PENDING) {}
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