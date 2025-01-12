#ifndef WJR_ARCH_X86_CPUINFO_HPP__
#define WJR_ARCH_X86_CPUINFO_HPP__

#include <cstdint>

#include <cpuinfo_x86.h>

namespace wjr {
extern cpu_features::X86Info x86_cpu_info;
extern bool __x86_is_enhanced_rep;

extern size_t __x86_rep_stosb_threshold_u16;
extern size_t __x86_rep_stosb_threshold_u32;
extern size_t __x86_rep_stosb_threshold_u64;

} // namespace wjr

#endif // WJR_ARCH_X86_CPUINFO_HPP__