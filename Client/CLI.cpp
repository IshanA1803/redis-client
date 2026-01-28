#include "CLI.h"
#include <vector>
#include <readline/readline.h>
#include <readline/history.h>

static bool lineReady = false;
static std::string latestInput = "";

// readline callback when a full line is ready
static void handleLine(char* line) {
    if (!line) {
        lineReady = true;
        latestInput = "exit";  // treat Ctrl+D as exit
        return;
    }

    latestInput = std::string(line);
    add_history(latestInput.c_str());
    free(line);
    lineReady = true;
}

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

    rl_callback_handler_install((host + ":" + std::to_string(port) + "> ").c_str(),handleLine);

    while (true) {
        // Let readline process one character if available
        rl_callback_read_char();

        // If a full line was entered (Enter pressed)
        if (lineReady) {
            std::string line = trim(latestInput);
            lineReady = false;

            if (line == "quit" || line == "exit") {
                std::cout << "Goodbye.\n";
                break;
            }

            if (line == "help") {
                std::cout << "Displaing help\n";
                continue;
            }

            // Split command into tokens
            std::vector<std::string> args = CommandHandler::splitArgs(line);
            if (args.empty()) {
                continue;
            }

            // Build RESP command
            std::string command = CommandHandler::buildRESPcommand(args);

            // Send to Redis
            if (!redisClient.sendCommand(command)) {
                std::cerr << "(Error) Failed to send command.\n";
                break;
            }

            // Parse and print response
            std::string response =
                ResponseParser::parseResponse(redisClient.getSocketFD());
            std::cout << response << "\n";
            rl_on_new_line();
            rl_redisplay();

        }
    }
    rl_callback_handler_remove();
    redisClient.disconnect();
}