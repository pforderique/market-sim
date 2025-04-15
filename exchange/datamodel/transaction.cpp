#include "transaction.h"

namespace datamodel
{
std::string Transaction::to_string() const{
    const std::string delimiter = "|";
    std::stringstream stream;

    stream << datamodel::to_string(security_id) << delimiter
            << string_utils::round_two_places(price) << delimiter
            << string_utils::round_two_places(qty) << delimiter
            << buyer_id << delimiter
            << seller_id << delimiter
            << std::chrono::system_clock::to_time_t(timestamp);
    return stream.str();
}
} // namespace datamodel
