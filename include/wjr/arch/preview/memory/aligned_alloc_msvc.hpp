#ifndef WJR_ARCH_PREVIEW_MEMORY_ALIGNED_ALLOC_MSVC_HPP__
#define WJR_ARCH_PREVIEW_MEMORY_ALIGNED_ALLOC_MSVC_HPP__

#include <malloc.h>

#include <wjr/preprocessor.hpp>

namespace wjr {

WJR_MALLOC inline void *aligned_alloc(size_t size, size_t alignment) noexcept {
    return ::_aligned_malloc(size, alignment);
}

inline void aligned_free(void *ptr) noexcept { ::_aligned_free(ptr); }

} // namespace wjr

#endif // WJR_ARCH_PREVIEW_MEMORY_ALIGNED_ALLOC_MSVC_HPP__
