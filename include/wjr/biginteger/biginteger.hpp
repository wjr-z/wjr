#ifndef WJR_BIGINTEGER_BIGINTEGER_HPP__
#define WJR_BIGINTEGER_BIGINTEGER_HPP__

#include <wjr/compressed_pair.hpp>
#include <wjr/math.hpp>
#include <wjr/vector.hpp>

namespace wjr {

namespace biginteger_details {

inline uint32_t normalize(uint64_t *ptr, uint32_t n) {
    return reverse_find_not_n(ptr, 0, n);
}

} // namespace biginteger_details

class default_biginteger_size_reference : noncopyable {
public:
    default_biginteger_size_reference() = delete;
    explicit default_biginteger_size_reference(int32_t &size) noexcept : m_size(size) {}
    ~default_biginteger_size_reference() = default;
    default_biginteger_size_reference(default_biginteger_size_reference &&) = default;
    default_biginteger_size_reference &
    operator=(default_biginteger_size_reference &&) = default;

    default_biginteger_size_reference &operator=(uint32_t size) noexcept {
        m_size = __fasts_get_sign_mask(m_size) | size;
        return *this;
    }

    WJR_PURE operator uint32_t() const noexcept { return __fasts_abs(m_size); }

    default_biginteger_size_reference &operator++() noexcept {
        ++m_size;
        return *this;
    }

    default_biginteger_size_reference &operator--() noexcept {
        --m_size;
        return *this;
    }

    default_biginteger_size_reference &operator+=(uint32_t size) noexcept {
        m_size += size;
        return *this;
    }

    default_biginteger_size_reference &operator-=(uint32_t size) noexcept {
        m_size -= size;
        return *this;
    }

private:
    int32_t &m_size;
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
        int32_t m_size = 0;
        uint32_t m_capacity = 0;
    };

    using data_type = Data;

public:
    default_biginteger_vector_storage() noexcept = default;

    template <typename _Alloc>
    default_biginteger_vector_storage(_Alloc &&al) noexcept
        : m_pair(std::forward<_Alloc>(al), {}) {}

    template <typename _Alloc>
    default_biginteger_vector_storage(_Alloc &&al, size_type size, size_type capacity,
                                      in_place_reallocate_t) noexcept
        : m_pair(std::forward<_Alloc>(al), {}) {
        uninitialized_construct(size, capacity);
    }

    ~default_biginteger_vector_storage() noexcept = default;

    WJR_PURE _Alty &get_allocator() noexcept { return m_pair.first(); }
    WJR_PURE const _Alty &get_allocator() const noexcept { return m_pair.first(); }

    void destroy() noexcept {
        if (WJR_BUILTIN_CONSTANT_P(data() == nullptr) && data() == nullptr) {
            return;
        }

        const size_type __size = size();

        if (WJR_BUILTIN_CONSTANT_P(__size == 0) && __size == 0) {
            return;
        }

        destroy_n_using_allocator(data(), __size, get_allocator());
    }

    void destroy_and_deallocate() noexcept {
        if (WJR_BUILTIN_CONSTANT_P(capacity() == 0) && capacity() == 0) {
            return;
        }

        if (data()) {
            destroy();
            get_allocator().deallocate(data(), capacity());
        }
    }

    void uninitialized_construct(size_type _size, size_type capacity) {
        auto &al = get_allocator();
        auto &m_storage = __get_data();
        m_storage.m_data = al.allocate(capacity);
        size() = _size;
        m_storage.m_capacity = capacity;
    }

    void take_storage(default_biginteger_vector_storage &&other) noexcept {
        auto &other_storage = other.__get_data();
        auto &__storage = __get_data();
        __storage.m_data = other_storage.m_data;
        size() = other_storage.m_size;
        __storage.m_capacity = other_storage.m_capacity;
        other_storage = {};
    }

    void swap_storage(default_biginteger_vector_storage &other) noexcept {
        std::swap(__get_data(), other.__get_data());
    }

    WJR_PURE default_biginteger_size_reference size() noexcept {
        return default_biginteger_size_reference(__get_data().m_size);
    }
    WJR_PURE size_type size() const noexcept { return __fasts_abs(__get_data().m_size); }
    WJR_PURE size_type capacity() const noexcept { return __get_data().m_capacity; }

    WJR_PURE pointer data() noexcept { return __get_data().m_data; }
    WJR_PURE const_pointer data() const noexcept { return __get_data().m_data; }

    // extension

    WJR_PURE int32_t get_ssize() const noexcept { return __get_data().m_size; }
    void set_ssize(int32_t size) noexcept {
        WJR_ASSUME(__fasts_abs(size) <= capacity());
        __get_data().m_size = size;
    }

private:
    WJR_PURE data_type &__get_data() noexcept { return m_pair.second(); }
    WJR_PURE const data_type &__get_data() const noexcept { return m_pair.second(); }

    compressed_pair<_Alty, data_type> m_pair;
};

template <>
struct unref_wrapper<default_biginteger_size_reference> {
    using type = uint32_t &;
};

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

    explicit basic_biginteger(const allocator_type &al) : m_vec(al) {}

    template <typename Iter, std::enable_if_t<is_iterator_v<Iter>, int> = 0>
    basic_biginteger(Iter first, Iter last, const allocator_type &al)
        : m_vec(first, last, al) {
        __check_high_bit();
    }

    basic_biginteger(const basic_biginteger &other, const allocator_type &al)
        : m_vec(other.m_vec, al) {}

    basic_biginteger(basic_biginteger &&other, const allocator_type &al)
        : m_vec(std::move(other.m_vec), al) {}

    basic_biginteger(std::initializer_list<value_type> il, const allocator_type &al)
        : m_vec(il, al) {
        __check_high_bit();
    }

    basic_biginteger(size_type n, in_place_default_construct_t, const allocator_type &al)
        : m_vec(n, in_place_default_construct, al) {}

    basic_biginteger &operator=(std::initializer_list<value_type> il) {
        m_vec = il;
        __check_high_bit();
        return *this;
    }

    template <typename Iter, std::enable_if_t<is_iterator_v<Iter>, int> = 0>
    void assign(Iter first, Iter last) {
        m_vec.assign(first, last);
        __check_high_bit();
    }

    void assign(std::initializer_list<value_type> il) {
        m_vec.assign(il);
        __check_high_bit();
    }

    allocator_type &get_allocator() noexcept { return m_vec.get_allocator(); }
    const allocator_type &get_allocator() const noexcept { return m_vec.get_allocator(); }

    reference at(size_type pos) { return m_vec.at(pos); }
    const_reference at(size_type pos) const { return m_vec.at(pos); }

    reference operator[](size_type pos) { return m_vec[pos]; }
    const_reference operator[](size_type pos) const { return m_vec[pos]; }

    reference front() { return m_vec.front(); }
    const_reference front() const { return m_vec.front(); }

    reference back() { return m_vec.back(); }
    const_reference back() const { return m_vec.back(); }

    WJR_PURE pointer data() noexcept { return m_vec.data(); }
    WJR_PURE const_pointer data() const noexcept { return m_vec.data(); }

    WJR_PURE iterator begin() noexcept { return m_vec.begin(); }
    WJR_PURE const_iterator begin() const noexcept { return m_vec.begin(); }

    WJR_PURE iterator end() noexcept { return m_vec.end(); }
    WJR_PURE const_iterator end() const noexcept { return m_vec.end(); }

    WJR_PURE const_iterator cbegin() const noexcept { return m_vec.cbegin(); }

    WJR_PURE const_iterator cend() const noexcept { return m_vec.cend(); }

    WJR_PURE reverse_iterator rbegin() noexcept { return m_vec.rbegin(); }
    WJR_PURE const_reverse_iterator rbegin() const noexcept { return m_vec.rbegin(); }

    WJR_PURE reverse_iterator rend() noexcept { return m_vec.rend(); }
    WJR_PURE const_reverse_iterator rend() const noexcept { return m_vec.rend(); }

    WJR_PURE const_reverse_iterator crbegin() const noexcept { return m_vec.crbegin(); }
    WJR_PURE const_reverse_iterator crend() const noexcept { return m_vec.crend(); }

    WJR_PURE bool empty() const noexcept { return m_vec.empty(); }
    WJR_PURE size_type size() const noexcept { return m_vec.size(); }
    WJR_PURE size_type capacity() const noexcept { return m_vec.capacity(); }

    WJR_CONST static size_type get_growth_capacity(size_type old_capacity,
                                                   size_type new_size) noexcept {
        return vector_type::get_growth_capacity(old_capacity, new_size);
    }

    void reserve(size_type new_capacity) { m_vec.reserve(new_capacity); }

    void shrink_to_fit() { m_vec.shrink_to_fit(); }

    void clear() { m_vec.clear(); }

    void swap(basic_biginteger &other) noexcept { m_vec.swap(other.m_vec); }

    /**
     * @brief Get the sign of biginteger
     *
     * @return
     * false : if the biginteger is positive \n
     * true : if the biginteger is negative
     */
    WJR_PURE bool get_sign() const noexcept { return get_ssize() < 0; }

    WJR_PURE int32_t get_ssize() const { return __get_storage().get_ssize(); }
    void set_ssize(int32_t new_size) { __get_storage().set_ssize(new_size); }

    friend bool operator==(const basic_biginteger &lhs, const basic_biginteger &rhs) {
        return lhs.m_vec == rhs.m_vec;
    }

    friend bool operator!=(const basic_biginteger &lhs, const basic_biginteger &rhs) {
        return lhs.m_vec != rhs.m_vec;
    }

    friend bool operator<(const basic_biginteger &lhs, const basic_biginteger &rhs) {
        return lhs.m_vec < rhs.m_vec;
    }

    friend bool operator>(const basic_biginteger &lhs, const basic_biginteger &rhs) {
        return lhs.m_vec > rhs.m_vec;
    }

    friend bool operator<=(const basic_biginteger &lhs, const basic_biginteger &rhs) {
        return lhs.m_vec <= rhs.m_vec;
    }

    friend bool operator>=(const basic_biginteger &lhs, const basic_biginteger &rhs) {
        return lhs.m_vec >= rhs.m_vec;
    }

    friend void add(basic_biginteger &dst, const basic_biginteger &lhs,
                    const basic_biginteger &rhs) {
        __addsub<false>(&dst, &lhs, &rhs);
    }

    friend void sub(basic_biginteger &dst, const basic_biginteger &lhs,
                    const basic_biginteger &rhs) {
        __addsub<true>(&dst, &lhs, &rhs);
    }

    friend void mul(basic_biginteger &dst, const basic_biginteger &lhs,
                    const basic_biginteger &rhs) {
        __mul(&dst, &lhs, &rhs);
    }

private:
    WJR_PURE storage_type &__get_storage() noexcept { return m_vec.get_storage(); }
    WJR_PURE const storage_type &__get_storage() const noexcept {
        return m_vec.get_storage();
    }

    void __check_high_bit() const {
        WJR_ASSERT(size() == 0 || back() != 0, "biginteger should not have leading zero");
    }

    template <bool xsign>
    static void __addsub(basic_biginteger *dst, const basic_biginteger *lhs,
                         const basic_biginteger *rhs);

    static void __mul(basic_biginteger *dst, const basic_biginteger *lhs,
                      const basic_biginteger *rhs);

    vector_type m_vec;
};

using biginteger =
    basic_biginteger<default_biginteger_vector_storage<std::allocator<uint64_t>>>;

template <typename Storage>
void swap(basic_biginteger<Storage> &lhs, basic_biginteger<Storage> &rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Storage>
template <bool xsign>
void basic_biginteger<Storage>::__addsub(basic_biginteger *dst,
                                         const basic_biginteger *lhs,
                                         const basic_biginteger *rhs) {
    int32_t lssize = lhs->get_ssize();
    int32_t rssize = xsign ? __fasts_negate(rhs->get_ssize()) : rhs->get_ssize();
    uint32_t lusize = __fasts_abs(lssize);
    uint32_t rusize = __fasts_abs(rssize);

    if (lusize < rusize) {
        std::swap(lhs, rhs);
        std::swap(lssize, rssize);
        std::swap(lusize, rusize);
    }

    dst->reserve(lusize + 1);

    auto dp = dst->data();
    auto lp = lhs->data();
    auto rp = rhs->data();
    int32_t dssize;

    // different sign
    if ((lssize ^ rssize) < 0) {
        if (lusize != rusize) {
            subc_sz(dp, lp, lusize, rp, rusize);
            dssize = __fasts_get_sign_mask(lssize) | normalize(dp, lusize);
        } else {
            if (WJR_UNLIKELY(lusize == 0)) {
                dssize = 0;
            } else {
                dssize = __fasts_get_sign_mask(lssize) ^ abs_subc_n(dp, lp, rp, rusize);
            }
        }
    } else {
        auto cf = addc_sz(dp, lp, lusize, rp, rusize);
        dssize = __fasts_get_sign_mask(lssize) | (lusize + cf);
        if (cf) {
            dp[lusize] = 1;
        }
    }

    dst->set_ssize(dssize);
}

template <typename Storage>
void basic_biginteger<Storage>::__mul(basic_biginteger *dst, const basic_biginteger *lhs,
                                      const basic_biginteger *rhs) {
    using namespace biginteger_details;

    int32_t lssize = lhs->get_ssize();
    int32_t rssize = rhs->get_ssize();
    int32_t mask = __fasts_get_sign_mask(lssize ^ rssize);
    uint32_t lusize = __fasts_abs(lssize);
    uint32_t rusize = __fasts_abs(rssize);

    if (lusize < rusize) {
        std::swap(lhs, rhs);
        std::swap(lusize, rusize);
    }

    if (WJR_UNLIKELY(rusize == 0)) {
        dst->set_ssize(0);
        return;
    }

    int32_t dssize;

    if (rusize == 1) {
        dst->reserve(lusize + 1);
        auto cf = mul_1(dst->data(), lhs->data(), lusize, rhs->front());
        dssize = mask | (lusize + (cf != 0));
        if (cf != 0) {
            (*dst)[lusize] = cf;
        }
        dst->set_ssize(dssize);
        return;
    }

    dssize = lusize + rusize;

    auto dp = dst->data();
    auto lp = (pointer)lhs->data();
    auto rp = (pointer)rhs->data();

    std::aligned_storage_t<sizeof(basic_biginteger), alignof(basic_biginteger)> storage;
    // no need to destroy this object
    auto &temp = *reinterpret_cast<basic_biginteger *>(&storage);

    unique_stack_allocator stkal(math_details::stack_alloc);

    if (dst->capacity() < dssize) {
        new (&temp)
            basic_biginteger(dssize, in_place_default_construct, dst->get_allocator());

        dp = temp.data();
    } else {
        new (&temp) basic_biginteger;

        if (dp == lp) {
            lp = (pointer)stkal.allocate(lusize);
            if (dp == rp) {
                rp = lp;
            }
            std::copy_n(dp, lusize, lp);
        } else if (dp == rp) {
            rp = (pointer)stkal.allocate(rusize);
            std::copy_n(dp, rusize, rp);
        }
    }

    if (WJR_UNLIKELY(lp == rp)) {
        sqr(dp, lp, lusize);
    } else {
        mul_s(dp, lp, lusize, rp, rusize);
    }

    bool cf = dp[dssize - 1] == 0;
    dssize = mask | (dssize - cf);

    if (temp.data() != nullptr) {
        *dst = std::move(temp);
    }

    dst->set_ssize(dssize);
}

} // namespace wjr

#endif