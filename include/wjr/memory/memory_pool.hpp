#ifndef WJR_MEMORY_MEMORY_POOL_HPP__
#define WJR_MEMORY_MEMORY_POOL_HPP__

#include <wjr/container/intrusive/list.hpp>
#include <wjr/memory/detail.hpp>

namespace wjr {

struct automatic_free_pool {
    struct chunk : list_node<intrusive_tag<chunk>> {};

    automatic_free_pool() noexcept { init(&head); }
    ~automatic_free_pool() noexcept {
        for (auto iter = head.begin(); iter != head.end();) {
            const auto now = iter++;
            chunk *const node = &**now;
            free(node);
        }
    }

    WJR_MALLOC void *allocate(size_t n) noexcept {
        auto *const ptr = (chunk *)malloc(n + sizeof(chunk));
        push_back(&head, ptr);
        return reinterpret_cast<char *>(ptr) + sizeof(chunk);
    }

    void deallocate(void *ptr) noexcept {
        auto *const node = (chunk *)(static_cast<char *>(ptr) - sizeof(chunk));
        remove_uninit(node);
        free(node);
    }

    static automatic_free_pool &get_instance() noexcept {
        static thread_local automatic_free_pool instance;
        return instance;
    }

    chunk head;
};

namespace memory_pool_detail {

static constexpr uint8_t __ctz_table[32] = {
    0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
};

} // namespace memory_pool_detail

class __default_alloc_template__ {
private:
    union obj {
        union obj *free_list_link;
        char client_data[1];
    };

    WJR_CONST static constexpr size_t __round_up(size_t bytes) noexcept {
        return (((bytes) + 2048 - 1) & ~(2048 - 1));
    }

    WJR_CONST static constexpr uint8_t __get_index(uint32_t bytes) noexcept {
        if (bytes <= 256) {
            return memory_pool_detail::__ctz_table[(bytes - 1) >> 3];
        }

        return memory_pool_detail::__ctz_table[(bytes - 1) >> 9] + 6;
    }

    WJR_CONST static constexpr uint32_t __get_size(uint8_t idx) noexcept {
        return static_cast<uint32_t>(1) << (idx + 3);
    }

    static automatic_free_pool &get_chunk() noexcept {
        return automatic_free_pool::get_instance();
    }

public:
    struct object {

        WJR_INTRINSIC_INLINE allocation_result<void *>
        __small_allocate(uint32_t n) noexcept {
            const uint8_t idx = __get_index(n);
            const size_t size = __get_size(idx);
            obj *volatile *const my_free_list = free_list + idx;
            obj *const result = *my_free_list;
            if (WJR_LIKELY(result != nullptr)) {
                *my_free_list = result->free_list_link;
                return {result, size};
            }

            return {refill(idx), size};
        }

        WJR_INTRINSIC_INLINE void __small_deallocate(void *p, uint32_t n) noexcept {
            auto *const q = static_cast<obj *>(p);
            obj *volatile *const my_free_list = free_list + __get_index(n);
            q->free_list_link = *my_free_list;
            *my_free_list = q;
        }

        // n must be > 0
        WJR_INTRINSIC_INLINE allocation_result<void *> allocate(size_t n) noexcept {
            if (WJR_LIKELY(n <= 16384)) {
                return __small_allocate(static_cast<uint32_t>(n));
            }

            return {malloc(n), n};
        }

        // p must not be 0
        WJR_INTRINSIC_INLINE void deallocate(void *p, size_t n) noexcept {
            if (WJR_LIKELY(n <= 16384)) {
                return __small_deallocate(p, static_cast<uint32_t>(n));
            }

            free(p);
        }

        allocation_result<void *> chunk_allocate(size_t n) noexcept {
            if (WJR_LIKELY(n <= 16384)) {
                return __small_allocate(static_cast<uint32_t>(n));
            }

            return {get_chunk().allocate(n), n};
        }

        // p must not be 0
        WJR_INTRINSIC_INLINE void chunk_deallocate(void *p, size_t n) noexcept {
            if (WJR_LIKELY(n <= 16384)) {
                return __small_deallocate(p, static_cast<uint32_t>(n));
            }

            get_chunk().deallocate(p);
        }

    private:
        // Allocates a chunk for nobjs of size "size".  nobjs may be reduced
        // if it is inconvenient to allocate the requested number.
        WJR_MALLOC char *chunk_alloc(uint8_t idx, unsigned int &nobjs) noexcept;

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

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(memory_pool);

    template <typename Other>
    constexpr memory_pool(const memory_pool<Other> &) noexcept {}

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
     * @details Allocate memory, don't need to deallocate it until the thread exits.    \n
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