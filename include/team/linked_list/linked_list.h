// FILE: linked_list.h
// PROVIDES: the linked_list<Item> template class, a singly linked list of
//           Item values (see node.h).
//
// Item may be any type with a default constructor, a copy constructor, an
// assignment operator, and operator== (used by search).
//
// INVARIANT for the linked_list class:
//   1. head_ptr points to the first node of the list, or is NULL if the list is empty.
//   2. tail_ptr points to the last node of the list, or is NULL if the list is empty.
//   3. many_nodes is the number of nodes in the list.
//   4. Every node in the list was allocated with new and is owned by this object.
//   5. counters holds the totals across every linked_list<Item> (for one
//      Item type) since the program started or the last reset_counters().

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <cstdlib>  // Provides size_t and NULL
#include <iostream> // Provides ostream
#include "kit/counters.h"
#include "node.h"

namespace alert_service
{
    template <class Item>
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

        // void head_insert(const Item &entry)
        //   Postcondition: A new node holding entry is at the front of the list.
        void head_insert(const Item &entry);

        // void insert(node<Item> *previous_ptr, const Item &entry)
        //   Precondition:  previous_ptr points to a node in this list.
        //   Postcondition: A new node holding entry has been inserted directly
        //                  after the node that previous_ptr points to. If that
        //                  was the last node, the new node is now the tail.
        void insert(node<Item> *previous_ptr, const Item &entry);

        // void head_remove()
        //   Precondition:  The list is not empty.
        //   Postcondition: The first node has been deleted.
        void head_remove();

        // void remove(node<Item> *previous_ptr)
        //   Precondition:  previous_ptr points to a node in this list, and that
        //                  node is not the last node (it has a successor).
        //   Postcondition: The node after previous_ptr has been deleted and
        //                  previous_ptr now links to the node that followed it.
        //                  If the deleted node was the tail, previous_ptr's
        //                  node is now the tail.
        void remove(node<Item> *previous_ptr);

        // void clear()
        //   Postcondition: Every node has been deleted and the list is empty.
        void clear();

        // node<Item> *search(const Item &target)
        //   Postcondition: Returns a pointer to the first node whose data equals
        //                  target, or NULL if no such node exists.
        node<Item> *search(const Item &target);

        // node<Item> *locate(size_t position)
        //   Precondition:  position > 0. Positions are 1-based.
        //   Postcondition: Returns a pointer to the node at the given position,
        //                  or NULL if the list has fewer than position nodes.
        node<Item> *locate(size_t position);

        // node<Item> *get_head()
        //   Postcondition: Returns a pointer to the first node, or NULL if empty.
        node<Item> *get_head();

        // node<Item> *get_tail()
        //   Postcondition: Returns a pointer to the last node, or NULL if empty.
        node<Item> *get_tail();

        // Constant member functions

        // size_t length() const
        //   Postcondition: Returns the number of nodes in the list.
        size_t length() const;

        // bool empty() const
        //   Postcondition: Returns true if the list has no nodes.
        bool empty() const;

        // const node<Item> *search(const Item &target) const
        //   Same as above, but for read-only lists; returns a const pointer.
        const node<Item> *search(const Item &target) const;

        // const node<Item> *locate(size_t position) const
        //   Same as above, but for read-only lists; returns a const pointer.
        const node<Item> *locate(size_t position) const;

        // const node<Item> *get_head() const
        //   Postcondition: Returns a read-only pointer to the first node, or NULL.
        const node<Item> *get_head() const;

        // const node<Item> *get_tail() const
        //   Postcondition: Returns a read-only pointer to the last node, or NULL.
        const node<Item> *get_tail() const;

        // Counter functions

        // static const kit::Counters &get_counters()
        //   Postcondition: Returns the counters (see kit/counters.h):
        //                  comparisons  - items compared by search
        //                  visits       - nodes a cursor has moved to
        //                  allocations  - nodes allocated with new
        //                  extra["deallocations"] - nodes deleted
        static const kit::Counters &get_counters();

        // static void reset_counters()
        //   Postcondition: Every counter is 0. No list's nodes are changed.
        static void reset_counters();

        // static void print_counters(std::ostream &out)
        //   Precondition:  out is open for writing.
        //   Postcondition: Every counter's name and value has been written to out.
        static void print_counters(std::ostream &out);

    private:
        node<Item> *head_ptr;
        node<Item> *tail_ptr;
        size_t many_nodes;

        // Static: one shared set for every linked_list of the same Item type,
        // so STATS gets the total across the app's many small lists, and
        // const functions like search() can still update it.
        // linked_list<int> and linked_list<double> count separately.
        static kit::Counters counters;

        // void copy_from(const node<Item> *source_ptr)
        //   Precondition:  This list is empty.
        //   Postcondition: This list holds the same values as the list at
        //                  source_ptr, in the same order, in new nodes.
        void copy_from(const node<Item> *source_ptr);
    };
}

// A template's function bodies must be visible wherever the class is used,
// so the implementation is included here instead of compiled on its own.
#include "linked_list.template"

#endif
