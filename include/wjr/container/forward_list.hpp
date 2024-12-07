#ifndef WJR_CONTAINER_FORWARD_LIST_HPP__
#define WJR_CONTAINER_FORWARD_LIST_HPP__

#include <wjr/assert.hpp>
#include <wjr/container/intrusive_detail.hpp>
#include <wjr/type_traits.hpp>

namespace wjr::intrusive {

template <typename T = void, typename Tag = void>
class forward_list_node {
public:
    forward_list_node() noexcept : m_next(nullptr) {}
    forward_list_node(default_construct_t) noexcept {}
    forward_list_node(const forward_list_node &) = default;
    forward_list_node(forward_list_node &&) = default;
    forward_list_node &operator=(const forward_list_node &) = default;
    forward_list_node &operator=(forward_list_node &&) = default;
    ~forward_list_node() = default;

    constexpr forward_list_node *next() noexcept { return m_next; }
    constexpr const forward_list_node *next() const noexcept { return m_next; }

    constexpr void set_next(forward_list_node *node) noexcept { m_next = node; }

    constexpr T *self() noexcept { return static_cast<T *>(this); }
    constexpr const T *self() const noexcept { return static_cast<const T *>(this); }

    constexpr void init_self() noexcept { m_next = nullptr; }

    constexpr bool empty() const noexcept { return m_next == nullptr; }

    constexpr void insert_after(forward_list_node *head, forward_list_node *tail) noexcept {
        tail->m_next = m_next;
        m_next = head;
    }

    constexpr void insert_after(forward_list_node *node) noexcept { insert_after(node, node); }

    constexpr void push_back(forward_list_node *node) noexcept {
        WJR_ASSERT(m_next == nullptr);
        m_next = node;
    }

private:
    forward_list_node *m_next;
};

static_assert(std::is_standard_layout_v<forward_list_node<>>);

} // namespace wjr::intrusive

#endif // WJR_CONTAINER_FORWARD_LIST_HPP__