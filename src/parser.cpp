#include "parser.h"

#include <sstream>
#include <string>

namespace alert_service
{
    Command parse_command(const std::string& line)
    {
        Command command;
        command.type = CommandType::INVALID;

        std::istringstream input(line);
        std::string operation;

        if (!(input >> operation))
            return command;

        if (operation == "SUB")
        {
            command.type = CommandType::SUB;

            if (!(input >> command.user >> command.topic))
                return Command{};

            std::string option;

            while (input >> option)
            {
                if (option == "below")
                {
                    if (!(input >> command.price))
                        return Command{};

                    command.has_price = true;
                }
                else if (option == "at" || option == "every")
                {
                    if (!command.time_option.empty())
                        return Command{};

                    command.time_option = option;

                    if (!(input >> command.message))
                        return Command{};
                }
                else
                {
                    return Command{};
                }
            }
        }
        else if (operation == "UNSUB")
        {
            command.type = CommandType::UNSUB;

            if (!(input >> command.user >> command.topic))
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
        }
        else if (operation == "PRICE")
        {
            command.type = CommandType::PRICE;

            if (!(input >> command.item >> command.price))
                return Command{};
        }
        else if (operation == "TICK")
        {
            command.type = CommandType::TICK;

            if (!(input >> command.number) || command.number < 0)
                return Command{};
        }
        else if (operation == "INBOX" || operation == "HISTORY")
        {
            command.type = (operation == "INBOX")
                ? CommandType::INBOX : CommandType::HISTORY;

            if (!(input >> command.user))
                return Command{};

            if (input >> command.number)
            {
                if (command.number < 0)
                    return Command{};
            }
            else
            {
                command.number = 0;
                input.clear();
            }
        }
        else if (operation == "STATS")
        {
            command.type = CommandType::STATS;
        }

        return command;
    }
}