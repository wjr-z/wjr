#ifndef WJR_CONTAINER_INTRUSIVE_LIST_HPP__
#define WJR_CONTAINER_INTRUSIVE_LIST_HPP__

#include <wjr/type_traits.hpp>
#include <wjr/assert.hpp>

namespace wjr::intrusive {

struct list_node_base {
    list_node_base *m_prev;
    list_node_base *m_next;
};

constexpr void init(list_node_base *node) noexcept {
    node->m_prev = node;
    node->m_next = node;
}

WJR_INTRINSIC_CONSTEXPR bool __insert_check(list_node_base *prev, list_node_base *next,
                                            list_node_base *node) noexcept {
    if (WJR_LIKELY(next->m_prev == prev && prev->m_next == next && node != prev &&
                   node != next)) {
        return true;
    }

    return false;
}

constexpr void insert(list_node_base *prev, list_node_base *next,
                      list_node_base *node) noexcept {
    WJR_ASSERT_L2(__insert_check(prev, next, node));

    prev->m_next = node;
    node->m_prev = prev;
    next->m_prev = node;
    node->m_next = next;
}

constexpr void push_back(list_node_base *head, list_node_base *node) noexcept {
    insert(head->m_prev, head, node);
}

constexpr void push_front(list_node_base *head, list_node_base *node) noexcept {
    insert(head, head->m_next, node);
}

constexpr bool empty(const list_node_base *node) noexcept { return node->m_next == node; }

constexpr list_node_base *next(list_node_base *node) noexcept { return node->m_next; }

constexpr const list_node_base *next(const list_node_base *node) noexcept {
    return node->m_next;
}

constexpr list_node_base *prev(list_node_base *node) noexcept { return node->m_prev; }

constexpr const list_node_base *prev(const list_node_base *node) noexcept {
    return node->m_prev;
}

constexpr void remove_uninit(list_node_base *node) noexcept {
    node->m_prev->m_next = node->m_next;
    node->m_next->m_prev = node->m_prev;
}

constexpr void remove_init(list_node_base *node) noexcept {
    remove_uninit(node);
    init(node);
}

constexpr void replace_uninit(list_node_base *from, list_node_base *to) noexcept {
    to->m_next = from->m_next;
    from->m_next->m_prev = to;
    to->m_prev = from->m_prev;
    from->m_prev->m_next = to;
}

constexpr void replace_init(list_node_base *from, list_node_base *to) noexcept {
    replace_uninit(from, to);
    init(from);
}

template <typename T>
struct list_node;

template <typename T>
class list_node_const_iterator {
    using node_type = list_node<T>;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = node_type;
    using reference = const node_type &;
    using pointer = const node_type *;
    using difference_type = std::ptrdiff_t;

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(list_node_const_iterator);

    constexpr list_node_const_iterator(pointer node) noexcept
        : m_node(const_cast<node_type *>(node)) {}

    constexpr reference operator*() const noexcept { return *m_node; }
    constexpr pointer operator->() const noexcept { return m_node; }

    constexpr list_node_const_iterator &operator++() noexcept {
        m_node = m_node->next();
        return *this;
    }

    constexpr list_node_const_iterator operator++(int) noexcept {
        list_node_const_iterator tmp(*this);
        ++(*this);
        return tmp;
    }

    constexpr list_node_const_iterator &operator--() noexcept {
        m_node = m_node->prev();
        return *this;
    }

    constexpr list_node_const_iterator operator--(int) noexcept {
        list_node_const_iterator tmp(*this);
        --(*this);
        return tmp;
    }

    constexpr bool operator==(const list_node_const_iterator &other) const noexcept {
        return m_node == other.m_node;
    }

    constexpr bool operator!=(const list_node_const_iterator &other) const noexcept {
        return !(*this == other);
    }

private:
    node_type *m_node{};
};

template <typename T>
class list_node_iterator : public list_node_const_iterator<T> {
    using Mybase = list_node_const_iterator<T>;
    using node_type = list_node<T>;

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

    constexpr list_node_iterator &operator++() noexcept {
        Mybase::operator++();
        return *this;
    }

    constexpr list_node_iterator operator++(int) noexcept {
        list_node_iterator tmp(*this);
        ++(*this);
        return tmp;
    }

    constexpr list_node_iterator &operator--() noexcept {
        Mybase::operator--();
        return *this;
    }

    constexpr list_node_iterator operator--(int) noexcept {
        list_node_iterator tmp(*this);
        --(*this);
        return tmp;
    }
};

template <>
struct list_node<void> : list_node_base {
    using iterator = list_node_iterator<void>;
    using const_iterator = list_node_const_iterator<void>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    list_node() = default;
    list_node(const list_node &) = delete;
    list_node(list_node &&) = delete;
    list_node &operator=(const list_node &) = delete;
    list_node &operator=(list_node &&) = delete;
    ~list_node() = default;

    constexpr iterator begin() noexcept { return iterator(next()); }
    constexpr const_iterator begin() const noexcept { return const_iterator(next()); }
    constexpr const_iterator cbegin() const noexcept { return const_iterator(next()); }

    constexpr iterator end() noexcept { return iterator(this); }
    constexpr const_iterator end() const noexcept { return const_iterator(this); }
    constexpr const_iterator cend() const noexcept { return const_iterator(this); }

    constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    constexpr list_node_base *base() noexcept { return this; }
    constexpr const list_node_base *base() const noexcept { return this; }

    constexpr list_node *next() noexcept {
        return static_cast<list_node *>(wjr::intrusive::next(this));
    }

    constexpr const list_node *next() const noexcept {
        return static_cast<const list_node *>(wjr::intrusive::next(this));
    }

    constexpr list_node *prev() noexcept {
        return static_cast<list_node *>(wjr::intrusive::prev(this));
    }
    constexpr const list_node *prev() const noexcept {
        return static_cast<const list_node *>(wjr::intrusive::prev(this));
    }

    constexpr bool empty() const noexcept { return wjr::intrusive::empty(this); }
};

template <typename T>
struct list_node : list_node_base {
    using iterator = list_node_iterator<T>;
    using const_iterator = list_node_const_iterator<T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    list_node() = default;
    list_node(const list_node &) = delete;
    list_node(list_node &&) = delete;
    list_node &operator=(const list_node &) = delete;
    list_node &operator=(list_node &&) = delete;
    ~list_node() = default;

    constexpr iterator begin() noexcept { return iterator(next()); }
    constexpr const_iterator begin() const noexcept { return const_iterator(next()); }
    constexpr const_iterator cbegin() const noexcept { return const_iterator(next()); }

    constexpr iterator end() noexcept { return iterator(this); }
    constexpr const_iterator end() const noexcept { return const_iterator(this); }
    constexpr const_iterator cend() const noexcept { return const_iterator(this); }

    constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    constexpr list_node_base *base() noexcept { return this; }
    constexpr const list_node_base *base() const noexcept { return this; }

    constexpr list_node *next() noexcept {
        return static_cast<list_node *>(wjr::intrusive::next(this));
    }

    constexpr const list_node *next() const noexcept {
        return static_cast<const list_node *>(wjr::intrusive::next(this));
    }

    constexpr list_node *prev() noexcept {
        return static_cast<list_node *>(wjr::intrusive::prev(this));
    }
    constexpr const list_node *prev() const noexcept {
        return static_cast<const list_node *>(wjr::intrusive::prev(this));
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

#endif // WJR_CONTAINER_INTRUSIVE_LIST_HPP__