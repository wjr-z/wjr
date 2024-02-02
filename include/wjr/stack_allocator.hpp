#ifndef WJR_STACK_ALLOCATOR_HPP__
#define WJR_STACK_ALLOCATOR_HPP__

#include <cstddef>
#include <vector>

#include <wjr/compressed_pair.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

class basic_stack_alloc {
public:
    basic_stack_alloc() = delete;
    basic_stack_alloc(void *buffer, void *end)
        : m_ptr(static_cast<char *>(buffer)), m_end(static_cast<char *>(end)) {
        WJR_ASSERT(m_end >= m_ptr);
    }
    basic_stack_alloc(const basic_stack_alloc &) = delete;
    basic_stack_alloc &operator=(const basic_stack_alloc &) = delete;
    ~basic_stack_alloc() = default;

    WJR_NODISCARD constexpr void *allocate(size_t n) {
        WJR_ASSERT(m_ptr != nullptr);
        WJR_ASSERT(static_cast<size_t>(m_end - m_ptr) >= n);
        char *ret = m_ptr;
        m_ptr += n;
        return static_cast<void *>(ret);
    }

    WJR_INTRINSIC_CONSTEXPR void deallocate(void *old, WJR_MAYBE_UNUSED size_t n) {
        WJR_ASSERT(m_ptr == static_cast<char *>(old) + n);
        m_ptr = static_cast<char *>(old);
        (void)(n);
    }

    WJR_INTRINSIC_CONSTEXPR char *ptr() const { return m_ptr; }
    WJR_INTRINSIC_CONSTEXPR char *end() const { return m_end; }

private:
    char *m_ptr;
    char *m_end;
};

template <size_t cache0, size_t threshold0, size_t bufsize0, size_t cache1,
          size_t threshold1, size_t bufsize1,
          size_t alignment = alignof(std::max_align_t)>
class stack_alloc {

    template <size_t cache, size_t bufsize>
    class __stack_alloc {

        WJR_COLD WJR_CONSTEXPR20 void __malloc_node() {
            size_t capacity = 0;

            if (WJR_UNLIKELY(m_idx == 0)) {
                capacity = cache;
            } else {
                auto &node = m_stk.back();
                capacity = node.end - node.buffer;
                capacity += capacity / 3;
            }

            auto buffer = static_cast<char *>(malloc(capacity));
            alloc_node node = {buffer, buffer, buffer + capacity};

            m_stk.emplace_back(node);
        }

    public:
        __stack_alloc() = default;
        __stack_alloc(const __stack_alloc &) = delete;
        __stack_alloc(__stack_alloc &&) = default;
        __stack_alloc &operator=(const __stack_alloc &) = delete;
        __stack_alloc &operator=(__stack_alloc &&) = default;
        ~__stack_alloc() = default;

        WJR_NODISCARD WJR_CONSTEXPR20 void *allocate(size_t n) {
            if (WJR_UNLIKELY(m_end - m_ptr < n)) {

                if (WJR_LIKELY(m_idx != -1ull)) {
                    m_stk[m_idx].ptr = m_ptr;
                }

                ++m_idx;
                if (WJR_UNLIKELY(m_idx == m_stk.size())) {
                    __malloc_node();
                }

                const auto &node = m_stk[m_idx];
                m_buffer = node.buffer;
                m_ptr = m_buffer;
                m_end = node.end;
            }

            auto ret = static_cast<void *>(m_ptr);
            m_ptr += n;
            return ret;
        }

        WJR_CONSTEXPR20 void deallocate(void *ptr, size_t n) {
            m_ptr = static_cast<char *>(ptr);

            if (WJR_UNLIKELY(m_ptr == m_buffer)) {
                --m_idx;

                if (WJR_UNLIKELY(m_stk.size() - m_idx >= bufsize + 2)) {
                    free(m_stk.back().buffer);
                    m_stk.pop_back();
                }

                if (WJR_UNLIKELY(m_idx == -1ull)) {
                    m_buffer = m_ptr = m_end = nullptr;
                } else {

                    const auto &node = m_stk[m_idx];
                    m_buffer = node.buffer;
                    m_ptr = node.ptr;
                    m_end = node.end;
                }
            }
        }

    private:
        struct alloc_node {
            char *buffer;
            char *ptr;
            char *end;
        };

        char *m_buffer = nullptr;
        char *m_ptr = nullptr;
        char *m_end = nullptr;

        size_t m_idx = -1ull;
        std::vector<alloc_node> m_stk;
    };

public:
    stack_alloc() = default;
    stack_alloc(const stack_alloc &) = delete;
    stack_alloc(stack_alloc &&) = default;
    stack_alloc &operator=(const stack_alloc &) = delete;
    stack_alloc &operator=(stack_alloc &&) = default;
    ~stack_alloc() = default;

    WJR_NODISCARD WJR_CONSTEXPR20 void *allocate(size_t n) {
        if (WJR_UNLIKELY(n >= threshold0)) {
            if (WJR_UNLIKELY(n >= threshold1)) {
                return malloc(n);
            }
            return mid_alloc.allocate(n);
        }

        return small_alloc.allocate(n);
    }

    WJR_CONSTEXPR20 void deallocate(void *ptr, WJR_MAYBE_UNUSED size_t n) {
        if (WJR_UNLIKELY(n >= threshold0)) {
            if (WJR_UNLIKELY(n >= threshold1)) {
                free(ptr);
                return;
            }
            return mid_alloc.deallocate(ptr, n);
        }

        return small_alloc.deallocate(ptr, n);
    }

private:
    __stack_alloc<cache0, bufsize0> small_alloc;
    __stack_alloc<cache1, bufsize1> mid_alloc;
};

template <size_t cache0, size_t threshold0, size_t buffer0, size_t cache1,
          size_t threshold1, size_t buffer1, size_t alignment = alignof(std::max_align_t)>
class stack_allocator {
    using alloc =
        stack_alloc<cache0, threshold0, buffer0, cache1, threshold1, buffer1, alignment>;
    static thread_local alloc __alloc;

public:
    using pointer = void *;

    stack_allocator() = default;
    stack_allocator(const stack_allocator &) = default;
    stack_allocator &operator=(const stack_allocator &) = default;
    ~stack_allocator() = default;

    WJR_NODISCARD WJR_CONSTEXPR20 void *allocate(size_t n) const {
        return __alloc.allocate(n);
    }

    WJR_CONSTEXPR20 void deallocate(void *ptr, size_t n) const {
        return __alloc.deallocate(ptr, n);
    }
};

template <size_t cache0, size_t threshold0, size_t buffer0, size_t cache1,
          size_t threshold1, size_t buffer1, size_t alignment>
thread_local typename stack_allocator<cache0, threshold0, buffer0, cache1, threshold1,
                                      buffer1, alignment>::alloc
    stack_allocator<cache0, threshold0, buffer0, cache1, threshold1, buffer1,
                    alignment>::__alloc = {};

// preview :
// Disable move constructor
template <typename StackAllocator>
class unique_stack_ptr {
    using pointer = typename StackAllocator::pointer;

public:
    WJR_CONSTEXPR20 unique_stack_ptr(const StackAllocator &al, size_t size)
        : pair(al, Malloc{nullptr, size}) {
        pair.second().ptr = static_cast<void *>(al.allocate(size));
    }

    WJR_CONSTEXPR20 ~unique_stack_ptr() {
        auto &al = pair.first();
        auto &mlo = pair.second();
        al.deallocate(static_cast<pointer>(mlo.ptr), mlo.size);
    }

    unique_stack_ptr(const unique_stack_ptr &) = delete;
    unique_stack_ptr &operator=(const unique_stack_ptr &) = delete;

    WJR_INTRINSIC_CONSTEXPR20 void *get() const { return pair.second().ptr; }

private:
    struct Malloc {
        void *ptr;
        size_t size;
    };

    compressed_pair<StackAllocator, Malloc> pair;
};

template <typename StackAllocator>
unique_stack_ptr(StackAllocator &, size_t) -> unique_stack_ptr<StackAllocator>;

} // namespace wjr

#endif // WJR_STACK_ALLOCATOR_HPP__