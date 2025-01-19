#ifndef WJR_STACK_ALLOCATOR_HPP__
#define WJR_STACK_ALLOCATOR_HPP__

#include <algorithm>

#include <wjr/math/literals.hpp>
#include <wjr/memory/memory_pool.hpp>
#include <wjr/string.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

inline constexpr size_t stack_allocator_threshold = 16_KB;

class unique_stack_allocator;

class stack_allocator_object {
    friend class unique_stack_allocator;

    constexpr static size_t Cache = 32_KB;
    constexpr static uint_fast16_t bufsize = 4;

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
        uint_fast16_t idx;
        large_stack_top *large;
    };

private:
    void *__large_allocate(size_t n, stack_top &top) noexcept {
        auto *const buffer = static_cast<large_stack_top *>(malloc(sizeof(large_stack_top) + n));
        buffer->prev = top.large;
        top.large = buffer;
        return buffer + 1;
    }

    WJR_NOINLINE void __small_reallocate(stack_top &top) noexcept;

    WJR_NOINLINE void __small_redeallocate() noexcept {
        const uint_fast16_t new_size = m_idx + bufsize - 1;
        __default_alloc_template__ pool;

        for (uint_fast16_t i = new_size; i < m_size; ++i) {
            pool.chunk_deallocate(m_ptr[i].ptr, m_ptr[i].end - m_ptr[i].ptr);
        }

        m_size = new_size;
    }

    void __small_deallocate(const stack_top &top) noexcept {
        if (WJR_UNLIKELY(top.ptr == nullptr)) {
            return;
        }

        m_cache.ptr = top.ptr;
        if (top.idx != UINT_FAST16_MAX) {
            const uint_fast16_t idx = top.idx;
            m_cache.end = m_ptr[idx].end;
            m_idx = idx;
            if (WJR_UNLIKELY(m_size - idx >= bufsize)) {
                __small_redeallocate();
            }
        }
    }

    WJR_MALLOC void *__small_allocate(size_t n, stack_top &top) noexcept {
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

    WJR_NODISCARD WJR_MALLOC void *allocate(size_t n, stack_top &top) noexcept {
        if (WJR_UNLIKELY(static_cast<size_t>(m_cache.end - m_cache.ptr) < n)) {
            if (n >= stack_allocator_threshold) {
                return __large_allocate(n, top);
            }

            __small_reallocate(top);
        }

        WJR_ASSERT_ASSUME_L2(m_cache.ptr != nullptr);
        WJR_ASSERT_ASSUME_L2(top.ptr != nullptr);

        auto *const ret = m_cache.ptr;
        m_cache.ptr += n;
        return ret;
    }

    void deallocate(const stack_top &top) noexcept {
        __small_deallocate(top);

        auto *buffer = top.large;
        while (WJR_UNLIKELY(buffer != nullptr)) {
            auto *const prev = buffer->prev;
            free(buffer);
            buffer = prev;
        }
    }

    void set(stack_top &top) const noexcept {
        top.ptr = m_cache.ptr;
        top.idx = UINT_FAST16_MAX;
        top.large = nullptr;
    }

private:
    alloc_node m_cache = {nullptr, nullptr};
    uint_fast16_t m_idx = UINT_FAST16_MAX;
    uint_fast16_t m_size = 0;
    uint_fast16_t m_capacity = 0;
    alloc_node *m_ptr = nullptr;
};

/**
 * @brief A stack allocator for fast simulation of stack memory on the heap,
 * singleton mode.
 */
struct singleton_stack_allocator_object {
    using Instance = stack_allocator_object;

    static Instance &get_instance() noexcept {
        static thread_local Instance instance;
        return instance;
    }
};

/**
 * @details Used for container. This allocator won't deallocate memory allocated
 * by
 * __small_allocate until container is destroyed.
 *
 */
template <typename T>
class weak_stack_allocator;

/**
 * @brief A unique stack allocator for fast simulation of stack memory on the
 * heap.
 *
 * @details When a unique_stack_allocator object is destroyed, all the memory it
 * allocates is released.\n And a new unique_stack_allocator constructed in the
 * lifetime of a unique_stack_allocator object must be destroyed in the current
 * lifetime.
 *
 */
class unique_stack_allocator {
    using Object = singleton_stack_allocator_object;
    using Instance = typename Object::Instance;
    using stack_top = typename Instance::stack_top;

    template <typename T>
    friend class weak_stack_allocator;

public:
    WJR_INTRINSIC_INLINE
    unique_stack_allocator() noexcept : m_instance(std::addressof(Object::get_instance())) {
        m_instance->set(m_top);
    }

    unique_stack_allocator(const unique_stack_allocator &) = delete;
    unique_stack_allocator(unique_stack_allocator &&) = delete;
    unique_stack_allocator &operator=(const unique_stack_allocator &) = delete;
    unique_stack_allocator &operator=(unique_stack_allocator &&) = delete;

    ~unique_stack_allocator() noexcept { m_instance->deallocate(m_top); }

    WJR_NODISCARD WJR_MALLOC WJR_INTRINSIC_INLINE void *allocate(size_t n) noexcept {
        return m_instance->allocate(n, m_top);
    }

    template <typename Ty>
    WJR_NODISCARD WJR_MALLOC WJR_INTRINSIC_INLINE Ty *allocate(size_t n) noexcept {
        return static_cast<Ty *>(allocate(n * sizeof(Ty)));
    }

private:
    WJR_NODISCARD WJR_MALLOC WJR_INTRINSIC_INLINE void *__small_allocate(size_t n) noexcept {
        return m_instance->__small_allocate(n, m_top);
    }

    Instance *m_instance;
    stack_top m_top;
};

/**
 * @brief Point to unique_stack_allocator.
 *
 * @details Use a pointer to unique_stack_allocator to allocate memory. This
 * class must be used carefully. If recursively using it as a reference and
 * allocating memory, unique_stack_allocator should be avoided from being reused
 * in the current function.
 *
 */
template <typename T>
class weak_stack_allocator {
    using StackAllocator = singleton_stack_allocator_object;
    using UniqueStackAllocator = unique_stack_allocator;

    template <typename U>
    friend class weak_stack_allocator;

public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_trivially_allocator = std::true_type;

    template <typename Other>
    struct rebind {
        using other = weak_stack_allocator<Other>;
    };

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(weak_stack_allocator);

    weak_stack_allocator(UniqueStackAllocator &alloc) noexcept : m_alloc(std::addressof(alloc)) {}

    template <typename U>
    weak_stack_allocator(const weak_stack_allocator<U> &other) noexcept : m_alloc(other.m_alloc) {}

    WJR_NODISCARD WJR_MALLOC T *allocate(size_type n) noexcept {
        const size_t size = n * sizeof(T);
        if (WJR_UNLIKELY(size >= stack_allocator_threshold)) {
            return static_cast<T *>(malloc(size));
        }

        return static_cast<T *>(m_alloc->__small_allocate(size));
    }

    void deallocate(WJR_MAYBE_UNUSED T *ptr, WJR_MAYBE_UNUSED size_type n) noexcept {
        const size_t size = n * sizeof(T);
        if (WJR_UNLIKELY(size >= stack_allocator_threshold)) {
            free(ptr);
        }
    }

private:
    UniqueStackAllocator *m_alloc = nullptr;
};

namespace string_detail {
using weak_stack_allocator_string =
    std::basic_string<char, std::char_traits<char>, weak_stack_allocator<char>>;
}

WJR_REGISTER_STRING_UNINITIALIZED_RESIZE(weak_stack_allocator_string,
                                         string_detail::weak_stack_allocator_string);

} // namespace wjr

#endif // WJR_STACK_ALLOCATOR_HPP__