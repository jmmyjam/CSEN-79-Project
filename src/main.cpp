
#include <iostream>
#include <string>

#include "parser.h"
#include "alert_service.h"

int main()
{
    std::string line;

    alert_service::AlertService service;

    std::cout << "Subscription Alert Service\n";
    std::cout << "Enter commands (type EXIT to quit).\n";

    while (std::getline(std::cin, line))
    {
        if (line == "EXIT")
            break;

        alert_service::Command command =
            alert_service::parse_command(line);

        switch (command.type)
        {
        case alert_service::CommandType::SUB:
           service.subscribe(
           command.user,
           command.topic,
           command.has_price,
           command.price);
            break;

        case alert_service::CommandType::UNSUB:
            service.unsubscribe(command.user, command.topic);
            break;

        case alert_service::CommandType::PUB:
            service.publish(command.topic, command.message);
            break;

        case alert_service::CommandType::INBOX:
            service.print_inbox(command.user);
            break;

        case alert_service::CommandType::HISTORY:
            service.print_history(command.user);
            break;

        case alert_service::CommandType::STATS:
            service.print_stats();
            break;

       case alert_service::CommandType::PRICE:
           service.set_price(command.item, command.price);
            break;
        default:
            std::cout << "Command not implemented yet.\n";
            break;
        }
    }

    return 0;
}

