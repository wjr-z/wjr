#ifndef WJR_ARCH_X86_CPUINFO_HPP__
#define WJR_ARCH_X86_CPUINFO_HPP__

#include <cstddef>

#include <cpuinfo_x86.h>

namespace wjr {
extern cpu_features::X86Info x86_cpu_info;

extern size_t _x86_rep_stosb_threshold_u16;
extern size_t _x86_rep_stosb_threshold_u32;
extern size_t _x86_rep_stosb_threshold_u64;

} // namespace wjr

#endif // WJR_ARCH_X86_CPUINFO_HPP__