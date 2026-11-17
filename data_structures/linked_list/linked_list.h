// FILE: linked_list.h
// PROVIDES: the linked_list class, a singly linked list of
//           node::value_type items (see node.h).
//
// INVARIANT for the linked_list class:
//   1. head_ptr points to the first node of the list, or is NULL if the list is empty.
//   2. tail_ptr points to the last node of the list, or is NULL if the list is empty.
//   3. many_nodes is the number of nodes in the list.
//   4. Every node in the list was allocated with new and is owned by this object.
//   5. The counters hold the totals across every linked_list since the
//      program started or the last reset_counters().

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

        // linked_list &operator=(const linked_list &src)
        //   Postcondition: This list's old nodes have been deleted and it is now a
        //                  deep copy of src. Returns *this so assignments can chain.
        linked_list &operator=(const linked_list &src);

        // void head_insert(const node::value_type &entry)
        //   Postcondition: A new node holding entry is at the front of the list.
        void head_insert(const node::value_type &entry);

        // void insert(node *previous_ptr, const node::value_type &entry)
        //   Precondition:  previous_ptr points to a node in this list.
        //   Postcondition: A new node holding entry has been inserted directly
        //                  after the node that previous_ptr points to. If that
        //                  was the last node, the new node is now the tail.
        void insert(node *previous_ptr, const node::value_type &entry);

        // void head_remove()
        //   Precondition:  The list is not empty.
        //   Postcondition: The first node has been deleted.
        void head_remove();

        // void remove(node *previous_ptr)
        //   Precondition:  previous_ptr points to a node in this list, and that
        //                  node is not the last node (it has a successor).
        //   Postcondition: The node after previous_ptr has been deleted and
        //                  previous_ptr now links to the node that followed it.
        //                  If the deleted node was the tail, previous_ptr's
        //                  node is now the tail.
        void remove(node *previous_ptr);

        // void clear()
        //   Postcondition: Every node has been deleted and the list is empty.
        void clear();

        // node *search(const node::value_type &target)
        //   Postcondition: Returns a pointer to the first node whose data equals
        //                  target, or NULL if no such node exists.
        node *search(const node::value_type &target);

        // node *locate(size_t position)
        //   Precondition:  position > 0. Positions are 1-based.
        //   Postcondition: Returns a pointer to the node at the given position,
        //                  or NULL if the list has fewer than position nodes.
        node *locate(size_t position);

        // node *get_head()
        //   Postcondition: Returns a pointer to the first node, or NULL if empty.
        node *get_head();

        // node *get_tail()
        //   Postcondition: Returns a pointer to the last node, or NULL if empty.
        node *get_tail();

        // Constant member functions

        // size_t length() const
        //   Postcondition: Returns the number of nodes in the list.
        size_t length() const;

        // bool empty() const
        //   Postcondition: Returns true if the list has no nodes.
        bool empty() const;

        // const node *search(const node::value_type &target) const
        //   Same as above, but for read-only lists; returns a const pointer.
        const node *search(const node::value_type &target) const;

        // const node *locate(size_t position) const
        //   Same as above, but for read-only lists; returns a const pointer.
        const node *locate(size_t position) const;

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

    private:
        node *head_ptr;
        node *tail_ptr;
        size_t many_nodes;

        // The counters are static: one shared set for every linked_list, so
        // STATS gets the total across the app's many small lists.
        static size_t comparisons;
        static size_t nodes_visited;
        static size_t allocations;
        static size_t deallocations;

        // void copy_from(const node *source_ptr)
        //   Precondition:  This list is empty.
        //   Postcondition: This list holds the same values as the list at
        //                  source_ptr, in the same order, in new nodes.
        void copy_from(const node *source_ptr);
    };
}

#endif
