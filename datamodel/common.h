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

inline std::string to_string(Side side) {
    switch (side) {
        case BID: return "BID";
        case ASK: return "ASK";
        default: return "UNKNOWN";
    }
}

// Enum for security identifiers
enum SecurityID {
    AAPL,
    MSFT,
    GOOG,
    AMZN,
    TSLA,

    // Special values for iteration and bounds checking.
    FIRST = AAPL,
    LAST = TSLA,
};


inline std::string to_string(SecurityID id) {
    switch (id) {
        case AAPL: return "AAPL";
        case MSFT: return "MSFT";
        case GOOG: return "GOOG";
        case AMZN: return "AMZN";
        case TSLA: return "TSLA";
        default: return "UNKNOWN";
    }
}

using OrderID = std::string;
using ClientID = std::string;
} // namespace datamodel