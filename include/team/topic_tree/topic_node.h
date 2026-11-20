// FILE: topic_node.h
// PROVIDES: the topic_node<Item> template class, one topic in a topic tree.
//           Each node holds the topic's name (one segment of a path, such as
//           "sports"), a list of its child topics, and a list of the
//           subscribers to exactly this topic.
//
// Item is the type stored for each subscriber, such as a user name. It may be
// any type with a default constructor, a copy constructor, an assignment
// operator, and operator== (see linked_list.h).

#ifndef TOPIC_NODE_H
#define TOPIC_NODE_H

#include <string> // Provides string
#include "../linked_list/linked_list.h"

namespace alert_service
{
    template <class Item>
    class topic_node
    {
    public:
        // TYPEDEF
        typedef Item value_type;

        // CONSTRUCTOR

        // topic_node(const std::string &init_name)
        //   Postcondition: The node is named init_name and has no children
        //                  and no subscribers.
        topic_node(const std::string &init_name = "")
        {
            name_field = init_name;
        }

        // Member functions

        // linked_list<topic_node *> &children()
        //   Postcondition: Returns this node's list of child topics, which can
        //                  be changed through it.
        linked_list<topic_node *> &children() { return children_field; }

        // linked_list<Item> &subscribers()
        //   Postcondition: Returns this node's list of subscribers, which can
        //                  be changed through it.
        linked_list<Item> &subscribers() { return subscribers_field; }

        // Constant member functions

        // const std::string &name() const
        //   Postcondition: Returns this topic's name.
        const std::string &name() const { return name_field; }

        // const linked_list<topic_node *> &children() const
        //   Postcondition: Returns a read-only view of this node's child topics.
        const linked_list<topic_node *> &children() const { return children_field; }

        // const linked_list<Item> &subscribers() const
        //   Postcondition: Returns a read-only view of this node's subscribers.
        const linked_list<Item> &subscribers() const { return subscribers_field; }

    private:
        std::string name_field;
        // Holds pointers, not nodes: the topic_tree that made the children
        // owns them and deletes them, not this list.
        linked_list<topic_node *> children_field;
        linked_list<Item> subscribers_field;

        // Declared but never defined, so copying a topic_node will not
        // compile. A copy would share the same child pointers, and both
        // copies would later be deleted by the tree.
        topic_node(const topic_node &source);
        topic_node &operator=(const topic_node &source);
    };
}

#endif
