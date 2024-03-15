#ifndef WJR_STACK_ALLOCATOR_HPP__
#define WJR_STACK_ALLOCATOR_HPP__

#include <cstddef>
#include <vector>

#include <wjr/compressed_pair.hpp>
#include <wjr/crtp.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

template <size_t cache, size_t threshold>
class stack_alloc {

    constexpr static size_t bufsize = 5;

    class __stack_alloc {
    public:
        struct stack_top {
            char *ptr = nullptr;
            size_t idx = 0;
        };

    private:
        WJR_NOINLINE WJR_CONSTEXPR20 void __allocate(stack_top &top) {
            ++m_idx;
            if (WJR_UNLIKELY(m_idx == m_stk.size())) {
                size_t capacity = 0;

                if (WJR_UNLIKELY(m_idx == 0)) {
                    capacity = cache;
                } else {
                    auto &node = m_stk.back();
                    capacity = node.end - node.ptr;
                    capacity += capacity / 2;
                }

                auto buffer = static_cast<char *>(malloc(capacity));
                auto node = alloc_node{buffer, buffer + capacity};
                m_stk.emplace_back(std::move(node));

                if (WJR_UNLIKELY(m_idx == 0)) {
                    top.ptr = m_stk[0].ptr;
                    top.idx = 0;
                }
            } else {
                WJR_ASSERT(top.ptr != nullptr);
            }

            m_node = m_stk[m_idx];
        }

        WJR_CONSTEXPR20 void __deallocate() {
            size_t n = m_stk.size() - m_idx - bufsize + 1;
            for (size_t i = 0; i < n; ++i) {
                free(m_stk.back().ptr);
                m_stk.pop_back();
            }
        }

    public:
        __stack_alloc() = default;
        __stack_alloc(const __stack_alloc &) = delete;
        __stack_alloc(__stack_alloc &&) = default;
        __stack_alloc &operator=(const __stack_alloc &) = delete;
        __stack_alloc &operator=(__stack_alloc &&) = default;
        ~__stack_alloc() {
            for (const auto &node : m_stk) {
                free(node.ptr);
            }
        }

        WJR_CONSTEXPR20 void *allocate(size_t n, stack_top &top) {
            if (WJR_UNLIKELY(top.ptr == nullptr)) {
                top.ptr = m_node.ptr;
                top.idx = m_idx;
            }

            if (WJR_UNLIKELY(static_cast<size_t>(m_node.end - m_node.ptr) < n)) {
                __allocate(top);
                WJR_ASSUME(top.idx != m_idx);
            }

            WJR_ASSERT_ASSUME(m_node.ptr != nullptr);
            WJR_ASSERT_ASSUME(top.ptr != nullptr);

            auto ptr = m_node.ptr;
            m_node.ptr += n;
            return ptr;
        }

        WJR_CONSTEXPR20 void deallocate(const stack_top &top) {
            size_t idx = top.idx;
            if (WJR_UNLIKELY(top.ptr == nullptr)) {
                return;
            } else {
                if (WJR_LIKELY(idx == m_idx)) {
                    m_node.ptr = top.ptr;
                } else {
                    m_idx = idx;
                    m_node = {top.ptr, m_stk[m_idx].end};
                    if (WJR_UNLIKELY(m_stk.size() - m_idx >= bufsize)) {
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

        // cache of m_stk
        alloc_node m_node = {nullptr, nullptr};
        size_t m_idx = -1ull;
        std::vector<alloc_node> m_stk;
    };

    struct large_stack_top {
        large_stack_top *prev;
        char buffer[];
    };

public:
    struct stack_top {
        typename __stack_alloc::stack_top small;
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

    stack_alloc() = default;
    stack_alloc(const stack_alloc &) = delete;
    stack_alloc(stack_alloc &&) = default;
    stack_alloc &operator=(const stack_alloc &) = delete;
    stack_alloc &operator=(stack_alloc &&) = default;
    ~stack_alloc() = default;

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
    __stack_alloc alloc = __stack_alloc();
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
 * @note When allocating memory less than threadshold, use pre-allocated heap memory,
 * otherwise use malloc to allocate heap memory.
 *
 * @tparam cache The size of the first heap memory allocation
 * @tparam threshold The threshold for using malloc to allocate heap memory
 */
template <size_t cache, size_t threshold>
using stack_allocator = singleton_stack_allocator_adapter<stack_alloc<cache, threshold>>;

template <typename StackAllocator>
class unique_stack_allocator;

template <size_t cache, size_t threshold>
class unique_stack_allocator<stack_allocator<cache, threshold>> : private nonsendable {
    using StackAllocator = stack_allocator<cache, threshold>;
    using stack_top = typename StackAllocator::stack_top;

    using Mybase = nonsendable;

public:
    unique_stack_allocator(const StackAllocator &al) : pair(al, {}) {}
    ~unique_stack_allocator() {
        Mybase::check();
        pair.first().deallocate(pair.second());
    }

    WJR_CONSTEXPR20 void *allocate(size_t n) {
        Mybase::check();
        return pair.first().allocate(n, pair.second());
    }

private:
    compressed_pair<StackAllocator, stack_top> pair;
};

template <typename StackAllocator>
unique_stack_allocator(const StackAllocator &) -> unique_stack_allocator<StackAllocator>;

} // namespace wjr

#endif // WJR_STACK_ALLOCATOR_HPP__