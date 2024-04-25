#ifndef WJR_BIGINTEGER_BIGINTEGER_HPP__
#define WJR_BIGINTEGER_BIGINTEGER_HPP__

#include <optional>

#include <wjr/format/ostream_insert.hpp>
#include <wjr/math.hpp>
#include <wjr/span.hpp>
#include <wjr/vector.hpp>

namespace wjr {

namespace biginteger_details {

inline uint32_t normalize(uint64_t *ptr, uint32_t n) {
    return reverse_find_not_n(ptr, 0, n);
}

} // namespace biginteger_details

class default_biginteger_size_reference {
public:
    default_biginteger_size_reference() = delete;
    default_biginteger_size_reference(const default_biginteger_size_reference &) = delete;
    default_biginteger_size_reference(default_biginteger_size_reference &&) = default;
    default_biginteger_size_reference &
    operator=(const default_biginteger_size_reference &) = delete;
    default_biginteger_size_reference &
    operator=(default_biginteger_size_reference &&) = default;

    explicit default_biginteger_size_reference(int32_t &size) noexcept : m_size(&size) {}
    ~default_biginteger_size_reference() = default;

    default_biginteger_size_reference &operator=(uint32_t size) noexcept {
        *m_size = __fasts_get_sign_mask(*m_size) | size;
        return *this;
    }

    WJR_PURE operator uint32_t() const noexcept { return __fasts_abs(*m_size); }

    default_biginteger_size_reference &operator++() noexcept {
        ++*m_size;
        return *this;
    }

    default_biginteger_size_reference &operator--() noexcept {
        --*m_size;
        return *this;
    }

    default_biginteger_size_reference &operator+=(uint32_t size) noexcept {
        *m_size += size;
        return *this;
    }

    default_biginteger_size_reference &operator-=(uint32_t size) noexcept {
        *m_size -= size;
        return *this;
    }

private:
    int32_t *m_size;
};

template <>
struct __unref_wrapper_helper<default_biginteger_size_reference> {
    using type = uint32_t &;
};

/**
 * @struct biginteger_data
 * @brief The data structure for biginteger
 *
 */
template <typename Alloc>
class default_biginteger_vector_storage {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<uint64_t>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using value_type = uint64_t;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using size_type = uint32_t;
    using difference_type = int32_t;
    using allocator_type = Alloc;
    using storage_traits_type = vector_storage_traits<uint64_t, Alloc>;
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

    default_biginteger_vector_storage(const default_biginteger_vector_storage &) = delete;
    default_biginteger_vector_storage(default_biginteger_vector_storage &&) noexcept =
        delete;
    default_biginteger_vector_storage &
    operator=(const default_biginteger_vector_storage &) = delete;
    default_biginteger_vector_storage &
    operator=(default_biginteger_vector_storage &&) noexcept = delete;

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

    void take_storage(default_biginteger_vector_storage &other) noexcept {
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

template <typename Storage>
class basic_biginteger;

template <typename S>
from_chars_result<> from_chars(const char *first, const char *last,
                               basic_biginteger<S> &dst, unsigned int base = 10);

template <typename S, typename Iter>
Iter to_chars_unchecked(Iter ptr, const basic_biginteger<S> &src, unsigned int base = 10);

#define WJR_REGISTER_BIGINTEGER_COMPARE(op)                                              \
    template <typename S>                                                                \
    WJR_PURE bool operator op(const basic_biginteger<S> &lhs,                            \
                              const basic_biginteger<S> &rhs);                           \
    template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>            \
    WJR_PURE bool operator op(const basic_biginteger<S> &lhs, T rhs);                    \
    template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>            \
    WJR_PURE bool operator op(T lhs, const basic_biginteger<S> &rhs);

WJR_REGISTER_BIGINTEGER_COMPARE(==)
WJR_REGISTER_BIGINTEGER_COMPARE(!=)
WJR_REGISTER_BIGINTEGER_COMPARE(<)
WJR_REGISTER_BIGINTEGER_COMPARE(>)
WJR_REGISTER_BIGINTEGER_COMPARE(<=)
WJR_REGISTER_BIGINTEGER_COMPARE(>=)

#undef WJR_REGISTER_BIGINTEGER_COMPARE

#define WJR_REGISTER_BIGINTEGER_ADDSUB(ADDSUB)                                           \
    template <typename S>                                                                \
    void ADDSUB(basic_biginteger<S> &dst, const basic_biginteger<S> &lhs,                \
                const basic_biginteger<S> &rhs);                                         \
    template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>            \
    void ADDSUB(basic_biginteger<S> &dst, const basic_biginteger<S> &lhs, T rhs);        \
    template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>            \
    void ADDSUB(basic_biginteger<S> &dst, T lhs, const basic_biginteger<S> &rhs);

WJR_REGISTER_BIGINTEGER_ADDSUB(add)
WJR_REGISTER_BIGINTEGER_ADDSUB(sub)

#undef WJR_REGISTER_BIGINTEGER_ADDSUB

template <typename S>
void mul(basic_biginteger<S> &dst, const basic_biginteger<S> &lhs,
         const basic_biginteger<S> &rhs);

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void mul(basic_biginteger<S> &dst, const basic_biginteger<S> &lhs, T rhs);

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void mul(basic_biginteger<S> &dst, T lhs, const basic_biginteger<S> &rhs);

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

    basic_biginteger(const basic_biginteger &other,
                     const allocator_type &al = allocator_type())
        : m_vec(other.m_vec, al) {
        set_ssize(other.get_ssize());
    }

    basic_biginteger(size_type n, in_place_reserve_t,
                     const allocator_type &al = allocator_type())
        : m_vec(n, in_place_reserve, al) {}

    basic_biginteger(basic_biginteger &&other) = default;
    basic_biginteger &operator=(const basic_biginteger &other) {
        if (WJR_UNLIKELY(this == std::addressof(other))) {
            return *this;
        }

        m_vec = other.m_vec;
        set_ssize(other.get_ssize());
        return *this;
    }

    basic_biginteger &operator=(basic_biginteger &&other) = default;
    ~basic_biginteger() = default;

    explicit basic_biginteger(const allocator_type &al) : m_vec(al) {}

    basic_biginteger(basic_biginteger &&other, const allocator_type &al)
        : m_vec(std::move(other.m_vec), al) {}

    template <typename UnsignedValue,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    explicit basic_biginteger(UnsignedValue value,
                              const allocator_type &al = allocator_type())
        : m_vec(value != 0, value, al) {}

    template <typename SignedValue,
              WJR_REQUIRES(is_nonbool_signed_integral_v<SignedValue>)>
    explicit basic_biginteger(SignedValue value,
                              const allocator_type &al = allocator_type())
        : m_vec(al) {
        if (value != 0) {
            m_vec.emplace_back(value < 0 ? -to_unsigned(value) : to_unsigned(value));
            set_ssize(__fasts_conditional_negate(value < 0, 1));
        }
    }

    explicit basic_biginteger(span<const char> sp, unsigned int base = 10,
                              const allocator_type &al = allocator_type())
        : m_vec(al) {
        from_string(sp, base);
    }

    template <typename UnsignedValue,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    basic_biginteger &operator=(UnsignedValue value) {
        clear();
        if (value != 0) {
            m_vec.emplace_back(value);
        }
        return *this;
    }

    template <typename SignedValue,
              WJR_REQUIRES(is_nonbool_signed_integral_v<SignedValue>)>
    basic_biginteger &operator=(SignedValue value) {
        clear();
        if (value != 0) {
            m_vec.emplace_back(value < 0 ? -to_unsigned(value) : to_unsigned(value));
            set_ssize(__fasts_conditional_negate(value < 0, 1));
        }
        return *this;
    }

    basic_biginteger &operator=(span<const char> sp) { return from_string(sp); }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    explicit operator T() const noexcept {
        if (empty()) {
            return static_cast<T>(0);
        }

        if constexpr (std::is_unsigned_v<T>) {
            return static_cast<T>(front());
        } else {
            const auto ret = front();
            return is_negate() ? -ret : ret;
        }
    }

    basic_biginteger &from_string(span<const char> sp, unsigned int base = 10) {
        (void)from_chars(sp.data(), sp.data() + sp.size(), *this, base);
        return *this;
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

    void reserve(size_type new_capacity) { m_vec.reserve(new_capacity); }

    void shrink_to_fit() { m_vec.shrink_to_fit(); }

    void clear() {
        m_vec.clear();
        set_ssize(0);
    }

    void swap(basic_biginteger &other) noexcept { m_vec.swap(other.m_vec); }

    WJR_PURE int32_t get_ssize() const { return __get_storage().get_ssize(); }
    void set_ssize(int32_t new_size) { __get_storage().set_ssize(new_size); }

    WJR_CONST static size_type get_growth_capacity(size_type old_capacity,
                                                   size_type new_size) noexcept {
        return vector_type::get_growth_capacity(old_capacity, new_size);
    }

    void conditional_negate(bool condition) noexcept {
        if (const int32_t xssize = get_ssize(); xssize != 0) {
            set_ssize(__fasts_conditional_negate(condition, xssize));
        }
    }

    void negate() noexcept { conditional_negate(true); }

    WJR_PURE bool is_negate() const noexcept { return get_ssize() < 0; }

private:
    WJR_PURE storage_type &__get_storage() noexcept { return m_vec.get_storage(); }
    WJR_PURE const storage_type &__get_storage() const noexcept {
        return m_vec.get_storage();
    }

    void __check_high_bit() const {
        WJR_ASSERT(size() == 0 || back() != 0, "biginteger should not have leading zero");
    }

    static from_chars_result<> __from_chars_impl(const char *first, const char *last,
                                                 basic_biginteger *dst,
                                                 unsigned int base);

    static int __compare_impl(const basic_biginteger *lhs, const basic_biginteger *rhs);

    static int __compare_ui_impl(const basic_biginteger *lhs, uint64_t rhs);
    static int __compare_si_impl(const basic_biginteger *lhs, int64_t rhs);

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    static int __compare_impl(const basic_biginteger *lhs, T rhs) {
        if (WJR_BUILTIN_CONSTANT_P(rhs == 0) && rhs == 0) {
            return lhs->empty() ? 0 : lhs->is_negate() ? -1 : 1;
        }

        if constexpr (std::is_unsigned_v<T>) {
            return __compare_ui_impl(lhs, rhs);
        } else {
            return __compare_si_impl(lhs, rhs);
        }
    }

    template <bool xsign>
    static void __addsub_impl(basic_biginteger *dst, const basic_biginteger *lhs,
                              uint64_t rhs);

    static void __ui_sub_impl(basic_biginteger *dst, uint64_t lhs,
                              const basic_biginteger *rhs);

    template <bool xsign>
    static void __addsub_impl(basic_biginteger *dst, const basic_biginteger *lhs,
                              const basic_biginteger *rhs);

    static void __add_impl(basic_biginteger *dst, const basic_biginteger *lhs,
                           const basic_biginteger *rhs) {
        __addsub_impl<false>(dst, lhs, rhs);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    static void __add_impl(basic_biginteger *dst, const basic_biginteger *lhs, T rhs) {
        if constexpr (std::is_unsigned_v<T>) {
            __addsub_impl<false>(dst, lhs, rhs);
        } else {
            if (rhs < 0) {
                __addsub_impl<true>(dst, lhs, -to_unsigned(rhs));
            } else {
                __addsub_impl<false>(dst, lhs, to_unsigned(rhs));
            }
        }
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    static void __add_impl(basic_biginteger *dst, T lhs, const basic_biginteger *rhs) {
        __add_impl(dst, rhs, lhs);
    }

    static void __sub_impl(basic_biginteger *dst, const basic_biginteger *lhs,
                           const basic_biginteger *rhs) {
        __addsub_impl<true>(dst, lhs, rhs);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    static void __sub_impl(basic_biginteger *dst, const basic_biginteger *lhs, T rhs) {
        if constexpr (std::is_unsigned_v<T>) {
            __addsub_impl<true>(dst, lhs, rhs);
        } else {
            if (rhs < 0) {
                __addsub_impl<false>(dst, lhs, -to_unsigned(rhs));
            } else {
                __addsub_impl<true>(dst, lhs, to_unsigned(rhs));
            }
        }
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    static void __sub_impl(basic_biginteger *dst, T lhs, const basic_biginteger *rhs) {
        if constexpr (std::is_unsigned_v<T>) {
            __ui_sub_impl(dst, lhs, rhs);
        } else {
            if (lhs < 0) {
                __addsub_impl<false>(dst, rhs, -to_unsigned(lhs));
                dst->negate();
            } else {
                __ui_sub_impl(dst, to_unsigned(lhs), rhs);
            }
        }
    }

    static void __mul_ui_impl(basic_biginteger *dst, const basic_biginteger *lhs,
                              uint64_t rhs);

    static void __mul_impl(basic_biginteger *dst, const basic_biginteger *lhs,
                           const basic_biginteger *rhs);

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    static void __mul_impl(basic_biginteger *dst, const basic_biginteger *lhs, T rhs) {
        if constexpr (std::is_unsigned_v<T>) {
            __mul_ui_impl(dst, lhs, rhs);
        } else {
            auto value = to_unsigned(rhs);
            bool cond = false;
            if (rhs < 0) {
                value = -value;
                cond = true;
            }

            __mul_ui_impl(dst, lhs, value);
            dst->conditional_negate(cond);
        }
    }

    static void __addsubmul_ui_impl(basic_biginteger *dst, const basic_biginteger *lhs,
                                    uint64_t rhs, bool xsign);

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    static void __addmul_impl(basic_biginteger *dst, const basic_biginteger *lhs, T rhs) {
        if constexpr (std::is_unsigned_v<T>) {
        } else {
        }
    }

    template <typename S>
    friend from_chars_result<> from_chars(const char *first, const char *last,
                                          basic_biginteger<S> &dst, unsigned int base);

    template <typename S, typename Iter>
    friend Iter to_chars_unchecked(Iter ptr, const basic_biginteger<S> &src,
                                   unsigned int base);

#define WJR_REGISTER_BIGINTEGER_COMPARE(op)                                              \
    template <typename S>                                                                \
    WJR_PURE friend bool operator op(const basic_biginteger<S> &lhs,                     \
                                     const basic_biginteger<S> &rhs);                    \
    template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>          \
    WJR_PURE friend bool operator op(const basic_biginteger<S> &lhs, T rhs);             \
    template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>          \
    WJR_PURE friend bool operator op(T lhs, const basic_biginteger<S> &rhs);

    WJR_REGISTER_BIGINTEGER_COMPARE(==)
    WJR_REGISTER_BIGINTEGER_COMPARE(!=)
    WJR_REGISTER_BIGINTEGER_COMPARE(<)
    WJR_REGISTER_BIGINTEGER_COMPARE(>)
    WJR_REGISTER_BIGINTEGER_COMPARE(<=)
    WJR_REGISTER_BIGINTEGER_COMPARE(>=)

#undef WJR_REGISTER_BIGINTEGER_COMPARE

#define WJR_REGISTER_BIGINTEGER_ADDSUB(ADDSUB)                                           \
    template <typename S>                                                                \
    friend void ADDSUB(basic_biginteger<S> &dst, const basic_biginteger<S> &lhs,         \
                       const basic_biginteger<S> &rhs);                                  \
    template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>          \
    friend void ADDSUB(basic_biginteger<S> &dst, const basic_biginteger<S> &lhs, T rhs); \
    template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>          \
    friend void ADDSUB(basic_biginteger<S> &dst, T lhs, const basic_biginteger<S> &rhs);

    WJR_REGISTER_BIGINTEGER_ADDSUB(add)
    WJR_REGISTER_BIGINTEGER_ADDSUB(sub)

#undef WJR_REGISTER_BIGINTEGER_ADDSUB

    template <typename S>
    friend void mul(basic_biginteger<S> &dst, const basic_biginteger<S> &lhs,
                    const basic_biginteger<S> &rhs);

    template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
    friend void mul(basic_biginteger<S> &dst, const basic_biginteger<S> &lhs, T rhs);

    template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
    friend void mul(basic_biginteger<S> &dst, T lhs, const basic_biginteger<S> &rhs);

    vector_type m_vec;
};

template <typename Alloc = std::allocator<uint64_t>>
using default_biginteger = basic_biginteger<default_biginteger_vector_storage<Alloc>>;

using biginteger = default_biginteger<std::allocator<uint64_t>>;

template <typename Storage>
void swap(basic_biginteger<Storage> &lhs, basic_biginteger<Storage> &rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Storage>
from_chars_result<>
basic_biginteger<Storage>::__from_chars_impl(const char *first, const char *last,
                                             basic_biginteger *dst, unsigned int base) {

    uint8_t ch;
    from_chars_result<> result{first, std::errc{}};

    auto invalid = [&dst, &result]() {
        dst->clear();
        result.ec = std::errc::invalid_argument;
        return result;
    };

    if (first == last) {
        return invalid();
    }

    while (convert_details::__isspace(ch = *first++) && first != last)
        ;

    int sign = 0;
    if (ch == '-') {
        sign = 1;

        if (WJR_UNLIKELY(first == last)) {
            return invalid();
        }

        ch = *first++;
    }

    if (base == 0) {
        base = 10;
        if (ch == '0') {
            base = 8;
            if (WJR_UNLIKELY(first == last)) {
                return invalid();
            }
            ch = *first++;
            if (ch == 'x' || ch == 'X') {
                base = 16;
                if (WJR_UNLIKELY(first == last)) {
                    return invalid();
                }
                ch = *first++;
            } else {
                if (ch == 'b' || ch == 'B') {
                    base = 2;
                    if (WJR_UNLIKELY(first == last)) {
                        return invalid();
                    }
                    ch = *first++;
                }
            }
        }
    }

    auto __first = first;

    while (ch == '0') {
        if (WJR_UNLIKELY(first == last)) {
            dst->clear();
            result.ptr = first;
            return result;
        }

        ch = *first++;
    }

    const auto start = first;
    if (base <= 10) {
        while (ch >= '0' && ch < '0' + base) {
            if (first == last) {
                ++first;
                break;
            }

            ch = *first++;
        }
    } else {
        ch = 0;
        while (ch < base) {
            if (first == last) {
                ++first;
                break;
            }

            ch = char_converter.from(*first++);
        }
    }

    if (WJR_UNLIKELY(first == __first)) {
        return invalid();
    }

    if (first == start) {
        dst->clear();
        result.ptr = first;
        return result;
    }

    size_t str_size = first - start;
    size_t capacity;

    switch (base) {
    case 2: {
        capacity = __ceil_div(str_size, 64);
        break;
    }
    case 8: {
        capacity = __ceil_div(str_size * 3, 64);
        break;
    }
    case 16: {
        capacity = __ceil_div(str_size, 16);
        break;
    }
    case 4:
    case 32: {
        const int bits = base == 4 ? 2 : 5;
        capacity = __ceil_div(str_size * bits, 64);
        break;
    }
    case 10: {
        // capacity = (str_size * log2(10) + 63) / 64;
        capacity = __ceil_div(str_size * 10 / 3, 64);
        break;
    }
    default: {
        return invalid();
    }
    }

    dst->reserve(capacity);
    auto ptr = dst->data();
    int32_t dssize = biginteger_from_chars(start - 1, first - 1, ptr, base) - ptr;
    dssize = __fasts_conditional_negate(sign, dssize);
    dst->set_ssize(dssize);
    result.ptr = first;
    return result;
}

template <typename Storage>
int basic_biginteger<Storage>::__compare_impl(const basic_biginteger *lhs,
                                              const basic_biginteger *rhs) {
    const auto lssize = lhs->get_ssize();
    const auto rssize = rhs->get_ssize();

    if (lssize != rssize) {
        return lssize < rssize ? -1 : 1;
    }

    const int ans = reverse_compare_n(lhs->data(), rhs->data(), __fasts_abs(lssize));
    return lssize < 0 ? -ans : ans;
}

template <typename Storage>
int basic_biginteger<Storage>::__compare_ui_impl(const basic_biginteger *lhs,
                                                 uint64_t rhs) {
    const auto lssize = lhs->get_ssize();

    if (lssize == 0) {
        return -(rhs != 0);
    }

    if (lssize == 1) {
        const auto lvalue = lhs->front();
        return (lvalue != rhs ? (lvalue < rhs ? -1 : 1) : 0);
    }

    return lssize < 0 ? -1 : 1;
}

template <typename Storage>
int basic_biginteger<Storage>::__compare_si_impl(const basic_biginteger *lhs,
                                                 int64_t rhs) {
    const auto lssize = lhs->get_ssize();
    const auto rssize = rhs != 0 ? (rhs < 0 ? -1 : 1) : 0;

    if (lssize != rssize) {
        return lssize < rssize ? -1 : 1;
    }

    if (lssize == 0) {
        return 0;
    }

    const auto lvalue = lhs->front();
    const auto rvalue = to_unsigned(rhs);

    const int ans = (lvalue != rvalue ? (lvalue < rvalue ? -1 : 1) : 0);
    return lssize < 0 ? -ans : ans;
}

template <typename Storage>
template <bool xsign>
void basic_biginteger<Storage>::__addsub_impl(basic_biginteger *dst,
                                              const basic_biginteger *lhs, uint64_t rhs) {
    int32_t lssize = lhs->get_ssize();
    if (lssize == 0) {
        dst->reserve(1);
        dst->front() = rhs;
        dst->set_ssize(__fasts_conditional_negate<int32_t>(xsign, rhs != 0));
        return;
    }

    uint32_t lusize = __fasts_abs(lssize);
    dst->reserve(lusize + 1);

    auto dp = dst->data();
    auto lp = lhs->data();
    int32_t dssize;

    using compare =
        std::conditional_t<xsign, std::less<uint64_t>, std::greater<uint64_t>>;

    if (compare{}(dssize, 0)) {
        auto cf = addc_1(dp, lp, lusize, rhs);
        dssize = __fasts_conditional_negate(xsign, lusize + cf);
        if (cf) {
            dp[lusize] = 1;
        }
    } else {
        if (lusize == 1 && dp[0] < rhs) {
            dp[0] = rhs - dp[0];
            dssize = __fasts_conditional_negate(xsign, 1);
        } else {
            (void)subc_1(dp, lp, lusize, rhs);
            dssize = __fasts_conditional_negate(!xsign, lusize - (dp[lusize - 1] == 0));
        }
    }

    dst->set_ssize(dssize);
}

template <typename Storage>
void basic_biginteger<Storage>::__ui_sub_impl(basic_biginteger *dst, uint64_t lhs,
                                              const basic_biginteger *rhs) {
    int32_t rssize = rhs->get_ssize();
    if (rssize == 0) {
        dst->reserve(1);
        dst->front() = lhs;
        dst->set_ssize(1);
        return;
    }

    uint32_t rusize = __fasts_abs(rssize);
    dst->reserve(rusize);

    auto dp = dst->data();
    auto rp = rhs->data();
    int32_t dssize;

    if (rssize < 0) {
        auto cf = addc_1(dp, rp, rusize, lhs);
        dssize = rusize + cf;
        if (cf) {
            dp[rusize] = 1;
        }
    } else {
        // lhs >= rhs
        if (rusize == 1 && lhs >= rp[0]) {
            dp[0] = lhs - rp[0];
            dssize = dp[0] != 0;
        }
        // lhs < rhs
        else {
            (void)subc_1(dp, rp, rusize, lhs);
            dssize = __fasts_conditional_negate(1, rusize - (dp[rusize - 1] == 0));
        }
    }

    dst->set_ssize(dssize);
}

template <typename Storage>
template <bool xsign>
void basic_biginteger<Storage>::__addsub_impl(basic_biginteger *dst,
                                              const basic_biginteger *lhs,
                                              const basic_biginteger *rhs) {
    int32_t lssize = lhs->get_ssize();
    int32_t rssize = __fasts_conditional_negate(xsign, rhs->get_ssize());
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
            dssize =
                __fasts_get_sign_mask(lssize) | biginteger_details::normalize(dp, lusize);
        } else {
            if (WJR_UNLIKELY(lusize == 0)) {
                dssize = 0;
            } else {
                auto ans = abs_subc_n(dp, lp, rp, rusize);
                dssize = ans == 0 ? 0 : (__fasts_get_sign_mask(lssize) ^ ans);
            }
        }
    } else {
        auto cf = addc_sz(dp, lp, lusize, rp, rusize);
        // seems can be optimized
        dssize = __fasts_get_sign_mask(lssize) | (lusize + cf);
        if (cf) {
            dp[lusize] = 1;
        }
    }

    dst->set_ssize(dssize);
}

template <typename Storage>
void basic_biginteger<Storage>::__mul_ui_impl(basic_biginteger *dst,
                                              const basic_biginteger *lhs, uint64_t rhs) {
    int32_t lssize = lhs->get_ssize();
    uint32_t lusize = __fasts_abs(lssize);

    if (lusize == 0 || rhs == 0) {
        dst->set_ssize(0);
        return;
    }

    dst->reserve(lusize + 1);

    auto dp = dst->data();
    auto lp = lhs->data();

    auto cf = mul_1(dp, lp, lusize, rhs);
    int32_t dssize = lssize + (cf != 0);
    if (cf != 0) {
        dp[lusize] = cf;
    }

    dst->set_ssize(dssize);
}

template <typename Storage>
void basic_biginteger<Storage>::__mul_impl(basic_biginteger *dst,
                                           const basic_biginteger *lhs,
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
    uint32_t dusize;

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

    dusize = lusize + rusize;

    auto dp = dst->data();
    auto lp = (pointer)lhs->data();
    auto rp = (pointer)rhs->data();

    std::optional<uninitialized<basic_biginteger>> tmp;

    unique_stack_allocator stkal(math_details::stack_alloc);

    if (dst->capacity() < to_unsigned(dusize)) {
        tmp.emplace(dst->get_growth_capacity(dst->capacity(), dusize), in_place_reserve,
                    dst->get_allocator());
        dp = (**tmp).data();
    } else {
        if (dp == lp) {
            lp = (pointer)stkal.allocate(lusize * sizeof(uint64_t));
            if (dp == rp) {
                rp = lp;
            }
            std::copy_n(dp, lusize, lp);
        } else if (dp == rp) {
            rp = (pointer)stkal.allocate(rusize * sizeof(uint64_t));
            std::copy_n(dp, rusize, rp);
        }
    }

    if (WJR_UNLIKELY(lp == rp)) {
        sqr(dp, lp, lusize);
    } else {
        mul_s(dp, lp, lusize, rp, rusize);
    }

    bool cf = dp[dusize - 1] == 0;
    dssize = mask | (dusize - cf);

    if (tmp.has_value()) {
        *dst = **std::move(tmp);
    }

    dst->set_ssize(dssize);
}

template <typename Storage>
void basic_biginteger<Storage>::__addsubmul_ui_impl(basic_biginteger *dst,
                                                    const basic_biginteger *lhs,
                                                    uint64_t rhs, bool xsign) {
    int32_t lssize = lhs->get_ssize();

    if (lssize == 0 || rhs == 0) {
        return;
    }

    uint32_t lusize = __fasts_abs(lssize);
    lssize = __fasts_conditional_negate(xsign, lssize);
    int32_t dssize = dst->get_ssize();

    auto dp = dst->data();
    auto lp = lhs->data();

    if (dssize == 0) {
        dst->reserve(lusize + 1);
        auto cf = mul_1(dp, lp, lusize, rhs);
        dssize = lssize + (cf != 0);
        if (cf != 0) {
            dp[lusize] = cf;
        }

        dst->set_ssize(dssize);
        return;
    }

    uint32_t dusize = __fasts_abs(dssize);
}

template <typename S>
from_chars_result<> from_chars(const char *first, const char *last,
                               basic_biginteger<S> &dst, unsigned int base) {
    return basic_biginteger<S>::__from_chars_impl(first, last, &dst, base);
}

template <typename S, typename Iter>
Iter to_chars_unchecked(Iter ptr, const basic_biginteger<S> &src, unsigned int base) {
    if (src.empty()) {
        *ptr++ = '0';
        return ptr;
    }

    if (src.is_negate()) {
        *ptr++ = '-';
    }

    return biginteger_to_chars(ptr, src.data(), src.size(), base);
}

#define WJR_REGISTER_BIGINTEGER_COMPARE(op)                                              \
    template <typename S>                                                                \
    WJR_PURE bool operator op(const basic_biginteger<S> &lhs,                            \
                              const basic_biginteger<S> &rhs) {                          \
        return basic_biginteger<S>::__compare_impl(&lhs, &rhs) op 0;                     \
    }                                                                                    \
    template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>          \
    WJR_PURE bool operator op(const basic_biginteger<S> &lhs, T rhs) {                   \
        return basic_biginteger<S>::__compare_impl(&lhs, rhs) op 0;                      \
    }                                                                                    \
    template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>          \
    WJR_PURE bool operator op(T lhs, const basic_biginteger<S> &rhs) {                   \
        return basic_biginteger<S>::__compare_impl(lhs, &rhs) op 0;                      \
    }

WJR_REGISTER_BIGINTEGER_COMPARE(==)
WJR_REGISTER_BIGINTEGER_COMPARE(!=)
WJR_REGISTER_BIGINTEGER_COMPARE(<)
WJR_REGISTER_BIGINTEGER_COMPARE(>)
WJR_REGISTER_BIGINTEGER_COMPARE(<=)
WJR_REGISTER_BIGINTEGER_COMPARE(>=)

#undef WJR_REGISTER_BIGINTEGER_COMPARE

#define WJR_REGISTER_BIGINTEGER_ADDSUB(ADDSUB)                                           \
    template <typename S>                                                                \
    void ADDSUB(basic_biginteger<S> &dst, const basic_biginteger<S> &lhs,                \
                const basic_biginteger<S> &rhs) {                                        \
        basic_biginteger<S>::WJR_PP_CONCAT(__, WJR_PP_CONCAT(ADDSUB, _impl))(&dst, &lhs, \
                                                                             &rhs);      \
    }                                                                                    \
    template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>          \
    void ADDSUB(basic_biginteger<S> &dst, const basic_biginteger<S> &lhs, T rhs) {       \
        basic_biginteger<S>::WJR_PP_CONCAT(__, WJR_PP_CONCAT(ADDSUB, _impl))(&dst, &lhs, \
                                                                             rhs);       \
    }                                                                                    \
    template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>          \
    void ADDSUB(basic_biginteger<S> &dst, T lhs, const basic_biginteger<S> &rhs) {       \
        basic_biginteger<S>::WJR_PP_CONCAT(__, WJR_PP_CONCAT(ADDSUB, _impl))(&dst, lhs,  \
                                                                             &rhs);      \
    }

WJR_REGISTER_BIGINTEGER_ADDSUB(add)
WJR_REGISTER_BIGINTEGER_ADDSUB(sub)

#undef WJR_REGISTER_BIGINTEGER_ADDSUB

template <typename S>
void mul(basic_biginteger<S> &dst, const basic_biginteger<S> &lhs,
         const basic_biginteger<S> &rhs) {
    basic_biginteger<S>::__mul_impl(&dst, &lhs, &rhs);
}

template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
void mul(basic_biginteger<S> &dst, const basic_biginteger<S> &lhs, T rhs) {
    basic_biginteger<S>::__mul_impl(&dst, &lhs, rhs);
}

template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
void mul(basic_biginteger<S> &dst, T lhs, const basic_biginteger<S> &rhs) {
    basic_biginteger<S>::__mul_impl(&dst, lhs, &rhs);
}

template <typename S>
std::istream &operator>>(std::istream &is, basic_biginteger<S> &dst) {
    std::string str;
    is >> str;
    from_chars(str.data(), str.data() + str.size(), dst);
    return is;
}

template <typename S>
std::ostream &operator<<(std::ostream &os, const basic_biginteger<S> &src) {
    std::ios_base::iostate state = std::ios::goodbit;

    if (const std::ostream::sentry ok(os); ok) {
        unique_stack_allocator stkal(math_details::stack_alloc);

        vector<char, math_details::weak_stack_alloc<char>> buffer(stkal);
        buffer.reserve(512);

        const std::ios_base::fmtflags flags = os.flags();

        if ((flags & std::ios::showpos) && !src.is_negate()) {
            buffer.push_back('+');
        }

        int base = 10;

        if (const auto basefield = flags & std::ios::basefield; basefield != 0) {
            if (basefield == std::ios::oct) {
                base = 8;
                if (flags & std::ios::showbase) {
                    buffer.append({'0'});
                }
            } else if (basefield == std::ios::hex) {
                base = 16;
                if (flags & std::ios::showbase) {
                    buffer.append({'0', 'x'});
                }
            }
        }

        (void)to_chars_unchecked(std::back_inserter(buffer), src, base);

        if (!buffer.empty()) {
            __ostream_insert_unchecked(os, buffer.data(), buffer.size());
        }
    }

    os.setstate(state);
    return os;
}

} // namespace wjr

#endif