#include "api.hpp"

#include <cstring>

#include <wjr/memory/automatic_free_pool.hpp>

extern "C" const void *wjr_foreign_default_arena() noexcept {
    return std::addressof(wjr::mem::_stack_allocator_singleton_object);
}

extern "C" uintptr_t wjr_foreign_default_scope(wjr::unique_stack_allocator &owner,
                                               size_t bytes) noexcept {
    wjr::unique_stack_allocator local;
    return reinterpret_cast<uintptr_t>(owner.allocate(bytes));
}

extern "C" uintptr_t wjr_foreign_bound_scope(wjr::unique_stack_allocator &owner,
                                             size_t bytes) noexcept {
    auto local = owner.make_scope();
    return reinterpret_cast<uintptr_t>(owner.allocate(bytes));
}

extern "C" void wjr_foreign_refill(wjr::stack_allocator_object &arena) noexcept {
    wjr::stack_allocator_object::stack_context scope(&arena);
    for (size_t i = 0; i < 64; ++i) {
        std::memset(arena.allocate(1024, scope), static_cast<int>(i), 1024);
    }
}

extern "C" void wjr_foreign_clear_pool() noexcept {
    /// @note Model destruction of the foreign module's pool without unloading its code.
    wjr::mem::_get_stack_free_pool().clear();
}
