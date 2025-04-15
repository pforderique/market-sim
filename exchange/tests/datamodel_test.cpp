#include <gtest/gtest.h>

#include "../datamodel/datamodel.h"


// Fixture for testing the MatchingEngine class
class DatamodelTest : public ::testing::Test {
protected:
    // Set up method that runs before each test
    void SetUp() override {
        // Initialize any common test resources
    }

    // Tear down method that runs after each test
    void TearDown() override {
        // Clean up any resources if needed
    }

    // Shared resources for tests
    std::shared_ptr<datamodel::AddOrderRequest> bid_request;
    std::shared_ptr<datamodel::AddOrderRequest> ask_request;
    std::shared_ptr<datamodel::Order> bid_order;
    std::shared_ptr<datamodel::Order> ask_order;
};

TEST_F(DatamodelTest, AddOrderRequest_ToString) {
    datamodel::AddOrderRequest request{
        .side = datamodel::Side::BID,
        .security_id = datamodel::SecurityID::AAPL,
        .price = 250.0,
        .qty = 10,
        .client_id = "piero",
    };

    std::string expected = "BID|AAPL|250.00|10.00|piero";
    ASSERT_EQ(request.to_string(), expected);
}

TEST_F(DatamodelTest, AddOrderRequest_FromString) {
    std::string request_str = "BID|AAPL|250.00|10.00|piero";
    datamodel::AddOrderRequest request = datamodel::AddOrderRequest::from_string(request_str);

    ASSERT_EQ(request.side, datamodel::Side::BID);
    ASSERT_EQ(request.security_id, datamodel::SecurityID::AAPL);
    ASSERT_EQ(request.price, 250.0);
    ASSERT_EQ(request.qty, 10.0);
    ASSERT_EQ(request.client_id, "piero");
}

TEST_F(DatamodelTest, Order_ToString) {
    datamodel::AddOrderRequest request{
        .side = datamodel::Side::BID,
        .security_id = datamodel::SecurityID::AAPL,
        .price = 250.0,
        .qty = 10,
        .client_id = "piero",
    };
    datamodel::Order order(request);

    std::string expected = "BID|AAPL|250.00|10.00|piero|10.00|PENDING";
    ASSERT_TRUE(order.to_string().rfind(expected, 0) == 0);
}

TEST_F(DatamodelTest, Order_FromString) {
    std::string order_str = "BID|AAPL|250.00|10.00|piero|10.00|PENDING|0";
    datamodel::Order order = datamodel::Order::from_string(order_str);

    ASSERT_EQ(order.side, datamodel::Side::BID);
    ASSERT_EQ(order.security_id, datamodel::SecurityID::AAPL);
    ASSERT_EQ(order.price, 250.0);
    ASSERT_EQ(order.qty, 10.0);
    ASSERT_EQ(order.client_id, "piero");
    ASSERT_EQ(order.remaining_qty, 10.0);
    ASSERT_EQ(order.status, datamodel::OrderStatus::PENDING);
}