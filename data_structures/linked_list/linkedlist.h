#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <cstdlib> // Provides size_t and NULL
#include "node.h"

namespace alert_service
{
    class linkedlist
    {

    public:
        // CONSTRUCTORS AND DESTRUCTOR
        linkedlist();
        linkedlist(const linkedlist &src);
        ~linkedlist();

        // Modification member functions
        node *get_head();
        node *get_tail();
        linkedlist &operator=(const linkedlist &src);

        // Constant member functions
        const node *get_head() const;
        const node *get_tail() const;

    private:
        node *head_ptr;
        node *tail_ptr;
    };

    // Linked list toolkit functions
    size_t list_length(const node *head_ptr);
    void list_head_insert(node *&head_ptr, const node::value_type &entry);
    void list_insert(node *previous_ptr, const node::value_type &entry);
    node *list_search(node *head_ptr, const node::value_type &target);
    const node *list_search(const node *head_ptr, const node::value_type &target);
    node *list_locate(node *head_ptr, size_t position);
    const node *list_locate(const node *head_ptr, size_t position);
    void list_head_remove(node *&head_ptr);
    void list_remove(node *previous_ptr);
    void list_clear(node *&head_ptr);
    void list_copy(const node *source_ptr, node *&head_ptr, node *&tail_ptr);
}

#endif
