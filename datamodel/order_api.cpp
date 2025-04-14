#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <vector>

#include "./order_api.h"

std::string round_two_places(double num) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << num;
    return stream.str();
};

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \n\r\t");
    size_t end = str.find_last_not_of(" \n\r\t");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

constexpr char DELIM = '|';

namespace datamodel
{
std::string to_string(OrderStatus status) {
    switch (status) {
        case PENDING: return "PENDING";
        case PARTIALLY_FILLED: return "PARTIALLY_FILLED";
        case FILLED: return "FILLED";
        case CANCELLED: return "CANCELLED";
        default: throw std::invalid_argument("Unknown OrderStatus");
    }
}

OrderStatus order_status_from_string(const std::string& status) {
    if (status == "PENDING") return PENDING;
    if (status == "PARTIALLY_FILLED") return PARTIALLY_FILLED;
    if (status == "FILLED") return FILLED;
    if (status == "CANCELLED") return CANCELLED;
    throw std::invalid_argument("Unknown OrderStatus string");
}

std::string AddOrderRequest::to_string() const {
    std::stringstream ss;
    ss << datamodel::to_string(side) << 
    DELIM << datamodel::to_string(security_id) << 
    DELIM << round_two_places(price) << 
    DELIM << round_two_places(qty) << 
    DELIM << client_id;
    return trim(ss.str());
}

AddOrderRequest AddOrderRequest::from_string(const std::string& request) {
    std::stringstream ss(trim(request));
    AddOrderRequest req;
    std::string token;
    std::getline(ss, token, DELIM); req.side = side_from_string(token);
    std::getline(ss, token, DELIM); req.security_id = security_id_from_string(token);
    std::getline(ss, token, DELIM); req.price = std::stod(token);
    std::getline(ss, token, DELIM); req.qty = std::stod(token);
    std::getline(ss, token, DELIM); req.client_id = token;
    return req;
}

std::string Order::to_string() const {
    std::stringstream ss;
    ss << from_request.to_string() << 
    DELIM << round_two_places(remaining_qty) << 
    DELIM << datamodel::to_string(status) << 
    DELIM << order_id;
    return trim(ss.str());
}

Order Order::from_string(const std::string& order) {
    std::stringstream ss(trim(order));
    Order ord;
    std::string token;

    std::getline(ss, token, DELIM); ord.side = side_from_string(token);
    std::getline(ss, token, DELIM); ord.security_id = security_id_from_string(token);
    std::getline(ss, token, DELIM); ord.price = std::stod(token);
    std::getline(ss, token, DELIM); ord.qty = std::stod(token);
    std::getline(ss, token, DELIM); ord.client_id = token;
    std::getline(ss, token, DELIM); ord.remaining_qty = std::stod(token);
    std::getline(ss, token, DELIM); ord.status = order_status_from_string(token);
    std::getline(ss, token, DELIM); ord.order_id = token;
    return ord;
}

OrderID Order::generate_order_id(const AddOrderRequest& request) {
    auto request_attrs = request.client_id + std::to_string(
        std::chrono::system_clock::now().time_since_epoch().count()
    );
    return std::to_string(std::hash<std::string>{}(request_attrs));
}

} // namespace datamodel