// File: exchange.h
// Contains the Exchange Server with the matching engine and security (asset) data structures.

#pragma once
#include <memory>
#include "./modules/matching_engine.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <unistd.h>

class Exchange
{
public:
    Exchange(std::shared_ptr<IMatchingEngine> matching_engine);
    ~Exchange();

    void start();
    void stop();

    datamodel::AddOrderResponse add_order(const datamodel::AddOrderRequest &request);

    std::shared_ptr<IMatchingEngine> get_matching_engine()
    {
        return matching_engine;
    }

private:
    std::shared_ptr<IMatchingEngine> matching_engine;

    // Private implementation pattern (pimpl) to hide TCP server details
    struct ExchangeImpl;
    std::unique_ptr<ExchangeImpl> impl;
    // TODO: Add Security data
};
