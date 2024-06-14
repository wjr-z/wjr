#ifndef WJR_CONTAINER_INTRUSIVE_LIST_HPP__
#define WJR_CONTAINER_INTRUSIVE_LIST_HPP__

#include <wjr/container/intrusive/details.hpp>

namespace wjr {

template <typename Tag = void>
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

template <typename Obj, typename Tag = void>
struct list_tag {
    using obj_type = Obj;
};

template <typename Tag>
struct is_list_tag : std::false_type {};

template <typename Obj, typename Tag>
struct is_list_tag<list_tag<Obj, Tag>>
    : std::is_base_of<list_node<list_tag<Obj, Tag>>, Obj> {};

template <typename Tag>
inline constexpr bool is_list_tag_v = is_list_tag<Tag>::value;

template <typename Tag>
using list_obj_t = typename Tag::obj_type;

template <typename T>
class list_node_const_iterator {
    using node_type = list_node<T>;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = node_type;
    using reference = const node_type &;
    using pointer = const node_type *;
    using difference_type = std::ptrdiff_t;

    constexpr list_node_const_iterator() noexcept = default;
    constexpr list_node_const_iterator(const list_node_const_iterator &) noexcept =
        default;
    constexpr list_node_const_iterator(list_node_const_iterator &&) noexcept = default;
    constexpr list_node_const_iterator &
    operator=(const list_node_const_iterator &) noexcept = default;
    constexpr list_node_const_iterator &
    operator=(list_node_const_iterator &&) noexcept = default;
    ~list_node_const_iterator() = default;

    constexpr list_node_const_iterator(const node_type *node) noexcept
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

    constexpr operator const node_type *() const noexcept { return m_node; }

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

    constexpr list_node_iterator() noexcept = default;
    constexpr list_node_iterator(const list_node_iterator &) noexcept = default;
    constexpr list_node_iterator(list_node_iterator &&) noexcept = default;
    constexpr list_node_iterator &
    operator=(const list_node_iterator &) noexcept = default;
    constexpr list_node_iterator &operator=(list_node_iterator &&) noexcept = default;
    ~list_node_iterator() = default;

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

    constexpr operator node_type *() const noexcept {
        return const_cast<node_type *>(static_cast<const node_type *>(*this));
    }
};

template <typename Tag>
struct list_node {
    using iterator = list_node_iterator<Tag>;
    using const_iterator = list_node_const_iterator<Tag>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr list_node() noexcept = default;
    list_node(const list_node &) = delete;
    list_node(list_node &&) = delete;
    list_node &operator=(const list_node &) = delete;
    list_node &operator=(list_node &&) = delete;
    ~list_node() = default;

    constexpr iterator begin() noexcept { return iterator(next(this)); }
    constexpr const_iterator begin() const noexcept { return const_iterator(next(this)); }
    constexpr const_iterator cbegin() const noexcept {
        return const_iterator(next(this));
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

    constexpr bool empty() const noexcept { return wjr::empty(this); }

    template <typename U = Tag, WJR_REQUIRES(is_list_tag_v<U>)>
    constexpr list_obj_t<U> *operator->() noexcept {
        return static_cast<list_obj_t<Tag> *>(this);
    }

    template <typename U = Tag, WJR_REQUIRES(is_list_tag_v<U>)>
    constexpr const list_obj_t<U> *operator->() const noexcept {
        return static_cast<const list_obj_t<Tag> *>(this);
    }

    template <typename U = Tag, WJR_REQUIRES(is_list_tag_v<U>)>
    constexpr list_obj_t<U> &operator*() noexcept {
        return *operator->();
    }

    template <typename U = Tag, WJR_REQUIRES(is_list_tag_v<U>)>
    constexpr const list_obj_t<U> &operator*() const noexcept {
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