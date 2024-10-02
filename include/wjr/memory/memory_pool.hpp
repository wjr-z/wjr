/**
 * @file memory_pool.hpp
 * @author wjr
 * @brief
 * @version 0.1
 * @date 2024-09-22
 * @todo Multithread memory pool.
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef WJR_MEMORY_MEMORY_POOL_HPP__
#define WJR_MEMORY_MEMORY_POOL_HPP__

#include <wjr/container/forward_list.hpp>
#include <wjr/container/list.hpp>
#include <wjr/math/bit.hpp>
#include <wjr/math/literals.hpp>
#include <wjr/memory/detail.hpp>

namespace wjr {

struct automatic_free_pool {
    using chunk = intrusive::list_node;

    automatic_free_pool() noexcept { init(&head); }
    ~automatic_free_pool() noexcept {
        chunk *node = head.next;
        while (node != std::addressof(head)) {
            auto *const nxt = node->next;
            free(node);
            node = nxt;
        }
    }

    automatic_free_pool(const automatic_free_pool &) = delete;
    automatic_free_pool(automatic_free_pool &&) = delete;
    automatic_free_pool &operator=(const automatic_free_pool &) = delete;
    automatic_free_pool &operator=(automatic_free_pool &&) = delete;

    WJR_MALLOC void *allocate(size_t n) noexcept {
        auto *const ptr = static_cast<chunk *>(malloc(n + sizeof(chunk)));
        push_back(&head, ptr);
        return reinterpret_cast<char *>(ptr) + sizeof(chunk);
    }

    void deallocate(void *ptr) noexcept {
        auto *const node =
            reinterpret_cast<chunk *>(static_cast<char *>(ptr) - sizeof(chunk));
        remove_uninit(node);
        free(node);
    }

    static automatic_free_pool &get_instance() noexcept {
        static thread_local automatic_free_pool instance;
        return instance;
    }

    chunk head;
};

class __default_alloc_template__ {
    union obj {
        union obj *free_list_link;
        char client_data[1];
    };

    WJR_CONST static constexpr size_t __round_up(size_t bytes) noexcept {
        return (((bytes) + 2048 - 1) & ~(2048 - 1));
    }

    WJR_CONST static WJR_CONSTEXPR20 unsigned int
    __get_index(unsigned int bytes) noexcept {
        return static_cast<unsigned int>(
            bit_width<uint16_t>(static_cast<uint16_t>((bytes - 1) >> 3)));
    }

    WJR_CONST static constexpr unsigned int __get_size(unsigned int idx) noexcept {
        return static_cast<unsigned int>(1) << (idx + 3);
    }

    static automatic_free_pool &get_chunk() noexcept {
        return automatic_free_pool::get_instance();
    }

    struct object {
        WJR_MALLOC static void *chunk_allocate(size_t n) noexcept {
            return get_chunk().allocate(n);
        }

        // p must not be 0
        WJR_INTRINSIC_INLINE static void chunk_deallocate(void *p, size_t n) noexcept {
            get_chunk().deallocate(p);
        }
    };

public:
    static object &get_instance() noexcept {
        static object instance;
        return instance;
    }

    // n must be > 0
    WJR_MALLOC static void *chunk_allocate(size_t n) noexcept {
        return get_instance().chunk_allocate(n);
    }

    // p must not be 0
    static void chunk_deallocate(void *p, size_t n) noexcept {
        get_instance().chunk_deallocate(p, n);
    }
};

template <typename Ty>
class memory_pool : private std::allocator<Ty> {
private:
    using allocator_type = __default_alloc_template__;
    using Mybase = std::allocator<Ty>;

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

    WJR_NODISCARD WJR_CONSTEXPR20 auto allocate_at_least(size_type n) noexcept {
        return wjr::allocate_at_least(static_cast<Mybase &>(*this), n);
    }

    WJR_NODISCARD WJR_CONSTEXPR20 WJR_MALLOC Ty *allocate(size_type n) noexcept {
        return Mybase::allocate(n);
    }

    WJR_CONSTEXPR20 void deallocate(Ty *ptr, size_type n) noexcept {
        Mybase::deallocate(ptr, n);
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
        return static_cast<Ty *>(allocator_type::chunk_allocate(n * sizeof(Ty)));
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