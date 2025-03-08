# pragma once
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

#include "./common.h"
#include "./order_requests.h"


namespace datamodel
{
// OrderStatus is an enum that represents the status of an order.
enum OrderStatus {
    PENDING,
    PARTIALLY_FILLED,
    FILLED,
    CANCELLED,
};

inline std::string to_string(OrderStatus status) {
    switch (status) {
        case PENDING: return "PENDING";
        case PARTIALLY_FILLED: return "PARTIALLY_FILLED";
        case FILLED: return "FILLED";
        case CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}

// Order is a struct that represents an order response.
struct Order : public AddOrderRequest {
    // The remaining quantity of the order
    double remaining_qty;
    // The status of the order
    OrderStatus status;
    // The uuid of the order
    OrderID order_id;
    // The timestamp of when the order was created.
    std::chrono::time_point<std::chrono::system_clock> timestamp;

    // Constructor to create Order from AddOrderRequest
    Order(const AddOrderRequest& request)
        : AddOrderRequest(request), remaining_qty(request.qty), status(PENDING),
          order_id(generate_order_id(request)),
          timestamp(std::chrono::system_clock::now()) {}

    std::string to_string() const {
        return "OrderID: " + order_id + 
               ", SecurityID: " + datamodel::to_string(security_id) +
               ", Side: " + datamodel::to_string(side) +
               ", Price: $" + round_two_places(price) +
               ", Qty: " + round_two_places(qty) +
               ", RemainingQty: " + round_two_places(remaining_qty) +
               ", ClientID: " + client_id +
               ", Status: " + datamodel::to_string(status);
    }

private:
    std::string round_two_places(double num) const {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << num;
        return stream.str();
    }
    datamodel::OrderID generate_order_id(
        const datamodel::AddOrderRequest& request
    ) {
        auto request_attrs = request.client_id + std::to_string(
            std::chrono::system_clock::now().time_since_epoch().count()
        );
        return std::to_string(std::hash<std::string>{}(request_attrs));
    }
};

// TODO: Add ModifyOrderResponse and CancelOrderResponse structs here.
}