// File: exchange.h
// Contains the matching engine and security (asset) data structures.

#pragma once
#include <memory>
#include "./modules/matching_engine.h"

class Exchange 
{
public:
    Exchange();
    ~Exchange();

    // void start();
    // void stop();

    std::shared_ptr<MatchingEngine> get_matching_engine() {
        return matching_engine;
    }

private:
    std::shared_ptr<MatchingEngine> matching_engine;

    // TODO: Add Security data
};