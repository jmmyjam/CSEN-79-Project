//Quality Lead - Linked List Unit Tests

#include "doctest/doctest.h"
#include "../data_structures/linked_list/linked_list.h"

using namespace alert_service;

// Test #1: A new linked list should be empty.
TEST_CASE("New linked list is empty")
{
    linked_list<int> list;

    CHECK(list.empty());
    CHECK(list.length() == 0);
    CHECK(list.get_head() == nullptr);
    CHECK(list.get_tail() == nullptr);

    // Searching an empty list should return nullptr.
    CHECK(list.search(10) == nullptr);
    CHECK(list.locate(1) == nullptr);
}

// Test #2: Insert elements at the beginning.
TEST_CASE("Linked list head insertion")
{
    linked_list<int> list;

    SUBCASE("Insert one element")
    {
        list.head_insert(10);

        CHECK_FALSE(list.empty());
        CHECK(list.length() == 1);

        REQUIRE(list.get_head() != nullptr);
        CHECK(list.get_head()->data() == 10);
        CHECK(list.get_head() == list.get_tail());
        CHECK(list.get_tail()->link() == nullptr);
    }

    SUBCASE("Insert multiple elements")
    {
        list.head_insert(10);
        list.head_insert(15);
        list.head_insert(20);

        // Expected list: 20 -> 15 -> 10
        REQUIRE(list.length() == 3);

        REQUIRE(list.locate(1) != nullptr);
        REQUIRE(list.locate(2) != nullptr);
        REQUIRE(list.locate(3) != nullptr);

        CHECK(list.locate(1)->data() == 20);
        CHECK(list.locate(2)->data() == 15);
        CHECK(list.locate(3)->data() == 10);

        REQUIRE(list.get_tail() != nullptr);
        CHECK(list.get_tail()->data() == 10);
    }
}

// Test #3: Insert after a specific node.
TEST_CASE("Linked list insertion after a node")
{
    linked_list<int> list;

    list.head_insert(10);
    list.insert(list.get_head(), 30);

    SUBCASE("Insert in the middle")
    {
        list.insert(list.get_head(), 20);

        // Expected list: 10 -> 20 -> 30
        REQUIRE(list.length() == 3);

        REQUIRE(list.locate(1) != nullptr);
        REQUIRE(list.locate(2) != nullptr);
        REQUIRE(list.locate(3) != nullptr);

        CHECK(list.locate(1)->data() == 10);
        CHECK(list.locate(2)->data() == 20);
        CHECK(list.locate(3)->data() == 30);
    }

    SUBCASE("Insert at the tail")
    {
        list.insert(list.get_tail(), 40);

        CHECK(list.length() == 3);

        REQUIRE(list.get_tail() != nullptr);
        CHECK(list.get_tail()->data() == 40);
        CHECK(list.get_tail()->link() == nullptr);
    }
}

// Test #4: Remove the first node.
TEST_CASE("Linked list head removal")
{
    linked_list<int> list;

    SUBCASE("Remove the first of multiple elements")
    {
        list.head_insert(10);
        list.head_insert(20);

        list.head_remove();

        CHECK(list.length() == 1);

        REQUIRE(list.get_head() != nullptr);
        CHECK(list.get_head()->data() == 10);
        CHECK(list.get_head() == list.get_tail());
    }

    SUBCASE("Remove the only element")
    {
        list.head_insert(10);
        list.head_remove();

        CHECK(list.empty());
        CHECK(list.length() == 0);
        CHECK(list.get_head() == nullptr);
        CHECK(list.get_tail() == nullptr);
    }
}

// Test #5: Remove middle and last nodes.
TEST_CASE("Linked list node removal")
{
    linked_list<int> list;

    list.head_insert(30);
    list.head_insert(20);
    list.head_insert(10);

    // Initial list: 10 -> 20 -> 30

    SUBCASE("Remove the middle node")
    {
        list.remove(list.get_head());

        // Expected list: 10 -> 30
        CHECK(list.length() == 2);

        REQUIRE(list.locate(2) != nullptr);
        CHECK(list.locate(2)->data() == 30);
        CHECK(list.search(20) == nullptr);
    }

    SUBCASE("Remove the last node")
    {
        REQUIRE(list.locate(2) != nullptr);
        list.remove(list.locate(2));

        // Expected list: 10 -> 20
        CHECK(list.length() == 2);

        REQUIRE(list.get_tail() != nullptr);
        CHECK(list.get_tail()->data() == 20);
        CHECK(list.get_tail()->link() == nullptr);
    }
}

// Test #6: Search for existing and missing values.
TEST_CASE("Linked list search")
{
    linked_list<int> list;

    list.head_insert(10);
    list.head_insert(20);
    list.head_insert(30);

    SUBCASE("Search for an existing value")
    {
        REQUIRE(list.search(20) != nullptr);
        CHECK(list.search(20)->data() == 20);
    }

    SUBCASE("Search for a missing value")
    {
        CHECK(list.search(99) == nullptr);
    }
}

// Test #7: Duplicates should be preserved.
TEST_CASE("Linked list duplicate values")
{
    linked_list<int> list;

    list.head_insert(5);
    list.head_insert(5);
    list.head_insert(5);

    SUBCASE("All duplicates are stored")
    {
        REQUIRE(list.length() == 3);

        REQUIRE(list.locate(1) != nullptr);
        REQUIRE(list.locate(2) != nullptr);
        REQUIRE(list.locate(3) != nullptr);

        CHECK(list.locate(1)->data() == 5);
        CHECK(list.locate(2)->data() == 5);
        CHECK(list.locate(3)->data() == 5);
    }

    SUBCASE("Search returns the first matching node")
    {
        CHECK(list.search(5) == list.get_head());
    }

    SUBCASE("Removing one duplicate preserves the others")
    {
        list.head_remove();

        CHECK(list.length() == 2);
        CHECK(list.search(5) != nullptr);
    }
}

// Test #8: Locate elements using 1-based positions.
TEST_CASE("Linked list locate")
{
    linked_list<int> list;

    list.head_insert(30);
    list.head_insert(20);
    list.head_insert(10);

    SUBCASE("Locate valid positions")
    {
        REQUIRE(list.locate(1) != nullptr);
        REQUIRE(list.locate(2) != nullptr);
        REQUIRE(list.locate(3) != nullptr);

        CHECK(list.locate(1)->data() == 10);
        CHECK(list.locate(2)->data() == 20);
        CHECK(list.locate(3)->data() == 30);
    }

    SUBCASE("Locate a position outside the list")
    {
        CHECK(list.locate(4) == nullptr);
    }
}

// Test #9: Clear should remove every node.
TEST_CASE("Linked list clear")
{
    linked_list<int> list;

    SUBCASE("Clear a populated list")
    {
        list.head_insert(10);
        list.head_insert(20);
        list.head_insert(30);

        list.clear();

        CHECK(list.empty());
        CHECK(list.length() == 0);
        CHECK(list.get_head() == nullptr);
        CHECK(list.get_tail() == nullptr);
    }

    SUBCASE("Clear an already empty list")
    {
        list.clear();

        CHECK(list.empty());
        CHECK(list.length() == 0);
    }

    SUBCASE("Insert again after clearing")
    {
        list.head_insert(10);
        list.clear();
        list.head_insert(20);

        CHECK(list.length() == 1);

        REQUIRE(list.get_head() != nullptr);
        CHECK(list.get_head()->data() == 20);
        CHECK(list.get_head() == list.get_tail());
    }
}

// Test #10: Copy constructor should create a deep copy.
TEST_CASE("Linked list copy constructor")
{
    linked_list<int> original;

    original.head_insert(10);
    original.head_insert(20);

    linked_list<int> copy(original);

    CHECK(copy.length() == original.length());

    REQUIRE(copy.locate(1) != nullptr);
    REQUIRE(copy.locate(2) != nullptr);

    CHECK(copy.locate(1)->data() == 20);
    CHECK(copy.locate(2)->data() == 10);

    // The lists should not share nodes.
    CHECK(copy.get_head() != original.get_head());
    CHECK(copy.get_tail() != original.get_tail());

    // Changing the copy should not change the original.
    copy.head_remove();

    CHECK(copy.length() == 1);
    CHECK(original.length() == 2);

    REQUIRE(original.get_head() != nullptr);
    CHECK(original.get_head()->data() == 20);
}

// Test #11: Assignment should create an independent copy.
TEST_CASE("Linked list assignment operator")
{
    linked_list<int> original;
    original.head_insert(10);
    original.head_insert(20);

    linked_list<int> copy;
    copy.head_insert(99);

    SUBCASE("Assign a populated list")
    {
        copy = original;

        REQUIRE(copy.length() == 2);
        REQUIRE(copy.locate(1) != nullptr);
        REQUIRE(copy.locate(2) != nullptr);

        CHECK(copy.locate(1)->data() == 20);
        CHECK(copy.locate(2)->data() == 10);

        CHECK(copy.get_head() != original.get_head());
        CHECK(copy.get_tail() != original.get_tail());

        copy.head_remove();

        CHECK(copy.length() == 1);
        CHECK(original.length() == 2);
    }

    SUBCASE("Self-assignment")
    {
        original = original;

        CHECK(original.length() == 2);

        REQUIRE(original.get_head() != nullptr);
        CHECK(original.get_head()->data() == 20);
    }

    SUBCASE("Assign an empty list")
    {
        linked_list<int> empty_list;

        copy = empty_list;

        CHECK(copy.empty());
        CHECK(copy.length() == 0);
        CHECK(copy.get_head() == nullptr);
        CHECK(copy.get_tail() == nullptr);
    }
}

// Test #12: Const accessors should work correctly.
TEST_CASE("Linked list const accessors")
{
    linked_list<int> list;

    list.head_insert(10);
    list.head_insert(20);

    const linked_list<int>& read_only = list;

    CHECK(read_only.length() == 2);
    CHECK_FALSE(read_only.empty());

    REQUIRE(read_only.get_head() != nullptr);
    REQUIRE(read_only.get_tail() != nullptr);

    CHECK(read_only.get_head()->data() == 20);
    CHECK(read_only.get_tail()->data() == 10);

    CHECK(read_only.search(10) != nullptr);
    CHECK(read_only.search(99) == nullptr);

    REQUIRE(read_only.locate(2) != nullptr);
    CHECK(read_only.locate(2)->data() == 10);
    CHECK(read_only.locate(3) == nullptr);
}

// Test #13: Allocation and deallocation counters.
TEST_CASE("Linked list memory counters")
{
    linked_list<int>::reset_counters();

    {
        linked_list<int> list;

        list.head_insert(10);
        list.head_insert(20);

        CHECK(linked_list<int>::get_allocations() == 2);
        CHECK(linked_list<int>::get_deallocations() == 0);

        list.head_remove();

        CHECK(linked_list<int>::get_deallocations() == 1);
    }

    // The destructor should delete the remaining node.
    CHECK(linked_list<int>::get_allocations() == 2);
    CHECK(linked_list<int>::get_deallocations() == 2);

    linked_list<int>::reset_counters();

    CHECK(linked_list<int>::get_allocations() == 0);
    CHECK(linked_list<int>::get_deallocations() == 0);
    CHECK(linked_list<int>::get_comparisons() == 0);
    CHECK(linked_list<int>::get_nodes_visited() == 0);
}

// Test #14: Search operation counters.
TEST_CASE("Linked list search counters")
{
    linked_list<int> list;

    list.head_insert(30);
    list.head_insert(20);
    list.head_insert(10);

    linked_list<int>::reset_counters();

    // Searching for 30 requires visiting all three nodes.
    REQUIRE(list.search(30) != nullptr);

    CHECK(linked_list<int>::get_comparisons() == 3);
    CHECK(linked_list<int>::get_nodes_visited() == 3);
}
