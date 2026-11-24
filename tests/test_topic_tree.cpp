// Quality Lead - Topic Tree Unit Tests

#include "doctest/doctest.h"
#include "../data_structures/topic_tree/topic_tree.h"
#include <string>

using namespace alert_service;

// Test #1: A newly created topic tree should be empty.
TEST_CASE("Topic tree is initially empty")
{
    topic_tree<std::string> tree;

    CHECK(tree.empty());
    CHECK(tree.size() == 0);
    CHECK(tree.find("news") == nullptr);
}

// Test #2: Create a single topic.
TEST_CASE("Topic tree single topic creation")
{
    topic_tree<std::string> tree;

    auto* topic = tree.find_or_create("news");

    REQUIRE(topic != nullptr);

    CHECK(topic->name() == "news");
    CHECK_FALSE(tree.empty());
    CHECK(tree.size() == 1);
    CHECK(topic->children().empty());
    CHECK(topic->subscribers().empty());
}

// Test #3: Create nested topics.
TEST_CASE("Topic tree nested topic creation")
{
    topic_tree<std::string> tree;

    auto* soccer = tree.find_or_create("news/sports/soccer");

    REQUIRE(soccer != nullptr);

    CHECK(soccer->name() == "soccer");
    CHECK(tree.size() == 3);

    auto* news = tree.find("news");
    auto* sports = tree.find("news/sports");

    REQUIRE(news != nullptr);
    REQUIRE(sports != nullptr);

    CHECK(news->name() == "news");
    CHECK(sports->name() == "sports");

    // Verify the parent-child relationships.
    REQUIRE(news->children().length() == 1);
    REQUIRE(sports->children().length() == 1);

    CHECK(news->children().get_head()->data() == sports);
    CHECK(sports->children().get_head()->data() == soccer);
    CHECK(soccer->children().empty());
}

// Test #4: Creating a duplicate topic should not
// allocate another topic node.
TEST_CASE("Topic tree duplicate topic creation")
{
    topic_tree<std::string> tree;

    auto* first = tree.find_or_create("news/sports");
    auto* second = tree.find_or_create("news/sports");

    REQUIRE(first != nullptr);
    REQUIRE(second != nullptr);

    CHECK(first == second);
    CHECK(tree.size() == 2);
}

// Test #5: Topics with shared parents should reuse
// the existing parent nodes.
TEST_CASE("Topic tree shared parent topics")
{
    topic_tree<std::string> tree;

    tree.find_or_create("news/sports/soccer");
    tree.find_or_create("news/sports/basketball");

    CHECK(tree.size() == 4);

    auto* sports = tree.find("news/sports");
    auto* soccer = tree.find("news/sports/soccer");
    auto* basketball = tree.find("news/sports/basketball");

    REQUIRE(sports != nullptr);
    REQUIRE(soccer != nullptr);
    REQUIRE(basketball != nullptr);

    CHECK(sports->children().length() == 2);
    CHECK(soccer != basketball);
}

// Test #6: Create multiple independent top-level topics.
TEST_CASE("Topic tree multiple top-level topics")
{
    topic_tree<std::string> tree;

    tree.find_or_create("news");
    tree.find_or_create("technology");
    tree.find_or_create("weather");

    CHECK(tree.size() == 3);

    CHECK(tree.find("news") != nullptr);
    CHECK(tree.find("technology") != nullptr);
    CHECK(tree.find("weather") != nullptr);
}

// Test #7: Identical topic names under different
// parents should represent different topics.
TEST_CASE("Topic tree same name under different parents")
{
    topic_tree<std::string> tree;

    auto* first = tree.find_or_create("news/local");
    auto* second = tree.find_or_create("weather/local");

    REQUIRE(first != nullptr);
    REQUIRE(second != nullptr);

    CHECK(first->name() == second->name());
    CHECK(first != second);
    CHECK(tree.size() == 4);
}

// Test #8: Search for existing and missing topics.
TEST_CASE("Topic tree find")
{
    topic_tree<std::string> tree;

    tree.find_or_create("news/sports/soccer");

    SUBCASE("Find existing topics")
    {
        CHECK(tree.find("news") != nullptr);
        CHECK(tree.find("news/sports") != nullptr);
        CHECK(tree.find("news/sports/soccer") != nullptr);
    }

    SUBCASE("Find missing topics")
    {
        CHECK(tree.find("weather") == nullptr);
        CHECK(tree.find("news/technology") == nullptr);
        CHECK(tree.find("news/sports/tennis") == nullptr);

        // Searching should not create new topics.
        CHECK(tree.size() == 3);
    }

    SUBCASE("Find an empty path")
    {
        CHECK(tree.find("") == nullptr);
        CHECK(tree.find("/") == nullptr);
        CHECK(tree.find("///") == nullptr);

        CHECK(tree.size() == 3);
    }
}

// Test #9: Extra slashes should be ignored.
TEST_CASE("Topic tree path normalization")
{
    topic_tree<std::string> tree;

    auto* first = tree.find_or_create("news/sports");
    auto* second = tree.find_or_create("/news//sports/");

    REQUIRE(first != nullptr);
    REQUIRE(second != nullptr);

    CHECK(first == second);
    CHECK(tree.size() == 2);

    CHECK(tree.find("/news/sports/") == first);
    CHECK(tree.find("///news///sports///") == first);
}

// Test #10: Return all existing nodes along a path
// in order from the parent to the final topic.
TEST_CASE("Topic tree complete path traversal")
{
    topic_tree<std::string> tree;

    tree.find_or_create("news/sports/soccer");

    linked_list<topic_node<std::string>*> result;

    tree.path_nodes("news/sports/soccer", result);

    REQUIRE(result.length() == 3);

    CHECK(result.locate(1)->data()->name() == "news");
    CHECK(result.locate(2)->data()->name() == "sports");
    CHECK(result.locate(3)->data()->name() == "soccer");

    CHECK(tree.size() == 3);
}

// Test #11: Traversal should stop at the first
// nonexistent topic.
TEST_CASE("Topic tree partial path traversal")
{
    topic_tree<std::string> tree;

    tree.find_or_create("news/sports");

    linked_list<topic_node<std::string>*> result;

    tree.path_nodes("news/sports/soccer", result);

    REQUIRE(result.length() == 2);

    CHECK(result.locate(1)->data()->name() == "news");
    CHECK(result.locate(2)->data()->name() == "sports");

    // Traversal should not create the missing topic.
    CHECK(tree.size() == 2);
    CHECK(tree.find("news/sports/soccer") == nullptr);
}

// Test #12: Traversal of missing or empty paths.
TEST_CASE("Topic tree empty path traversal")
{
    topic_tree<std::string> tree;

    linked_list<topic_node<std::string>*> result;

    SUBCASE("Traverse an empty tree")
    {
        tree.path_nodes("news/sports", result);

        CHECK(result.empty());
        CHECK(tree.empty());
    }

    SUBCASE("Traverse a missing top-level topic")
    {
        tree.find_or_create("news/sports");

        tree.path_nodes("weather/local", result);

        CHECK(result.empty());
        CHECK(tree.size() == 2);
    }

    SUBCASE("Traverse an empty path")
    {
        tree.find_or_create("news");

        tree.path_nodes("", result);

        CHECK(result.empty());

        tree.path_nodes("///", result);

        CHECK(result.empty());
        CHECK(tree.size() == 1);
    }
}

// Test #13: path_nodes should clear the previous
// contents of the result list.
TEST_CASE("Topic tree path result replacement")
{
    topic_tree<std::string> tree;

    tree.find_or_create("news/sports/soccer");
    tree.find_or_create("weather/local");

    linked_list<topic_node<std::string>*> result;

    tree.path_nodes("news/sports/soccer", result);

    REQUIRE(result.length() == 3);

    tree.path_nodes("weather/local", result);

    REQUIRE(result.length() == 2);

    CHECK(result.locate(1)->data()->name() == "weather");
    CHECK(result.locate(2)->data()->name() == "local");

    // A missing path should also clear previous results.
    tree.path_nodes("technology", result);

    CHECK(result.empty());
    CHECK(tree.size() == 5);
}

// Test #14: Store subscribers in the correct topic.
TEST_CASE("Topic tree subscriber insertion")
{
    topic_tree<std::string> tree;

    auto* news = tree.find_or_create("news");
    auto* sports = tree.find_or_create("news/sports");

    REQUIRE(news != nullptr);
    REQUIRE(sports != nullptr);

    news->subscribers().head_insert("Alice");
    sports->subscribers().head_insert("Bob");

    CHECK(news->subscribers().length() == 1);
    CHECK(sports->subscribers().length() == 1);

    CHECK(news->subscribers().search("Alice") != nullptr);
    CHECK(sports->subscribers().search("Bob") != nullptr);

    // Subscribers should not automatically be copied
    // between parent and child topics.
    CHECK(news->subscribers().search("Bob") == nullptr);
    CHECK(sports->subscribers().search("Alice") == nullptr);

    CHECK(tree.size() == 2);
}

// Test #15: Remove subscribers from a topic.
TEST_CASE("Topic tree subscriber removal")
{
    topic_tree<std::string> tree;

    auto* topic = tree.find_or_create("news");

    REQUIRE(topic != nullptr);

    topic->subscribers().head_insert("Alice");
    topic->subscribers().head_insert("Bob");
    topic->subscribers().head_insert("Charlie");

    REQUIRE(topic->subscribers().length() == 3);

    SUBCASE("Remove first subscriber")
    {
        topic->subscribers().head_remove();

        CHECK(topic->subscribers().length() == 2);
        CHECK(topic->subscribers().search("Charlie") == nullptr);
        CHECK(topic->subscribers().search("Alice") != nullptr);
        CHECK(topic->subscribers().search("Bob") != nullptr);
    }

    SUBCASE("Remove last subscriber")
    {
        auto* previous = topic->subscribers().locate(2);

        REQUIRE(previous != nullptr);

        topic->subscribers().remove(previous);

        CHECK(topic->subscribers().length() == 2);
        CHECK(topic->subscribers().search("Alice") == nullptr);
    }

    SUBCASE("Remove all subscribers")
    {
        topic->subscribers().head_remove();
        topic->subscribers().head_remove();
        topic->subscribers().head_remove();

        CHECK(topic->subscribers().empty());

        // The topic itself should still exist.
        CHECK(tree.find("news") == topic);
        CHECK(tree.size() == 1);
    }
}

// Test #16: The same user may be stored in different
// topics independently.
TEST_CASE("Topic tree subscriber in multiple topics")
{
    topic_tree<std::string> tree;

    auto* news = tree.find_or_create("news");
    auto* sports = tree.find_or_create("news/sports");

    REQUIRE(news != nullptr);
    REQUIRE(sports != nullptr);

    news->subscribers().head_insert("Alice");
    sports->subscribers().head_insert("Alice");

    CHECK(news->subscribers().length() == 1);
    CHECK(sports->subscribers().length() == 1);

    news->subscribers().head_remove();

    CHECK(news->subscribers().empty());
    CHECK(sports->subscribers().search("Alice") != nullptr);
}

// Test #17: Verify topic node memory management.
TEST_CASE("Topic tree memory counters")
{
    topic_tree<std::string>::reset_counters();

    {
        topic_tree<std::string> tree;

#if KIT_COUNTERS_ENABLED
        // The unnamed root is allocated even when
        // the tree contains no actual topics.
        CHECK(topic_tree<std::string>::get_counters()
                  .allocations == 1);
#endif

        tree.find_or_create("news/sports/soccer");
        tree.find_or_create("news/sports/basketball");

        CHECK(tree.size() == 4);

#if KIT_COUNTERS_ENABLED
        // Four topic nodes plus the unnamed root.
        CHECK(topic_tree<std::string>::get_counters()
                  .allocations == 5);
#endif

        tree.find_or_create("news/sports/soccer");

#if KIT_COUNTERS_ENABLED
        // Duplicate creation should not allocate nodes.
        CHECK(topic_tree<std::string>::get_counters()
                  .allocations == 5);
#endif
    }

    // The destructor should delete every topic
    // node, including the unnamed root.
#if KIT_COUNTERS_ENABLED
    CHECK(topic_tree<std::string>::get_counters()
              .allocations == 5);

    CHECK(topic_tree<std::string>::get_counters()
              .extra.at("deallocations") == 5);
#endif
}

// Test #18: Verify search operation counters.
TEST_CASE("Topic tree search counters")
{
    topic_tree<std::string> tree;

    tree.find_or_create("news");
    tree.find_or_create("weather");
    tree.find_or_create("technology");

    topic_tree<std::string>::reset_counters();

    CHECK(tree.find("technology") != nullptr);

#if KIT_COUNTERS_ENABLED
    // The search examines news, weather,
    // and technology in insertion order.
    CHECK(topic_tree<std::string>::get_counters()
              .visits == 3);

    CHECK(topic_tree<std::string>::get_counters()
              .comparisons == 3);

    CHECK(topic_tree<std::string>::get_counters()
              .allocations == 0);
#endif
}

// Test #19: Resetting counters should not modify
// the contents of the topic tree.
TEST_CASE("Topic tree counter reset")
{
    topic_tree<std::string> tree;

    tree.find_or_create("news/sports");

    topic_tree<std::string>::reset_counters();

    CHECK(topic_tree<std::string>::get_counters()
              .allocations == 0);

    CHECK(topic_tree<std::string>::get_counters()
              .comparisons == 0);

    CHECK(topic_tree<std::string>::get_counters()
              .visits == 0);

    CHECK(topic_tree<std::string>::get_counters()
              .extra.empty());

    CHECK(tree.size() == 2);
    CHECK(tree.find("news/sports") != nullptr);
}
