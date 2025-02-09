#include "./matching_engine.h"
#include "../../datamodel/datamodel.h"

MatchingEngine::MatchingEngine() {
    printf("MatchingEngine created.\n");
}

MatchingEngine::~MatchingEngine() {
    printf("MatchingEngine destroyed.\n");
}

datamodel::AddOrderResponse MatchingEngine::add_order(const datamodel::AddOrderRequest& request) {
    // Create order response
    datamodel::AddOrderResponse response;
    response.security_id = request.security_id;
    response.side = request.side;
    response.price = request.price;
    response.qty = request.qty;
    response.client_id = request.client_id;
    response.timestamp = std::chrono::system_clock::now();
    response.order_id = std::to_string(response.timestamp.time_since_epoch().count());

    return response;
}