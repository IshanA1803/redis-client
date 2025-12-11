#include "CommandHandler.h"
#include <regex>
#include <sstream>

// Splits the input string into tokens, handling quoted substrings as single tokens
std::vector<std::string> CommandHandler::splitArgs(const std::string &input) {
    std::vector<std::string> tokens;

    // Regex to match words or quoted strings
    std::regex rgx(R"((\"[^\"]+\"|\S+))");
    auto words_begin = std::sregex_iterator(input.begin(), input.end(), rgx);
    auto words_end = std::sregex_iterator();

    // Iterate through all matches and add them to the tokens vector
    for (auto it = words_begin; it != words_end; ++it) {
        std::string token = it->str();
        // Remove quotes
        if (token.size() >= 2 && token.front() == '\"' && token.back() == '\"') {
            token = token.substr(1, token.size() - 2);
        }
        tokens.push_back(token);
    }

    return tokens;
}

// Builds a RESP formatted command from the given tokens
/*
    RESP format:
    *<number of args>\r\n
    $<len of arg1>\r\n
    <arg1>\r\n
*/
std::string CommandHandler::buildRESPcommand(const std::vector<std::string> &args) {
    std::ostringstream oss; // use stringstream for efficient string building
    oss << "*" << args.size() << "\r\n"; // number of args

    for (const auto &arg: args) {
        oss << "$" << arg.size() << "\r\n" << arg << "\r\n"; // len and value of arg
    }
    return oss.str(); 
}