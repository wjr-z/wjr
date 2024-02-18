#ifndef WJR_STACK_ALLOCATOR_HPP__
#define WJR_STACK_ALLOCATOR_HPP__

#include <cstddef>
#include <vector>

#include <wjr/compressed_pair.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

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
            auto node = alloc_node{buffer, buffer, buffer + capacity};
            m_stk.emplace_back(std::move(node));
        }

        WJR_COLD WJR_CONSTEXPR20 void __allocate() {
            if (WJR_LIKELY(m_idx != -1ull)) {
                m_stk[m_idx].ptr = m_node.ptr;
            }

            ++m_idx;
            if (WJR_UNLIKELY(m_idx == m_stk.size())) {
                __malloc_node();
            }

            m_node = m_stk[m_idx];
        }

        WJR_COLD WJR_CONSTEXPR20 void __deallocate() {
            --m_idx;

            if (WJR_UNLIKELY(m_stk.size() - m_idx >= bufsize + 2)) {
                free(m_stk.back().buffer);
                m_stk.pop_back();
            }

            m_node = m_stk[m_idx];
        }

    public:
        __stack_alloc() = default;
        __stack_alloc(const __stack_alloc &) = delete;
        __stack_alloc(__stack_alloc &&) = default;
        __stack_alloc &operator=(const __stack_alloc &) = delete;
        __stack_alloc &operator=(__stack_alloc &&) = default;
        ~__stack_alloc() {
            for (const auto &node : m_stk) {
                free(node.buffer);
            }
        }

        WJR_NODISCARD WJR_CONSTEXPR20 void *allocate(size_t n) {
            if (WJR_UNLIKELY(static_cast<size_t>(m_node.end - m_node.ptr) < n)) {
                __allocate();
            }

            auto ret = static_cast<void *>(m_node.ptr);
            m_node.ptr += n;
            return ret;
        }

        WJR_CONSTEXPR20 void deallocate(void *ptr, WJR_MAYBE_UNUSED size_t n) {
            WJR_ASSERT(m_node.ptr == static_cast<char *>(ptr) + n);
            m_node.ptr = static_cast<char *>(ptr);

            if (WJR_UNLIKELY(m_node.ptr == m_node.buffer) && m_idx) {
                __deallocate();
            }
        }

    private:
        struct alloc_node {
            char *buffer;
            char *ptr;
            char *end;
        };

        alloc_node m_node = {nullptr, nullptr, nullptr};
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

    WJR_NODISCARD WJR_MALLOC WJR_CONSTEXPR20 void *allocate(size_t n) {
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

    WJR_NODISCARD WJR_MALLOC WJR_CONSTEXPR20 void *allocate(size_t n) const {
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

// Universal alternative solutions for alloca
template <typename StackAllocator, size_t Extent = dynamic_extent>
class unique_stack_allocator {
    using pointer = typename StackAllocator::pointer;
    static constexpr size_t InlineThreshold = 4;
    static constexpr bool IsInline = Extent <= InlineThreshold;

    template <size_t E = Extent, std::enable_if_t<!(E <= InlineThreshold), int> = 0>
    WJR_CONSTEXPR20 void *__allocate(size_t n) {
        auto &al = pair.first();
        auto &vec = pair.second();

        size_t cap = sizeof(NoinlineBuffer) + n;
        auto buffer = static_cast<NoinlineBuffer *>(al.allocate(cap));
        void *ret = buffer->buffer;
        buffer->prev = vec.vec;
        buffer->size = cap;
        vec.vec = buffer;
        return ret;
    }

    template <size_t E = Extent, std::enable_if_t<(E <= InlineThreshold), int> = 0>
    WJR_CONSTEXPR20 void *__allocate(size_t n) {
        auto &al = pair.first();
        auto &arr = pair.second();
        auto buffer = al.allocate(n);
        arr.arr[arr.idx++] = {buffer, n};
        return buffer;
    }

    template <size_t E = Extent, std::enable_if_t<!(E <= InlineThreshold), int> = 0>
    WJR_CONSTEXPR20 void __release_one() {
        auto &al = pair.first();
        auto &vec = pair.second();
        NoinlineBuffer *buffer = vec.vec;
        auto prev = buffer->prev;
        al.deallocate(buffer, buffer->size);
        vec.vec = prev;
    }

    template <size_t E = Extent, std::enable_if_t<(E <= InlineThreshold), int> = 0>
    WJR_CONSTEXPR20 void __release_one() {
        auto &al = pair.first();
        auto &arr = pair.second();
        InlineBuffer &buffer = arr.arr[--arr.idx];
        al.deallocate(buffer.buffer, buffer.size);
    }

    template <size_t E = Extent, std::enable_if_t<!(E <= InlineThreshold), int> = 0>
    WJR_CONSTEXPR20 void __release() {
        auto &al = pair.first();
        auto &vec = pair.second();
        NoinlineBuffer *buffer = vec.vec;

        while (buffer != nullptr) {
            auto prev = buffer->prev;
            al.deallocate(buffer, buffer->size);
            buffer = prev;
        }

        vec.vec = nullptr;
    }

    template <size_t E = Extent, std::enable_if_t<(E <= InlineThreshold), int> = 0>
    WJR_CONSTEXPR20 void __release() {
        auto &al = pair.first();
        auto &arr = pair.second();

        for (size_t idx = arr.idx - 1; ~idx; --idx) {
            InlineBuffer &buffer = arr.arr[idx];
            al.deallocate(buffer.buffer, buffer.size);
        }

        arr.idx = 0;
    }

public:
    WJR_CONSTEXPR20
    unique_stack_allocator(const StackAllocator &al,
                           std::in_place_index_t<Extent> = std::in_place_index<Extent>)
        : pair(al, {}) {}

    WJR_MALLOC WJR_CONSTEXPR20 void *allocate(size_t n) { return __allocate(n); }

    WJR_CONSTEXPR20 void release_one() { __release_one(); }

    WJR_CONSTEXPR20 void release() { __release(); }

    WJR_INTRINSIC_CONSTEXPR20 ~unique_stack_allocator() { release(); }

    unique_stack_allocator(const unique_stack_allocator &) = delete;
    unique_stack_allocator &operator=(const unique_stack_allocator &) = delete;

private:
    struct InlineBuffer {
        void *buffer;
        size_t size;
    };

    struct InlineBufferArray {
        size_t idx = 0;
        std::array<InlineBuffer, Extent> arr;
    };

    struct NoinlineBuffer {
        NoinlineBuffer *prev;
        size_t size;
        char buffer[];
    };

    struct NoinlineBufferVec {
        NoinlineBuffer *vec = nullptr;
    };

    using __Buffer = std::conditional_t<IsInline, InlineBufferArray, NoinlineBufferVec>;

    compressed_pair<StackAllocator, __Buffer> pair;
};

template <typename StackAllocator, size_t Extent>
unique_stack_allocator(StackAllocator &, std::in_place_index_t<Extent>)
    -> unique_stack_allocator<StackAllocator, Extent>;

} // namespace wjr

#endif // WJR_STACK_ALLOCATOR_HPP__