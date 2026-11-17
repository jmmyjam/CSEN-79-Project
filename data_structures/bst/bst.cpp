#include <cassert> // Provides assert
#include "bst.h"

namespace alert_service
{
    size_t bst::comparisons = 0;
    size_t bst::nodes_visited = 0;
    size_t bst::allocations = 0;
    size_t bst::deallocations = 0;

    bst::bst()
    {
        root_ptr = NULL;
        many_nodes = 0;
    }

    bst::bst(const bst &source)
    {
        root_ptr = copy_tree(source.root_ptr);
        many_nodes = source.many_nodes;
    }

    bst::~bst()
    {
        clear_tree(root_ptr);
        many_nodes = 0;
    }

    void bst::insert(const binary_tree_node::value_type &entry)
    {
        binary_tree_node *cursor = root_ptr;
        bool done = false;

        if (root_ptr == NULL)
        {
            root_ptr = new binary_tree_node(entry);
            ++allocations;
            ++many_nodes;
            return;
        }

        do
        {
            ++nodes_visited;
            ++comparisons;
            if (entry < cursor->data())
            { // Go left

                if (cursor->left() == NULL)
                {
                    cursor->set_left(new binary_tree_node(entry));
                    ++allocations;
                    ++many_nodes;
                    done = true;
                }
                else
                    cursor = cursor->left();
            }
            else
            {
                ++comparisons;
                if (entry > cursor->data())
                { // Go right

                    if (cursor->right() == NULL)
                    {
                        cursor->set_right(new binary_tree_node(entry));
                        ++allocations;
                        ++many_nodes;
                        done = true;
                    }
                    else
                        cursor = cursor->right();
                }
                else
                    return; // entry is already in the tree
            }
        } while (!done);
    }

    void bst::remove(const binary_tree_node::value_type &target)
    {
        if (remove_at(root_ptr, target))
            --many_nodes;
    }

    void bst::remove_max()
    {
        assert(!empty());

        binary_tree_node::value_type discarded;
        remove_max_at(root_ptr, discarded);
        --many_nodes;
    }

    bst &bst::operator=(const bst &source)
    {
        // Self-assignment guard: without it, clear_tree would delete source's
        // nodes before copy_tree got a chance to read them.
        if (this == &source)
            return *this;

        clear_tree(root_ptr);
        root_ptr = copy_tree(source.root_ptr);
        many_nodes = source.many_nodes;
        return *this;
    }

    bool bst::empty() const
    {
        return root_ptr == NULL;
    }

    binary_tree_node::value_type bst::max() const
    {
        assert(!empty());

        const binary_tree_node *cursor = root_ptr;
        ++nodes_visited;
        while (cursor->right() != NULL)
        {
            cursor = cursor->right();
            ++nodes_visited;
        }
        return cursor->data();
    }

    bool bst::contains(const binary_tree_node::value_type &target) const
    {
        const binary_tree_node *cursor = root_ptr;

        while (cursor != NULL)
        {
            ++nodes_visited;

            ++comparisons;
            if (target < cursor->data())
                cursor = cursor->left();
            else
            {
                ++comparisons;
                if (target > cursor->data())
                    cursor = cursor->right();
                else
                    return true;
            }
        }
        return false;
    }

    size_t bst::size() const
    {
        return many_nodes;
    }

    size_t bst::height() const
    {
        return subtree_height(root_ptr);
    }

    size_t bst::get_comparisons()
    {
        return comparisons;
    }

    size_t bst::get_nodes_visited()
    {
        return nodes_visited;
    }

    size_t bst::get_allocations()
    {
        return allocations;
    }

    size_t bst::get_deallocations()
    {
        return deallocations;
    }

    void bst::reset_counters()
    {
        comparisons = 0;
        nodes_visited = 0;
        allocations = 0;
        deallocations = 0;
    }

    void bst::print_counters(std::ostream &out)
    {
        out << "bst comparisons=" << comparisons
            << " nodes_visited=" << nodes_visited
            << " allocations=" << allocations
            << " deallocations=" << deallocations << std::endl;
    }

    bool bst::remove_at(binary_tree_node *&subtree_ptr, const binary_tree_node::value_type &target)
    {
        binary_tree_node *oldroot_ptr;

        if (subtree_ptr == NULL)
        { // Empty tree
            return false;
        }

        ++nodes_visited;

        ++comparisons;
        if (target < subtree_ptr->data())
        { // Continue looking in the left subtree
            return remove_at(subtree_ptr->left(), target);
        }

        ++comparisons;
        if (target > subtree_ptr->data())
        { // Continue looking in the right subtree
            return remove_at(subtree_ptr->right(), target);
        }

        // Target found
        if (subtree_ptr->left() == NULL)
        { // Target was found and there is no left subtree, so we can
          // remove this node, making the right child be the new root.

            oldroot_ptr = subtree_ptr;
            subtree_ptr = subtree_ptr->right();
            delete oldroot_ptr;
            ++deallocations;
            return true;
        }

        // If code reaches this point, then we must remove the target from
        // the current node. We'll actually replace this target with the
        // maximum item of left subtree.
        remove_max_at(subtree_ptr->left(), subtree_ptr->data());
        return true;
    }

    void bst::remove_max_at(binary_tree_node *&subtree_ptr, binary_tree_node::value_type &removed)
    {
        binary_tree_node *oldroot_ptr;

        assert(subtree_ptr != NULL);
        ++nodes_visited;

        if (subtree_ptr->right() == NULL)
        { // This node is the max. Its left child (or NULL) takes its place.
            removed = subtree_ptr->data();
            oldroot_ptr = subtree_ptr;
            subtree_ptr = subtree_ptr->left();
            delete oldroot_ptr;
            ++deallocations;
        }
        else
            remove_max_at(subtree_ptr->right(), removed);
    }

    size_t bst::subtree_height(const binary_tree_node *subtree_ptr) const
    {
        if (subtree_ptr == NULL)
            return 0;

        size_t left_height = subtree_height(subtree_ptr->left());
        size_t right_height = subtree_height(subtree_ptr->right());

        if (left_height > right_height)
            return 1 + left_height;
        else
            return 1 + right_height;
    }

    binary_tree_node *bst::copy_tree(const binary_tree_node *source_ptr)
    {
        binary_tree_node *l_ptr;
        binary_tree_node *r_ptr;

        if (source_ptr == NULL)
            return NULL;

        l_ptr = copy_tree(source_ptr->left());
        r_ptr = copy_tree(source_ptr->right());
        ++allocations;
        return new binary_tree_node(source_ptr->data(), l_ptr, r_ptr);
    }

    void bst::clear_tree(binary_tree_node *&subtree_ptr)
    {
        if (subtree_ptr == NULL)
            return;

        clear_tree(subtree_ptr->left());
        clear_tree(subtree_ptr->right());
        delete subtree_ptr;
        ++deallocations;
        subtree_ptr = NULL;
    }
}
