// File: transaction.cpp
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
};
}