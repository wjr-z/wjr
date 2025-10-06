#ifndef WJR_MEMORY_STACK_ALLOCATOR_HPP__
#define WJR_MEMORY_STACK_ALLOCATOR_HPP__

#include <algorithm>

#include <wjr/math/literals.hpp>
#include <wjr/memory/aligned_alloc.hpp>
#include <wjr/string.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

/// @todo Make it configurable.
inline constexpr size_t stack_allocator_threshold = 16_KB;

class unique_stack_allocator;

class stack_allocator_object {
    friend class unique_stack_allocator;

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
        large_memory *m_prev;
    };

private:
    WJR_MALLOC static void *__large_allocate(size_t n, large_memory *&mem) noexcept {
        auto *const raw = mem::__default_allocate(n + aligned_header_size, std::nothrow);
        auto *const buffer = static_cast<large_memory *>(raw);
        buffer->prev = mem;
        mem = buffer;
        return wjr::assume_aligned<mem::default_new_alignment>(
            static_cast<void *>(static_cast<std::byte *>(raw) + aligned_header_size));
    }

    static void __large_deallocate(large_memory *buffer) noexcept;

    WJR_MALLOC void *__small_allocate(size_t n) noexcept {
        if (WJR_UNLIKELY(static_cast<size_t>(m_cache.end - m_cache.ptr) < n)) {
            __small_reallocate();
        }

        WJR_ASSUME(m_cache.ptr != nullptr);

        auto *const ret = wjr::assume_aligned<mem::default_new_alignment>(m_cache.ptr);
        m_cache.ptr += align_up(n, mem::default_new_alignment);
        return ret;
    }

    void __small_deallocate(const stack_context &context) noexcept {
        if (WJR_UNLIKELY(context.m_ptr == nullptr))
            m_cache.ptr = m_first;
        else
            m_cache.ptr = context.m_ptr;

        const auto prev = context.m_idx;
        // Fast path.
        if (WJR_LIKELY(m_idx == prev)) {
            return;
        }

        m_idx = prev;
        m_cache.end = m_ptr[m_idx].end;
        if (WJR_UNLIKELY(m_size - m_idx >= bufsize))
            __small_redeallocate();
    }

    void __small_reallocate() noexcept;
    void __small_redeallocate() noexcept;

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
            return __large_allocate(n, context.m_prev);
        }

        if (WJR_UNLIKELY(static_cast<size_t>(m_cache.end - m_cache.ptr) < n)) {
            if (n >= stack_allocator_threshold) {
                return __large_allocate(n, context.m_prev);
            }

            __small_reallocate();
        }

        WJR_ASSUME(m_cache.ptr != nullptr);

        auto *const ret = wjr::assume_aligned<mem::default_new_alignment>(m_cache.ptr);
        m_cache.ptr += align_up(n, mem::default_new_alignment);
        return ret;
    }

    void deallocate(const stack_context &context) noexcept {
        __small_deallocate(context);
        if (auto *buffer = context.m_prev; buffer != nullptr)
            __large_deallocate(buffer);
    }

private:
    alignas(16) alloc_node m_cache = {nullptr, nullptr};
    uint_fast32_t m_idx = 0;
    uint_fast32_t m_size = 0;
    uint_fast32_t m_capacity = 0;
    alloc_node *m_ptr = nullptr;

    /// @brief The first allocated buffer, used to reset m_cache.ptr in __small_deallocate.
    std::byte *m_first = nullptr;
};

inline stack_allocator_object::stack_context::stack_context(stack_allocator_object *object) noexcept
    : m_object(object), m_ptr(object->m_cache.ptr), m_idx(object->m_idx), m_prev(nullptr) {
    WJR_ASSUME(m_object != nullptr);
}

inline stack_allocator_object::stack_context::~stack_context() noexcept {
    m_object->deallocate(*this);
}

static_assert(std::is_trivially_destructible_v<stack_allocator_object>);

namespace mem {
/**
 * @note Shared library may use different stack_allocator_object instance. If applied for and
 * released from the same library, there is generally no problem.
 *
 * @todo Fix!!! This is an important issue.
 *
 */
inline thread_local stack_allocator_object __stack_allocator_singleton_object;
} // namespace mem

/**
 * @details Used for container. This allocator won't deallocate memory allocated
 * by __small_allocate until container is destroyed.
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
        return m_context.object()->__small_allocate(n);
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
 * @note It is almost only applicable to exponential growth allocation and does not frequently
 * involve shrink_to_fit in between. If these conditions are not met, it may lead to a large number
 * of memory leaks. If you are using it in this uncertain situation, you may need to customize a new
 * memory allocator that limits the maximum amount of allocated memory. When the allocated amount
 * exceeds this limit, it will revert to using regular malloc/free for allocation and deallocation.
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

    weak_stack_allocator() = delete;
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

    void deallocate(T *ptr, size_type n) noexcept {
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

    UniqueStackAllocator *alloc() const noexcept { return m_alloc; }

private:
    UniqueStackAllocator *m_alloc = nullptr;
};

template <typename T>
struct is_weak_stack_allocator : std::false_type {};

template <typename T>
struct is_weak_stack_allocator<weak_stack_allocator<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_weak_stack_allocator_v = is_weak_stack_allocator<T>::value;

namespace string_detail {
using weak_stack_allocator_string =
    std::basic_string<char, std::char_traits<char>, weak_stack_allocator<char>>;
}

WJR_REGISTER_STRING_UNINITIALIZED_RESIZE(weak_stack_allocator_string,
                                         string_detail::weak_stack_allocator_string);

class unique_stack_allocator_wrapper {
public:
    template <typename... Args>
    unique_stack_allocator_wrapper(Args *...) noexcept {}

    unique_stack_allocator *operator->() noexcept { return &stkal; }
    unique_stack_allocator &operator*() noexcept { return stkal; }

private:
    unique_stack_allocator stkal;
};

/**
 * @brief Reuse the first weak_stack_biginteger's allocator in Args as the allocator for
 * unique_stack_allocator_ref.
 *
 */
class unique_stack_allocator_ref {
    struct tag {};

    template <typename T, typename... Args,
              WJR_REQUIRES(!is_weak_stack_allocator_v<typename T::allocator_type>)>
    unique_stack_allocator_ref(tag, T *, Args *...args)
        : unique_stack_allocator_ref(tag{}, args...) {}

    template <typename T, typename... Args,
              WJR_REQUIRES(is_weak_stack_allocator_v<typename T::allocator_type>)>
    unique_stack_allocator_ref(tag, T *arg, Args *...) noexcept
        : stkal(*arg->get_allocator().alloc()) {}

public:
    // find first weak_stack_biginteger in arguments and bind its allocator
    template <typename... Args>
    unique_stack_allocator_ref(Args *...args) noexcept
        : unique_stack_allocator_ref(tag{}, args...) {}

    unique_stack_allocator *operator->() noexcept { return &stkal; }
    unique_stack_allocator &operator*() noexcept { return stkal; }

private:
    unique_stack_allocator &stkal;
};

template <typename... Args>
using unique_stack_allocator_selector = std::conditional_t<
    std::disjunction_v<is_weak_stack_allocator<typename Args::allocator_type>...>,
    unique_stack_allocator_ref, unique_stack_allocator_wrapper>;

} // namespace wjr

#endif // WJR_MEMORY_STACK_ALLOCATOR_HPP__