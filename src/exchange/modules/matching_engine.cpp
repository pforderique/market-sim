#include "./matching_engine.h"

std::string generate_order_id(const datamodel::AddOrderRequest& request) {
    auto request_attrs = std::to_string(request.security_id) + std::to_string(request.side) +
                  std::to_string(request.price) + std::to_string(request.qty) + request.client_id;
    return std::to_string(std::hash<std::string>{}(request_attrs));
}

datamodel::AddOrderResponse MatchingEngine::add_order(const datamodel::AddOrderRequest& request) {
    // Create order response
    datamodel::AddOrderResponse response(request, generate_order_id(request));

    // Add order to the order book
    Order order(response);


    return response;
}