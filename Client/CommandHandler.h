#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <vector>
#include <string>

class CommandHandler {
public:
    // Split commands into tokens
    static std::vector<std::string> splitArgs(const std::string &input);
    // Build RESP formatted command from tokens
    static std::string buildRESPcommand(const std::vector<std::string> &args);
};

#endif // COMMAND_HANDLER_H