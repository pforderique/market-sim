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
    char delim = ',';
    std::stringstream ss;
    ss << datamodel::to_string(security_id) << 
    delim << datamodel::to_string(side) << 
    delim << round_two_places(price) << 
    delim << round_two_places(qty) << 
    delim << client_id;
    return ss.str();
}

AddOrderRequest AddOrderRequest::from_string(const std::string& request) {
    std::stringstream ss(request);
    AddOrderRequest req;
    std::string token;
    char delim = ',';
    std::getline(ss, token, delim); req.security_id = security_id_from_string(token);
    std::getline(ss, token, delim); req.side = side_from_string(token);
    std::getline(ss, token, delim); req.price = std::stod(token);
    std::getline(ss, token, delim); req.qty = std::stod(token);
    std::getline(ss, token, delim); req.client_id = token;
    return req;
}

std::string Order::to_string() const {
    char delim = ',';
    std::stringstream ss;
    ss << from_request.to_string() << 
    delim << round_two_places(remaining_qty) << 
    delim << datamodel::to_string(status) << 
    delim << order_id;
    return ss.str();
}

Order Order::from_string(const std::string& order) {
    std::stringstream ss(order);
    Order ord;
    std::string token;
    char delim = ',';

    std::getline(ss, token, delim); ord.security_id = security_id_from_string(token);
    std::getline(ss, token, delim); ord.side = side_from_string(token);
    std::getline(ss, token, delim); ord.price = std::stod(token);
    std::getline(ss, token, delim); ord.qty = std::stod(token);
    std::getline(ss, token, delim); ord.client_id = token;
    std::getline(ss, token, delim); ord.remaining_qty = std::stod(token);
    std::getline(ss, token, delim); ord.status = order_status_from_string(token);
    std::getline(ss, token, delim); ord.order_id = token;
    return ord;
}

OrderID Order::generate_order_id(const AddOrderRequest& request) {
    auto request_attrs = request.client_id + std::to_string(
        std::chrono::system_clock::now().time_since_epoch().count()
    );
    return std::to_string(std::hash<std::string>{}(request_attrs));
}

} // namespace datamodel