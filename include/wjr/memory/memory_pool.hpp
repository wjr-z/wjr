#ifndef WJR_MEMORY_MEMORY_POOL_HPP__
#define WJR_MEMORY_MEMORY_POOL_HPP__

#include <wjr/container/intrusive/list.hpp>
#include <wjr/crtp/trivially_allocator_base.hpp>
#include <wjr/math/details.hpp>
#include <wjr/memory/details.hpp>

namespace wjr {

namespace memory_pool_details {

static constexpr uint8_t __ctz_table[32] = {
    0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
};

} // namespace memory_pool_details

class __default_alloc_template__ {
private:
    union obj {
        union obj *free_list_link;
        char client_data[1];
    };

    struct __list_node : intrusive::list_node<> {};

    struct malloc_chunk {

        malloc_chunk() noexcept { init(&head); }
        ~malloc_chunk() noexcept {
            for (auto iter = head.begin(); iter != head.end();) {
                auto now = iter++;
                auto node = static_cast<__list_node *>(&*now);
                free(node);
            }
        }

        WJR_MALLOC void *allocate(size_t n) noexcept {
            __list_node *ptr = (__list_node *)malloc(n + sizeof(__list_node));
            push_back(&head, ptr);
            return (char *)(ptr) + sizeof(__list_node);
        }

        void deallocate(void *ptr) noexcept {
            auto node = (__list_node *)((char *)(ptr) - sizeof(__list_node));
            remove_uninit(node);
            free(node);
        }

        __list_node head;
    };

    static WJR_INTRINSIC_CONSTEXPR size_t __round_up(size_t bytes) noexcept {
        return (((bytes) + 2048 - 1) & ~(2048 - 1));
    }

    static WJR_INTRINSIC_CONSTEXPR uint8_t __get_index(size_t bytes) noexcept {
        if (bytes <= 256) {
            return memory_pool_details::__ctz_table[(bytes - 1) >> 3];
        }

        return memory_pool_details::__ctz_table[(bytes - 1) >> 9] + 6;
    }

    static WJR_INTRINSIC_CONSTEXPR uint16_t __get_size(uint8_t idx) noexcept {
        return (uint16_t)(1) << (idx + 3);
    }

    static malloc_chunk &get_chunk() noexcept {
        static thread_local malloc_chunk chunk;
        return chunk;
    }

public:
    struct object {

        WJR_INTRINSIC_INLINE allocation_result<void *>
        __small_allocate(size_t n) noexcept {
            const size_t idx = __get_index(n);
            const size_t size = __get_size(idx);
            obj *volatile *my_free_list = free_list + idx;
            obj *result = *my_free_list;
            if (WJR_LIKELY(result != nullptr)) {
                *my_free_list = result->free_list_link;
                return {result, size};
            }

            return {refill(idx), size};
        }

        WJR_INTRINSIC_INLINE void __small_deallocate(void *p, size_t n) noexcept {
            obj *q = (obj *)p;
            obj *volatile *my_free_list = free_list + __get_index(n);
            q->free_list_link = *my_free_list;
            *my_free_list = q;
        }

        // n must be > 0
        allocation_result<void *> allocate(size_t n) noexcept {
            if (WJR_LIKELY(n <= 16384)) {
                return __small_allocate(n);
            }

            return {malloc(n), n};
        }

        // p must not be 0
        WJR_INTRINSIC_INLINE void deallocate(void *p, size_t n) noexcept {
            if (WJR_LIKELY(n <= 16384)) {
                return __small_deallocate(p, n);
            }

            free(p);
        }

        allocation_result<void *> chunk_allocate(size_t n) noexcept {
            if (WJR_LIKELY(n <= 16384)) {
                return __small_allocate(n);
            }

            return {get_chunk().allocate(n), n};
        }

        // p must not be 0
        WJR_INTRINSIC_INLINE void chunk_deallocate(void *p, size_t n) noexcept {
            if (WJR_LIKELY(n <= 16384)) {
                return __small_deallocate(p, n);
            }

            get_chunk().deallocate(p);
        }

    private:
        // Allocates a chunk for nobjs of size "size".  nobjs may be reduced
        // if it is inconvenient to allocate the requested number.
        WJR_MALLOC char *chunk_alloc(uint8_t idx, int &nobjs) noexcept;

        // Returns an object of size n, and optionally adds to size n free list.
        WJR_MALLOC void *refill(uint8_t idx) noexcept;

        obj *volatile free_list[12] = {nullptr};
        char *start_free = nullptr;
        char *end_free = nullptr;
        size_t heap_size = 0;
    };

    static object &get_instance() noexcept {
        static thread_local object instance;
        return instance;
    }

    // n must be > 0
    static allocation_result<void *> allocate(size_t n) noexcept {
        return get_instance().allocate(n);
    }

    // p must not be 0
    WJR_INTRINSIC_INLINE static void deallocate(void *p, size_t n) noexcept {
        get_instance().deallocate(p, n);
    }

    // n must be > 0
    static allocation_result<void *> chunk_allocate(size_t n) noexcept {
        return get_instance().chunk_allocate(n);
    }

    // p must not be 0
    WJR_INTRINSIC_INLINE static void chunk_deallocate(void *p, size_t n) noexcept {
        get_instance().chunk_deallocate(p, n);
    }
};

template <typename Ty>
class memory_pool {
private:
    using allocator_type = __default_alloc_template__;

public:
    using value_type = Ty;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::true_type;
    using is_trivially_allocator = std::true_type;

    template <typename Other>
    struct rebind {
        using other = memory_pool<Other>;
    };

    constexpr memory_pool() noexcept = default;
    constexpr memory_pool(const memory_pool &) noexcept = default;
    template <typename Other>
    constexpr memory_pool(const memory_pool<Other> &) noexcept {}
    ~memory_pool() = default;
    memory_pool &operator=(const memory_pool &) noexcept = default;

    WJR_NODISCARD WJR_CONSTEXPR20 allocation_result<Ty *>
    allocate_at_least(size_type n) const noexcept {
        const auto ret = allocator_type::allocate(n * sizeof(Ty));
        return {static_cast<Ty *>(ret.ptr), ret.count};
    }

    WJR_NODISCARD WJR_CONSTEXPR20 allocation_result<Ty *>
    chunk_allocate_at_least(size_type n) const noexcept {
        const auto ret = allocator_type::chunk_allocate(n * sizeof(Ty));
        return {static_cast<Ty *>(ret.ptr), ret.count};
    }

    WJR_NODISCARD WJR_CONSTEXPR20 WJR_MALLOC Ty *allocate(size_type n) const noexcept {
        return allocate_at_least(n).ptr;
    }

    WJR_CONSTEXPR20 void deallocate(Ty *ptr, size_type n) const noexcept {
        return allocator_type::deallocate(static_cast<void *>(ptr), sizeof(Ty) * n);
    }

    /**
     * @details Allocate memory, don't need to deallocate it until the thread exits.   \n
     * Automatically deallocate memory when the thread exits.                          \n
     * Used in thread_local memory pool that only needs to allocate memory once and    \n
     * deallocate it when the thread exits.                                            \n
     *
     */
    WJR_NODISCARD WJR_CONSTEXPR20 WJR_MALLOC Ty *
    chunk_allocate(size_type n) const noexcept {
        return chunk_allocate_at_least(n).ptr;
    }

    WJR_CONSTEXPR20 void chunk_deallocate(Ty *ptr, size_type n) const noexcept {
        return allocator_type::chunk_deallocate(static_cast<void *>(ptr), sizeof(Ty) * n);
    }

    constexpr size_t max_size() const noexcept {
        return static_cast<size_t>(-1) / sizeof(Ty);
    }
};

template <typename T, typename U>
constexpr bool operator==(const memory_pool<T> &, const memory_pool<U> &) noexcept {
    return true;
}

template <typename T, typename U>
constexpr bool operator!=(const memory_pool<T> &, const memory_pool<U> &) noexcept {
    return false;
}

} // namespace wjr

#endif // WJR_MEMORY_MEMORY_POOL_HPP__