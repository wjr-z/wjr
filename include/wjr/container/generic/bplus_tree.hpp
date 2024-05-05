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

template <typename Key, typename T, typename Compare, size_t Size, bool Multi = true>
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
    using InlineKey = inline_key<Key, inline_keys>;
    using node_type = bplus_tree_node<bplus_tree_traits>;
    using inner_node_type = bplus_tree_inner_node<bplus_tree_traits>;
    using leaf_node_type = bplus_tree_leaf_node<bplus_tree_traits, inline_keys>;
    static constexpr bool multi = Multi;

    WJR_INTRINSIC_INLINE static const key_type &
    get_key(const value_type &value) noexcept {
        return value.first;
    }

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
    using InlineKey = typename Traits::InlineKey;
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

    int m_size;
    unsigned int m_pos;
    bplus_tree_node *m_parent;
};

template <typename Traits>
struct bplus_tree_inner_node : bplus_tree_node<Traits> {
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;
    constexpr static size_t node_size = Traits::node_size;
    using InlineKey = typename Traits::InlineKey;

    alignas(16) InlineKey m_keys[node_size];
    alignas(16) bplus_tree_node<Traits> *m_sons[node_size + 1];
};

template <typename Traits, bool InlineKeys>
struct bplus_tree_leaf_node : bplus_tree_node<Traits>, intrusive::list_node<> {
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;
    constexpr static size_t node_size = Traits::node_size;
    using InlineKey = typename Traits::InlineKey;
    using ListNode = intrusive::list_node<>;

    const key_type &__get_key(unsigned int pos) const noexcept { return *m_keys[pos]; }

    WJR_INTRINSIC_INLINE void __copy(unsigned int start, unsigned int end,
                                     unsigned int dst_start, bplus_tree_leaf_node *dst) {
        Traits::copy(m_keys + start, m_keys + end, dst->m_keys + dst_start);
        Traits::copy(m_values + start, m_values + end, dst->m_values + dst_start);
    }

    WJR_INTRINSIC_INLINE void __copy_backward(unsigned int start, unsigned int end,
                                              unsigned int dst_end,
                                              bplus_tree_leaf_node *dst) {
        Traits::copy_backward(m_keys + start, m_keys + end, dst->m_keys + dst_end);
        Traits::copy_backward(m_values + start, m_values + end, dst->m_values + dst_end);
    }

    WJR_INTRINSIC_INLINE void __assign(unsigned int idx, value_type *const value) {
        m_keys[idx] = value->first;
        m_values[idx] = value;
    }

    WJR_INTRINSIC_CONSTEXPR ListNode *__get_list() noexcept { return this; }
    WJR_INTRINSIC_CONSTEXPR const ListNode *__get_list() const noexcept { return this; }

    alignas(16) InlineKey m_keys[node_size];
    alignas(16) value_type *m_values[node_size];
};

template <typename Traits>
struct bplus_tree_leaf_node<Traits, false> : bplus_tree_node<Traits>,
                                             intrusive::list_node<> {
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;
    constexpr static size_t node_size = Traits::node_size;
    using ListNode = intrusive::list_node<>;

    const key_type &__get_key(unsigned int pos) const noexcept {
        return m_values[pos]->first;
    }

    WJR_INTRINSIC_INLINE void __copy(unsigned int start, unsigned int end,
                                     unsigned int dst_start, bplus_tree_leaf_node *dst) {
        Traits::copy(m_values + start, m_values + end, dst->m_values + dst_start);
    }

    WJR_INTRINSIC_INLINE void __copy_backward(unsigned int start, unsigned int end,
                                              unsigned int dst_end,
                                              bplus_tree_leaf_node *dst) {
        Traits::copy_backward(m_values + start, m_values + end, dst->m_values + dst_end);
    }

    WJR_INTRINSIC_INLINE void __assign(unsigned int idx, value_type *const value) {
        m_values[idx] = value;
    }

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

    using ListNode = intrusive::list_node<>;

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

    bplus_tree_const_iterator(const ListNode *list_node, unsigned int pos) noexcept
        : m_node(const_cast<ListNode *>(list_node)), m_pos(pos) {}

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
            m_node = m_node->prev();
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

    ListNode *get_node() const noexcept { return m_node; }

    unsigned int get_pos() const noexcept { return m_pos; }

protected:
    bplus_tree_const_iterator &__adjust_next() noexcept {
        if (m_pos == (unsigned int)(-get_leaf()->m_size)) {
            m_node = m_node->next();
            m_pos = 0;
        }

        return *this;
    }

private:
    ListNode *m_node = nullptr;
    unsigned int m_pos = 0;
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
    using InlineKey = typename Traits::InlineKey;
    static constexpr size_t floor_half = node_size / 2;
    static constexpr size_t ceil_half = node_size - floor_half;

    using node_type = typename Traits::node_type;
    using inner_node_type = typename Traits::inner_node_type;
    using leaf_node_type = typename Traits::leaf_node_type;

    using ListNode = intrusive::list_node<>;

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
        return iterator(__search<true>(key).__adjust_next());
    }

    const_iterator upper_bound(const key_type &key) const {
        return const_iterator(__search<true>(key).__adjust_next());
    }

    constexpr key_compare &key_comp() noexcept { return m_pair.first(); }
    constexpr const key_compare &key_comp() const noexcept { return m_pair.first(); }

    iterator begin() noexcept { return iterator(m_sentry.next(), 0); }
    const_iterator begin() const noexcept { return const_iterator(m_sentry.next(), 0); }
    const_iterator cbegin() const noexcept { return const_iterator(m_sentry.next(), 0); }

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

    void erase(const_iterator iter) { __erase_iter(iter); }

    void __debug(bool print = true) {
        auto root = __get_root();
        if (root != nullptr) {
            (void)__debug(root, print);
        } else {
            if (print)
                printf("empty");
        }
        if (print)
            printf("\n");
    }

private:
    void __take_tree(basic_bplus_tree &&other) noexcept {
        const auto root = other.__get_root();
        if (root == nullptr) {
            init(&m_sentry);
            return;
        }

        __get_root() = root;
        other.__get_root() = nullptr;
        replace_uninit(&other.m_sentry, &m_sentry);
        init(&other.m_sentry);
    }

    // member function for container_fn (START)

    WJR_PURE WJR_INTRINSIC_CONSTEXPR _Alty &__get_allocator() noexcept {
        return m_pair.second().first();
    }

    WJR_PURE WJR_INTRINSIC_CONSTEXPR const _Alty &__get_allocator() const noexcept {
        return m_pair.second().first();
    }

    WJR_NOINLINE void __destroy_and_deallocate() noexcept {
        node_type *current = __get_root();

        // empty tree
        if (current == nullptr) {
            return;
        }

        auto &al = __get_allocator();
        int cur_size = current->m_size;

        // root is leaf
        if (cur_size < 0) {
            const auto leaf = current->as_leaf();
            const unsigned int cur_usize = -cur_size;

            for (unsigned int i = 0; i < cur_usize; ++i) {
                _Alty_traits::destroy(al, leaf->m_values[i]);
                _Alty_traits::deallocate(al, (uint8_t *)leaf->m_values[i],
                                         sizeof(value_type));
            }

            _Alty_traits::deallocate(al, (uint8_t *)leaf, sizeof(leaf_node_type));
            return;
        }

        // skip to the leftmost leaf
        current = begin().get_leaf();
        cur_size = -current->m_size;

        // cache of parent and parent's size
        node_type *parent = current->m_parent;
        unsigned int par_size = parent->m_size;

        // cache of `current' node's position in parent
        unsigned int pos = 0;

        do {
            const auto leaf = current->as_leaf();
            const unsigned int cur_usize = cur_size;

            for (unsigned int i = 0; i < cur_usize; ++i) {
                _Alty_traits::destroy(al, leaf->m_values[i]);
                _Alty_traits::deallocate(al, (uint8_t *)leaf->m_values[i],
                                         sizeof(value_type));
            }

            ListNode *next = leaf->next();
            _Alty_traits::deallocate(al, (uint8_t *)leaf, sizeof(leaf_node_type));

            // if `current' is not the last child of parent
            if (WJR_UNLIKELY(pos++ == par_size)) {
                do {
                    current = parent;
                    parent = current->m_parent;
                    pos = current->m_pos;
                    _Alty_traits::deallocate(al, (uint8_t *)current,
                                             sizeof(inner_node_type));
                    // if `current' is the rightmost leaf
                    if (parent == nullptr) {
                        return;
                    }
                    // if `current' is the last child of parent
                } while (pos == (unsigned int)parent->m_size);

                parent = static_cast<leaf_node_type *>(next)->m_parent;
                par_size = parent->m_size;
                pos = 0;
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
        const auto root = (leaf_node_type *)_Alty_traits::allocate(
            __get_allocator(), sizeof(leaf_node_type));

        __get_root() = root;

        root->m_size = -1;
        root->m_parent = nullptr;
        root->__assign(0, xval);
        m_sentry.push_back(root);
        return;
    }

    WJR_NOINLINE void __insert_iter(const_iterator iter, value_type *xval) {
        unsigned int pos;
        unsigned int cur_size;
        node_type *current;
        node_type *inst;

        do {
            leaf_node_type *leaf;
            do {
                ListNode *const node = iter.get_node();

                // empty
                if (node == &m_sentry) {
                    __insert_root(xval);
                    return;
                }

                leaf = static_cast<leaf_node_type *>(node);
            } while (0);

            pos = iter.get_pos();
            cur_size = -leaf->m_size;

            // non-full leaf
            if (WJR_LIKELY(cur_size != node_size)) {
                WJR_ASSERT_ASSUME(pos <= cur_size);

                leaf->__copy_backward(pos, cur_size, cur_size + 1, leaf);

                leaf->m_size = -(cur_size + 1);
                leaf->__assign(pos, xval);
                return;
            }

            const auto tmp_inst = (leaf_node_type *)_Alty_traits::allocate(
                __get_allocator(), sizeof(leaf_node_type));
            inst = tmp_inst;
            leaf->__get_list()->push_back(tmp_inst->__get_list());

            leaf->m_size = -(int)(floor_half + 1);
            tmp_inst->m_size = -(int)(node_size - floor_half);

            if (pos <= floor_half) {
                leaf->__copy(floor_half, node_size, 0, tmp_inst);
                leaf->__copy_backward(pos, floor_half, floor_half + 1, leaf);
                leaf->__assign(pos, xval);
            } else {
                // pos in tmp_inst
                const unsigned int rpos = pos - floor_half - 1;
                leaf->__copy(floor_half + 1, pos, 0, tmp_inst);
                leaf->__copy(pos, node_size, rpos + 1, tmp_inst);
                tmp_inst->__assign(rpos, xval);
            }

            current = leaf;
        } while (0);

        node_type *parent = current->m_parent;
        InlineKey key = inst->as_leaf()->__get_key(0);

        while (parent != nullptr) {
            inst->m_parent = parent;
            pos = current->m_pos + 1;
            current = parent;
            const auto inner = current->as_inner();

            cur_size = inner->m_size + 1;
            InlineKey *const keys = inner->m_keys;
            node_type **const sons = inner->m_sons;

            // non-full inner
            if (WJR_LIKELY(cur_size != node_size + 1)) {
                Traits::copy_backward(keys + pos - 1, keys + cur_size - 1,
                                      keys + cur_size);
                Traits::copy_backward(sons + pos, sons + cur_size, sons + cur_size + 1);

                inner->m_size = cur_size;
                keys[pos - 1] = key;
                sons[pos] = inst;

                inst->m_pos = pos;
                for (unsigned int i = pos + 1; i <= cur_size; ++i) {
                    sons[i]->m_pos = i;
                }

                return;
            }

            parent = inner->m_parent;

            const auto tmp_inst = (inner_node_type *)_Alty_traits::allocate(
                __get_allocator(), sizeof(inner_node_type));

            inner->m_size = (int)(ceil_half);
            tmp_inst->m_size = (int)(floor_half);

            InlineKey next_key;

            if (pos <= ceil_half) {
                next_key = keys[ceil_half - 1];

                Traits::copy(keys + ceil_half, keys + node_size, tmp_inst->m_keys);
                Traits::copy(sons + ceil_half, sons + node_size + 1, tmp_inst->m_sons);
                Traits::copy_backward(keys + pos - 1, keys + ceil_half - 1,
                                      keys + ceil_half);
                Traits::copy_backward(sons + pos, sons + ceil_half, sons + ceil_half + 1);

                keys[pos - 1] = key;
                sons[pos] = inst;

                inst->m_pos = pos;
                for (unsigned int i = pos + 1; i <= ceil_half; ++i) {
                    sons[i]->m_pos = i;
                }
            } else {
                if (pos == ceil_half + 1) {
                    next_key = key;

                    Traits::copy(keys + ceil_half, keys + node_size, tmp_inst->m_keys);
                    Traits::copy(sons + ceil_half + 1, sons + node_size + 1,
                                 tmp_inst->m_sons + 1);

                    tmp_inst->m_sons[0] = inst;
                } else {
                    next_key = keys[ceil_half];

                    Traits::copy(keys + ceil_half + 1, keys + pos - 1, tmp_inst->m_keys);
                    Traits::copy(sons + ceil_half + 1, sons + pos, tmp_inst->m_sons);

                    const unsigned int rpos = pos - ceil_half - 1;

                    Traits::copy(keys + pos - 1, keys + node_size,
                                 tmp_inst->m_keys + rpos);
                    Traits::copy(sons + pos, sons + node_size + 1,
                                 tmp_inst->m_sons + rpos + 1);

                    tmp_inst->m_keys[rpos - 1] = key;
                    tmp_inst->m_sons[rpos] = inst;
                }
            }

            for (unsigned int i = 0; i <= floor_half; ++i) {
                tmp_inst->m_sons[i]->m_parent = tmp_inst;
                tmp_inst->m_sons[i]->m_pos = i;
            }

            key = next_key;
            inst = tmp_inst;
        }

        const auto new_root = (inner_node_type *)_Alty_traits::allocate(
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
        __insert_iter(__search<false>(xval->first), xval);
    }

    template <bool Upper>
    WJR_PURE WJR_INTRINSIC_INLINE static bool
    __compare(const key_type &a, const key_type &key, const key_compare &comp) {
        if constexpr (Upper) {
            return comp(key, a);
        } else {
            return !comp(a, key);
        }
    }

    template <bool Upper>
    WJR_PURE WJR_NOINLINE const_iterator __search(const key_type &key) const {
        const node_type *current = __get_root();

        if (current == nullptr) {
            return cend();
        }

        unsigned int pos;

        int cur_size = current->m_size;
        const auto &comp = key_comp();

        // root search
        if (WJR_UNLIKELY(cur_size < 0)) {
            pos = __search<Upper, 1, node_size, 0>(current->as_leaf(), -cur_size, key,
                                                   comp);
            return const_iterator(current->as_leaf()->__get_list(), pos);
        }

        if (!__compare<Upper>(*current->as_inner()->m_keys[0], key, comp)) {
            goto NOT_LEFTMOST_AT_ROOT;
        }

        current = current->as_inner()->m_sons[0];
        cur_size = current->m_size;

        while (cur_size >= 0) {
            if (!__compare<Upper>(*current->as_inner()->m_keys[0], key, comp)) {
                goto NOT_LEFTMOST_AT_INNER;
            }

            current = current->as_inner()->m_sons[0];
            cur_size = current->m_size;
        }

        // leftmost leaf need to check first key
        if (__compare<Upper>(current->as_leaf()->__get_key(0), key, comp)) {
            return const_iterator(current->as_leaf()->__get_list(), 0);
        }

        goto LEAF;

    NOT_LEFTMOST_AT_ROOT:
        pos = __search<Upper, 1, node_size, 1>(current->as_inner(), cur_size, key, comp);
        current = current->as_inner()->m_sons[pos];
        cur_size = current->m_size;

        if (cur_size < 0) {
            goto LEAF;
        }

        goto INNER_LOOP;

    NOT_LEFTMOST_AT_INNER:
        pos = __search<Upper, floor_half, node_size, 1>(current->as_inner(), cur_size,
                                                        key, comp);

        current = current->as_inner()->m_sons[pos];
        cur_size = current->m_size;

        if (cur_size < 0) {
            goto LEAF;
        }

    INNER_LOOP:
        do {
            pos = __search<Upper, floor_half, node_size, 0>(current->as_inner(), cur_size,
                                                            key, comp);

            current = current->as_inner()->m_sons[pos];
            cur_size = current->m_size;
        } while (cur_size >= 0);

    LEAF:
        pos = __search<Upper, floor_half, node_size, 1>(current->as_leaf(), -cur_size,
                                                        key, comp);
        return const_iterator(current->as_leaf()->__get_list(), pos);
    }

    template <size_t Min, size_t Max, size_t Offset, typename Compare>
    WJR_PURE WJR_INTRINSIC_INLINE static unsigned int
    __search(const node_type *current, unsigned int size, Compare &&comp) {
        static_assert(Offset == 0 || Offset == 1, "");
        static_assert(Min != 0, "");

        WJR_ASSERT_ASSUME(size >= Min);
        WJR_ASSERT_ASSUME(size <= Max);

        if constexpr (Min == 1 && Offset == 1) {
            if (size == 1) {
                return 1;
            }
        }

        if constexpr (Max <= 16) {
            if constexpr (Offset == 0) {
                if (comp(current, 0)) {
                    return 0;
                }
            }

#define WJR_REGISTER_BLPUS_SEARCH_2(A, B, C)                                             \
    do {                                                                                 \
        if constexpr (A == Max) {                                                        \
            return A;                                                                    \
        } else if constexpr (B == Max) {                                                 \
            if (size == A || comp(current, A)) {                                         \
                return A;                                                                \
            }                                                                            \
            return B;                                                                    \
        } else if constexpr (C <= Max) {                                                 \
            if constexpr (Min < C) {                                                     \
                if (size < C) {                                                          \
                    if constexpr (Min <= A) {                                            \
                        if (size == A || comp(current, A)) {                             \
                            return A;                                                    \
                        }                                                                \
                    } else {                                                             \
                        if (comp(current, A)) {                                          \
                            return A;                                                    \
                        }                                                                \
                    }                                                                    \
                    return B;                                                            \
                }                                                                        \
            }                                                                            \
            if (comp(current, B)) {                                                      \
                if (comp(current, A)) {                                                  \
                    return A;                                                            \
                }                                                                        \
                return B;                                                                \
            }                                                                            \
        }                                                                                \
    } while (0)
#define WJR_REGISTER_BLPUS_SEARCH_4(A, B, C, D, E)                                       \
    do {                                                                                 \
        if constexpr (E > Max) {                                                         \
            WJR_REGISTER_BLPUS_SEARCH_2(A, B, C);                                        \
            WJR_REGISTER_BLPUS_SEARCH_2(C, D, E);                                        \
        } else {                                                                         \
            if constexpr (Min < E) {                                                     \
                if (size < E) {                                                          \
                    WJR_REGISTER_BLPUS_SEARCH_2(A, B, C);                                \
                    if constexpr (Min <= C) {                                            \
                        if (size == C || comp(current, C)) {                             \
                            return C;                                                    \
                        }                                                                \
                    } else {                                                             \
                        if (comp(current, C)) {                                          \
                            return C;                                                    \
                        }                                                                \
                    }                                                                    \
                    return D;                                                            \
                }                                                                        \
            }                                                                            \
            if (comp(current, D)) {                                                      \
                if (comp(current, B)) {                                                  \
                    if (comp(current, A)) {                                              \
                        return A;                                                        \
                    }                                                                    \
                    return B;                                                            \
                }                                                                        \
                if (comp(current, C)) {                                                  \
                    return C;                                                            \
                }                                                                        \
                return D;                                                                \
            }                                                                            \
        }                                                                                \
    } while (0)

            if constexpr (Max <= 8 || Min == 1) {
                WJR_REGISTER_BLPUS_SEARCH_2(1, 2, 3);
                WJR_REGISTER_BLPUS_SEARCH_2(3, 4, 5);
                WJR_REGISTER_BLPUS_SEARCH_2(5, 6, 7);
                WJR_REGISTER_BLPUS_SEARCH_2(7, 8, 9);
                WJR_REGISTER_BLPUS_SEARCH_2(9, 10, 11);
                WJR_REGISTER_BLPUS_SEARCH_2(11, 12, 13);
                WJR_REGISTER_BLPUS_SEARCH_2(13, 14, 15);

                if constexpr (Max == 15) {
                    return 15;
                } else if constexpr (Max == 16) {
                    if (size == 15 || comp(current, 15)) {
                        return 15;
                    }

                    return 16;
                }
            } else {
                WJR_REGISTER_BLPUS_SEARCH_2(1, 2, 3);
                WJR_REGISTER_BLPUS_SEARCH_4(3, 4, 5, 6, 7);
                WJR_REGISTER_BLPUS_SEARCH_4(7, 8, 9, 10, 11);
                WJR_REGISTER_BLPUS_SEARCH_4(11, 12, 13, 14, 15);

                if constexpr (Max == 15) {
                    return 15;
                } else if constexpr (Max == 16) {
                    if (size == 15 || comp(current, 15)) {
                        return 15;
                    }

                    return 16;
                }
            }

#undef WJR_REGISTER_BLPUS_SEARCH_4
#undef WJR_REGISTER_BLPUS_SEARCH_2
        } else {
            unsigned int pos = Offset;
            do {
                if (comp(current, pos)) {
                    break;
                }
            } while (++pos != size);
            return pos;
        }
    }

    template <bool Upper, size_t Min, size_t Max, size_t Offset>
    WJR_PURE WJR_INTRINSIC_INLINE static unsigned int
    __search(const inner_node_type *current, unsigned int size, const key_type &key,
             const key_compare &comp) {
        return __search<Min, Max, Offset>(
            current, size, [&key, &comp](const node_type *current, unsigned int pos) {
                return __compare<Upper>(*current->as_inner()->m_keys[pos], key, comp);
            });
    }

    template <bool Upper, size_t Min, size_t Max, size_t Offset>
    WJR_PURE WJR_INTRINSIC_INLINE static unsigned int
    __search(const leaf_node_type *current, unsigned int size, const key_type &key,
             const key_compare &comp) {
        return __search<Min, Max, Offset>(
            current, size, [&key, &comp](const node_type *current, unsigned int pos) {
                return __compare<Upper>(current->as_leaf()->__get_key(pos), key, comp);
            });
    }

    template <typename T>
    WJR_INTRINSIC_INLINE static unsigned int
    __init_remove_rotate(const inner_node_type *parent, unsigned int pos,
                         unsigned int par_size, T *&lhs, T *&rhs) {
        unsigned int size;

        do {
            if (pos != par_size) {
                const auto tmp = static_cast<T *>(parent->m_sons[pos + 1]);
                unsigned int tmp_size;

                if constexpr (std::is_same_v<T, leaf_node_type>) {
                    tmp_size = -tmp->m_size;
                } else {
                    tmp_size = tmp->m_size;
                }

                WJR_ASSERT_ASSUME(tmp_size >= floor_half);

                rhs = tmp;
                size = tmp_size;
            } else {
                auto tmp = static_cast<T *>(parent->m_sons[pos - 1]);
                lhs = tmp;

                if constexpr (std::is_same_v<T, leaf_node_type>) {
                    return -tmp->m_size;
                } else {
                    return tmp->m_size;
                }
            }
        } while (0);

        do {
            if (pos != 0) {
                const auto tmp = static_cast<T *>(parent->m_sons[pos - 1]);
                unsigned int tmp_size;

                if constexpr (std::is_same_v<T, leaf_node_type>) {
                    tmp_size = -tmp->m_size;
                } else {
                    tmp_size = tmp->m_size;
                }

                if (tmp_size >= size) {
                    lhs = tmp;
                    size = tmp_size;
                    break;
                }
            }

            lhs = nullptr;
        } while (0);

        return size;
    }

    static InlineKey __debug(node_type *cur, bool print) {
        auto n = cur->m_size;
        if (n < 0) {
            n = -n;

            if (print) {
                printf("[");
                for (unsigned int i = 0; i < n; ++i) {
                    if (i != 0)
                        printf(",");
                    printf("%d", cur->as_leaf()->__get_key(i));
                }
                printf("]");
            }

            return cur->as_leaf()->__get_key(0);
        }

        if (print) {
            printf("{");
        }
        InlineKey ret;
        for (unsigned int i = 0; i <= n; ++i) {
            auto x = cur->as_inner()->m_sons[i];
            if (x->m_pos != i) {
                printf("error!\n");
                exit(-1);
            }

            if (x->m_parent != cur) {
                printf("error 1!\n");
                exit(-1);
            }

            auto key = __debug(x, print);
            if (i != 0) {
                if (*cur->as_inner()->m_keys[i - 1] != *key) {
                    printf("error 2!\n");
                    exit(-1);
                }
            } else {
                ret = key;
            }
        }
        if (print) {
            printf("}");
        }
        return ret;
    }

    WJR_NOINLINE void __erase_iter(const_iterator iter) {
        constexpr unsigned int merge_size = floor_half * 2;

        unsigned int pos;
        unsigned int cur_size;
        node_type *current;
        node_type *parent;
        unsigned int par_pos;
        unsigned int par_size;

        do {
            leaf_node_type *leaf = iter.get_leaf();
            pos = iter.get_pos();
            cur_size = -leaf->m_size;
            parent = leaf->m_parent;

            _Alty_traits::destroy(__get_allocator(), leaf->m_values[pos]);
            _Alty_traits::deallocate(__get_allocator(), (uint8_t *)leaf->m_values[pos],
                                     sizeof(value_type));

            if (cur_size > floor_half) {
                leaf->__copy(pos + 1, cur_size, pos, leaf);
                leaf->m_size = -(cur_size - 1);

                // first key in leaf is changed
                if (pos == 0 && parent != nullptr) {
                    current = leaf;

                    do {
                        pos = current->m_pos;
                        current = parent;
                        parent = current->m_parent;
                    } while (pos == 0 && parent != nullptr);

                    if (pos != 0) {
                        current->as_inner()->m_keys[pos - 1] = leaf->__get_key(0);
                    }
                }

                return;
            }

            if (parent == nullptr) {
                if (cur_size == 1) {
                    _Alty_traits::deallocate(__get_allocator(), (uint8_t *)leaf,
                                             sizeof(leaf_node_type));
                    __get_root() = nullptr;
                    init(&m_sentry);
                    return;
                }

                leaf->__copy(pos + 1, cur_size, pos, leaf);
                leaf->m_size = -(cur_size - 1);
                return;
            }

            WJR_ASSERT_ASSUME(cur_size == floor_half);

            const auto inner = parent->as_inner();
            par_pos = leaf->m_pos;
            par_size = inner->m_size;
            leaf_node_type *lhs;
            leaf_node_type *rhs;

            unsigned int next_size =
                __init_remove_rotate(inner, par_pos, par_size, lhs, rhs);

            do {
                if (lhs != nullptr) {
                    rhs = leaf;

                    if (next_size == floor_half) {
                        leaf->__copy(0, pos, floor_half, lhs);
                        leaf->__copy(pos + 1, floor_half, pos + floor_half, lhs);
                        break;
                    }

                    const unsigned int moved_elements = (next_size - floor_half + 1) / 2;

                    if (moved_elements != 1) {
                        leaf->__copy_backward(pos + 1, floor_half,
                                              floor_half + moved_elements - 1, leaf);
                    }

                    leaf->__copy_backward(0, pos, pos + moved_elements, leaf);
                    lhs->__copy(next_size - moved_elements, next_size, 0, leaf);

                    lhs->m_size = -(next_size - moved_elements);
                    leaf->m_size = -(floor_half + moved_elements - 1);
                } else {
                    WJR_ASSERT_ASSUME(rhs != nullptr);

                    lhs = leaf;

                    leaf->__copy(pos + 1, floor_half, pos, leaf);

                    // merge rhs to leaf, and pos of iter is zero, then
                    // need to update key in parent
                    if (pos == 0) {
                        current = leaf;

                        unsigned int tmp_pos;
                        node_type *tmp_parent = parent;

                        do {
                            tmp_pos = current->m_pos;
                            current = tmp_parent;
                            tmp_parent = current->m_parent;
                        } while (tmp_pos == 0 && tmp_parent != nullptr);

                        if (tmp_pos != 0) {
                            current->as_inner()->m_keys[tmp_pos - 1] = leaf->__get_key(0);
                        }
                    }

                    if (next_size == floor_half) {
                        rhs->__copy(0, floor_half, floor_half - 1, leaf);

                        ++par_pos;
                        break;
                    }

                    const unsigned int moved_elements = (next_size - floor_half + 1) / 2;

                    rhs->__copy(0, moved_elements, floor_half - 1, leaf);
                    rhs->__copy(moved_elements, next_size, 0, rhs);

                    rhs->m_size = -(next_size - moved_elements);
                    leaf->m_size = -(floor_half + moved_elements - 1);
                    current = rhs;
                }

                current = rhs;

                pos = current->m_pos;
                current = parent;
                parent = current->m_parent;

                current->as_inner()->m_keys[pos - 1] = rhs->__get_key(0);

                return;
            } while (0);

            lhs->m_size = -(merge_size - 1);
            rhs->remove_uninit();
            _Alty_traits::deallocate(__get_allocator(), (uint8_t *)rhs,
                                     sizeof(leaf_node_type));

        } while (0);

        pos = par_pos;
        cur_size = par_size;
        current = parent;
        parent = current->m_parent;

        while (parent != nullptr) {
            WJR_ASSERT_ASSUME(pos > 0);

            const auto inner = current->as_inner();

            InlineKey *const keys = inner->m_keys;
            node_type **const sons = inner->m_sons;

            if (cur_size > floor_half) {
                Traits::copy(keys + pos, keys + cur_size, keys + pos - 1);
                Traits::copy(sons + pos + 1, sons + cur_size + 1, sons + pos);

                for (unsigned int i = pos; i < cur_size; ++i) {
                    sons[i]->m_pos = i;
                }

                inner->m_size = cur_size - 1;
                return;
            }

            WJR_ASSERT_ASSUME(cur_size == floor_half);

            const auto par_inner = parent->as_inner();
            par_pos = inner->m_pos;
            par_size = par_inner->m_size;
            inner_node_type *lhs;
            inner_node_type *rhs;

            unsigned int next_size =
                __init_remove_rotate(par_inner, par_pos, par_size, lhs, rhs);

            do {
                if (lhs != nullptr) {
                    rhs = inner;

                    if (next_size == floor_half) {
                        Traits::copy(keys, keys + pos - 1, lhs->m_keys + floor_half + 1);
                        Traits::copy(sons, sons + pos, lhs->m_sons + floor_half + 1);
                        Traits::copy(keys + pos, keys + floor_half,
                                     lhs->m_keys + floor_half + pos);
                        Traits::copy(sons + pos + 1, sons + floor_half + 1,
                                     lhs->m_sons + floor_half + pos + 1);

                        for (unsigned int i = floor_half; i <= merge_size; ++i) {
                            lhs->m_sons[i]->m_parent = lhs;
                            lhs->m_sons[i]->m_pos = i;
                        }

                        lhs->m_keys[floor_half] = par_inner->m_keys[par_pos - 1];
                        break;
                    }

                    const unsigned int moved_elements = (next_size - floor_half + 1) / 2;

                    InlineKey key = lhs->m_keys[next_size - moved_elements];

                    if (moved_elements != 1) {
                        Traits::copy_backward(keys + pos, keys + floor_half,
                                              keys + floor_half + moved_elements - 1);
                        Traits::copy_backward(sons + pos + 1, sons + floor_half + 1,
                                              sons + floor_half + moved_elements);
                        for (unsigned int i = pos + moved_elements;
                             i < floor_half + moved_elements; ++i) {
                            sons[i]->m_pos = i;
                        }
                    }

                    Traits::copy_backward(keys, keys + pos - 1,
                                          keys + pos + moved_elements - 1);
                    Traits::copy_backward(sons, sons + pos, sons + pos + moved_elements);
                    Traits::copy(lhs->m_keys + next_size - moved_elements + 1,
                                 lhs->m_keys + next_size, keys);
                    Traits::copy(lhs->m_sons + next_size - moved_elements + 1,
                                 lhs->m_sons + next_size + 1, sons);

                    keys[moved_elements - 1] = par_inner->m_keys[par_pos - 1];
                    par_inner->m_keys[par_pos - 1] = key;

                    for (unsigned int i = 0; i < moved_elements; ++i) {
                        sons[i]->m_parent = inner;
                        sons[i]->m_pos = i;
                    }

                    for (unsigned int i = moved_elements; i < pos + moved_elements; ++i) {
                        sons[i]->m_pos = i;
                    }

                    lhs->m_size = next_size - moved_elements;
                    inner->m_size = floor_half + moved_elements - 1;
                } else {
                    WJR_ASSERT_ASSUME(rhs != nullptr);

                    lhs = inner;

                    if (next_size == floor_half) {
                        Traits::copy(keys + pos, keys + floor_half, keys + pos - 1);
                        Traits::copy(sons + pos + 1, sons + floor_half + 1, sons + pos);
                        Traits::copy(rhs->m_keys, rhs->m_keys + floor_half,
                                     keys + floor_half);
                        Traits::copy(rhs->m_sons, rhs->m_sons + floor_half + 1,
                                     sons + floor_half);

                        for (unsigned int i = pos; i < floor_half; ++i) {
                            inner->m_sons[i]->m_pos = i;
                        }

                        for (unsigned int i = floor_half; i <= merge_size; ++i) {
                            inner->m_sons[i]->m_parent = inner;
                            inner->m_sons[i]->m_pos = i;
                        }

                        lhs->m_keys[floor_half - 1] = par_inner->m_keys[par_pos];
                        ++par_pos;
                        break;
                    }

                    const unsigned int moved_elements = (next_size - floor_half + 1) / 2;

                    InlineKey key = rhs->m_keys[moved_elements - 1];

                    Traits::copy(keys + pos, keys + floor_half, keys + pos - 1);
                    Traits::copy(sons + pos + 1, sons + floor_half + 1, sons + pos);
                    Traits::copy(rhs->m_keys, rhs->m_keys + moved_elements - 1,
                                 keys + floor_half);
                    Traits::copy(rhs->m_sons, rhs->m_sons + moved_elements,
                                 sons + floor_half);
                    Traits::copy(rhs->m_keys + moved_elements, rhs->m_keys + next_size,
                                 rhs->m_keys);
                    Traits::copy(rhs->m_sons + moved_elements,
                                 rhs->m_sons + next_size + 1, rhs->m_sons);

                    keys[floor_half - 1] = par_inner->m_keys[par_pos];
                    par_inner->m_keys[par_pos] = key;

                    for (unsigned int i = pos; i < floor_half; ++i) {
                        sons[i]->m_pos = i;
                    }

                    for (unsigned int i = floor_half; i < floor_half + moved_elements;
                         ++i) {
                        sons[i]->m_parent = inner;
                        sons[i]->m_pos = i;
                    }

                    for (unsigned int i = 0; i <= next_size - moved_elements; ++i) {
                        rhs->m_sons[i]->m_pos = i;
                    }

                    rhs->m_size = next_size - moved_elements;
                    inner->m_size = floor_half + moved_elements - 1;
                }

                return;
            } while (0);

            lhs->m_size = merge_size;
            _Alty_traits::deallocate(__get_allocator(), (uint8_t *)rhs,
                                     sizeof(inner_node_type));

            pos = par_pos;
            cur_size = par_size;
            current = parent;
            parent = current->m_parent;
        }

        const auto inner = current->as_inner();

        if (cur_size == 1) {
            _Alty_traits::deallocate(__get_allocator(), (uint8_t *)inner,
                                     sizeof(inner_node_type));
            node_type *root = inner->m_sons[0];
            __get_root() = root;
            root->m_parent = nullptr;
            return;
        }

        Traits::copy(inner->m_keys + pos, inner->m_keys + cur_size,
                     inner->m_keys + pos - 1);
        Traits::copy(inner->m_sons + pos + 1, inner->m_sons + cur_size + 1,
                     inner->m_sons + pos);

        for (unsigned int i = pos; i < cur_size; ++i) {
            inner->m_sons[i]->m_pos = i;
        }

        inner->m_size = cur_size - 1;
    }

    WJR_INTRINSIC_CONSTEXPR node_type *&__get_root() noexcept {
        return m_pair.second().second();
    }

    WJR_INTRINSIC_CONSTEXPR const node_type *__get_root() const noexcept {
        return m_pair.second().second();
    }

    compressed_pair<key_compare, compressed_pair<_Alty, node_type *>> m_pair;
    ListNode m_sentry;
}; // namespace wjr

} // namespace wjr

#endif // WJR_CONTAINER_GENERIC_BPLUS_TREE_HPP__