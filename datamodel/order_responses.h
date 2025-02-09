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
};

// TODO: Add ModifyOrderResponse and CancelOrderResponse structs here.
}