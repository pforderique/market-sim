// File: exchange.h
// Contains the matching engine and security (asset) data structures.

#pragma once
#include <memory>
#include "./modules/matching_engine.h"

class Exchange 
{
public:
    Exchange(std::shared_ptr<IMatchingEngine> matching_engine) : matching_engine(matching_engine) {
        printf("Exchange created.\n");
    }
    ~Exchange();

    // void start();
    // void stop();

    std::shared_ptr<IMatchingEngine> get_matching_engine() {
        return matching_engine;
    }

private:
    std::shared_ptr<IMatchingEngine> matching_engine;

    // TODO: Add Security data
};