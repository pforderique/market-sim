#pragma once
#include <chrono>
#include <functional>
#include <queue>
#include "../../../datamodel/datamodel.h"


// Interface for the Matching Engine
struct IMatchingEngine
{
public:
    // Add an order to the matching engine
    virtual datamodel::AddOrderResponse add_order(const datamodel::AddOrderRequest& request) = 0;
    // Match an order with the existing orders in the order book
    virtual std::vector<datamodel::Transaction> match_order(datamodel::Order& order) = 0;
    virtual ~IMatchingEngine() {};
};

// Matching Engine implementation
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
    std::vector<datamodel::Transaction> match_order(datamodel::Order& order) override;

private:

    struct CompareAsk {
        bool operator()(const datamodel::Order& lhs, const datamodel::Order& rhs) const {
            if (lhs.price == rhs.price) {
                return lhs.timestamp > rhs.timestamp;
            }
            return lhs.price > rhs.price;
        }
    };

    struct CompareBid {
        bool operator()(const datamodel::Order& lhs, const datamodel::Order& rhs) const {
            if (lhs.price == rhs.price) {
                return lhs.timestamp > rhs.timestamp;
            }
            return lhs.price < rhs.price;
        }
    };

    OnTransactionCallback on_transaction;

    // Order book
    std::priority_queue<datamodel::Order, std::vector<datamodel::Order>, CompareAsk> ask_orders;
    std::priority_queue<datamodel::Order, std::vector<datamodel::Order>, CompareBid> bid_orders;
};