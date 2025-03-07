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
    int qty;
    ClientID buyer_id;
    ClientID seller_id;
    std::chrono::time_point<std::chrono::system_clock> timestamp;

    Transaction(
        SecurityID security_id,
        double price,
        int qty,
        ClientID buyer_id, 
        ClientID seller_id
    ) : security_id(security_id), 
        price(price),
        qty(qty),
        buyer_id(buyer_id),
        seller_id(seller_id),
        timestamp(std::chrono::system_clock::now()) {}
};
}