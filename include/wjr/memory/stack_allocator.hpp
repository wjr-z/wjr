#ifndef WJR_STACK_ALLOCATOR_HPP__
#define WJR_STACK_ALLOCATOR_HPP__

#include <algorithm>

#include <wjr/crtp/nonsendable.hpp>
#include <wjr/crtp/trivial_allocator_base.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

template <typename StackAllocator>
class unique_stack_allocator;

template <size_t threshold, size_t cache>
class stack_allocator_object {
    static_assert(threshold <= cache, "threshold must be less than or equal to cache.");

    template <typename StackAllocator>
    friend class unique_stack_allocator;

    constexpr static uint16_t bufsize = 5;

    struct alloc_node {
        char *ptr;
        char *end;
    };

    struct large_stack_top {
        large_stack_top *prev;
        char buffer[];
    };

public:
    struct stack_top {
        char *ptr = nullptr;
        char *end;
        large_stack_top *large;
        uint16_t idx;
    };

private:
    WJR_CONSTEXPR20 void *__large_allocate(size_t n, stack_top &top) {
        auto buffer = (large_stack_top *)malloc(sizeof(large_stack_top) + n);
        buffer->prev = top.large;
        top.large = buffer;
        return buffer->buffer;
    }

    WJR_NOINLINE WJR_CONSTEXPR20 void __small_reallocate(stack_top &top) {
        if (WJR_UNLIKELY(top.end == nullptr)) {
            top.end = m_cache.end;
            top.idx = m_idx;
        }

        ++m_idx;
        if (WJR_UNLIKELY(m_idx == m_size)) {

            if (WJR_UNLIKELY(m_size == m_capacity)) {
                uint16_t new_capacity = m_idx + 2 * (bufsize - 1);
                auto new_ptr =
                    static_cast<alloc_node *>(malloc(new_capacity * sizeof(alloc_node)));
                if (WJR_LIKELY(m_idx != 0)) {
                    std::copy_n(m_ptr, m_idx, new_ptr);
                    free(m_ptr);
                }
                m_ptr = new_ptr;
                m_capacity = new_capacity;
            }

            ++m_size;

            size_t capacity = cache << ((3 * m_idx + 2) / 5);
            auto buffer = static_cast<char *>(malloc(capacity));
            alloc_node node = {buffer, buffer + capacity};
            m_ptr[m_idx] = node;

            if (WJR_UNLIKELY(m_idx == 0)) {
                top.ptr = node.ptr;
                top.end = node.end;
                top.idx = 0;
            }

            m_cache = node;
        } else {
            m_cache = m_ptr[m_idx];
        }

        WJR_ASSERT(top.ptr != nullptr);
        WJR_ASSERT(top.end != nullptr);
    }

    WJR_COLD WJR_CONSTEXPR20 void __small_redeallocate() {
        uint16_t new_size = m_idx + bufsize - 1;

        for (uint16_t i = new_size; i < m_size; ++i) {
            free(m_ptr[i].ptr);
        }

        m_size = new_size;
    }

    WJR_CONSTEXPR20 void __small_deallocate_change_idx(const stack_top &top) {
        uint16_t idx = top.idx;
        m_cache = {top.ptr, top.end};
        m_idx = idx;
        if (WJR_UNLIKELY(m_size - idx >= bufsize)) {
            __small_redeallocate();
        }
    }

    WJR_CONSTEXPR20 void __small_deallocate(const stack_top &top) {
        if (WJR_UNLIKELY(top.ptr == invalid)) {
            return;
        }

        if (WJR_LIKELY(top.end == nullptr)) {
            m_cache.ptr = top.ptr;
        } else {
            __small_deallocate_change_idx(top);
        }
    }

    WJR_CONSTEXPR20 void *__small_allocate(size_t n, stack_top &top) {
        if (WJR_UNLIKELY(static_cast<size_t>(m_cache.end - m_cache.ptr) < n)) {
            __small_reallocate(top);
            WJR_ASSERT_ASSUME_L1(top.end != nullptr);
        }

        WJR_ASSERT_ASSUME_L1(m_cache.ptr != nullptr);
        WJR_ASSERT_ASSUME_L1(top.ptr != nullptr);

        auto ptr = m_cache.ptr;
        m_cache.ptr += n;
        return ptr;
    }

public:
    using value_type = void;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;

    stack_allocator_object() = default;
    stack_allocator_object(stack_allocator_object &) = delete;
    stack_allocator_object(stack_allocator_object &&) = delete;
    stack_allocator_object &operator=(stack_allocator_object &) = delete;
    stack_allocator_object &operator=(stack_allocator_object &&) = delete;
    ~stack_allocator_object() {
        for (uint16_t i = 0; i < m_size; ++i) {
            free(m_ptr[i].ptr);
        }

        free(m_ptr);
    }

    WJR_NODISCARD WJR_MALLOC WJR_CONSTEXPR20 void *allocate(size_t n, stack_top &top) {
        if (WJR_UNLIKELY(n >= threshold)) {
            return __large_allocate(n, top);
        }

        return __small_allocate(n, top);
    }

    WJR_CONSTEXPR20 void deallocate(const stack_top &top) {
        __small_deallocate(top);

        auto buffer = top.large;
        while (WJR_UNLIKELY(buffer != nullptr)) {
            auto prev = buffer->prev;
            free(buffer);
            buffer = prev;
        }
    }

    WJR_CONSTEXPR20 void set(stack_top &top) {
        top.ptr = m_cache.ptr;
        top.end = nullptr;
        top.large = nullptr;
    }

private:
    static char *const invalid;

    alloc_node m_cache = {invalid, invalid};
    uint16_t m_idx = in_place_max;
    alignas(32) uint16_t m_size = 0;
    uint16_t m_capacity = 0;
    alloc_node *m_ptr = nullptr;
};

template <size_t threshold, size_t cache>
char *const stack_allocator_object<threshold, cache>::invalid = (char *)(0x0c);

template <typename StackAllocator>
class singleton_stack_allocator_adapter {
public:
    using value_type = typename StackAllocator::value_type;
    using size_type = typename StackAllocator::size_type;
    using difference_type = typename StackAllocator::difference_type;
    using propagate_on_container_move_assignment =
        typename StackAllocator::propagate_on_container_move_assignment;
    using allocator_type = StackAllocator;
    using stack_top = typename StackAllocator::stack_top;

    static StackAllocator &get_instance() {
        static thread_local StackAllocator instance;
        return instance;
    }
};

template <typename Alloc>
constexpr bool operator==(const singleton_stack_allocator_adapter<Alloc> &,
                          const singleton_stack_allocator_adapter<Alloc> &) {
    return true;
}

template <typename Alloc>
constexpr bool operator!=(const singleton_stack_allocator_adapter<Alloc> &,
                          const singleton_stack_allocator_adapter<Alloc> &) {
    return false;
}

/**
 * @brief A stack allocator for fast simulation of stack memory on the heap, singleton
 * mode.
 *
 * @details When allocating memory less than threadshold,
 * use pre-allocated heap memory, otherwise use malloc to allocate heap memory. \n
 * Notice that the pre-allocated heap memory is not released until the program exits. \n
 * This allocator is not thread-safe and can't be used in container.
 *
 * @tparam threshold The threshold for using malloc to allocate heap memory
 * @tparam cache The size of the first heap memory allocation
 */
template <size_t threshold, size_t cache>
using singleton_stack_allocator_object =
    singleton_stack_allocator_adapter<stack_allocator_object<threshold, cache>>;

/**
 * @details Used for container. This allocator won't deallocate memory allocated by
 * __small_allocate until container is destroyed.
 *
 */
template <typename T, typename StackAllocator>
class weak_stack_allocator;

/**
 * @brief A unique stack allocator for fast simulation of stack memory on the heap.
 *
 * @details When a unique_stack_allocator object is destroyed, all the memory it allocates
 * is released.\n And a new unique_stack_allocator constructed in the lifetime of a
 * unique_stack_allocator object must be destroyed in the current lifetime.
 *
 */
template <size_t threshold, size_t cache>
class unique_stack_allocator<singleton_stack_allocator_object<threshold, cache>>
    : public nonsendable<
          unique_stack_allocator<singleton_stack_allocator_object<threshold, cache>>> {
    using Mybase = nonsendable<
        unique_stack_allocator<singleton_stack_allocator_object<threshold, cache>>>;
    using StackAllocatorObject = singleton_stack_allocator_object<threshold, cache>;
    using stack_top = typename StackAllocatorObject::stack_top;
    using allocator_type = typename StackAllocatorObject::allocator_type;

    template <typename T, typename StackAllocator>
    friend class weak_stack_allocator;

public:
    unique_stack_allocator(const StackAllocatorObject &al) : m_obj(&al) {}
    ~unique_stack_allocator() {
        if (m_top.ptr != nullptr) {
            m_instance->deallocate(m_top);
        }
    }

    WJR_NODISCARD WJR_MALLOC WJR_CONSTEXPR20 void *allocate(size_t n) {
        Mybase::check();

        if (WJR_UNLIKELY(m_top.ptr == nullptr)) {
            m_instance = &m_obj->get_instance();
            m_instance->set(m_top);
            WJR_ASSERT_ASSUME_L1(m_top.ptr != nullptr);
        }

        return m_instance->allocate(n, m_top);
    }

private:
    WJR_NODISCARD WJR_MALLOC WJR_CONSTEXPR20 void *__small_allocate(size_t n) {
        Mybase::check();

        if (WJR_UNLIKELY(m_top.ptr == nullptr)) {
            m_instance = &m_obj->get_instance();
            m_instance->set(m_top);
            WJR_ASSERT_ASSUME_L1(m_top.ptr != nullptr);
        }

        return m_instance->__small_allocate(n, m_top);
    }

    union {
        const StackAllocatorObject *m_obj;
        allocator_type *m_instance;
    };

    stack_top m_top;
};

template <typename StackAllocator>
unique_stack_allocator(const StackAllocator &) -> unique_stack_allocator<StackAllocator>;

template <typename T, size_t threshold, size_t cache>
class weak_stack_allocator<T, singleton_stack_allocator_object<threshold, cache>>
    : trivial_allocator_base_t {
    using StackAllocator = singleton_stack_allocator_object<threshold, cache>;
    using UniqueStackAllocator = unique_stack_allocator<StackAllocator>;

public:
    using value_type = T;
    using size_type = typename StackAllocator::size_type;
    using difference_type = typename StackAllocator::difference_type;
    using propagate_on_container_move_assignment =
        typename StackAllocator::propagate_on_container_move_assignment;

    weak_stack_allocator() = default;
    weak_stack_allocator(UniqueStackAllocator &alloc) : m_alloc(&alloc) {}
    weak_stack_allocator(const weak_stack_allocator &) = default;
    weak_stack_allocator &operator=(const weak_stack_allocator &) = default;
    weak_stack_allocator(weak_stack_allocator &&) = default;
    weak_stack_allocator &operator=(weak_stack_allocator &&) = default;
    ~weak_stack_allocator() = default;

    WJR_NODISCARD WJR_MALLOC WJR_CONSTEXPR20 T *allocate(size_type n) {
        const size_t size = n * sizeof(T);
        if (WJR_UNLIKELY(size >= threshold)) {
            return static_cast<T *>(malloc(size));
        }

        return static_cast<T *>(m_alloc->__small_allocate(size));
    }

    WJR_CONSTEXPR20 void deallocate(WJR_MAYBE_UNUSED T *ptr,
                                    WJR_MAYBE_UNUSED size_type n) {
        const size_t size = n * sizeof(T);
        if (WJR_UNLIKELY(size >= threshold)) {
            free(ptr);
        }
    }

private:
    UniqueStackAllocator *m_alloc = nullptr;
};

} // namespace wjr

#endif // WJR_STACK_ALLOCATOR_HPP__