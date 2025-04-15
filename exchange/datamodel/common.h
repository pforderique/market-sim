// File: common.cpp
// Common data structures and serialization functions

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

    // Special values for iteration and bounds checking.
    FIRST = AAPL,
    LAST = TSLA,
};

inline std::string to_string(Side side) {
    switch (side) {
        case BID: return "BID";
        case ASK: return "ASK";
        default: return "UNKNOWN";
    }
}

inline Side side_from_string(const std::string& side) {
    if (side == "BID") return BID;
    if (side == "ASK") return ASK;
    throw std::runtime_error("Invalid Side string: " + side);
}

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

inline SecurityID security_id_from_string(const std::string& id) {
    if (id == "AAPL") return AAPL;
    if (id == "MSFT") return MSFT;
    if (id == "GOOG") return GOOG;
    if (id == "AMZN") return AMZN;
    if (id == "TSLA") return TSLA;
    throw std::runtime_error("Invalid SecurityID string: " + id);
}

using OrderID = std::string;
using ClientID = std::string;
} // namespace datamodel