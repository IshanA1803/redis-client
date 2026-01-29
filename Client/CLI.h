#ifndef CLI_H
#define CLI_H

#include <string>
#include "RedisClient.h"
#include "CommandHandler.h"
#include "ResponseParser.h"

class CLI {
public:
    CLI(const std::string &host, int port);
    void run();
    void handleSubscription(const std::vector<std::string>& commandArgs);

private:
    RedisClient redisClient;
};

#endif