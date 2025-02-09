// File: common.cpp
#pragma once
#include <string>

namespace datamodel 
{
// Enum for the side of a trade
enum Side {
    BID,
    ASK
};

// Enum for security identifiers
enum SecurityID {
    AAPL,
    MSFT,
    GOOG,
    AMZN,
    TSLA,
};

using OrderID = std::string;
using ClientID = std::string;
}