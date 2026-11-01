#ifndef WJR_TEST_STACK_DOMAIN_API_HPP__
#define WJR_TEST_STACK_DOMAIN_API_HPP__

#include <cstddef>
#include <cstdint>

#include <wjr/memory/stack_allocator.hpp>

#if defined(WJR_STACK_DOMAIN_EXPORTS)
    #define WJR_STACK_DOMAIN_API WJR_SYMBOL_EXPORT
#else
    #define WJR_STACK_DOMAIN_API
#endif

extern "C" {
WJR_STACK_DOMAIN_API const void *wjr_foreign_default_arena() noexcept;
WJR_STACK_DOMAIN_API uintptr_t wjr_foreign_default_scope(wjr::unique_stack_allocator &,
                                                         size_t) noexcept;
WJR_STACK_DOMAIN_API uintptr_t wjr_foreign_bound_scope(wjr::unique_stack_allocator &,
                                                       size_t) noexcept;
WJR_STACK_DOMAIN_API void wjr_foreign_refill(wjr::stack_allocator_object &) noexcept;
WJR_STACK_DOMAIN_API void wjr_foreign_clear_pool() noexcept;
}

#endif // WJR_TEST_STACK_DOMAIN_API_HPP__
