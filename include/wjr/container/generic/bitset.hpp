#ifndef WJR_CONTAINER_GENERIC_BITSET_HPP__
#define WJR_CONTAINER_GENERIC_BITSET_HPP__

#include <cstring>

#include <wjr/math/bit.hpp>
#include <wjr/math/shift.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

template <size_t N>
class bitset;

template <typename Bitset>
class bitset_const_iterator {
    template <size_t _N>
    friend class bitset;

public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = bool;
    using difference_type = std::ptrdiff_t;
    using pointer = void;
    using reference = bool;

    bitset_const_iterator() = default;
    bitset_const_iterator(const bitset_const_iterator &) = default;
    bitset_const_iterator(bitset_const_iterator &&) = default;
    bitset_const_iterator &operator=(const bitset_const_iterator &) = default;
    bitset_const_iterator &operator=(bitset_const_iterator &&) = default;
    ~bitset_const_iterator() = default;

    constexpr reference operator*() const noexcept { return m_bitset->test(m_pos); }

    constexpr bitset_const_iterator &operator++() noexcept {
        ++m_pos;
        return *this;
    }

    constexpr bitset_const_iterator operator++(int) noexcept {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr bitset_const_iterator &operator--() noexcept {
        --m_pos;
        return *this;
    }

    constexpr bitset_const_iterator operator--(int) noexcept {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    constexpr bitset_const_iterator &operator+=(difference_type n) noexcept {
        m_pos += n;
        return *this;
    }

    constexpr bitset_const_iterator operator+(difference_type n) const noexcept {
        auto tmp = *this;
        return tmp += n;
    }

    friend constexpr bitset_const_iterator
    operator+(difference_type n, const bitset_const_iterator &rhs) noexcept {
        return rhs + n;
    }

    constexpr bitset_const_iterator &operator-=(difference_type n) noexcept {
        m_pos -= n;
        return *this;
    }

    constexpr bitset_const_iterator operator-(difference_type n) const noexcept {
        auto tmp = *this;
        return tmp -= n;
    }

    WJR_PURE constexpr difference_type
    operator-(const bitset_const_iterator &rhs) const noexcept {
        return m_pos - rhs.m_pos;
    }

    WJR_PURE constexpr reference operator[](difference_type n) const noexcept {
        return *(*this + n);
    }

    WJR_PURE constexpr bool operator==(const bitset_const_iterator &rhs) const noexcept {
        return m_pos == rhs.m_pos;
    }

    WJR_PURE constexpr bool operator!=(const bitset_const_iterator &rhs) const noexcept {
        return !(*this == rhs);
    }

    WJR_PURE constexpr bool operator<(const bitset_const_iterator &rhs) const noexcept {
        return m_pos < rhs.m_pos;
    }

    WJR_PURE constexpr bool operator>(const bitset_const_iterator &rhs) const noexcept {
        return rhs < *this;
    }

    WJR_PURE constexpr bool operator<=(const bitset_const_iterator &rhs) const noexcept {
        return !(rhs < *this);
    }

    WJR_PURE constexpr bool operator>=(const bitset_const_iterator &rhs) const noexcept {
        return !(*this < rhs);
    }

protected:
    bitset_const_iterator(const Bitset *_bitset, size_t _pos) noexcept
        : m_bitset(const_cast<Bitset *>(_bitset)), m_pos(_pos) {}

    Bitset *m_bitset;
    size_t m_pos;
};

template <typename Bitset>
class bitset_iterator : public bitset_const_iterator<Bitset> {
    template <size_t _N>
    friend class bitset;

    using Mybase = bitset_const_iterator<Bitset>;

public:
    using iterator_category = typename Mybase::iterator_category;
    using value_type = typename Mybase::value_type;
    using difference_type = typename Mybase::difference_type;
    using pointer = typename Mybase::pointer;
    using reference = typename Bitset::reference;

    using Mybase::Mybase;

    constexpr reference operator*() const noexcept {
        return reference(this->m_bitset, this->m_pos);
    }

    constexpr bitset_iterator &operator++() noexcept {
        Mybase::operator++();
        return *this;
    }

    constexpr bitset_iterator operator++(int) noexcept {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr bitset_iterator &operator--() noexcept {
        Mybase::operator--();
        return *this;
    }

    constexpr bitset_iterator operator--(int) noexcept {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    constexpr bitset_iterator &operator+=(difference_type n) noexcept {
        Mybase::operator+=(n);
        return *this;
    }

    constexpr bitset_iterator operator+(difference_type n) const noexcept {
        auto tmp = *this;
        return tmp += n;
    }

    friend constexpr bitset_iterator operator+(difference_type n,
                                               const bitset_iterator &rhs) noexcept {
        return rhs + n;
    }

    constexpr bitset_iterator &operator-=(difference_type n) noexcept {
        Mybase::operator-=(n);
        return *this;
    }

    constexpr bitset_iterator operator-(difference_type n) const noexcept {
        auto tmp = *this;
        return tmp -= n;
    }

    WJR_PURE constexpr reference operator[](difference_type n) const noexcept {
        return *(*this + n);
    }
};

template <size_t N>
class bitset {
    static constexpr size_t bits = N <= 32 ? 32 : 64;
    static constexpr size_t bytes_size = (N + bits - 1) / bits;
    static constexpr bool full = N == bytes_size * bits;
    static constexpr bool fast_memset =
        bytes_size >= 4 && N <= 1024 && is_zero_or_single_bit(bytes_size);
    using bit_type = std::conditional_t<N <= 32, uint32_t, uint64_t>;
    static constexpr bit_type mask = (static_cast<bit_type>(1) << (N % bits)) - 1;
    static constexpr bit_type maxn = static_cast<bit_type>(in_place_max);

public:
    constexpr bitset() noexcept : m_data() {}
    constexpr bitset(dctor_t) noexcept {}

    bitset(const bitset &) = default;
    bitset(bitset &&) = default;
    bitset &operator=(const bitset &) = default;
    bitset &operator=(bitset &&) = default;
    ~bitset() = default;

    constexpr bitset(uint64_t val) noexcept {
        if constexpr (N < 64) {
            m_data[0] = static_cast<bit_type>(val) & mask;
        } else if constexpr (N == 64) {
            m_data[0] = val;
        } else {
            if constexpr (fast_memset) {
                std::fill_n(m_data, bytes_size, 0);
                m_data[0] = val;
            } else {
                m_data[0] = val;
                std::fill_n(m_data + 1, bytes_size - 1, 0);
            }
        }
    }

    friend constexpr bool operator==(const bitset &lhs, const bitset &rhs) noexcept {
        return std::equal(lhs.m_data, lhs.m_data + bytes_size, rhs.m_data);
    }

    friend constexpr bool operator!=(const bitset &lhs, const bitset &rhs) noexcept {
        return !(lhs == rhs);
    }

    class reference {
        friend class bitset;

    public:
        reference() = delete;
        reference(const reference &) = default;
        reference(reference &&) = default;
        reference &operator=(const reference &) = default;
        reference &operator=(reference &&) = default;
        ~reference() = default;

        constexpr reference &operator=(bool value) noexcept {
            m_bitset->set(m_pos, value);
            return *this;
        }

        constexpr operator bool() const noexcept { return m_bitset->test(m_pos); }

        constexpr bool operator~() const noexcept { return !m_bitset->test(m_pos); }

        constexpr reference &flip() noexcept {
            m_bitset->flip(m_pos);
            return *this;
        }

    private:
        constexpr reference(const bitset *_bitset, size_t _pos) noexcept
            : m_bitset(const_cast<bitset *>(_bitset)), m_pos(_pos) {}

        bitset *m_bitset;
        size_t m_pos;
    };

    constexpr bool operator[](size_t pos) const noexcept { return test(pos); }
    constexpr reference operator[](size_t pos) noexcept { return reference(this, pos); }

    constexpr bool test(size_t pos) const noexcept {
        return ((m_data[pos / bits] >> (pos % bits)) & 1) != 0;
    }

    constexpr static size_t size() noexcept { return N; }

    constexpr bitset &operator&=(const bitset &other) noexcept {
        for (size_t i = 0; i < bytes_size; ++i) {
            m_data[i] &= other.m_data[i];
        }

        return *this;
    }

    constexpr bitset &operator|=(const bitset &other) noexcept {
        for (size_t i = 0; i < bytes_size; ++i) {
            m_data[i] |= other.m_data[i];
        }

        return *this;
    }

    constexpr bitset &operator^=(const bitset &other) noexcept {
        for (size_t i = 0; i < bytes_size; ++i) {
            m_data[i] ^= other.m_data[i];
        }

        return *this;
    }

    constexpr bitset operator~() const noexcept {
        bitset ret;

        for (size_t i = 0; i < bytes_size; ++i) {
            ret.m_data[i] = ~m_data[i];
        }

        if constexpr (!full) {
            ret.m_data[bytes_size - 1] &= mask;
        }

        return ret;
    }

    constexpr bitset &set() noexcept {
        if constexpr (full) {
            std::fill_n(m_data, bytes_size, maxn);
        } else {
            if constexpr (fast_memset) {
                std::fill_n(m_data, bytes_size, maxn);
            } else {
                std::fill_n(m_data, bytes_size - 1, maxn);
            }
            m_data[bytes_size - 1] = mask;
        }
    }

    constexpr bitset &set(size_t pos, bool value = true) noexcept {
        WJR_ASSERT(pos < N);

        auto &word = m_data[pos / bits];
        const auto set_byte = static_cast<bit_type>(1) << (pos % bits);

        if (value) {
            word |= set_byte;
        } else {
            word &= ~set_byte;
        }

        return *this;
    }

    constexpr bitset &reset() noexcept { std::fill_n(m_data, bytes_size, 0); }

    constexpr bitset &reset(size_t pos) noexcept { return set(pos, false); }

    constexpr bitset &flip() noexcept {
        for (size_t i = 0; i < bytes_size; ++i) {
            m_data[i] = ~m_data[i];
        }

        if constexpr (!full) {
            m_data[bytes_size - 1] &= mask;
        }

        return *this;
    }

    constexpr bitset &flip(size_t pos) noexcept {
        m_data[pos / bits] ^= static_cast<bit_type>(1) << (pos % bits);
        return *this;
    }

    friend constexpr bitset operator|(const bitset &lhs, const bitset &rhs) noexcept {
        bitset ret(lhs);
        ret |= rhs;
        return ret;
    }

    friend constexpr bitset operator&(const bitset &lhs, const bitset &rhs) noexcept {
        bitset ret(lhs);
        ret &= rhs;
        return ret;
    }

    friend constexpr bitset operator^(const bitset &lhs, const bitset &rhs) noexcept {
        bitset ret(lhs);
        ret ^= rhs;
        return ret;
    }

    WJR_CONST constexpr size_t count() const noexcept {
        size_t cnt = 0;
        for (size_t i = 0; i < bytes_size; ++i) {
            cnt += popcount(m_data[i]);
        }

        return cnt;
    }

    constexpr bitset &operator<<=(size_t offset) noexcept {
        bit_type low = m_data[0];
        m_data[0] <<= offset;

        if constexpr (bytes_size & 1) {
            if constexpr (bytes_size == 1) {
                if constexpr (!full) {
                    m_data[0] &= mask;
                }
            } else {
                if constexpr (full) {
                    for (size_t i = 1; i != bytes_size; i += 2) {
                        bit_type now = m_data[i];
                        m_data[i] = shld(now, low, offset);
                        low = m_data[i + 1];
                        m_data[i + 1] = shld(low, now, offset);
                    }
                } else {
                    for (size_t i = 1; i < bytes_size - 2; i += 2) {
                        bit_type now = m_data[i];
                        m_data[i] = shld(now, low, offset);
                        low = m_data[i + 1];
                        m_data[i + 1] = shld(low, now, offset);
                    }

                    bit_type now = m_data[bytes_size - 2];
                    m_data[bytes_size - 2] = shld(now, low, offset);
                    low = m_data[bytes_size - 1];
                    m_data[bytes_size - 1] = shld(low, now, offset) & mask;
                }
            }
        } else {
            for (size_t i = 1; i != bytes_size - 1; i += 2) {
                bit_type now = m_data[i];
                m_data[i] = shld(now, low, offset);
                low = m_data[i + 1];
                m_data[i + 1] = shld(low, now, offset);
            }

            m_data[bytes_size - 1] = shld(m_data[bytes_size - 1], low, offset) & mask;
        }

        return *this;
    }

    constexpr bitset &operator>>=(size_t offset) noexcept {
        bit_type high = m_data[bytes_size - 1];
        m_data[bytes_size - 1] >>= offset;

        if constexpr (bytes_size & 1) {
            if constexpr (bytes_size != 1) {
                if constexpr (full) {
                    for (size_t i = bytes_size - 1; i != 0; i -= 2) {
                        bit_type now = m_data[i - 1];
                        m_data[i - 1] = shrd(now, high, offset);
                        high = m_data[i - 2];
                        m_data[i - 2] = shrd(high, now, offset);
                    }
                } else {
                    for (size_t i = bytes_size - 1; i != 2; i -= 2) {
                        bit_type now = m_data[i - 1];
                        m_data[i - 1] = shrd(now, high, offset);
                        high = m_data[i - 2];
                        m_data[i - 2] = shrd(high, now, offset);
                    }

                    bit_type now = m_data[1];
                    m_data[1] = shrd(now, high, offset);
                    high = m_data[0];
                    m_data[0] = shrd(high, now, offset);
                }
            }
        } else {
            for (size_t i = bytes_size - 1; i != 1; i -= 2) {
                bit_type now = m_data[i - 1];
                m_data[i - 1] = shrd(now, high, offset);
                high = m_data[i - 2];
                m_data[i - 2] = shrd(high, now, offset);
            }

            m_data[0] = shrd(m_data[0], high, offset);
        }

        return *this;
    }

    friend constexpr bitset operator<<(const bitset &bs, size_t offset) noexcept {
        bitset ret(bs);
        ret <<= offset;
        return ret;
    }

    friend constexpr bitset operator>>(const bitset &bs, size_t offset) noexcept {
        bitset ret(bs);
        ret >>= offset;
        return ret;
    }

    template <typename Char = char, typename Traits = std::char_traits<Char>,
              typename Alloc = std::allocator<Char>>
    WJR_CONSTEXPR20 std::basic_string<Char, Traits, Alloc>
    to_string(const Char elem0 = static_cast<Char>('0'),
              const Char elem1 = static_cast<Char>('1')) const {
        std::basic_string<Char, Traits, Alloc> str;
        str.reserve(N);

        for (auto pos = N; pos > 0;) {
            str.push_back(test(--pos) ? elem1 : elem0);
        }

        return str;
    }

private:
    bit_type m_data[bytes_size];
};

template <typename Char, typename Traits, size_t N>
std::basic_ostream<Char, Traits> &operator<<(std::basic_ostream<Char, Traits> &os,
                                             const bitset<N> &bs) {
    const auto &fac = std::use_facet<std::ctype<Char>>(os.getloc());
    os << bs.to_string(fac.widen('0'), fac.widen('1'));
    return os;
}

} // namespace wjr

#endif // WJR_CONTAINER_GENERIC_BITSET_HPP__