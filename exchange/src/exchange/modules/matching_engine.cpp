#include "./matching_engine.h"

const std::string DB_PATH = "data/orders.db";

MatchingEngine::MatchingEngine(OnTransactionCallback on_transaction, bool use_db)
    : on_transaction(on_transaction) {
    if (use_db) {
        this->order_db = std::make_unique<storage::OrderDB>(DB_PATH);
        if (!this->order_db->init()) {
            throw std::runtime_error("Failed to initialize order database.");
        }
        this->load_unfilled_orders();
    }
}

MatchingEngine::~MatchingEngine() {
    if (this->order_db != nullptr) {
        this->save_unfilled_orders();
    }
}

std::vector<datamodel::Transaction> MatchingEngine::match_order(
    datamodel::Order &order)
{
    // Acquire lock for the security's order book.
    auto securityIdx = static_cast<size_t>(order.security_id);
    auto &securityBook = this->order_books[securityIdx];
    std::lock_guard<std::mutex> lock(securityBook.mtx);

    // Transactions to return
    std::vector<datamodel::Transaction> transactions;

    if (order.side == datamodel::Side::BID)
    {
        while (
            !securityBook.ask_orders.empty() &&
            order.price >= securityBook.ask_orders.top().price)
        {
            datamodel::Order ask_order = securityBook.ask_orders.top();
            securityBook.ask_orders.pop();

            // price favors new order - so the min of the new bid and the ask
            double fulfilled_price = std::min(ask_order.price, order.price);
            double fulfilled_qty = std::min(
                ask_order.remaining_qty, order.remaining_qty);

            // Record transaction and broadcast
            datamodel::Transaction t(
                order.security_id,
                fulfilled_price,
                fulfilled_qty,
                order.client_id,
                ask_order.client_id);
            if (this->on_transaction != nullptr)
            {
                this->on_transaction(t);
            }
            transactions.push_back(t);

            // update order qtys
            order.remaining_qty -= fulfilled_qty;
            ask_order.remaining_qty -= fulfilled_qty;

            // Update ask_order status
            if (ask_order.remaining_qty == 0)
            {
                ask_order.status = datamodel::OrderStatus::FILLED;
            }
            else
            {
                ask_order.status = datamodel::OrderStatus::PARTIALLY_FILLED;
                securityBook.ask_orders.push(ask_order);
            }

            // Update bid order status
            if (order.remaining_qty == 0)
            {
                order.status = datamodel::OrderStatus::FILLED;
                break;
            }
            else
            {
                order.status = datamodel::OrderStatus::PARTIALLY_FILLED;
            }
        }

        // After all matching, check if order needs to be added to queue.
        if (order.remaining_qty > 0)
        {
            securityBook.bid_orders.push(order);
        }
    }
    else if (order.side == datamodel::Side::ASK)
    {
        while (
            !securityBook.bid_orders.empty() &&
            order.price <= securityBook.bid_orders.top().price)
        {
            datamodel::Order bid_order = securityBook.bid_orders.top();
            securityBook.bid_orders.pop();

            // price favors new order - so the max of the new ask and the bid
            double fulfilled_price = std::max(bid_order.price, order.price);
            double fulfilled_qty = std::min(
                bid_order.remaining_qty, order.remaining_qty);

            // Record transaction and broadcast
            datamodel::Transaction t(
                order.security_id,
                fulfilled_price,
                fulfilled_qty,
                bid_order.client_id,
                order.client_id);

            if (this->on_transaction != nullptr)
            {
                this->on_transaction(t);
            }
            transactions.push_back(t);

            // update order qtys
            order.remaining_qty -= fulfilled_qty;
            bid_order.remaining_qty -= fulfilled_qty;

            // update bid_order status
            if (bid_order.remaining_qty == 0)
            {
                bid_order.status = datamodel::OrderStatus::FILLED;
            }
            else
            {
                bid_order.status = datamodel::OrderStatus::PARTIALLY_FILLED;
                securityBook.bid_orders.push(bid_order);
            }

            // update ask order status
            if (order.remaining_qty == 0)
            {
                order.status = datamodel::OrderStatus::FILLED;
                break;
            }
            else
            {
                order.status = datamodel::OrderStatus::PARTIALLY_FILLED;
            }
        }

        // After all matching, check if order needs to be added to queue.
        if (order.remaining_qty > 0)
        {
            securityBook.ask_orders.push(order);
        }
    }
    return transactions;
}

datamodel::AddOrderResponse MatchingEngine::add_order(
    const datamodel::AddOrderRequest &request)
{
    // Create order response
    datamodel::AddOrderResponse response(request);

    // Match the order, if possible
    datamodel::Order order(response);

    // TODO: Make this thread safe, and kick off in a non-blocking manner.
    this->match_order(order);

    return response;
}

void MatchingEngine::save_unfilled_orders() {
    std::vector<datamodel::Order> orders_to_save;
    for (size_t i = 0; i <= static_cast<size_t>(datamodel::SecurityID::LAST); ++i) {
        SecurityBook& book = order_books[i];
        std::lock_guard<std::mutex> lock(book.mtx);

        // Save all remaining ask orders
        while (!book.ask_orders.empty()) {
            datamodel::Order order = book.ask_orders.top();
            book.ask_orders.pop();
            orders_to_save.push_back(order);
        }
        // Save all remaining bid orders
        while (!book.bid_orders.empty()) {
            datamodel::Order order = book.bid_orders.top();
            book.bid_orders.pop();
            orders_to_save.push_back(order);
        }
    }
    printf("Saving %zu unfilled orders to database...\n", orders_to_save.size());
    this->order_db->save_orders(orders_to_save);
}

void MatchingEngine::load_unfilled_orders() {
    printf("Loading unfilled orders from database...\n");

    std::vector<datamodel::Order> orders = this->order_db->load_orders();

    for (auto& order : orders) {
        size_t idx = static_cast<size_t>(order.security_id);
        auto& book = order_books[idx];
        std::lock_guard<std::mutex> lock(book.mtx);

        if (order.side == datamodel::Side::BID) {
            book.bid_orders.push(order);
        } else {
            book.ask_orders.push(order);
        }
    }

    printf("%zu orders loaded into order book.\n", orders.size());
}
