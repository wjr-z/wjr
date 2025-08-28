#ifndef WJR_ARCH_X86_MATH_LARGE_FIND_IMPL_HPP__
#define WJR_ARCH_X86_MATH_LARGE_FIND_IMPL_HPP__

#include <wjr/arch/x86/simd/simd.hpp>

namespace wjr {

#if WJR_HAS_SIMD(SSE4_1)
    #define WJR_HAS_BUILTIN_FIND_N WJR_HAS_DEF
    #define WJR_HAS_BUILTIN_REVERSE_FIND_N WJR_HAS_DEF
    #define WJR_HAS_BUILTIN_FIND_NOT_N WJR_HAS_DEF
    #define WJR_HAS_BUILTIN_REVERSE_FIND_NOT_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(FIND_NOT_N)

template <typename T>
WJR_ALL_NONNULL WJR_PURE size_t large_builtin_find_not_n(const T *src0, const T *src1,
                                                         size_t n) noexcept;

extern template size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src0,
                                                          const uint64_t *src1, size_t n) noexcept;

template <typename T>
WJR_ALL_NONNULL WJR_PURE size_t large_builtin_find_not_n(const T *src, T val, size_t n) noexcept;

extern template size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src, uint64_t val,
                                                          size_t n) noexcept;

#endif // WJR_HAS_BUILTIN(FIND_NOT_N)

#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)

template <typename T>
WJR_ALL_NONNULL WJR_PURE size_t large_builtin_reverse_find_not_n(const T *src0, const T *src1,
                                                                 size_t n) noexcept;

extern template size_t large_builtin_reverse_find_not_n<uint64_t>(const uint64_t *src0,
                                                                  const uint64_t *src1,
                                                                  size_t n) noexcept;

template <typename T>
WJR_ALL_NONNULL WJR_PURE size_t large_builtin_reverse_find_not_n(const T *src, T val,
                                                                 size_t n) noexcept;

extern template size_t large_builtin_reverse_find_not_n<uint64_t>(const uint64_t *src, uint64_t val,
                                                                  size_t n) noexcept;

#endif // WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)

} // namespace wjr

#endif // WJR_ARCH_X86_MATH_LARGE_FIND_IMPL_HPP__