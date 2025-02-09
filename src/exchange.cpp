#include "./exchange.h"

Exchange::Exchange() {
    matching_engine = std::make_shared<MatchingEngine>();
    printf("Exchange created.\n");
}

Exchange::~Exchange() {
    printf("Exchange destroyed.\n");
}