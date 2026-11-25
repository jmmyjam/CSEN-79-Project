// FILE: topic_tree.h
// PROVIDES: the topic_tree<Item> template class, a tree of hierarchical topics
//           such as news/sports/soccer (see topic_node.h).
//
// A path is a list of topic names separated by '/'. Each name is one level of
// the tree: news/sports/soccer is the child "soccer" of the child "sports" of
// the top-level topic "news". Empty names are ignored, so "/news//sports/"
// means the same as "news/sports".
//
// Item is the type stored for each subscriber (see topic_node.h).
//
// INVARIANT for the topic_tree class:
//   1. root_ptr points to an unnamed root node that is not itself a topic.
//      Every top-level topic is a child of the root.
//   2. No node has two children with the same name.
//   3. many_topics is the number of nodes in the tree, not counting the root.
//   4. Every node was allocated with new and is owned by this object.
//   5. counters holds the totals across every topic_tree<Item> (for one
//      Item type) since the program started or the last reset_counters().

#ifndef TOPIC_TREE_H
#define TOPIC_TREE_H

#include <cstdlib>  // Provides size_t and NULL
#include <iostream> // Provides ostream
#include <string>   // Provides string
#include "kit/counters.h"
#include "../linked_list/linked_list.h"
#include "topic_node.h"

namespace alert_service
{
    template <class Item>
    class topic_tree
    {
    public:
        // CONSTRUCTOR AND DESTRUCTOR

        // topic_tree()
        //   Postcondition: The tree has no topics.
        topic_tree();

        // ~topic_tree()
        //   Postcondition: Every node owned by this tree has been deleted.
        ~topic_tree();

        // Modification member functions

        // topic_node<Item> *find_or_create(const std::string &path)
        //   Precondition:  path has at least one topic name.
        //   Postcondition: Every topic along path exists, creating any that
        //                  were missing. Returns a pointer to the last one.
        //                  Used by SUB.
        topic_node<Item> *find_or_create(const std::string &path);

        // topic_node<Item> *find(const std::string &path)
        //   Postcondition: Returns a pointer to the topic at path, or NULL if
        //                  it does not exist or path has no topic names. The
        //                  tree is unchanged. Used by UNSUB.
        topic_node<Item> *find(const std::string &path);

        // void path_nodes(const std::string &path, linked_list<topic_node<Item> *> &result)
        //   Postcondition: result's old contents have been removed, and it now
        //                  holds the topics along path that exist, from the
        //                  top-level topic down. It stops at the first topic
        //                  that does not exist, so it may be shorter than the
        //                  path or empty. The tree is unchanged. Used by PUB:
        //                  these are the topics whose subscribers receive it.
        void path_nodes(const std::string &path, linked_list<topic_node<Item> *> &result);

        // Constant member functions

        // size_t size() const
        //   Postcondition: Returns the number of topics in the tree.
        size_t size() const;

        // bool empty() const
        //   Postcondition: Returns true if the tree has no topics.
        bool empty() const;

        // bool check_invariants() const
        //   Postcondition: Returns true if invariants 1 to 3 hold, and every
        //                  topic's child and subscriber lists pass their own
        //                  check_invariants(). The counters are not changed.
        //                  Used by the tests.
        bool check_invariants() const;

        // static bool next_name(const std::string &path, size_t &start, std::string &name)
        //   Precondition:  start <= path.length().
        //   Postcondition: If path has another topic name at or after start,
        //                  it has been copied into name, start has moved past
        //                  it, and true is returned. Otherwise returns false.
        //                  Public so stl_topic_tree splits paths the same way.
        static bool next_name(const std::string &path, size_t &start, std::string &name);

        // Counter functions

        // static const kit::Counters &get_counters()
        //   Postcondition: Returns the counters (see kit/counters.h):
        //                  comparisons  - topic names compared
        //                  visits       - child topics examined while
        //                                 searching for a name
        //                  allocations  - topic nodes allocated with new,
        //                                 including each tree's root
        //                  extra["deallocations"] - topic nodes deleted
        static const kit::Counters &get_counters();

        // static void reset_counters()
        //   Postcondition: Every counter is 0. No tree's topics are changed.
        static void reset_counters();

        // static void print_counters(std::ostream &out)
        //   Precondition:  out is open for writing.
        //   Postcondition: Every counter's name and value has been written to out.
        static void print_counters(std::ostream &out);

    private:
        topic_node<Item> *root_ptr;
        size_t many_topics;

        // Static: one shared set for every topic_tree of the same Item type.
        // It counts only the tree's own work; the linked lists inside each
        // node keep their own counters (see linked_list.h).
        static kit::Counters counters;

        // Declared but never defined, so copying a topic_tree will not
        // compile. A copy would share every node with the original, and both
        // destructors would delete them.
        topic_tree(const topic_tree &source);
        topic_tree &operator=(const topic_tree &source);

        // Helpers

        // topic_node<Item> *find_child(topic_node<Item> *parent, const std::string &name)
        //   Precondition:  parent is not NULL.
        //   Postcondition: Returns a pointer to parent's child with the given
        //                  name, or NULL if it has none.
        topic_node<Item> *find_child(topic_node<Item> *parent, const std::string &name);

        // void clear_subtree(topic_node<Item> *subtree_ptr)
        //   Precondition:  subtree_ptr is not NULL.
        //   Postcondition: subtree_ptr's node and every node below it have
        //                  been deleted.
        void clear_subtree(topic_node<Item> *subtree_ptr);

        // bool check_subtree(const topic_node<Item> *subtree_ptr, size_t &count) const
        //   Precondition:  subtree_ptr is not NULL.
        //   Postcondition: Returns true if every topic below subtree_ptr has a
        //                  valid name, no two children of one topic share a
        //                  name, and every list is valid. count has gone up by
        //                  the number of topics below subtree_ptr.
        bool check_subtree(const topic_node<Item> *subtree_ptr, size_t &count) const;
    };
}

// A template's function bodies must be visible wherever the class is used,
// so the implementation is included here instead of compiled on its own.
#include "topic_tree.template"

#endif
