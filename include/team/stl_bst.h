#ifndef STL_BST_H
#define STL_BST_H

#include <cstdlib>
#include <iostream>
#include <set>
#include "kit/counters.h"
#include "linked_list/linked_list.h"

namespace alert_service
{
    template <class Item>
    class stl_bst
    {
    public:
        void insert(const Item &entry)
        {
            // insert(...).second is false when entry was already there, in
            // which case std::set allocated nothing.
            if (items.insert(entry).second)
                KIT_COUNT(counters, allocations);
        }

        void remove(const Item &target)
        {
            if (items.erase(target) > 0)
                KIT_COUNT_EXTRA(counters, "deallocations");
        }

        bool empty() const { return items.empty(); }
        bool contains(const Item &target) const { return items.count(target) > 0; }
        size_t size() const { return items.size(); }

        void range(const Item &low, const Item &high, linked_list<Item> &result) const
        {
            result.clear();

            // lower_bound jumps straight to the first item that is at least
            // low, the same job bst::range does by walking down the tree.
            for (typename std::set<Item>::const_iterator it = items.lower_bound(low);
                 it != items.end() && *it < high; ++it)
            {
                KIT_COUNT(counters, visits);
                if (result.empty())
                    result.head_insert(*it);
                else
                    result.insert(result.get_tail(), *it);
            }
        }

        // comparisons are not counted: std::set does them inside its own
        // code, where they cannot be seen.
        static const kit::Counters &get_counters() { return counters; }
        static void reset_counters() { counters.reset(); }
        static void print_counters(std::ostream &out) { counters.print(out, "stl_bst"); }

    private:
        std::set<Item> items;
        static kit::Counters counters;
    };

    template <class Item>
    kit::Counters stl_bst<Item>::counters;
}

#endif
