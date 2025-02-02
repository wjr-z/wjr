#ifndef WJR_CONTAINER_LIST_HPP__
#define WJR_CONTAINER_LIST_HPP__

#include <wjr/assert.hpp>
#include <wjr/container/intrusive_detail.hpp>
#include <wjr/type_traits.hpp>

namespace wjr::intrusive {

template <typename T = void, typename Tag = void>
struct list_node {
    list_node() noexcept : next(this), prev(this) {}
    list_node(default_construct_t) noexcept {}
    list_node(const list_node &) = default;
    list_node(list_node &&) = default;
    list_node &operator=(const list_node &) = default;
    list_node &operator=(list_node &&) = default;
    ~list_node() = default;

    constexpr T *self() noexcept { return static_cast<T *>(this); }
    constexpr const T *self() const noexcept { return static_cast<const T *>(this); }
    constexpr void init_self() noexcept { next = prev = this; }

    constexpr bool empty() const noexcept { return next == this; }

    friend constexpr void insert(list_node *pre, list_node *nex, list_node *node) noexcept {
        pre->next = node;
        node->prev = pre;
        nex->prev = node;
        node->next = nex;
    }

    constexpr void push_back(list_node *node) noexcept { insert(prev, this, node); }
    constexpr void push_front(list_node *node) noexcept { insert(this, next, node); }

    constexpr void remove() noexcept {
        prev->next = next;
        next->prev = prev;
    }

    friend constexpr void replace(list_node *from, list_node *to) noexcept {
        to->next = from->next;
        from->next->prev = to;
        to->prev = from->prev;
        from->prev->next = to;
    }

    template <typename U, WJR_REQUIRES(std::is_same_v<U, Tag>)>
    constexpr list_node *get_list_node() noexcept {
        return this;
    }

    template <typename U, WJR_REQUIRES(std::is_same_v<U, Tag>)>
    constexpr const list_node *get_list_node() const noexcept {
        return this;
    }

    list_node *next;
    list_node *prev;
};

static_assert(std::is_standard_layout_v<list_node<>>);

#define WJR_LIST_FOR_EACH(pos, head) for (pos = (head)->next; (pos) != (head); pos = (pos)->next)

#define WJR_LIST_FOR_EACH_SAFE(pos, n, head)                                                       \
    for (pos = (head)->next, n = (pos)->next; (pos) != (head); pos = n, n = (n)->next)

#define WJR_LIST_FOR_EACH_ENTRY(pos, head)                                                         \
    for (pos = (head)->next->self(); (pos) != (head); pos = (pos)->next->self())

#define WJR_LIST_FOR_EACH_ENTRY_SAFE(pos, n, head)                                                 \
    for (pos = (head)->next->self(), n = (pos)->next->self(); (pos) != (head);                     \
         pos = n, n = (n)->next->self())

} // namespace wjr::intrusive

#endif // WJR_CONTAINER_LIST_HPP__