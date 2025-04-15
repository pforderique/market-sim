#include "transaction.h"

namespace datamodel
{
    std::string Transaction::to_string() const
    {
        const std::string delimiter = "|";
        std::stringstream stream;

        auto time_t_timestamp = std::chrono::system_clock::to_time_t(timestamp);
        std::tm tm_timestamp;
        localtime_r(&time_t_timestamp, &tm_timestamp);

        stream << datamodel::to_string(security_id) << delimiter
               << string_utils::round_two_places(price) << delimiter
               << string_utils::round_two_places(qty) << delimiter
               << buyer_id << delimiter
               << seller_id << delimiter
               << std::put_time(&tm_timestamp, "%m%d%y%H%M%S");
        return stream.str();
    }
} // namespace datamodel
