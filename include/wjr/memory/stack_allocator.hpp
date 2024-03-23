#ifndef WJR_STACK_ALLOCATOR_HPP__
#define WJR_STACK_ALLOCATOR_HPP__

#include <cstddef>

#include <wjr/compressed_pair.hpp>
#include <wjr/crtp/noncopyable.hpp>
#include <wjr/crtp/nonsendable.hpp>
#include <wjr/crtp/trivial_allocator_base.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

template <size_t threshold, size_t cache>
class stack_allocator_object {
    static_assert(threshold <= cache, "threshold must be less than or equal to cache.");

    constexpr static size_t bufsize = 5;

    class __stack_allocator_object : noncopyable {
    public:
        struct stack_top {
            char *ptr = nullptr;
            uint32_t idx = 0;
        };

    private:
        WJR_NOINLINE WJR_CONSTEXPR20 void __allocate(stack_top &top) {
            ++m_idx;
            if (WJR_UNLIKELY(m_idx == m_capacity)) {
                uint32_t new_capacity = m_idx + (m_idx <= 2 ? 1 : m_idx / 2);
                auto new_ptr =
                    static_cast<alloc_node *>(malloc(new_capacity * sizeof(alloc_node)));
                if (WJR_LIKELY(m_idx != 0)) {
                    std::copy_n(m_ptr, m_idx, new_ptr);
                    free(m_ptr);
                }
                m_ptr = new_ptr;
                m_capacity = new_capacity;

                size_t capacity = 0;

                if (WJR_UNLIKELY(m_idx == 0)) {
                    capacity = cache;
                } else {
                    auto &node = m_ptr[m_idx - 1];
                    capacity = node.end - node.ptr;
                    capacity += capacity / 2;
                }

                auto buffer = static_cast<char *>(malloc(capacity));
                m_ptr[m_idx] = {buffer, buffer + capacity};

                if (WJR_UNLIKELY(m_idx == 0)) {
                    top.ptr = buffer;
                    top.idx = 0;
                }
            } else {
                WJR_ASSERT(top.ptr != nullptr);
            }

            m_cache = m_ptr[m_idx];
        }

        WJR_CONSTEXPR20 void __deallocate() {
            uint32_t new_capacity = m_idx + bufsize - 1;

            for (uint32_t i = new_capacity; i < m_capacity; ++i) {
                free(m_ptr[i].ptr);
            }

            auto new_ptr =
                static_cast<alloc_node *>(malloc(new_capacity * sizeof(alloc_node)));
            std::copy_n(m_ptr, new_capacity, new_ptr);
            free(m_ptr);
            m_ptr = new_ptr;
            m_capacity = new_capacity;
        }

    public:
        __stack_allocator_object() = default;
        __stack_allocator_object(__stack_allocator_object &&) = default;
        __stack_allocator_object &operator=(__stack_allocator_object &&) = default;
        ~__stack_allocator_object() {
            for (uint32_t i = 0; i < m_capacity; ++i) {
                free(m_ptr[i].ptr);
            }

            free(m_ptr);
        }

        WJR_CONSTEXPR20 void *allocate(size_t n, stack_top &top) {
            if (WJR_UNLIKELY(top.ptr == nullptr)) {
                top.ptr = m_cache.ptr;
                top.idx = m_idx;
            }

            if (WJR_UNLIKELY(static_cast<size_t>(m_cache.end - m_cache.ptr) < n)) {
                __allocate(top);
            }

            WJR_ASSERT_ASSUME(m_cache.ptr != nullptr);
            WJR_ASSERT_ASSUME(top.ptr != nullptr);

            auto ptr = m_cache.ptr;
            m_cache.ptr += n;
            return ptr;
        }

        WJR_CONSTEXPR20 void deallocate(const stack_top &top) {
            if (WJR_UNLIKELY(top.ptr == nullptr)) {
                return;
            } else {
                uint32_t idx = top.idx;
                if (WJR_LIKELY(idx == m_idx)) {
                    m_cache.ptr = top.ptr;
                } else {
                    m_idx = idx;
                    m_cache = {top.ptr, m_ptr[idx].end};
                    if (WJR_UNLIKELY(m_capacity - idx >= bufsize)) {
                        __deallocate();
                    }
                }
            }
        }

    private:
        struct alloc_node {
            char *ptr;
            char *end;
        };

        alloc_node m_cache = {nullptr, nullptr};
        uint32_t m_idx = -1u;
        alloc_node *m_ptr = nullptr;
        uint32_t m_capacity = 0;
    };

    struct large_stack_top {
        large_stack_top *prev;
        char buffer[];
    };

public:
    struct stack_top {
        typename __stack_allocator_object::stack_top small;
        large_stack_top *large = nullptr;
    };

private:
    WJR_CONSTEXPR20 void *__large_allocate(size_t n, stack_top &top) {
        auto buffer = (large_stack_top *)malloc(sizeof(large_stack_top) + n);
        buffer->prev = top.large;
        top.large = buffer;
        return buffer->buffer;
    }

public:
    using value_type = void;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;

    stack_allocator_object() = default;
    stack_allocator_object(stack_allocator_object &&) = default;
    stack_allocator_object &operator=(stack_allocator_object &&) = default;
    ~stack_allocator_object() = default;

    WJR_NODISCARD WJR_MALLOC WJR_CONSTEXPR20 void *allocate(size_t n, stack_top &top) {
        if (WJR_UNLIKELY(n >= threshold)) {
            return __large_allocate(n, top);
        }

        return alloc.allocate(n, top.small);
    }

    WJR_CONSTEXPR20 void deallocate(const stack_top &top) {
        alloc.deallocate(top.small);
        auto buffer = top.large;
        while (WJR_UNLIKELY(buffer != nullptr)) {
            auto prev = buffer->prev;
            free(buffer);
            buffer = prev;
        }
    }

private:
    __stack_allocator_object alloc = __stack_allocator_object();
};

template <typename StackAllocator>
class singleton_stack_allocator_adapter {
    static StackAllocator &get_instance() {
        static thread_local StackAllocator instance;
        return instance;
    }

public:
    using value_type = typename StackAllocator::value_type;
    using size_type = typename StackAllocator::size_type;
    using difference_type = typename StackAllocator::difference_type;
    using propagate_on_container_move_assignment =
        typename StackAllocator::propagate_on_container_move_assignment;
    using stack_top = typename StackAllocator::stack_top;

    WJR_CONSTEXPR20 value_type *allocate(size_type n, stack_top &top) const {
        return get_instance().allocate(n, top);
    }

    WJR_CONSTEXPR20 void deallocate(stack_top &top) const {
        get_instance().deallocate(top);
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

template <typename StackAllocator>
class unique_stack_allocator;

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
    : nonsendable {
    using StackAllocator = singleton_stack_allocator_object<threshold, cache>;
    using stack_top = typename StackAllocator::stack_top;

public:
    unique_stack_allocator(const StackAllocator &al) : m_pair(al, {}) {}
    ~unique_stack_allocator() { m_pair.first().deallocate(m_pair.second()); }

    WJR_CONSTEXPR20 void *allocate(size_t n) {
        nonsendable::check();
        return m_pair.first().allocate(n, m_pair.second());
    }

private:
    compressed_pair<StackAllocator, stack_top> m_pair;
};

template <typename StackAllocator>
unique_stack_allocator(const StackAllocator &) -> unique_stack_allocator<StackAllocator>;

template <typename T, typename StackAllocator>
class weak_stack_allocator;

template <typename T, size_t threshold, size_t cache>
class weak_stack_allocator<T, singleton_stack_allocator_object<threshold, cache>>
    : private trivial_allocator_base {
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

    WJR_CONSTEXPR20 T *allocate(size_type n) {
        return static_cast<T *>(m_alloc->allocate(n * sizeof(T)));
    }

    WJR_CONSTEXPR20 void deallocate(WJR_MAYBE_UNUSED T *ptr,
                                    WJR_MAYBE_UNUSED size_type n) {}

private:
    UniqueStackAllocator *m_alloc = nullptr;
};

} // namespace wjr

#endif // WJR_STACK_ALLOCATOR_HPP__