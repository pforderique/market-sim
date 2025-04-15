#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

#include "broadcast.h"

constexpr const char *BROADCAST_IP = "255.255.255.255";
constexpr int BROADCAST_PORT = 9999;

void broadcast_transaction(const datamodel::Transaction &txn)
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        return;

    int broadcastEnable = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(BROADCAST_PORT);
    addr.sin_addr.s_addr = inet_addr(BROADCAST_IP);

    std::string msg = txn.to_string();
    sendto(sock, msg.c_str(), msg.size(), 0, (sockaddr *)&addr, sizeof(addr));

    close(sock);
}
