#include <wjr/arch/x86/cpuinfo.hpp>

namespace wjr {
cpu_features::X86Info x86_cpu_info = cpu_features::GetX86Info();
bool __x86_is_enhanced_rep = x86_cpu_info.features.erms != 0;

#ifndef WJR_X86_REP_STOSB_DEFAULT_THRESHOLD
    #define WJR_X86_REP_STOSB_DEFAULT_THRESHOLD ((size_t)(1) << 24)
#endif

inline constexpr size_t __x86_rep_stosb_default_threshold = WJR_X86_REP_STOSB_DEFAULT_THRESHOLD;

template <typename T>
size_t __x86_rep_stosb_threshold =
    __x86_is_enhanced_rep ? __x86_rep_stosb_default_threshold / sizeof(T)
                          : std::numeric_limits<size_t>::max();

template size_t __x86_rep_stosb_threshold<uint16_t>;
template size_t __x86_rep_stosb_threshold<uint32_t>;
template size_t __x86_rep_stosb_threshold<uint64_t>;

} // namespace wjr