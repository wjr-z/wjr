#ifndef WJR_CONTAINER_LIST_HPP__
#define WJR_CONTAINER_LIST_HPP__

#include <wjr/assert.hpp>
#include <wjr/container/intrusive_detail.hpp>
#include <wjr/type_traits.hpp>

namespace wjr::intrusive {

template <typename T = void, typename Tag = void>
class list_node {
public:
    list_node() noexcept : m_next(this), m_prev(this) {}
    list_node(default_construct_t) noexcept {}
    list_node(const list_node &) = default;
    list_node(list_node &&) = default;
    list_node &operator=(const list_node &) = default;
    list_node &operator=(list_node &&) = default;
    ~list_node() = default;

    constexpr list_node *next() noexcept { return m_next; }
    constexpr const list_node *next() const noexcept { return m_next; }
    constexpr list_node *prev() noexcept { return m_prev; }
    constexpr const list_node *prev() const noexcept { return m_prev; }

    constexpr void set_next(list_node *node) noexcept { m_next = node; }
    constexpr void set_prev(list_node *node) noexcept { m_prev = node; }

    constexpr T *self() noexcept { return static_cast<T *>(this); }
    constexpr const T *self() const noexcept { return static_cast<const T *>(this); }

    constexpr void init_self() noexcept { m_next = m_prev = this; }

    constexpr bool empty() const noexcept { return m_next == this; }

    friend constexpr void insert(list_node *prev, list_node *next, list_node *node) noexcept {
        prev->m_next = node;
        node->m_prev = prev;
        next->m_prev = node;
        node->m_next = next;
    }

    friend constexpr void push_back(list_node *head, list_node *node) noexcept {
        insert(head->m_prev, head, node);
    }

    friend constexpr void push_front(list_node *head, list_node *node) noexcept {
        insert(head, head->m_next, node);
    }

    constexpr void remove() noexcept {
        m_prev->m_next = m_next;
        m_next->m_prev = m_prev;
    }

    friend constexpr void replace(list_node *from, list_node *to) noexcept {
        to->m_next = from->m_next;
        from->m_next->m_prev = to;
        to->m_prev = from->m_prev;
        from->m_prev->m_next = to;
    }

private:
    list_node *m_next;
    list_node *m_prev;
};

static_assert(std::is_standard_layout_v<list_node<>>);

} // namespace wjr::intrusive

#endif // WJR_CONTAINER_LIST_HPP__