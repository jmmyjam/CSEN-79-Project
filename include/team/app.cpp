#include "parser.h"
#include <map>
#include <vector>
#include <algorithm>

#include "app.h"

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
        const std::string &subscribed_topic,
        const std::string &published_topic) const
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
        const std::string &user,
        const std::string &topic,
        bool has_price,
        double threshold,
        const std::string &schedule_type,
        const std::string &schedule_value)
    {

        node<Subscription> *current = subscriptions.get_head();
        while (current != NULL)
        {
            Subscription existing = current->data();
            if (existing.user == user && existing.topic == topic)
            {
                std::cout << "ERROR already subscribed " << user << ' ' << topic << std::endl;
                return;
            }

            current = current->link();
        }

        long long first_due = -1;

        if (schedule_type == "at")
        {
            // The CLI passes "at 08:00", direct callers can pass "08:00".
            const std::string time_text = schedule_value.compare(0, 3, "at ") == 0
                ? schedule_value.substr(3)
                : schedule_value;

            long long minute_of_day = 0;
            if (!parse_clock(time_text, minute_of_day))
            {
                std::cout << "ERROR bad command\n";
                return;
            }

            first_due = current_minute - current_minute % 1440 + minute_of_day;
            if (first_due <= current_minute)
                first_due += 1440;
        }

        Subscription new_subscription;
        new_subscription.user = user;
        new_subscription.topic = topic;
        new_subscription.has_price_threshold = has_price;
        new_subscription.price_threshold = threshold;
        new_subscription.schedule_type = schedule_type;
        new_subscription.schedule_value = schedule_value;
        new_subscription.next_delivery_minute = first_due;
        new_subscription.schedule_order = next_schedule_order++;

        subscriptions.head_insert(new_subscription);

        std::cout << "OK SUB " << user << ' ' << topic << '\n';
    }

    // Remove only the exact user/topic subscription.
    void AlertService::unsubscribe(
        const std::string &user,
        const std::string &topic)
    {
        // Find the exact subscription and keep its predecessor for removal.
        node<Subscription> *previous = nullptr;
        auto *current = subscriptions.get_head();

        while (current != nullptr)
        {
            const Subscription sub = current->data();
            if (sub.user == user && sub.topic == topic)
                break;

            previous = current;
            current = current->link();
        }

        if (current == nullptr)
        {
            std::cout << "ERROR not subscribed " << user << ' ' << topic << '\n';
            return;
        }

        if (previous == nullptr)
            subscriptions.head_remove();
        else
            subscriptions.remove(previous);

        // Cancel publications held by this subscription. The published topic
        // may be a child path, so match against subscription_topic.
        node<PendingEvent> *previous_event = nullptr;
        auto *event_node = pending_events.get_head();

        while (event_node != nullptr)
        {
            const PendingEvent event = event_node->data();

            // Save the next node before removal, which deletes event_node.
            auto *next = event_node->link();

            if (event.user == user && event.subscription_topic == topic)
            {
                if (previous_event == nullptr)
                    pending_events.head_remove();
                else
                    pending_events.remove(previous_event);

                // Keep the predecessor in place for consecutive removals.
            }
            else
            {
                previous_event = event_node;
            }

            event_node = next;
        }

        std::cout << "OK UNSUB " << user << ' ' << topic << '\n';
    }

    // Publish a message to matching subscribers.
    // A user receives at most one notification for each publication.
    void AlertService::publish(
        const std::string &topic,
        const std::string &message)
    {
        // Select the most specific subscription that accepts publications (i.e the most concrete/longest topic path)
        std::map<std::string, Subscription> most_specific_by_user;

        for (auto *current = subscriptions.get_head(); current != nullptr; current = current->link())
        {
            const Subscription sub = current->data();

            if (sub.has_price_threshold || !topic_matches(sub.topic, topic))
                continue;

            auto selected = most_specific_by_user.find(sub.user);
            if (selected == most_specific_by_user.end() ||
                sub.topic.size() > selected->second.topic.size())
            {
                most_specific_by_user.insert_or_assign(sub.user, sub);
            }
        }


        // Sort the publish matches by their schedule order for alerts in right order
        std::vector<Subscription> matches;
        matches.reserve(most_specific_by_user.size());

        for (const auto &entry : most_specific_by_user)
            matches.push_back(entry.second);

        std::sort(matches.begin(), matches.end(),
                [](const Subscription &a, const Subscription &b) {
                    return a.schedule_order < b.schedule_order;
                });


        // Postpone matched publication alert to PendingEvent for "at" and "every" options
        for (const Subscription &sub : matches)
        {
            if (sub.schedule_type == "at" || sub.schedule_type == "every")
            {
                const PendingEvent event{sub.user, sub.topic, topic, message};

                auto *tail = pending_events.get_tail();
                if (tail != nullptr)
                    pending_events.insert(tail, event);
                else
                    pending_events.head_insert(event);

                continue;
            }


            // Else just create and send alert right away
            Alert alert;
            alert.minute = current_minute;
            alert.user = sub.user;
            alert.topic = topic;
            alert.message = message;
            alerts.head_insert(alert);

            std::cout << "ALERT " << format_time(alert.minute) << ' '
                << alert.user << ' ' << alert.topic << ' '
                << alert.message << '\n';
        }
    }

    // Display the user's inbox.
    // Alerts are stored newest first.
    void AlertService::print_inbox(const std::string &user) const
    {
        std::cout << "\nInbox for " << user << ":\n";

        const node<Alert> *current = alerts.get_head();
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
    void AlertService::print_history(const std::string &user) const
    {
        std::cout << "\nHistory for " << user << ":\n";

        const node<Alert> *current = alerts.get_head();
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
        const std::string &item,
        double new_price)
    {
        std::string topic = "items/" + item;

        node<Subscription> *current = subscriptions.get_head();
        node<Subscription> *previous = NULL;

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

    void AlertService::tick(long long minutes)
    {
        if (minutes < 0)
        {
            std::cout << "TICK cannot be negative.\n";
            return;
        }

        
        const long long new_time = current_minute + minutes;

        
        using SubscriptionKey = std::pair<std::string, std::string>;

        struct DeliveryTime
        {
            long long minute;
            long long order;
        };

        std::map<SubscriptionKey, DeliveryTime> due;


        for (auto *current = subscriptions.get_head(); current != nullptr; current = current->link())
        {
            Subscription sub = current->data();
            if (sub.schedule_type != "at" || sub.next_delivery_minute > new_time)
                continue;

            due.emplace(SubscriptionKey{sub.user, sub.topic},
                        DeliveryTime{sub.next_delivery_minute, sub.schedule_order});

            const long long days = (new_time - sub.next_delivery_minute) / 1440 + 1;
            sub.next_delivery_minute += days * 1440;
            current->set_data(sub);
        }

        struct Delivery
        {
            DeliveryTime time;
            PendingEvent event;
        };
        std::vector<Delivery> deliveries;

        if (!due.empty())
        {
            node<PendingEvent> *previous = nullptr;
            auto *current = pending_events.get_head();

            while (current)
            {
                const PendingEvent event = current->data();
                auto *next = current->link();
                const auto match = due.find({event.user, event.subscription_topic});

                if (match != due.end())
                {
                    deliveries.push_back({match->second, event});
                    if (previous)
                        pending_events.remove(previous);
                    else
                        pending_events.head_remove();
                }
                else
                {
                    previous = current;
                }

                current = next;
            }
        }

        // Equal keys are publications for the same subscription. Stable sorting keeps those publications in their original order
        std::stable_sort(deliveries.begin(), deliveries.end(),
                        [](const Delivery &a, const Delivery &b) {
                            if (a.time.minute != b.time.minute)
                                return a.time.minute < b.time.minute;
                            return a.time.order < b.time.order;
                        });

        for (const Delivery &delivery : deliveries)
        {
            const Alert alert{delivery.time.minute,
                            delivery.event.user,
                            delivery.event.published_topic,
                            delivery.event.message};

            alerts.head_insert(alert);
            std::cout << "ALERT " << format_time(alert.minute) << ' '
                    << alert.user << ' ' << alert.topic << ' '
                    << alert.message << '\n';
        }


        current_minute = new_time;
        std::cout << "TIME " << format_time(current_minute) << '\n';
    }

}
