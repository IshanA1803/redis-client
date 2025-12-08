#include "CLI.h"

CLI::CLI(const std::string &host, int port) 
    : redisClient(host, port) {}

void CLI::run() {
    if (!redisClient.connectToServer()) {
        return;
    }
    int sockfd = redisClient.getSocketFD();
    std::cout << "Connected to Redis at "<< sockfd << "\n";
}