#ifndef WJR_CONTAINER_INTRUSIVE_SLIST_HPP__
#define WJR_CONTAINER_INTRUSIVE_SLIST_HPP__

#include <wjr/container/intrusive/details.hpp>

namespace wjr {

template <typename Tag = void>
struct slist_node;

template <typename T>
constexpr void init(slist_node<T> *node) noexcept;

template <typename T>
constexpr void push_front(slist_node<T> *head, slist_node<T> *node) noexcept;

template <typename T>
constexpr void pop_front_uninit(slist_node<T> *head) noexcept;

template <typename T>
constexpr bool empty(const slist_node<T> *node) noexcept;

template <typename T>
constexpr slist_node<T> *next(slist_node<T> *node) noexcept;

template <typename T>
constexpr const slist_node<T> *next(const slist_node<T> *node) noexcept;

template <typename T>
class slist_node_const_iterator {
    using node_type = slist_node<T>;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = node_type;
    using reference = const node_type &;
    using pointer = const node_type *;
    using difference_type = std::ptrdiff_t;

    constexpr slist_node_const_iterator() = default;
    constexpr slist_node_const_iterator(const slist_node_const_iterator &) = default;
    constexpr slist_node_const_iterator(slist_node_const_iterator &&) = default;
    constexpr slist_node_const_iterator &
    operator=(const slist_node_const_iterator &) = default;
    constexpr slist_node_const_iterator &
    operator=(slist_node_const_iterator &&) = default;
    ~slist_node_const_iterator() = default;

    constexpr slist_node_const_iterator(const node_type *node) noexcept
        : m_node(const_cast<node_type *>(node)) {}

    constexpr reference operator*() const noexcept { return *m_node; }
    constexpr pointer operator->() const noexcept { return m_node; }

    constexpr slist_node_const_iterator &operator++() noexcept {
        m_node = next(m_node);
        return *this;
    }

    constexpr slist_node_const_iterator operator++(int) noexcept {
        slist_node_const_iterator tmp(*this);
        ++(*this);
        return tmp;
    }

    constexpr bool operator==(const slist_node_const_iterator &other) const noexcept {
        return m_node == other.m_node;
    }

    constexpr bool operator!=(const slist_node_const_iterator &other) const noexcept {
        return !(*this == other);
    }

    constexpr operator const node_type *() const noexcept { return m_node; }

private:
    node_type *m_node{};
};

template <typename T>
class slist_node_iterator : public slist_node_const_iterator<T> {
    using Mybase = slist_node_const_iterator<T>;
    using node_type = slist_node<T>;

public:
    using iterator_category = typename Mybase::iterator_category;
    using value_type = typename Mybase::value_type;
    using reference = node_type &;
    using pointer = node_type *;
    using difference_type = typename Mybase::difference_type;

    using Mybase::Mybase;

    constexpr slist_node_iterator() = default;
    constexpr slist_node_iterator(const slist_node_iterator &) = default;
    constexpr slist_node_iterator(slist_node_iterator &&) = default;
    constexpr slist_node_iterator &
    operator=(const slist_node_iterator &) = default;
    constexpr slist_node_iterator &operator=(slist_node_iterator &&) = default;
    ~slist_node_iterator() = default;

    constexpr reference operator*() const noexcept {
        return const_cast<reference>(Mybase::operator*());
    }

    constexpr pointer operator->() const noexcept {
        return const_cast<pointer>(Mybase::operator->());
    }

    constexpr slist_node_iterator &operator++() noexcept {
        Mybase::operator++();
        return *this;
    }

    constexpr slist_node_iterator operator++(int) noexcept {
        slist_node_iterator tmp(*this);
        ++(*this);
        return tmp;
    }

    constexpr operator node_type *() const noexcept {
        return const_cast<node_type *>(static_cast<const node_type *>(*this));
    }
};

template <typename Tag>
struct slist_node {
    using iterator = slist_node_iterator<Tag>;
    using const_iterator = slist_node_const_iterator<Tag>;

    constexpr slist_node() = default;
    slist_node(const slist_node &) = delete;
    slist_node(slist_node &&) = delete;
    slist_node &operator=(const slist_node &) = delete;
    slist_node &operator=(slist_node &&) = delete;
    ~slist_node() = default;

    constexpr iterator begin() noexcept { return iterator(next(this)); }
    constexpr const_iterator begin() const noexcept { return const_iterator(next(this)); }
    constexpr const_iterator cbegin() const noexcept {
        return const_iterator(next(this));
    }

    constexpr iterator end() noexcept { return iterator(nullptr); }
    constexpr const_iterator end() const noexcept { return const_iterator(nullptr); }
    constexpr const_iterator cend() const noexcept { return const_iterator(nullptr); }

    constexpr bool empty() const noexcept { return wjr::empty(this); }

    template <typename T>
    friend constexpr void init(slist_node<T> *node) noexcept {
        node->m_next = nullptr;
    }

    template <typename T>
    friend constexpr void push_front(slist_node<T> *head, slist_node<T> *node) noexcept {
        node->m_next = head->m_next;
        head->m_next = node;
    }

    template <typename T>
    friend constexpr void pop_front_uninit(slist_node<T> *head) noexcept {
        head->m_next = head->m_next->m_next;
    }

    template <typename T>
    friend constexpr bool empty(const slist_node<T> *node) noexcept {
        return node->m_next == nullptr;
    }

    template <typename T>
    friend constexpr slist_node<T> *next(slist_node<T> *node) noexcept {
        return node->m_next;
    }

    template <typename T>
    friend constexpr const slist_node<T> *next(const slist_node<T> *node) noexcept {
        return node->m_next;
    }

    slist_node *m_next;
};

} // namespace wjr

#endif // WJR_CONTAINER_INTRUSIVE_SLIST_HPP__