
#include "alert_service.h"

#include <iostream>

namespace alert_service
{

AlertService::AlertService()
 : current_minute(0),
   next_schedule_order(0)
{
}

// Check whether a subscription matches a published topic.
// Example: "news" matches "news/sports".
// Example: "news/sports" does not match "news".
bool AlertService::topic_matches(
    const std::string& subscribed_topic,
    const std::string& published_topic) const
{
    if (subscribed_topic == published_topic)
        return true;

    if (published_topic.size() <= subscribed_topic.size())
        return false;

    if (published_topic.compare(
            0, subscribed_topic.size(), subscribed_topic) != 0)
        return false;

    return published_topic[subscribed_topic.size()] == '/';
}

// Add a subscription.
// If the user already subscribed to the exact topic,
// replace the existing subscription instead of adding a duplicate.

void AlertService::subscribe(
    const std::string& user,
    const std::string& topic,
    bool has_price,
    double threshold,
    const std::string& schedule_type,
    const std::string& schedule_value)
{
    node<Subscription>* current = subscriptions.get_head();

    while (current != NULL)
    {
        Subscription existing = current->data();

        if (existing.user == user && existing.topic == topic)
        {
            existing.has_price_threshold = has_price;
            existing.price_threshold = threshold;
            existing.schedule_type = schedule_type;
            existing.schedule_value = schedule_value;
            existing.next_delivery_minute = -1;

            current->set_data(existing);

            std::cout << user << " updated subscription to "
                      << topic << '\n';
            return;
        }

        current = current->link();
    }

    Subscription new_subscription;
    new_subscription.user = user;
    new_subscription.topic = topic;
    new_subscription.has_price_threshold = has_price;
    new_subscription.price_threshold = threshold;
    new_subscription.schedule_type = schedule_type;
    new_subscription.schedule_value = schedule_value;

    subscriptions.head_insert(new_subscription);

    std::cout << user << " subscribed to " << topic << '\n';
}
    
// Remove only the exact user/topic subscription.
void AlertService::unsubscribe(
    const std::string& user,
    const std::string& topic)
{
    node<Subscription>* current = subscriptions.get_head();
    node<Subscription>* previous = NULL;

    while (current != NULL)
    {
        Subscription existing = current->data();

        if (existing.user == user && existing.topic == topic)
        {
            if (previous == NULL)
                subscriptions.head_remove();
            else
                subscriptions.remove(previous);

            std::cout << user << " unsubscribed from "
                      << topic << '\n';
            return;
        }

        previous = current;
        current = current->link();
    }

    std::cout << "Subscription not found.\n";
}

// Publish a message to matching subscribers.
// A user receives at most one notification for each publication.
void AlertService::publish(
    const std::string& topic,
    const std::string& message)
{
    linked_list<std::string> notified_users;

    node<Subscription>* current = subscriptions.get_head();

    while (current != NULL)
    {
        Subscription subscription = current->data();

        if (topic_matches(subscription.topic, topic))
        {
            // Check whether this user was already notified.
            bool already_notified = false;

            node<std::string>* notified =
                notified_users.get_head();

            while (notified != NULL)
            {
                if (notified->data() == subscription.user)
                {
                    already_notified = true;
                    break;
                }

                notified = notified->link();
            }

            if (!already_notified)
            {
                Alert new_alert;
                new_alert.user = subscription.user;
                new_alert.topic = topic;
                new_alert.message = message;

                alerts.head_insert(new_alert);
                notified_users.head_insert(subscription.user);

                std::cout << "Alert sent to "
                          << subscription.user << '\n';
            }
        }

        current = current->link();
    }
}

// Display the user's inbox.
// Alerts are stored newest first.
void AlertService::print_inbox(const std::string& user) const
{
    std::cout << "\nInbox for " << user << ":\n";

    const node<Alert>* current = alerts.get_head();
    bool found = false;

    while (current != NULL)
    {
        Alert alert = current->data();

        if (alert.user == user)
        {
            std::cout << "[" << alert.topic << "] "
                      << alert.message << '\n';
            found = true;
        }

        current = current->link();
    }

    if (!found)
        std::cout << "No alerts.\n";
}

// Display the user's full notification history.
// For now, history contains the same stored alerts as the inbox.
void AlertService::print_history(const std::string& user) const
{
    std::cout << "\nHistory for " << user << ":\n";

    const node<Alert>* current = alerts.get_head();
    bool found = false;

    while (current != NULL)
    {
        Alert alert = current->data();

        if (alert.user == user)
        {
            std::cout << "[" << alert.topic << "] "
                      << alert.message << '\n';
            found = true;
        }

        current = current->link();
    }

    if (!found)
        std::cout << "No history.\n";
}

// Display basic linked-list statistics.
void AlertService::print_stats() const
{
    std::cout << "\nSubscription count: "
              << subscriptions.length() << '\n';

    std::cout << "Alert count: "
              << alerts.length() << '\n';
}


void AlertService::set_price(
    const std::string& item,
    double new_price)
{
    std::string topic = "items/" + item;

    node<Subscription>* current = subscriptions.get_head();
    node<Subscription>* previous = NULL;

    while (current != NULL)
    {
        Subscription subscription = current->data();

        if (subscription.topic == topic &&
            subscription.has_price_threshold &&
            new_price < subscription.price_threshold)
        {
            Alert new_alert;
            new_alert.user = subscription.user;
            new_alert.topic = topic;
            new_alert.message =
                "Price dropped to $" + std::to_string(new_price);

            alerts.head_insert(new_alert);

            std::cout << "Price alert sent to "
                      << subscription.user << '\n';

            if (previous == NULL)
                subscriptions.head_remove();
            else
                subscriptions.remove(previous);

            current = (previous == NULL)
                ? subscriptions.get_head()
                : previous->link();
        }
        else
        {
            previous = current;
            current = current->link();
        }
    }
}

void AlertService::tick(int minutes)
{
    if (minutes < 0)
    {
        std::cout << "TICK cannot be negative.\n";
        return;
    }

    current_minute += minutes;

    long long day = current_minute / 1440 + 1;
    long long hour = (current_minute % 1440) / 60;
    long long minute = current_minute % 60;

    std::cout << "Clock: Day " << day << " ";

    if (hour < 10)
        std::cout << "0";

    std::cout << hour << ":";

    if (minute < 10)
        std::cout << "0";

    std::cout << minute << '\n';
}

}
