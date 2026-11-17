// FILE: linked_list.h
// PROVIDES: the linked_list class and the linked list toolkit functions,
//           which work on any chain of nodes (see node.h).
//
// INVARIANT for the linked_list class:
//   1. head_ptr points to the first node of the list, or is NULL if the list is empty.
//   2. tail_ptr points to the last node of the list, or is NULL if the list is empty.
//   3. Every node in the list was allocated with new and is owned by this object.

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <cstdlib>  // Provides size_t and NULL
#include <iostream> // Provides ostream
#include "node.h"

namespace alert_service
{
    class linked_list
    {

    public:
        // CONSTRUCTORS AND DESTRUCTOR

        // linked_list()
        //   Postcondition: The list is empty (head_ptr and tail_ptr are NULL).
        linked_list();

        // linked_list(const linked_list &src)
        //   Postcondition: This list is a deep copy of src -- same values in the
        //                  same order, but in newly allocated nodes.
        linked_list(const linked_list &src);

        // ~linked_list()
        //   Postcondition: Every node owned by this list has been deleted.
        ~linked_list();

        // Modification member functions

        // node *get_head()
        //   Postcondition: Returns a pointer to the first node, or NULL if empty.
        node *get_head();

        // node *get_tail()
        //   Postcondition: Returns a pointer to the last node, or NULL if empty.
        node *get_tail();

        // linked_list &operator=(const linked_list &src)
        //   Postcondition: This list's old nodes have been deleted and it is now a
        //                  deep copy of src. Returns *this so assignments can chain.
        linked_list &operator=(const linked_list &src);

        // Constant member functions

        // const node *get_head() const
        //   Postcondition: Returns a read-only pointer to the first node, or NULL.
        const node *get_head() const;

        // const node *get_tail() const
        //   Postcondition: Returns a read-only pointer to the last node, or NULL.
        const node *get_tail() const;

        // Counter functions

        // static size_t get_comparisons()
        //   Postcondition: Returns the number of item comparisons made.
        static size_t get_comparisons();

        // static size_t get_nodes_visited()
        //   Postcondition: Returns the number of nodes a cursor has moved to.
        static size_t get_nodes_visited();

        // static size_t get_allocations()
        //   Postcondition: Returns the number of nodes allocated with new.
        static size_t get_allocations();

        // static size_t get_deallocations()
        //   Postcondition: Returns the number of nodes deleted.
        static size_t get_deallocations();

        // static void reset_counters()
        //   Postcondition: Every counter is 0. No list's nodes are changed.
        static void reset_counters();

        // static void print_counters(std::ostream &out)
        //   Precondition:  out is open for writing.
        //   Postcondition: Every counter's name and value has been written to out.
        static void print_counters(std::ostream &out);

        // The toolkit functions are not members, so they call these to count
        // the work they do.

        // static void count_comparison()
        //   Postcondition: The comparison counter has gone up by 1.
        static void count_comparison();

        // static void count_node_visit()
        //   Postcondition: The nodes-visited counter has gone up by 1.
        static void count_node_visit();

        // static void count_allocation()
        //   Postcondition: The allocation counter has gone up by 1.
        static void count_allocation();

        // static void count_deallocation()
        //   Postcondition: The deallocation counter has gone up by 1.
        static void count_deallocation();

    private:
        node *head_ptr;
        node *tail_ptr;

        // Static members are only declared here. Each one also needs a
        // definition in linked_list.cpp (for example
        // size_t linked_list::comparisons = 0;) or the program will not link.
        static size_t comparisons;
        static size_t nodes_visited;
        static size_t allocations;
        static size_t deallocations;
    };

    // Linked list toolkit functions

    // size_t list_length(const node *head_ptr)
    //   Precondition:  head_ptr is the head pointer of a linked list (may be NULL).
    //   Postcondition: Returns the number of nodes in the list.
    size_t list_length(const node *head_ptr);

    // void list_head_insert(node *&head_ptr, const node::value_type &entry)
    //   Precondition:  head_ptr is the head pointer of a linked list (may be NULL).
    //   Postcondition: A new node holding entry is at the front of the list,
    //                  and head_ptr now points to it.
    void list_head_insert(node *&head_ptr, const node::value_type &entry);

    // void list_insert(node *previous_ptr, const node::value_type &entry)
    //   Precondition:  previous_ptr points to a node in a linked list.
    //   Postcondition: A new node holding entry has been inserted directly
    //                  after the node that previous_ptr points to.
    void list_insert(node *previous_ptr, const node::value_type &entry);

    // node *list_search(node *head_ptr, const node::value_type &target)
    //   Precondition:  head_ptr is the head pointer of a linked list (may be NULL).
    //   Postcondition: Returns a pointer to the first node whose data equals
    //                  target, or NULL if no such node exists.
    node *list_search(node *head_ptr, const node::value_type &target);

    // const node *list_search(const node *head_ptr, const node::value_type &target)
    //   Same as above, but for read-only lists; returns a const pointer.
    const node *list_search(const node *head_ptr, const node::value_type &target);

    // node *list_locate(node *head_ptr, size_t position)
    //   Precondition:  head_ptr is the head pointer of a linked list (may be NULL),
    //                  and position > 0. Positions are 1-based.
    //   Postcondition: Returns a pointer to the node at the given position,
    //                  or NULL if the list has fewer than position nodes.
    node *list_locate(node *head_ptr, size_t position);

    // const node *list_locate(const node *head_ptr, size_t position)
    //   Same as above, but for read-only lists; returns a const pointer.
    const node *list_locate(const node *head_ptr, size_t position);

    // void list_head_remove(node *&head_ptr)
    //   Precondition:  head_ptr is the head pointer of a non-empty linked list.
    //   Postcondition: The first node has been deleted, and head_ptr now points
    //                  to the new first node (or NULL if the list became empty).
    void list_head_remove(node *&head_ptr);

    // void list_remove(node *previous_ptr)
    //   Precondition:  previous_ptr points to a node in a linked list, and that
    //                  node is not the last node (it has a successor).
    //   Postcondition: The node after previous_ptr has been deleted and
    //                  previous_ptr now links to the node that followed it.
    void list_remove(node *previous_ptr);

    // void list_clear(node *&head_ptr)
    //   Precondition:  head_ptr is the head pointer of a linked list (may be NULL).
    //   Postcondition: Every node has been deleted and head_ptr is NULL.
    void list_clear(node *&head_ptr);

    // void list_copy(const node *source_ptr, node *&head_ptr, node *&tail_ptr)
    //   Precondition:  source_ptr is the head pointer of a linked list (may be NULL).
    //   Postcondition: head_ptr and tail_ptr are the head and tail of a new list
    //                  holding the same values as the source, in the same order.
    //                  If the source is empty, both are NULL.
    //   Note: head_ptr and tail_ptr are overwritten, not freed -- the caller must
    //         clear any list they previously pointed to, or it will leak.
    void list_copy(const node *source_ptr, node *&head_ptr, node *&tail_ptr);
}

#endif
