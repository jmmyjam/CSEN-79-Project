// FILE: stl_topic_tree.h
// PROVIDES: the stl_topic_tree<Item> template class, the alternative to
//           topic_tree<Item> that --topics=alt switches to. It has the same
//           functions the app calls on a topic_tree and hands back the same
//           topic_node<Item> objects, but finds each topic by looking up its
//           whole path ("news/sports") in a std::map, instead of walking
//           down the tree one name at a time.
//
// Item is the type stored for each subscriber (see topic_node.h).
//
// INVARIANT for the stl_topic_tree class:
//   1. topics maps each topic's full path, written without extra '/'
//      characters ("news/sports"), to that topic's node.
//   2. If a path is in topics, every path above it is too, so path_nodes
//      gives the same answer as topic_tree::path_nodes.
//   3. Every node was allocated with new and is owned by this object.
//   4. counters holds the totals across every stl_topic_tree<Item> (for one
//      Item type) since the program started or the last reset_counters().

#ifndef STL_TOPIC_TREE_H
#define STL_TOPIC_TREE_H

#include <cassert>  // Provides assert
#include <cstdlib>  // Provides size_t and NULL
#include <iostream> // Provides ostream
#include <map>      // Provides map
#include <string>   // Provides string
#include "../kit/counters.h"
#include "linked_list/linked_list.h"
#include "topic_tree/topic_node.h"
#include "topic_tree/topic_tree.h" // Provides topic_tree<Item>::next_name

namespace alert_service
{
    template <class Item>
    class stl_topic_tree
    {
    public:
        // TYPEDEF
        typedef std::map<std::string, topic_node<Item> *> map_type;

        // CONSTRUCTOR AND DESTRUCTOR

        // stl_topic_tree()
        //   Postcondition: The tree has no topics.
        stl_topic_tree() {}

        // ~stl_topic_tree()
        //   Postcondition: Every node owned by this tree has been deleted.
        ~stl_topic_tree()
        {
            for (typename map_type::iterator it = topics.begin(); it != topics.end(); ++it)
            {
                delete it->second;
                KIT_COUNT_EXTRA(counters, "deallocations");
            }
        }

        // topic_node<Item> *find_or_create(const std::string &path)
        //   Same as topic_tree::find_or_create.
        topic_node<Item> *find_or_create(const std::string &path)
        {
            topic_node<Item> *cursor = NULL;
            std::string prefix;
            std::string name;
            size_t start = 0;

            // Every topic above path is created too (invariant 2).
            while (topic_tree<Item>::next_name(path, start, name))
            {
                prefix = prefix.empty() ? name : prefix + "/" + name;

                KIT_COUNT(counters, visits);
                typename map_type::iterator it = topics.find(prefix);
                if (it != topics.end())
                    cursor = it->second;
                else
                {
                    cursor = new topic_node<Item>(name);
                    KIT_COUNT(counters, allocations);
                    topics[prefix] = cursor;
                }
            }

            assert(cursor != NULL);
            return cursor;
        }

        // topic_node<Item> *find(const std::string &path)
        //   Same as topic_tree::find.
        topic_node<Item> *find(const std::string &path)
        {
            std::string prefix;
            std::string name;
            size_t start = 0;

            while (topic_tree<Item>::next_name(path, start, name))
                prefix = prefix.empty() ? name : prefix + "/" + name;

            if (prefix.empty())
                return NULL;

            KIT_COUNT(counters, visits);
            typename map_type::iterator it = topics.find(prefix);
            if (it == topics.end())
                return NULL;
            return it->second;
        }

        // void path_nodes(const std::string &path, linked_list<topic_node<Item> *> &result)
        //   Same as topic_tree::path_nodes.
        void path_nodes(const std::string &path, linked_list<topic_node<Item> *> &result)
        {
            std::string prefix;
            std::string name;
            size_t start = 0;

            result.clear();

            while (topic_tree<Item>::next_name(path, start, name))
            {
                prefix = prefix.empty() ? name : prefix + "/" + name;

                KIT_COUNT(counters, visits);
                typename map_type::iterator it = topics.find(prefix);
                if (it == topics.end())
                    return; // invariant 2: nothing below a missing topic exists

                if (result.empty())
                    result.head_insert(it->second);
                else
                    result.insert(result.get_tail(), it->second);
            }
        }

        // size_t size() const
        //   Postcondition: Returns the number of topics in the tree.
        size_t size() const { return topics.size(); }

        // bool empty() const
        //   Postcondition: Returns true if the tree has no topics.
        bool empty() const { return topics.empty(); }

        // static const kit::Counters &get_counters()
        //   Postcondition: Returns the counters (see kit/counters.h):
        //                  visits       - full paths looked up in the map
        //                  allocations  - topic nodes allocated with new
        //                  extra["deallocations"] - topic nodes deleted
        //                  comparisons are not counted: std::map does them
        //                  inside its own code, where they cannot be seen.
        static const kit::Counters &get_counters() { return counters; }

        // static void reset_counters()
        //   Postcondition: Every counter is 0. No tree's topics are changed.
        static void reset_counters() { counters.reset(); }

        // static void print_counters(std::ostream &out)
        //   Precondition:  out is open for writing.
        //   Postcondition: Every counter's name and value has been written to out.
        static void print_counters(std::ostream &out) { counters.print(out, "stl_topic_tree"); }

    private:
        map_type topics;
        static kit::Counters counters;

        // Declared but never defined, so copying will not compile: both
        // copies would own, and later delete, the same nodes.
        stl_topic_tree(const stl_topic_tree &source);
        stl_topic_tree &operator=(const stl_topic_tree &source);
    };

    template <class Item>
    kit::Counters stl_topic_tree<Item>::counters;
}

#endif
