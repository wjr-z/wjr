#ifndef WJR_CONTAINER_GENERIC_BPLUS_TREE_HPP__
#define WJR_CONTAINER_GENERIC_BPLUS_TREE_HPP__

#include <wjr/assert.hpp>
#include <wjr/compressed_pair.hpp>
#include <wjr/container/generic/container_fn.hpp>
#include <wjr/container/intrusive/list.hpp>
#include <wjr/inline_key.hpp>
#include <wjr/memory/uninitialized.hpp>

namespace wjr {

template <typename Traits>
struct bplus_tree_node;

template <typename Traits>
struct bplus_tree_inner_node;

template <typename Traits, bool InlineKeys>
struct bplus_tree_leaf_node;

template <typename Key, typename T, typename Compare, size_t Size>
struct bplus_tree_traits {
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<const key_type, mapped_type>;
    using key_compare = Compare;

    constexpr static size_t node_size = Size;
    static constexpr size_t stack_size = node_size == 3   ? 48
                                         : node_size < 7  ? 32
                                         : node_size < 15 ? 24
                                                          : 16;
    static constexpr bool inline_keys =
        is_possible_inline_key_v<key_type> && sizeof(key_type) <= 8;
    using KeyPtr = inline_key<Key, inline_keys>;
    using node_type = bplus_tree_node<bplus_tree_traits>;
    using inner_node_type = bplus_tree_inner_node<bplus_tree_traits>;
    using leaf_node_type = bplus_tree_leaf_node<bplus_tree_traits, inline_keys>;

private:
    template <typename Other>
    static void __native_copy(Other *WJR_RESTRICT first, Other *WJR_RESTRICT last,
                              Other *WJR_RESTRICT dest) noexcept {
        for (; first != last; ++first, ++dest) {
            *dest = *first;
            WJR_COMPILER_EMPTY_ASM();
        }
    }

    template <typename Other>
    static void __native_copy_backward(Other *first, Other *last, Other *dest) noexcept {
        for (; first != last;) {
            *--dest = *--last;
            WJR_COMPILER_EMPTY_ASM();
        }
    }

public:
    template <typename Other>
    static void copy(Other *WJR_RESTRICT first, Other *WJR_RESTRICT last,
                     Other *WJR_RESTRICT dest) noexcept {
        if constexpr (node_size <= 8) {
            return __native_copy(first, last, dest);
        } else {
            (void)std::copy(first, last, dest);
        }
    }

    template <typename Other>
    static void copy_backward(Other *first, Other *last, Other *dest) noexcept {
        if constexpr (node_size <= 8) {
            return __native_copy_backward(first, last, dest);
        } else {
            (void)std::copy_backward(first, last, dest);
        }
    }
};

template <typename Traits>
struct bplus_tree_node {
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;
    constexpr static size_t node_size = Traits::node_size;
    using KeyPtr = typename Traits::KeyPtr;
    using inner_node_type = typename Traits::inner_node_type;
    using leaf_node_type = typename Traits::leaf_node_type;

    WJR_INTRINSIC_CONSTEXPR inner_node_type *as_inner() noexcept {
        return (inner_node_type *)(this);
    }

    WJR_INTRINSIC_CONSTEXPR const inner_node_type *as_inner() const noexcept {
        return (inner_node_type *)(this);
    }

    WJR_INTRINSIC_CONSTEXPR leaf_node_type *as_leaf() noexcept {
        return (leaf_node_type *)(this);
    }

    WJR_INTRINSIC_CONSTEXPR const leaf_node_type *as_leaf() const noexcept {
        return (leaf_node_type *)(this);
    }

    int32_t m_size;
    uint32_t m_pos;
    bplus_tree_node *m_parent;
};

template <typename Traits>
struct bplus_tree_inner_node : bplus_tree_node<Traits> {
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;
    constexpr static size_t node_size = Traits::node_size;
    using KeyPtr = typename Traits::KeyPtr;

    alignas(16) KeyPtr m_keys[node_size];
    alignas(16) bplus_tree_node<Traits> *m_sons[node_size + 1];
};

template <typename Traits, bool InlineKeys>
struct bplus_tree_leaf_node : bplus_tree_node<Traits>, intrusive::list_node {
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;
    constexpr static size_t node_size = Traits::node_size;
    using KeyPtr = typename Traits::KeyPtr;
    using ListNode = intrusive::list_node;

    const key_type &__get_key(uint8_t pos) const noexcept { return *m_keys[pos]; }

    WJR_INTRINSIC_CONSTEXPR ListNode *__get_list() noexcept { return this; }
    WJR_INTRINSIC_CONSTEXPR const ListNode *__get_list() const noexcept { return this; }

    alignas(16) KeyPtr m_keys[node_size];
    alignas(16) value_type *m_values[node_size];
};

template <typename Traits>
struct bplus_tree_leaf_node<Traits, false> : bplus_tree_node<Traits>,
                                             intrusive::list_node {
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;
    constexpr static size_t node_size = Traits::node_size;
    using ListNode = intrusive::list_node;

    const key_type &__get_key(uint8_t pos) const noexcept { return m_values[pos]->first; }

    WJR_INTRINSIC_CONSTEXPR ListNode *__get_list() noexcept { return this; }
    WJR_INTRINSIC_CONSTEXPR const ListNode *__get_list() const noexcept { return this; }

    alignas(16) value_type *m_values[node_size];
};

template <typename Traits, typename Alloc>
class basic_bplus_tree;

template <typename Traits>
class bplus_tree_const_iterator {
    using node_type = typename Traits::node_type;
    using inner_node_type = typename Traits::inner_node_type;
    using leaf_node_type = typename Traits::leaf_node_type;

    template <typename Other, typename Alloc>
    friend class basic_bplus_tree;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename Traits::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type *;
    using reference = const value_type &;

    bplus_tree_const_iterator() noexcept = default;
    bplus_tree_const_iterator(const bplus_tree_const_iterator &) noexcept = default;
    bplus_tree_const_iterator(bplus_tree_const_iterator &&) noexcept = default;
    bplus_tree_const_iterator &
    operator=(const bplus_tree_const_iterator &) noexcept = default;
    bplus_tree_const_iterator &operator=(bplus_tree_const_iterator &&) noexcept = default;
    ~bplus_tree_const_iterator() noexcept = default;

    bplus_tree_const_iterator(const intrusive::list_node *list_node, uint8_t pos) noexcept
        : m_node(const_cast<intrusive::list_node *>(list_node)), m_pos(pos) {}

    reference operator*() const noexcept { return *get_leaf()->m_values[m_pos]; }

    pointer operator->() const noexcept { return get_leaf()->m_values[m_pos]; }

    bplus_tree_const_iterator &operator++() noexcept {
        ++m_pos;
        return __adjust_next();
    }

    bplus_tree_const_iterator operator++(int) noexcept {
        bplus_tree_const_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    bplus_tree_const_iterator &operator--() noexcept {
        if (m_pos != 0) {
            --m_pos;
        } else {
            m_node = m_node->prev;
            m_pos = -get_leaf()->m_size - 1;
        }

        return *this;
    }

    bplus_tree_const_iterator operator--(int) noexcept {
        bplus_tree_const_iterator tmp = *this;
        --*this;
        return tmp;
    }

    bool operator==(const bplus_tree_const_iterator &other) const noexcept {
        return m_node == other.m_node && m_pos == other.m_pos;
    }

    bool operator!=(const bplus_tree_const_iterator &other) const noexcept {
        return !(*this == other);
    }

    leaf_node_type *get_leaf() const noexcept {
        return static_cast<leaf_node_type *>(m_node);
    }

    intrusive::list_node *get_node() const noexcept { return m_node; }

    uint8_t get_pos() const noexcept { return m_pos; }

protected:
    bplus_tree_const_iterator &__adjust_next() noexcept {
        if (m_pos == (uint8_t)(-get_leaf()->m_size)) {
            m_node = m_node->next;
            m_pos = 0;
        }

        return *this;
    }

private:
    intrusive::list_node *m_node = nullptr;
    uint8_t m_pos = 0;
};

template <typename Traits>
class bplus_tree_iterator : public bplus_tree_const_iterator<Traits> {
    using Mybase = bplus_tree_const_iterator<Traits>;

    template <typename Other, typename Alloc>
    friend class basic_bplus_tree;

public:
    using Mybase::Mybase;

    using iterator_category = typename Mybase::iterator_category;
    using value_type = typename Mybase::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type *;
    using reference = value_type &;

    bplus_tree_iterator(const Mybase &other) noexcept : Mybase(other) {}

    value_type &operator*() const noexcept {
        return const_cast<value_type &>(Mybase::operator*());
    }

    value_type *operator->() const noexcept {
        return const_cast<value_type *>(Mybase::operator->());
    }

    bplus_tree_iterator &operator++() noexcept {
        Mybase::operator++();
        return *this;
    }

    bplus_tree_iterator operator++(int) noexcept {
        bplus_tree_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    bplus_tree_iterator &operator--() noexcept {
        Mybase::operator--();
        return *this;
    }

    bplus_tree_iterator operator--(int) noexcept {
        bplus_tree_iterator tmp = *this;
        --*this;
        return tmp;
    }

    bool operator==(const bplus_tree_iterator &other) const noexcept {
        return Mybase::operator==(other);
    }

    bool operator!=(const bplus_tree_iterator &other) const noexcept {
        return Mybase::operator!=(other);
    }

protected:
    bplus_tree_iterator &__adjust_next() noexcept {
        Mybase::__adjust_next();
        return *this;
    }
};

template <typename Traits, typename Alloc>
class basic_bplus_tree {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<uint8_t>;
    using _Alty_traits = std::allocator_traits<_Alty>;
    using storage_fn_type = container_fn<_Alty>;

    friend class container_fn<_Alty>;

    using mapped_type = typename Traits::mapped_type;
    static constexpr size_t node_size = Traits::node_size;
    static constexpr size_t stack_size = Traits::stack_size;
    static constexpr bool inline_keys = Traits::inline_keys;
    using KeyPtr = typename Traits::KeyPtr;
    static constexpr size_t floor_half = node_size / 2;
    static constexpr size_t ceil_half = node_size - floor_half;

    using node_type = typename Traits::node_type;
    using inner_node_type = typename Traits::inner_node_type;
    using leaf_node_type = typename Traits::leaf_node_type;

public:
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;
    using key_compare = typename Traits::key_compare;
    using allocator_type = Alloc;
    using size_type = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;
    using iterator = bplus_tree_iterator<Traits>;
    using const_iterator = bplus_tree_const_iterator<Traits>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static_assert(node_size >= 3, "node_size must be greater than or equal to 3.");
    static_assert(node_size <= 255, "node_size must be less than or equal to 255.");

    basic_bplus_tree() noexcept(std::is_nothrow_default_constructible_v<_Alty>) {
        init(&m_sentry);
    }

    // not implemented currently
    basic_bplus_tree(const basic_bplus_tree &) = delete;

    basic_bplus_tree(basic_bplus_tree &&other) noexcept(
        std::is_nothrow_move_constructible_v<key_compare>
            &&std::is_nothrow_move_constructible_v<_Alty>)
        : m_pair(std::piecewise_construct,
                 std::forward_as_tuple(std::move(other.key_comp())),
                 std::forward_as_tuple(
                     std::piecewise_construct,
                     std::forward_as_tuple(std::move(other.__get_allocator())),
                     std::forward_as_tuple())) {
        __take_tree(std::move(other));
    }

    ~basic_bplus_tree() { __destroy_and_deallocate(); }

    template <typename... Args>
    void emplace(Args &&...args) {
        auto &al = __get_allocator();
        value_type *const xval =
            (value_type *)_Alty_traits::allocate(al, sizeof(value_type));
        uninitialized_construct_using_allocator(xval, al, std::forward<Args>(args)...);

        __insert_value_ptr(xval);
    }

    void insert(const value_type &val) { emplace(val); }
    void insert(value_type &&val) { emplace(std::move(val)); }

    void erase(const key_type &key);

    iterator upper_bound(const key_type &key) {
        return iterator(__upper_bound(key).__adjust_next());
    }

    const_iterator upper_bound(const key_type &key) const {
        return const_iterator(__upper_bound(key).__adjust_next());
    }

    constexpr key_compare &key_comp() noexcept { return m_pair.first(); }
    constexpr const key_compare &key_comp() const noexcept { return m_pair.first(); }

    iterator begin() noexcept { return iterator(intrusive::begin(&m_sentry), 0); }
    const_iterator begin() const noexcept {
        return const_iterator(intrusive::begin(&m_sentry), 0);
    }
    const_iterator cbegin() const noexcept {
        return const_iterator(intrusive::begin(&m_sentry), 0);
    }

    iterator end() noexcept { return iterator(&m_sentry, 0); }
    const_iterator end() const noexcept { return const_iterator(&m_sentry, 0); }
    const_iterator cend() const noexcept { return const_iterator(&m_sentry, 0); }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

private:
    void __take_tree(basic_bplus_tree &&other) noexcept {
        auto root = other.__get_root();
        if (root == nullptr) {
            init(&m_sentry);
            return;
        }

        __get_root() = root;
        other.__get_root() = nullptr;
        replace(&other.m_sentry, &m_sentry);
        init(&other.m_sentry);
    }

    // member function for container_fn (START)

    WJR_PURE WJR_INTRINSIC_CONSTEXPR _Alty &__get_allocator() noexcept {
        return m_pair.second().first();
    }

    WJR_PURE WJR_INTRINSIC_CONSTEXPR const _Alty &__get_allocator() const noexcept {
        return m_pair.second().first();
    }

    void __destroy_and_deallocate() noexcept {
        node_type *current = __get_root();
        if (current == nullptr) {
            return;
        }

        auto &al = __get_allocator();
        int32_t cur_size = current->m_size;

        if (cur_size < 0) {
            cur_size = -cur_size;

            for (uint8_t i = 0; i < cur_size; ++i) {
                _Alty_traits::destroy(al, current->as_leaf()->m_values[i]);
                _Alty_traits::deallocate(al, (uint8_t *)current->as_leaf()->m_values[i],
                                         sizeof(value_type));
            }

            _Alty_traits::deallocate(al, (uint8_t *)current, sizeof(leaf_node_type));
            return;
        }

        while (cur_size >= 0) {
            current = current->as_inner()->m_sons[0];
            cur_size = current->m_size;
        }

        cur_size = -cur_size;

        do {
            for (uint8_t i = 0; i < cur_size; ++i) {
                _Alty_traits::destroy(al, current->as_leaf()->m_values[i]);
                _Alty_traits::deallocate(al, (uint8_t *)current->as_leaf()->m_values[i],
                                         sizeof(value_type));
            }

            intrusive::list_node *next = current->as_leaf()->next;

            node_type *parent = current->m_parent;
            uint8_t pos = current->m_pos;
            _Alty_traits::deallocate(al, (uint8_t *)current, sizeof(leaf_node_type));

            while (pos == parent->m_size) {
                current = parent;
                parent = current->m_parent;
                pos = current->m_pos;
                _Alty_traits::deallocate(al, (uint8_t *)current, sizeof(inner_node_type));

                if (parent == nullptr) {
                    return;
                }
            }

            WJR_ASSERT(next != &m_sentry);

            current = static_cast<leaf_node_type *>(next);
            cur_size = -current->m_size;
        } while (true);
    }

    void __take_storage(basic_bplus_tree &&other) noexcept {
        key_comp() = std::move(other.key_comp());
        __take_tree(std::move(other));
    }

    // member function for container_fn (END)

    void __insert_root(value_type *xval) {
        leaf_node_type *root = (leaf_node_type *)_Alty_traits::allocate(
            __get_allocator(), sizeof(leaf_node_type));

        __get_root() = root;

        root->m_size = -1;
        root->m_parent = nullptr;
        if constexpr (inline_keys) {
            root->m_keys[0] = xval->first;
        }
        root->m_values[0] = xval;
        push_back(&m_sentry, root);
        return;
    }

    WJR_NOINLINE void __insert_iter(const_iterator iter, value_type *xval) {
        leaf_node_type *leaf;

        do {
            intrusive::list_node *node = iter.get_node();

            if (node == &m_sentry) {
                __insert_root(xval);
                return;
            }

            leaf = static_cast<leaf_node_type *>(node);
        } while (0);

        uint8_t pos = iter.get_pos();

        KeyPtr *keys;
        node_type *inst;

        KeyPtr key = xval->first;
        int32_t cur_size = -leaf->m_size;

        do {
            value_type **values;

            if constexpr (inline_keys) {
                keys = leaf->m_keys;
            }

            values = leaf->m_values;

            if (WJR_LIKELY(cur_size != node_size)) {
                WJR_ASSUME(pos <= cur_size);

                if constexpr (inline_keys) {
                    Traits::copy_backward(keys + pos, keys + cur_size,
                                          keys + cur_size + 1);
                }

                Traits::copy_backward(values + pos, values + cur_size,
                                      values + cur_size + 1);

                leaf->m_size = -(cur_size + 1);
                if constexpr (inline_keys) {
                    keys[pos] = xval->first;
                }

                values[pos] = xval;
                return;
            }

            leaf_node_type *leaf_inst = (leaf_node_type *)_Alty_traits::allocate(
                __get_allocator(), sizeof(leaf_node_type));
            inst = leaf_inst;
            push_back(leaf->__get_list(), leaf_inst->__get_list());

            leaf->m_size = -(int32_t)(floor_half + 1);
            leaf_inst->m_size = -(int32_t)(node_size - floor_half);

            if (pos <= floor_half) {
                if constexpr (inline_keys) {
                    Traits::copy(keys + floor_half, keys + node_size, leaf_inst->m_keys);
                }

                Traits::copy(values + floor_half, values + node_size,
                             leaf_inst->m_values);

                if constexpr (inline_keys) {
                    Traits::copy_backward(keys + pos, keys + floor_half,
                                          keys + floor_half + 1);
                }

                Traits::copy_backward(values + pos, values + floor_half,
                                      values + floor_half + 1);

                if constexpr (inline_keys) {
                    keys[pos] = xval->first;
                }

                values[pos] = xval;
            } else {
                if constexpr (inline_keys) {
                    Traits::copy(keys + floor_half + 1, keys + pos, leaf_inst->m_keys);
                }

                Traits::copy(values + floor_half + 1, values + pos, leaf_inst->m_values);

                const uint8_t rpos = pos - floor_half - 1;

                if constexpr (inline_keys) {
                    Traits::copy(keys + pos, keys + node_size,
                                 leaf_inst->m_keys + rpos + 1);
                }

                Traits::copy(values + pos, values + node_size,
                             leaf_inst->m_values + rpos + 1);

                if constexpr (inline_keys) {
                    leaf_inst->m_keys[rpos] = xval->first;
                }

                leaf_inst->m_values[rpos] = xval;
            }

            key = leaf_inst->__get_key(0);
        } while (0);

        inner_node_type *parent = leaf->m_parent->as_inner();
        inner_node_type *current = leaf->as_inner();

        while (parent != nullptr) {
            inst->m_parent = parent;
            pos = current->m_pos + 1;
            current = parent;
            cur_size = current->m_size + 1;
            keys = current->m_keys;
            node_type **sons = current->m_sons;

            if (WJR_LIKELY(cur_size != node_size)) {
                WJR_ASSERT(pos <= cur_size);

                Traits::copy_backward(keys + pos - 1, keys + cur_size - 1,
                                      keys + cur_size);
                Traits::copy_backward(sons + pos, sons + cur_size, sons + cur_size + 1);

                current->m_size = cur_size;
                keys[pos - 1] = key;
                sons[pos] = inst;

                inst->m_pos = pos;
                for (uint8_t i = pos + 1; i <= cur_size; ++i) {
                    sons[i]->m_pos = i;
                }

                return;
            }

            parent = current->m_parent->as_inner();

            inner_node_type *pinst = (inner_node_type *)_Alty_traits::allocate(
                __get_allocator(), sizeof(inner_node_type));

            current->m_size = (int32_t)(floor_half);
            const uint32_t rsize = node_size - floor_half - 1;
            pinst->m_size = (int32_t)(rsize);

            KeyPtr next_key;

            if (pos <= floor_half) {
                next_key = keys[floor_half - 1];

                Traits::copy(keys + floor_half, keys + node_size - 1, pinst->m_keys);
                Traits::copy(sons + floor_half, sons + node_size, pinst->m_sons);
                Traits::copy_backward(keys + pos - 1, keys + floor_half - 1,
                                      keys + floor_half);
                Traits::copy_backward(sons + pos, sons + floor_half,
                                      sons + floor_half + 1);

                keys[pos - 1] = key;
                sons[pos] = inst;

                inst->m_pos = pos;
                for (uint8_t i = pos + 1; i <= floor_half; ++i) {
                    sons[i]->m_pos = i;
                }
            } else {
                if (pos == ceil_half + 1) {
                    next_key = key;
                    Traits::copy(keys + floor_half, keys + node_size - 1, pinst->m_keys);
                    Traits::copy(sons + floor_half + 1, sons + node_size,
                                 pinst->m_sons + 1);

                    pinst->m_sons[0] = inst;
                } else {
                    next_key = keys[floor_half];
                    Traits::copy(keys + floor_half + 1, keys + pos - 1, pinst->m_keys);
                    Traits::copy(sons + floor_half + 1, sons + pos, pinst->m_sons);

                    const uint8_t rpos = pos - floor_half - 2;

                    Traits::copy(keys + pos - 1, keys + node_size - 1,
                                 pinst->m_keys + rpos + 1);
                    Traits::copy(sons + pos, sons + node_size, pinst->m_sons + rpos + 2);

                    pinst->m_keys[rpos] = key;
                    pinst->m_sons[rpos + 1] = inst;
                }
            }

            for (uint8_t i = 0; i <= rsize; ++i) {
                pinst->m_sons[i]->m_parent = pinst;
                pinst->m_sons[i]->m_pos = i;
            }

            key = next_key;
            inst = pinst;
        }

        inner_node_type *new_root = (inner_node_type *)_Alty_traits::allocate(
            __get_allocator(), sizeof(inner_node_type));
        new_root->m_size = 1;
        new_root->m_parent = nullptr;
        new_root->m_keys[0] = key;
        new_root->m_sons[0] = current;
        new_root->m_sons[1] = inst;
        current->m_pos = 0;
        inst->m_pos = 1;

        current->m_parent = new_root;
        inst->m_parent = new_root;

        __get_root() = new_root;
        return;
    }

    void __insert_value_ptr(value_type *xval) {
        __insert_iter(__upper_bound(xval->first), xval);
    }

    void __erase_iter(const_iterator iter) {
        leaf_node_type *leaf = iter.get_leaf();
        uint8_t pos = iter.get_pos();
        inner_node_type *parent = leaf->m_parent->as_inner();

        if (parent == nullptr) {

            return;
        }
    }

    WJR_PURE const_iterator __upper_bound(const key_type &key) const {
        const node_type *current = __get_root();

        if (current == nullptr) {
            return cend();
        }

        int32_t cur_size = current->m_size;
        const auto &comp = key_comp();

        uint8_t pos;

        while (cur_size >= 0) {
            if (!comp(key, *current->as_inner()->m_keys[0])) {
                goto NOT_LEFT;
            }

            current = current->as_inner()->m_sons[0];
            cur_size = current->m_size;
        }

        cur_size = -cur_size;

        if (comp(key, current->as_leaf()->__get_key(0))) {
            return const_iterator(current->as_leaf()->__get_list(), 0);
        }

        if (cur_size == 1) {
            return const_iterator(current->as_leaf()->__get_list(), 1);
        }

        goto LEAF;

    NOT_LEFT:
        if (cur_size == 1) {
            pos = 1;
        } else {
            pos = 1 + __upper_search(current->as_inner(), cur_size - 1, key, comp);
        }

        current = current->as_inner()->m_sons[pos];
        cur_size = current->m_size;

        while (cur_size >= 0) {
            pos = __upper_search(current->as_inner(), cur_size, key, comp);

            current = current->as_inner()->m_sons[pos];
            cur_size = current->m_size;
        }

        cur_size = -cur_size;

    LEAF:
        // not left node, don't need to compare with the first key
        pos = 1 + __upper_search(current->as_leaf(), cur_size - 1, key, comp);

        return const_iterator(current->as_leaf()->__get_list(), pos);
    }

    WJR_PURE WJR_INTRINSIC_INLINE static uint8_t
    __upper_search(const inner_node_type *current, uint8_t size, const key_type &key,
                   const key_compare &comp) {
        uint8_t pos = 0;
        do {
            if (comp(key, *current->m_keys[pos])) {
                break;
            }
        } while (++pos != size);
        return pos;
    }

    WJR_PURE WJR_INTRINSIC_INLINE static uint8_t
    __upper_search(const leaf_node_type *current, uint8_t size, const key_type &key,
                   const key_compare &comp) {
        uint8_t pos = 0;
        do {
            if (comp(key, current->__get_key(pos))) {
                break;
            }
        } while (++pos != size);
        return pos;
    }

    WJR_INTRINSIC_CONSTEXPR node_type *&__get_root() noexcept {
        return m_pair.second().second();
    }

    WJR_INTRINSIC_CONSTEXPR const node_type *__get_root() const noexcept {
        return m_pair.second().second();
    }

    compressed_pair<key_compare, compressed_pair<_Alty, node_type *>> m_pair;
    intrusive::list_node m_sentry;
};

} // namespace wjr

#endif // WJR_CONTAINER_GENERIC_BPLUS_TREE_HPP__