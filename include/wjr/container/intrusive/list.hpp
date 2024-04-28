#ifndef WJR_CONTAINER_INTRUSIVE_LIST_HPP__
#define WJR_CONTAINER_INTRUSIVE_LIST_HPP__

#include <wjr/preprocessor.hpp>

namespace wjr::intrusive {

struct forward_list_node {
    forward_list_node *next;
};

struct list_node {
    list_node *prev;
    list_node *next;
};

WJR_INTRINSIC_CONSTEXPR void init(list_node *node) {
    node->prev = node;
    node->next = node;
}

WJR_INTRINSIC_CONSTEXPR void insert(list_node *prev, list_node *next, list_node *node) {
    prev->next = node;
    node->prev = prev;
    next->prev = node;
    node->next = next;
}

WJR_INTRINSIC_CONSTEXPR void insert_head(list_node *head, list_node *node) {
    insert(head, head->next, node);
}

WJR_INTRINSIC_CONSTEXPR void insert_tail(list_node *head, list_node *node) {
    insert(head->prev, head, node);
}

WJR_INTRINSIC_CONSTEXPR void remove(list_node *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->prev = node;
    node->next = node;
}

WJR_INTRINSIC_CONSTEXPR bool empty(const list_node *node) { return node->next == node; }

} // namespace wjr::intrusive

#endif // WJR_CONTAINER_INTRUSIVE_LIST_HPP__