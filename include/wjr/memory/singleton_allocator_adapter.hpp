#ifndef WJR_MEMORY_SINGLETON_ALLOCATOR_ADAPTER_HPP__
#define WJR_MEMORY_SINGLETON_ALLOCATOR_ADAPTER_HPP__

#include <wjr/assert.hpp>
#include <wjr/preprocessor.hpp>


namespace wjr {

template <typename Alloc, bool ThreadLocal = false>
class singleton_allocator_adapter {
    static Alloc &get_instance() {
        if constexpr (ThreadLocal) {
            static thread_local Alloc instance;
            return instance;
        } else {
            static Alloc instance;
            return instance;
        }
    }

public:
    using value_type = typename Alloc::value_type;
    using size_type = typename Alloc::size_type;
    using difference_type = typename Alloc::difference_type;
    using propagate_on_container_move_assignment =
        typename Alloc::propagate_on_container_move_assignment;

    // Allocate memory
    WJR_CONSTEXPR20 value_type *allocate(size_type n) const {
        return get_instance().allocate(n);
    }

    // Deallocate memory
    WJR_CONSTEXPR20 void deallocate(value_type *p, size_type n) const {
        get_instance().deallocate(p, n);
    }
};

template <typename Alloc, bool ThreadLocal>
constexpr bool operator==(const singleton_allocator_adapter<Alloc, ThreadLocal> &,
                          const singleton_allocator_adapter<Alloc, ThreadLocal> &) {
    return true;
}

template <typename Alloc, bool ThreadLocal>
constexpr bool operator!=(const singleton_allocator_adapter<Alloc, ThreadLocal> &,
                          const singleton_allocator_adapter<Alloc, ThreadLocal> &) {
    return false;
}

} // namespace wjr

#endif // WJR_MEMORY_SINGLETON_ALLOCATOR_ADAPTER_HPP__