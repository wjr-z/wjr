#include <cstdint>
#include <limits>

#include <wjr/arch/x86/cpuinfo.hpp>

namespace wjr {
cpu_features::X86Info x86_cpu_info = cpu_features::GetX86Info();

#ifndef WJR_X86_REP_STOSB_DEFAULT_THRESHOLD
    #define WJR_X86_REP_STOSB_DEFAULT_THRESHOLD ((size_t)(1) << 24)
#endif

inline constexpr size_t __x86_rep_stosb_default_threshold = WJR_X86_REP_STOSB_DEFAULT_THRESHOLD;

namespace {
template <typename T>
inline size_t __x86_rep_stosb_threshold_impl =
    x86_cpu_info.features.fs_rep_stosb ? __x86_rep_stosb_default_threshold / sizeof(T)
                                       : std::numeric_limits<size_t>::max();
}

size_t __x86_rep_stosb_threshold_u16 = __x86_rep_stosb_threshold_impl<uint16_t>;
size_t __x86_rep_stosb_threshold_u32 = __x86_rep_stosb_threshold_impl<uint32_t>;
size_t __x86_rep_stosb_threshold_u64 = __x86_rep_stosb_threshold_impl<uint64_t>;

} // namespace wjr