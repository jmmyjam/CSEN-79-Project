#ifndef PARSER_H
#define PARSER_H

#include <string>

namespace alert_service
{
    enum class CommandType
    {
        SUB,
        UNSUB,
        PUB,
        PRICE,
        TICK,
        INBOX,
        HISTORY,
        STATS,
        INVALID
    };

    struct Command
    {
        CommandType type;
        std::string user;
        std::string topic;
        std::string message;
        std::string item;
        std::string time_option;
        double price = 0.0;
        int number = 0;
        bool has_price = false;
    };

    Command parse_command(const std::string& line);
}

#endif