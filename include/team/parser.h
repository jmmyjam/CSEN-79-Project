
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
        INVALID,
        UNKNOWN
    };

    struct Command
    {
        CommandType type = CommandType::INVALID;

        std::string user;
        std::string topic;
        std::string message;

        std::string option;
        std::string typed;

        long long price_cents = 0;
        long long at_minute = 0;
        long long every_hours = 0;
        long long tick_minutes = 0;
        long long count = 0;
        bool has_count = false;
    };

    // Ensure that an input only consists of digits and not something like 12asdas
    inline bool parse_whole(const std::string &text, long long &value)
    {
        if (text.empty() || text.length() > 15)
            return false;

        value = 0;
        for (size_t i = 0; i < text.length(); ++i)
        {
            if (text[i] < '0' || text[i] > '9')
                return false;
            value = value * 10 + (text[i] - '0');
        }
        return true;
    }

    // Turn a price with decimals fully into cent format, like 12.55 (dollars) turns into 1255 (cents)
    inline bool parse_cents(const std::string &text, long long &cents)
    {
        size_t point = text.find('.');
        long long dollars;
        long long fraction = 0;

        if (point == std::string::npos)
        {
            if (!parse_whole(text, dollars))
                return false;
        }
        else
        {
            std::string after = text.substr(point + 1);
            if (!parse_whole(text.substr(0, point), dollars) ||
                after.length() > 2 ||
                !parse_whole(after, fraction))
                return false;
            if (after.length() == 1)
                fraction *= 10;
        }
        cents = dollars * 100 + fraction;
        return true;
    }

    // turn the military time input into pure minutes 08:00 > 480
    inline bool parse_clock(const std::string &text, long long &minute_of_day)
    {
        long long hours;
        long long minutes;

        if (text.length() != 5 || text[2] != ':')
            return false;
        if (!parse_whole(text.substr(0, 2), hours) ||
            !parse_whole(text.substr(3, 2), minutes))
            return false;
        if (hours > 23 || minutes > 59)
            return false;

        minute_of_day = hours * 60 + minutes;
        return true;
    }

    // format into something like D0 06:00
    inline std::string format_time(long long minute)
    {
        long long day = minute / 1440;
        long long hours = (minute % 1440) / 60;
        long long minutes = minute % 60;
        std::string result = "D" + std::to_string(day) + " ";

        if (hours < 10)
            result += "0";
        result += std::to_string(hours) + ":";
        if (minutes < 10)
            result += "0";
        result += std::to_string(minutes);

        return result;
    }

    inline std::string format_cents(long long cents)
    {
        std::string result = std::to_string(cents / 100) + ".";

        if (cents % 100 < 10)
            result += "0";
        result += std::to_string(cents % 100);

        return result;
    }
}

#endif
