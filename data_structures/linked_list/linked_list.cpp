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
    }

    linked_list::linked_list(const linked_list &src)
    {
        list_copy(src.head_ptr, head_ptr, tail_ptr);
    }

    linked_list::~linked_list()
    {
        list_clear(head_ptr);
        tail_ptr = NULL;
    }

    node *linked_list::get_head()
    {
        return head_ptr;
    }

    node *linked_list::get_tail()
    {
        return tail_ptr;
    }

    linked_list &linked_list::operator=(const linked_list &src)
    {
        // Self-assignment guard: without it, list_clear would delete src's
        // nodes before list_copy got a chance to read them.
        if (this == &src)
            return *this;

        list_clear(head_ptr);
        list_copy(src.head_ptr, head_ptr, tail_ptr);
        return *this;
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

    void linked_list::count_comparison()
    {
        ++comparisons;
    }

    void linked_list::count_node_visit()
    {
        ++nodes_visited;
    }

    void linked_list::count_allocation()
    {
        ++allocations;
    }

    void linked_list::count_deallocation()
    {
        ++deallocations;
    }

    size_t list_length(const node *head_ptr)
    {
        size_t answer = 0;

        for (const node *cursor = head_ptr; cursor != NULL; cursor = cursor->link())
        {
            linked_list::count_node_visit();
            ++answer;
        }

        return answer;
    }

    void list_head_insert(node *&head_ptr, const node::value_type &entry)
    {
        head_ptr = new node(entry, head_ptr);
        linked_list::count_allocation();
    }

    void list_insert(node *previous_ptr, const node::value_type &entry)
    {
        assert(previous_ptr != NULL);

        node *insert_ptr = new node(entry, previous_ptr->link());
        linked_list::count_allocation();
        previous_ptr->set_link(insert_ptr);
    }

    node *list_search(node *head_ptr, const node::value_type &target)
    {
        for (node *cursor = head_ptr; cursor != NULL; cursor = cursor->link())
        {
            linked_list::count_node_visit();
            linked_list::count_comparison();
            if (target == cursor->data())
                return cursor;
        }

        return NULL;
    }

    const node *list_search(const node *head_ptr, const node::value_type &target)
    {
        for (const node *cursor = head_ptr; cursor != NULL; cursor = cursor->link())
        {
            linked_list::count_node_visit();
            linked_list::count_comparison();
            if (target == cursor->data())
                return cursor;
        }

        return NULL;
    }

    node *list_locate(node *head_ptr, size_t position)
    {
        assert(0 < position);

        node *cursor = head_ptr;
        for (size_t i = 1; (i < position) && (cursor != NULL); ++i)
        {
            linked_list::count_node_visit();
            cursor = cursor->link();
        }

        return cursor;
    }

    const node *list_locate(const node *head_ptr, size_t position)
    {
        assert(0 < position);

        const node *cursor = head_ptr;
        for (size_t i = 1; (i < position) && (cursor != NULL); ++i)
        {
            linked_list::count_node_visit();
            cursor = cursor->link();
        }

        return cursor;
    }

    void list_head_remove(node *&head_ptr)
    {
        assert(head_ptr != NULL);

        node *remove_ptr = head_ptr;
        head_ptr = head_ptr->link();
        delete remove_ptr;
        linked_list::count_deallocation();
    }

    void list_remove(node *previous_ptr)
    {
        assert(previous_ptr != NULL);
        assert(previous_ptr->link() != NULL);

        node *remove_ptr = previous_ptr->link();
        previous_ptr->set_link(remove_ptr->link());
        delete remove_ptr;
        linked_list::count_deallocation();
    }

    void list_clear(node *&head_ptr)
    {
        while (head_ptr != NULL)
            list_head_remove(head_ptr);
    }

    void list_copy(const node *source_ptr, node *&head_ptr, node *&tail_ptr)
    {
        head_ptr = NULL;
        tail_ptr = NULL;

        if (source_ptr == NULL)
            return;

        // The first node goes in with head_insert; every later node is appended
        // after tail_ptr, which keeps the copy in the same order as the source.
        list_head_insert(head_ptr, source_ptr->data());
        linked_list::count_node_visit();
        tail_ptr = head_ptr;

        for (source_ptr = source_ptr->link(); source_ptr != NULL; source_ptr = source_ptr->link())
        {
            linked_list::count_node_visit();
            list_insert(tail_ptr, source_ptr->data());
            tail_ptr = tail_ptr->link();
        }
    }
}
