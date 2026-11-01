#include "api.hpp"

#include <cstring>

namespace {
int check_pool_ownership() {
    wjr::stack_allocator_object arena;
    wjr::stack_allocator_object::stack_context owner(&arena);
    std::memset(arena.allocate(16, owner), 0x2a, 16);
    wjr_foreign_refill(arena);
    wjr_foreign_clear_pool();
    /// @note Reusing the retained second slab must not touch memory freed by the foreign pool.
    for (size_t i = 0; i < 64; ++i) {
        auto *bytes = static_cast<unsigned char *>(arena.allocate(1024, owner));
        std::memset(bytes, 0x6b, 1024);
        if (bytes[0] != 0x6b || bytes[1023] != 0x6b)
            return 1;
    }
    return 0;
}
} // namespace

int main(int argc, char **) {
    if (argc > 1)
        return check_pool_ownership();
    if (wjr_foreign_default_arena() == std::addressof(wjr::mem::_stack_allocator_singleton_object))
        return 2;
    wjr::unique_stack_allocator owner;
    auto *kept = static_cast<unsigned char *>(owner.allocate(16));
    kept[0] = 0x3d;
    const auto local = wjr_foreign_default_scope(owner, 16);
    if (reinterpret_cast<uintptr_t>(owner.allocate(16)) == local)
        return 3;
    const auto bound = wjr_foreign_bound_scope(owner, 16);
    if (reinterpret_cast<uintptr_t>(owner.allocate(16)) != bound)
        return 4;
    if (kept[0] != 0x3d)
        return 5;
    return check_pool_ownership();
}
