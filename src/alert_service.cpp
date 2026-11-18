
#include "alert_service.h"

#include <iostream>

namespace alert_service
{

AlertService::AlertService()
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
    const std::string& topic)
{
    node<Subscription>* current = subscriptions.get_head();

    while (current != NULL)
    {
        Subscription existing = current->data();

        if (existing.user == user && existing.topic == topic)
        {
            std::cout << user << " updated subscription to "
                      << topic << '\n';
            return;
        }

        current = current->link();
    }

    Subscription new_subscription;
    new_subscription.user = user;
    new_subscription.topic = topic;

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

}
