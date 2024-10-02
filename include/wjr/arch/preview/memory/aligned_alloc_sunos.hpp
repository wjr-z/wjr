#ifndef WJR_ARCH_PREVIEW_MEMORY_ALIGNED_ALLOC_SUNOS_HPP__
#define WJR_ARCH_PREVIEW_MEMORY_ALIGNED_ALLOC_SUNOS_HPP__

#include <cstdlib>

#include <wjr/memory/align.hpp>

namespace wjr {

WJR_MALLOC inline void *aligned_alloc(size_t size, size_t alignment) noexcept {
    WJR_ASSERT(has_single_bit(alignment));
    return ::memalign(alignment, size);
}

inline void aligned_free(void *ptr) noexcept { ::free(ptr); }

} // namespace wjr

#endif // WJR_ARCH_PREVIEW_MEMORY_ALIGNED_ALLOC_SUNOS_HPP__