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
    constexpr static uint_fast32_t bufsize = 3;

    struct alloc_node {
        std::byte *ptr;
        std::byte *end;
    };

    struct large_memory {
        large_memory *prev;
    };

public:
    class stack_restorer {
        friend class stack_allocator_object;

    public:
        WJR_INTRINSIC_INLINE stack_restorer(stack_allocator_object *object) noexcept;
        inline ~stack_restorer() noexcept;

        stack_restorer(const stack_restorer &) = delete;
        stack_restorer(stack_restorer &&) = delete;

        WJR_PURE stack_allocator_object *object() noexcept { return m_object; }

    private:
        stack_allocator_object *m_object;
        std::byte * m_ptr;
        uint_fast32_t m_idx;
        large_memory *m_large;
    };

private:
    void *__large_allocate(size_t n, large_memory *&restorer_large) noexcept {
        auto *const buffer = static_cast<large_memory *>(malloc(sizeof(large_memory) + n));
        buffer->prev = restorer_large;
        restorer_large = buffer;
        return buffer + 1;
    }

    void __large_deallocate(large_memory *buffer) noexcept;

    WJR_MALLOC void *__small_allocate(size_t n, stack_restorer &restorer) noexcept {
        if (WJR_UNLIKELY(static_cast<size_t>(m_cache.end - m_cache.ptr) < n)) {
            __small_reallocate(restorer.m_ptr);
        }

        WJR_ASSERT_ASSUME_L2(m_cache.ptr != nullptr);
        auto *const ret = m_cache.ptr;
        m_cache.ptr += n;
        return ret;
    }

    void __small_deallocate(const stack_restorer &restorer) noexcept {
        const auto curr = m_idx;
        m_idx = restorer.m_idx;
        m_cache.ptr = restorer.m_ptr;
        // Fast path.
        if (WJR_LIKELY(m_idx == curr)) {
            return;
        }

        m_cache.end = m_ptr[m_idx].end;
        if (WJR_UNLIKELY(m_size - m_idx >= bufsize)) {
            __small_redeallocate();
        }
    }

    void __small_reallocate(std::byte *&restore_ptr) noexcept;

    WJR_NOINLINE void __small_redeallocate() noexcept {
        const uint_fast32_t new_size = m_idx + bufsize - 1;
        __default_alloc_template__ pool;

        for (uint_fast32_t i = new_size; i < m_size; ++i) {
            pool.chunk_deallocate(m_ptr[i].ptr, m_ptr[i].end - m_ptr[i].ptr);
        }

        m_size = new_size;
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

    WJR_NODISCARD WJR_MALLOC void *allocate(size_t n, stack_restorer &restorer) noexcept {
        if (WJR_BUILTIN_CONSTANT_P_TRUE(n >= stack_allocator_threshold)) {
            return __large_allocate(n, restorer.m_large);
        }

        if (WJR_UNLIKELY(static_cast<size_t>(m_cache.end - m_cache.ptr) < n)) {
            if (n >= stack_allocator_threshold) {
                return __large_allocate(n, restorer.m_large);
            }

            __small_reallocate(restorer.m_ptr);
        }

        WJR_ASSERT_ASSUME_L2(m_cache.ptr != nullptr);
        auto *const ret = m_cache.ptr;
        m_cache.ptr += n;
        return ret;
    }

    void deallocate(const stack_restorer &restorer) noexcept {
        __small_deallocate(restorer);
        if (auto *buffer = restorer.m_large; buffer != nullptr)
            __large_deallocate(buffer);
    }

private:
    alignas(16) alloc_node m_cache = {nullptr, nullptr};
    uint_fast32_t m_idx = 0;
    uint_fast32_t m_size = 0;
    uint_fast32_t m_capacity = 0;
    alloc_node *m_ptr = nullptr;
};

stack_allocator_object::stack_restorer::stack_restorer(stack_allocator_object *object) noexcept
    : m_object(object), m_ptr(object->m_cache.ptr), m_idx(object->m_idx), m_large(nullptr) {}
stack_allocator_object::stack_restorer::~stack_restorer() noexcept { m_object->deallocate(*this); }

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
    using stack_restorer = typename Instance::stack_restorer;

    template <typename T>
    friend class weak_stack_allocator;

public:
    WJR_INTRINSIC_INLINE
    unique_stack_allocator() noexcept : m_restorer(std::addressof(Object::get_instance())) {}

    unique_stack_allocator(const unique_stack_allocator &) = delete;
    unique_stack_allocator(unique_stack_allocator &&) = delete;
    unique_stack_allocator &operator=(const unique_stack_allocator &) = delete;
    unique_stack_allocator &operator=(unique_stack_allocator &&) = delete;

    ~unique_stack_allocator() = default;

    WJR_NODISCARD WJR_MALLOC WJR_INTRINSIC_INLINE void *allocate(size_t n) noexcept {
        return m_restorer.object()->allocate(n, m_restorer);
    }

    template <typename Ty>
    WJR_NODISCARD WJR_MALLOC WJR_INTRINSIC_INLINE Ty *allocate(size_t n) noexcept {
        return static_cast<Ty *>(allocate(n * sizeof(Ty)));
    }

private:
    WJR_NODISCARD WJR_MALLOC WJR_INTRINSIC_INLINE void *__small_allocate(size_t n) noexcept {
        return m_restorer.object()->__small_allocate(n, m_restorer);
    }

    stack_restorer m_restorer;
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