#ifndef WJR_STACK_ALLOCATOR_HPP__
#define WJR_STACK_ALLOCATOR_HPP__

#include <cstddef>

#include <wjr/container/list.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

template <size_t cache, size_t alignment = alignof(std::max_align_t)>
class native_stack_allocator {
public:
    native_stack_allocator() = default;
    native_stack_allocator(const native_stack_allocator &) = delete;
    native_stack_allocator &operator=(const native_stack_allocator &) = delete;
    ~native_stack_allocator() = default;

    WJR_ATTRIBUTES(NODISCARD, INTRINSIC_CONSTEXPR20) void *allocate(size_t n) {
        auto ret = ptr;
        ptr += n;
        WJR_ASSERT_L(1, ptr < bytes + cache, "stack allocator allocate overflow.");
        return static_cast<void *>(ret);
    }

    WJR_INTRINSIC_CONSTEXPR20 void deallocate(void *old, WJR_MAYBE_UNUSED size_t n) {
        char *set = static_cast<char *>(old);
        WJR_ASSERT_L(1, ptr == set + n);

        ptr = set;
        WJR_ASSERT_L(1, ptr >= bytes && ptr < bytes + cache,
                     "stack allocator reset error.");

        (void)(n);
    }

    WJR_INTRINSIC_CONSTEXPR size_t remain() const { return (bytes + cache) - ptr; }

private:
    char *ptr = bytes;
    alignas(alignment) char bytes[cache];
};

template <size_t cache, size_t obj_threashold, size_t cache_blocks = 2,
          size_t alignment = alignof(std::max_align_t)>
class stack_allocator {

    static_assert(obj_threashold <= cache / 2,
                  "malloc size must less or equal then cache / 2.");

    constexpr static size_t remain_cache_blocks = cache_blocks + 1;

    struct stack_node {
        stack_node *next = nullptr;
        stack_node *prev = nullptr;
        native_stack_allocator<cache, alignment> alloc;
    };

    WJR_INTRINSIC_CONSTEXPR20 void malloc_node() {
        WJR_ASSERT(rest == 0);
        rest = 1;
        auto node = new stack_node;

        if (tail != nullptr) {
            tail->next = node;
            node->prev = tail;
        }

        ptr = node;
        tail = node;
    }

    WJR_ATTRIBUTES(NODISCARD, INTRINSIC_CONSTEXPR20) void *allocate_small(size_t n) {
        if constexpr (cache_blocks != 0) {
            WJR_ASSERT(ptr != nullptr);

            if (WJR_UNLIKELY(ptr->alloc.remain() < n)) {
                --rest;
                ptr = ptr->next;
                if (WJR_UNLIKELY(ptr == nullptr)) {
                    malloc_node();
                }
            }
        } else {
            if (WJR_UNLIKELY(ptr && ptr->alloc.remain() < n)) {
                ptr = ptr->next;
                --rest;
            }

            if (WJR_UNLIKELY(ptr == nullptr)) {
                malloc_node();
            }
        }

        return ptr->alloc.allocate(n);
    }

    WJR_ATTRIBUTES(NODISCARD, INTRINSIC_CONSTEXPR20) void *allocate_large(size_t n) {
        return malloc(n);
    }

    WJR_INTRINSIC_CONSTEXPR20
    void deallocate_small(void *p, size_t n) {
        ptr->alloc.deallocate(p, n);

        if (ptr->alloc.remain() == 0) {
            ptr = ptr->prev;
            if (WJR_UNLIKELY(++rest == remain_cache_blocks)) {
                --rest;
                stack_node *tmp = tail;
                tail = tail->prev;
                delete tmp;
            }

            WJR_ASSERT(reset < remain_cache_blocks);
        }
    }

    WJR_INTRINSIC_CONSTEXPR20
    void deallocate_large(void *ptr, WJR_MAYBE_UNUSED size_t n) {
        free(ptr);
        (void)(n);
    }

public:
    WJR_CONSTEXPR20 stack_allocator() {
        if constexpr (cache_blocks != 0) {
            malloc_node();
        }
    }

    ~stack_allocator() = default;
    stack_allocator(const stack_allocator &) = delete;
    stack_allocator &operator=(const stack_allocator &) = delete;

    WJR_ATTRIBUTES(NODISCARD, INTRINSIC_CONSTEXPR20) void *allocate(size_t n) {
        if (WJR_UNLIKELY(n >= obj_threashold)) {
            return allocate_large(n);
        }

        return allocate_small(n);
    }

    WJR_INTRINSIC_CONSTEXPR20 void deallocate(void *p, size_t n) {
        if (WJR_UNLIKELY(n >= obj_threashold)) {
            return deallocate_large(p, n);
        }

        return deallocate_small(p, n);
    }

private:
    stack_node *ptr = nullptr;
    stack_node *tail = nullptr;
    size_t rest = 0;
};

// Disable move constructor
// It is necessary to ensure that the release sequence is correct
template <typename StackAllocator>
class unique_stack_ptr {
public:
    WJR_CONSTEXPR20 unique_stack_ptr(StackAllocator &al, size_t size)
        : al(al), size(size) {
        ptr = al.allocate(size);
    }

    ~unique_stack_ptr() {
        if (ptr) {
            al.deallocate(ptr, size);
        }
    }

    unique_stack_ptr(const unique_stack_ptr &) = delete;
    unique_stack_ptr &operator=(const unique_stack_ptr &) = delete;

    WJR_CONSTEXPR20 void *get() const { return ptr; }

    void release() {
        al.deallocate(ptr, size);
        ptr = nullptr;
    }

private:
    StackAllocator &al;
    void *ptr;
    size_t size;
};

template <typename StackAllocator>
unique_stack_ptr(StackAllocator &, size_t) -> unique_stack_ptr<StackAllocator>;

} // namespace wjr

#endif // WJR_STACK_ALLOCATOR_HPP__