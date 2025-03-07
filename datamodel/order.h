# pragma once
#include <chrono>
#include "./common.h"
#include "./order_requests.h"


// datamodel::OrderID generate_order_id(const datamodel::AddOrderRequest& request) {
//     auto request_attrs =
//         std::to_string(request.security_id) + 
//         std::to_string(request.side) +
//         std::to_string(request.price) +
//         std::to_string(request.qty) +
//         request.client_id +
//         std::to_string(
//             std::chrono::system_clock::now().time_since_epoch().count()
//         );
//     return std::to_string(std::hash<std::string>{}(request_attrs));
// }


namespace datamodel
{
// OrderStatus is an enum that represents the status of an order.
enum OrderStatus {
    PENDING,
    PARTIALLY_FILLED,
    FILLED,
    CANCELLED,
};

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
          order_id("some_id"),
          timestamp(std::chrono::system_clock::now()) {}
};

// TODO: Add ModifyOrderResponse and CancelOrderResponse structs here.
}