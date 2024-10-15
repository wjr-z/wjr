#ifndef WJR_FORMAT_ENCODING_HUFFMAN_HPP__
#define WJR_FORMAT_ENCODING_HUFFMAN_HPP__

#include <algorithm>

#include <wjr/compressed_pair.hpp>
#include <wjr/vector.hpp>

namespace wjr {

class huffman_node {
public:
    huffman_node() = default;
    huffman_node(const huffman_node &) = delete;
    huffman_node &operator=(const huffman_node &) = delete;

    void set_left(huffman_node *left) noexcept { m_left = left; }
    void set_right(huffman_node *right) noexcept { m_right = right; }
    huffman_node *get_left() noexcept { return m_left; }
    const huffman_node *get_left() const noexcept { return m_left; }
    huffman_node *get_right() noexcept { return m_right; }

    bool is_leaf() const noexcept { return m_left == nullptr; }
    bool is_inner() const noexcept { return m_left != nullptr; }

private:
    huffman_node *m_left = nullptr;
    huffman_node *m_right = nullptr;
};

template <typename T>
class huffman_leaf_node : public huffman_node {
public:
    huffman_leaf_node() noexcept : huffman_node(), m_value() {}

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<T, Args &&...>)>
    huffman_leaf_node(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args &&...>)
        : m_value(std::forward<Args>(args)...) {}

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
constexpr huffman_leaf_node<T> *get_leaf(huffman_node *node) {
    WJR_ASSERT(node->is_leaf());
    return static_cast<huffman_leaf_node<T> *>(node);
}

/// @todo Allocator.
template <typename T, typename Alloc>
class huffman_tree {
public:
    using vector_type = fixed_vector<T, Alloc>;
    using size_type = typename vector_type::size_type;

private:
    fixed_vector<T, Alloc> m_vec;
};

template <typename Getter>
struct huffman_encode_node {
    using size_type = typename Getter::size_type;
    using value_type = typename Getter::value_type;

    value_type m_value;
};

class huffman_encoder {
public:
    /// @brief Iter doesn't necessarily need to be an iterator.
    template <typename Iter, typename Getter>
    static decltype(auto) encode_tree(Iter first, Iter last, Getter getter) noexcept {
        using size_type = typename Getter::size_type;
        using value_type = typename Getter::value_type;
        static constexpr size_type npos = std::numeric_limits<size_type>::max();

        const auto n = last - first;
        fixed_vector<compressed_pair<size_type, value_type>> tmp_vec(n, in_place_reserve);
        for (; first != last; ++first) {
            tmp_vec.emplace_back(getter.get_size(first), getter.get_value(first));
        }

        std::sort(tmp_vec.begin(), tmp_vec.end(), [](const auto &lhs, const auto &rhs) {
            return lhs.first() < rhs.first();
        });
    }
};

} // namespace wjr

#endif // WJR_FORMAT_ENCODING_HUFFMAN_HPP__