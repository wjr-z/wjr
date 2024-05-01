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

WJR_INTRINSIC_CONSTEXPR void push_back(list_node *head, list_node *node) {
    insert(head, head->next, node);
}

WJR_INTRINSIC_CONSTEXPR void push_front(list_node *head, list_node *node) {
    insert(head->prev, head, node);
}

WJR_INTRINSIC_CONSTEXPR void remove(list_node *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->prev = node;
    node->next = node;
}

WJR_INTRINSIC_CONSTEXPR bool empty(const list_node *node) { return node->next == node; }

WJR_INTRINSIC_CONSTEXPR list_node *next(list_node *node) { return node->next; }
WJR_INTRINSIC_CONSTEXPR const list_node *next(const list_node *node) {
    return node->next;
}

WJR_INTRINSIC_CONSTEXPR list_node *prev(list_node *node) { return node->prev; }
WJR_INTRINSIC_CONSTEXPR const list_node *prev(const list_node *node) {
    return node->prev;
}

WJR_INTRINSIC_CONSTEXPR list_node *begin(list_node *node) { return next(node); }
WJR_INTRINSIC_CONSTEXPR const list_node *begin(const list_node *node) {
    return next(node);
}

WJR_INTRINSIC_CONSTEXPR list_node *end(list_node *node) { return node; }
WJR_INTRINSIC_CONSTEXPR const list_node *end(const list_node *node) { return node; }

WJR_INTRINSIC_CONSTEXPR void replace(list_node *from, list_node *to) {
    WJR_ASSERT(!empty(from));

    to->prev = from->prev;
    to->next = from->next;
    from->prev->next = to;
    from->next->prev = to;
}

} // namespace wjr::intrusive

#endif // WJR_CONTAINER_INTRUSIVE_LIST_HPP__