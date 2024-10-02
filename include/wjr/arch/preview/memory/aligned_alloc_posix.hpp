#ifndef WJR_ARCH_PREVIEW_MEMORY_ALIGNED_ALLOC_POSIX_HPP__
#define WJR_ARCH_PREVIEW_MEMORY_ALIGNED_ALLOC_POSIX_HPP__

#include <cstdlib>

#include <wjr/memory/align.hpp>

namespace wjr {

WJR_MALLOC inline void *aligned_alloc(size_t size, size_t alignment) noexcept {
    WJR_ASSERT(has_single_bit(alignment));
    if (alignment < sizeof(void *)) {
        alignment = sizeof(void *);
    }
    void *p;
    if (::posix_memalign(&p, alignment, size) != 0) {
        p = 0;
    }
    return p;
}

inline void aligned_free(void *ptr) noexcept { ::free(ptr); }

} // namespace wjr

#endif // WJR_ARCH_PREVIEW_MEMORY_ALIGNED_ALLOC_POSIX_HPP__
