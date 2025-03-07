#include "./matching_engine.h"


std::vector<datamodel::Transaction> MatchingEngine::match_order(
    datamodel::Order& order
) {
    // TODO: Current implementation IGNORES security_id. How do we include this
    // and parallelize this? perhaps an "order book" per security_id and have a
    // lock per security_id?
    std::vector<datamodel::Transaction> transactions;

    if (order.side == datamodel::Side::BID) {
        while (!this->ask_orders.empty() && order.price >= this->ask_orders.top().price) {
            datamodel::Order ask_order = this->ask_orders.top();
            this->ask_orders.pop();

            // price favors new order - so the min of the new bid and the ask
            double fulfilled_price = std::min(ask_order.price, order.price);
            double fulfilled_qty = std::min(ask_order.remaining_qty, order.remaining_qty);

            // Record transaction and broadcast
            datamodel::Transaction t(order.security_id, fulfilled_price, fulfilled_qty, order.client_id, ask_order.client_id);
            this->on_transaction(t);
            transactions.push_back(t);

            // update order qtys
            order.remaining_qty -= fulfilled_qty;
            ask_order.remaining_qty -= fulfilled_qty;

            // Update ask_order status
            if (ask_order.remaining_qty == 0) {
                ask_order.status = datamodel::OrderStatus::FILLED;
            } else {
                ask_order.status = datamodel::OrderStatus::PARTIALLY_FILLED;
                this->ask_orders.push(ask_order);
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
            this->bid_orders.push(order);
        }
    }

    // TODO: Implement the ASK side of the matching engine.
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