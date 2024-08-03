#ifndef WJR_BIGINTEGER_BIGINTEGER_HPP__
#define WJR_BIGINTEGER_BIGINTEGER_HPP__

#include <istream>
#include <optional>

#include <wjr/format/ostream_insert.hpp>
#include <wjr/math.hpp>
#include <wjr/span.hpp>
#include <wjr/vector.hpp>

namespace wjr {

namespace biginteger_detail {

/**
 * @brief Remove leading zeros.
 *
 */
inline uint32_t normalize(const uint64_t *ptr, uint32_t n) noexcept {
    return reverse_find_not_n(ptr, 0, n);
}

} // namespace biginteger_detail

class default_biginteger_size_reference {
public:
    default_biginteger_size_reference() = delete;
    default_biginteger_size_reference(const default_biginteger_size_reference &) = delete;
    default_biginteger_size_reference(default_biginteger_size_reference &&) = default;
    default_biginteger_size_reference &
    operator=(const default_biginteger_size_reference &) = delete;
    default_biginteger_size_reference &
    operator=(default_biginteger_size_reference &&) = default;
    ~default_biginteger_size_reference() = default;

    constexpr explicit default_biginteger_size_reference(int32_t &size) noexcept
        : m_size(&size) {}

    constexpr default_biginteger_size_reference &operator=(uint32_t size) noexcept {
        *m_size = __fasts_negate_with<int32_t>(*m_size, size);
        return *this;
    }

    WJR_PURE constexpr operator uint32_t() const noexcept { return __fasts_abs(*m_size); }

    constexpr default_biginteger_size_reference &operator++() noexcept {
        *m_size = __fasts_increment(*m_size);
        return *this;
    }

    constexpr default_biginteger_size_reference &operator--() noexcept {
        *m_size = __fasts_decrement(*m_size);
        return *this;
    }

    constexpr default_biginteger_size_reference &operator+=(uint32_t size) noexcept {
        *m_size = __fasts_add(*m_size, size);
        return *this;
    }

    constexpr default_biginteger_size_reference &operator-=(uint32_t size) noexcept {
        *m_size = __fasts_sub(*m_size, size);
        return *this;
    }

private:
    int32_t *m_size;
};

/// @private
template <>
struct __unref_wrapper_helper<default_biginteger_size_reference> {
    using type = uint32_t &;
};

/**
 * @brief data_type of biginteger
 *
 * @details View the data of biginteger. Used for type erasure. Manage memory allocation
 * and release on your own.
 *
 */
struct biginteger_data {
    WJR_PURE constexpr const uint64_t *data() const noexcept { return m_data; }
    WJR_PURE constexpr uint32_t size() const noexcept { return __fasts_abs(m_size); }

    WJR_PURE constexpr bool empty() const noexcept { return m_size == 0; }
    WJR_PURE constexpr bool is_negate() const noexcept { return m_size < 0; }

    WJR_PURE constexpr int32_t get_ssize() const noexcept { return m_size; }

    uint64_t *m_data = nullptr;
    int32_t m_size = 0;
    uint32_t m_capacity = 0;
};

/**
 * @struct default_biginteger_data
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

    default_biginteger_vector_storage() = default;

    default_biginteger_vector_storage(const default_biginteger_vector_storage &) = delete;
    default_biginteger_vector_storage(default_biginteger_vector_storage &&) noexcept =
        delete;
    default_biginteger_vector_storage &
    operator=(const default_biginteger_vector_storage &) = delete;
    default_biginteger_vector_storage &
    operator=(default_biginteger_vector_storage &&) = delete;

    ~default_biginteger_vector_storage() = default;

    void destroy(_Alty &al) noexcept {
        if (WJR_BUILTIN_CONSTANT_P_TRUE(data() == nullptr)) {
            return;
        }

        const size_type __size = size();

        if (WJR_BUILTIN_CONSTANT_P_TRUE(__size == 0)) {
            return;
        }

        destroy_n_using_allocator(data(), __size, al);
    }

    void destroy_and_deallocate(_Alty &al) noexcept {
        if (WJR_BUILTIN_CONSTANT_P_TRUE(capacity() == 0)) {
            return;
        }

        if (data()) {
            WJR_ASSERT_ASSUME_L2(capacity() != 0);

            destroy(al);
            al.deallocate(data(), capacity());
        }
    }

    void uninitialized_construct(
        default_biginteger_vector_storage &other, size_type size, size_type capacity,
        _Alty &al) noexcept(noexcept(allocate_at_least(al, capacity))) {
        const auto result = allocate_at_least(al, capacity);

        auto &storage = other.m_storage;
        storage.m_data = result.ptr;
        storage.m_size = __fasts_negate_with<int32_t>(m_storage.m_size, size);
        storage.m_capacity = result.count;
    }

    void take_storage(default_biginteger_vector_storage &other, _Alty &) noexcept {
        auto &other_storage = other.m_storage;
        m_storage = other_storage;
        other_storage.m_data = nullptr;
        other_storage.m_size = other_storage.m_capacity = 0;
    }

    void swap_storage(default_biginteger_vector_storage &other, _Alty &) noexcept {
        std::swap(m_storage, other.m_storage);
    }

    WJR_PURE default_biginteger_size_reference size() noexcept {
        return default_biginteger_size_reference(m_storage.m_size);
    }

    WJR_PURE size_type size() const noexcept { return __fasts_abs(m_storage.m_size); }
    WJR_PURE size_type capacity() const noexcept { return m_storage.m_capacity; }

    WJR_PURE pointer data() noexcept { return m_storage.m_data; }
    WJR_PURE const_pointer data() const noexcept { return m_storage.m_data; }

    // extension

    WJR_PURE int32_t get_ssize() const noexcept { return m_storage.m_size; }

    template <typename T>
    void set_ssize(T size) = delete;

    void set_ssize(int32_t size) noexcept {
        WJR_ASSERT_ASSUME(__fasts_abs(size) <= capacity());
        m_storage.m_size = size;
    }

    WJR_PURE const biginteger_data *__get_data() const noexcept { return &m_storage; }

private:
    biginteger_data m_storage;
};

template <typename Storage>
class basic_biginteger;

WJR_PURE WJR_INTRINSIC_CONSTEXPR biginteger_data
make_biginteger_data(span<const uint64_t> sp) noexcept {
    return biginteger_data{const_cast<uint64_t *>(sp.data()),
                           static_cast<int32_t>(sp.size()), 0};
}

namespace biginteger_detail {

// const basic_biginteger<Storage>* don't need to get allocator
// use const Storage* instead of const basic_biginteger<Storage>*

/// @private
template <typename S>
WJR_PURE bool __equal_pointer(const basic_biginteger<S> *lhs,
                              const basic_biginteger<S> *rhs) noexcept {
    return lhs == rhs;
}

/// @private
template <typename S0, typename S1>
WJR_PURE bool __equal_pointer(const basic_biginteger<S0> *,
                              const basic_biginteger<S1> *) noexcept {
    return false;
}

/// @private
template <typename S>
WJR_PURE bool __equal_pointer(const basic_biginteger<S> *lhs,
                              const biginteger_data *rhs) noexcept {
    return lhs->__get_data() == rhs;
}

/// @private
template <typename S>
WJR_PURE bool __equal_pointer(const biginteger_data *lhs,
                              const basic_biginteger<S> *rhs) noexcept {
    return lhs == rhs->__get_data();
}

/// @private
WJR_PURE inline bool __equal_pointer(const biginteger_data *lhs,
                                     const biginteger_data *rhs) noexcept {
    return lhs == rhs;
}

/// @private
template <typename S>
from_chars_result<const char *> __from_chars_impl(const char *first, const char *last,
                                                  basic_biginteger<S> *dst,
                                                  unsigned int base) noexcept;

/// @private
WJR_PURE inline int32_t __compare_impl(const biginteger_data *lhs,
                                       const biginteger_data *rhs) noexcept;

/// @private
WJR_PURE inline int32_t __compare_ui_impl(const biginteger_data *lhs,
                                          uint64_t rhs) noexcept;

/// @private
WJR_PURE inline int32_t __compare_si_impl(const biginteger_data *lhs,
                                          int64_t rhs) noexcept;

/// @private
template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
WJR_PURE int32_t __compare_impl(const biginteger_data *lhs, T rhs) noexcept {
    if (WJR_BUILTIN_CONSTANT_P_TRUE(rhs == 0)) {
        const int32_t ssize = lhs->get_ssize();
        return ssize == 0 ? 0 : ssize < 0 ? -1 : 1;
    }

    if constexpr (std::is_unsigned_v<T>) {
        return __compare_ui_impl(lhs, rhs);
    } else {
        if (WJR_BUILTIN_CONSTANT_P_TRUE(rhs >= 0)) {
            return __compare_ui_impl(lhs, to_unsigned(rhs));
        }

        return __compare_si_impl(lhs, rhs);
    }
}

/// @private
template <bool xsign, typename S>
void __addsub_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                   uint64_t rhs) noexcept;

/// @private
template <typename S>
void __ui_sub_impl(basic_biginteger<S> *dst, uint64_t lhs,
                   const biginteger_data *rhs) noexcept;

/// @private
template <bool xsign, typename S>
void __addsub_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                   const biginteger_data *rhs) noexcept;

/// @private
template <typename S>
void __add_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                const biginteger_data *rhs) noexcept {
    __addsub_impl<false>(dst, lhs, rhs);
}

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void __add_impl(basic_biginteger<S> *dst, const biginteger_data *lhs, T rhs) noexcept {
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

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void __add_impl(basic_biginteger<S> *dst, T lhs, const biginteger_data *rhs) noexcept {
    __add_impl(dst, rhs, lhs);
}

/// @private
template <typename S>
void __sub_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                const biginteger_data *rhs) noexcept {
    __addsub_impl<true>(dst, lhs, rhs);
}

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void __sub_impl(basic_biginteger<S> *dst, const biginteger_data *lhs, T rhs) noexcept {
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

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void __sub_impl(basic_biginteger<S> *dst, T lhs, const biginteger_data *rhs) noexcept {
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

/// @private
template <typename S>
void __mul_ui_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                   uint64_t rhs) noexcept;

/// @private
template <typename S>
void __mul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                const biginteger_data *rhs) noexcept;

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void __mul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs, T rhs) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        __mul_ui_impl(dst, lhs, rhs);
    } else {
        uint64_t value = to_unsigned(rhs);
        bool cond = false;

        if (rhs < 0) {
            value = -value;
            cond = true;
        }

        __mul_ui_impl(dst, lhs, value);
        dst->conditional_negate(cond);
    }
}

/// @private
template <typename S>
void __sqr_impl(basic_biginteger<S> *dst, const biginteger_data *src) noexcept;

/// @private
template <typename S>
void __addsubmul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs, uint64_t rhs,
                      int32_t xmask) noexcept;

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void __addmul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs, T rhs) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        __addsubmul_impl(dst, lhs, rhs, 0);
    } else {
        uint64_t rvalue = to_unsigned(rhs);
        int32_t xsign = 0;

        if (rhs < 0) {
            rvalue = -rvalue;
            xsign = -1;
        }

        __addsubmul_impl(dst, lhs, rvalue, xsign);
    }
}

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void __submul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs, T rhs) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        __addsubmul_impl(dst, lhs, rhs, -1);
    } else {
        uint64_t rvalue = to_unsigned(rhs);
        int32_t xsign = -1;

        if (rhs < 0) {
            rvalue = -rvalue;
            xsign = 0;
        }

        __addsubmul_impl(dst, lhs, rvalue, xsign);
    }
}

/// @private
template <typename S>
void __addsubmul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                      const biginteger_data *rhs, int32_t xmask) noexcept;

/// @private
template <typename S>
void __addmul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                   const biginteger_data *rhs) noexcept {
    __addsubmul_impl(dst, lhs, rhs, 0);
}

/// @private
template <typename S>
void __submul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                   const biginteger_data *rhs) noexcept {
    __addsubmul_impl(dst, lhs, rhs, -1);
}

/// @private
template <typename S>
void __mul_2exp_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                     uint32_t shift) noexcept;

/// @private
template <typename S0, typename S1>
void __tdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                    const biginteger_data *num, const biginteger_data *div) noexcept;

/// @private
template <typename S>
void __tdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                   const biginteger_data *div) noexcept;

/// @private
template <typename S>
void __tdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                   const biginteger_data *div) noexcept;

/// @private
template <typename S0, typename S1>
uint64_t __tdiv_qr_ui_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                           const biginteger_data *num, uint64_t div) noexcept;

/// @private
template <typename S>
uint64_t __tdiv_q_ui_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                          uint64_t div) noexcept;

/// @private
template <typename S>
uint64_t __tdiv_r_ui_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                          uint64_t div) noexcept;

/// @private
template <typename S0, typename S1, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __tdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                        const biginteger_data *num, T div) noexcept;

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __tdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                       T div) noexcept;

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __tdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                       uint64_t div) noexcept;

/// @private
template <typename S0, typename S1>
void __fdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                    const biginteger_data *num, const biginteger_data *div) noexcept;

/// @private
template <typename S>
void __fdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                   const biginteger_data *div) noexcept;

/// @private
template <typename S>
void __fdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                   const biginteger_data *div) noexcept;

/// @private
template <typename S0, typename S1, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __fdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                        const biginteger_data *num, T div) noexcept;

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __fdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                       T div) noexcept;

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __fdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                       uint64_t div) noexcept;

/// @private
template <typename S0, typename S1>
void __cdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                    const biginteger_data *num, const biginteger_data *div) noexcept;

/// @private
template <typename S>
void __cdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                   const biginteger_data *div) noexcept;

/// @private
template <typename S>
void __cdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                   const biginteger_data *div) noexcept;

/// @private
template <typename S0, typename S1, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __cdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                        const biginteger_data *num, T div) noexcept;

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __cdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                       T div) noexcept;

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __cdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                       uint64_t div) noexcept;

/// @private
template <typename S>
void __tdiv_q_2exp_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                        uint32_t shift) noexcept;

/// @private
template <typename S>
void __tdiv_r_2exp_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                        uint32_t shift) noexcept;

/// @private
template <typename S>
void __cfdiv_q_2exp_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                         uint32_t shift, int32_t xdir) noexcept;

/// @private
template <typename S>
void __cdiv_q_2exp_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                        uint32_t shift) noexcept {
    __cfdiv_q_2exp_impl(quot, num, shift, 1);
}

/// @private
template <typename S>
void __fdiv_q_2exp_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                        uint32_t shift) noexcept {
    __cfdiv_q_2exp_impl(quot, num, shift, -1);
}

/// @private
template <typename S>
void __cfdiv_r_2exp_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                         uint32_t shift, int32_t xdir) noexcept;

/// @private
template <typename S>
void __cdiv_r_2exp_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                        uint32_t shift) noexcept {
    __cfdiv_r_2exp_impl(rem, num, shift, 1);
}

/// @private
template <typename S>
void __fdiv_r_2exp_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                        uint32_t shift) noexcept {
    __cfdiv_r_2exp_impl(rem, num, shift, -1);
}

/// @private
template <typename S, typename Engine,
          WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_bit_impl(basic_biginteger<S> *dst, uint32_t size, Engine &engine) noexcept;

/// @private
template <typename S, typename Engine,
          WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_exact_bit_impl(basic_biginteger<S> *dst, uint32_t size,
                              Engine &engine) noexcept;

/// @private
template <typename S, typename Engine,
          WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_impl(basic_biginteger<S> *dst, const biginteger_data *limit,
                    Engine &engine) noexcept;

/// @private
template <typename S, typename Engine,
          WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_exact_impl(basic_biginteger<S> *dst, const biginteger_data *limit,
                          Engine &engine) noexcept;

/// @private
WJR_PURE inline uint32_t __bit_width_impl(const biginteger_data *num) noexcept;

/// @private
WJR_PURE inline uint32_t __ctz_impl(const biginteger_data *num) noexcept;

/// @todo optimize
template <typename S>
void __pow_impl(basic_biginteger<S> *dst, const biginteger_data *num,
                uint32_t exp) noexcept;

/// @private
struct __powmod_iterator {
    __powmod_iterator(const uint64_t *ptr_, uint32_t size_) noexcept
        : ptr(ptr_), cache(ptr[0]), size(size_) {}

    constexpr void next() noexcept {
        if (++offset == 64) {
            offset = 0;
            ++pos;
            WJR_ASSERT(pos != size);
            cache = ptr[pos];
        }
    }

    constexpr uint64_t get() const noexcept { return (cache >> offset); }
    constexpr bool end() const noexcept { return pos == size - 1 && get() == 1; }

    const uint64_t *ptr;
    uint64_t cache;
    uint32_t offset = 0;
    uint32_t pos = 0;
    uint32_t size;
};

/// @todo optimize
template <typename S>
void __powmod_impl(basic_biginteger<S> *dst, const biginteger_data *num,
                   __powmod_iterator *iter, const biginteger_data *mod) noexcept;

/// @todo optimize
template <typename S>
void __powmod_impl(basic_biginteger<S> *dst, const biginteger_data *num,
                   const biginteger_data *exp, const biginteger_data *mod) noexcept;

/// @todo optimize
template <typename S>
void __powmod_impl(basic_biginteger<S> *dst, const biginteger_data *num, uint64_t exp,
                   const biginteger_data *mod) noexcept;

} // namespace biginteger_detail

template <typename S>
from_chars_result<const char *> from_chars(const char *first, const char *last,
                                           basic_biginteger<S> &dst,
                                           unsigned int base = 10) noexcept;

template <typename Iter>
Iter to_chars_unchecked(Iter ptr, const biginteger_data &src,
                        unsigned int base = 10) noexcept {
    if (src.empty()) {
        *ptr++ = '0';
        return ptr;
    }

    if (src.is_negate()) {
        *ptr++ = '-';
    }

    return biginteger_to_chars(ptr, src.data(), src.size(), base);
}

template <typename S>
std::istream &operator>>(std::istream &is, basic_biginteger<S> &dst) noexcept;

template <typename Traits>
std::basic_ostream<char, Traits> &operator<<(std::basic_ostream<char, Traits> &os,
                                             const biginteger_data &src) noexcept;

WJR_NODISCARD WJR_PURE inline int32_t compare(const biginteger_data &lhs,
                                              const biginteger_data &rhs) noexcept {
    return biginteger_detail::__compare_impl(&lhs, &rhs);
}

template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
WJR_NODISCARD WJR_PURE int32_t compare(const biginteger_data &lhs, T rhs) noexcept {
    return biginteger_detail::__compare_impl(&lhs, rhs);
}

template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
WJR_NODISCARD WJR_PURE int32_t compare(T lhs, const biginteger_data &rhs) noexcept {
    return -compare(rhs, lhs);
}

#define WJR_REGISTER_BIGINTEGER_COMPARE(op)                                              \
    WJR_NODISCARD WJR_PURE inline bool operator op(                                      \
        const biginteger_data &lhs, const biginteger_data &rhs) noexcept {               \
        return compare(lhs, rhs) op 0;                                                   \
    }                                                                                    \
    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>                        \
    WJR_NODISCARD WJR_PURE bool operator op(const biginteger_data &lhs,                  \
                                            T rhs) noexcept {                            \
        return compare(lhs, rhs) op 0;                                                   \
    }                                                                                    \
    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>                        \
    WJR_NODISCARD WJR_PURE bool operator op(T lhs,                                       \
                                            const biginteger_data &rhs) noexcept {       \
        return compare(lhs, rhs) op 0;                                                   \
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
    void ADDSUB(basic_biginteger<S> &dst, const biginteger_data &lhs,                    \
                const biginteger_data &rhs) noexcept {                                   \
        biginteger_detail::WJR_PP_CONCAT(__, WJR_PP_CONCAT(ADDSUB, _impl))(&dst, &lhs,   \
                                                                           &rhs);        \
    }                                                                                    \
    template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>            \
    void ADDSUB(basic_biginteger<S> &dst, const biginteger_data &lhs, T rhs) noexcept {  \
        biginteger_detail::WJR_PP_CONCAT(__, WJR_PP_CONCAT(ADDSUB, _impl))(&dst, &lhs,   \
                                                                           rhs);         \
    }                                                                                    \
    template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>            \
    void ADDSUB(basic_biginteger<S> &dst, T lhs, const biginteger_data &rhs) noexcept {  \
        biginteger_detail::WJR_PP_CONCAT(__, WJR_PP_CONCAT(ADDSUB, _impl))(&dst, lhs,    \
                                                                           &rhs);        \
    }

WJR_REGISTER_BIGINTEGER_ADDSUB(add)
WJR_REGISTER_BIGINTEGER_ADDSUB(sub)

#undef WJR_REGISTER_BIGINTEGER_ADDSUB

template <typename S>
void increment(basic_biginteger<S> &dst) noexcept {
    add(dst, dst, 1u);
}

template <typename S>
void decrement(basic_biginteger<S> &dst) noexcept {
    sub(dst, dst, 1u);
}

template <typename S>
void negate(basic_biginteger<S> &dst) noexcept;

template <typename S>
void absolute(basic_biginteger<S> &dst) noexcept;

template <typename S>
void sqr(basic_biginteger<S> &dst, const biginteger_data &src) noexcept {
    biginteger_detail::__sqr_impl(&dst, &src);
}

template <typename S>
void mul(basic_biginteger<S> &dst, const biginteger_data &lhs,
         const biginteger_data &rhs) noexcept {
    if (WJR_BUILTIN_CONSTANT_P_TRUE(std::addressof(lhs) == std::addressof(rhs))) {
        sqr(dst, lhs);
        return;
    }

    biginteger_detail::__mul_impl(&dst, &lhs, &rhs);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void mul(basic_biginteger<S> &dst, const biginteger_data &lhs, T rhs) noexcept {
    biginteger_detail::__mul_impl(&dst, &lhs, rhs);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void mul(basic_biginteger<S> &dst, T lhs, const biginteger_data &rhs) noexcept {
    biginteger_detail::__mul_impl(&dst, &rhs, lhs);
}

template <typename S>
void addmul(basic_biginteger<S> &dst, const biginteger_data &lhs,
            const biginteger_data &rhs) noexcept {
    biginteger_detail::__addmul_impl(&dst, &lhs, &rhs);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void addmul(basic_biginteger<S> &dst, const biginteger_data &lhs, T rhs) noexcept {
    biginteger_detail::__addmul_impl(&dst, &lhs, rhs);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void addmul(basic_biginteger<S> &dst, T lhs, const biginteger_data &rhs) noexcept {
    biginteger_detail::__addmul_impl(&dst, &rhs, lhs);
}

template <typename S>
void submul(basic_biginteger<S> &dst, const biginteger_data &lhs,
            const biginteger_data &rhs) noexcept {
    biginteger_detail::__submul_impl(&dst, &lhs, &rhs);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void submul(basic_biginteger<S> &dst, const biginteger_data &lhs, T rhs) noexcept {
    biginteger_detail::__submul_impl(&dst, &lhs, rhs);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void submul(basic_biginteger<S> &dst, T lhs, const biginteger_data &rhs) noexcept {
    biginteger_detail::__submul_impl(&dst, &rhs, lhs);
}

template <typename S>
void mul_2exp(basic_biginteger<S> &dst, const biginteger_data &lhs,
              uint32_t shift) noexcept {
    biginteger_detail::__mul_2exp_impl(&dst, &lhs, shift);
}

template <typename S0, typename S1>
void tdiv_qr(basic_biginteger<S0> &quot, basic_biginteger<S1> &rem,
             const biginteger_data &num, const biginteger_data &div) noexcept {
    biginteger_detail::__tdiv_qr_impl(&quot, &rem, &num, &div);
}

template <typename S>
void tdiv_q(basic_biginteger<S> &quot, const biginteger_data &num,
            const biginteger_data &div) noexcept {
    biginteger_detail::__tdiv_q_impl(&quot, &num, &div);
}

template <typename S0>
void tdiv_r(basic_biginteger<S0> &rem, const biginteger_data &num,
            const biginteger_data &div) noexcept {
    biginteger_detail::__tdiv_r_impl(&rem, &num, &div);
}

template <typename S0, typename S1, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t tdiv_qr(basic_biginteger<S0> &quot, basic_biginteger<S1> &rem,
                 const biginteger_data &num, T div) noexcept {
    return biginteger_detail::__tdiv_qr_impl(&quot, &rem, &num, div);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t tdiv_q(basic_biginteger<S> &quot, const biginteger_data &num, T div) noexcept {
    return biginteger_detail::__tdiv_q_impl(&quot, &num, div);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t tdiv_r(basic_biginteger<S> &rem, const biginteger_data &num, T div) noexcept {
    return biginteger_detail::__tdiv_r_impl(&rem, &num, div);
}

template <typename S0, typename S1>
void fdiv_qr(basic_biginteger<S0> &quot, basic_biginteger<S1> &rem,
             const biginteger_data &num, const biginteger_data &div) noexcept {
    biginteger_detail::__fdiv_qr_impl(&quot, &rem, &num, &div);
}

template <typename S>
void fdiv_q(basic_biginteger<S> &quot, const biginteger_data &num,
            const biginteger_data &div) noexcept {
    biginteger_detail::__fdiv_q_impl(&quot, &num, &div);
}

template <typename S0>
void fdiv_r(basic_biginteger<S0> &rem, const biginteger_data &num,
            const biginteger_data &div) noexcept {
    biginteger_detail::__fdiv_r_impl(&rem, &num, &div);
}

template <typename S0, typename S1, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t fdiv_qr(basic_biginteger<S0> &quot, basic_biginteger<S1> &rem,
                 const biginteger_data &num, T div) noexcept {
    return biginteger_detail::__fdiv_qr_impl(&quot, &rem, &num, div);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t fdiv_q(basic_biginteger<S> &quot, const biginteger_data &num, T div) noexcept {
    return biginteger_detail::__fdiv_q_impl(&quot, &num, div);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t fdiv_r(basic_biginteger<S> &rem, const biginteger_data &num, T div) noexcept {
    return biginteger_detail::__fdiv_r_impl(&rem, &num, div);
}

template <typename S0, typename S1>
void cdiv_qr(basic_biginteger<S0> &quot, basic_biginteger<S1> &rem,
             const biginteger_data &num, const biginteger_data &div) noexcept {
    biginteger_detail::__cdiv_qr_impl(&quot, &rem, &num, &div);
}

template <typename S>
void cdiv_q(basic_biginteger<S> &quot, const biginteger_data &num,
            const biginteger_data &div) noexcept {
    biginteger_detail::__cdiv_q_impl(&quot, &num, &div);
}

template <typename S0>
void cdiv_r(basic_biginteger<S0> &rem, const biginteger_data &num,
            const biginteger_data &div) noexcept {
    biginteger_detail::__cdiv_r_impl(&rem, &num, &div);
}

template <typename S0, typename S1, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t cdiv_qr(basic_biginteger<S0> &quot, basic_biginteger<S1> &rem,
                 const biginteger_data &num, T div) noexcept {
    return biginteger_detail::__cdiv_qr_impl(&quot, &rem, &num, div);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t cdiv_q(basic_biginteger<S> &quot, const biginteger_data &num, T div) noexcept {
    return biginteger_detail::__cdiv_q_impl(&quot, &num, div);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t cdiv_r(basic_biginteger<S> &rem, const biginteger_data &num, T div) noexcept {
    return biginteger_detail::__cdiv_r_impl(&rem, &num, div);
}

template <typename S>
void tdiv_q_2exp(basic_biginteger<S> &quot, const biginteger_data &num,
                 uint32_t shift) noexcept {
    biginteger_detail::__tdiv_q_2exp_impl(&quot, &num, shift);
}

template <typename S>
void tdiv_r_2exp(basic_biginteger<S> &rem, const biginteger_data &num,
                 uint32_t shift) noexcept {
    biginteger_detail::__tdiv_r_2exp_impl(&rem, &num, shift);
}

template <typename S>
void fdiv_q_2exp(basic_biginteger<S> &quot, const biginteger_data &num,
                 uint32_t shift) noexcept {
    biginteger_detail::__fdiv_q_2exp_impl(&quot, &num, shift);
}

template <typename S>
void cdiv_q_2exp(basic_biginteger<S> &quot, const biginteger_data &num,
                 uint32_t shift) noexcept {
    biginteger_detail::__cdiv_q_2exp_impl(&quot, &num, shift);
}

template <typename S>
void cdiv_r_2exp(basic_biginteger<S> &rem, const biginteger_data &num,
                 uint32_t shift) noexcept {
    biginteger_detail::__cdiv_r_2exp_impl(&rem, &num, shift);
}

template <typename S>
void fdiv_r_2exp(basic_biginteger<S> &rem, const biginteger_data &num,
                 uint32_t shift) noexcept {
    biginteger_detail::__fdiv_r_2exp_impl(&rem, &num, shift);
}

template <typename S, typename Engine,
          WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void urandom_bit(basic_biginteger<S> &dst, uint32_t size, Engine &engine) noexcept {
    biginteger_detail::__urandom_bit_impl(&dst, size, engine);
}

template <typename S, typename Engine,
          WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void urandom_exact_bit(basic_biginteger<S> &dst, uint32_t size, Engine &engine) noexcept {
    biginteger_detail::__urandom_exact_bit_impl(&dst, size, engine);
}

template <typename S, typename Engine,
          WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void urandom(basic_biginteger<S> &dst, const biginteger_data &limit,
             Engine &engine) noexcept {
    biginteger_detail::__urandom_impl(&dst, &limit, engine);
}

template <typename S, typename Engine,
          WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void urandom_exact(basic_biginteger<S> &dst, const biginteger_data &limit,
                   Engine &engine) noexcept {
    biginteger_detail::__urandom_exact_impl(&dst, &limit, engine);
}

inline uint32_t bit_width(const biginteger_data &num) noexcept {
    return biginteger_detail::__bit_width_impl(&num);
}

inline uint32_t ctz(const biginteger_data &num) noexcept {
    return biginteger_detail::__ctz_impl(&num);
}

template <typename S>
void pow(basic_biginteger<S> &dst, const biginteger_data &num, uint32_t exp) noexcept {
    biginteger_detail::__pow_impl(&dst, &num, exp);
}

/**
 * @todo Need to optimize
 */
template <typename S>
void powmod(basic_biginteger<S> &dst, const biginteger_data &num,
            const biginteger_data &exp, const biginteger_data &mod) noexcept {
    biginteger_detail::__powmod_impl(&dst, &num, &exp, &mod);
}

/**
 * @todo Need to optimize
 */
template <typename S>
void powmod(basic_biginteger<S> &dst, const biginteger_data &num, uint64_t exp,
            const biginteger_data &mod) noexcept {
    biginteger_detail::__powmod_impl(&dst, &num, exp, &mod);
}

template <typename Storage>
class basic_biginteger {

public:
    using storage_type = Storage;
    using vector_type = basic_vector<storage_type>;

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
    static_assert(std::is_same_v<pointer, uint64_t *>, "pointer must be uint64_t *");
    static_assert(std::is_same_v<const_pointer, const uint64_t *>,
                  "const_pointer must be const uint64_t *");
    static_assert(std::is_same_v<size_type, uint32_t>, "size_type must be uint32_t");
    static_assert(std::is_same_v<difference_type, int32_t>,
                  "difference_type must be int32_t");

    basic_biginteger() = default;
    basic_biginteger(basic_biginteger &&other) = default;
    basic_biginteger &operator=(basic_biginteger &&other) = default;
    ~basic_biginteger() = default;

    basic_biginteger(const basic_biginteger &other,
                     const allocator_type &al = allocator_type())
        : m_vec(other.m_vec, al) {
        set_ssize(other.get_ssize());
    }

    basic_biginteger(size_type n, in_place_reserve_t,
                     const allocator_type &al = allocator_type())
        : m_vec(n, in_place_reserve, al) {}

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
            set_ssize(__fasts_conditional_negate<int32_t>(value < 0, 1));
        }
    }

    explicit basic_biginteger(span<const char> sp, unsigned int base = 10,
                              const allocator_type &al = allocator_type())
        : m_vec(al) {
        from_string(sp, base);
    }

    template <typename OthterStorage>
    explicit basic_biginteger(const basic_biginteger<OthterStorage> &other,
                              const allocator_type &al = allocator_type())
        : m_vec(other.begin(), other.end(), al) {
        set_ssize(other.get_ssize());
    }

    explicit basic_biginteger(const biginteger_data &data,
                              const allocator_type &al = allocator_type())
        : m_vec(data.data(), data.data() + data.size(), al) {
        set_ssize(data.get_ssize());
    }

    basic_biginteger &operator=(const basic_biginteger &other) {
        m_vec = other.m_vec;
        set_ssize(other.get_ssize());
        return *this;
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
            set_ssize(__fasts_conditional_negate<int32_t>(value < 0, 1));
        }
        return *this;
    }

    basic_biginteger &operator=(span<const char> sp) { return from_string(sp); }

    template <typename OthterStorage>
    basic_biginteger &operator=(const basic_biginteger<OthterStorage> &other) {
        m_vec.assign(other.begin(), other.end());
        set_ssize(other.get_ssize());
        return *this;
    }

    basic_biginteger &operator=(const biginteger_data &data) {
        if (WJR_UNLIKELY(__get_data() == std::addressof(data))) {
            return *this;
        }

        m_vec.assign(data.data(), data.data() + data.size());
        set_ssize(data.get_ssize());
        return *this;
    }

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

    basic_biginteger &from_string(span<const char> sp, unsigned int base = 10) noexcept {
        (void)from_chars(sp.data(), sp.data() + sp.size(), *this, base);
        return *this;
    }

    allocator_type &get_allocator() noexcept { return m_vec.get_allocator(); }
    const allocator_type &get_allocator() const noexcept { return m_vec.get_allocator(); }

    WJR_PURE pointer data() noexcept { return m_vec.data(); }
    WJR_PURE const_pointer data() const noexcept { return m_vec.data(); }

    WJR_PURE bool empty() const noexcept { return m_vec.empty(); }
    WJR_PURE size_type size() const noexcept { return m_vec.size(); }
    WJR_PURE size_type capacity() const noexcept { return m_vec.capacity(); }
    WJR_PURE bool zero() const noexcept { return empty(); }

    void reserve(size_type new_capacity) noexcept { m_vec.reserve(new_capacity); }
    void clear_if_reserved(size_type new_capacity) noexcept {
        m_vec.clear_if_reserved(new_capacity);
    }

    void shrink_to_fit() { m_vec.shrink_to_fit(); }

    /// equal to set_ssize(0)
    void clear() { m_vec.clear(); }

    void swap(basic_biginteger &other) noexcept { m_vec.swap(other.m_vec); }

    void conditional_negate(bool condition) noexcept {
        set_ssize(__fasts_conditional_negate<int32_t>(condition, get_ssize()));
    }

    void negate() noexcept { conditional_negate(true); }

    WJR_PURE bool is_negate() const noexcept { return get_ssize() < 0; }

    void absolute() noexcept { set_ssize(__fasts_abs(get_ssize())); }

    basic_biginteger &operator++() {
        increment(*this);
        return *this;
    }

    basic_biginteger &operator--() {
        decrement(*this);
        return *this;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    basic_biginteger &operator+=(T rhs) {
        add(*this, *this, rhs);
        return *this;
    }

    basic_biginteger &operator+=(const biginteger_data &rhs) {
        add(*this, *this, rhs);
        return *this;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    basic_biginteger &operator-=(T rhs) {
        sub(*this, *this, rhs);
        return *this;
    }

    basic_biginteger &operator-=(const biginteger_data &rhs) {
        sub(*this, *this, rhs);
        return *this;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    basic_biginteger &operator*=(T rhs) {
        mul(*this, *this, rhs);
        return *this;
    }

    basic_biginteger &operator*=(const biginteger_data &rhs) {
        mul(*this, *this, rhs);
        return *this;
    }

    basic_biginteger &operator<<=(uint32_t shift) {
        mul_2exp(*this, *this, shift);
        return *this;
    }

    // extension

    reference at(size_type pos) { return m_vec.at(pos); }
    const_reference at(size_type pos) const { return m_vec.at(pos); }

    reference operator[](size_type pos) noexcept { return m_vec[pos]; }
    const_reference operator[](size_type pos) const noexcept { return m_vec[pos]; }

    reference front() { return m_vec.front(); }
    const_reference front() const { return m_vec.front(); }

    reference back() { return m_vec.back(); }
    const_reference back() const { return m_vec.back(); }

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

    WJR_PURE int32_t get_ssize() const { return get_storage().get_ssize(); }
    template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T> ||
                                       std::is_same_v<T, int32_t>)>
    void set_ssize(T new_size) noexcept {
        if constexpr (std::is_unsigned_v<T>) {
            const auto u32size = static_cast<uint32_t>(new_size);
            WJR_ASSUME(u32size == new_size);
            get_storage().set_ssize(__fasts_from_unsigned(u32size));
        } else {
            get_storage().set_ssize(new_size);
        }
    }

    WJR_CONST static size_type get_growth_capacity(size_type old_capacity,
                                                   size_type new_size) noexcept {
        return vector_type::get_growth_capacity(old_capacity, new_size);
    }

    void take_storage(storage_type &other) noexcept { m_vec.take_storage(other); }

    void uninitialized_construct(storage_type &other, size_type siz,
                                 size_type cap) noexcept {
        m_vec.uninitialized_construct(other, siz, cap);
    }

    WJR_PURE storage_type &get_storage() noexcept { return m_vec.get_storage(); }
    WJR_PURE const storage_type &get_storage() const noexcept {
        return m_vec.get_storage();
    }

    WJR_PURE const biginteger_data *__get_data() const noexcept {
        return get_storage().__get_data();
    }

    WJR_PURE const biginteger_data &__get_ref() const noexcept { return *__get_data(); }
    WJR_PURE operator const biginteger_data &() const noexcept { return __get_ref(); }

private:
    void __check_high_bit() const {
        WJR_ASSERT(size() == 0 || back() != 0, "biginteger should not have leading zero");
    }

    vector_type m_vec;
};

template <typename Alloc>
using default_biginteger = basic_biginteger<default_biginteger_vector_storage<Alloc>>;

using biginteger = default_biginteger<memory_pool<uint64_t>>;
using stack_biginteger = default_biginteger<math_detail::weak_stack_alloc<uint64_t>>;

template <typename Storage>
void swap(basic_biginteger<Storage> &lhs, basic_biginteger<Storage> &rhs) noexcept {
    lhs.swap(rhs);
}

namespace biginteger_detail {

template <typename S>
from_chars_result<const char *> __from_chars_impl(const char *first, const char *last,
                                                  basic_biginteger<S> *dst,
                                                  unsigned int base) noexcept {
    const auto *__first = first;

    do {
        if (WJR_UNLIKELY(first == last)) {
            break;
        }

        uint8_t ch;
        ch = *first;

        if (charconv_detail::isspace(ch)) {
            do {
                if (++first == last) {
                    break;
                }

                ch = *first;
            } while (charconv_detail::isspace(ch));
        }

        int sign = 0;
        if (ch == '-') {
            sign = 1;
            if (++first == last) {
                break;
            }

            ch = *first;
        }

        if (base == 0) {
            base = 10;
            if (ch == '0') {
                base = 8;
                if (++first == last) {
                    break;
                }

                ch = *first;
                if (ch == 'x' || ch == 'X') {
                    base = 16;
                    if (++first == last) {
                        break;
                    }

                    ch = *first;
                } else {
                    if (ch == 'b' || ch == 'B') {
                        base = 2;
                        if (++first == last) {
                            break;
                        }

                        ch = *first;
                    }
                }
            }
        }

        if (base <= 10) {
            const auto __try_match = [base](uint8_t &ch) {
                ch -= '0';
                return ch < base;
            };

            if (WJR_UNLIKELY(!__try_match(ch))) {
                break;
            }

            if (WJR_UNLIKELY(ch == 0)) {
                goto LOOP_HEAD_0;

                do {
                    ch = *first;
                    if (ch != '0') {
                        goto LOOP_END_0;
                    }

                LOOP_HEAD_0:
                    ++first;
                } while (first != last);

                dst->set_ssize(0);
                return {first, std::errc{}};
            LOOP_END_0:

                if (!__try_match(ch)) {
                    dst->set_ssize(0);
                    return {first, std::errc{}};
                }
            }

            __first = first;

            if (++first != last) {
                if (last - first >= 8) {
                    do {
                        if (!check_eight_digits<branch::free>(first, base)) {
                            break;
                        }

                        first += 8;
                    } while (last - first >= 8);
                }

                ch = *first;
                if (__try_match(ch)) {
                    do {
                        ++first;
                        ch = *first;
                    } while (__try_match(ch));
                }
            }
        } else {
            const auto __try_match = [base](uint8_t &ch) {
                ch = char_converter.from(ch);
                return ch < base;
            };

            if (WJR_UNLIKELY(!__try_match(ch))) {
                break;
            }

            if (WJR_UNLIKELY(ch == 0)) {
                goto LOOP_HEAD_1;

                do {
                    ch = *first;
                    if (ch != '0') {
                        goto LOOP_END_1;
                    }

                LOOP_HEAD_1:
                    ++first;
                } while (first != last);

                dst->clear();
                return {first, std::errc{}};
            LOOP_END_1:

                if (!__try_match(ch)) {
                    dst->clear();
                    return {first, std::errc{}};
                }
            }

            __first = first;

            do {
                ++first;
                if (first == last) {
                    break;
                }

                ch = *first;
            } while (__try_match(ch));
        }

        const size_t str_size = first - __first;
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
            WJR_UNREACHABLE();
            break;
        }
        }

        dst->clear_if_reserved(capacity);
        auto *const ptr = dst->data();
        int32_t dssize = biginteger_from_chars(__first, first, ptr, base) - ptr;
        dssize = __fasts_conditional_negate<int32_t>(sign, dssize);
        dst->set_ssize(dssize);
        return {first, std::errc{}};
    } while (0);

    dst->clear();
    return {__first, std::errc::invalid_argument};
}

inline int32_t __compare_impl(const biginteger_data *lhs,
                              const biginteger_data *rhs) noexcept {
    const auto lssize = lhs->get_ssize();
    const auto rssize = rhs->get_ssize();

    if (lssize != rssize) {
        return lssize < rssize ? -1 : 1;
    }

    const int32_t ans = reverse_compare_n(lhs->data(), rhs->data(), __fasts_abs(lssize));
    return lssize < 0 ? -ans : ans;
}

inline int32_t __compare_ui_impl(const biginteger_data *lhs, uint64_t rhs) noexcept {
    const auto lssize = lhs->get_ssize();

    if (lssize == 0) {
        return -(rhs != 0);
    }

    if (lssize == 1) {
        const uint64_t lvalue = lhs->data()[0];
        return (lvalue != rhs ? (lvalue < rhs ? -1 : 1) : 0);
    }

    return lssize;
}

inline int32_t __compare_si_impl(const biginteger_data *lhs, int64_t rhs) noexcept {
    const auto lssize = lhs->get_ssize();
    const int32_t rssize = rhs == 0 ? 0 : __fasts_conditional_negate<int32_t>(rhs < 0, 1);

    if (lssize != rssize) {
        return lssize - rssize;
    }

    if (lssize == 0) {
        return 0;
    }

    const uint64_t lvalue = lhs->data()[0];
    const uint64_t rvalue = rhs >= 0 ? to_unsigned(rhs) : -to_unsigned(rhs);

    if (lvalue == rvalue) {
        return 0;
    }

    if (lvalue > rvalue) {
        return lssize;
    }

    return -lssize;
}

template <bool xsign, typename S>
void __addsub_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                   uint64_t rhs) noexcept {
    const int32_t lssize = lhs->get_ssize();
    if (lssize == 0) {
        dst->clear_if_reserved(1);

        if (rhs == 0) {
            dst->set_ssize(0);
        } else {
            dst->front() = rhs;
            dst->set_ssize(__fasts_conditional_negate<int32_t>(xsign, 1));
        }

        return;
    }

    const uint32_t lusize = __fasts_abs(lssize);
    dst->reserve(lusize + 1);

    auto *const dp = dst->data();
    const auto *const lp = lhs->data();

    using compare = std::conditional_t<xsign, std::less<>, std::greater<>>;
    int32_t dssize;

    if (compare{}(lssize, 0)) {
        const auto cf = addc_1(dp, lp, lusize, rhs);
        dssize = __fasts_conditional_negate<int32_t>(xsign, lusize + cf);
        if (cf) {
            dp[lusize] = 1;
        }
    } else {
        if (lusize == 1 && lp[0] < rhs) {
            dp[0] = rhs - lp[0];
            dssize = __fasts_conditional_negate<int32_t>(xsign, 1);
        } else {
            (void)subc_1(dp, lp, lusize, rhs);
            dssize = __fasts_conditional_negate<int32_t>(!xsign,
                                                         lusize - (dp[lusize - 1] == 0));
        }
    }

    dst->set_ssize(dssize);
}

template <typename S>
void __ui_sub_impl(basic_biginteger<S> *dst, uint64_t lhs,
                   const biginteger_data *rhs) noexcept {
    const auto rssize = rhs->get_ssize();
    if (rssize == 0) {
        dst->clear_if_reserved(1);

        if (lhs == 0) {
            dst->set_ssize(0);
        } else {
            dst->front() = lhs;
            dst->set_ssize(1);
        }

        return;
    }

    const uint32_t rusize = __fasts_abs(rssize);
    dst->clear_if_reserved(rusize);

    auto *const dp = dst->data();
    const auto *const rp = rhs->data();
    int32_t dssize;

    if (rssize < 0) {
        const auto cf = addc_1(dp, rp, rusize, lhs);
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
            dssize = __fasts_negate<int32_t>(rusize - (dp[rusize - 1] == 0));
        }
    }

    dst->set_ssize(dssize);
}

template <bool xsign, typename S>
void __addsub_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                   const biginteger_data *rhs) noexcept {
    auto lssize = lhs->get_ssize();
    int32_t rssize = __fasts_conditional_negate<int32_t>(xsign, rhs->get_ssize());
    uint32_t lusize = __fasts_abs(lssize);
    uint32_t rusize = __fasts_abs(rssize);

    if (lusize < rusize) {
        std::swap(lhs, rhs);
        std::swap(lssize, rssize);
        std::swap(lusize, rusize);
    }

    dst->reserve(lusize + 1);

    auto *const dp = dst->data();
    const auto *const lp = lhs->data();
    const auto *const rp = rhs->data();
    int32_t dssize;

    if (rusize == 0) {
        if (lp != dp) {
            std::copy_n(lp, lusize, dp);
            dst->set_ssize(lssize);
        }
        return;
    }

    // different sign
    if ((lssize ^ rssize) < 0) {
        const auto ans = static_cast<int32_t>(abs_subc_s_pos(dp, lp, lusize, rp, rusize));
        dssize = __fasts_negate_with<int32_t>(lssize, ans);
    } else {
        const auto cf = addc_s(dp, lp, lusize, rp, rusize);
        dssize = __fasts_negate_with<int32_t>(lssize, lusize + cf);
        if (cf) {
            dp[lusize] = 1;
        }
    }

    dst->set_ssize(dssize);
}

template <typename S>
void __mul_ui_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                   uint64_t rhs) noexcept {
    const auto lssize = lhs->get_ssize();
    const uint32_t lusize = __fasts_abs(lssize);

    if (lusize == 0 || rhs == 0) {
        dst->set_ssize(0);
        return;
    }

    dst->reserve(lusize + 1);

    auto *const dp = dst->data();
    const auto *const lp = lhs->data();

    const auto cf = mul_1(dp, lp, lusize, rhs);
    const uint32_t dusize = lusize + (cf != 0);
    if (cf != 0) {
        dp[lusize] = cf;
    }

    dst->set_ssize(__fasts_conditional_negate<int32_t>(lssize < 0, dusize));
}

template <typename S>
void __mul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                const biginteger_data *rhs) noexcept {
    int32_t lssize = lhs->get_ssize();
    int32_t rssize = rhs->get_ssize();
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

    const int32_t mask = lssize ^ rssize;

    int32_t dssize;
    uint32_t dusize;

    if (rusize == 1) {
        dst->reserve(lusize + 1);
        const auto cf = mul_1(dst->data(), lhs->data(), lusize, rhs->data()[0]);
        dssize = __fasts_negate_with<int32_t>(mask, lusize + (cf != 0));
        if (cf != 0) {
            dst->data()[lusize] = cf;
        }
        dst->set_ssize(dssize);
        return;
    }

    dusize = lusize + rusize;

    using pointer = uint64_t *;

    auto *dp = dst->data();
    auto *lp = const_cast<pointer>(lhs->data());
    auto *rp = const_cast<pointer>(rhs->data());

    unique_stack_allocator stkal(math_detail::stack_alloc);
    std::optional<uninitialized<basic_biginteger<S>>> tmp;

    if (dst->capacity() < dusize) {
        tmp.emplace(dst->get_growth_capacity(dst->capacity(), dusize), in_place_reserve,
                    dst->get_allocator());
        dp = (**tmp).data();
    } else {
        if (dp == lp) {
            lp = static_cast<pointer>(stkal.allocate(lusize * sizeof(uint64_t)));
            if (dp == rp) {
                rp = lp;
            }
            std::copy_n(dp, lusize, lp);
        } else if (dp == rp) {
            rp = static_cast<pointer>(stkal.allocate(rusize * sizeof(uint64_t)));
            std::copy_n(dp, rusize, rp);
        }
    }

    if (WJR_UNLIKELY(lp == rp)) {
        sqr(dp, lp, lusize);
    } else {
        mul_s(dp, lp, lusize, rp, rusize);
    }

    const bool cf = dp[dusize - 1] == 0;
    dssize = __fasts_negate_with<int32_t>(mask, dusize - cf);

    if (tmp.has_value()) {
        *dst = **std::move(tmp);
        tmp->reset();
    }

    dst->set_ssize(dssize);
}

template <typename S>
void __sqr_impl(basic_biginteger<S> *dst, const biginteger_data *src) noexcept {
    int32_t sssize = src->get_ssize();
    uint32_t susize = __fasts_abs(sssize);

    if (WJR_UNLIKELY(susize == 0)) {
        dst->set_ssize(0);
        return;
    }

    int32_t dssize;
    uint32_t dusize;

    if (susize == 1) {
        dst->reserve(susize + 1);
        const uint64_t num = src->data()[0];
        uint64_t cf;
        dst->data()[0] = mul(num, num, cf);
        dssize = 1 + (cf != 0);
        if (cf != 0) {
            dst->data()[1] = cf;
        }
        dst->set_ssize(dssize);
        return;
    }

    dusize = susize * 2;

    using pointer = uint64_t *;

    auto *dp = dst->data();
    auto *sp = const_cast<pointer>(src->data());

    std::optional<uninitialized<basic_biginteger<S>>> tmp;

    unique_stack_allocator stkal(math_detail::stack_alloc);

    if (dst->capacity() < dusize) {
        tmp.emplace(dst->get_growth_capacity(dst->capacity(), dusize), in_place_reserve,
                    dst->get_allocator());
        dp = (**tmp).data();
    } else {
        if (dp == sp) {
            sp = static_cast<pointer>(stkal.allocate(susize * sizeof(uint64_t)));
            std::copy_n(dp, susize, sp);
        }
    }

    sqr(dp, sp, susize);

    const bool cf = dp[dusize - 1] == 0;
    dssize = dusize - cf;

    if (tmp.has_value()) {
        *dst = **std::move(tmp);
        tmp->reset();
    }

    dst->set_ssize(dssize);
}

template <typename S>
void __addsubmul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs, uint64_t rhs,
                      int32_t xmask) noexcept {
    const int32_t lssize = lhs->get_ssize();

    if (lssize == 0 || rhs == 0) {
        return;
    }

    const uint32_t lusize = __fasts_abs(lssize);
    int32_t dssize = dst->get_ssize();

    if (dssize == 0) {
        dst->reserve(lusize + 1);
        const auto dp = dst->data();
        const auto cf = mul_1(dp, lhs->data(), lusize, rhs);
        dssize = lssize + (cf != 0);
        if (cf != 0) {
            dp[lusize] = cf;
        }

        dst->set_ssize(__fasts_negate_with(xmask, dssize));
        return;
    }

    xmask ^= lssize;
    xmask ^= dssize;

    const uint32_t dusize = __fasts_abs(dssize);

    uint32_t new_dusize = std::max(lusize, dusize);
    const uint32_t min_size = std::min(lusize, dusize);
    dst->reserve(new_dusize + 1);

    auto *dp = dst->data();
    const auto *lp = lhs->data();

    uint64_t cf;

    // dst += abs(lhs) * abs(rhs)
    if (xmask >= 0) {
        cf = addmul_1(dp, lp, min_size, rhs);

        dp += min_size;
        lp += min_size;

        int32_t sdelta = lusize - dusize;

        if (sdelta != 0) {
            uint64_t cf2;
            if (sdelta > 0) {
                cf2 = mul_1(dp, lp, sdelta, rhs);
            } else {
                sdelta = -sdelta;
                cf2 = 0;
            }

            cf = cf2 + addc_1(dp, dp, sdelta, cf);
        }

        dp[sdelta] = cf;
        new_dusize += (cf != 0);
    }
    // dst -= abs(lhs) * abs(rhs)
    else {
        cf = submul_1(dp, lp, min_size, rhs);

        do {
            if (dusize >= lusize) {
                if (dusize != lusize) {
                    cf = subc_1(dp + lusize, dp + lusize, dusize - lusize, cf);
                }

                if (cf != 0) {
                    cf += negate_n(dp, dp, new_dusize) - 1;
                    dp[new_dusize] = cf;
                    ++new_dusize;
                    dssize = __fasts_negate(dssize);
                }
            } else {
                cf += negate_n(dp, dp, dusize) - 1;

                const auto cf2 = cf == (uint64_t)in_place_max;
                cf += cf2;

                const auto cf3 = mul_1(dp + dusize, lp + dusize, lusize - dusize, rhs);
                cf = cf3 + addc_1(dp + dusize, dp + dusize, lusize - dusize, cf);

                dp[new_dusize] = cf;
                new_dusize += (cf != 0);

                if (cf2) {
                    (void)subc_1(dp + dusize, dp + dusize, new_dusize - dusize, 1);
                }

                dssize = __fasts_negate(dssize);
            }

            new_dusize = normalize(dp, new_dusize);
        } while (0);
    }

    dst->set_ssize(__fasts_conditional_negate<int32_t>(dssize < 0, new_dusize));
}

template <typename S>
void __addsubmul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                      const biginteger_data *rhs, int32_t xmask) noexcept {
    int32_t lssize = lhs->get_ssize();
    int32_t rssize = rhs->get_ssize();

    if (lssize == 0 || rssize == 0) {
        return;
    }

    uint32_t lusize = __fasts_abs(lssize);
    uint32_t rusize = __fasts_abs(rssize);

    if (lusize < rusize) {
        std::swap(lhs, rhs);
        std::swap(lssize, rssize);
        std::swap(lusize, rusize);
    }

    xmask ^= rssize;

    if (rusize == 1) {
        __addsubmul_impl(dst, lhs, rhs->data()[0], xmask);
        return;
    }

    xmask ^= lssize;

    int32_t dssize = dst->get_ssize();
    xmask ^= dssize;
    uint32_t dusize = __fasts_abs(dssize);

    uint32_t tusize = lusize + rusize;
    dst->reserve(std::max(tusize, dusize) + 1);
    auto *const dp = dst->data();

    if (dssize == 0) {
        mul_s(dp, lhs->data(), lusize, rhs->data(), rusize);
        tusize -= dp[tusize - 1] == 0;
        dst->set_ssize(__fasts_conditional_negate<int32_t>(xmask < 0, tusize));
        return;
    }

    unique_stack_allocator stkal(math_detail::stack_alloc);
    auto *tp = static_cast<uint64_t *>(stkal.allocate(tusize * sizeof(uint64_t)));

    mul_s(tp, lhs->data(), lusize, rhs->data(), rusize);
    tusize -= tp[tusize - 1] == 0;

    auto *up = dp;
    uint32_t uusize = dusize;

    if (xmask >= 0) {
        if (uusize < tusize) {
            up = tp;
            uusize = tusize;
            tp = dp;
            tusize = dusize;

            dusize = uusize;
        }

        const auto cf = addc_s(dp, up, uusize, tp, tusize);
        dp[uusize] = cf;
        dusize = uusize + (cf != 0);
    } else {
        if (uusize < tusize ||
            (uusize == tusize && reverse_compare_n(up, tp, uusize) < 0)) {
            up = tp;
            uusize = tusize;
            tp = dp;
            tusize = dusize;

            dusize = uusize;

            dssize = __fasts_negate(dssize);
        }

        (void)subc_s(dp, up, uusize, tp, tusize);
        dssize = normalize(dp, dusize);
    }

    dst->set_ssize(__fasts_conditional_negate<int32_t>(dssize < 0, dusize));
}

template <typename S>
void __mul_2exp_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                     uint32_t shift) noexcept {
    int32_t lssize = lhs->get_ssize();

    if (lssize == 0) {
        dst->set_ssize(0);
        return;
    }

    uint32_t lusize = __fasts_abs(lssize);
    uint32_t offset = shift / 64;
    shift %= 64;
    uint32_t dusize = lusize + offset;

    dst->reserve(dusize + 1);
    auto *const dp = dst->data();
    const auto *const lp = lhs->data();

    const auto cf = lshift_n(dp + offset, lp, lusize, shift);
    set_n(dp, 0, offset);

    dp[dusize] = cf;
    dusize += (cf != 0);

    dst->set_ssize(__fasts_conditional_negate<int32_t>(lssize < 0, dusize));
}

template <typename S0, typename S1>
void __tdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                    const biginteger_data *num, const biginteger_data *div) noexcept {
    WJR_ASSERT_ASSUME(!__equal_pointer(quot, rem), "quot should not be the same as rem");

    const auto nssize = num->get_ssize();
    const auto dssize = div->get_ssize();
    const auto nusize = __fasts_abs(nssize);
    auto dusize = __fasts_abs(dssize);
    int32_t qssize = nusize - dusize + 1;

    WJR_ASSERT(dusize != 0, "division by zero");

    rem->reserve(dusize);
    auto *rp = rem->data();

    // num < div
    if (qssize <= 0) {
        auto np = num->data();
        if (np != rp) {
            std::copy_n(np, nusize, rp);
            rem->set_ssize(nssize);
        }

        quot->set_ssize(0);
        return;
    }

    using pointer = uint64_t *;

    quot->reserve(qssize);
    auto *qp = quot->data();

    auto *np = const_cast<pointer>(num->data());
    auto *dp = const_cast<pointer>(div->data());

    unique_stack_allocator stkal(math_detail::stack_alloc);

    if (dp == rp || dp == qp) {
        auto *tp = (pointer)stkal.allocate(dusize * sizeof(uint64_t));
        std::copy_n(dp, dusize, tp);
        dp = tp;
    }

    if (np == rp || np == qp) {
        auto *tp = (pointer)stkal.allocate(nusize * sizeof(uint64_t));
        std::copy_n(np, nusize, tp);
        np = tp;
    }

    div_qr_s(qp, rp, np, nusize, dp, dusize);

    qssize -= qp[qssize - 1] == 0;
    dusize = normalize(rp, dusize);

    quot->set_ssize(__fasts_conditional_negate<int32_t>((nssize ^ dssize) < 0, qssize));
    rem->set_ssize(__fasts_conditional_negate<int32_t>(nssize < 0, dusize));
}

template <typename S>
void __tdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                   const biginteger_data *div) noexcept {
    const auto nssize = num->get_ssize();
    const auto dssize = div->get_ssize();
    const auto nusize = __fasts_abs(nssize);
    const auto dusize = __fasts_abs(dssize);
    int32_t qssize = nusize - dusize + 1;

    WJR_ASSERT(dusize != 0, "division by zero");

    // num < div
    if (qssize <= 0) {
        quot->set_ssize(0);
        return;
    }

    using pointer = uint64_t *;

    quot->reserve(qssize);
    auto qp = quot->data();

    auto np = (pointer)num->data();
    auto dp = (pointer)div->data();

    unique_stack_allocator stkal(math_detail::stack_alloc);

    if (dp == qp) {
        auto tp = (pointer)stkal.allocate(dusize * sizeof(uint64_t));
        std::copy_n(dp, dusize, tp);
        dp = tp;
    }

    if (np == qp) {
        auto tp = (pointer)stkal.allocate(nusize * sizeof(uint64_t));
        std::copy_n(np, nusize, tp);
        np = tp;
    }

    const auto rp = (pointer)stkal.allocate(dusize * sizeof(uint64_t));

    div_qr_s(qp, rp, np, nusize, dp, dusize);

    qssize -= qp[qssize - 1] == 0;

    quot->set_ssize(__fasts_conditional_negate<int32_t>((nssize ^ dssize) < 0, qssize));
}

template <typename S>
void __tdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                   const biginteger_data *div) noexcept {
    const auto nssize = num->get_ssize();
    const auto dssize = div->get_ssize();
    const auto nusize = __fasts_abs(nssize);
    auto dusize = __fasts_abs(dssize);
    const int32_t qssize = nusize - dusize + 1;

    WJR_ASSERT(dusize != 0, "division by zero");

    rem->reserve(dusize);
    auto rp = rem->data();

    // num < div
    if (qssize <= 0) {
        const auto np = num->data();
        if (np != rp) {
            std::copy_n(np, nusize, rp);
            rem->set_ssize(nssize);
        }

        return;
    }

    using pointer = uint64_t *;

    auto np = (pointer)num->data();
    auto dp = (pointer)div->data();

    unique_stack_allocator stkal(math_detail::stack_alloc);

    if (dp == rp) {
        const auto tp = (pointer)stkal.allocate(dusize * sizeof(uint64_t));
        std::copy_n(dp, dusize, tp);
        dp = tp;
    }

    if (np == rp) {
        const auto tp = (pointer)stkal.allocate(nusize * sizeof(uint64_t));
        std::copy_n(np, nusize, tp);
        np = tp;
    }

    const auto qp = (pointer)stkal.allocate(qssize * sizeof(uint64_t));

    div_qr_s(qp, rp, np, nusize, dp, dusize);

    dusize = normalize(rp, dusize);

    rem->set_ssize(__fasts_conditional_negate<int32_t>(nssize < 0, dusize));
}

template <typename S0, typename S1>
uint64_t __tdiv_qr_ui_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                           const biginteger_data *num, uint64_t div) noexcept {
    const auto nssize = num->get_ssize();

    WJR_ASSERT(div != 0, "division by zero");

    if (nssize == 0) {
        quot->set_ssize(0);
        rem->set_ssize(0);
        return 0;
    }

    const auto nusize = __fasts_abs(nssize);
    quot->reserve(nusize);
    const auto qp = quot->data();
    const auto np = num->data();

    uint64_t remv;
    div_qr_1(qp, remv, np, nusize, div);

    if (remv == 0) {
        rem->set_ssize(0);
    } else {
        rem->reserve(1);
        rem->front() = remv;
        rem->set_ssize(__fasts_conditional_negate<int32_t>(nssize < 0, 1));
    }

    const auto qusize = nusize - (qp[nusize - 1] == 0);

    quot->set_ssize(__fasts_conditional_negate<int32_t>(nssize < 0, qusize));
    return remv;
}

template <typename S>
uint64_t __tdiv_q_ui_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                          uint64_t div) noexcept {
    const auto nssize = num->get_ssize();

    WJR_ASSERT(div != 0, "division by zero");

    if (nssize == 0) {
        quot->set_ssize(0);
        return 0;
    }

    const auto nusize = __fasts_abs(nssize);
    quot->reserve(nusize);
    const auto qp = quot->data();
    const auto np = num->data();

    uint64_t remv;
    div_qr_1(qp, remv, np, nusize, div);

    const auto qusize = nusize - (qp[nusize - 1] == 0);

    quot->set_ssize(__fasts_conditional_negate<int32_t>(nssize < 0, qusize));
    return remv;
}

template <typename S>
uint64_t __tdiv_r_ui_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                          uint64_t div) noexcept {
    const auto nssize = num->get_ssize();

    WJR_ASSERT(div != 0, "division by zero");

    if (nssize == 0) {
        rem->set_ssize(0);
        return 0;
    }

    const auto nusize = __fasts_abs(nssize);
    const auto np = num->data();

    uint64_t remv;
    remv = mod_1(np, nusize, div);

    if (remv == 0) {
        rem->set_ssize(0);
    } else {
        rem->reserve(1);
        rem->front() = remv;
        rem->set_ssize(__fasts_conditional_negate<int32_t>(nssize < 0, 1));
    }

    return remv;
}

template <typename S0, typename S1, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __tdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                        const biginteger_data *num, T div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        return __tdiv_qr_ui_impl(quot, rem, num, div);
    } else {
        uint64_t udiv = to_unsigned(div);
        bool xsign = false;
        if (div < 0) {
            udiv = -udiv;
            xsign = true;
        }

        uint64_t remv = __tdiv_qr_ui_impl(quot, rem, num, udiv);

        quot->conditional_negate(xsign);
        return remv;
    }
}

template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __tdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                       T div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        return __tdiv_q_ui_impl(quot, num, div);
    } else {
        uint64_t udiv = to_unsigned(div);
        bool xsign = false;
        if (div < 0) {
            udiv = -udiv;
            xsign = true;
        }

        uint64_t remv = __tdiv_q_ui_impl(quot, num, udiv);

        quot->conditional_negate(xsign);
        return remv;
    }
}

template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __tdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                       uint64_t div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        return __tdiv_q_ui_impl(rem, num, div);
    } else {
        uint64_t udiv = to_unsigned(div);
        if (div < 0) {
            udiv = -udiv;
        }

        return __tdiv_r_ui_impl(rem, num, udiv);
    }
}

template <typename S0, typename S1>
void __fdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                    const biginteger_data *num, const biginteger_data *div) noexcept {

    WJR_ASSERT_ASSUME(!__equal_pointer(quot, rem), "quot should not be the same as rem");

    unique_stack_allocator stkal(math_detail::stack_alloc);

    biginteger_data tmp_div;

    const auto dssize = div->get_ssize();

    if (__equal_pointer(div, quot) || __equal_pointer(div, rem)) {
        const auto dusize = __fasts_abs(dssize);
        const auto ptr = (uint64_t *)stkal.allocate(dusize * sizeof(uint64_t));
        tmp_div = {ptr, dssize, 0};
        std::copy_n(div->data(), dusize, ptr);

        div = &tmp_div;
    }

    const auto xsize = num->get_ssize() ^ dssize;

    __tdiv_qr_impl(quot, rem, num, div);

    if (xsize < 0 && !rem->empty()) {
        __sub_impl(quot, quot->__get_data(), 1u);
        __add_impl(rem, rem->__get_data(), div);
    }
}

template <typename S>
void __fdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                   const biginteger_data *div) noexcept {
    unique_stack_allocator stkal(math_detail::stack_alloc);
    stack_biginteger rem(stkal);

    const auto xsize = num->get_ssize() ^ div->get_ssize();

    __tdiv_qr_impl(quot, &rem, num, div);

    if (xsize < 0 && !rem.empty()) {
        __sub_impl(quot, quot->__get_data(), 1u);
    }
}

template <typename S>
void __fdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                   const biginteger_data *div) noexcept {
    unique_stack_allocator stkal(math_detail::stack_alloc);

    biginteger_data tmp_div;

    const auto dssize = div->get_ssize();

    if (__equal_pointer(div, rem)) {
        const auto dusize = __fasts_abs(dssize);
        const auto ptr = (uint64_t *)stkal.allocate(dusize * sizeof(uint64_t));
        tmp_div = {ptr, dssize, 0};
        std::copy_n(div->data(), dusize, ptr);

        div = &tmp_div;
    }

    const auto xsize = num->get_ssize() ^ dssize;

    __tdiv_r_impl(rem, num, div);

    if (xsize < 0 && !rem->empty()) {
        __add_impl(rem, rem->__get_data(), div);
    }
}

template <typename S0, typename S1, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __fdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                        const biginteger_data *num, T div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        const auto xssize = num->get_ssize();

        uint64_t remv = __tdiv_qr_ui_impl(quot, rem, num, div);

        if (xssize < 0 && remv != 0) {
            WJR_ASSERT(rem->is_negate());

            __sub_impl(quot, quot->__get_data(), 1u);
            rem->set_ssize(1);
            remv = div - remv;
            rem->front() = remv;
        }

        return remv;
    } else {
        uint64_t udiv = to_unsigned(div);
        int32_t xsign = 0;

        if (div < 0) {
            udiv = -udiv;
            xsign = -1;
        }

        const int32_t nssize = num->get_ssize();

        uint64_t remv = __tdiv_qr_ui_impl(quot, rem, num, udiv);

        quot->conditional_negate(xsign);

        if ((nssize ^ xsign) < 0 && remv != 0) {
            __sub_impl(quot, quot->__get_data(), 1u);
            rem->set_ssize(__fasts_conditional_negate(xsign < 0, 1));
            remv = div - remv;
            rem->front() = remv;
        }

        return remv;
    }
}

template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __fdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                       T div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        const auto xssize = num->get_ssize();

        uint64_t remv = __tdiv_q_ui_impl(quot, num, div);

        if (xssize < 0 && remv != 0) {
            __sub_impl(quot, quot->__get_data(), 1u);
            remv = div - remv;
        }

        return remv;
    } else {
        uint64_t udiv = to_unsigned(div);
        int32_t xsign = 0;

        if (div < 0) {
            udiv = -udiv;
            xsign = -1;
        }

        const int32_t nssize = num->get_ssize();

        uint64_t remv = __tdiv_q_ui_impl(quot, num, udiv);

        quot->conditional_negate(xsign);

        if ((nssize ^ xsign) < 0 && remv != 0) {
            __sub_impl(quot, quot->__get_data(), 1u);
            remv = div - remv;
        }

        return remv;
    }
}

template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __fdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                       uint64_t div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        const auto xssize = num->get_ssize();

        uint64_t remv = __tdiv_r_ui_impl(rem, num, div);

        if (xssize < 0 && remv != 0) {
            WJR_ASSERT(rem->is_negate());

            rem->set_ssize(1);
            remv = div - remv;
            rem->front() = remv;
        }

        return remv;
    } else {
        uint64_t udiv = to_unsigned(div);
        int32_t xsign = 0;

        if (div < 0) {
            udiv = -udiv;
            xsign = -1;
        }

        const int32_t nssize = num->get_ssize();

        uint64_t remv = __tdiv_r_ui_impl(rem, num, udiv);

        if ((nssize ^ xsign) < 0 && remv != 0) {
            rem->set_ssize(__fasts_conditional_negate(xsign < 0, 1));
            remv = div - remv;
            rem->front() = remv;
        }

        return remv;
    }
}

template <typename S0, typename S1>
void __cdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                    const biginteger_data *num, const biginteger_data *div) noexcept {

    WJR_ASSERT_ASSUME(!__equal_pointer(quot, rem), "quot should not be the same as rem");

    unique_stack_allocator stkal(math_detail::stack_alloc);

    biginteger_data tmp_div;

    const auto dssize = div->get_ssize();

    if (__equal_pointer(div, quot) || __equal_pointer(div, rem)) {
        const auto dusize = __fasts_abs(dssize);
        const auto ptr = (uint64_t *)stkal.allocate(dusize * sizeof(uint64_t));
        tmp_div = {ptr, dssize, 0};
        std::copy_n(div->data(), dusize, ptr);

        div = &tmp_div;
    }

    const auto xsize = num->get_ssize() ^ dssize;

    __tdiv_qr_impl(quot, rem, num, div);

    if (xsize >= 0 && !rem->empty()) {
        __add_impl(quot, quot->__get_data(), 1u);
        __sub_impl(rem, rem->__get_data(), div);
    }
}

template <typename S>
void __cdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                   const biginteger_data *div) noexcept {
    unique_stack_allocator stkal(math_detail::stack_alloc);
    stack_biginteger rem(stkal);

    const auto xsize = num->get_ssize() ^ div->get_ssize();

    __tdiv_qr_impl(quot, &rem, num, div);

    if (xsize >= 0 && !rem.empty()) {
        __add_impl(quot, quot->__get_data(), 1u);
    }
}

template <typename S>
void __cdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                   const biginteger_data *div) noexcept {
    unique_stack_allocator stkal(math_detail::stack_alloc);

    biginteger_data tmp_div;

    const auto dssize = div->get_ssize();

    if (__equal_pointer(div, rem)) {
        const auto dusize = __fasts_abs(dssize);
        const auto ptr = (uint64_t *)stkal.allocate(dusize * sizeof(uint64_t));
        tmp_div = {ptr, dssize, 0};
        std::copy_n(div->data(), dusize, ptr);

        div = &tmp_div;
    }

    const auto xsize = num->get_ssize() ^ dssize;

    __tdiv_r_impl(rem, num, div);

    if (xsize >= 0 && !rem->empty()) {
        __sub_impl(rem, rem->__get_data(), div);
    }
}

template <typename S0, typename S1, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __cdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                        const biginteger_data *num, T div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        const auto xssize = num->get_ssize();

        uint64_t remv = __tdiv_qr_ui_impl(quot, rem, num, div);

        if (xssize >= 0 && remv != 0) {
            WJR_ASSERT(rem->is_negate());

            __add_impl(quot, quot->__get_data(), 1u);
            rem->set_ssize(-1);
            remv = div - remv;
            rem->front() = remv;
        }

        return remv;
    } else {
        uint64_t udiv = to_unsigned(div);
        int32_t xsign = 0;

        if (div < 0) {
            udiv = -udiv;
            xsign = -1;
        }

        const int32_t nssize = num->get_ssize();

        uint64_t remv = __tdiv_qr_ui_impl(quot, rem, num, udiv);

        quot->conditional_negate(xsign);

        if ((nssize ^ xsign) >= 0 && remv != 0) {
            __add_impl(quot, quot->__get_data(), 1u);
            rem->set_ssize(__fasts_conditional_negate(xsign >= 0, 1));
            remv = div - remv;
            rem->front() = remv;
        }

        return remv;
    }
}

template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __cdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                       T div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        const auto xssize = num->get_ssize();

        uint64_t remv = __tdiv_q_ui_impl(quot, num, div);

        if (xssize >= 0 && remv != 0) {
            __add_impl(quot, quot->__get_data(), 1u);
            remv = div - remv;
        }

        return remv;
    } else {
        uint64_t udiv = to_unsigned(div);
        int32_t xsign = 0;

        if (div < 0) {
            udiv = -udiv;
            xsign = -1;
        }

        const int32_t nssize = num->get_ssize();

        uint64_t remv = __tdiv_q_ui_impl(quot, num, udiv);

        quot->conditional_negate(xsign);

        if ((nssize ^ xsign) >= 0 && remv != 0) {
            __add_impl(quot, quot->__get_data(), 1u);
            remv = div - remv;
        }

        return remv;
    }
}

template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __cdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                       uint64_t div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        const auto xssize = num->get_ssize();

        uint64_t remv = __tdiv_r_ui_impl(rem, num, div);

        if (xssize >= 0 && remv != 0) {
            WJR_ASSERT(rem->is_negate());

            rem->set_ssize(-1);
            remv = div - remv;
            rem->front() = remv;
        }

        return remv;
    } else {
        uint64_t udiv = to_unsigned(div);
        int32_t xsign = 0;

        if (div < 0) {
            udiv = -udiv;
            xsign = -1;
        }

        const int32_t nssize = num->get_ssize();

        uint64_t remv = __tdiv_r_ui_impl(rem, num, udiv);

        if ((nssize ^ xsign) >= 0 && remv != 0) {
            rem->set_ssize(__fasts_conditional_negate(xsign >= 0, 1));
            remv = div - remv;
            rem->front() = remv;
        }

        return remv;
    }
}

template <typename S>
void __tdiv_q_2exp_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                        uint32_t shift) noexcept {
    int32_t nssize = num->get_ssize();
    uint32_t nusize = __fasts_abs(nssize);
    uint32_t offset = shift / 64;

    int32_t qssize = nusize - offset;

    if (qssize <= 0) {
        quot->set_ssize(0);
        return;
    }

    quot->reserve(qssize);
    const auto qp = quot->data();
    const auto np = num->data();

    (void)rshift_n(qp, np + offset, qssize, shift % 64);
    qssize -= qp[qssize - 1] == 0;

    quot->set_ssize(__fasts_conditional_negate<int32_t>(nssize < 0, qssize));
}

template <typename S>
void __tdiv_r_2exp_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                        uint32_t shift) noexcept {
    const int32_t nssize = num->get_ssize();
    uint32_t nusize = __fasts_abs(nssize);
    uint32_t offset = shift / 64;

    uint32_t rusize;

    if (nusize <= offset) {
        rusize = nusize;
        offset = nusize;
        rem->reserve(rusize);
    } else {
        uint64_t high = num->data()[offset] & (((uint64_t)(1) << (shift % 64)) - 1);
        if (high != 0) {
            rusize = offset + 1;
            rem->reserve(rusize);
            rem->data()[offset] = high;
        } else {
            rusize = normalize(num->data(), offset);
            offset = rusize;
            rem->reserve(rusize);
        }
    }

    if (!__equal_pointer(rem, num)) {
        std::copy_n(num->data(), offset, rem->data());
    }

    rem->set_ssize(__fasts_conditional_negate<int32_t>(nssize < 0, rusize));
}

template <typename S>
void __cfdiv_q_2exp_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                         uint32_t shift, int32_t xdir) noexcept {
    int32_t nssize = num->get_ssize();
    uint32_t nusize = __fasts_abs(nssize);
    uint32_t offset = shift / 64;

    int32_t qssize = nusize - offset;

    if (qssize <= 0) {
        if (nssize == 0) {
            quot->set_ssize(0);
            return;
        }

        quot->clear_if_reserved(1);
        quot->data()[0] = 1;

        quot->set_ssize((nssize ^ xdir) < 0 ? 0 : xdir);
        return;
    }

    quot->reserve(qssize + 1);
    const auto qp = quot->data();
    const auto np = num->data();

    uint64_t xmask = (nssize ^ xdir) < 0 ? 0 : (uint64_t)in_place_max;
    uint64_t round = 0;

    if (xmask) {
        // all is zero, then round is zero
        round = find_not_n(np, 0, offset) == offset ? 0 : 1;
    }

    round |= xmask & rshift_n(qp, np + offset, qssize, shift % 64);
    qssize -= qp[qssize - 1] == 0;

    if (WJR_LIKELY(round != 0)) {
        if (WJR_LIKELY(qssize != 0)) {
            const auto cf = addc_1(qp, qp, qssize, 1u);
            if (cf != 0) {
                qp[qssize] = cf;
                ++qssize;
            }
        } else {
            qp[0] = 1;
            qssize = 1;
        }
    }

    quot->set_ssize(__fasts_conditional_negate<int32_t>(nssize < 0, qssize));
}

template <typename S>
void __cfdiv_r_2exp_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                         uint32_t shift, int32_t xdir) noexcept {
    int32_t nssize = num->get_ssize();

    if (nssize == 0) {
        rem->set_ssize(0);
        return;
    }

    uint32_t nusize = __fasts_abs(nssize);
    uint32_t offset = shift / 64;
    shift %= 64;

    uint64_t *rp;
    auto np = (uint64_t *)(num->data());

    if ((nssize ^ xdir) < 0) {
        if (__equal_pointer(rem, num)) {
            if (nusize <= offset) {
                return;
            }

            rp = np;
        } else {
            const auto size = std::min<uint32_t>(nusize, offset + 1);
            rem->reserve(size);
            rp = rem->data();
            std::copy_n(np, size, rp);

            if (nusize <= offset) {
                rem->set_ssize(nssize);
                return;
            }
        }
    } else {
        do {
            if (nusize <= offset) {
                break;
            }

            if (find_not_n(np, 0, offset) != offset) {
                break;
            }

            if ((np[offset] & (((uint64_t)(1) << shift) - 1)) != 0) {
                break;
            }

            rem->set_ssize(0);
            return;
        } while (0);

        rem->reserve(offset + 1);
        rp = rem->data();
        np = (uint64_t *)(num->data());

        const auto size = std::min<uint32_t>(nusize, offset + 1);
        (void)negate_n(rp, np, size);
        for (uint32_t i = size; i <= offset; ++i) {
            rp[i] = static_cast<uint64_t>(in_place_max);
        }

        nssize = -nssize;
    }

    uint64_t hi = rp[offset] & (((uint64_t)(1) << shift) - 1);
    rp[offset] = hi;

    if (hi == 0) {
        offset = normalize(rp, offset);
    } else {
        ++offset;
    }

    rem->set_ssize(__fasts_conditional_negate<int32_t>(nssize < 0, offset));
}

template <typename S, typename Engine,
          WJR_REQUIRES_I(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_bit_impl(basic_biginteger<S> *dst, uint32_t size,
                        Engine &engine) noexcept {
    const uint32_t dusize = size / 64;
    size %= 64;
    uint32_t dssize = dusize + (size != 0);

    dst->reserve(dssize);
    const auto dp = dst->data();

    do {
        for (uint64_t i = 0; i < dusize; ++i) {
            dp[i] = engine();
        }
    } while (0);

    if (size != 0) {
        dp[dusize] = engine() >> (64 - size);
    }

    dssize = normalize(dp, dssize);
    dst->set_ssize(dssize);
}

template <typename S, typename Engine,
          WJR_REQUIRES_I(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_exact_bit_impl(basic_biginteger<S> *dst, uint32_t size,
                              Engine &engine) noexcept {
    if (WJR_UNLIKELY(size == 0)) {
        dst->set_ssize(0);
        return;
    }

    const uint32_t dusize = (size - 1) / 64;
    size = (size - 1) % 64;
    const uint32_t dssize = dusize + 1;

    dst->reserve(dssize);
    const auto dp = dst->data();

    do {
        for (uint64_t i = 0; i < dusize; ++i) {
            dp[i] = engine();
        }
    } while (0);

    do {
        uint64_t high = (uint64_t)(1) << size;

        if (size != 0) {
            high |= engine() >> (64 - size);
        }

        dp[dusize] = high;
    } while (0);

    dst->set_ssize(dssize);
}

template <typename S, typename Engine,
          WJR_REQUIRES_I(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_impl(basic_biginteger<S> *dst, const biginteger_data *limit,
                    Engine &engine) noexcept {
    std::optional<uninitialized<math_detail::unique_stack_alloc>> stkal;
    uint32_t size = limit->size();

    WJR_ASSERT(size != 0);

    if (WJR_UNLIKELY(size == 1)) {
        std::uniform_int_distribution<uint64_t> head(0, limit->data()[0] - 1);
        const uint64_t gen = head(engine);

        dst->clear_if_reserved(1);
        dst->data()[0] = gen;
        dst->set_ssize(gen == 0 ? 0 : 1);
        return;
    }

    dst->reserve(size);

    const auto dp = dst->data();
    const uint64_t *lp = limit->data();

    if (__equal_pointer(dst, limit)) {
        stkal.emplace(math_detail::stack_alloc);
        const auto tp = (uint64_t *)(*stkal)->allocate(size * sizeof(uint64_t));
        std::copy_n(lp, size, tp);
        lp = tp;
    }

    const uint32_t lst_pos = size - 1;
    const uint64_t lst_val = lp[lst_pos];
    std::uniform_int_distribution<uint64_t> head(0, lst_val);

    while (true) {
        uint64_t gen = head(engine);
        uint32_t pos = lst_pos;

        if (WJR_UNLIKELY(gen == lst_val)) {
            uint64_t now;
            do {
                dp[pos--] = gen;
                gen = engine();
                now = lp[pos];

                if (WJR_LIKELY(gen != now)) {
                    goto NEXT;
                }

            } while (pos);

            continue;

        NEXT:

            if (gen > now) {
                continue;
            }
        }

        dp[pos] = gen;
        while (pos) {
            dp[--pos] = engine();
        }

        break;
    }

    size = normalize(dp, size);
    dst->set_ssize(size);

    if (stkal.has_value()) {
        stkal->reset();
    }
}

template <typename S, typename Engine,
          WJR_REQUIRES_I(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_exact_impl(basic_biginteger<S> *dst, const biginteger_data *limit,
                          Engine &engine) noexcept {
    std::optional<uninitialized<math_detail::unique_stack_alloc>> stkal;
    uint32_t size = limit->size();

    if (WJR_UNLIKELY(size <= 1)) {
        if (size == 0) {
            dst->set_ssize(0);
            return;
        }

        std::uniform_int_distribution<uint64_t> head(0, limit->data()[0]);
        const uint64_t gen = head(engine);

        dst->clear_if_reserved(1);
        dst->data()[0] = gen;
        dst->set_ssize(gen == 0 ? 0 : 1);
        return;
    }

    dst->reserve(size);

    const auto dp = dst->data();
    const uint64_t *lp = limit->data();

    if (__equal_pointer(dst, limit)) {
        stkal.emplace(math_detail::stack_alloc);
        const auto tp = (uint64_t *)(*stkal)->allocate(size * sizeof(uint64_t));
        std::copy_n(lp, size, tp);
        lp = tp;
    }

    const uint32_t lst_pos = size - 1;
    const uint64_t lst_val = lp[lst_pos];
    std::uniform_int_distribution<uint64_t> head(0, lst_val);

    while (true) {
        uint64_t gen = head(engine);
        uint32_t pos = lst_pos;

        if (WJR_UNLIKELY(gen == lst_val)) {
            uint64_t now;
            do {
                dp[pos--] = gen;
                gen = engine();
                now = lp[pos];
            } while (gen == now && pos);

            if (gen > now) {
                continue;
            }
        }

        dp[pos] = gen;

        while (pos) {
            dp[--pos] = engine();
        }

        break;
    }

    size = normalize(dp, size);
    dst->set_ssize(size);

    if (stkal.has_value()) {
        stkal->reset();
    }
}

/// @private
inline uint32_t __bit_width_impl(const biginteger_data *num) noexcept {
    const uint32_t size = num->size();
    if (WJR_UNLIKELY(size == 0)) {
        return 0;
    }

    return 64 * size - clz(num->data()[size - 1]);
}

/// @private
inline uint32_t __ctz_impl(const biginteger_data *num) noexcept {
    if (num->empty()) {
        return 0;
    }

    // can be optimize by using SIMD

    const auto *const ptr = num->data();
    const uint32_t size = num->size();
    const uint32_t idx = find_not_n(ptr, 0, size);
    WJR_ASSERT(idx != size);
    return idx * 64 + ctz(ptr[idx]);
}

template <typename S>
void __pow_impl(basic_biginteger<S> *dst, const biginteger_data *num,
                uint32_t exp) noexcept {
    if (exp == 0) {
        *dst = 1u;
        return;
    }

    const uint32_t nbits = __bit_width_impl(num);
    const uint32_t dbits = nbits * exp;
    const uint32_t max_dusize = (dbits + 63) / 64;

    dst->reserve(max_dusize);
    *dst = *num;

    while (!(exp & 1)) {
        sqr(*dst, *dst);
        exp >>= 1;
    }

    if (exp == 1) {
        return;
    }

    basic_biginteger<S> tmp;
    exp >>= 1;
    sqr(tmp, *dst);

    if (exp & 1) {
        mul(*dst, *dst, tmp);
    }

    if (exp != 1) {
        do {
            exp >>= 1;
            sqr(tmp, tmp);

            if (exp & 1) {
                mul(*dst, *dst, tmp);
            }
        } while (exp != 1);
    }
}

/// @private
template <typename S>
void __powmod_impl(basic_biginteger<S> *dst, const biginteger_data *num,
                   __powmod_iterator *iter, const biginteger_data *mod) noexcept {
    WJR_ASSERT(mod->size() != 0);

    const auto size = iter->size;

    if (size == 0) {
        uint64_t val;
        if (WJR_UNLIKELY(mod->size() == 1 && mod->data()[0] == 1)) {
            val = 0u;
        } else {
            val = 1u;
        }

        *dst = val;
        return;
    }

    tdiv_r(*dst, *num, *mod);

    while (!(iter->get() & 1)) {
        sqr(*dst, *dst);
        tdiv_r(*dst, *dst, *mod);
        iter->next();
    }

    if (iter->end()) {
        return;
    }

    basic_biginteger<S> tmp;
    iter->next();
    sqr(tmp, *dst);
    tdiv_r(tmp, tmp, *mod);

    if (iter->get() & 1) {
        mul(*dst, *dst, tmp);
        tdiv_r(*dst, *dst, *mod);
    }

    if (!iter->end()) {
        do {
            iter->next();
            sqr(tmp, tmp);
            tdiv_r(tmp, tmp, *mod);

            if (iter->get() & 1) {
                mul(*dst, *dst, tmp);
                tdiv_r(*dst, *dst, *mod);
            }
        } while (!iter->end());
    }
}

template <typename S>
void __powmod_impl(basic_biginteger<S> *dst, const biginteger_data *num,
                   const biginteger_data *exp, const biginteger_data *mod) noexcept {
    __powmod_iterator iter(exp->data(), exp->size());
    __powmod_impl(dst, num, &iter, mod);
}

template <typename S>
void __powmod_impl(basic_biginteger<S> *dst, const biginteger_data *num, uint64_t exp,
                   const biginteger_data *mod) noexcept {
    uint64_t tmp[] = {exp};
    __powmod_iterator iter(tmp, exp == 0 ? 0 : 1);
    __powmod_impl(dst, num, &iter, mod);
}

} // namespace biginteger_detail

template <typename S>
from_chars_result<const char *> from_chars(const char *first, const char *last,
                                           basic_biginteger<S> &dst,
                                           unsigned int base) noexcept {
    return biginteger_detail::__from_chars_impl(first, last, &dst, base);
}

template <typename S>
void negate(basic_biginteger<S> &dst) noexcept {
    dst.negate();
}

template <typename S>
void absolute(basic_biginteger<S> &dst) noexcept {
    dst.absolute();
}

template <typename S>
std::istream &operator>>(std::istream &is, basic_biginteger<S> &dst) noexcept {
    std::string str;
    is >> str;
    from_chars(str.data(), str.data() + str.size(), dst, 0);
    return is;
}

template <typename Traits>
std::basic_ostream<char, Traits> &operator<<(std::basic_ostream<char, Traits> &os,
                                             const biginteger_data &src) noexcept {
    std::ios_base::iostate state = std::ios::goodbit;

    if (const std::ostream::sentry ok(os); ok) {
        unique_stack_allocator stkal(math_detail::stack_alloc);

        vector<char, math_detail::weak_stack_alloc<char>> buffer(stkal);
        buffer.clear_if_reserved(128);

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

namespace std {

template <typename Storage>
constexpr void
swap(wjr::basic_biginteger<Storage> &lhs,
     wjr::basic_biginteger<Storage> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

} // namespace std

#endif