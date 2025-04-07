#ifndef WJR_CONTAINER_FORWARD_LIST_HPP__
#define WJR_CONTAINER_FORWARD_LIST_HPP__

#include <wjr/assert.hpp>
#include <wjr/type_traits.hpp>

namespace wjr::intrusive {

template <typename T = void, typename Tag = void>
struct hlist_node {
    hlist_node() noexcept : next(nullptr) {}
    hlist_node(default_construct_t) noexcept {}
    hlist_node(const hlist_node &) = default;
    hlist_node(hlist_node &&) = default;
    hlist_node &operator=(const hlist_node &) = default;
    hlist_node &operator=(hlist_node &&) = default;
    ~hlist_node() = default;

    constexpr T *self() noexcept { return static_cast<T *>(this); }
    constexpr const T *self() const noexcept { return static_cast<const T *>(this); }
    constexpr void init_self() noexcept { next = nullptr; }

    constexpr bool empty() const noexcept { return next == nullptr; }

    constexpr void push_front(hlist_node *head, hlist_node *tail) noexcept {
        tail->next = next;
        next = head;
    }

    constexpr void push_front(hlist_node *node) noexcept { push_front(node, node); }

    constexpr void push_back(hlist_node *node) noexcept {
        WJR_ASSERT(next == nullptr);
        next = node;
    }

    constexpr hlist_node *front() noexcept { return next; }
    constexpr const hlist_node *front() const noexcept { return next; }

    hlist_node *next;
};

static_assert(std::is_standard_layout_v<hlist_node<>>);

} // namespace wjr::intrusive

#endif // WJR_CONTAINER_FORWARD_LIST_HPP__