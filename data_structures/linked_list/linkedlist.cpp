// FILE: linkedlist.cpp
// IMPLEMENTS: the linkedlist class and the linked list toolkit (see linkedlist.h)
//
// INVARIANT for the linkedlist class:
//   1. head_ptr points to the first node of the list, or is NULL if the list is empty.
//   2. tail_ptr points to the last node of the list, or is NULL if the list is empty.
//   3. Every node in the list was allocated with new and is owned by this object.

#include <cassert> // Provides assert
#include "linkedlist.h"

namespace alert_service
{
    // ========================================================================
    // CONSTRUCTORS AND DESTRUCTOR
    // ========================================================================

    // linkedlist()
    //   Precondition:  None.
    //   Postcondition: The list is empty (head_ptr and tail_ptr are NULL).
    linkedlist::linkedlist()
    {
        head_ptr = NULL;
        tail_ptr = NULL;
    }

    // linkedlist(const linkedlist &src)
    //   Precondition:  None.
    //   Postcondition: This list is a deep copy of src -- same values in the
    //                  same order, but in newly allocated nodes.
    linkedlist::linkedlist(const linkedlist &src)
    {
        list_copy(src.head_ptr, head_ptr, tail_ptr);
    }

    // ~linkedlist()
    //   Precondition:  None.
    //   Postcondition: Every node owned by this list has been deleted.
    linkedlist::~linkedlist()
    {
        list_clear(head_ptr);
        tail_ptr = NULL;
    }

    // ========================================================================
    // MODIFICATION MEMBER FUNCTIONS
    // ========================================================================

    // node *get_head()
    //   Precondition:  None.
    //   Postcondition: Returns a pointer to the first node, or NULL if empty.
    node *linkedlist::get_head()
    {
        return head_ptr;
    }

    // node *get_tail()
    //   Precondition:  None.
    //   Postcondition: Returns a pointer to the last node, or NULL if empty.
    node *linkedlist::get_tail()
    {
        return tail_ptr;
    }

    // linkedlist &operator=(const linkedlist &src)
    //   Precondition:  None.
    //   Postcondition: This list's old nodes have been deleted and it is now a
    //                  deep copy of src. Returns *this so assignments can chain.
    linkedlist &linkedlist::operator=(const linkedlist &src)
    {
        // Self-assignment guard: without it, list_clear would delete src's
        // nodes before list_copy got a chance to read them.
        if (this == &src)
            return *this;

        list_clear(head_ptr);
        list_copy(src.head_ptr, head_ptr, tail_ptr);
        return *this;
    }

    // ========================================================================
    // CONSTANT MEMBER FUNCTIONS
    // ========================================================================

    // const node *get_head() const
    //   Precondition:  None.
    //   Postcondition: Returns a read-only pointer to the first node, or NULL.
    const node *linkedlist::get_head() const
    {
        return head_ptr;
    }

    // const node *get_tail() const
    //   Precondition:  None.
    //   Postcondition: Returns a read-only pointer to the last node, or NULL.
    const node *linkedlist::get_tail() const
    {
        return tail_ptr;
    }

    // ========================================================================
    // LINKED LIST TOOLKIT FUNCTIONS
    // ========================================================================

    // size_t list_length(const node *head_ptr)
    //   Precondition:  head_ptr is the head pointer of a linked list (may be NULL).
    //   Postcondition: Returns the number of nodes in the list.
    size_t list_length(const node *head_ptr)
    {
        size_t answer = 0;

        for (const node *cursor = head_ptr; cursor != NULL; cursor = cursor->link())
            ++answer;

        return answer;
    }

    // void list_head_insert(node *&head_ptr, const node::value_type &entry)
    //   Precondition:  head_ptr is the head pointer of a linked list (may be NULL).
    //   Postcondition: A new node holding entry is at the front of the list,
    //                  and head_ptr now points to it.
    void list_head_insert(node *&head_ptr, const node::value_type &entry)
    {
        head_ptr = new node(entry, head_ptr);
    }

    // void list_insert(node *previous_ptr, const node::value_type &entry)
    //   Precondition:  previous_ptr points to a node in a linked list.
    //   Postcondition: A new node holding entry has been inserted directly
    //                  after the node that previous_ptr points to.
    void list_insert(node *previous_ptr, const node::value_type &entry)
    {
        assert(previous_ptr != NULL);

        node *insert_ptr = new node(entry, previous_ptr->link());
        previous_ptr->set_link(insert_ptr);
    }

    // node *list_search(node *head_ptr, const node::value_type &target)
    //   Precondition:  head_ptr is the head pointer of a linked list (may be NULL).
    //   Postcondition: Returns a pointer to the first node whose data equals
    //                  target, or NULL if no such node exists.
    node *list_search(node *head_ptr, const node::value_type &target)
    {
        for (node *cursor = head_ptr; cursor != NULL; cursor = cursor->link())
            if (target == cursor->data())
                return cursor;

        return NULL;
    }

    // const node *list_search(const node *head_ptr, const node::value_type &target)
    //   Same as above, but for read-only lists; returns a const pointer.
    const node *list_search(const node *head_ptr, const node::value_type &target)
    {
        for (const node *cursor = head_ptr; cursor != NULL; cursor = cursor->link())
            if (target == cursor->data())
                return cursor;

        return NULL;
    }

    // node *list_locate(node *head_ptr, size_t position)
    //   Precondition:  head_ptr is the head pointer of a linked list (may be NULL),
    //                  and position > 0. Positions are 1-based.
    //   Postcondition: Returns a pointer to the node at the given position,
    //                  or NULL if the list has fewer than position nodes.
    node *list_locate(node *head_ptr, size_t position)
    {
        assert(0 < position);

        node *cursor = head_ptr;
        for (size_t i = 1; (i < position) && (cursor != NULL); ++i)
            cursor = cursor->link();

        return cursor;
    }

    // const node *list_locate(const node *head_ptr, size_t position)
    //   Same as above, but for read-only lists; returns a const pointer.
    const node *list_locate(const node *head_ptr, size_t position)
    {
        assert(0 < position);

        const node *cursor = head_ptr;
        for (size_t i = 1; (i < position) && (cursor != NULL); ++i)
            cursor = cursor->link();

        return cursor;
    }

    // void list_head_remove(node *&head_ptr)
    //   Precondition:  head_ptr is the head pointer of a non-empty linked list.
    //   Postcondition: The first node has been deleted, and head_ptr now points
    //                  to the new first node (or NULL if the list became empty).
    void list_head_remove(node *&head_ptr)
    {
        assert(head_ptr != NULL);

        node *remove_ptr = head_ptr;
        head_ptr = head_ptr->link();
        delete remove_ptr;
    }

    // void list_remove(node *previous_ptr)
    //   Precondition:  previous_ptr points to a node in a linked list, and that
    //                  node is not the last node (it has a successor).
    //   Postcondition: The node after previous_ptr has been deleted and
    //                  previous_ptr now links to the node that followed it.
    void list_remove(node *previous_ptr)
    {
        assert(previous_ptr != NULL);
        assert(previous_ptr->link() != NULL);

        node *remove_ptr = previous_ptr->link();
        previous_ptr->set_link(remove_ptr->link());
        delete remove_ptr;
    }

    // void list_clear(node *&head_ptr)
    //   Precondition:  head_ptr is the head pointer of a linked list (may be NULL).
    //   Postcondition: Every node has been deleted and head_ptr is NULL.
    void list_clear(node *&head_ptr)
    {
        while (head_ptr != NULL)
            list_head_remove(head_ptr);
    }

    // void list_copy(const node *source_ptr, node *&head_ptr, node *&tail_ptr)
    //   Precondition:  source_ptr is the head pointer of a linked list (may be NULL).
    //   Postcondition: head_ptr and tail_ptr are the head and tail of a new list
    //                  holding the same values as the source, in the same order.
    //                  If the source is empty, both are NULL.
    //   Note: head_ptr and tail_ptr are overwritten, not freed -- the caller must
    //         clear any list they previously pointed to, or it will leak.
    void list_copy(const node *source_ptr, node *&head_ptr, node *&tail_ptr)
    {
        head_ptr = NULL;
        tail_ptr = NULL;

        if (source_ptr == NULL)
            return;

        // The first node goes in with head_insert; every later node is appended
        // after tail_ptr, which keeps the copy in the same order as the source.
        list_head_insert(head_ptr, source_ptr->data());
        tail_ptr = head_ptr;

        for (source_ptr = source_ptr->link(); source_ptr != NULL; source_ptr = source_ptr->link())
        {
            list_insert(tail_ptr, source_ptr->data());
            tail_ptr = tail_ptr->link();
        }
    }
}
