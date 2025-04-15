// File: transaction.cpp
# pragma once
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

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

    // Convert the transaction to a string for broadcasting.
    // Format: "security_id|price|qty|buyer_id|seller_id|timestamp"
    // Example: `1|100.50|10.00|123|456|2023-10-01T12:34:56Z`
    std::string to_string() const;

private:
    std::string round_two_places(double num) const {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << num;
        return stream.str();
    }
};
}