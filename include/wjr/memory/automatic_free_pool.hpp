#ifndef WJR_MEMORY_AUTOMATIC_FREE_POOL_HPP__
#define WJR_MEMORY_AUTOMATIC_FREE_POOL_HPP__

#include <wjr/container/list.hpp>

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

} // namespace wjr

#endif // WJR_MEMORY_AUTOMATIC_FREE_POOL_HPP__