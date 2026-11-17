// FILE: node.h
// PROVIDES: the node class, one node of a singly linked list. Each node holds
//           an item and a link to the next node (NULL at the end of the list).

#ifndef NODE_H
#define NODE_H

#include <cstdlib> // Provides size_t and NULL

namespace alert_service
{
    class node
    {
    public:
        // TYPEDEF
        typedef double value_type;

        // CONSTRUCTOR

        // node(const value_type &init_data, node *init_link)
        //   Postcondition: The node holds init_data and links to init_link.
        node(const value_type &init_data = value_type(), node *init_link = NULL)
        {
            data_field = init_data;
            link_field = init_link;
        }

        // Member functions

        // void set_data(const value_type &new_data)
        //   Postcondition: This node's item is now new_data.
        void set_data(const value_type &new_data) { data_field = new_data; }

        // void set_link(node *new_link)
        //   Postcondition: This node now links to new_link.
        void set_link(node *new_link) { link_field = new_link; }

        // node *link()
        //   Postcondition: Returns a pointer to the next node, or NULL if this
        //                  is the last node.
        node *link() { return link_field; }

        // Constant member functions

        // value_type data() const
        //   Postcondition: Returns a copy of this node's item.
        value_type data() const { return data_field; }

        // const node *link() const
        //   Postcondition: Returns a read-only pointer to the next node, or NULL.
        const node *link() const { return link_field; }

    private:
        value_type data_field;
        node *link_field;
    };
}

#endif
