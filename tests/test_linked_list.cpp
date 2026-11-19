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

  
}




















