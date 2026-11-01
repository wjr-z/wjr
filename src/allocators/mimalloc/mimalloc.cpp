#include <new>

#include <wjr/macros/exceptions.hpp>
#include <wjr/memory/mimalloc.hpp>

namespace wjr {

void mimalloc_detail::allocation_failure() { WJR_THROW(std::bad_alloc{}); }

void *mimalloc_memory_resource::do_allocate(std::size_t bytes, std::size_t alignment) {
    if (auto *pointer = mi_malloc_aligned(bytes, alignment)) {
        return pointer;
    }
    mimalloc_detail::allocation_failure();
}

void mimalloc_memory_resource::do_deallocate(void *pointer, std::size_t bytes,
                                             std::size_t alignment) noexcept {
    mi_free_size_aligned(pointer, bytes, alignment);
}

bool mimalloc_memory_resource::do_is_equal(const std::pmr::memory_resource &other) const noexcept {
    return this == &other;
}

} // namespace wjr
