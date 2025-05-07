#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <netdb.h>
#include <cstring>

#include "multicast.h"

constexpr const char* MULTICAST_IP = "239.255.42.99";
constexpr int BROADCAST_PORT = 9999;

void multicast_transaction(const datamodel::Transaction &txn)
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        return;

    // Set TTL (time-to-live) so multicast can cross router hops if needed
    int ttl = 1;
    setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(BROADCAST_PORT);
    addr.sin_addr.s_addr = inet_addr(MULTICAST_IP);

    std::string msg = txn.to_string() + "\n";
    sendto(sock, msg.c_str(), msg.size(), 0, (sockaddr *)&addr, sizeof(addr));

    close(sock);
}
