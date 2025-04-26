#ifndef WJR_MEMORY_AUTOMATIC_FREE_POOL_HPP__
#define WJR_MEMORY_AUTOMATIC_FREE_POOL_HPP__

#include <wjr/container/list.hpp>
#include <wjr/memory/align.hpp>

namespace wjr {

/**
 * @brief Automatic free all memory when thread exit.
 * @details For example, it is possible to have a trivial destructor to thread_local
 * variables that require high performance. Such as stack allocator.
 *
 */
struct automatic_free_pool {
    struct chunk : intrusive::list_node<chunk> {};
    static constexpr size_t aligned_header_size =
        align_up(sizeof(chunk), mem::default_new_alignment);

    automatic_free_pool() = default;
    ~automatic_free_pool() noexcept {
        chunk *node, *next;
        WJR_LIST_FOR_EACH_ENTRY_SAFE(node, next, &head) { free(node); }
    }

    automatic_free_pool(const automatic_free_pool &) = delete;
    automatic_free_pool(automatic_free_pool &&other) {
        replace(&other.head, &head);
        other.head.init_self();
    }
    automatic_free_pool &operator=(const automatic_free_pool &) = delete;
    automatic_free_pool &operator=(automatic_free_pool &&) = delete;

    WJR_MALLOC void *allocate(size_t n) {
        auto *const raw = malloc(n + aligned_header_size);
        head.push_back(static_cast<chunk *>(raw));
        return static_cast<void *>(static_cast<std::byte *>(raw) + aligned_header_size);
    }

    void deallocate(void *ptr) noexcept {
        auto *const node = static_cast<std::byte *>(ptr) - aligned_header_size;
        reinterpret_cast<chunk *>(node)->remove();
        free(node);
    }

    static automatic_free_pool &get_instance() noexcept {
        static thread_local automatic_free_pool instance;
        return instance;
    }

    chunk head;
};

} // namespace wjr

#endif // WJR_MEMORY_AUTOMATIC_FREE_POOL_HPP__