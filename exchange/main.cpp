#include <iostream>
#include <memory>
#include <thread>

#include "datamodel/datamodel.h"
#include "src/exchange/modules/multicast.h"
#include "src/exchange/modules/matching_engine.h"
#include "src/exchange/exchange.h"

int main()
{
    auto on_transaction = [](const datamodel::Transaction &t)
    {
        printf("<TRANSACTION: %s>\n", t.to_string().c_str());
        multicast_transaction(t);
    };
    auto me = std::make_shared<MatchingEngine>(on_transaction);
    auto exchange = std::make_shared<Exchange>(me);

    exchange->start();
    if (isatty(STDIN_FILENO)) {
        std::string input;
        while (true) {
            std::cout << "Type 'q' to stop the exchange:\n";
            std::getline(std::cin, input);
            if (input == "q") break;
        }
    } else {
        // No interactive terminal — just run indefinitely
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(60));
        }
    }
    exchange->stop();
    return 0;
}
