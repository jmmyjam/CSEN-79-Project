// FILE: binary_tree_node.h
// PROVIDES: the binary_tree_node<Item> template class, one node of a binary
//           tree. Each node holds an item and links to its left and right
//           children.
//
// Item may be any type with a default constructor, a copy constructor, and
// an assignment operator.

#ifndef BINARY_TREE_NODE_H
#define BINARY_TREE_NODE_H

#include <cstdlib> // Provides size_t and NULL

namespace alert_service
{
    template <class Item>
    class binary_tree_node
    {
    public:
        // TYPEDEF
        typedef Item value_type;

        // CONSTRUCTOR

        // binary_tree_node(init_data, init_left_link, init_right_link)
        //   Postcondition: The node holds init_data, and its children are
        //                  init_left_link and init_right_link.
        binary_tree_node(const Item &init_data = Item(), binary_tree_node *init_left_link = NULL, binary_tree_node *init_right_link = NULL)
        {
            data_field = init_data;
            left_field = init_left_link;
            right_field = init_right_link;
        }

        // Member functions

        // Item &data()
        //   Postcondition: Returns a reference to this node's item, which can
        //                  be changed through it.
        Item &data() { return data_field; }

        // void set_data(const Item &new_data)
        //   Postcondition: This node's item is now new_data.
        void set_data(const Item &new_data) { data_field = new_data; }

        // void set_left(binary_tree_node *new_link)
        //   Postcondition: This node's left child is now new_link.
        void set_left(binary_tree_node *new_link) { left_field = new_link; }

        // void set_right(binary_tree_node *new_link)
        //   Postcondition: This node's right child is now new_link.
        void set_right(binary_tree_node *new_link) { right_field = new_link; }

        // binary_tree_node *&left()
        //   Postcondition: Returns a reference to the left child pointer, so
        //                  the caller can replace the child in place.
        binary_tree_node *&left() { return left_field; }

        // binary_tree_node *&right()
        //   Postcondition: Returns a reference to the right child pointer, so
        //                  the caller can replace the child in place.
        binary_tree_node *&right() { return right_field; }

        // Constant member functions

        // const Item &data() const
        //   Postcondition: Returns a read-only reference to this node's item.
        const Item &data() const { return data_field; }

        // const binary_tree_node *left() const
        //   Postcondition: Returns a read-only pointer to the left child, or NULL.
        const binary_tree_node *left() const { return left_field; }

        // const binary_tree_node *right() const
        //   Postcondition: Returns a read-only pointer to the right child, or NULL.
        const binary_tree_node *right() const { return right_field; }

    private:
        Item data_field;
        binary_tree_node *left_field;
        binary_tree_node *right_field;
    };
}

#endif
