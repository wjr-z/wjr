#ifndef WJR_CONTAINER_RBTREE_HPP__
#define WJR_CONTAINER_RBTREE_HPP__

#include <wjr/assert.hpp>
#include <wjr/type_traits.hpp>

namespace wjr::intrusive {

#pragma pack(push, 1)

struct rbtree_node {
    enum COLOR : unsigned int {
        RED,
        BLACK,
    };

    COLOR color = RED;
    rbtree_node *parent;
    rbtree_node *left;
    rbtree_node *right;
};

#pragma pack(pop)

} // namespace wjr::intrusive

#endif // WJR_CONTAINER_RBTREE_HPP__