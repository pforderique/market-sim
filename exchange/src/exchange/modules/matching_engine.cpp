#include "./matching_engine.h"


std::vector<datamodel::Transaction> MatchingEngine::match_order(
    datamodel::Order& order
) {
    // Acquire lock for the security's order book.
    auto securityIdx = static_cast<size_t>(order.security_id);
    auto &securityBook = this->order_books[securityIdx];
    std::lock_guard<std::mutex> lock(securityBook.mtx);

    // Transactions to return
    std::vector<datamodel::Transaction> transactions;

    if (order.side == datamodel::Side::BID) {
        while (
            !securityBook.ask_orders.empty() && 
            order.price >= securityBook.ask_orders.top().price
        ) {
            datamodel::Order ask_order = securityBook.ask_orders.top();
            securityBook.ask_orders.pop();

            // price favors new order - so the min of the new bid and the ask
            double fulfilled_price = std::min(ask_order.price, order.price);
            double fulfilled_qty = std::min(
                ask_order.remaining_qty, order.remaining_qty
            );

            // Record transaction and broadcast
            datamodel::Transaction t(
                order.security_id,
                fulfilled_price,
                fulfilled_qty,
                order.client_id,
                ask_order.client_id
            );
            if (this->on_transaction != nullptr) {
                this->on_transaction(t);
            }
            transactions.push_back(t);

            // update order qtys
            order.remaining_qty -= fulfilled_qty;
            ask_order.remaining_qty -= fulfilled_qty;

            // Update ask_order status
            if (ask_order.remaining_qty == 0) {
                ask_order.status = datamodel::OrderStatus::FILLED;
            } else {
                ask_order.status = datamodel::OrderStatus::PARTIALLY_FILLED;
                securityBook.ask_orders.push(ask_order);
            }

            // Update bid order status
            if (order.remaining_qty == 0) {
                order.status = datamodel::OrderStatus::FILLED;
                break;
            } else {
                order.status = datamodel::OrderStatus::PARTIALLY_FILLED;
            }
        }

        // After all matching, check if order needs to be added to queue.
        if (order.remaining_qty > 0) {
            securityBook.bid_orders.push(order);
        }
    } else if (order.side == datamodel::Side::ASK) {
        while (
            !securityBook.bid_orders.empty() &&
            order.price <= securityBook.bid_orders.top().price
        ) {
            datamodel::Order bid_order = securityBook.bid_orders.top();
            securityBook.bid_orders.pop();

            // price favors new order - so the max of the new ask and the bid
            double fulfilled_price = std::max(bid_order.price, order.price);
            double fulfilled_qty = std::min(
                bid_order.remaining_qty, order.remaining_qty
            );

            // Record transaction and broadcast
            datamodel::Transaction t(
                order.security_id,
                fulfilled_price,
                fulfilled_qty,
                bid_order.client_id,
                order.client_id
            );

            if (this->on_transaction != nullptr) {
                this->on_transaction(t);
            }
            transactions.push_back(t);

            // update order qtys
            order.remaining_qty -= fulfilled_qty;
            bid_order.remaining_qty -= fulfilled_qty;

            // update bid_order status
            if (bid_order.remaining_qty == 0) {
                bid_order.status = datamodel::OrderStatus::FILLED;
            } else {
                bid_order.status = datamodel::OrderStatus::PARTIALLY_FILLED;
                securityBook.bid_orders.push(bid_order);
            }

            // update ask order status
            if (order.remaining_qty == 0) {
                order.status = datamodel::OrderStatus::FILLED;
                break;
            } else {
                order.status = datamodel::OrderStatus::PARTIALLY_FILLED;
            }
        }

        // After all matching, check if order needs to be added to queue.
        if (order.remaining_qty > 0) {
            securityBook.ask_orders.push(order);
        }
    }
    return transactions;
}

datamodel::AddOrderResponse MatchingEngine::add_order(
    const datamodel::AddOrderRequest& request
) {
    // Create order response
    datamodel::AddOrderResponse response(request);

    // Match the order, if possible
    datamodel::Order order(response);

    // TODO: Make this thread safe, and kick off in a non-blocking manner.
    this->match_order(order);

    return response;
}