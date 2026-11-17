#include <cassert> // Provides assert
#include "bst.h"

namespace alert_service
{

    bst::bst();

    bst::bst(const bst &source);

    bst::~bst();

    void bst::insert(const binary_tree_node::value_type &entry);

    void bst::remove(const binary_tree_node::value_type &target);

    void bst::remove_max();

    bst &bst::operator=(const bst &source);

    bool bst::empty() const;

    binary_tree_node::value_type bst::max() const;

    bool bst::contains(const binary_tree_node::value_type &target) const;

    size_t bst::size() const;

    size_t bst::height() const;

    size_t bst::get_comparisons();

    size_t bst::get_nodes_visited();

    size_t bst::get_allocations();

    size_t bst::get_deallocations();

    void bst::reset_counters();

    void bst::print_counters(std::ostream &out);

    size_t bst::subtree_height(const binary_tree_node *subtree_ptr) const;

    binary_tree_node *bst::copy_tree(const binary_tree_node *source_ptr);

    void bst::clear_tree(binary_tree_node *&subtree_ptr);
}