#ifndef WJR_BIGINTEGER_BIGINTEGER_HPP__
#define WJR_BIGINTEGER_BIGINTEGER_HPP__

#include <wjr/compressed_pair.hpp>
#include <wjr/math.hpp>
#include <wjr/vector.hpp>

namespace wjr {

namespace biginteger_details {

WJR_CONST inline uint32_t abs_size(uint32_t n) { return n & 0x7FFFFFFF; }
WJR_CONST inline uint32_t mask_sign(uint32_t n) { return n & 0x80000000; }

} // namespace biginteger_details

class default_biginteger_size_reference : noncopyable {
public:
    default_biginteger_size_reference() = delete;
    WJR_CONSTEXPR20 explicit default_biginteger_size_reference(uint32_t &size) noexcept
        : m_size(size) {}
    ~default_biginteger_size_reference() = default;

    WJR_CONSTEXPR20 default_biginteger_size_reference &operator=(uint32_t size) noexcept {
        using namespace biginteger_details;
        WJR_ASSUME(!mask_sign(size));
        m_size = mask_sign(m_size) | size;
        return *this;
    }

    WJR_PURE WJR_CONSTEXPR20 operator uint32_t() const noexcept {
        return biginteger_details::abs_size(m_size);
    }

    WJR_CONSTEXPR20 default_biginteger_size_reference &operator++() noexcept {
        ++m_size;
        return *this;
    }

    WJR_CONSTEXPR20 default_biginteger_size_reference &operator--() noexcept {
        --m_size;
        return *this;
    }

    WJR_CONSTEXPR20 default_biginteger_size_reference &
    operator+=(uint32_t size) noexcept {
        m_size += size;
        return *this;
    }

    WJR_CONSTEXPR20 default_biginteger_size_reference &
    operator-=(uint32_t size) noexcept {
        m_size -= size;
        return *this;
    }

private:
    uint32_t &m_size;
};

/**
 * @struct biginteger_data
 * @brief The data structure for biginteger
 *
 */
template <typename Alloc>
class default_biginteger_vector_storage : noncopyable {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<uint64_t>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using value_type = uint64_t;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using size_type = uint32_t;
    using difference_type = int32_t;
    using allocator_type = Alloc;
    using is_reallocatable = std::true_type;

private:
    struct Data {
        pointer m_data = {};
        uint32_t m_size = 0;
        uint32_t m_capacity = 0;
    };

    using data_type = Data;

public:
    default_biginteger_vector_storage() noexcept = default;

    template <typename _Alloc>
    WJR_CONSTEXPR20 default_biginteger_vector_storage(_Alloc &&al) noexcept
        : m_pair(std::forward<_Alloc>(al), {}) {}

    template <typename _Alloc>
    WJR_CONSTEXPR20 default_biginteger_vector_storage(_Alloc &&al, size_type size,
                                                      size_type capacity,
                                                      in_place_reallocate_t) noexcept
        : m_pair(std::forward<_Alloc>(al), {}) {
        uninitialized_construct(size, capacity);
    }

    ~default_biginteger_vector_storage() noexcept = default;

    WJR_PURE WJR_CONSTEXPR20 _Alty &get_allocator() noexcept { return m_pair.first(); }
    WJR_PURE WJR_CONSTEXPR20 const _Alty &get_allocator() const noexcept {
        return m_pair.first();
    }

    WJR_CONSTEXPR20 void destroy() noexcept {
        const size_type __size = size();
        destroy_n_using_allocator(data(), __size, get_allocator());
    }

    WJR_CONSTEXPR20 void deallocate() noexcept {
        get_allocator().deallocate(data(), capacity());
    }

    WJR_CONSTEXPR20 void uninitialized_construct(size_type _size, size_type capacity) {
        auto &al = get_allocator();
        auto &m_storage = __get_data();
        m_storage.m_data = al.allocate(capacity);
        size() = _size;
        m_storage.m_capacity = capacity;
    }

    WJR_CONSTEXPR20 void
    take_storage(default_biginteger_vector_storage &&other) noexcept {
        auto &other_storage = other.__get_data();
        auto &__storage = __get_data();
        __storage.m_data = other_storage.m_data;
        __storage.m_size =
            biginteger_details::mask_sign(__storage.m_size) | other_storage.m_size;
        __storage.m_capacity = other_storage.m_capacity;
        other_storage = {};
    }

    WJR_CONSTEXPR20 void swap_storage(default_biginteger_vector_storage &other) noexcept {
        std::swap(__get_data(), other.__get_data());
    }

    WJR_PURE WJR_CONSTEXPR20 default_biginteger_size_reference size() noexcept {
        return default_biginteger_size_reference(__get_data().m_size);
    }
    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept {
        return biginteger_details::abs_size(__get_data().m_size);
    }
    WJR_PURE WJR_CONSTEXPR20 size_type capacity() const noexcept {
        return __get_data().m_capacity;
    }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept { return __get_data().m_data; }
    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept {
        return __get_data().m_data;
    }

    // extension

    WJR_PURE WJR_CONSTEXPR20 int32_t get_ssize() const noexcept {
        return __get_data().m_size;
    }
    WJR_CONSTEXPR20 void set_ssize(int32_t size) noexcept { __get_data().m_size = size; }

private:
    WJR_PURE WJR_CONSTEXPR20 data_type &__get_data() noexcept { return m_pair.second(); }
    WJR_PURE WJR_CONSTEXPR20 const data_type &__get_data() const noexcept {
        return m_pair.second();
    }

    compressed_pair<_Alty, data_type> m_pair;
};

template <>
struct unref_wrapper<default_biginteger_size_reference> {
    using type = uint32_t &;
};

namespace biginteger_details {

inline uint32_t normalize(uint64_t *ptr, uint32_t n) {
    return reverse_find_not_n(ptr, 0, n);
}

template <bool xsign>
WJR_CONST WJR_CONSTEXPR20 inline int32_t conditional_negate(int32_t x) noexcept {
    return xsign ? (x & 1ull << 31) : x;
}

} // namespace biginteger_details

template <typename Storage>
class basic_biginteger {
    using storage_type = Storage;
    using vector_type = basic_vector<storage_type>;

public:
    using value_type = typename vector_type::value_type;
    using size_type = typename vector_type::size_type;
    using difference_type = typename vector_type::difference_type;
    using reference = typename vector_type::reference;
    using const_reference = typename vector_type::const_reference;
    using pointer = typename vector_type::pointer;
    using const_pointer = typename vector_type::const_pointer;
    using iterator = typename vector_type::iterator;
    using const_iterator = typename vector_type::const_iterator;
    using reverse_iterator = typename vector_type::reverse_iterator;
    using const_reverse_iterator = typename vector_type::const_reverse_iterator;
    using allocator_type = typename vector_type::allocator_type;

    static_assert(std::is_same_v<value_type, uint64_t>, "value_type must be uint64_t");

    basic_biginteger() = default;
    basic_biginteger(const basic_biginteger &other) = default;
    basic_biginteger(basic_biginteger &&other) = default;
    basic_biginteger &operator=(const basic_biginteger &other) = default;
    basic_biginteger &operator=(basic_biginteger &&other) = default;
    ~basic_biginteger() = default;

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept { return m_vec.data(); }
    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept { return m_vec.data(); }

    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept { return m_vec.size(); }
    WJR_PURE WJR_CONSTEXPR20 size_type capacity() const noexcept {
        return m_vec.capacity();
    }

    /**
     * @brief Get the sign of biginteger

     *
     * @return
     * false : if the biginteger is positive \n
     * true : if the biginteger is negative
     */
    WJR_PURE WJR_CONSTEXPR20 bool get_sign() const noexcept { return __get_ssize() < 0; }

    WJR_CONSTEXPR20 void reserve(size_type new_capacity) { m_vec.reserve(new_capacity); }

    WJR_CONSTEXPR20 void resize(size_type new_size) { m_vec.resize(new_size); }
    WJR_CONSTEXPR20 void resize(size_type new_size, value_type value) {
        m_vec.resize(new_size, value);
    }
    WJR_CONSTEXPR20 void resize(size_type new_size, in_place_default_construct_t) {
        m_vec.resize(new_size, in_place_default_construct);
    }

    friend void add(basic_biginteger &dst, const basic_biginteger &lhs,
                    const basic_biginteger &rhs) {
        __addsub<false>(&dst, &lhs, &rhs);
    }

    friend void sub(basic_biginteger &dst, const basic_biginteger &lhs,
                    const basic_biginteger &rhs) {
        __addsub<true>(&dst, &lhs, &rhs);
    }

private:
    WJR_PURE WJR_CONSTEXPR20 storage_type &__get_storage() noexcept {
        return m_vec.get_storage();
    }
    WJR_PURE WJR_CONSTEXPR20 const storage_type &__get_storage() const noexcept {
        return m_vec.get_storage();
    }

    WJR_PURE WJR_CONSTEXPR20 int32_t __get_ssize() const noexcept {
        return __get_storage().get_ssize();
    }
    WJR_CONSTEXPR20 void __set_ssize(int32_t size) noexcept {
        __get_storage().set_ssize(size);
    }

    template <bool xsign>
    static void __addsub(basic_biginteger *dst, const basic_biginteger *lhs,
                         const basic_biginteger *rhs);

    vector_type m_vec;
};

using biginteger =
    basic_biginteger<default_biginteger_vector_storage<std::allocator<uint64_t>>>;

template <typename Storage>
template <bool xsign>
void basic_biginteger<Storage>::__addsub(basic_biginteger *dst,
                                         const basic_biginteger *lhs,
                                         const basic_biginteger *rhs) {
    using namespace biginteger_details;

    int32_t lssize = lhs->__get_ssize();
    int32_t rssize = conditional_negate<xsign>(rhs->__get_ssize());
    uint32_t lusize = abs_size(lssize);
    uint32_t rusize = abs_size(rssize);

    if (lusize < rusize) {
        std::swap(lhs, rhs);
        std::swap(lssize, rssize);
        std::swap(lusize, rusize);
    }

    dst->reserve(lusize + 1);

    int32_t dssize;
    auto dp = dst->data();
    auto lp = lhs->data();
    auto rp = rhs->data();

    // different sign
    if ((lssize ^ rssize) < 0) {
        if (lusize != rusize) {
            subc_sz(dp, lp, lusize, rp, rusize);
            dssize = normalize(dp, lusize);
            dssize = mask_sign(lssize) | rssize;
        } else {
            if (WJR_UNLIKELY(lusize == 0)) {
                dssize = 0;
            } else {
                dssize = abs_subc_n(dp, lp, rp, rusize);
                dssize = mask_sign(lssize) | rssize;
            }
        }
    } else {
        auto cf = addc_sz(dp, lp, lusize, rp, rusize);
        dssize = mask_sign(lssize) | (lusize + cf);
        if (cf) {
            dp[lusize] = 1;
        }
    }

    dst->__set_ssize(dssize);
}

} // namespace wjr

#endif