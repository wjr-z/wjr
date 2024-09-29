#ifndef WJR_STACK_ALLOCATOR_HPP__
#define WJR_STACK_ALLOCATOR_HPP__

#include <algorithm>

#include <wjr/memory/memory_pool.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

template <typename StackAllocator>
class unique_stack_allocator;

template <size_t Cache>
class stack_allocator_object {
    template <typename StackAllocator>
    friend class unique_stack_allocator;

    constexpr static uint16_t bufsize = 5;

    struct alloc_node {
        char *ptr;
        char *end;
    };

    struct large_stack_top {
        large_stack_top *prev;
    };

public:
    struct stack_top {
        char *ptr;
        uint16_t idx;
        large_stack_top *large;
    };

private:
    WJR_CONSTEXPR20 void *__large_allocate(size_t n, stack_top &top) noexcept {
        auto *const buffer =
            static_cast<large_stack_top *>(malloc(sizeof(large_stack_top) + n));
        buffer->prev = top.large;
        top.large = buffer;
        return buffer + 1;
    }

    WJR_NOINLINE WJR_CONSTEXPR20 void __small_reallocate(stack_top &top) noexcept {
        if (WJR_UNLIKELY(top.idx == UINT16_MAX)) {
            top.idx = m_idx;
        }

        ++m_idx;
        if (WJR_UNLIKELY(m_idx == m_size)) {

            if (WJR_UNLIKELY(m_size == m_capacity)) {
                const uint16_t new_capacity = m_idx + 2 * (bufsize - 1);
                memory_pool<alloc_node> pool;
                auto *const new_ptr = pool.allocate(new_capacity);
                if (WJR_LIKELY(m_idx != 0)) {
                    std::copy_n(m_ptr, m_idx, new_ptr);
                    pool.deallocate(m_ptr, m_capacity);
                }
                m_ptr = new_ptr;
                m_capacity = new_capacity;
            }

            ++m_size;

            const size_t capacity = Cache << ((3 * m_idx + 2) / 5);
            memory_pool<char> pool;
            auto *const buffer = pool.chunk_allocate(capacity);
            const alloc_node node = {buffer, buffer + capacity};
            m_ptr[m_idx] = node;

            if (WJR_UNLIKELY(m_idx == 0)) {
                top.ptr = node.ptr;
                top.idx = 0;
            }

            m_cache = node;
        } else {
            m_cache = m_ptr[m_idx];
        }

        WJR_ASSERT(top.ptr != nullptr);
    }

    WJR_NOINLINE WJR_CONSTEXPR20 void __small_redeallocate() noexcept {
        const uint16_t new_size = m_idx + bufsize - 1;
        memory_pool<char> pool;

        for (uint16_t i = new_size; i < m_size; ++i) {
            pool.chunk_deallocate(m_ptr[i].ptr, m_ptr[i].end - m_ptr[i].ptr);
        }

        m_size = new_size;
    }

    WJR_CONSTEXPR20 void __small_deallocate(const stack_top &top) noexcept {
        if (WJR_UNLIKELY(top.ptr == nullptr)) {
            return;
        }

        m_cache.ptr = top.ptr;

        if (WJR_UNLIKELY(top.idx != UINT16_MAX)) {
            const uint16_t idx = top.idx;
            m_cache.end = m_ptr[idx].end;
            m_idx = idx;
            if (WJR_UNLIKELY(m_size - idx >= bufsize)) {
                __small_redeallocate();
            }
        }
    }

    WJR_MALLOC WJR_CONSTEXPR20 void *__small_allocate(size_t n, stack_top &top) noexcept {
        if (WJR_UNLIKELY(static_cast<size_t>(m_cache.end - m_cache.ptr) < n)) {
            __small_reallocate(top);
        }

        WJR_ASSERT_ASSUME_L2(m_cache.ptr != nullptr);
        WJR_ASSERT_ASSUME_L2(top.ptr != nullptr);

        auto *const ret = m_cache.ptr;
        m_cache.ptr += n;
        return ret;
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
    ~stack_allocator_object() = default;

    WJR_NODISCARD WJR_MALLOC WJR_CONSTEXPR20 void *allocate(size_t n, stack_top &top,
                                                            size_t threshold) noexcept {
        if (WJR_UNLIKELY(n >= threshold)) {
            return __large_allocate(n, top);
        }

        return __small_allocate(n, top);
    }

    WJR_CONSTEXPR20 void deallocate(const stack_top &top) noexcept {
        __small_deallocate(top);

        auto *buffer = top.large;
        while (WJR_UNLIKELY(buffer != nullptr)) {
            auto *const prev = buffer->prev;
            free(buffer);
            buffer = prev;
        }
    }

    WJR_CONSTEXPR20 void set(stack_top &top) const noexcept {
        top.ptr = m_cache.ptr;
        top.idx = UINT16_MAX;
        top.large = nullptr;
    }

private:
    alloc_node m_cache = {nullptr, nullptr};
    uint16_t m_idx = UINT16_MAX;
    uint16_t m_size = 0;
    uint16_t m_capacity = 0;
    alloc_node *m_ptr = nullptr;
};

/**
 * @brief A stack allocator for fast simulation of stack memory on the heap, singleton
 * mode.
 *
 * @tparam Cache The size of the first heap memory allocation
 */
template <size_t Cache, size_t DefaultThreshold>
struct singleton_stack_allocator_object {
    using Instance = stack_allocator_object<Cache>;
    constexpr static size_t __default_threshold = DefaultThreshold;

    static_assert(DefaultThreshold < Cache, "DefaultThreshold must be less than Cache.");

    static Instance &get_instance() noexcept {
        static thread_local Instance instance;
        return instance;
    }
};

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
template <typename StackAllocator>
class unique_stack_allocator {
    using Instance = typename StackAllocator::Instance;
    using stack_top = typename Instance::stack_top;

    constexpr static size_t __default_threshold = StackAllocator::__default_threshold;

    template <typename T, typename S>
    friend class weak_stack_allocator;

public:
    WJR_INTRINSIC_INLINE unique_stack_allocator(const StackAllocator &al) noexcept
        : m_instance(&(al.get_instance())) {
        m_instance->set(m_top);
    }

    unique_stack_allocator(const unique_stack_allocator &) = delete;
    unique_stack_allocator(unique_stack_allocator &&) = delete;
    unique_stack_allocator &operator=(const unique_stack_allocator &) = delete;
    unique_stack_allocator &operator=(unique_stack_allocator &&) = delete;

    ~unique_stack_allocator() noexcept { m_instance->deallocate(m_top); }

    WJR_NODISCARD WJR_MALLOC WJR_INTRINSIC_INLINE void *
    allocate(size_t n, size_t threshold = __default_threshold) noexcept {
        return m_instance->allocate(n, m_top, threshold);
    }

private:
    WJR_NODISCARD WJR_MALLOC WJR_INTRINSIC_INLINE void *
    __small_allocate(size_t n) noexcept {
        return m_instance->__small_allocate(n, m_top);
    }

    Instance *m_instance;
    stack_top m_top;
};

template <typename StackAllocator>
unique_stack_allocator(const StackAllocator &) -> unique_stack_allocator<StackAllocator>;

/**
 * @brief Point to unique_stack_allocator.
 *
 * @details Use a pointer to unique_stack_allocator to allocate memory. This class must be
 * used carefully. If recursively using it as a reference and allocating memory,
 * unique_stack_allocator should be avoided from being reused in the current function.
 *
 */
template <typename T, size_t Cache, size_t DefaultThreshold>
class weak_stack_allocator<T, singleton_stack_allocator_object<Cache, DefaultThreshold>> {
    using StackAllocator = singleton_stack_allocator_object<Cache, DefaultThreshold>;
    using UniqueStackAllocator = unique_stack_allocator<StackAllocator>;

    constexpr static size_t __default_threshold = StackAllocator::__default_threshold;

    template <typename U, typename A>
    friend class weak_stack_allocator;

public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_trivially_allocator = std::true_type;

    template <typename Other>
    struct rebind {
        using other = weak_stack_allocator<Other, StackAllocator>;
    };

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(weak_stack_allocator);

    weak_stack_allocator(UniqueStackAllocator &alloc) noexcept : m_alloc(&alloc) {}

    template <typename U>
    weak_stack_allocator(const weak_stack_allocator<U, StackAllocator> &other) noexcept
        : m_alloc(other.m_alloc) {}

    WJR_NODISCARD WJR_MALLOC WJR_CONSTEXPR20 T *allocate(size_type n) noexcept {
        const size_t size = n * sizeof(T);
        if (WJR_UNLIKELY(size >= __default_threshold)) {
            return static_cast<T *>(malloc(size));
        }

        return static_cast<T *>(m_alloc->__small_allocate(size));
    }

    WJR_CONSTEXPR20 void deallocate(WJR_MAYBE_UNUSED T *ptr,
                                    WJR_MAYBE_UNUSED size_type n) noexcept {
        const size_t size = n * sizeof(T);
        if (WJR_UNLIKELY(size >= __default_threshold)) {
            free(ptr);
        }
    }

private:
    UniqueStackAllocator *m_alloc = nullptr;
};

} // namespace wjr

#endif // WJR_STACK_ALLOCATOR_HPP__