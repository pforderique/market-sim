#include "order_db.h"
#include <filesystem>
#include <sqlite3.h>
#include <iostream>

namespace storage
{

    OrderDB::OrderDB(const std::string &db_path) : db_path_(db_path) {}
    OrderDB::~OrderDB() {}

    bool OrderDB::init()
    {
        auto dir = std::filesystem::path(db_path_).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }

        sqlite3 *db;
        if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK)
        {
            std::cerr << "Failed to open DB\n";
            return false;
        }

        const char *sql = R"(
        CREATE TABLE IF NOT EXISTS unfilled_orders (
            order_id TEXT PRIMARY KEY,
            side TEXT,
            security_id TEXT,
            price REAL,
            qty REAL,
            remaining_qty REAL,
            status TEXT,
            client_id TEXT,
            timestamp TEXT
        );
    )";

        char *err_msg = nullptr;
        if (sqlite3_exec(db, sql, nullptr, nullptr, &err_msg) != SQLITE_OK)
        {
            std::cerr << "Failed to create table: " << err_msg << "\n";
            sqlite3_free(err_msg);
            sqlite3_close(db);
            return false;
        }

        sqlite3_close(db);
        return true;
    }

    bool OrderDB::save_orders(const std::vector<datamodel::Order> &orders)
    {
        sqlite3 *db;
        if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK)
            return false;

        const char *clear_sql = "DELETE FROM unfilled_orders;";
        sqlite3_exec(db, clear_sql, nullptr, nullptr, nullptr); // Clear stale entries

        const char *insert_sql = R"(
        INSERT INTO unfilled_orders (order_id, side, security_id, price, qty, remaining_qty, status, client_id, timestamp)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);
    )";

        sqlite3_stmt *stmt;
        sqlite3_prepare_v2(db, insert_sql, -1, &stmt, nullptr);

        for (const auto &order : orders)
        {
            // Store string conversions to maintain their lifetime during binding
            std::string side_str = datamodel::to_string(order.side);
            std::string security_id_str = datamodel::to_string(order.security_id);
            std::string status_str = datamodel::to_string(order.status);
            std::string timestamp_str = std::to_string(order.timestamp.time_since_epoch().count());

            sqlite3_bind_text(stmt, 1, order.order_id.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, side_str.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, security_id_str.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(stmt, 4, order.price);
            sqlite3_bind_double(stmt, 5, order.qty);
            sqlite3_bind_double(stmt, 6, order.remaining_qty);
            sqlite3_bind_text(stmt, 7, status_str.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 8, order.client_id.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 9, timestamp_str.c_str(), -1, SQLITE_TRANSIENT);

            int result = sqlite3_step(stmt);
            if (result != SQLITE_DONE)
            {
                // Handle error - optionally log the error message from sqlite3_errmsg(db)
            }
            sqlite3_reset(stmt);
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return true;
    }

    std::vector<datamodel::Order> OrderDB::load_orders()
    {
        std::vector<datamodel::Order> orders;
        sqlite3 *db;
        if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK)
            return orders;

        const char *select_sql = "SELECT side, security_id, price, qty, remaining_qty, status, client_id, order_id, timestamp FROM unfilled_orders;";
        sqlite3_stmt *stmt;
        if (sqlite3_prepare_v2(db, select_sql, -1, &stmt, nullptr) != SQLITE_OK)
        {
            sqlite3_close(db);
            return orders;
        }

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            datamodel::AddOrderRequest req;
            req.side = datamodel::side_from_string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
            req.security_id = datamodel::security_id_from_string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
            req.price = sqlite3_column_double(stmt, 2);
            req.qty = sqlite3_column_double(stmt, 3);
            req.client_id = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));

            datamodel::Order order(req);
            order.remaining_qty = sqlite3_column_double(stmt, 4);
            order.status = datamodel::order_status_from_string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5)));
            order.order_id = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 7));

            auto timestamp_val = std::stoll(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8)));
            order.timestamp = std::chrono::time_point<std::chrono::system_clock>(std::chrono::system_clock::duration(timestamp_val));

            orders.push_back(order);
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return orders;
    }

} // namespace storage
