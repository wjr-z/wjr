#ifndef WJR_STACK_ALLOCATOR_HPP__
#define WJR_STACK_ALLOCATOR_HPP__

#include <algorithm>

#include <wjr/math/literals.hpp>
#include <wjr/memory/aligned_alloc.hpp>
#include <wjr/string.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

inline constexpr size_t stack_allocator_threshold = 16_KB;

class unique_stack_allocator;

class stack_allocator_object {
    friend class unique_stack_allocator;

    constexpr static size_t cache_size = align_up(16_KB, mem::default_new_alignment);
    constexpr static uint_fast32_t bufsize = 3;

    struct alloc_node {
        std::byte *ptr;
        std::byte *end;
    };

    struct large_memory {
        large_memory *prev;
    };

    static constexpr size_t aligned_header_size =
        align_up(sizeof(large_memory), mem::default_new_alignment);

public:
    class stack_context {
        friend class stack_allocator_object;

    public:
        stack_context(stack_allocator_object *object) noexcept;
        ~stack_context() noexcept;

        stack_context(const stack_context &) = delete;
        stack_context(stack_context &&) = delete;

        stack_allocator_object *object() noexcept { return m_object; }

    private:
        stack_allocator_object *m_object;
        std::byte *m_ptr;
        uint_fast32_t m_idx;
        large_memory *m_large;
    };

private:
    static void *__large_allocate(size_t n, large_memory *&mem) noexcept {
        auto *const raw = mem::__default_allocate(n + aligned_header_size, std::nothrow);
        auto *const buffer = static_cast<large_memory *>(raw);
        buffer->prev = mem;
        mem = buffer;
        return static_cast<void *>(static_cast<std::byte *>(raw) + aligned_header_size);
    }

    static void __large_deallocate(large_memory *buffer) noexcept;

    WJR_MALLOC void *__small_allocate(size_t n, stack_context &context) noexcept {
        if (WJR_UNLIKELY(static_cast<size_t>(m_cache.end - m_cache.ptr) < n)) {
            if (auto ptr = __small_reallocate(); ptr)
                context.m_ptr = static_cast<std::byte *>(ptr);
        }

        WJR_ASSERT_ASSUME_L2(m_cache.ptr != nullptr);

        auto *const ret = wjr::assume_aligned<mem::default_new_alignment>(m_cache.ptr);
        m_cache.ptr += align_up(n, mem::default_new_alignment);
        return ret;
    }

    void __small_deallocate(const stack_context &context) noexcept {
        const auto curr = m_idx;
        m_idx = context.m_idx;
        m_cache.ptr = context.m_ptr;
        // Fast path.
        if (WJR_LIKELY(m_idx == curr)) {
            return;
        }

        m_cache.end = m_ptr[m_idx].end;
        if (WJR_UNLIKELY(m_size - m_idx >= bufsize))
            __small_redeallocate();
    }

    void *__small_reallocate() noexcept;

    WJR_NOINLINE void __small_redeallocate() noexcept;

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

    WJR_NODISCARD WJR_MALLOC void *allocate(size_t n, stack_context &context) noexcept {
        if (WJR_BUILTIN_CONSTANT_P_TRUE(n >= stack_allocator_threshold)) {
            return __large_allocate(n, context.m_large);
        }

        if (WJR_UNLIKELY(static_cast<size_t>(m_cache.end - m_cache.ptr) < n)) {
            if (n >= stack_allocator_threshold) {
                return __large_allocate(n, context.m_large);
            }

            if (auto ptr = __small_reallocate(); ptr)
                context.m_ptr = static_cast<std::byte *>(ptr);
        }

        WJR_ASSERT_ASSUME_L2(m_cache.ptr != nullptr);

        auto *const ret = wjr::assume_aligned<mem::default_new_alignment>(m_cache.ptr);
        m_cache.ptr += align_up(n, mem::default_new_alignment);
        return ret;
    }

    void deallocate(const stack_context &context) noexcept {
        __small_deallocate(context);
        if (auto *buffer = context.m_large; buffer != nullptr)
            __large_deallocate(buffer);
    }

private:
    alignas(16) alloc_node m_cache = {nullptr, nullptr};
    uint_fast32_t m_idx = 0;
    uint_fast32_t m_size = 0;
    uint_fast32_t m_capacity = 0;
    alloc_node *m_ptr = nullptr;
};

inline stack_allocator_object::stack_context::stack_context(stack_allocator_object *object) noexcept
    : m_object(object), m_ptr(object->m_cache.ptr), m_idx(object->m_idx), m_large(nullptr) {
    WJR_ASSUME(m_object != nullptr);
}

inline stack_allocator_object::stack_context::~stack_context() noexcept {
    m_object->deallocate(*this);
}

static_assert(std::is_trivially_destructible_v<stack_allocator_object>);

namespace mem {
inline thread_local stack_allocator_object __stack_allocator_singleton_object;
}

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
    using Instance = stack_allocator_object;
    using stack_context = typename Instance::stack_context;

    template <typename T>
    friend class weak_stack_allocator;

public:
    WJR_INTRINSIC_INLINE
    unique_stack_allocator() noexcept
        : m_context(std::addressof(mem::__stack_allocator_singleton_object)) {}

    unique_stack_allocator(const unique_stack_allocator &) = delete;
    unique_stack_allocator(unique_stack_allocator &&) = delete;
    unique_stack_allocator &operator=(const unique_stack_allocator &) = delete;
    unique_stack_allocator &operator=(unique_stack_allocator &&) = delete;

    ~unique_stack_allocator() = default;

    WJR_NODISCARD WJR_MALLOC WJR_INTRINSIC_INLINE void *allocate(size_t n) noexcept {
        return m_context.object()->allocate(n, m_context);
    }

    template <typename Ty>
    WJR_NODISCARD WJR_MALLOC WJR_INTRINSIC_INLINE Ty *allocate(size_t n) noexcept {
        return static_cast<Ty *>(allocate(n * sizeof(Ty)));
    }

private:
    WJR_NODISCARD WJR_MALLOC WJR_INTRINSIC_INLINE void *__small_allocate(size_t n) noexcept {
        return m_context.object()->__small_allocate(n, m_context);
    }

    stack_context m_context;
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
    using UniqueStackAllocator = unique_stack_allocator;

    template <typename U>
    friend class weak_stack_allocator;

public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap = std::false_type;
    using is_always_equal = std::false_type;
    using is_trivially_allocator = std::true_type;

    template <typename Other>
    struct rebind {
        using other = weak_stack_allocator<Other>;
    };

    weak_stack_allocator() = default;
    weak_stack_allocator(const weak_stack_allocator &) = default;
    weak_stack_allocator(weak_stack_allocator &&) = default;
    weak_stack_allocator &operator=(const weak_stack_allocator &) = delete;
    weak_stack_allocator &operator=(weak_stack_allocator &&) = delete;
    ~weak_stack_allocator() = default;

    weak_stack_allocator(UniqueStackAllocator &alloc) noexcept : m_alloc(std::addressof(alloc)) {}

    template <typename U>
    weak_stack_allocator(const weak_stack_allocator<U> &other) noexcept : m_alloc(other.m_alloc) {}

    WJR_NODISCARD WJR_MALLOC T *allocate(size_type n) noexcept {
        const size_t size = n * sizeof(T);
        if (WJR_UNLIKELY(size >= stack_allocator_threshold)) {
            return static_cast<T *>(
                mem::__default_allocate<mem::__new_alignof<T>>(size, std::nothrow));
        }

        return static_cast<T *>(m_alloc->__small_allocate(size));
    }

    void deallocate(WJR_MAYBE_UNUSED T *ptr, WJR_MAYBE_UNUSED size_type n) noexcept {
        const size_t size = n * sizeof(T);
        if (WJR_UNLIKELY(size >= stack_allocator_threshold)) {
            mem::__default_deallocate<mem::__new_alignof<T>>(ptr, std::nothrow);
        }
    }

    friend bool operator==(const weak_stack_allocator &lhs,
                           const weak_stack_allocator &rhs) noexcept {
        return lhs.m_alloc == rhs.m_alloc;
    }

    friend bool operator!=(const weak_stack_allocator &lhs,
                           const weak_stack_allocator &rhs) noexcept {
        return !(lhs == rhs);
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
