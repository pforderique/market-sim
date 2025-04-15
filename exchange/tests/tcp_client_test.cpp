// // File: tcp_client_test.cpp
// // Simple test client to verify the Exchange TCP server

// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <unistd.h>
// #include <iostream>
// #include <string>
// #include <cstring>

// int main(int argc, char* argv[]) {
//     // Default server address and port
//     const char* server_ip = "127.0.0.1";
//     int server_port = 8888;
    
//     // Parse command line arguments
//     if (argc > 1) server_ip = argv[1];
//     if (argc > 2) server_port = std::stoi(argv[2]);
    
//     // Create socket
//     int sock = socket(AF_INET, SOCK_STREAM, 0);
//     if (sock < 0) {
//         std::cerr << "Failed to create socket" << std::endl;
//         return 1;
//     }
    
//     // Connect to server
//     struct sockaddr_in server_addr;
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(server_port);
    
//     if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
//         std::cerr << "Invalid address or address not supported" << std::endl;
//         close(sock);
//         return 1;
//     }
    
//     if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
//         std::cerr << "Connection failed" << std::endl;
//         close(sock);
//         return 1;
//     }
    
//     std::cout << "Connected to exchange server at " << server_ip << ":" << server_port << std::endl;
    
//     // Send a sample order request
//     // In a real implementation, you would serialize a proper order request object
//     std::string message = "ADD_ORDER:AAPL:BUY:100:150.25";
    
//     if (send(sock, message.c_str(), message.length(), 0) < 0) {
//         std::cerr << "Failed to send message" << std::endl;
//         close(sock);
//         return 1;
//     }
    
//     std::cout << "Sent: " << message << std::endl;
    
//     // Receive response
//     char buffer[1024] = {0};
//     int bytes_read = read(sock, buffer, 1024);
//     if (bytes_read < 0) {
//         std::cerr << "Error reading response" << std::endl;
//     } else {
//         std::cout << "Response: " << std::string(buffer, bytes_read) << std::endl;
//     }
    
//     // Close the socket
//     close(sock);
    
//     return 0;
// }