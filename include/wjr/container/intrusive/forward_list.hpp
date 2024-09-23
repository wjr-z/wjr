#ifndef WJR_CONTAINER_INTRUSIVE_FORWARD_LIST_HPP__
#define WJR_CONTAINER_INTRUSIVE_FORWARD_LIST_HPP__

#include <wjr/assert.hpp>
#include <wjr/type_traits.hpp>

namespace wjr::intrusive {

struct forward_list_node_base {
    forward_list_node_base *m_next;
};

constexpr void init(forward_list_node_base *node) noexcept { node->m_next = nullptr; }

constexpr void insert_after(forward_list_node_base *prev,
                            forward_list_node_base *node) noexcept {
    node->m_next = prev->m_next;
    prev->m_next = node;
}

constexpr void push_front(forward_list_node_base *head,
                          forward_list_node_base *node) noexcept {
    insert_after(head, node);
}

constexpr void push_back(forward_list_node_base *tail,
                         forward_list_node_base *node) noexcept {
    WJR_ASSERT_ASSUME(tail->m_next == nullptr);
    tail->m_next = node;
}

constexpr bool empty(const forward_list_node_base *node) noexcept {
    return node->m_next == nullptr;
}

constexpr forward_list_node_base *next(forward_list_node_base *node) noexcept {
    return node->m_next;
}

constexpr const forward_list_node_base *
next(const forward_list_node_base *node) noexcept {
    return node->m_next;
}

constexpr void pop_front_uninit(forward_list_node_base *head,
                                forward_list_node_base *node) noexcept {
    WJR_ASSERT_ASSUME(head->m_next == node);
    head->m_next = node->m_next;
}

constexpr void pop_front_init(forward_list_node_base *head,
                              forward_list_node_base *node) noexcept {
    pop_front_uninit(head, node);
    init(node);
}

template <typename T>
struct forward_list_node;

template <typename T>
class forward_list_node_const_iterator {
    using node_type = forward_list_node<T>;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = node_type;
    using reference = const node_type &;
    using pointer = const node_type *;
    using difference_type = std::ptrdiff_t;

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(forward_list_node_const_iterator);

    constexpr forward_list_node_const_iterator(pointer node) noexcept
        : m_node(const_cast<node_type *>(node)) {}

    constexpr reference operator*() const noexcept { return *m_node; }
    constexpr pointer operator->() const noexcept { return m_node; }

    constexpr forward_list_node_const_iterator &operator++() noexcept {
        m_node = m_node->next();
        return *this;
    }

    constexpr forward_list_node_const_iterator operator++(int) noexcept {
        forward_list_node_const_iterator tmp(*this);
        ++(*this);
        return tmp;
    }

    constexpr bool
    operator==(const forward_list_node_const_iterator &other) const noexcept {
        return m_node == other.m_node;
    }

    constexpr bool
    operator!=(const forward_list_node_const_iterator &other) const noexcept {
        return !(*this == other);
    }

private:
    node_type *m_node{};
};

template <typename T>
class forward_list_node_iterator : public forward_list_node_const_iterator<T> {
    using Mybase = forward_list_node_const_iterator<T>;
    using node_type = forward_list_node<T>;

public:
    using iterator_category = typename Mybase::iterator_category;
    using value_type = typename Mybase::value_type;
    using reference = node_type &;
    using pointer = node_type *;
    using difference_type = typename Mybase::difference_type;

    using Mybase::Mybase;

    constexpr reference operator*() const noexcept {
        return const_cast<reference>(Mybase::operator*());
    }

    constexpr pointer operator->() const noexcept {
        return const_cast<pointer>(Mybase::operator->());
    }

    constexpr forward_list_node_iterator &operator++() noexcept {
        Mybase::operator++();
        return *this;
    }

    constexpr forward_list_node_iterator operator++(int) noexcept {
        forward_list_node_iterator tmp(*this);
        ++(*this);
        return tmp;
    }
};

template <>
struct forward_list_node<void> : forward_list_node_base {
    using iterator = forward_list_node_iterator<void>;
    using const_iterator = forward_list_node_const_iterator<void>;

    forward_list_node() = default;
    forward_list_node(const forward_list_node &) = delete;
    forward_list_node(forward_list_node &&) = delete;
    forward_list_node &operator=(const forward_list_node &) = delete;
    forward_list_node &operator=(forward_list_node &&) = delete;
    ~forward_list_node() = default;

    constexpr iterator begin() noexcept { return iterator(next()); }
    constexpr const_iterator begin() const noexcept { return const_iterator(next()); }
    constexpr const_iterator cbegin() const noexcept { return const_iterator(next()); }

    constexpr iterator end() noexcept { return iterator(this); }
    constexpr const_iterator end() const noexcept { return const_iterator(this); }
    constexpr const_iterator cend() const noexcept { return const_iterator(this); }

    constexpr forward_list_node_base *base() noexcept { return this; }
    constexpr const forward_list_node_base *base() const noexcept { return this; }

    constexpr forward_list_node *next() noexcept {
        return static_cast<forward_list_node *>(wjr::intrusive::next(this));
    }

    constexpr const forward_list_node *next() const noexcept {
        return static_cast<const forward_list_node *>(wjr::intrusive::next(this));
    }

    constexpr bool empty() const noexcept { return wjr::intrusive::empty(this); }
};

template <typename T>
struct forward_list_node : forward_list_node_base {
    using iterator = forward_list_node_iterator<T>;
    using const_iterator = forward_list_node_const_iterator<T>;

    forward_list_node() = default;
    forward_list_node(const forward_list_node &) = delete;
    forward_list_node(forward_list_node &&) = delete;
    forward_list_node &operator=(const forward_list_node &) = delete;
    forward_list_node &operator=(forward_list_node &&) = delete;
    ~forward_list_node() = default;

    constexpr iterator begin() noexcept { return iterator(next()); }
    constexpr const_iterator begin() const noexcept { return const_iterator(next()); }
    constexpr const_iterator cbegin() const noexcept { return const_iterator(next()); }

    constexpr iterator end() noexcept { return iterator(this); }
    constexpr const_iterator end() const noexcept { return const_iterator(this); }
    constexpr const_iterator cend() const noexcept { return const_iterator(this); }

    constexpr forward_list_node_base *base() noexcept { return this; }
    constexpr const forward_list_node_base *base() const noexcept { return this; }

    constexpr forward_list_node *next() noexcept {
        return static_cast<forward_list_node *>(wjr::intrusive::next(this));
    }

    constexpr const forward_list_node *next() const noexcept {
        return static_cast<const forward_list_node *>(wjr::intrusive::next(this));
    }

    constexpr bool empty() const noexcept { return wjr::intrusive::empty(this); }

    constexpr T *operator->() noexcept { return static_cast<T *>(this); }

    constexpr const T *operator->() const noexcept {
        return static_cast<const T *>(this);
    }

    constexpr T &operator*() noexcept { return *operator->(); }

    constexpr const T &operator*() const noexcept { return *operator->(); }
};

} // namespace wjr::intrusive

#endif // WJR_CONTAINER_INTRUSIVE_FORWARD_LIST_HPP__