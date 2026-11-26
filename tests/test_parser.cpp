// Quality Lead - Parser File Unit Tests

#include "doctest/doctest.h"
#include "../src/parser.h"
#include <string>

// Forward declaration because parser.h currently
// does not declare parse_command().
namespace alert_service
{
    Command parse_command(const std::string& line);
}

using namespace alert_service;

// ============================================
// HELPER FUNCTION TESTS
// ============================================

// Test #1: Parse valid whole numbers.
TEST_CASE("Parser valid whole numbers")
{
    long long value = 0;

    REQUIRE(parse_whole("123", value));
    CHECK(value == 123);

    REQUIRE(parse_whole("0", value));
    CHECK(value == 0);

    REQUIRE(parse_whole("0012", value));
    CHECK(value == 12);

    REQUIRE(parse_whole("999999999999999", value));
    CHECK(value == 999999999999999LL);
}

// Test #2: Reject invalid whole numbers.
TEST_CASE("Parser invalid whole numbers")
{
    long long value = 0;

    CHECK_FALSE(parse_whole("", value));
    CHECK_FALSE(parse_whole("-5", value));
    CHECK_FALSE(parse_whole("+5", value));
    CHECK_FALSE(parse_whole("12abc", value));
    CHECK_FALSE(parse_whole("abc", value));
    CHECK_FALSE(parse_whole("12.5", value));
    CHECK_FALSE(parse_whole("1 2", value));
    CHECK_FALSE(parse_whole("1234567890123456", value));
}

// Test #3: Convert valid prices into cents.
TEST_CASE("Parser valid prices")
{
    long long cents = 0;

    REQUIRE(parse_cents("12.55", cents));
    CHECK(cents == 1255);

    REQUIRE(parse_cents("12.5", cents));
    CHECK(cents == 1250);

    REQUIRE(parse_cents("12", cents));
    CHECK(cents == 1200);

    REQUIRE(parse_cents("0.01", cents));
    CHECK(cents == 1);

    REQUIRE(parse_cents("0", cents));
    CHECK(cents == 0);

    REQUIRE(parse_cents("10.05", cents));
    CHECK(cents == 1005);
}

// Test #4: Reject invalid prices.
TEST_CASE("Parser invalid prices")
{
    long long cents = 0;

    CHECK_FALSE(parse_cents("", cents));
    CHECK_FALSE(parse_cents("-12.50", cents));
    CHECK_FALSE(parse_cents("12.555", cents));
    CHECK_FALSE(parse_cents("12.", cents));
    CHECK_FALSE(parse_cents(".50", cents));
    CHECK_FALSE(parse_cents("12abc", cents));
    CHECK_FALSE(parse_cents("12.5.5", cents));
    CHECK_FALSE(parse_cents("abc", cents));
}

// Test #5: Convert valid clock times into minutes.
TEST_CASE("Parser valid clock times")
{
    long long minutes = 0;

    REQUIRE(parse_clock("00:00", minutes));
    CHECK(minutes == 0);

    REQUIRE(parse_clock("08:00", minutes));
    CHECK(minutes == 480);

    REQUIRE(parse_clock("12:30", minutes));
    CHECK(minutes == 750);

    REQUIRE(parse_clock("23:59", minutes));
    CHECK(minutes == 1439);
}

// Test #6: Reject invalid clock times.
TEST_CASE("Parser invalid clock times")
{
    long long minutes = 0;

    CHECK_FALSE(parse_clock("24:00", minutes));
    CHECK_FALSE(parse_clock("12:60", minutes));
    CHECK_FALSE(parse_clock("8:00", minutes));
    CHECK_FALSE(parse_clock("12:5", minutes));
    CHECK_FALSE(parse_clock("1200", minutes));
    CHECK_FALSE(parse_clock("ab:cd", minutes));
    CHECK_FALSE(parse_clock("", minutes));
}

// Test #7: Format minutes into day and clock time.
TEST_CASE("Parser time formatting")
{
    CHECK(format_time(0) == "D0 00:00");
    CHECK(format_time(5) == "D0 00:05");
    CHECK(format_time(480) == "D0 08:00");
    CHECK(format_time(1439) == "D0 23:59");
    CHECK(format_time(1440) == "D1 00:00");
    CHECK(format_time(1500) == "D1 01:00");
    CHECK(format_time(2880) == "D2 00:00");
}

// Test #8: Format cents into dollars.
TEST_CASE("Parser price formatting")
{
    CHECK(format_cents(0) == "0.00");
    CHECK(format_cents(1) == "0.01");
    CHECK(format_cents(5) == "0.05");
    CHECK(format_cents(100) == "1.00");
    CHECK(format_cents(1205) == "12.05");
    CHECK(format_cents(1255) == "12.55");
    CHECK(format_cents(1250) == "12.50");
}

// ============================================
// VALID COMMAND TESTS
// ============================================

// Test #9: Parse a regular subscription.
TEST_CASE("Parser regular SUB command")
{
    Command command = parse_command(
        "SUB alice news/sports");

    REQUIRE(command.type == CommandType::SUB);

    CHECK(command.user == "alice");
    CHECK(command.topic == "news/sports");
    CHECK(command.option.empty());
    CHECK(command.typed.empty());
}

// Test #10: Parse a price-based subscription.
TEST_CASE("Parser price-based SUB command")
{
    Command command = parse_command(
        "SUB alice price/laptop below 500.25");

    REQUIRE(command.type == CommandType::SUB);

    CHECK(command.user == "alice");
    CHECK(command.topic == "price/laptop");
    CHECK(command.option == "below");
    CHECK(command.price_cents == 50025);
    CHECK(command.typed == "below 500.25");
}

// Test #11: Parse a daily scheduled subscription.
TEST_CASE("Parser daily SUB command")
{
    Command command = parse_command(
        "SUB bob horoscope/leo at 08:30");

    REQUIRE(command.type == CommandType::SUB);

    CHECK(command.user == "bob");
    CHECK(command.topic == "horoscope/leo");
    CHECK(command.option == "at");
    CHECK(command.at_minute == 510);
    CHECK(command.typed == "at 08:30");
}

// Test #12: Parse a recurring subscription.
TEST_CASE("Parser recurring SUB command")
{
    Command command = parse_command(
        "SUB charlie news every 3");

    REQUIRE(command.type == CommandType::SUB);

    CHECK(command.user == "charlie");
    CHECK(command.topic == "news");
    CHECK(command.option == "every");
    CHECK(command.every_hours == 3);
    CHECK(command.typed == "every 3");
}

// Test #13: Parse an unsubscribe command.
TEST_CASE("Parser UNSUB command")
{
    Command command = parse_command(
        "UNSUB alice news/sports");

    REQUIRE(command.type == CommandType::UNSUB);

    CHECK(command.user == "alice");
    CHECK(command.topic == "news/sports");
}

// Test #14: Parse a published message.
TEST_CASE("Parser PUB command")
{
    SUBCASE("Single-word message")
    {
        Command command = parse_command(
            "PUB news Hello");

        REQUIRE(command.type == CommandType::PUB);

        CHECK(command.topic == "news");
        CHECK(command.message == "Hello");
    }

    SUBCASE("Message containing multiple words")
    {
        Command command = parse_command(
            "PUB news/sports The team won the game");

        REQUIRE(command.type == CommandType::PUB);

        CHECK(command.topic == "news/sports");
        CHECK(command.message == "The team won the game");
    }

    SUBCASE("Message containing punctuation")
    {
        Command command = parse_command(
            "PUB news Breaking news: We won!");

        REQUIRE(command.type == CommandType::PUB);

        CHECK(command.message == "Breaking news: We won!");
    }
}

// Test #15: Parse a price update.
TEST_CASE("Parser PRICE command")
{
    SUBCASE("Decimal price")
    {
        Command command = parse_command(
            "PRICE laptop 499.99");

        REQUIRE(command.type == CommandType::PRICE);

        CHECK(command.topic == "laptop");
        CHECK(command.price_cents == 49999);
    }

    SUBCASE("Whole-number price")
    {
        Command command = parse_command(
            "PRICE laptop 500");

        REQUIRE(command.type == CommandType::PRICE);

        CHECK(command.price_cents == 50000);
    }
}

// Test #16: Parse a clock advancement.
TEST_CASE("Parser TICK command")
{
    SUBCASE("Positive number of minutes")
    {
        Command command = parse_command("TICK 60");

        REQUIRE(command.type == CommandType::TICK);

        CHECK(command.tick_minutes == 60);
    }

    SUBCASE("Zero minutes")
    {
        Command command = parse_command("TICK 0");

        REQUIRE(command.type == CommandType::TICK);

        CHECK(command.tick_minutes == 0);
    }
}

// Test #17: Parse inbox commands.
TEST_CASE("Parser INBOX command")
{
    SUBCASE("Without optional count")
    {
        Command command = parse_command("INBOX alice");

        REQUIRE(command.type == CommandType::INBOX);

        CHECK(command.user == "alice");
        CHECK_FALSE(command.has_count);
    }

    SUBCASE("With optional count")
    {
        Command command = parse_command("INBOX alice 5");

        REQUIRE(command.type == CommandType::INBOX);

        CHECK(command.user == "alice");
        CHECK(command.has_count);
        CHECK(command.count == 5);
    }
}

// Test #18: Parse history commands.
TEST_CASE("Parser HISTORY command")
{
    SUBCASE("Without optional count")
    {
        Command command = parse_command("HISTORY bob");

        REQUIRE(command.type == CommandType::HISTORY);

        CHECK(command.user == "bob");
        CHECK_FALSE(command.has_count);
    }

    SUBCASE("With optional count")
    {
        Command command = parse_command("HISTORY bob 10");

        REQUIRE(command.type == CommandType::HISTORY);

        CHECK(command.user == "bob");
        CHECK(command.has_count);
        CHECK(command.count == 10);
    }
}

// Test #19: Parse a statistics command.
TEST_CASE("Parser STATS command")
{
    Command command = parse_command("STATS");

    CHECK(command.type == CommandType::STATS);
}

// ============================================
// INVALID COMMAND TESTS
// ============================================

// Test #20: Reject malformed subscriptions.
TEST_CASE("Parser invalid SUB commands")
{
    CHECK(parse_command("SUB").type
          == CommandType::INVALID);

    CHECK(parse_command("SUB alice").type
          == CommandType::INVALID);

    CHECK(parse_command("SUB alice news below").type
          == CommandType::INVALID);

    CHECK(parse_command("SUB alice news below abc").type
          == CommandType::INVALID);

    CHECK(parse_command("SUB alice news below -10").type
          == CommandType::INVALID);

    CHECK(parse_command("SUB alice news at 24:00").type
          == CommandType::INVALID);

    CHECK(parse_command("SUB alice news at 12:60").type
          == CommandType::INVALID);

    CHECK(parse_command("SUB alice news every 0").type
          == CommandType::INVALID);

    CHECK(parse_command("SUB alice news every -2").type
          == CommandType::INVALID);

    CHECK(parse_command("SUB alice news every 1.5").type
          == CommandType::INVALID);

    CHECK(parse_command("SUB alice news random 5").type
          == CommandType::INVALID);

    // Multiple subscription options are not allowed.
    CHECK(parse_command(
              "SUB alice news below 10 at 08:00").type
          == CommandType::INVALID);
}

// Test #21: Reject malformed unsubscribe commands.
TEST_CASE("Parser invalid UNSUB commands")
{
    CHECK(parse_command("UNSUB").type
          == CommandType::INVALID);

    CHECK(parse_command("UNSUB alice").type
          == CommandType::INVALID);

    CHECK(parse_command("UNSUB alice news extra").type
          == CommandType::INVALID);
}

// Test #22: Reject incomplete publish commands.
TEST_CASE("Parser invalid PUB commands")
{
    CHECK(parse_command("PUB").type
          == CommandType::INVALID);

    CHECK(parse_command("PUB news").type
          == CommandType::INVALID);

    CHECK(parse_command("PUB news ").type
          == CommandType::INVALID);
}

// Test #23: Reject malformed price commands.
TEST_CASE("Parser invalid PRICE commands")
{
    CHECK(parse_command("PRICE").type
          == CommandType::INVALID);

    CHECK(parse_command("PRICE laptop").type
          == CommandType::INVALID);

    CHECK(parse_command("PRICE laptop abc").type
          == CommandType::INVALID);

    CHECK(parse_command("PRICE laptop -10").type
          == CommandType::INVALID);

    CHECK(parse_command("PRICE laptop 10.555").type
          == CommandType::INVALID);

    CHECK(parse_command("PRICE laptop 500 extra").type
          == CommandType::INVALID);
}

// Test #24: Reject malformed clock advancements.
TEST_CASE("Parser invalid TICK commands")
{
    CHECK(parse_command("TICK").type
          == CommandType::INVALID);

    CHECK(parse_command("TICK -60").type
          == CommandType::INVALID);

    CHECK(parse_command("TICK abc").type
          == CommandType::INVALID);

    CHECK(parse_command("TICK 1.5").type
          == CommandType::INVALID);

    CHECK(parse_command("TICK 60 extra").type
          == CommandType::INVALID);
}

// Test #25: Reject malformed inbox commands.
TEST_CASE("Parser invalid INBOX commands")
{
    CHECK(parse_command("INBOX").type
          == CommandType::INVALID);

    CHECK(parse_command("INBOX alice -5").type
          == CommandType::INVALID);

    CHECK(parse_command("INBOX alice abc").type
          == CommandType::INVALID);

    CHECK(parse_command("INBOX alice 5 extra").type
          == CommandType::INVALID);
}

// Test #26: Reject malformed history commands.
TEST_CASE("Parser invalid HISTORY commands")
{
    CHECK(parse_command("HISTORY").type
          == CommandType::INVALID);

    CHECK(parse_command("HISTORY alice -5").type
          == CommandType::INVALID);

    CHECK(parse_command("HISTORY alice abc").type
          == CommandType::INVALID);

    CHECK(parse_command("HISTORY alice 5 extra").type
          == CommandType::INVALID);
}

// Test #27: Reject extra arguments after STATS.
TEST_CASE("Parser invalid STATS command")
{
    CHECK(parse_command("STATS extra").type
          == CommandType::INVALID);
}

// Test #28: Handle unknown commands.
TEST_CASE("Parser unknown commands")
{
    CHECK(parse_command("HELLO").type
          == CommandType::UNKNOWN);

    CHECK(parse_command("DELETE alice").type
          == CommandType::UNKNOWN);

    CHECK(parse_command("sub alice news").type
          == CommandType::UNKNOWN);
}

// Test #29: Handle empty input.
TEST_CASE("Parser empty input")
{
    CHECK(parse_command("").type
          == CommandType::INVALID);

    CHECK(parse_command("   ").type
          == CommandType::INVALID);

    CHECK(parse_command("\t").type
          == CommandType::INVALID);
}

// Test #30: Handle extra whitespace.
TEST_CASE("Parser whitespace handling")
{
    Command command = parse_command(
        "   SUB   alice   news   ");

    REQUIRE(command.type == CommandType::SUB);

    CHECK(command.user == "alice");
    CHECK(command.topic == "news");
}
