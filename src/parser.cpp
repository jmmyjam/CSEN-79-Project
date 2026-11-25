#include "../include/team/parser.h"

#include <sstream>
#include <string>

namespace alert_service
{
    // Used to reject trailing words, such as the "at 08:00" in
    // "SUB carol horoscope/leo below 10 at 08:00".
    inline bool no_more_words(std::istringstream &input)
    {
        std::string extra;
        return !(input >> extra);
    }

    Command parse_command(const std::string &line)
    {
        Command command;
        command.type = CommandType::INVALID;

        std::istringstream input(line);
        std::string operation, value;

        if (!(input >> operation))
            return command;

        if (operation == "SUB")
        {
            command.type = CommandType::SUB;

            if (!(input >> command.user >> command.topic))
                return Command{};

            if (input >> command.option)
            {
                if (!(input >> value) || !no_more_words(input))
                    return Command{};

                if (command.option == "below")
                {
                    if (!parse_cents(value, command.price_cents))
                        return Command{};
                }
                else if (command.option == "at")
                {
                    if (!parse_clock(value, command.at_minute))
                        return Command{};
                }
                else if (command.option == "every")
                {
                    if (!parse_whole(value, command.every_hours) || command.every_hours == 0)
                        return Command{};
                }
                else
                {
                    return Command{};
                }

                command.typed = command.option + " " + value;
            }
        }
        else if (operation == "UNSUB")
        {
            command.type = CommandType::UNSUB;

            if (!(input >> command.user >> command.topic) || !no_more_words(input))
                return Command{};
        }
        else if (operation == "PUB")
        {
            command.type = CommandType::PUB;

            if (!(input >> command.topic))
                return Command{};

            std::getline(input, command.message);

            if (!command.message.empty() && command.message[0] == ' ')
                command.message.erase(0, 1);

            if (command.message.empty())
                return Command{};
        }
        else if (operation == "PRICE")
        {
            command.type = CommandType::PRICE;

            if (!(input >> command.topic >> value) || !no_more_words(input) ||
                !parse_cents(value, command.price_cents))
                return Command{};
        }
        else if (operation == "TICK")
        {
            command.type = CommandType::TICK;

            if (!(input >> value) || !no_more_words(input) ||
                !parse_whole(value, command.tick_minutes))
                return Command{};
        }
        else if (operation == "INBOX" || operation == "HISTORY")
        {
            command.type = (operation == "INBOX")
                               ? CommandType::INBOX
                               : CommandType::HISTORY;

            if (!(input >> command.user))
                return Command{};

            if (input >> value)
            {
                if (!no_more_words(input) || !parse_whole(value, command.count))
                    return Command{};

                command.has_count = true;
            }
        }
        else if (operation == "STATS")
        {
            command.type = CommandType::STATS;

            if (!no_more_words(input))
                return Command{};
        }
        else
        {
            command.type = CommandType::UNKNOWN;
        }

        return command;
    }
}
