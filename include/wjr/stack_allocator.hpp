#ifndef WJR_STACK_ALLOCATOR_HPP__
#define WJR_STACK_ALLOCATOR_HPP__

#include <cstddef>
#include <vector>

#include <wjr/container/list.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

// preview :
// stack allocator

class native_stack_allocator {
public:
    native_stack_allocator(void *ptr) : ptr(ptr) {}
    native_stack_allocator(const native_stack_allocator &) = delete;
    native_stack_allocator &operator=(const native_stack_allocator &) = delete;
    ~native_stack_allocator() = default;

    WJR_ATTRIBUTES(NODISCARD, INTRINSIC_CONSTEXPR20) void *allocate(size_t n) {
        auto ret = ptr;
        ptr = static_cast<void *>(static_cast<char *>(ptr) + n);
        return static_cast<void *>(ret);
    }

    WJR_INTRINSIC_CONSTEXPR20 void deallocate(void *old, WJR_MAYBE_UNUSED size_t n) {
        WJR_ASSERT(ptr == static_cast<void *>(static_cast<char *>(old) + n));
        ptr = old;
        (void)(n);
    }

    WJR_INTRINSIC_CONSTEXPR void *get() const { return ptr; }

private:
    void *ptr;
};

template <size_t cache, size_t alignment = alignof(std::max_align_t)>
class basic_stack_allocator {
public:
    basic_stack_allocator() : alloc(bytes) {}
    basic_stack_allocator(const basic_stack_allocator &) = delete;
    basic_stack_allocator &operator=(const basic_stack_allocator &) = delete;
    ~basic_stack_allocator() { WJR_ASSERT(alloc.get() == bytes); }

    WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 void *allocate(size_t n) {
        return alloc.allocate(n);
    }

    WJR_INTRINSIC_CONSTEXPR20 void deallocate(void *old, WJR_MAYBE_UNUSED size_t n) {
        return alloc.deallocate(old, n);
    }

    WJR_INTRINSIC_CONSTEXPR size_t size() const {
        return static_cast<char *>(alloc.get()) - bytes;
    }

private:
    native_stack_allocator alloc;
    alignas(alignment) char bytes[cache];
};

// TODO
// 1. optimize for cache_blocks == -1.
// 2. optimize memory fragmentation caused by large object allocation
template <size_t cache0, size_t threshold0, size_t cache1, size_t threshold1,
          size_t alignment = alignof(std::max_align_t)>
class stack_allocator {

    template <size_t cache>
    class stack_alloc {
        using node = basic_stack_allocator<cache, alignment>;

        WJR_CONSTEXPR20 void malloc_node() {
            WJR_ASSERT(idx == stk.size());
            stk.emplace_back(new node);
        }

    public:
        stack_alloc() = default;
        stack_alloc(const stack_alloc &) = delete;
        stack_alloc(stack_alloc &&) = default;
        stack_alloc &operator=(const stack_alloc &) = delete;
        stack_alloc &operator=(stack_alloc &&) = default;
        ~stack_alloc() = default;

        WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 void *allocate(size_t n) {
            if (WJR_UNLIKELY(stk.empty() || stk[idx]->size() + n > cache)) {
                ++idx;
                if (WJR_UNLIKELY(idx == stk.size())) {
                    malloc_node();
                }
            }

            return stk[idx]->allocate(n);
        }

        WJR_INTRINSIC_CONSTEXPR20 void deallocate(void *ptr, size_t n) {
            stk[idx]->deallocate(ptr, n);
            if (WJR_UNLIKELY(idx && stk[idx]->size() == 0)) {
                --idx;
                auto size = stk.size();
                auto idx2 = size <= 8 ? idx + 4 : idx + idx / 2;

                if (WJR_UNLIKELY(idx2 <= size)) {
                    delete stk.back();
                    stk.pop_back();
                }
            }
        }

    private:
        size_t idx = -1ull;
        std::vector<node *> stk;
    };

public:
    stack_allocator() = default;
    stack_allocator(const stack_allocator &) = delete;
    stack_allocator(stack_allocator &&) = default;
    stack_allocator &operator=(const stack_allocator &) = delete;
    stack_allocator &operator=(stack_allocator &&) = default;
    ~stack_allocator() = default;

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
    stack_alloc<cache0> small_alloc;
    stack_alloc<cache1> mid_alloc;
};

// Disable move constructor
template <typename StackAllocator>
class unique_stack_ptr {
public:
    WJR_INTRINSIC_CONSTEXPR20 unique_stack_ptr(StackAllocator &al, size_t size)
        : al(al), size(size) {
        ptr = al.allocate(size);
    }

    WJR_INTRINSIC_CONSTEXPR20 ~unique_stack_ptr() {
        if (ptr) {
            al.deallocate(ptr, size);
        }
    }

    unique_stack_ptr(const unique_stack_ptr &) = delete;
    unique_stack_ptr &operator=(const unique_stack_ptr &) = delete;

    WJR_INTRINSIC_CONSTEXPR20 void *get() const { return ptr; }

    WJR_INTRINSIC_CONSTEXPR20 void release() {
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