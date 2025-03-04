#include <chrono>
#include "./common.h"
#include "./order_requests.h"

namespace datamodel
{
// AddOrderResponse is a struct that represents an order response.
struct AddOrderResponse : public AddOrderRequest {
    // The order ID of the order response.
    OrderID order_id;
    // The timestamp of the order response.
    std::chrono::time_point<std::chrono::system_clock> timestamp;

    // Constructor to create AddOrderResponse from AddOrderRequest
    AddOrderResponse(const AddOrderRequest& request, OrderID id)
        : AddOrderRequest(request), order_id(id), timestamp(std::chrono::system_clock::now()) {}
};

// TODO: Add ModifyOrderResponse and CancelOrderResponse structs here.
}