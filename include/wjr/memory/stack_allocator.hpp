#ifndef WJR_STACK_ALLOCATOR_HPP__
#define WJR_STACK_ALLOCATOR_HPP__

#include <algorithm>

#include <wjr/crtp/nonsendable.hpp>
#include <wjr/crtp/trivially_allocator_base.hpp>
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
        char buffer[];
    };

public:
    struct stack_top {
        char *ptr = nullptr;
        char *end;
        uint16_t idx;
        large_stack_top *large;
    };

private:
    WJR_CONSTEXPR20 void *__large_allocate(size_t n, stack_top &top) {
        const auto buffer = (large_stack_top *)malloc(sizeof(large_stack_top) + n);
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

            const size_t capacity = Cache << ((3 * m_idx + 2) / 5);
            const auto buffer = static_cast<char *>(malloc(capacity));
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
        const uint16_t new_size = m_idx + bufsize - 1;

        for (uint16_t i = new_size; i < m_size; ++i) {
            free(m_ptr[i].ptr);
        }

        m_size = new_size;
    }

    WJR_CONSTEXPR20 void __small_deallocate_change_idx(const stack_top &top) {
        const uint16_t idx = top.idx;
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

        const auto ptr = m_cache.ptr;
        m_cache.ptr += n;
        return ptr;
    }

public:
    using value_type = void;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;

    stack_allocator_object() noexcept = default;
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

    WJR_NODISCARD WJR_MALLOC WJR_CONSTEXPR20 void *allocate(size_t n, stack_top &top,
                                                            size_t threshold) {
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

    WJR_CONSTEXPR20 void set(stack_top &top) const noexcept {
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

template <size_t Cache>
char *const stack_allocator_object<Cache>::invalid = (char *)(0x0c);

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
class unique_stack_allocator
    : public nonsendable<unique_stack_allocator<StackAllocator>> {
    using Mybase = nonsendable<unique_stack_allocator<StackAllocator>>;
    using Instance = typename StackAllocator::Instance;
    using stack_top = typename Instance::stack_top;

    constexpr static size_t __default_threshold = StackAllocator::__default_threshold;

    template <typename T, typename S>
    friend class weak_stack_allocator;

public:
    unique_stack_allocator(const StackAllocator &al) noexcept : m_obj(&al) {}
    ~unique_stack_allocator() {
        if (m_top.ptr != nullptr) {
            m_instance->deallocate(m_top);
        }
    }

    WJR_NODISCARD WJR_MALLOC WJR_CONSTEXPR20 void *
    allocate(size_t n, size_t threshold = __default_threshold) {
        Mybase::check();

        if (WJR_UNLIKELY(m_top.ptr == nullptr)) {
            m_instance = &m_obj->get_instance();
            m_instance->set(m_top);
            WJR_ASSERT_ASSUME_L1(m_top.ptr != nullptr);
        }

        return m_instance->allocate(n, m_top, threshold);
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
        const StackAllocator *m_obj;
        Instance *m_instance;
    };

    stack_top m_top;
};

template <typename StackAllocator>
unique_stack_allocator(const StackAllocator &) -> unique_stack_allocator<StackAllocator>;

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

    weak_stack_allocator() noexcept = default;
    weak_stack_allocator(UniqueStackAllocator &alloc) noexcept : m_alloc(&alloc) {}
    weak_stack_allocator(const weak_stack_allocator &) noexcept = default;
    weak_stack_allocator &operator=(const weak_stack_allocator &) noexcept = default;
    weak_stack_allocator(weak_stack_allocator &&) noexcept = default;
    weak_stack_allocator &operator=(weak_stack_allocator &&) noexcept = default;
    ~weak_stack_allocator() noexcept = default;

    template <typename U>
    weak_stack_allocator(const weak_stack_allocator<U, StackAllocator> &other) noexcept
        : m_alloc(other.m_alloc) {}

    WJR_NODISCARD WJR_MALLOC WJR_CONSTEXPR20 T *allocate(size_type n) {
        const size_t size = n * sizeof(T);
        if (WJR_UNLIKELY(size >= __default_threshold)) {
            return static_cast<T *>(malloc(size));
        }

        return static_cast<T *>(m_alloc->__small_allocate(size));
    }

    WJR_CONSTEXPR20 void deallocate(WJR_MAYBE_UNUSED T *ptr,
                                    WJR_MAYBE_UNUSED size_type n) {
        const size_t size = n * sizeof(T);
        if (WJR_UNLIKELY(size >= __default_threshold)) {
            free(ptr);
        }
    }

private:
    UniqueStackAllocator *m_alloc = nullptr;
};

template <typename T, typename StackAllocator>
class auto_weak_stack_allocator;

/**
 * @brief Automatically deallocate memory.
 *
 * @details Unlike weak_stack_allocator, weak_stack_allocator need to mannuallly call
 * deallocate to release memory. auto_weak_stack_allocator won't deallocate memory.
 * When unique_stack_allocator object is destroyed, all the memory it allocates is
 * released.
 *
 */
template <typename T, size_t Cache, size_t DefaultThreshold>
class auto_weak_stack_allocator<
    T, singleton_stack_allocator_object<Cache, DefaultThreshold>> {
    using StackAllocator = singleton_stack_allocator_object<Cache, DefaultThreshold>;
    using UniqueStackAllocator = unique_stack_allocator<StackAllocator>;

    constexpr static size_t __default_threshold = StackAllocator::__default_threshold;

    template <typename U, typename A>
    friend class auto_weak_stack_allocator;

public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_trivially_allocator = std::true_type;

    auto_weak_stack_allocator() noexcept = default;
    auto_weak_stack_allocator(UniqueStackAllocator &alloc) noexcept : m_alloc(&alloc) {}
    auto_weak_stack_allocator(const auto_weak_stack_allocator &) noexcept = default;
    auto_weak_stack_allocator &
    operator=(const auto_weak_stack_allocator &) noexcept = default;
    auto_weak_stack_allocator(auto_weak_stack_allocator &&) noexcept = default;
    auto_weak_stack_allocator &operator=(auto_weak_stack_allocator &&) noexcept = default;
    ~auto_weak_stack_allocator() noexcept = default;

    template <typename U>
    auto_weak_stack_allocator(
        const auto_weak_stack_allocator<U, StackAllocator> &other) noexcept
        : m_alloc(other.m_alloc) {}

    WJR_NODISCARD WJR_MALLOC WJR_CONSTEXPR20 T *allocate(size_type n) {
        const size_t size = n * sizeof(T);
        return static_cast<T *>(m_alloc->allocate(size));
    }

    WJR_CONSTEXPR20 void deallocate(WJR_MAYBE_UNUSED T *ptr,
                                    WJR_MAYBE_UNUSED size_type n) {}

private:
    UniqueStackAllocator *m_alloc = nullptr;
};

} // namespace wjr

#endif // WJR_STACK_ALLOCATOR_HPP__