#ifndef WJR_CONTAINER_INTRUSIVE_LIST_HPP__
#define WJR_CONTAINER_INTRUSIVE_LIST_HPP__

#include <wjr/container/intrusive/detail.hpp>

namespace wjr {

template <typename Tag = intrusive_tag<>>
struct list_node;

template <typename T>
constexpr void init(list_node<T> *node) noexcept;

template <typename T>
constexpr void insert(list_node<T> *prev, list_node<T> *next,
                      list_node<T> *node) noexcept;

template <typename T>
constexpr void push_back(list_node<T> *head, list_node<T> *node) noexcept;

template <typename T>
constexpr void push_front(list_node<T> *head, list_node<T> *node) noexcept;

template <typename T>
constexpr void remove_uninit(list_node<T> *node) noexcept;

template <typename T>
constexpr bool empty(const list_node<T> *node) noexcept;

template <typename T>
constexpr list_node<T> *next(list_node<T> *node) noexcept;

template <typename T>
constexpr const list_node<T> *next(const list_node<T> *node) noexcept;

template <typename T>
constexpr list_node<T> *prev(list_node<T> *node) noexcept;

template <typename T>
constexpr const list_node<T> *prev(const list_node<T> *node) noexcept;

template <typename T>
constexpr void replace_uninit(list_node<T> *from, list_node<T> *to) noexcept;

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
        m_node = next(m_node);
        return *this;
    }

    constexpr list_node_const_iterator operator++(int) noexcept {
        list_node_const_iterator tmp(*this);
        ++(*this);
        return tmp;
    }

    constexpr list_node_const_iterator &operator--() noexcept {
        m_node = prev(m_node);
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

template <typename Tag>
struct list_node {
    using iterator = list_node_iterator<Tag>;
    using const_iterator = list_node_const_iterator<Tag>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    list_node() = default;
    list_node(const list_node &) = delete;
    list_node(list_node &&) = delete;
    list_node &operator=(const list_node &) = delete;
    list_node &operator=(list_node &&) = delete;
    ~list_node() = default;

    constexpr iterator begin() noexcept { return iterator(wjr::next(this)); }
    constexpr const_iterator begin() const noexcept {
        return const_iterator(wjr::next(this));
    }
    constexpr const_iterator cbegin() const noexcept {
        return const_iterator(wjr::next(this));
    }

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

    constexpr list_node *next() noexcept { return wjr::next(this); }
    constexpr const list_node *next() const noexcept { return wjr::next(this); }

    constexpr list_node *prev() noexcept { return wjr::prev(this); }
    constexpr const list_node *prev() const noexcept { return wjr::prev(this); }

    constexpr bool empty() const noexcept { return wjr::empty(this); }

    template <typename U = Tag, WJR_REQUIRES(intrusive_use_hook_v<U>)>
    constexpr intrusive_hook_t<U> *operator->() noexcept {
        return static_cast<intrusive_hook_t<U> *>(this);
    }

    template <typename U = Tag, WJR_REQUIRES(intrusive_use_hook_v<U>)>
    constexpr const intrusive_hook_t<U> *operator->() const noexcept {
        return static_cast<const intrusive_hook_t<U> *>(this);
    }

    template <typename U = Tag, WJR_REQUIRES(intrusive_use_hook_v<U>)>
    constexpr intrusive_hook_t<U> &operator*() noexcept {
        return *operator->();
    }

    template <typename U = Tag, WJR_REQUIRES(intrusive_use_hook_v<U>)>
    constexpr const intrusive_hook_t<U> &operator*() const noexcept {
        return *operator->();
    }

    list_node *m_prev;
    list_node *m_next;
};

template <typename T>
constexpr void init(list_node<T> *node) noexcept {
    node->m_prev = node;
    node->m_next = node;
}

template <typename T>
constexpr void insert(list_node<T> *prev, list_node<T> *next,
                      list_node<T> *node) noexcept {
    prev->m_next = node;
    node->m_prev = prev;
    next->m_prev = node;
    node->m_next = next;
}

template <typename T>
constexpr void push_back(list_node<T> *head, list_node<T> *node) noexcept {
    insert(head->m_prev, head, node);
}

template <typename T>
constexpr void push_front(list_node<T> *head, list_node<T> *node) noexcept {
    insert(head, head->m_next, node);
}

template <typename T>
constexpr void remove_uninit(list_node<T> *node) noexcept {
    node->m_prev->m_next = node->m_next;
    node->m_next->m_prev = node->m_prev;
}

template <typename T>
constexpr bool empty(const list_node<T> *node) noexcept {
    return node->m_next == node;
}

template <typename T>
constexpr list_node<T> *next(list_node<T> *node) noexcept {
    return node->m_next;
}

template <typename T>
constexpr const list_node<T> *next(const list_node<T> *node) noexcept {
    return node->m_next;
}

template <typename T>
constexpr list_node<T> *prev(list_node<T> *node) noexcept {
    return node->m_prev;
}

template <typename T>
constexpr const list_node<T> *prev(const list_node<T> *node) noexcept {
    return node->m_prev;
}

template <typename T>
constexpr void replace_uninit(list_node<T> *from, list_node<T> *to) noexcept {
    to->m_prev = from->m_prev;
    to->m_next = from->m_next;
    from->m_prev->m_next = to;
    from->m_next->m_prev = to;
}

} // namespace wjr

#endif // WJR_CONTAINER_INTRUSIVE_LIST_HPP__