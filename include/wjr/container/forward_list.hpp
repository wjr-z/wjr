#ifndef WJR_CONTAINER_FORWARD_LIST_HPP__
#define WJR_CONTAINER_FORWARD_LIST_HPP__

#include <wjr/assert.hpp>
#include <wjr/type_traits.hpp>

namespace wjr::intrusive {

template <typename T = void>
class hlist_node_base;

template <typename T>
class hlist_node_const_iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using pointer = const T *;
    using reference = const T &;

    hlist_node_const_iterator() = default;
    hlist_node_const_iterator(const hlist_node_const_iterator &) = default;
    hlist_node_const_iterator(hlist_node_const_iterator &&) = default;
    hlist_node_const_iterator &operator=(const hlist_node_const_iterator &) = default;
    hlist_node_const_iterator &operator=(hlist_node_const_iterator &&) = default;

    hlist_node_const_iterator(const hlist_node_base<T> *node) noexcept
        : m_node(const_cast<hlist_node_base<T> *>(node)) {}

    std::enable_if_t<!std::is_void_v<T>, pointer> operator->() const noexcept {
        return m_node->self();
    }

    std::enable_if_t<!std::is_void_v<T>, reference> operator*() const noexcept {
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
    hlist_node_base<T> *m_node = nullptr;
};

template <typename T>
class hlist_node_iterator : public hlist_node_const_iterator<T> {
    using Mybase = hlist_node_const_iterator<T>;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using pointer = T *;
    using reference = T &;

    using Mybase::Mybase;

    std::enable_if_t<!std::is_void_v<T>, pointer> operator->() const noexcept {
        return const_cast<pointer>(Mybase::operator->());
    }

    std::enable_if_t<!std::is_void_v<T>, reference> operator*() const noexcept {
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

    hlist_node_iterator &operator--() {
        Mybase::operator--();
        return *this;
    }

    hlist_node_iterator operator--(int) {
        hlist_node_iterator tmp(*this);
        --*this;
        return tmp;
    }

    bool operator==(const hlist_node_iterator &other) const noexcept {
        return this->m_node == other.m_node;
    }

    bool operator!=(const hlist_node_iterator &other) const noexcept {
        return this->m_node != other.m_node;
    }
};

template <typename T>
class hlist_node_base {
    template <typename U>
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

    constexpr hlist_node_base *base() noexcept { return this; }
    constexpr const hlist_node_base *base() const noexcept { return this; }

    constexpr void set_next(hlist_node_base *node) { m_next = node; }

protected:
    hlist_node_base *m_next;
};

template <typename T = void, typename Tag = void>
struct hlist_node : public hlist_node_base<T> {
private:
    using Mybase = hlist_node_base<T>;

public:
    using iterator = hlist_node_iterator<T>;
    using const_iterator = hlist_node_const_iterator<T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

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

    constexpr hlist_node *front() noexcept { return this->m_next; }
    constexpr const hlist_node *front() const noexcept { return this->m_next; }

    constexpr iterator begin() noexcept { return iterator(next()); }
    constexpr const_iterator begin() const noexcept { return const_iterator(next()); }
    constexpr const_iterator cbegin() const noexcept { return const_iterator(next()); }

    constexpr iterator end() noexcept { return iterator(nullptr); }
    constexpr const_iterator end() const noexcept { return const_iterator(nullptr); }
    constexpr const_iterator cend() const noexcept { return const_iterator(nullptr); }
};

static_assert(std::is_standard_layout_v<hlist_node<>>);

#define WJR_HLIST_FOR_EACH(pos, head) for (pos = (head)->next(); (pos); pos = (pos)->next())

#define WJR_HLIST_FOR_EACH_SAFE(pos, n, head)                                                      \
    for (pos = (head)->next(), n = (pos)->next(); (pos); pos = n, n = (n)->next())

#define WJR_HLIST_FOR_EACH_ENTRY(pos, head)                                                        \
    for (pos = (head)->next()->self(); (pos); pos = (pos)->next()->self())

#define WJR_HLIST_FOR_EACH_ENTRY_SAFE(pos, n, head)                                                \
    for (pos = (head)->next()->self(), n = (pos)->next()->self(); (pos);                           \
         pos = n, n = (n)->next()->self())

} // namespace wjr::intrusive

#endif // WJR_CONTAINER_FORWARD_LIST_HPP__