#ifndef WJR_CONTAINER_LIST_HPP__
#define WJR_CONTAINER_LIST_HPP__

#include <wjr/assert.hpp>
#include <wjr/type_traits.hpp>

namespace wjr::intrusive {

template <typename T = void>
class list_node_base;

template <typename T>
class list_node_const_iterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using pointer = const T *;
    using reference = const T &;

    list_node_const_iterator() = default;
    list_node_const_iterator(const list_node_const_iterator &) = default;
    list_node_const_iterator(list_node_const_iterator &&) = default;
    list_node_const_iterator &operator=(const list_node_const_iterator &) = default;
    list_node_const_iterator &operator=(list_node_const_iterator &&) = default;

    list_node_const_iterator(const list_node_base<T> *node) noexcept
        : m_node(const_cast<list_node_base<T> *>(node)) {}

    std::enable_if_t<!std::is_void_v<T>, pointer> operator->() const noexcept {
        return m_node->self();
    }

    std::enable_if_t<!std::is_void_v<T>, reference> operator*() const noexcept {
        return *m_node->self();
    }

    list_node_const_iterator &operator++() {
        m_node = m_node->m_next;
        return *this;
    }

    list_node_const_iterator operator++(int) {
        list_node_const_iterator tmp(*this);
        ++*this;
        return tmp;
    }

    list_node_const_iterator &operator--() {
        m_node = m_node->m_prev;
        return *this;
    }

    list_node_const_iterator operator--(int) {
        list_node_const_iterator tmp(*this);
        --*this;
        return tmp;
    }

    bool operator==(const list_node_const_iterator &other) const noexcept {
        return m_node == other.m_node;
    }

    bool operator!=(const list_node_const_iterator &other) const noexcept {
        return m_node != other.m_node;
    }

protected:
    list_node_base<T> *m_node = nullptr;
};

template <typename T>
class list_node_iterator : public list_node_const_iterator<T> {
    using Mybase = list_node_const_iterator<T>;

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

    list_node_iterator &operator++() {
        Mybase::operator++();
        return *this;
    }

    list_node_iterator operator++(int) {
        list_node_iterator tmp(*this);
        ++*this;
        return tmp;
    }

    list_node_iterator &operator--() {
        Mybase::operator--();
        return *this;
    }

    list_node_iterator operator--(int) {
        list_node_iterator tmp(*this);
        --*this;
        return tmp;
    }

    bool operator==(const list_node_iterator &other) const noexcept {
        return this->m_node == other.m_node;
    }

    bool operator!=(const list_node_iterator &other) const noexcept {
        return this->m_node != other.m_node;
    }
};

template <typename T>
class list_node_base {
    template <typename U>
    friend class list_node_const_iterator;

public:
    constexpr list_node_base() noexcept : m_next(this), m_prev(this) {}
    list_node_base(default_construct_t) noexcept {}
    list_node_base(const list_node_base &) = default;
    list_node_base(list_node_base &&) = default;
    list_node_base &operator=(const list_node_base &) = default;
    list_node_base &operator=(list_node_base &&) = default;
    ~list_node_base() = default;

    constexpr void init_self() noexcept { m_next = m_prev = this; }

    constexpr bool empty() const noexcept { return m_next == this; }

    friend constexpr void insert(list_node_base *pre, list_node_base *nex,
                                 list_node_base *node) noexcept {
        pre->m_next = node;
        node->m_prev = pre;
        nex->m_prev = node;
        node->m_next = nex;
    }

    constexpr void push_back(list_node_base *node) noexcept { insert(m_prev, this, node); }
    constexpr void push_front(list_node_base *node) noexcept { insert(this, m_next, node); }

    constexpr void remove() noexcept {
        m_prev->m_next = m_next;
        m_next->m_prev = m_prev;
    }

    friend constexpr void replace(list_node_base *from, list_node_base *to) noexcept {
        to->m_next = from->m_next;
        from->m_next->m_prev = to;
        to->m_prev = from->m_prev;
        from->m_prev->m_next = to;
    }

    constexpr T *self() noexcept { return static_cast<T *>(this); }
    constexpr const T *self() const noexcept { return static_cast<const T *>(this); }

    constexpr list_node_base *base() noexcept { return this; }
    constexpr const list_node_base *base() const noexcept { return this; }

    constexpr void set_next(list_node_base *node) { m_next = node; }
    constexpr void set_prev(list_node_base *node) { m_prev = node; }

protected:
    list_node_base *m_next;
    list_node_base *m_prev;
};

template <typename T = void, typename Tag = void>
struct list_node : public list_node_base<T> {
private:
    using Mybase = list_node_base<T>;

public:
    using iterator = list_node_iterator<T>;
    using const_iterator = list_node_const_iterator<T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using Mybase::Mybase;

    constexpr list_node *pop_front() noexcept {
        auto *n = next();
        n->remove();
        return n;
    }

    constexpr list_node *pop_back() noexcept {
        auto *p = prev();
        p->remove();
        return p;
    }

    constexpr list_node *next() { return static_cast<list_node *>(this->m_next); }
    constexpr const list_node *next() const { return static_cast<const list_node *>(this->m_next); }

    constexpr list_node *prev() { return static_cast<list_node *>(this->m_prev); }
    constexpr const list_node *prev() const { return static_cast<const list_node *>(this->m_prev); }

    constexpr list_node *front() noexcept { return this->m_next; }
    constexpr const list_node *front() const noexcept { return this->m_next; }

    constexpr list_node *back() noexcept { return this->m_prev; }
    constexpr const list_node *back() const noexcept { return this->m_prev; }

    constexpr iterator begin() noexcept { return iterator(next()); }
    constexpr const_iterator begin() const noexcept { return const_iterator(next()); }
    constexpr const_iterator cbegin() const noexcept { return const_iterator(next()); }

    constexpr iterator end() noexcept { return iterator(this); }
    constexpr const_iterator end() const noexcept { return const_iterator(this); }
    constexpr const_iterator cend() const noexcept { return const_iterator(this); }

    constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    constexpr const_reverse_iterator crend() const noexcept { return rend(); }
};

static_assert(std::is_standard_layout_v<list_node_base<>>);
static_assert(std::is_standard_layout_v<list_node<>>);

#define WJR_LIST_FOR_EACH(pos, head)                                                               \
    for (pos = (head)->next(); (pos) != (head); pos = (pos)->next())

#define WJR_LIST_FOR_EACH_SAFE(pos, n, head)                                                       \
    for (pos = (head)->next(), n = (pos)->next(); (pos) != (head); pos = n, n = (n)->next())

#define WJR_LIST_FOR_EACH_ENTRY(pos, head)                                                         \
    for (pos = (head)->next()->self(); (pos) != (head); pos = (pos)->next()->self())

#define WJR_LIST_FOR_EACH_ENTRY_SAFE(pos, n, head)                                                 \
    for (pos = (head)->next()->self(), n = (pos)->next()->self(); (pos) != (head);                 \
         pos = n, n = (n)->next()->self())

} // namespace wjr::intrusive

#endif // WJR_CONTAINER_LIST_HPP__