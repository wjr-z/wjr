#ifndef WJR_LIST_HPP__
#define WJR_LIST_HPP__

#include <wjr/preprocessor.hpp>

namespace wjr {

struct list_node {
    list_node *next;
    list_node *prev;
};

WJR_INTRINSIC_CONSTEXPR void list_init(list_node* node) {
    node->next = node;
    node->prev = node;
}

WJR_INTRINSIC_CONSTEXPR void list_add_impl(list_node *node, list_node *prev,
                                           list_node *next) {
    node->next = next;
    next->prev = prev;
    prev->next = next;
    node->prev = prev;
}

WJR_INTRINSIC_CONSTEXPR void list_add(list_node *node, list_node *head) {
    list_add_impl(node, head, head->next);
}

WJR_INTRINSIC_CONSTEXPR void list_add_tail(list_node *node, list_node *head) {
    list_add_impl(node, head->prev, head);
}

} // namespace wjr

#endif // WJR_LIST_HPP__