#include <wjr/x86/math/compare.hpp>
#include <wjr/x86/math/find.hpp>

namespace wjr {
#if WJR_HAS_BUILTIN(COMPARE_N)
template int large_builtin_compare_n<uint64_t>(const uint64_t *src0, const uint64_t *src1,
                                               size_t n);
#endif

#if WJR_HAS_BUILTIN(REVERSE_COMPARE_N)
template int large_builtin_reverse_compare_n<uint64_t>(const uint64_t *src0,
                                                       const uint64_t *src1, size_t n);
#endif

#if WJR_HAS_BUILTIN(FIND_NOT_N)
template size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src0,
                                                   const uint64_t *src1, size_t n);
template size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src, uint64_t val,
                                                   size_t n);
#endif

#if WJR_HAS_BUILTIN(REVERSE_FIND_NOT_N)
template size_t large_builtin_reverse_find_not_n<uint64_t>(const uint64_t *src0,
                                                           const uint64_t *src1,
                                                           size_t n);
template size_t large_builtin_reverse_find_not_n<uint64_t>(const uint64_t *src,
                                                           uint64_t val, size_t n);
#endif

} // namespace wjr