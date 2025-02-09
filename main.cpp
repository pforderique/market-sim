#include <iostream>
#include <memory>
#include "datamodel/datamodel.h"
#include "src/exchange/modules/matching_engine.h"
#include "src/exchange/exchange.h"

int main() {
    auto me = std::make_shared<MatchingEngine>();
    auto exchange = std::make_shared<Exchange>(me);

    datamodel::AddOrderRequest request;
    request.security_id = datamodel::SecurityID::AAPL;
    request.side = datamodel::Side::BID;
    request.price = 100.0;
    request.qty = 10;
    request.client_id = "Fabrizzio";

    datamodel::AddOrderResponse res = exchange
        ->get_matching_engine()
        ->add_order(request);
    printf("Order ID: %s\n", res.order_id.c_str());
    printf("Security ID: %d\n", res.security_id);
    return 0;
}