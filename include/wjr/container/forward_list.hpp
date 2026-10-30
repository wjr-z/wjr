#ifndef WJR_CONTAINER_FORWARD_LIST_HPP__
#define WJR_CONTAINER_FORWARD_LIST_HPP__

#include <cstddef>
#include <iterator>

#include <wjr/assert.hpp>
#include <wjr/type_traits.hpp>

namespace wjr::intrusive {

template <typename T = void, typename Tag = void>
class hlist_node_base;

template <typename T, typename Tag = void>
class hlist_node_const_iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T *;
    using reference = std::add_lvalue_reference_t<const T>;

    hlist_node_const_iterator() = default;
    hlist_node_const_iterator(const hlist_node_const_iterator &) = default;
    hlist_node_const_iterator(hlist_node_const_iterator &&) = default;
    hlist_node_const_iterator &operator=(const hlist_node_const_iterator &) = default;
    hlist_node_const_iterator &operator=(hlist_node_const_iterator &&) = default;

    hlist_node_const_iterator(const hlist_node_base<T, Tag> *node) noexcept
        : m_node(const_cast<hlist_node_base<T, Tag> *>(node)) {}

    pointer operator->() const noexcept
    requires(!std::is_void_v<T>)
    {
        return m_node->self();
    }

    reference operator*() const noexcept
    requires(!std::is_void_v<T>)
    {
        return *m_node->self();
    }

    hlist_node_const_iterator &operator++() {
        m_node = m_node->m_next;
        return *this;
    }

    hlist_node_const_iterator operator++(int) {
        hlist_node_const_iterator tmp(*this);
        ++*this;
        return tmp;
    }

    bool operator==(const hlist_node_const_iterator &other) const noexcept {
        return m_node == other.m_node;
    }

    bool operator!=(const hlist_node_const_iterator &other) const noexcept {
        return m_node != other.m_node;
    }

protected:
    hlist_node_base<T, Tag> *m_node = nullptr;
};

template <typename T, typename Tag = void>
class hlist_node_iterator : public hlist_node_const_iterator<T, Tag> {
    using Mybase = hlist_node_const_iterator<T, Tag>;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = std::add_lvalue_reference_t<T>;

    hlist_node_iterator() = default;

    /// Construct mutable access only from a mutable node.
    hlist_node_iterator(hlist_node_base<T, Tag> *node) noexcept : Mybase(node) {}

    using Mybase::operator==;
    using Mybase::operator!=;

    pointer operator->() const noexcept
    requires(!std::is_void_v<T>)
    {
        return const_cast<pointer>(Mybase::operator->());
    }

    reference operator*() const noexcept
    requires(!std::is_void_v<T>)
    {
        return *operator->();
    }

    hlist_node_iterator &operator++() {
        Mybase::operator++();
        return *this;
    }

    hlist_node_iterator operator++(int) {
        hlist_node_iterator tmp(*this);
        ++*this;
        return tmp;
    }

    bool operator==(const hlist_node_iterator &other) const noexcept {
        return this->m_node == other.m_node;
    }

    bool operator!=(const hlist_node_iterator &other) const noexcept {
        return this->m_node != other.m_node;
    }
};

template <typename T, typename Tag>
class hlist_node_base {
    template <typename U, typename V>
    friend class hlist_node_const_iterator;

public:
    constexpr hlist_node_base() noexcept : m_next(nullptr) {}
    hlist_node_base(default_construct_t) noexcept {}
    hlist_node_base(const hlist_node_base &) = delete;
    hlist_node_base(hlist_node_base &&) = delete;
    hlist_node_base &operator=(const hlist_node_base &) = delete;
    hlist_node_base &operator=(hlist_node_base &&) = delete;
    ~hlist_node_base() = default;

    constexpr void init_self() noexcept { m_next = nullptr; }

    constexpr bool empty() const noexcept { return m_next == nullptr; }

    constexpr void push_front(hlist_node_base *head, hlist_node_base *tail) noexcept {
        tail->m_next = m_next;
        m_next = head;
    }

    constexpr void push_front(hlist_node_base *node) noexcept { push_front(node, node); }

    friend void remove(hlist_node_base *prev, hlist_node_base *next) {
        prev->m_next = next->m_next;
    }

    friend void replace(hlist_node_base *from, hlist_node_base *to) { to->m_next = from->m_next; }

    constexpr T *self() noexcept { return static_cast<T *>(this); }
    constexpr const T *self() const noexcept { return static_cast<const T *>(this); }

    constexpr void set_next(hlist_node_base *node) { m_next = node; }

protected:
    hlist_node_base *m_next;
};

template <typename T = void, typename Tag = void>
struct hlist_node : public hlist_node_base<T, Tag> {
private:
    using Mybase = hlist_node_base<T, Tag>;

public:
    using iterator = hlist_node_iterator<T, Tag>;
    using const_iterator = hlist_node_const_iterator<T, Tag>;

    using Mybase::Mybase;

    constexpr hlist_node *pop_front() noexcept {
        auto *n = next();
        remove(this, n);
        return n;
    }

    constexpr hlist_node *next() { return static_cast<hlist_node *>(this->m_next); }
    constexpr const hlist_node *next() const {
        return static_cast<const hlist_node *>(this->m_next);
    }

    constexpr hlist_node *front() noexcept { return next(); }
    constexpr const hlist_node *front() const noexcept { return next(); }

    constexpr iterator begin() noexcept { return iterator(next()); }
    constexpr const_iterator begin() const noexcept { return const_iterator(next()); }
    constexpr const_iterator cbegin() const noexcept { return const_iterator(next()); }

    constexpr iterator end() noexcept { return iterator(nullptr); }
    constexpr const_iterator end() const noexcept { return const_iterator(nullptr); }
    constexpr const_iterator cend() const noexcept { return const_iterator(nullptr); }

    template <typename U = T, typename V = Tag>
    requires(std::is_same_v<U, T> && std::is_same_v<V, Tag>)
    constexpr hlist_node<U, V> *get_node() noexcept {
        return this;
    }

    template <typename U = T, typename V = Tag>
    requires(std::is_same_v<U, T> && std::is_same_v<V, Tag>)
    constexpr const hlist_node<U, V> *get_node() const noexcept {
        return this;
    }
};

static_assert(std::is_standard_layout_v<hlist_node<>>);

#define WJR_HLIST_FOR_EACH(pos, head) for (pos = (head)->next(); (pos); pos = (pos)->next())

#define WJR_HLIST_FOR_EACH_SAFE(pos, n, head)                                                      \
    for (pos = (head)->next(); (pos) && ((n) = (pos)->next(), true); (pos) = (n))

#define WJR_HLIST_FOR_EACH_ENTRY(pos, head)                                                        \
    for ((pos) = static_cast<std::remove_reference_t<decltype(pos)>>((head)->next()); (pos);       \
         (pos) = static_cast<std::remove_reference_t<decltype(pos)>>((pos)->next()))

#define WJR_HLIST_FOR_EACH_ENTRY_SAFE(pos, n, head)                                                \
    for ((pos) = static_cast<std::remove_reference_t<decltype(pos)>>((head)->next());              \
         (pos) && ((n) = static_cast<std::remove_reference_t<decltype(n)>>((pos)->next()), true);  \
         (pos) = (n))

} // namespace wjr::intrusive

#endif // WJR_CONTAINER_FORWARD_LIST_HPP__