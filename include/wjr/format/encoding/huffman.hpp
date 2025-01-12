/**
 * @file huffman.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef WJR_FORMAT_ENCODING_HUFFMAN_HPP__
#define WJR_FORMAT_ENCODING_HUFFMAN_HPP__

#include <algorithm>

#include <wjr/compressed_pair.hpp>
#include <wjr/vector.hpp>

namespace wjr {

template <typename T>
class huffman_node {
public:
    huffman_node() = default;
    huffman_node(const huffman_node &) = delete;
    huffman_node &operator=(const huffman_node &) = delete;

    huffman_node *left() noexcept { return m_left; }
    const huffman_node *left() const noexcept { return m_left; }
    huffman_node *right() noexcept { return m_right; }
    const huffman_node *right() const noexcept { return m_right; }

    void set_left(huffman_node *left) noexcept { m_left = left; }
    void set_right(huffman_node *right) noexcept { m_right = right; }

    bool is_leaf() const noexcept { return m_left == nullptr; }
    bool is_inner() const noexcept { return m_left != nullptr; }

private:
    huffman_node *m_left = nullptr;
    huffman_node *m_right = nullptr;
};

template <typename T>
class huffman_leaf_node : public huffman_node<T> {
public:
    huffman_leaf_node() noexcept : huffman_node<T>(), m_value() {}

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<T, Args &&...>)>
    huffman_leaf_node(Args &&...args) noexcept(std::is_nothrow_constructible_v<T, Args &&...>)
        : huffman_node<T>(), m_value(std::forward<Args>(args)...) {}

    huffman_leaf_node(const huffman_leaf_node &) = delete;
    huffman_leaf_node &operator=(const huffman_leaf_node &) = delete;

    T *operator->() noexcept { return std::addressof(m_value); }
    const T *operator->() const noexcept { return std::addressof(m_value); }

    T &operator*() noexcept { return m_value; }
    const T &operator*() const noexcept { return m_value; }

private:
    T m_value;
};

template <typename T>
constexpr huffman_leaf_node<T> *get_leaf(huffman_node<T> *node) {
    WJR_ASSERT(node->is_leaf());
    return static_cast<huffman_leaf_node<T> *>(node);
}

/// @todo Allocator.
template <typename T>
class huffman_tree {
public:
    using node_type = huffman_node<T>;

    huffman_tree(size_t n) : m_buffer(new char[n]) {}

    huffman_tree(const huffman_tree &) = delete;
    huffman_tree(huffman_tree &&) = default;
    huffman_tree &operator=(const huffman_tree &) = delete;
    huffman_tree &operator=(huffman_tree &&) = default;

    node_type *root() noexcept { return reinterpret_cast<node_type *>(m_buffer.get()); }
    const node_type *root() const noexcept { return reinterpret_cast<node_type *>(m_buffer.get()); }

private:
    std::unique_ptr<char[]> m_buffer;
};

namespace huffman {

template <typename T>
struct __trivial_pair {
    size_t first;
    T second;
};

/// @brief Iter doesn't necessarily need to be an iterator.
template <typename Getter>
decltype(auto) build_tree(size_t n, Getter getter) noexcept {
    using value_type = typename Getter::value_type;
    static_assert(std::is_trivial_v<value_type>);
    using tree_type = huffman_tree<value_type>;
    using pair_type = __trivial_pair<value_type>;
    using inner_type = huffman_node<value_type>;
    using leaf_type = huffman_leaf_node<value_type>;

    std::unique_ptr<char[]> buffer(new char[sizeof(size_t) * (n - 1) + sizeof(pair_type) * n]);

    auto *size_first = reinterpret_cast<size_t *>(buffer.get()) + (n - 1);
    auto *size_last = size_first;
    auto *pair_first = reinterpret_cast<pair_type *>(size_first);

    for (size_t i = 0; i < n; ++i) {
        pair_first[i].first = getter.get_size(i);
        pair_first[i].second = getter.get_value(i);
    }

    std::sort(pair_first, pair_first + n,
              [](const auto &lhs, const auto &rhs) { return lhs.first < rhs.first; });

    huffman_tree<value_type> tree((n - 1) * sizeof(inner_type) + n * sizeof(leaf_type));
    inner_type *const inner_end = tree.root();
    auto *inner_first = inner_end + (n - 1);
    auto *inner_last = inner_first;
    auto *leaf_first = reinterpret_cast<leaf_type *>(inner_first);
    auto *const leaf_last = leaf_first + n;

    for (size_t i = 0; i < n; ++i) {
        wjr::construct_at(leaf_first + i, pair_first[i].second);
    }

    if (n != 1) {
        do {
            size_t &total_size = size_last[-1];
            inner_type &new_node = inner_last[-1];
            do {
                if (inner_first == inner_last) {
                    total_size = pair_first[0].first + pair_first[1].first;
                    new_node.set_left(leaf_first);
                    new_node.set_left(leaf_first + 1);
                    leaf_first += 2;
                    pair_first += 2;
                    break;
                }

                size_t pair_size = pair_first->first;
                do {
                    if (size_first[-1] < pair_size) {
                        total_size = size_first[-1];
                        --inner_first;
                        --size_first;
                        new_node.set_left(inner_first);

                        if (inner_first != inner_last && size_first[-1] < pair_size) {
                            total_size += size_first[-1];
                            --inner_first;
                            --size_first;
                            new_node.set_right(inner_first);

                            WJR_ASSUME(leaf_first != leaf_last);
                            break;
                        }

                        total_size += pair_size;
                        new_node.set_right(leaf_first);
                        ++leaf_first;
                        ++pair_first;
                        break;
                    }

                    total_size = pair_size;
                    new_node.set_left(leaf_first);
                    ++leaf_first;
                    ++pair_first;

                    if (leaf_first == leaf_last ||
                        (pair_size = pair_first->first, size_first[-1] < pair_size)) {
                        total_size += size_first[-1];
                        --inner_first;
                        --size_first;
                        new_node.set_right(inner_first);
                        break;
                    }

                    total_size += pair_size;
                    new_node.set_right(leaf_first);
                    ++leaf_first;
                    ++pair_first;
                } while (false);
            } while (false);

            --inner_last;
            --size_last;
        } while (leaf_first != leaf_last);

        if (inner_last != inner_end) {
            do {
                inner_type &new_node = inner_last[-1];
                size_t &total_size = size_last[-1];

                total_size = size_first[-1] + size_first[-2];
                new_node.set_left(inner_first - 1);
                new_node.set_right(inner_first - 2);
                inner_first -= 2;
                size_first -= 2;

                --inner_last;
                --size_last;
            } while (inner_last != inner_end);
        }
    }

    std::destroy_n(pair_first, n);
    return tree;
}
} // namespace huffman

} // namespace wjr

#endif // WJR_FORMAT_ENCODING_HUFFMAN_HPP__