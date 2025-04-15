#pragma once
#include <chrono>
#include <functional>
#include <mutex>
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

// Matching Engine implementation with thread safety at the security level
class MatchingEngine : public IMatchingEngine
{
public:
    using OnTransactionCallback = std::function<void(const datamodel::Transaction&)>;

    MatchingEngine(OnTransactionCallback on_transaction = nullptr): on_transaction(on_transaction) {}
    ~MatchingEngine() {}

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

    struct SecurityBook {
        std::priority_queue<datamodel::Order, std::vector<datamodel::Order>, CompareAsk> ask_orders;
        std::priority_queue<datamodel::Order, std::vector<datamodel::Order>, CompareBid> bid_orders;
        std::mutex mtx;
    };

    std::array<SecurityBook, static_cast<size_t>(
        datamodel::SecurityID::LAST + 1
    )> order_books;
    OnTransactionCallback on_transaction;
};