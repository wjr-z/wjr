#ifndef WJR_ARCH_PREVIEW_MEMORY_ALIGNED_ALLOC_MALLOC_HPP__
#define WJR_ARCH_PREVIEW_MEMORY_ALIGNED_ALLOC_MALLOC_HPP__

#include <cstdlib>
#include <memory>
#include <type_traits>

#include <wjr/preprocessor.hpp>

namespace wjr {

WJR_MALLOC inline void *aligned_alloc(size_t size, size_t alignment) noexcept {
    enum { N = std::alignment_of<void *>::value };
    if (alignment < N) {
        alignment = N;
    }
    std::size_t n = size + alignment - N;
    void *p = std::malloc(sizeof(void *) + n);
    if (p) {
        void *r = static_cast<char *>(p) + sizeof(void *);
        (void)std::align(alignment, size, r, n);
        *(static_cast<void **>(r) - 1) = p;
        p = r;
    }
    return p;
}

inline void aligned_free(void *ptr) noexcept {
    if (ptr) {
        std::free(*(static_cast<void **>(ptr) - 1));
    }
}

} // namespace wjr

#endif // WJR_ARCH_PREVIEW_MEMORY_ALIGNED_ALLOC_MALLOC_HPP__