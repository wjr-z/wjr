#include <memory>
#include <wjr/memory/uninitialized.hpp>

extern "C" int *relocate_int(int *src, int *dst) {
    std::allocator<int> alloc;
    return wjr::uninitialized_relocate_at_using_allocator(src, dst, alloc);
}

struct resource {
    resource(resource &&) noexcept;
    ~resource();
};

extern "C" resource *relocate_resource(resource *src, resource *dst) {
    std::allocator<resource> alloc;
    return wjr::uninitialized_relocate_at_using_allocator(src, dst, alloc);
}
