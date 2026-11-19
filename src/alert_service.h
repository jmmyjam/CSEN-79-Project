
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

    bool has_price_threshold = false;
    double price_threshold = 0.0;

    // Schedule: "", "at", or "every"
    std::string schedule_type;
    std::string schedule_value;

    // Simulated minute when the next delivery is due.
    long long next_delivery_minute = -1;
    long long schedule_order = 0;

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

struct PendingEvent
{
    std::string user;
    std::string subscription_topic;
    std::string published_topic;
    std::string message;

    bool operator==(const PendingEvent& other) const
    {
        return user == other.user &&
               subscription_topic == other.subscription_topic &&
               published_topic == other.published_topic &&
               message == other.message;
    }
};

class AlertService
{
public:
    AlertService();

    void subscribe(const std::string& user,
                   const std::string& topic,
                   bool has_price = false,
                   double threshold = 0.0,
                   const std::string& schedule_type = "",
                   const std::string& schedule_value = "");

    void unsubscribe(const std::string& user,
                     const std::string& topic);

    void publish(const std::string& topic,
                 const std::string& message);

    void set_price(const std::string& item, double new_price);

    void tick(int minutes);

    void print_inbox(const std::string& user) const;
    void print_history(const std::string& user) const;
    void print_stats() const;

private:
    linked_list<Subscription> subscriptions;
    linked_list<Alert> alerts;
    linked_list<PendingEvent> pending_events;

    long long current_minute;
    long long next_schedule_order;

    bool topic_matches(const std::string& subscribed_topic,
                       const std::string& published_topic) const;
};

}

#endif

