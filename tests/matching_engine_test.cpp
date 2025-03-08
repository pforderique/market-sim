#include <gtest/gtest.h>

#include "../datamodel/datamodel.h"
#include "../src/exchange/modules/matching_engine.h" 


// Fixture for testing the MatchingEngine class
class MatchingEngineTest : public ::testing::Test {
protected:
    // Set up method that runs before each test
    void SetUp() override {
        // Initialize any common test resources
        matchingEngine = std::make_unique<MatchingEngine>();

        bid_request = std::make_shared<datamodel::AddOrderRequest>(
            datamodel::AddOrderRequest{
            .security_id = datamodel::SecurityID::AAPL,
            .side = datamodel::Side::BID,
            .price = 250.0,
            .qty = 10,
            .client_id = "piero",
            }
        );
        bid_order = std::make_shared<datamodel::Order>(*bid_request);

        ask_request = std::make_shared<datamodel::Order>(*bid_request);
        ask_request->side = datamodel::Side::ASK;
        ask_request->client_id = "fabrizzio";
        ask_order = std::make_shared<datamodel::Order>(*ask_request);
    }

    // Tear down method that runs after each test
    void TearDown() override {
        // Clean up any resources if needed
        matchingEngine.reset();
    }

    // Shared resources for tests
    std::unique_ptr<MatchingEngine> matchingEngine;
    std::shared_ptr<datamodel::AddOrderRequest> bid_request;
    std::shared_ptr<datamodel::AddOrderRequest> ask_request;
    std::shared_ptr<datamodel::Order> bid_order;
    std::shared_ptr<datamodel::Order> ask_order;
};

// Test for add_order method
TEST_F(MatchingEngineTest, AddOrder_Success) {
    datamodel::AddOrderResponse response =
        matchingEngine->add_order(*bid_request);

    // Check if the order response is correct
    ASSERT_EQ(response.security_id, bid_request->security_id);
    ASSERT_EQ(response.side, bid_request->side);
    ASSERT_EQ(response.price, bid_request->price);
    ASSERT_EQ(response.qty, bid_request->qty);
}

// Test match_order method
TEST_F(MatchingEngineTest, MatchOrder_NoMatchPrice) {
    std::vector<datamodel::Transaction> transactions;

    bid_order->price = 200.0;
    transactions = matchingEngine->match_order(*bid_order);
    ASSERT_EQ(transactions.size(), 0);

    ask_order->price = 300.0;
    transactions = matchingEngine->match_order(*ask_order);
    ASSERT_EQ(transactions.size(), 0);
}

TEST_F(MatchingEngineTest, MatchOrder_MatchPriceFullQty) {
    std::vector<datamodel::Transaction> transactions;

    bid_order->price = 200.0;
    bid_order->qty = 5;
    bid_order->remaining_qty = 5;
    transactions = matchingEngine->match_order(*bid_order);
    ASSERT_EQ(transactions.size(), 0);

    ask_order->price = 200.0;
    ask_order->qty = 5;
    ask_order->remaining_qty = 5;
    transactions = matchingEngine->match_order(*ask_order);
    ASSERT_EQ(transactions.size(), 1);
    auto transaction = transactions[0];
    ASSERT_EQ(transaction.security_id, ask_order->security_id);
    ASSERT_EQ(transaction.price, ask_order->price);
    ASSERT_EQ(transaction.qty, ask_order->qty);
    ASSERT_EQ(transaction.buyer_id, bid_order->client_id);
    ASSERT_EQ(transaction.seller_id, ask_order->client_id);
}

// Main function to run all tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}