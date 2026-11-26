//Quality Lead - Alert Service Unit Tests

#include "doctest/doctest.h"
#include "../include/team/app.h"

#include <iostream>
#include <sstream>
#include <string>

using namespace alert_service;

// Helper class to capture output from std::cout.
class OutputCapture
{
private:
    std::ostringstream output;
    std::streambuf* original;

public:
    OutputCapture()
        : original(std::cout.rdbuf(output.rdbuf()))
    {
    }

    ~OutputCapture()
    {
        std::cout.rdbuf(original);
    }

    std::string str() const
    {
        return output.str();
    }

    void clear()
    {
        output.str("");
        output.clear();
    }
};

// Helper function to check whether output contains text.
static bool contains_text(const std::string& output,
                          const std::string& text)
{
    return output.find(text) != std::string::npos;
}

// Helper function to count occurrences of a message.
static size_t count_occurrences(const std::string& output,
                                const std::string& text)
{
    size_t count = 0;
    size_t position = 0;

    while ((position = output.find(text, position))
           != std::string::npos)
    {
        ++count;
        position += text.length();
    }

    return count;
}

// --------------------------------------------------
// SUBSCRIPTION TESTS
// --------------------------------------------------

TEST_CASE("AlertService starts empty")
{
    OutputCapture capture;
    AlertService service;

    service.print_stats();

    CHECK(contains_text(capture.str(),
                        "Subscription count: 0"));

    CHECK(contains_text(capture.str(),
                        "Alert count: 0"));
}

TEST_CASE("AlertService adds a subscription")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news");

    capture.clear();
    service.print_stats();

    CHECK(contains_text(capture.str(),
                        "Subscription count: 1"));
}

TEST_CASE("AlertService replaces duplicate subscriptions")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news");
    service.subscribe("alice", "news");

    capture.clear();
    service.print_stats();

    CHECK(contains_text(capture.str(),
                        "Subscription count: 1"));
}

TEST_CASE("AlertService supports multiple subscribers")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news");
    service.subscribe("bob", "news");

    capture.clear();
    service.print_stats();

    CHECK(contains_text(capture.str(),
                        "Subscription count: 2"));
}

// --------------------------------------------------
// PUBLISH AND DELIVERY TESTS
// --------------------------------------------------

TEST_CASE("AlertService delivers an immediate alert")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news");
    service.publish("news", "Breaking news");

    capture.clear();
    service.print_inbox("alice");

    CHECK(contains_text(capture.str(), "Breaking news"));
}

TEST_CASE("AlertService matches hierarchical topics")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news");
    service.publish("news/sports/soccer", "Soccer update");

    capture.clear();
    service.print_inbox("alice");

    CHECK(contains_text(capture.str(), "Soccer update"));
}

TEST_CASE("AlertService does not match unrelated topics")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news");
    service.publish("weather", "Rain tomorrow");

    capture.clear();
    service.print_inbox("alice");

    CHECK_FALSE(contains_text(capture.str(), "Rain tomorrow"));
}

TEST_CASE("AlertService does not match partial topic names")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news");
    service.publish("newsletter", "Weekly update");

    capture.clear();
    service.print_inbox("alice");

    CHECK_FALSE(contains_text(capture.str(), "Weekly update"));
}

TEST_CASE("Child subscription does not receive parent events")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news/sports");
    service.publish("news", "General news");

    capture.clear();
    service.print_inbox("alice");

    CHECK_FALSE(contains_text(capture.str(), "General news"));
}

TEST_CASE("AlertService prevents duplicate alerts")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news");
    service.subscribe("alice", "news/sports");
    service.subscribe("alice", "news/sports/soccer");

    service.publish("news/sports/soccer", "Goal scored");

    capture.clear();
    service.print_inbox("alice");

    CHECK(count_occurrences(capture.str(), "Goal scored") == 1);
}

TEST_CASE("AlertService delivers events to multiple users")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news");
    service.subscribe("bob", "news");

    service.publish("news", "Important update");

    capture.clear();
    service.print_inbox("alice");

    CHECK(contains_text(capture.str(), "Important update"));

    capture.clear();
    service.print_inbox("bob");

    CHECK(contains_text(capture.str(), "Important update"));
}

TEST_CASE("Publishing without subscribers produces no alerts")
{
    OutputCapture capture;
    AlertService service;

    service.publish("news", "Breaking news");

    CHECK(capture.str().empty());

    capture.clear();
    service.print_stats();

    CHECK(contains_text(capture.str(), "Alert count: 0"));
}

// --------------------------------------------------
// UNSUBSCRIBE TESTS
// --------------------------------------------------

TEST_CASE("AlertService removes a subscription")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news");
    service.unsubscribe("alice", "news");

    capture.clear();
    service.print_stats();

    CHECK(contains_text(capture.str(),
                        "Subscription count: 0"));
}

TEST_CASE("Unsubscribed user receives no new alerts")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news");
    service.unsubscribe("alice", "news");

    service.publish("news", "New message");

    capture.clear();
    service.print_inbox("alice");

    CHECK_FALSE(contains_text(capture.str(), "New message"));
}

TEST_CASE("Unsubscribe removes only the exact topic")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news");
    service.subscribe("alice", "news/sports");

    service.unsubscribe("alice", "news");

    service.publish("news", "General update");
    service.publish("news/sports", "Sports update");

    capture.clear();
    service.print_inbox("alice");

    CHECK_FALSE(contains_text(capture.str(), "General update"));
    CHECK(contains_text(capture.str(), "Sports update"));
}

TEST_CASE("Unsubscribing one user does not affect another")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news");
    service.subscribe("bob", "news");

    service.unsubscribe("alice", "news");
    service.publish("news", "News update");

    capture.clear();
    service.print_inbox("alice");

    CHECK_FALSE(contains_text(capture.str(), "News update"));

    capture.clear();
    service.print_inbox("bob");

    CHECK(contains_text(capture.str(), "News update"));
}

TEST_CASE("Unsubscribe handles nonexistent subscriptions")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news");
    service.unsubscribe("bob", "weather");

    capture.clear();
    service.print_stats();

    CHECK(contains_text(capture.str(),
                        "Subscription count: 1"));
}

// --------------------------------------------------
// PRICE ALERT TESTS
// --------------------------------------------------

TEST_CASE("Price below threshold triggers an alert")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "items/phone", true, 600.0);
    service.set_price("phone", 550.0);

    capture.clear();
    service.print_inbox("alice");

    CHECK(contains_text(capture.str(), "Price dropped"));
}

TEST_CASE("Price equal to threshold does not trigger an alert")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "items/phone", true, 600.0);

    capture.clear();
    service.set_price("phone", 600.0);

    CHECK(capture.str().empty());

    capture.clear();
    service.print_inbox("alice");

    CHECK_FALSE(contains_text(capture.str(), "Price dropped"));
}

TEST_CASE("Price above threshold does not trigger an alert")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "items/phone", true, 600.0);
    service.set_price("phone", 650.0);

    capture.clear();
    service.print_inbox("alice");

    CHECK_FALSE(contains_text(capture.str(), "Price dropped"));
}

TEST_CASE("Price alert fires only once")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "items/phone", true, 600.0);

    service.set_price("phone", 550.0);
    service.set_price("phone", 500.0);

    capture.clear();
    service.print_inbox("alice");

    CHECK(count_occurrences(capture.str(), "Price dropped") == 1);

    capture.clear();
    service.print_stats();

    CHECK(contains_text(capture.str(),
                        "Subscription count: 0"));
}

TEST_CASE("Price alerts notify only eligible users")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "items/phone", true, 600.0);
    service.subscribe("bob", "items/phone", true, 500.0);

    service.set_price("phone", 550.0);

    capture.clear();
    service.print_inbox("alice");

    CHECK(contains_text(capture.str(), "Price dropped"));

    capture.clear();
    service.print_inbox("bob");

    CHECK_FALSE(contains_text(capture.str(), "Price dropped"));

    service.set_price("phone", 450.0);

    capture.clear();
    service.print_inbox("bob");

    CHECK(contains_text(capture.str(), "Price dropped"));
}

TEST_CASE("Price updates do not affect unrelated items")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "items/phone", true, 600.0);
    service.set_price("laptop", 400.0);

    capture.clear();
    service.print_inbox("alice");

    CHECK_FALSE(contains_text(capture.str(), "Price dropped"));
}

TEST_CASE("Replacing subscription updates price threshold")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "items/phone", true, 600.0);
    service.subscribe("alice", "items/phone", true, 500.0);

    service.set_price("phone", 550.0);

    capture.clear();
    service.print_inbox("alice");

    CHECK_FALSE(contains_text(capture.str(), "Price dropped"));

    service.set_price("phone", 450.0);

    capture.clear();
    service.print_inbox("alice");

    CHECK(contains_text(capture.str(), "Price dropped"));
}

// --------------------------------------------------
// SIMULATED CLOCK TESTS
// --------------------------------------------------

TEST_CASE("Simulated clock starts at Day 1")
{
    OutputCapture capture;
    AlertService service;

    service.tick(0);

    CHECK(contains_text(capture.str(), "Day 1 00:00"));
}

TEST_CASE("Simulated clock advances correctly")
{
    OutputCapture capture;
    AlertService service;

    service.tick(480);

    CHECK(contains_text(capture.str(), "Day 1 08:00"));
}

TEST_CASE("Simulated clock rolls over to next day")
{
    OutputCapture capture;
    AlertService service;

    service.tick(1440);

    CHECK(contains_text(capture.str(), "Day 2 00:00"));
}

TEST_CASE("Simulated clock supports multiple ticks")
{
    OutputCapture capture;
    AlertService service;

    service.tick(60);
    service.tick(30);

    CHECK(contains_text(capture.str(), "Day 1 01:30"));
}

TEST_CASE("Simulated clock rejects negative ticks")
{
    OutputCapture capture;
    AlertService service;

    service.tick(60);
    service.tick(-30);

    capture.clear();
    service.tick(0);

    CHECK(contains_text(capture.str(), "Day 1 01:00"));
}

// --------------------------------------------------
// INBOX TESTS
// --------------------------------------------------

TEST_CASE("Empty inbox contains no alerts")
{
    OutputCapture capture;
    AlertService service;

    service.print_inbox("alice");

    CHECK(contains_text(capture.str(), "No alerts."));
}

TEST_CASE("Inbox displays alerts newest first")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news");

    service.publish("news", "First message");
    service.publish("news", "Second message");

    capture.clear();
    service.print_inbox("alice");

    std::string output = capture.str();

    REQUIRE(contains_text(output, "First message"));
    REQUIRE(contains_text(output, "Second message"));

    CHECK(output.find("Second message") <
          output.find("First message"));
}

// --------------------------------------------------
// REQUIREMENT-BASED TESTS
// These may fail until application development
// is completed.
// --------------------------------------------------

TEST_CASE("Daily scheduled alert waits until delivery time")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "horoscope/leo",
                      false, 0.0, "at", "08:00");

    service.publish("horoscope/leo", "Daily horoscope");

    capture.clear();
    service.print_inbox("alice");

    CHECK_FALSE(contains_text(capture.str(), "Daily horoscope"));

    service.tick(479);

    capture.clear();
    service.print_inbox("alice");

    CHECK_FALSE(contains_text(capture.str(), "Daily horoscope"));

    service.tick(1);

    capture.clear();
    service.print_inbox("alice");

    CHECK(contains_text(capture.str(), "Daily horoscope"));
}

TEST_CASE("Recurring alert waits for its scheduled interval")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news",
                      false, 0.0, "every", "6");

    service.publish("news", "Scheduled news");

    service.tick(359);

    capture.clear();
    service.print_inbox("alice");

    CHECK_FALSE(contains_text(capture.str(), "Scheduled news"));

    service.tick(1);

    capture.clear();
    service.print_inbox("alice");

    CHECK(contains_text(capture.str(), "Scheduled news"));
}

TEST_CASE("Unsubscribe cancels pending scheduled alerts")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news",
                      false, 0.0, "every", "6");

    service.publish("news", "Pending update");
    service.unsubscribe("alice", "news");

    service.tick(360);

    capture.clear();
    service.print_inbox("alice");

    CHECK_FALSE(contains_text(capture.str(), "Pending update"));
}

TEST_CASE("Most specific subscription determines delivery time")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news");

    service.subscribe("alice", "news/sports",
                      false, 0.0, "every", "6");

    service.publish("news/sports", "Sports update");

    capture.clear();
    service.print_inbox("alice");

    CHECK_FALSE(contains_text(capture.str(), "Sports update"));

    service.tick(360);

    capture.clear();
    service.print_inbox("alice");

    CHECK(count_occurrences(capture.str(), "Sports update") == 1);
}

TEST_CASE("History displays alerts oldest first")
{
    OutputCapture capture;
    AlertService service;

    service.subscribe("alice", "news");

    service.publish("news", "First event");
    service.publish("news", "Second event");

    capture.clear();
    service.print_history("alice");

    std::string output = capture.str();

    REQUIRE(contains_text(output, "First event"));
    REQUIRE(contains_text(output, "Second event"));

    CHECK(output.find("First event") <
          output.find("Second event"));
}
