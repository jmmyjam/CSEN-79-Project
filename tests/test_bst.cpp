
// Quality Lead - Binary Search Tree Unit Tests

#include "doctest/doctest.h"
#include "../data_structures/bst/bst.h"

using namespace alert_service;

// Test #1: A newly created BST should be empty.
TEST_CASE("BST is initially empty")
{
    bst<int> tree;

    CHECK(tree.empty());
    CHECK(tree.size() == 0);
    CHECK(tree.height() == 0);
    CHECK_FALSE(tree.contains(10));
}

// Test #2: Insert elements into the BST.
TEST_CASE("BST insertion")
{
    bst<int> tree;

    SUBCASE("Insert a single element")
    {
        tree.insert(50);

        CHECK_FALSE(tree.empty());
        CHECK(tree.size() == 1);
        CHECK(tree.height() == 1);
        CHECK(tree.contains(50));
        CHECK(tree.max() == 50);
    }

    SUBCASE("Insert multiple elements")
    {
        tree.insert(50);
        tree.insert(30);
        tree.insert(70);
        tree.insert(20);
        tree.insert(40);
        tree.insert(60);
        tree.insert(80);

        CHECK(tree.size() == 7);
        CHECK(tree.height() == 3);

        CHECK(tree.contains(50));
        CHECK(tree.contains(30));
        CHECK(tree.contains(70));
        CHECK(tree.contains(20));
        CHECK(tree.contains(40));
        CHECK(tree.contains(60));
        CHECK(tree.contains(80));
    }
}

// Test #3: Duplicate values should not be inserted.
TEST_CASE("BST duplicate insertion")
{
    bst<int> tree;

    tree.insert(50);
    tree.insert(50);
    tree.insert(50);

    CHECK(tree.size() == 1);
    CHECK(tree.height() == 1);
    CHECK(tree.contains(50));

    tree.insert(30);
    tree.insert(70);
    tree.insert(30);
    tree.insert(70);

    CHECK(tree.size() == 3);
    CHECK(tree.height() == 2);
}

// Test #4: Search for existing and missing values.
TEST_CASE("BST contains")
{
    bst<int> tree;

    tree.insert(50);
    tree.insert(30);
    tree.insert(70);

    SUBCASE("Search for existing values")
    {
        CHECK(tree.contains(50));
        CHECK(tree.contains(30));
        CHECK(tree.contains(70));
    }

    SUBCASE("Search for missing values")
    {
        CHECK_FALSE(tree.contains(10));
        CHECK_FALSE(tree.contains(40));
        CHECK_FALSE(tree.contains(100));
    }
}

// Test #5: Find the maximum value.
TEST_CASE("BST maximum value")
{
    bst<int> tree;

    SUBCASE("Maximum of a single-node tree")
    {
        tree.insert(50);

        CHECK(tree.max() == 50);
    }

    SUBCASE("Maximum of multiple elements")
    {
        tree.insert(50);
        tree.insert(30);
        tree.insert(70);
        tree.insert(90);
        tree.insert(60);

        CHECK(tree.max() == 90);
    }

    SUBCASE("Maximum with negative values")
    {
        tree.insert(-10);
        tree.insert(-30);
        tree.insert(-5);

        CHECK(tree.max() == -5);
    }
}

// Test #6: Remove a leaf node.
TEST_CASE("BST leaf removal")
{
    bst<int> tree;

    tree.insert(50);
    tree.insert(30);
    tree.insert(70);

    tree.remove(30);

    CHECK(tree.size() == 2);
    CHECK_FALSE(tree.contains(30));
    CHECK(tree.contains(50));
    CHECK(tree.contains(70));
    CHECK(tree.height() == 2);
}

// Test #7: Remove a node with one child.
TEST_CASE("BST removal of a node with one child")
{
    bst<int> tree;

    tree.insert(50);
    tree.insert(30);

    SUBCASE("Node has only a left child")
    {
        tree.insert(20);

        tree.remove(30);

        CHECK(tree.size() == 2);
        CHECK_FALSE(tree.contains(30));
        CHECK(tree.contains(20));
        CHECK(tree.contains(50));
        CHECK(tree.height() == 2);
    }

    SUBCASE("Node has only a right child")
    {
        tree.insert(40);

        tree.remove(30);

        CHECK(tree.size() == 2);
        CHECK_FALSE(tree.contains(30));
        CHECK(tree.contains(40));
        CHECK(tree.contains(50));
        CHECK(tree.height() == 2);
    }
}

// Test #8: Remove a node with two children.
TEST_CASE("BST removal of a node with two children")
{
    bst<int> tree;

    tree.insert(50);
    tree.insert(30);
    tree.insert(70);
    tree.insert(20);
    tree.insert(40);

    tree.remove(30);

    CHECK(tree.size() == 4);
    CHECK_FALSE(tree.contains(30));

    // Both children and the other nodes should remain.
    CHECK(tree.contains(20));
    CHECK(tree.contains(40));
    CHECK(tree.contains(50));
    CHECK(tree.contains(70));
    CHECK(tree.max() == 70);
}

// Test #9: Remove the root node.
TEST_CASE("BST root removal")
{
    bst<int> tree;

    tree.insert(50);

    SUBCASE("Remove the only node")
    {
        tree.remove(50);

        CHECK(tree.empty());
        CHECK(tree.size() == 0);
        CHECK(tree.height() == 0);
        CHECK_FALSE(tree.contains(50));
    }

    SUBCASE("Remove root with a left child")
    {
        tree.insert(30);
        tree.remove(50);

        CHECK(tree.size() == 1);
        CHECK(tree.contains(30));
        CHECK_FALSE(tree.contains(50));
        CHECK(tree.height() == 1);
    }

    SUBCASE("Remove root with a right child")
    {
        tree.insert(70);
        tree.remove(50);

        CHECK(tree.size() == 1);
        CHECK(tree.contains(70));
        CHECK_FALSE(tree.contains(50));
        CHECK(tree.height() == 1);
    }

    SUBCASE("Remove root with two children")
    {
        tree.insert(30);
        tree.insert(70);
        tree.insert(20);
        tree.insert(40);
        tree.insert(60);
        tree.insert(80);

        tree.remove(50);

        CHECK(tree.size() == 6);
        CHECK_FALSE(tree.contains(50));

        CHECK(tree.contains(30));
        CHECK(tree.contains(70));
        CHECK(tree.contains(20));
        CHECK(tree.contains(40));
        CHECK(tree.contains(60));
        CHECK(tree.contains(80));

        CHECK(tree.max() == 80);
    }
}

// Test #10: Removing a missing value should not
// change the tree.
TEST_CASE("BST removal of a missing value")
{
    bst<int> tree;

    SUBCASE("Remove from an empty tree")
    {
        tree.remove(50);

        CHECK(tree.empty());
        CHECK(tree.size() == 0);
    }

    SUBCASE("Remove missing value from populated tree")
    {
        tree.insert(50);
        tree.insert(30);
        tree.insert(70);

        tree.remove(100);

        CHECK(tree.size() == 3);
        CHECK(tree.height() == 2);
        CHECK(tree.contains(50));
        CHECK(tree.contains(30));
        CHECK(tree.contains(70));
    }
}

// Test #11: Remove the maximum value.
TEST_CASE("BST remove maximum")
{
    bst<int> tree;

    SUBCASE("Remove maximum from a single-node tree")
    {
        tree.insert(50);
        tree.remove_max();

        CHECK(tree.empty());
        CHECK(tree.size() == 0);
    }

    SUBCASE("Remove maximum from multiple elements")
    {
        tree.insert(50);
        tree.insert(30);
        tree.insert(70);
        tree.insert(90);

        tree.remove_max();

        CHECK(tree.size() == 3);
        CHECK_FALSE(tree.contains(90));
        REQUIRE_FALSE(tree.empty());
        CHECK(tree.max() == 70);
    }

    SUBCASE("Maximum node has a left child")
    {
        tree.insert(50);
        tree.insert(30);
        tree.insert(70);
        tree.insert(60);

        tree.remove_max();

        CHECK(tree.size() == 3);
        CHECK_FALSE(tree.contains(70));
        CHECK(tree.contains(60));
        REQUIRE_FALSE(tree.empty());
        CHECK(tree.max() == 60);
    }

    SUBCASE("Repeatedly remove maximum until empty")
    {
        tree.insert(50);
        tree.insert(30);
        tree.insert(70);

        tree.remove_max();

        REQUIRE_FALSE(tree.empty());
        CHECK(tree.max() == 50);

        tree.remove_max();

        REQUIRE_FALSE(tree.empty());
        CHECK(tree.max() == 30);

        tree.remove_max();

        CHECK(tree.empty());
        CHECK(tree.size() == 0);
        CHECK(tree.height() == 0);
    }
}

// Test #12: Verify the height of different tree shapes.
TEST_CASE("BST height")
{
    bst<int> tree;

    SUBCASE("Balanced tree")
    {
        tree.insert(50);
        tree.insert(30);
        tree.insert(70);
        tree.insert(20);
        tree.insert(40);
        tree.insert(60);
        tree.insert(80);

        CHECK(tree.height() == 3);
    }

    SUBCASE("Right-skewed tree")
    {
        tree.insert(10);
        tree.insert(20);
        tree.insert(30);
        tree.insert(40);

        CHECK(tree.height() == 4);
    }

    SUBCASE("Left-skewed tree")
    {
        tree.insert(40);
        tree.insert(30);
        tree.insert(20);
        tree.insert(10);

        CHECK(tree.height() == 4);
    }
}

// Test #13: Copy constructor should create a deep copy.
TEST_CASE("BST copy constructor")
{
    bst<int> original;

    SUBCASE("Copy an empty tree")
    {
        bst<int> copy(original);

        CHECK(copy.empty());
        CHECK(copy.size() == 0);
    }

    SUBCASE("Copy a populated tree")
    {
        original.insert(50);
        original.insert(30);
        original.insert(70);

        bst<int> copy(original);

        CHECK(copy.size() == original.size());
        CHECK(copy.height() == original.height());

        CHECK(copy.contains(50));
        CHECK(copy.contains(30));
        CHECK(copy.contains(70));

        // Modifying the copy should not affect the original.
        copy.remove(30);
        copy.insert(90);

        CHECK_FALSE(copy.contains(30));
        CHECK(copy.contains(90));

        CHECK(original.contains(30));
        CHECK_FALSE(original.contains(90));
        CHECK(original.size() == 3);
    }
}

// Test #14: Assignment should create an independent copy.
TEST_CASE("BST assignment operator")
{
    bst<int> original;

    original.insert(50);
    original.insert(30);
    original.insert(70);

    bst<int> copy;
    copy.insert(99);

    SUBCASE("Assign a populated tree")
    {
        copy = original;

        CHECK(copy.size() == 3);
        CHECK(copy.height() == original.height());
        CHECK(copy.contains(50));
        CHECK(copy.contains(30));
        CHECK(copy.contains(70));
        CHECK_FALSE(copy.contains(99));

        copy.remove(30);

        CHECK_FALSE(copy.contains(30));
        CHECK(original.contains(30));
        CHECK(original.size() == 3);
    }

    SUBCASE("Self-assignment")
    {
        original = original;

        CHECK(original.size() == 3);
        CHECK(original.contains(50));
        CHECK(original.contains(30));
        CHECK(original.contains(70));
    }

    SUBCASE("Assign an empty tree")
    {
        bst<int> empty_tree;

        copy = empty_tree;

        CHECK(copy.empty());
        CHECK(copy.size() == 0);
        CHECK(copy.height() == 0);
    }
}

// Test #15: Verify memory allocation and deallocation.
TEST_CASE("BST memory counters")
{
    bst<int>::reset_counters();

    {
        bst<int> tree;

        tree.insert(50);
        tree.insert(30);
        tree.insert(70);

#if KIT_COUNTERS_ENABLED
        CHECK(bst<int>::get_counters().allocations == 3);
#endif

        tree.remove(30);

#if KIT_COUNTERS_ENABLED
        CHECK(bst<int>::get_counters()
                  .extra.at("deallocations") == 1);
#endif
    }

    // The destructor should delete the remaining nodes.
#if KIT_COUNTERS_ENABLED
    CHECK(bst<int>::get_counters().allocations == 3);
    CHECK(bst<int>::get_counters()
              .extra.at("deallocations") == 3);
#endif

    bst<int>::reset_counters();

    CHECK(bst<int>::get_counters().allocations == 0);
    CHECK(bst<int>::get_counters().comparisons == 0);
    CHECK(bst<int>::get_counters().visits == 0);
    CHECK(bst<int>::get_counters().moves == 0);
    CHECK(bst<int>::get_counters().extra.empty());
}

// Test #16: Verify search operation counters.
TEST_CASE("BST search counters")
{
    bst<int> tree;

    tree.insert(50);
    tree.insert(30);
    tree.insert(70);

    bst<int>::reset_counters();

    CHECK(tree.contains(70));

#if KIT_COUNTERS_ENABLED
    CHECK(bst<int>::get_counters().visits == 2);
    CHECK(bst<int>::get_counters().comparisons == 4);
#endif
}

// Test #17: Verify that removing a node with two
// children records a move.
TEST_CASE("BST removal move counter")
{
    bst<int> tree;

    tree.insert(50);
    tree.insert(30);
    tree.insert(70);

    bst<int>::reset_counters();

    tree.remove(50);

    CHECK(tree.size() == 2);
    CHECK_FALSE(tree.contains(50));
    CHECK(tree.contains(30));
    CHECK(tree.contains(70));

#if KIT_COUNTERS_ENABLED
    CHECK(bst<int>::get_counters().moves == 1);
    CHECK(bst<int>::get_counters()
              .extra.at("deallocations") == 1);
#endif
}
