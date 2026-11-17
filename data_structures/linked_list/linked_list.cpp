#include <cassert> // Provides assert
#include "linked_list.h"

namespace alert_service
{
    size_t linked_list::comparisons = 0;
    size_t linked_list::nodes_visited = 0;
    size_t linked_list::allocations = 0;
    size_t linked_list::deallocations = 0;

    linked_list::linked_list()
    {
        head_ptr = NULL;
        tail_ptr = NULL;
        many_nodes = 0;
    }

    linked_list::linked_list(const linked_list &src)
    {
        head_ptr = NULL;
        tail_ptr = NULL;
        many_nodes = 0;
        copy_from(src.head_ptr);
    }

    linked_list::~linked_list()
    {
        clear();
    }

    linked_list &linked_list::operator=(const linked_list &src)
    {
        // Self-assignment guard: without it, clear would delete src's
        // nodes before copy_from got a chance to read them.
        if (this == &src)
            return *this;

        clear();
        copy_from(src.head_ptr);
        return *this;
    }

    void linked_list::head_insert(const node::value_type &entry)
    {
        head_ptr = new node(entry, head_ptr);
        ++allocations;
        ++many_nodes;

        if (tail_ptr == NULL)
            tail_ptr = head_ptr;
    }

    void linked_list::insert(node *previous_ptr, const node::value_type &entry)
    {
        assert(previous_ptr != NULL);

        node *insert_ptr = new node(entry, previous_ptr->link());
        ++allocations;
        ++many_nodes;
        previous_ptr->set_link(insert_ptr);

        if (previous_ptr == tail_ptr)
            tail_ptr = insert_ptr;
    }

    void linked_list::head_remove()
    {
        assert(head_ptr != NULL);

        node *remove_ptr = head_ptr;
        head_ptr = head_ptr->link();
        delete remove_ptr;
        ++deallocations;
        --many_nodes;

        if (head_ptr == NULL)
            tail_ptr = NULL;
    }

    void linked_list::remove(node *previous_ptr)
    {
        assert(previous_ptr != NULL);
        assert(previous_ptr->link() != NULL);

        node *remove_ptr = previous_ptr->link();
        previous_ptr->set_link(remove_ptr->link());

        if (remove_ptr == tail_ptr)
            tail_ptr = previous_ptr;

        delete remove_ptr;
        ++deallocations;
        --many_nodes;
    }

    void linked_list::clear()
    {
        while (head_ptr != NULL)
            head_remove();
    }

    node *linked_list::search(const node::value_type &target)
    {
        for (node *cursor = head_ptr; cursor != NULL; cursor = cursor->link())
        {
            ++nodes_visited;
            ++comparisons;
            if (target == cursor->data())
                return cursor;
        }

        return NULL;
    }

    node *linked_list::locate(size_t position)
    {
        assert(0 < position);

        node *cursor = head_ptr;
        for (size_t i = 1; (i < position) && (cursor != NULL); ++i)
        {
            ++nodes_visited;
            cursor = cursor->link();
        }

        return cursor;
    }

    node *linked_list::get_head()
    {
        return head_ptr;
    }

    node *linked_list::get_tail()
    {
        return tail_ptr;
    }

    size_t linked_list::length() const
    {
        return many_nodes;
    }

    bool linked_list::empty() const
    {
        return head_ptr == NULL;
    }

    const node *linked_list::search(const node::value_type &target) const
    {
        for (const node *cursor = head_ptr; cursor != NULL; cursor = cursor->link())
        {
            ++nodes_visited;
            ++comparisons;
            if (target == cursor->data())
                return cursor;
        }

        return NULL;
    }

    const node *linked_list::locate(size_t position) const
    {
        assert(0 < position);

        const node *cursor = head_ptr;
        for (size_t i = 1; (i < position) && (cursor != NULL); ++i)
        {
            ++nodes_visited;
            cursor = cursor->link();
        }

        return cursor;
    }

    const node *linked_list::get_head() const
    {
        return head_ptr;
    }

    const node *linked_list::get_tail() const
    {
        return tail_ptr;
    }

    size_t linked_list::get_comparisons()
    {
        return comparisons;
    }

    size_t linked_list::get_nodes_visited()
    {
        return nodes_visited;
    }

    size_t linked_list::get_allocations()
    {
        return allocations;
    }

    size_t linked_list::get_deallocations()
    {
        return deallocations;
    }

    void linked_list::reset_counters()
    {
        comparisons = 0;
        nodes_visited = 0;
        allocations = 0;
        deallocations = 0;
    }

    void linked_list::print_counters(std::ostream &out)
    {
        out << "linked_list comparisons=" << comparisons
            << " nodes_visited=" << nodes_visited
            << " allocations=" << allocations
            << " deallocations=" << deallocations << std::endl;
    }

    void linked_list::copy_from(const node *source_ptr)
    {
        if (source_ptr == NULL)
            return;

        // The first node goes in with head_insert; every later node is appended
        // after tail_ptr, which keeps the copy in the same order as the source.
        head_insert(source_ptr->data());
        ++nodes_visited;

        for (source_ptr = source_ptr->link(); source_ptr != NULL; source_ptr = source_ptr->link())
        {
            ++nodes_visited;
            insert(tail_ptr, source_ptr->data());
        }
    }
}
