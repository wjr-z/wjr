#ifndef WJR_CONTAINER_LIST_HPP__
#define WJR_CONTAINER_LIST_HPP__

#include <wjr/assert.hpp>
#include <wjr/container/intrusive_detail.hpp>
#include <wjr/type_traits.hpp>

namespace wjr::intrusive {

struct list_node {
    list_node *prev;
    list_node *next;
};

static_assert(std::is_standard_layout_v<list_node>);

constexpr void init(list_node *node) noexcept {
    node->prev = node;
    node->next = node;
}

WJR_INTRINSIC_CONSTEXPR bool __insert_check(list_node *prev, list_node *next,
                                            list_node *node) noexcept {
    if (WJR_LIKELY(next->prev == prev && prev->next == next && node != prev && node != next)) {
        return true;
    }

    return false;
}

constexpr void insert(list_node *prev, list_node *next, list_node *node) noexcept {
    WJR_ASSERT_L2(__insert_check(prev, next, node));

    prev->next = node;
    node->prev = prev;
    next->prev = node;
    node->next = next;
}

constexpr void push_back(list_node *head, list_node *node) noexcept {
    insert(head->prev, head, node);
}

constexpr void push_front(list_node *head, list_node *node) noexcept {
    insert(head, head->next, node);
}

constexpr bool empty(const list_node *node) noexcept { return node->next == node; }

constexpr list_node *next(list_node *node) noexcept { return node->next; }
constexpr const list_node *next(const list_node *node) noexcept { return node->next; }
constexpr list_node *prev(list_node *node) noexcept { return node->prev; }
constexpr const list_node *prev(const list_node *node) noexcept { return node->prev; }

constexpr void remove(list_node *node) noexcept {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

constexpr void replace(list_node *from, list_node *to) noexcept {
    to->next = from->next;
    from->next->prev = to;
    to->prev = from->prev;
    from->prev->next = to;
}

} // namespace wjr::intrusive

#endif // WJR_CONTAINER_LIST_HPP__