#ifndef WJR_CONTAINER_GENERIC_DYNAMIC_BITSET_HPP__
#define WJR_CONTAINER_GENERIC_DYNAMIC_BITSET_HPP__

#include <wjr/math.hpp>
#include <wjr/span.hpp>
#include <wjr/vector.hpp>

namespace wjr {

/**
 * @todo Dynamic bitset.
 *
 */

/**
 * @brief Dynamic bitset.
 *
 * @note TODO: Add more functions.
 */
template <typename Allocator = std::allocator<uint64_t>>
class basic_dynamic_bitset {
public:
    using block_type = uint64_t;
    using allocator_type = Allocator;
    constexpr static size_t block_size = 64;

private:
    using vector_type = vector<block_type, allocator_type>;

public:
    using size_type = typename vector_type::size_type;

    class reference {
        friend class basic_dynamic_bitset;

    public:
        reference(block_type *val, size_type pos)
            : m_val(val), m_mask((block_type)1 << pos) {}

        ~reference() = default;

        reference &operator=(bool val) {
            if (val) {
                *m_val |= m_mask;
            } else {
                *m_val &= ~m_mask;
            }
            return *this;
        }

        bool operator~() const { return (*m_val & m_mask) == 0; }
        operator bool() const { return (*m_val & m_mask) != 0; }

        reference &operator=(const reference &other) { return *this = bool(other); }

        reference &flip() {
            *m_val ^= m_mask;
            return *this;
        }

    private:
        block_type *m_val;
        block_type m_mask;
    };

    using const_reference = bool;

    basic_dynamic_bitset() = default;

    /**
     * @brief Use from_chars_2 to optimize the performance.
     *
     * @details Very fast conversion from string to uint64_t.
     *
     * @note Only use from_chars_2 when type of T is uint64_t and CharT is char.
     */
    template <typename CharT, size_t Extent,
              std::enable_if_t<std::is_same_v<std::remove_const_t<CharT>, char>, int> = 0>
    explicit basic_dynamic_bitset(const span<CharT, Extent> &sp)
        : m_vec((sp.size() + block_size - 1) / block_size, dctor),
          m_bits(sp.size()) {
        (void)__biginteger_from_chars_2_impl((const uint8_t *)sp.data(), sp.size(),
                                             m_vec.data());
    }

    template <
        typename CharT, size_t Extent,
        std::enable_if_t<!std::is_same_v<std::remove_const_t<CharT>, char>, int> = 0>
    explicit basic_dynamic_bitset(const span<CharT, Extent> &sp)
        : basic_dynamic_bitset(sp, '0') {}

    template <typename CharT, size_t Extent>
    basic_dynamic_bitset(const span<CharT, Extent> &sp, type_identity_t<CharT> zero,
                         type_identity_t<CharT> one = '1')
        : basic_dynamic_bitset(sp, zero, one, std::equal_to<CharT>()) {}

    template <typename CharT, size_t Extent, typename Equal>
    basic_dynamic_bitset(const span<CharT, Extent> &sp,
                         WJR_MAYBE_UNUSED type_identity_t<CharT> zero,
                         type_identity_t<CharT> one, Equal equal)
        : m_vec((sp.size() + block_size - 1) / block_size, dctor),
          m_bits(sp.size()) {
        auto ptr = sp.data();
        size_type n = sp.size();
        size_type idx = 0;

        block_type value = 0;

        while (n >= block_size) {
            value = 0;

            for (size_type i = block_size - 1; ~i; --i) {
                value <<= 1;
                if (equal(ptr[i], one)) {
                    value |= 1;
                } else {
                    WJR_ASSERT(equal(ptr[i], zero));
                }
            }

            m_vec[idx] = value;
            ptr += block_size;
            n -= block_size;
            ++idx;
        }

        if (n) {
            value = 0;

            for (size_type i = n - 1; ~i; --i) {
                value <<= 1;
                if (equal(ptr[i], one)) {
                    value |= 1;
                } else {
                    WJR_ASSERT(equal(ptr[i], zero));
                }
            }

            m_vec[idx] = value;
        }
    }

    basic_dynamic_bitset(const basic_dynamic_bitset &other) = default;
    basic_dynamic_bitset(basic_dynamic_bitset &&other)
        : m_vec(std::move(other.m_vec)), m_bits(std::exchange(other.m_bits, 0)) {}
    basic_dynamic_bitset &operator=(const basic_dynamic_bitset &other) = default;
    basic_dynamic_bitset &operator=(basic_dynamic_bitset &&other) {
        m_vec = std::move(other.m_vec);
        m_bits = std::exchange(other.m_bits, 0);
        return *this;
    }
    ~basic_dynamic_bitset() = default;

    void swap(basic_dynamic_bitset &other) {
        m_vec.swap(other.m_vec);
        std::swap(m_bits, other.m_bits);
    }

    bool operator==(const basic_dynamic_bitset &other) const {
        return m_vec == other.m_vec;
    }

    bool operator!=(const basic_dynamic_bitset &other) const {
        return m_vec != other.m_vec;
    }

    reference operator[](size_type pos) {
        WJR_ASSERT(pos < m_bits, "Index out of range.");
        return reference(&m_vec[pos / block_size], pos % block_size);
    }

    const_reference operator[](size_type pos) const {
        WJR_ASSERT(pos < m_bits, "Index out of range.");
        return (m_vec[pos / block_size] >> (pos % block_size)) & 1;
    }

    bool test(size_type pos) const { return (*this)[pos]; }

    bool all() const {
        size_type n = m_vec.size();
        if (WJR_UNLIKELY(n == 0)) {
            return false;
        }

        if (m_vec.back() != ((block_type)1 << (m_bits % block_size)) - 1) {
            return false;
        }

        return find_not_n(m_vec.data(), ~(block_type)0, n - 1) == n - 1;
    }

    bool any() const {
        size_type n = m_vec.size();
        return find_n(m_vec.data(), (block_type)0, n) != n;
    }

    bool none() const { return !any(); }

    size_type count() const {
        size_type sum = 0;
        for (auto item : m_vec) {
            sum += popcount(item);
        }
        return sum;
    }

    size_type size() const { return m_bits; }

    allocator_type &get_allocator() noexcept { return m_vec.get_allocator(); }
    const allocator_type &get_allocator() const noexcept { return m_vec.get_allocator(); }

    basic_dynamic_bitset &operator&=(const basic_dynamic_bitset &other) {
        if (other.m_bits < m_bits) {
            m_bits = other.m_bits;
            m_vec.truncate(other.m_vec.size());
        }

        size_type n = m_vec.size();
        for (size_type i = 0; i < n; ++i) {
            m_vec[i] &= other.m_vec[i];
        }

        return *this;
    }

    basic_dynamic_bitset &operator|=(const basic_dynamic_bitset &other) {
        size_type n = m_vec.size();

        if (other.m_bits > m_bits) {
            m_bits = other.m_bits;
            m_vec.append(other.begin() + n, other.end());
        }

        for (size_type i = 0; i < n; ++i) {
            m_vec[i] |= other.m_vec[i];
        }

        return *this;
    }

    basic_dynamic_bitset &operator^=(const basic_dynamic_bitset &other) {
        size_type n = m_vec.size();

        if (other.m_bits > m_bits) {
            m_bits = other.m_bits;
            m_vec.append(other.begin() + n, other.end());
        }

        for (size_type i = 0; i < n; ++i) {
            m_vec[i] ^= other.m_vec[i];
        }

        return *this;
    }

    basic_dynamic_bitset operator~() const {
        basic_dynamic_bitset result(*this);
        result.flip();
        return result;
    }

    basic_dynamic_bitset &flip() {
        size_type n = m_vec.size();
        if (WJR_UNLIKELY(n == 0)) {
            return *this;
        }

        m_vec.back() = ((~m_vec.back()) & ((block_type)1 << (m_bits % block_size))) - 1;

        for (size_type i = 0; i < n - 1; ++i) {
            m_vec[i] = ~m_vec[i];
        }

        return *this;
    }

    basic_dynamic_bitset &flip(size_type pos) {
        WJR_ASSERT(pos < m_bits, "Index out of range.");
        m_vec[pos / block_size] ^= (block_type)1 << (pos % block_size);
        return *this;
    }

    basic_dynamic_bitset &set() {
        size_type n = m_vec.size();
        if (WJR_UNLIKELY(n == 0)) {
            return *this;
        }

        m_vec.back() = ((block_type)1 << (m_bits % block_size)) - 1;
        set_n(m_vec.data(), ~block_type(0), n - 1);

        return *this;
    }

    basic_dynamic_bitset &set(size_type pos, bool val = true) {
        WJR_ASSERT(pos < m_bits, "Index out of range.");
        if (val) {
            m_vec[pos / block_size] |= (block_type)1 << (pos % block_size);
        } else {
            m_vec[pos / block_size] &= ~((block_type)1 << (pos % block_size));
        }
        return *this;
    }

    basic_dynamic_bitset &reset() {
        set_n(m_vec.data(), 0, m_vec.size());
        return *this;
    }

    basic_dynamic_bitset &reset(size_type pos) {
        WJR_ASSERT(pos < m_bits, "Index out of range.");
        m_vec[pos / block_size] &= ~((block_type)1 << (pos % block_size));
        return *this;
    }

    friend basic_dynamic_bitset operator&(const basic_dynamic_bitset &lhs,
                                          const basic_dynamic_bitset &rhs) {
        basic_dynamic_bitset result(lhs);
        result &= rhs;
        return result;
    }

    friend basic_dynamic_bitset operator|(const basic_dynamic_bitset &lhs,
                                          const basic_dynamic_bitset &rhs) {
        basic_dynamic_bitset result(lhs);
        result |= rhs;
        return result;
    }

    friend basic_dynamic_bitset operator^(const basic_dynamic_bitset &lhs,
                                          const basic_dynamic_bitset &rhs) {
        basic_dynamic_bitset result(lhs);
        result ^= rhs;
        return result;
    }

private:
    vector<block_type, allocator_type> m_vec;
    size_type m_bits = 0;
};

using bitset = basic_dynamic_bitset<>;

} // namespace wjr

#endif // WJR_CONTAINER_GENERIC_DYNAMIC_BITSET_HPP__