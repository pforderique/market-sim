#pragma once

#include <vector>
#include <string>
#include "../../../datamodel/order_api.h"

namespace storage
{

    class OrderDB
    {
    public:
        explicit OrderDB(const std::string &db_path);
        ~OrderDB();

        bool init(); // Create table if not exists

        bool save_orders(const std::vector<datamodel::Order> &orders);
        std::vector<datamodel::Order> load_orders();

    private:
        std::string db_path_;
    };

} // namespace storage
