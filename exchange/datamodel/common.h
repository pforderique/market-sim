// File: common.cpp
// Common data structures and serialization functions

#pragma once
#include <string>
#include <sstream>
#include <iomanip>

namespace datamodel
{
    // Enum for the side of a trade
    enum Side
    {
        BID,
        ASK
    };

    // Enum for security identifiers
    enum SecurityID
    {
        AAPL,
        MSFT,
        GOOG,
        AMZN,
        TSLA,
        O,

        // Special values for iteration and bounds checking.
        FIRST = AAPL,
        LAST = O,
    };

    inline std::string to_string(Side side)
    {
        switch (side)
        {
        case BID:
            return "BID";
        case ASK:
            return "ASK";
        default:
            return "UNKNOWN";
        }
    }

    inline Side side_from_string(const std::string &side)
    {
        if (side == "BID")
            return BID;
        if (side == "ASK")
            return ASK;
        throw std::runtime_error("Invalid Side string: " + side);
    }

    inline std::string to_string(SecurityID id)
    {
        switch (id)
        {
        case AAPL:
            return "AAPL";
        case MSFT:
            return "MSFT";
        case GOOG:
            return "GOOG";
        case AMZN:
            return "AMZN";
        case TSLA:
            return "TSLA";
        case O:
            return "O";
        default:
            return "UNKNOWN";
        }
    }

    inline SecurityID security_id_from_string(const std::string &id)
    {
        if (id == "AAPL")
            return AAPL;
        if (id == "MSFT")
            return MSFT;
        if (id == "GOOG")
            return GOOG;
        if (id == "AMZN")
            return AMZN;
        if (id == "TSLA")
            return TSLA;
        if (id == "O")
            return O;
        throw std::runtime_error("Invalid SecurityID string: " + id);
    }

    using OrderID = std::string;
    using ClientID = std::string;
} // namespace datamodel

namespace string_utils
{
    inline std::string trim(const std::string &str)
    {
        size_t start = str.find_first_not_of(" \n\r\t");
        size_t end = str.find_last_not_of(" \n\r\t");
        return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }

    inline std::string round_two_places(double num)
    {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << num;
        return stream.str();
    };
} // namespace string_utils
