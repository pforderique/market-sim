// File: exchange.cpp
// Implementation of the Exchange class with TCP server functionality

#include <atomic>
#include <cstring>
#include <thread>
#include <vector>

#include "./exchange.h"

// Default port for the TCP server
constexpr int DEFAULT_PORT = 8888;
constexpr int MAX_CONNECTIONS = 10;
constexpr int BUFFER_SIZE = 4096;

// Private class members not exposed in the header
struct Exchange::ExchangeImpl
{
    int server_socket = -1;
    std::atomic<bool> running{false};
    std::thread server_thread;
    std::vector<std::thread> client_threads;
    Exchange *parent;

    void handle_client(int client_socket);
};

// Implementation of private ExchangeImpl
namespace
{
    std::string trim(const std::string &str)
    {
        size_t start = str.find_first_not_of(" \n\r\t");
        size_t end = str.find_last_not_of(" \n\r\t");
        return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }

    std::string get_log_prefix(bool is_error = false)
    {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm local_tm = *std::localtime(&now_time_t);
        char timestamp[20];
        std::strftime(timestamp, sizeof(timestamp), "%m/%d %H:%M:%S", &local_tm);
        std::string type = is_error ? "[ERROR] " : "[INFO] ";
        return "[" + std::string(timestamp) + "]" + "[Exchange Server]" + type + " ";
    }

    void log_error(const std::string &message)
    {
        std::cerr << get_log_prefix(true) << message << std::endl;
    }

    void log_info(const std::string &message)
    {
        std::cout << get_log_prefix(false) << message << std::endl;
    }
}

// Constructor/Destructor implementation
Exchange::Exchange(std::shared_ptr<IMatchingEngine> matching_engine) : matching_engine(matching_engine), impl(std::make_unique<ExchangeImpl>())
{
    impl->parent = this;
}

Exchange::~Exchange()
{
    stop();
}

void Exchange::start()
{
    // Don't start if already running
    if (impl->running.load())
    {
        log_info("Exchange server is already running");
        return;
    }

    // Create socket
    impl->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (impl->server_socket < 0)
    {
        log_error("Failed to create socket");
        return;
    }

    // Set socket options to allow reuse of address/port
    int opt = 1;
    if (setsockopt(impl->server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        log_error("Failed to set socket options");
        close(impl->server_socket);
        impl->server_socket = -1;
        return;
    }

    // Setup address structure
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(DEFAULT_PORT);

    // Bind socket to port
    if (bind(impl->server_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        log_error("Failed to bind socket to port " + std::to_string(DEFAULT_PORT));
        close(impl->server_socket);
        impl->server_socket = -1;
        return;
    }

    // Start listening for connections
    if (listen(impl->server_socket, MAX_CONNECTIONS) < 0)
    {
        log_error("Failed to listen on socket");
        close(impl->server_socket);
        impl->server_socket = -1;
        return;
    }

    // Mark as running and start server thread
    impl->running.store(true);
    impl->server_thread = std::thread([this]()
                                      {
        log_info("Exchange server started on port " + std::to_string(DEFAULT_PORT));

        while (impl->running.load()) {
            // Accept incoming connections
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);

            // Set the socket to non-blocking with a timeout to allow for clean shutdown
            struct timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;

            fd_set read_fds;
            FD_ZERO(&read_fds);
            FD_SET(impl->server_socket, &read_fds);

            int activity = select(impl->server_socket + 1, &read_fds, NULL, NULL, &timeout);

            if (activity < 0) {
                if (errno != EINTR) {
                    log_error("Select error: " + std::string(strerror(errno)));
                }
                continue;
            }
            
            if (activity == 0) {
                // Timeout, continue to check running flag
                continue;
            }
            
            int client_socket = accept(impl->server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
            if (client_socket < 0) {
                log_error("Failed to accept connection");
                continue;
            }

            // Log connection
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            log_info("New connection from " + std::string(client_ip) + ":" + 
                     std::to_string(ntohs(client_addr.sin_port)));

            // Start a new thread to handle this client
            impl->client_threads.emplace_back(&ExchangeImpl::handle_client, impl.get(), client_socket);
        }
        
        log_info("Exchange server main thread stopped"); });

    log_info("Exchange server starting...");
}

void Exchange::stop()
{
    // Only stop if currently running
    if (!impl->running.load())
    {
        return;
    }

    // Set the running flag to false to stop the server thread
    impl->running.store(false);

    // Close the server socket
    if (impl->server_socket >= 0)
    {
        close(impl->server_socket);
        impl->server_socket = -1;
    }

    // Wait for the server thread to finish
    if (impl->server_thread.joinable())
    {
        impl->server_thread.join();
    }

    // Join all client threads
    for (auto &thread : impl->client_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
    impl->client_threads.clear();

    log_info("Exchange server stopped");
}

datamodel::AddOrderResponse Exchange::add_order(const datamodel::AddOrderRequest &request)
{
    // Forward the request to the matching engine
    return matching_engine->add_order(request);
}

// Implementation of ExchangeImpl methods
void Exchange::ExchangeImpl::handle_client(int client_socket)
{
    char buffer[BUFFER_SIZE] = {0};

    try
    {
        while (running.load())
        {
            // Receive data from client
            memset(buffer, 0, BUFFER_SIZE);

            // Set timeout on receive to allow for clean shutdown
            struct timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);

            int bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

            if (bytes_read < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    // Timeout, check if we should continue running
                    continue;
                }
                log_error("Error reading from socket: " + std::string(strerror(errno)));
                break;
            }

            if (bytes_read == 0)
            {
                // Client disconnected
                log_info("Client disconnected");
                break;
            }

            // Process the received data
            std::string request_data(buffer, bytes_read);
            request_data = trim(request_data);
            log_info("Received: " + request_data);

            // Parse the request and process it
            try
            {
                datamodel::AddOrderRequest order_request =
                    datamodel::AddOrderRequest::from_string(request_data);

                // Forward the request to the exchange
                datamodel::AddOrderResponse response = parent->add_order(order_request);

                // Serialize the response and send it back
                std::string response_data = response.to_string();
                send(client_socket, response_data.c_str(), response_data.length(), 0);

                // Close the client socket after sending the response.
                // Else the client will hang waiting for more data.
                shutdown(client_socket, SHUT_WR);
                close(client_socket);

                log_info("Sent response: " + response_data);
                break;
            }
            catch (const std::exception &e)
            {
                // Send error response
                std::string error_msg = "Error processing request: " + std::string(e.what());
                log_error(error_msg);
                send(client_socket, error_msg.c_str(), error_msg.length(), 0);
            }
        }
    }
    catch (const std::exception &e)
    {
        log_error("Exception in client handler: " + std::string(e.what()));
    }

    // Close the client socket
    close(client_socket);
}
