#ifndef WJR_ARCH_PREVIEW_MEMORY_ALIGNED_ALLOC_MINGW_HPP__
#define WJR_ARCH_PREVIEW_MEMORY_ALIGNED_ALLOC_MINGW_HPP__

#include <malloc.h>

#include <wjr/config/attribute.hpp>

namespace wjr {

WJR_MALLOC inline void *aligned_alloc(size_t size, size_t alignment) noexcept {
    return ::__mingw_aligned_malloc(size, alignment);
}

inline void aligned_free(void *ptr) noexcept { ::__mingw_aligned_free(ptr); }

} // namespace wjr

#endif // WJR_ARCH_PREVIEW_MEMORY_ALIGNED_ALLOC_MINGW_HPP__