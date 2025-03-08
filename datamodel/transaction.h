// File: transaction.cpp
# pragma once
#include <chrono>
#include "./common.h"


namespace datamodel
{
// Transaction struct
struct Transaction {
    SecurityID security_id;
    double price;
    double qty;
    ClientID buyer_id;
    ClientID seller_id;
    std::chrono::time_point<std::chrono::system_clock> timestamp;

    Transaction(
        SecurityID security_id,
        double price,
        double qty,
        ClientID buyer_id, 
        ClientID seller_id
    ) : security_id(security_id), 
        price(price),
        qty(qty),
        buyer_id(buyer_id),
        seller_id(seller_id),
        timestamp(std::chrono::system_clock::now()) {}

    std::string to_string() const {
        return "{Security ID: " + datamodel::to_string(security_id) +
            ", Price: " + round_two_places(price) +
            ", Qty: " + round_two_places(qty) +
            ", Buyer ID: " + buyer_id +
            ", Seller ID: " + seller_id +
            ", Timestamp: " + std::to_string(
                timestamp.time_since_epoch().count()
            ) + 
            "}";
    }

private:
    std::string round_two_places(double num) const {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << num;
        return stream.str();
    }
};
}