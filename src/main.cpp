#include <fstream>
#include <iostream>
#include <string>

#include "../include/team/parser.h"
#include "../include/team/app.h"

using namespace alert_service;

void run_commands(std::istream &in, AlertService &service, bool quiet);

int main(int argc, char *argv[])
{
    bool quiet = false;
    bool use_alt = false;
    bool use_alt_topics = false;
    long long inbox_size = 10;
    std::string file_name;

    // loop to check terminal input for app launch
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--quiet")
            quiet = true;
        else if (arg == "--impl=main")
            use_alt = false;
        else if (arg == "--impl=alt")
            use_alt = true;
        else if (arg == "--topics=main")
            use_alt_topics = false;
        else if (arg == "--topics=alt")
            use_alt_topics = true;

        else if (arg == "--inbox" && i + 1 < argc &&
                 parse_whole(argv[i + 1], inbox_size) && inbox_size > 0)
            ++i; // skip K so it isn't taken as a file name
        else if (file_name.empty() && arg[0] != '-')
            file_name = arg;
        else
        {
            std::cerr << "usage: ./app [--quiet] [--inbox K] [--impl=main|alt] [--topics=main|alt] [file]\n";
            return 1;
        }
    }

    // start the alert service
    AlertService service(quiet, use_alt, use_alt_topics, inbox_size);

    if (file_name.empty())
        run_commands(std::cin, service, quiet);
    else
    {
        std::ifstream fp(file_name);
        if (!fp)
        {
            std::cerr << "app: cannot open " << file_name << std::endl;
            return 1;
        }
        run_commands(fp, service, quiet);
    }

    return 0;
}

void run_commands(std::istream &in, AlertService &service, bool quiet)
{
    std::string line;

    while (std::getline(in, line))
    {
        // Windows files end lines with "\r\n"; getline keeps the '\r'
        if (!line.empty() && line[line.length() - 1] == '\r')
            line.erase(line.length() - 1);

        // blank and # lines are not commands and must not print errors
        if (line.find_first_not_of(" \t") == std::string::npos || line[0] == '#')
            continue;

        Command command = parse_command(line);

        switch (command.type)
        {
        case CommandType::SUB:
            service.subscribe(
                command.user,
                command.topic,
                command.option,
                command.typed,
                command.price_cents,
                command.at_minute,
                command.every_hours);
            break;

        case CommandType::UNSUB:
            service.unsubscribe(command.user, command.topic);
            break;

        case CommandType::PUB:
            service.publish(command.topic, command.message);
            break;

        case CommandType::PRICE:
            service.set_price(command.topic, command.price_cents);
            break;

        case CommandType::TICK:
            service.tick(command.tick_minutes);
            break;

        case CommandType::INBOX:
            service.print_inbox(command.user, command.has_count, command.count);
            break;

        case CommandType::HISTORY:
            service.print_history(command.user, command.has_count, command.count);
            break;

        case CommandType::STATS:
            service.print_stats();
            break;

        case CommandType::UNKNOWN:
            if (!quiet)
                std::cout << "ERROR unknown command\n";
            break;

        case CommandType::INVALID:
            if (!quiet)
                std::cout << "ERROR bad command\n";
            break;
        }
    }
}