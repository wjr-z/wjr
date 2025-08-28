#ifndef WJR_ARCH_X86_MATH_LARGE_COMPARE_IMPL_HPP__
#define WJR_ARCH_X86_MATH_LARGE_COMPARE_IMPL_HPP__

#include <wjr/preprocessor.hpp>

namespace wjr {

#if WJR_HAS_SIMD(SSE4_1)
    #define WJR_HAS_BUILTIN_COMPARE_N WJR_HAS_DEF
    #define WJR_HAS_BUILTIN_REVERSE_COMPARE_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(COMPARE_N)

/**
 * @brief Use SIMD to compare two arrays of uint64_t.
 *
 */
template <typename T>
WJR_ALL_NONNULL WJR_PURE int large_builtin_compare_n(const T *src0, const T *src1,
                                                     size_t n) noexcept;

extern template int large_builtin_compare_n<uint64_t>(const uint64_t *src0, const uint64_t *src1,
                                                      size_t n) noexcept;

#endif

#if WJR_HAS_BUILTIN(REVERSE_COMPARE_N)

/**
 * @brief Use SIMD to compare two arrays of uint64_t in reverse order.
 *
 * @details @ref large_builtin_compare_n in reverse order.
 *
 */
template <typename T>
WJR_ALL_NONNULL WJR_PURE int large_builtin_reverse_compare_n(const T *src0, const T *src1,
                                                             size_t n) noexcept;

extern template int large_builtin_reverse_compare_n<uint64_t>(const uint64_t *src0,
                                                              const uint64_t *src1,
                                                              size_t n) noexcept;

#endif

} // namespace wjr

#endif // WJR_ARCH_X86_MATH_LARGE_COMPARE_IMPL_HPP__