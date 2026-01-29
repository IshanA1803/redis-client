#include "CLI.h"
#include <vector>
#include <readline/readline.h>
#include <readline/history.h>
#include <poll.h>

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

    // Setup poll for stdin and redis socket
    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;  // terminal
    fds[0].events = POLLIN;

    fds[1].fd = sockfd;       // redis socket
    fds[1].events = POLLIN;

    // Initialize readline in callback mode
    rl_callback_handler_install((host + ":" + std::to_string(port) + "> ").c_str(),handleLine);

    while (true) {
        int ret = poll(fds, 2, -1);  // block indefinitely
        if (ret < 0) {
            perror("(Error) poll failed");
            break;
        }
        
        // Redis socket error or hangup
        if (fds[1].revents & (POLLHUP | POLLERR)) {
            std::cout << "\nRedis connection lost. Exiting...\n";
            break;
        }

        // If Redis socket is readable then check for server closure
        if (fds[1].revents & POLLIN) {
            char buffer[1];
            ssize_t bytes = recv(sockfd, buffer, sizeof(buffer), MSG_PEEK);
        
            if (bytes == 0) {
                std::cout << "\nRedis server closed the connection. Exiting...\n";
                break;
            }
        }

        if (fds[0].revents & POLLIN) {
            rl_callback_read_char();
        }

        if (lineReady) {
            std::string line = trim(latestInput);
            lineReady = false;

            if (line == "quit" || line == "exit") {
                std::cout << "Goodbye.\n";
                break;
            }

            if (line == "help") {
                std::cout << "Displaing help\n";
                rl_on_new_line();
                rl_redisplay();
                continue;
            }

            std::vector<std::string> args = CommandHandler::splitArgs(line);
            if (args.empty()) {
                rl_on_new_line();
                rl_redisplay();
                continue;
            }

            std::string command = CommandHandler::buildRESPcommand(args);
            if (!redisClient.sendCommand(command)) {
                std::cerr << "(Error) Failed to send command.\n";
                break;
            }

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