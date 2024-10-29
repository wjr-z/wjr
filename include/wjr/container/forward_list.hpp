#ifndef WJR_CONTAINER_FORWARD_LIST_HPP__
#define WJR_CONTAINER_FORWARD_LIST_HPP__

#include <wjr/assert.hpp>
#include <wjr/container/intrusive_detail.hpp>
#include <wjr/type_traits.hpp>

namespace wjr::intrusive {

struct forward_list_node {
    forward_list_node *next;
};

static_assert(std::is_standard_layout_v<forward_list_node>);

constexpr void init(forward_list_node *node) noexcept { node->next = nullptr; }

constexpr void insert_after(forward_list_node *prev, forward_list_node *head,
                            forward_list_node *tail) noexcept {
    tail->next = prev->next;
    prev->next = head;
}

constexpr void insert_after(forward_list_node *prev, forward_list_node *node) noexcept {
    node->next = prev->next;
    prev->next = node;
}

constexpr void push_front(forward_list_node *prev, forward_list_node *head,
                          forward_list_node *tail) noexcept {
    insert_after(prev, head, tail);
}

constexpr void push_front(forward_list_node *prev, forward_list_node *node) noexcept {
    insert_after(prev, node);
}

constexpr void push_back(forward_list_node *tail, forward_list_node *node) noexcept {
    tail->next = node;
}

constexpr bool empty(const forward_list_node *node) noexcept { return node->next == nullptr; }

constexpr forward_list_node *next(forward_list_node *node) noexcept { return node->next; }
constexpr const forward_list_node *next(const forward_list_node *node) noexcept {
    return node->next;
}

constexpr void pop_front(forward_list_node *head, forward_list_node *node) noexcept {
    WJR_ASSERT_ASSUME(head->next == node);
    head->next = node->next;
}

} // namespace wjr::intrusive

#endif // WJR_CONTAINER_FORWARD_LIST_HPP__