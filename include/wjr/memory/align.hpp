#ifndef WJR_MEMORY_ALIGN_HPP__
#define WJR_MEMORY_ALIGN_HPP__

#include <wjr/assert.hpp>
#include <wjr/math/bit.hpp>

namespace wjr {

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST constexpr T align_down(T n, type_identity_t<T> alignment) noexcept {
    WJR_ASSERT_ASSUME_L2(has_single_bit(alignment));
    return n & (-alignment);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST constexpr T align_up(T n, type_identity_t<T> alignment) noexcept {
    WJR_ASSERT_ASSUME_L2(has_single_bit(alignment));
    return (n + alignment - 1) & (-alignment);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST constexpr T align_down_offset(T n, type_identity_t<T> alignment) noexcept {
    WJR_ASSERT_ASSUME_L2(has_single_bit(alignment));
    return n & (alignment - 1);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST constexpr T align_up_offset(T n, type_identity_t<T> alignment) noexcept {
    WJR_ASSERT_ASSUME_L2(has_single_bit(alignment));
    return (-n) & (alignment - 1);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST constexpr bool is_aligned(T n, type_identity_t<T> alignment) noexcept {
    WJR_ASSERT_ASSUME_L2(has_single_bit(alignment));
    return align_down_offset(n, alignment) == 0;
}

#if defined(WJR_CPP_20)
using std::assume_aligned;
#else
template <size_t N, typename T>
constexpr T *assume_aligned(T *ptr) noexcept {
    #if WJR_HAS_BUILTIN(__builtin_assume_aligned)
    return static_cast<T *>(__builtin_assume_aligned(ptr, N));
    #else
    WJR_ASSUME(is_aigned(ptr, N));
    return ptr;
    #endif
}
#endif

namespace mem {
#if defined(__STDCPP_DEFAULT_NEW_ALIGNMENT__)
constexpr size_t default_new_alignment =
    std::max<size_t>(alignof(max_align_t), __STDCPP_DEFAULT_NEW_ALIGNMENT__);
#else
constexpr size_t kAlignment = alignof(max_align_t);
#endif
} // namespace mem

} // namespace wjr

#endif // WJR_MEMORY_ALIGN_HPP__