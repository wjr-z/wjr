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

/**
 * @brief Automatic free all memory when thread exit.
 * @details For example, it is possible to have a trivial destructor to thread_local
 * variables that require high performance. Such as stack allocator.
 *
 */
struct automatic_free_pool {
    struct chunk : intrusive::list_node<chunk> {};

    automatic_free_pool() = default;
    ~automatic_free_pool() noexcept {
        chunk *node, *next;
        WJR_LIST_FOR_EACH_ENTRY_SAFE(node, next, &head) { free(node); }
    }

    automatic_free_pool(const automatic_free_pool &) = delete;
    automatic_free_pool(automatic_free_pool &&) = delete;
    automatic_free_pool &operator=(const automatic_free_pool &) = delete;
    automatic_free_pool &operator=(automatic_free_pool &&) = delete;

    WJR_MALLOC void *allocate(size_t n) noexcept {
        auto *const ptr = static_cast<chunk *>(::operator new[](n + sizeof(chunk)));
        head.push_back(ptr);
        return reinterpret_cast<std::byte *>(ptr) + sizeof(chunk);
    }

    void deallocate(void *ptr) noexcept {
        auto *const node = reinterpret_cast<chunk *>(static_cast<std::byte *>(ptr) - sizeof(chunk));
        node->remove();
        ::operator delete[](node);
    }

    static automatic_free_pool &get_instance() noexcept {
        static thread_local automatic_free_pool instance;
        return instance;
    }

    chunk head;
};

class __default_alloc_template__ {
    static automatic_free_pool &get_chunk() noexcept { return automatic_free_pool::get_instance(); }

    struct object {
        WJR_MALLOC static void *chunk_allocate(size_t n) noexcept {
            return get_chunk().allocate(n);
        }

        // p must not be 0
        WJR_INTRINSIC_INLINE static void chunk_deallocate(void *p, size_t) noexcept {
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

static_assert(std::is_empty_v<std::allocator<char>>);

} // namespace wjr

#endif // WJR_MEMORY_MEMORY_POOL_HPP__