// FILE: bst.h
// PROVIDES: the bst class, a binary search tree of binary_tree_node::value_type
//           items (see binary_tree_node.h).
//
// INVARIANT for the bst class:
//   1. root_ptr is the root of a binary search tree, or NULL if the tree is
//      empty: every item in a node's left subtree is less than the node's
//      item, and every item in its right subtree is greater.
//   2. many_nodes is the number of nodes in the tree.
//   3. Every node was allocated with new and is owned by this object.
//   4. The counters hold the totals across every bst since the program
//      started or the last reset_counters().

#ifndef BST_H
#define BST_H

#include <cstdlib>  // Provides size_t and NULL
#include <iostream> // Provides ostream
#include "binary_tree_node.h"

namespace alert_service
{
    class bst
    {
    public:
        // CONSTRUCTORS AND DESTRUCTOR

        // bst()
        //   Postcondition: The tree is empty.
        bst();

        // bst(const bst &source)
        //   Postcondition: This tree is a deep copy of source -- the same items
        //                  in the same shape, but in newly allocated nodes.
        bst(const bst &source);

        // ~bst()
        //   Postcondition: Every node owned by this tree has been deleted.
        ~bst();

        // Modification member functions

        // void insert(const binary_tree_node::value_type &entry)
        //   Postcondition: If entry was not already in the tree, it has been
        //                  added as a new leaf. If it was already there, the
        //                  tree is unchanged.
        void insert(const binary_tree_node::value_type &entry);

        // void remove(const binary_tree_node::value_type &target)
        //   Postcondition: If target was in the tree, it has been removed.
        //                  Otherwise the tree is unchanged.
        void remove(const binary_tree_node::value_type &target);

        // void remove_max()
        //   Precondition:  The tree is not empty.
        //   Postcondition: The largest item has been removed.
        void remove_max();

        // bst &operator=(const bst &source)
        //   Postcondition: This tree's old nodes have been deleted and it is
        //                  now a deep copy of source. Returns *this so
        //                  assignments can chain.
        bst &operator=(const bst &source);

        // Constant member functions

        // bool empty() const
        //   Postcondition: Returns true if the tree has no items.
        bool empty() const;

        // binary_tree_node::value_type max() const
        //   Precondition:  The tree is not empty.
        //   Postcondition: Returns the largest item in the tree.
        binary_tree_node::value_type max() const;

        // bool contains(const binary_tree_node::value_type &target) const
        //   Postcondition: Returns true if target is in the tree.
        bool contains(const binary_tree_node::value_type &target) const;

        // size_t size() const
        //   Postcondition: Returns the number of items in the tree.
        size_t size() const;

        // size_t height() const
        //   Postcondition: Returns the height of the tree: 0 if empty, 1 for a
        //                  single node. Takes time proportional to size(),
        //                  because no node stores its height.
        size_t height() const;

        // Counter functions

        // static size_t get_comparisons()
        //   Postcondition: Returns the number of item comparisons made.
        static size_t get_comparisons();

        // static size_t get_nodes_visited()
        //   Postcondition: Returns the number of nodes visited while searching.
        static size_t get_nodes_visited();

        // static size_t get_allocations()
        //   Postcondition: Returns the number of nodes allocated with new.
        static size_t get_allocations();

        // static size_t get_deallocations()
        //   Postcondition: Returns the number of nodes deleted.
        static size_t get_deallocations();

        // static void reset_counters()
        //   Postcondition: Every counter is 0. No tree's items are changed.
        static void reset_counters();

        // static void print_counters(std::ostream &out)
        //   Precondition:  out is open for writing.
        //   Postcondition: Every counter's name and value has been written to out.
        static void print_counters(std::ostream &out);

    private:
        binary_tree_node *root_ptr;
        size_t many_nodes;

        // The counters are static: one shared set for every bst, so STATS gets
        // the total across the app's one-tree-per-item. Being static also lets
        // const functions like contains() update them. Each one needs a
        // definition in bst.cpp (size_t bst::comparisons = 0;) or the program
        // will not link.
        static size_t comparisons;
        static size_t nodes_visited;
        static size_t allocations;
        static size_t deallocations;

        // Recursive helpers

        // bool remove_at(binary_tree_node *&subtree_ptr, const binary_tree_node::value_type &target)
        //   Postcondition: If target was in the subtree, it has been removed,
        //                  subtree_ptr points to the subtree's new root, and
        //                  true is returned. Otherwise returns false.
        bool remove_at(binary_tree_node *&subtree_ptr, const binary_tree_node::value_type &target);

        // void remove_max_at(binary_tree_node *&subtree_ptr, binary_tree_node::value_type &removed)
        //   Precondition:  subtree_ptr is not NULL.
        //   Postcondition: The largest item in the subtree has been copied into
        //                  removed and its node deleted, and subtree_ptr points
        //                  to the subtree's new root.
        void remove_max_at(binary_tree_node *&subtree_ptr, binary_tree_node::value_type &removed);

        // size_t subtree_height(const binary_tree_node *subtree_ptr) const
        //   Postcondition: Returns the height of the subtree rooted at
        //                  subtree_ptr, or 0 if subtree_ptr is NULL.
        size_t subtree_height(const binary_tree_node *subtree_ptr) const;

        // binary_tree_node *copy_tree(const binary_tree_node *source_ptr)
        //   Postcondition: Returns the root of a new tree with the same items
        //                  in the same shape as the tree at source_ptr, or
        //                  NULL if source_ptr is NULL.
        binary_tree_node *copy_tree(const binary_tree_node *source_ptr);

        // void clear_tree(binary_tree_node *&subtree_ptr)
        //   Postcondition: Every node in the subtree has been deleted and
        //                  subtree_ptr is NULL.
        void clear_tree(binary_tree_node *&subtree_ptr);
    };
}

#endif
