//Quality Lead - Linked List Unit Tests

#include <cassert>
#include <iostream>
#include "../data_structures/linked_list/linked_list.h"

using namespace alert_service;

//test #1: new linked list should be empty
void test_empty_list(){
  linked_list<int> list;

  assert(list.empty());
  assert(list.length() == 0);
  assert(list.get_head() == nullptr);
  assert(list.get_tail() == nullptr);
}

//test #2: insert elements at the beginning
void test_head_insert(){
  linked_list<int> list;

  list.head_insert(10);
  list.head_insert(15);
  list.head_insert(20);

  //checking size and head/tail pointers
  assert(list.length() == 3);
  assert(list.get_head()->data() == 20);
  assert(list.get_tail()->data() == 10);

  //checking location values
  assert(list.locate(1)->data() == 20);
  assert(list.locate(2)->data() == 15);
  assert(list.locate(3)->data(0 == 10);
  
}

//test #3: inserting a specific node
void test_insert(){
  linked_list<int> list;

  list.head_insert(10);
  list.insert(list.get_head(), 20);
  list.insert(list.get_tail(), 30);

  //checking for size
  assert(list.length() == 3);
  //check each node for the correct value inserted
  assert(list.locate(1)->data() == 10);
  assert(list.locate(2)->data() == 20);
  assert(list.locate(3)->data() == 30);
  //checks tail pointer
  assert(list.get_tail()->data() == 30); 
  //checks that after tail pointer is NULL
  assert(list.get_tail()->link() == nullptr);
  
}

//test #4: remove the first node
void test_head_remove(){
  linked_list<int> list;

  list.head_insert(10);
  list.head_insert(20);

  list.head_remove();

  assert(list.length() == 1);
  assert(list.get_head()->data() == 10);

  // Remove the only remaining element.
  list.head_remove();

  assert(list.empty());
  assert(list.length() == 0);
  assert(list.get_head() == nullptr);
  assert(list.get_tail() == nullptr);

}

//test #5: removing a middle node or the last node
void test_remove(){
  linked_list<int> list;

  list.head_insert(30);
  list.head_insert(20);
  list.head_insert(10);

  // List: 10 -> 20 -> 30

  // Remove the node after the head (20).
  list.remove(list.get_head());

  assert(list.length() == 2);
  assert(list.locate(2)->data() == 30);

  // Remove the last node (30).
  list.remove(list.get_head());

  assert(list.length() == 1);
  assert(list.get_head() == list.get_tail());
  assert(list.get_tail()->data() == 10);
  assert(list.get_tail()->link() == nullptr);
  
}

//test #6: search for an existing/missing value
void test_search(){
  linked_list<int> list;

  list.head_insert(10);
  list.head_insert(20);
  list.head_insert(30);

  assert(list.search(20) != nullptr);
  assert(list.search(20)->data() == 20);
  assert(list.search(99) == nullptr);

}


// test #7: duplicates should be preserved
void test_duplicates()
{
    linked_list<int> list;

    list.head_insert(5);
    list.head_insert(5);
    list.head_insert(5);

    assert(list.length() == 3);
    assert(list.locate(1)->data() == 5);
    assert(list.locate(2)->data() == 5);
    assert(list.locate(3)->data() == 5);

    // search should find the first matching node.
    assert(list.search(5) == list.get_head());
}

// Test 8: Locate elements using 1-based positions.
void test_locate()
{
    linked_list<int> list;

    list.head_insert(30);
    list.head_insert(20);
    list.head_insert(10);

    assert(list.locate(1)->data() == 10);
    assert(list.locate(2)->data() == 20);
    assert(list.locate(3)->data() == 30);

    // A position beyond the list returns nullptr.
    assert(list.locate(4) == nullptr);
}

// Test 9: Clear should remove every node.
void test_clear()
{
    linked_list<int> list;

    list.head_insert(10);
    list.head_insert(20);
    list.head_insert(30);

    list.clear();

    assert(list.empty());
    assert(list.length() == 0);
    assert(list.get_head() == nullptr);
    assert(list.get_tail() == nullptr);

    // Clearing an already empty list is allowed.
    list.clear();
    assert(list.empty());
}

// Test 10: Copy constructor should make a deep copy.
void test_copy_constructor()
{
    linked_list<int> original;

    original.head_insert(10);
    original.head_insert(20);

    linked_list<int> copy(original);

    assert(copy.length() == original.length());
    assert(copy.locate(1)->data() == 20);
    assert(copy.locate(2)->data() == 10);

    // The lists must not share the same nodes.
    assert(copy.get_head() != original.get_head());

    // Changing the copy must not change the original.
    copy.head_remove();

    assert(copy.length() == 1);
    assert(original.length() == 2);
}

// Test 11: Assignment should copy values independently.
void test_assignment()
{
    linked_list<int> original;
    original.head_insert(10);
    original.head_insert(20);

    linked_list<int> copy;
    copy.head_insert(99);

    copy = original;

    assert(copy.length() == 2);
    assert(copy.locate(1)->data() == 20);
    assert(copy.locate(2)->data() == 10);
    assert(copy.get_head() != original.get_head());

    // Self-assignment should not destroy the list.
    copy = copy;

    assert(copy.length() == 2);
    assert(copy.get_head()->data() == 20);

    // Assigning an empty list should clear the target.
    linked_list<int> empty_list;
    copy = empty_list;

    assert(copy.empty());
}

// Test 12: Const accessors should work correctly.
void test_const_accessors()
{
    linked_list<int> list;
    list.head_insert(10);
    list.head_insert(20);

    const linked_list<int>& read_only = list;

    assert(read_only.length() == 2);
    assert(!read_only.empty());
    assert(read_only.get_head()->data() == 20);
    assert(read_only.get_tail()->data() == 10);
    assert(read_only.search(10) != nullptr);
    assert(read_only.search(99) == nullptr);
    assert(read_only.locate(2)->data() == 10);
    assert(read_only.locate(3) == nullptr);
}

// Test 13: Allocation and deallocation counters.
void test_counters()
{
    linked_list<int>::reset_counters();

    {
        linked_list<int> list;

        list.head_insert(10);
        list.head_insert(20);

        assert(linked_list<int>::get_allocations() == 2);
        assert(linked_list<int>::get_deallocations() == 0);

        list.head_remove();

        assert(linked_list<int>::get_deallocations() == 1);
    }

    // Destructor should delete the remaining node.
    assert(linked_list<int>::get_allocations() == 2);
    assert(linked_list<int>::get_deallocations() == 2);

    linked_list<int>::reset_counters();
}

// Run all linked list tests.
int main()
{
    test_empty_list();
    test_head_insert();
    test_insert();
    test_head_remove();
    test_remove();
    test_search();
    test_duplicates();
    test_locate();
    test_clear();
    test_copy_constructor();
    test_assignment();
    test_const_accessors();
    test_counters();

    std::cout << "All linked list tests passed!" << std::endl;

    return 0;
}


