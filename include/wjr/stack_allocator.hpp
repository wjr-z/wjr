#ifndef WJR_STACK_ALLOCATOR_HPP__
#define WJR_STACK_ALLOCATOR_HPP__

#include <cstddef>
#include <vector>

#include <wjr/compressed_pair.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

template <size_t cache0, size_t threshold0, size_t buffer0, size_t cache1,
          size_t threshold1, size_t buffer1, size_t alignment = alignof(std::max_align_t)>
class stack_alloc {

    template <size_t cache, size_t buffer>
    class __stack_alloc {

        class __stack_alloc_node {
        public:
            __stack_alloc_node() = default;
            constexpr __stack_alloc_node(size_t size) : m_end(m_bytes + size) {}

            __stack_alloc_node(const __stack_alloc_node &) = delete;
            __stack_alloc_node &operator=(const __stack_alloc_node &) = delete;

            ~__stack_alloc_node() = default;

            WJR_NODISCARD constexpr void *allocate(size_t n) {
                WJR_ASSERT(m_ptr != nullptr);
                WJR_ASSERT(rest() >= n);
                char *ret = m_ptr;
                m_ptr += n;
                return static_cast<void *>(ret);
            }

            WJR_INTRINSIC_CONSTEXPR void deallocate(void *old,
                                                    WJR_MAYBE_UNUSED size_t n) {
                WJR_ASSERT(m_ptr == static_cast<char *>(old) + n);
                m_ptr = static_cast<char *>(old);
                (void)(n);
            }

            WJR_INTRINSIC_CONSTEXPR size_t size() const { return m_ptr - m_bytes; }
            WJR_INTRINSIC_CONSTEXPR size_t rest() const { return m_end - m_ptr; }
            WJR_INTRINSIC_CONSTEXPR size_t capacity() const { return m_end - m_bytes; }

        private:
            char *m_ptr = m_bytes;
            char *m_end;
            alignas(alignment) char m_bytes[];
        };

        WJR_COLD WJR_CONSTEXPR20 void __malloc_node() {
            WJR_ASSERT(idx == stk.size());
            size_t size = 0;

            if (WJR_UNLIKELY(idx == 0)) {
                size = cache;
            } else {
                size = stk.back()->capacity();
                size += size / 3;
            }

            auto node = (__stack_alloc_node *)malloc(sizeof(__stack_alloc_node) + size);
            new (node) __stack_alloc_node(size);
            stk.emplace_back(node);
        }

    public:
        __stack_alloc() = default;
        __stack_alloc(const __stack_alloc &) = delete;
        __stack_alloc(__stack_alloc &&) = default;
        __stack_alloc &operator=(const __stack_alloc &) = delete;
        __stack_alloc &operator=(__stack_alloc &&) = default;
        ~__stack_alloc() = default;

        WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 void *allocate(size_t n) {
            if (WJR_UNLIKELY(idx == -1ull || stk[idx]->rest() < n)) {
                ++idx;
                if (WJR_UNLIKELY(idx == stk.size())) {
                    __malloc_node();
                }
            }

            return stk[idx]->allocate(n);
        }

        WJR_INTRINSIC_CONSTEXPR20 void deallocate(void *ptr, size_t n) {
            stk[idx]->deallocate(ptr, n);

            if (WJR_UNLIKELY(idx && stk[idx]->size() == 0)) {
                --idx;
                if (WJR_UNLIKELY(idx + buffer + 1 <= stk.size())) {
                    delete stk.back();
                    stk.pop_back();
                }
            }
        }

    private:
        size_t idx = -1ull;
        std::vector<__stack_alloc_node *> stk;
    };

public:
    stack_alloc() = default;
    stack_alloc(const stack_alloc &) = delete;
    stack_alloc(stack_alloc &&) = default;
    stack_alloc &operator=(const stack_alloc &) = delete;
    stack_alloc &operator=(stack_alloc &&) = default;
    ~stack_alloc() = default;

    WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 void *allocate(size_t n) {
        if (WJR_LIKELY(n < threshold0)) {
            return small_alloc.allocate(n);
        }

        if (WJR_LIKELY(n < threshold1)) {
            return mid_alloc.allocate(n);
        }

        return malloc(n);
    }

    WJR_INTRINSIC_CONSTEXPR20 void deallocate(void *ptr, size_t n) {
        if (WJR_LIKELY(n < threshold0)) {
            return small_alloc.deallocate(ptr, n);
        }

        if (WJR_LIKELY(n < threshold1)) {
            return mid_alloc.deallocate(ptr, n);
        }

        free(ptr);
    }

private:
    __stack_alloc<cache0, buffer0> small_alloc;
    __stack_alloc<cache1, buffer1> mid_alloc;
};

template <size_t cache0, size_t threshold0, size_t buffer0, size_t cache1,
          size_t threshold1, size_t buffer1, size_t alignment = alignof(std::max_align_t)>
class stack_allocator {
    using alloc =
        stack_alloc<cache0, threshold0, buffer0, cache1, threshold1, buffer1, alignment>;
    static thread_local alloc __alloc;

public:
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
public:
    WJR_INTRINSIC_CONSTEXPR20 unique_stack_ptr(const StackAllocator &al, size_t size)
        : pair(al, Malloc{al.allocate(size), size}) {}

    WJR_INTRINSIC_CONSTEXPR20 ~unique_stack_ptr() {
        auto &al = pair.first();
        auto &mlo = pair.second();
        pair.first().deallocate(mlo.ptr, mlo.size);
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