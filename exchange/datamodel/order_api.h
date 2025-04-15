#pragma once
#include <chrono>
#include <string>
#include "./common.h"

namespace datamodel
{
// OrderStatus is an enum that represents the status of an order.
enum OrderStatus {
    PENDING,
    PARTIALLY_FILLED,
    FILLED,
    CANCELLED,
};
std::string to_string(OrderStatus status);
OrderStatus order_status_from_string(const std::string& status);

// AddOrderRequest is a struct that represents an order request.
// Wire format: side|security_id|price|qty|client_id
// Example: "BID|AAPL|150.25|100|fabrizzio"
struct AddOrderRequest {
    // The side of the order request.
    Side side;
    // The security ID of the order request.
    SecurityID security_id;
    // The price of the order request.
    double price;
    // The quantity of the order request.
    double qty;
    // The client ID of the order request.
    ClientID client_id;

    std::string to_string() const;
    static AddOrderRequest from_string(const std::string& request);
};

// Order is a struct that represents an order.
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
    Order() {}
    Order(const AddOrderRequest& request)
        : AddOrderRequest(request),
          remaining_qty(request.qty), status(PENDING), 
          order_id(generate_order_id(request)),
          timestamp(std::chrono::system_clock::now()),
          from_request(request) {}

    std::string to_string() const;
    static Order from_string(const std::string& order);

private:
    AddOrderRequest from_request = {};
    static OrderID generate_order_id(const AddOrderRequest& request);
};


// AddOrderResponse is a struct that represents an order response.
using AddOrderResponse = Order;


// TODO: Add ModifyOrderRequest and CancelOrderRequest structs here.
// TODO: Add ModifyOrderResponse and CancelOrderResponse structs here.
}