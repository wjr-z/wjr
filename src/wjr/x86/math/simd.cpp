#include <wjr/math/shift.hpp>
#include <wjr/arch/x86/math/large-compare-impl.hpp>
#include <wjr/arch/x86/math/large-find-impl.hpp>

namespace wjr {

#if WJR_HAS_BUILTIN(COMPARE_N)
template int large_builtin_compare_n<uint64_t>(const uint64_t *src0, const uint64_t *src1,
                                               size_t n) noexcept;
#endif

#if WJR_HAS_BUILTIN(REVERSE_COMPARE_N)
template int large_builtin_reverse_compare_n<uint64_t>(const uint64_t *src0,
                                                       const uint64_t *src1,
                                                       size_t n) noexcept;
#endif

#if WJR_HAS_BUILTIN(FIND_NOT_N)
template size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src0,
                                                   const uint64_t *src1,
                                                   size_t n) noexcept;

template size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src, uint64_t val,
                                                   size_t n) noexcept;
#endif

#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)
template size_t large_builtin_reverse_find_not_n<uint64_t>(const uint64_t *src0,
                                                           const uint64_t *src1,
                                                           size_t n) noexcept;

template size_t large_builtin_reverse_find_not_n<uint64_t>(const uint64_t *src,
                                                           uint64_t val,
                                                           size_t n) noexcept;
#endif

#if WJR_HAS_BUILTIN(LSHIFT_N)

template void large_builtin_lshift_n_impl<false, uint64_t>(uint64_t *dst,
                                                           const uint64_t *src, size_t n,
                                                           unsigned int c) noexcept;

template void large_builtin_lshift_n_impl<true, uint64_t>(uint64_t *dst,
                                                          const uint64_t *src, size_t n,
                                                          unsigned int c) noexcept;

#endif

#if WJR_HAS_BUILTIN(RSHIFT_N)

template void large_builtin_rshift_n_impl<false, uint64_t>(uint64_t *dst,
                                                           const uint64_t *src, size_t n,
                                                           unsigned int c) noexcept;

template void large_builtin_rshift_n_impl<true, uint64_t>(uint64_t *dst,
                                                          const uint64_t *src, size_t n,
                                                          unsigned int c) noexcept;

#endif

} // namespace wjr