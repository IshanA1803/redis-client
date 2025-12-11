#include "CLI.h"
#include <vector>

//Helper function to trim whitespaces
static std::string trim(const std::string &s) {
    size_t start = s.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\n\r\f\v");
    return s.substr(start, end - start + 1);
}

CLI::CLI(const std::string &host, int port) 
    : redisClient(host, port) {}

void CLI::run() {
    if (!redisClient.connectToServer()) {
        return;
    }
    int sockfd = redisClient.getSocketFD();
    std::cout << "Connected to Redis at "<< sockfd << "\n";

    std::string host = "127.0.0.1"; //Dummy host
    int port = 6379; //Dummy port

    while (true) {
        std::cout << host  << ":" << port << "> ";
        std::cout.flush(); // Ensure prompt is displayed immediately
        std::string line;
        if (!std::getline(std::cin, line)) break; 
        line = trim(line);
        if(line.empty()) continue; // Ignore empty lines
        if (line == "quit") {
            std::cout << "Goodbye.\n";
            break;
        }
        if (line == "help") {
            std::cout << "Displaing help\n"; // Placeholder for help information
            continue;
        }

        // Split command into tokens
        std::vector<std::string> args = CommandHandler::splitArgs(line);
        if(args.empty()) continue; // No valid command found

        // Build RESP command from tokens
        std::string command = CommandHandler::buildRESPcommand(args);
        // Send command to Redis server
        if (!redisClient.sendCommand(command)) {
            std::cerr << "(Error) Failed to send command.\n";
            break;
        }
        // Parse and print response will be handled here
    }

}