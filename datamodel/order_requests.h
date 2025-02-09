// File: order_requests.cpp
#pragma once
#include "./common.h"

namespace datamodel
{
// AddOrderRequest is a struct that represents an order request.
struct AddOrderRequest {
    // The security ID of the order request.
    SecurityID security_id;
    // The side of the order request.
    Side side;
    // The price of the order request.
    double price;
    // The quantity of the order request.
    double qty;
    // The client ID of the order request.
    ClientID client_id;
};

// TODO: Add ModifyOrderRequest and CancelOrderRequest structs here.
}