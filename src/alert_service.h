
#ifndef ALERT_SERVICE_H
#define ALERT_SERVICE_H

#include <string>
#include <cstddef>

#include "../data_structures/linked_list/linked_list.h"

namespace alert_service
{

struct Subscription
{
    std::string user;
    std::string topic;

    bool operator==(const Subscription& other) const
    {
        return user == other.user && topic == other.topic;
    }
};

struct Alert
{
    std::string user;
    std::string topic;
    std::string message;

    bool operator==(const Alert& other) const
    {
        return user == other.user &&
               topic == other.topic &&
               message == other.message;
    }
};

class AlertService
{
public:
    AlertService();

    void subscribe(const std::string& user,
                   const std::string& topic);

    void unsubscribe(const std::string& user,
                     const std::string& topic);

    void publish(const std::string& topic,
                 const std::string& message);

    void print_inbox(const std::string& user) const;

    void print_history(const std::string& user) const;

    void print_stats() const;

private:
    linked_list<Subscription> subscriptions;
    linked_list<Alert> alerts;

    bool topic_matches(const std::string& subscribed_topic,
                       const std::string& published_topic) const;
};

}

#endif
