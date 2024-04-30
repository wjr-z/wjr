#ifndef WJR_CONTAINER_GENERIC_BPLUS_TREE_HPP__
#define WJR_CONTAINER_GENERIC_BPLUS_TREE_HPP__

#include <wjr/compressed_pair.hpp>
#include <wjr/memory/uninitialized.hpp>

namespace wjr {

template <typename Key, bool = true>
struct bplus_key {
    bplus_key() noexcept = default;
    bplus_key(const bplus_key &) noexcept = default;
    bplus_key(bplus_key &&) noexcept = default;
    bplus_key &operator=(const bplus_key &) noexcept = default;
    bplus_key &operator=(bplus_key &&) noexcept = default;
    ~bplus_key() noexcept = default;

    bplus_key(Key key) noexcept : m_key(key) {}

    bplus_key &operator=(Key key) noexcept {
        m_key = key;
        return *this;
    }

    Key operator*() const noexcept { return m_key; }

    Key m_key;
};

template <typename Key>
struct bplus_key<Key, false> {
    bplus_key() noexcept = default;
    bplus_key(const bplus_key &) noexcept = default;
    bplus_key(bplus_key &&) noexcept = default;
    bplus_key &operator=(const bplus_key &) noexcept = default;
    bplus_key &operator=(bplus_key &&) noexcept = default;
    ~bplus_key() noexcept = default;

    bplus_key(const Key &key) noexcept : m_key(std::addressof(key)) {}

    bplus_key &operator=(const Key &key) noexcept {
        m_key = std::addressof(key);
        return *this;
    }

    const Key &operator*() const noexcept { return *m_key; }

    const Key *m_key;
};

template <typename Key, typename T, typename Compare, size_t Size>
struct bplus_tree_traits {
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<const key_type, mapped_type>;
    using key_compare = Compare;

    constexpr static size_t node_size = Size;

    using KeyPtr = bplus_key<Key, std::is_trivial_v<Key> &&
                                      std::is_standard_layout_v<Key> && sizeof(Key) <= 8>;
    static constexpr size_t stack_size = node_size == 3   ? 48
                                         : node_size < 7  ? 32
                                         : node_size < 15 ? 24
                                                          : 16;

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

    bplus_tree_node *&next() noexcept { return m_sons[node_size]; }

    int32_t m_size;
    uint32_t m_pos;
    bplus_tree_node *m_parent;
    alignas(16) KeyPtr m_keys[node_size];
    union alignas(16) {
        bplus_tree_node *m_sons[node_size + 1];
        value_type *m_values[node_size + 1];
    };
};

template <typename Traits>
class bplus_tree_const_iterator {
    using node_type = bplus_tree_node<Traits>;

public:
    using value_type = typename Traits::value_type;

    bplus_tree_const_iterator() noexcept = default;
    bplus_tree_const_iterator(const bplus_tree_const_iterator &) noexcept = default;
    bplus_tree_const_iterator(bplus_tree_const_iterator &&) noexcept = default;
    bplus_tree_const_iterator &
    operator=(const bplus_tree_const_iterator &) noexcept = default;
    bplus_tree_const_iterator &operator=(bplus_tree_const_iterator &&) noexcept = default;
    ~bplus_tree_const_iterator() noexcept = default;

    bplus_tree_const_iterator(const node_type *node, uint8_t pos) noexcept
        : m_node(const_cast<node_type *>(node)), m_pos(pos) {}

    const value_type &operator*() const noexcept { return *m_node->m_values[m_pos]; }

    const value_type *operator->() const noexcept { return m_node->m_values[m_pos]; }

    bplus_tree_const_iterator &operator++() noexcept {
        ++m_pos;

        if (m_pos == (uint8_t)(-m_node->m_size)) {
            m_node = m_node->next();
            m_pos = 0;
        }

        return *this;
    }

    bplus_tree_const_iterator operator++(int) noexcept {
        bplus_tree_const_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    bool operator==(const bplus_tree_const_iterator &other) const noexcept {
        return m_node == other.m_node && m_pos == other.m_pos;
    }

    bool operator!=(const bplus_tree_const_iterator &other) const noexcept {
        return !(*this == other);
    }

    node_type *get_node() const noexcept { return m_node; }
    uint8_t get_pos() const noexcept { return m_pos; }

private:
    node_type *m_node = nullptr;
    uint8_t m_pos = 0;
};

template <typename Traits>
class bplus_tree_iterator : public bplus_tree_const_iterator<Traits> {
    using Mybase = bplus_tree_const_iterator<Traits>;

public:
    using Mybase::Mybase;
    using value_type = typename Mybase::value_type;

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

    bool operator==(const bplus_tree_iterator &other) const noexcept {
        return Mybase::operator==(other);
    }

    bool operator!=(const bplus_tree_iterator &other) const noexcept {
        return Mybase::operator!=(other);
    }
};

template <typename Traits, typename Alloc>
class basic_bplus_tree {
    using node_type = bplus_tree_node<Traits>;
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<uint8_t>;
    using _Alty_traits = std::allocator_traits<_Alty>;

    using mapped_type = typename Traits::mapped_type;
    static constexpr size_t node_size = Traits::node_size;
    static constexpr size_t stack_size = Traits::stack_size;
    using KeyPtr = typename Traits::KeyPtr;

public:
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;
    using key_compare = typename Traits::key_compare;
    using allocator_type = Alloc;
    using size_type = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;
    using const_iterator = bplus_tree_const_iterator<Traits>;
    using iterator = bplus_tree_iterator<Traits>;

    static_assert(node_size >= 3, "node_size must be greater than or equal to 3.");
    static_assert(node_size <= 255, "node_size must be less than or equal to 255.");

    basic_bplus_tree() noexcept(std::is_nothrow_default_constructible_v<_Alty>) = default;

    ~basic_bplus_tree() {
        node_type *current = __get_root();
        if (current == nullptr) {
            return;
        }

        auto &al = __get_allocator();
        int32_t cur_size = current->m_size;

        if (cur_size < 0) {
            cur_size = -cur_size;

            for (uint8_t i = 0; i < cur_size; ++i) {
                _Alty_traits::destroy(al, current->m_values[i]);
                _Alty_traits::deallocate(al, (uint8_t *)current->m_values[i],
                                         sizeof(value_type));
            }

            _Alty_traits::deallocate(al, (uint8_t *)current, sizeof(node_type));
            return;
        }

        while (cur_size >= 0) {
            current = current->m_sons[0];
            cur_size = current->m_size;
        }

        cur_size = -cur_size;

        do {
            for (uint8_t i = 0; i < cur_size; ++i) {
                _Alty_traits::destroy(al, current->m_values[i]);
                _Alty_traits::deallocate(al, (uint8_t *)current->m_values[i],
                                         sizeof(value_type));
            }

            node_type *next = current->next();
            node_type *parent = current->m_parent;
            uint8_t pos = current->m_pos;
            _Alty_traits::deallocate(al, (uint8_t *)current, sizeof(node_type));

            while (pos == parent->m_size) {
                current = parent;
                parent = current->m_parent;
                pos = current->m_pos;
                _Alty_traits::deallocate(al, (uint8_t *)current, sizeof(node_type));

                if (parent == nullptr) {
                    return;
                }
            }

            current = next;
            WJR_ASSUME(current != nullptr);
            cur_size = -current->m_size;
        } while (true);
    }

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

    constexpr const key_compare &key_comp() const noexcept { return m_pair.first(); }

    iterator begin() noexcept { return iterator(m_left, 0); }
    const_iterator begin() const noexcept { return const_iterator(m_left, 0); }
    const_iterator cbegin() const noexcept { return const_iterator(m_left, 0); }

    iterator end() noexcept { return iterator(nullptr, 0); }
    const_iterator end() const noexcept { return const_iterator(nullptr, 0); }
    const_iterator cend() const noexcept { return const_iterator(nullptr, 0); }

private:
    void __insert_root(value_type *xval) {
        node_type *root =
            (node_type *)_Alty_traits::allocate(__get_allocator(), sizeof(node_type));
        __get_root() = root;
        m_left = root;

        root->m_size = -1;
        root->m_parent = nullptr;
        root->m_keys[0] = xval->first;
        root->m_values[0] = xval;
        root->next() = nullptr;
        return;
    }

    WJR_NOINLINE void __insert_hint(const_iterator hint, value_type *xval) {
        node_type *current = hint.get_node();

        if (WJR_UNLIKELY(current == nullptr)) {
            __insert_root(xval);
            return;
        }

        uint8_t pos = hint.get_pos();

        KeyPtr key = xval->first;
        int32_t cur_size = -current->m_size;

        auto keys = current->m_keys;
        const auto values = current->m_values;

        if (WJR_LIKELY(cur_size != node_size)) {
            WJR_ASSUME(pos <= cur_size);

            Traits::copy_backward(keys + pos, keys + cur_size, keys + cur_size + 1);
            Traits::copy_backward(values + pos, values + cur_size, values + cur_size + 1);

            current->m_size = -(cur_size + 1);
            keys[pos] = xval->first;
            values[pos] = xval;
            return;
        }

        node_type *inst =
            (node_type *)_Alty_traits::allocate(__get_allocator(), sizeof(node_type));
        inst->next() = current->next();
        current->next() = inst;

        constexpr size_t __half = node_size / 2;

        current->m_size = -(int32_t)(__half + 1);
        inst->m_size = -(int32_t)(node_size - __half);

        if (pos <= __half) {
            Traits::copy(keys + __half, keys + node_size, inst->m_keys);
            Traits::copy(values + __half, values + node_size, inst->m_values);
            Traits::copy_backward(keys + pos, keys + __half, keys + __half + 1);
            Traits::copy_backward(values + pos, values + __half, values + __half + 1);

            keys[pos] = xval->first;
            values[pos] = xval;
        } else {
            Traits::copy(keys + __half + 1, keys + pos, inst->m_keys);
            Traits::copy(values + __half + 1, values + pos, inst->m_values);

            const uint8_t rpos = pos - __half - 1;

            Traits::copy(keys + pos, keys + node_size, inst->m_keys + rpos + 1);
            Traits::copy(values + pos, values + node_size, inst->m_values + rpos + 1);

            inst->m_keys[rpos] = xval->first;
            inst->m_values[rpos] = xval;
        }

        node_type *parent = current->m_parent;

        key = inst->m_keys[0];

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

            parent = current->m_parent;

            node_type *pinst =
                (node_type *)_Alty_traits::allocate(__get_allocator(), sizeof(node_type));

            current->m_size = (int32_t)(__half);
            const uint32_t rsize = node_size - __half - 1;
            pinst->m_size = (int32_t)(rsize);

            KeyPtr next_key;

            if (pos <= __half) {
                next_key = keys[__half - 1];

                Traits::copy(keys + __half, keys + node_size - 1, pinst->m_keys);
                Traits::copy(sons + __half, sons + node_size, pinst->m_sons);
                Traits::copy_backward(keys + pos - 1, keys + __half - 1, keys + __half);
                Traits::copy_backward(sons + pos, sons + __half, sons + __half + 1);

                keys[pos - 1] = key;
                sons[pos] = inst;

                inst->m_pos = pos;
                for (uint8_t i = pos + 1; i <= __half; ++i) {
                    sons[i]->m_pos = i;
                }
            } else {
                if (pos == __half + 1) {
                    next_key = key;
                    Traits::copy(keys + __half, keys + node_size - 1, pinst->m_keys);
                    Traits::copy(sons + __half + 1, sons + node_size, pinst->m_sons + 1);

                    pinst->m_sons[0] = inst;
                } else {
                    next_key = keys[__half];
                    Traits::copy(keys + __half + 1, keys + pos - 1, pinst->m_keys);
                    Traits::copy(sons + __half + 1, sons + pos, pinst->m_sons);

                    const uint8_t rpos = pos - __half - 2;

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

        node_type *new_root =
            (node_type *)_Alty_traits::allocate(__get_allocator(), sizeof(node_type));
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
        __insert_hint(__upper_bound(xval->first), xval);
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
            if (!comp(key, *current->m_keys[0])) {
                goto NOT_LEFT;
            }

            current = current->m_sons[0];
            cur_size = current->m_size;
        }

        cur_size = -cur_size;

        if (comp(key, *current->m_keys[0])) {
            return const_iterator(current, 0);
        }

        if (cur_size == 1) {
            return const_iterator(current, 1);
        }

        goto LEAF;

    NOT_LEFT:
        if (cur_size == 1) {
            pos = 1;
        } else {
            pos = 1 + __upper_search(current->m_keys + 1, cur_size - 1, key, comp);
        }

        current = current->m_sons[pos];
        cur_size = current->m_size;

        while (cur_size >= 0) {
            pos = __upper_search(current->m_keys, cur_size, key, comp);

            current = current->m_sons[pos];
            cur_size = current->m_size;
        }

        cur_size = -cur_size;

    LEAF:
        // not left node, don't need to compare with the first key
        pos = 1 + __upper_search(current->m_keys + 1, cur_size - 1, key, comp);

        return const_iterator(current, pos);
    }

    WJR_PURE WJR_INTRINSIC_INLINE static uint8_t __upper_search(const KeyPtr *keys,
                                                                uint8_t size,
                                                                const key_type &key,
                                                                const key_compare &comp) {
        uint8_t pos = 0;
        do {
            if (comp(key, *keys[pos])) {
                break;
            }
        } while (++pos != size);
        return pos;
    }

    WJR_PURE constexpr _Alty &__get_allocator() noexcept {
        return m_pair.second().first();
    }
    WJR_PURE constexpr const _Alty &__get_allocator() const noexcept {
        return m_pair.second().first();
    }

    constexpr node_type *&__get_root() noexcept { return m_pair.second().second(); }
    constexpr const node_type *__get_root() const noexcept {
        return m_pair.second().second();
    }

    compressed_pair<key_compare, compressed_pair<_Alty, node_type *>> m_pair;
    node_type *m_left = nullptr;
};

} // namespace wjr

#endif // WJR_CONTAINER_GENERIC_BPLUS_TREE_HPP__