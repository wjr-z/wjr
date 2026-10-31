#ifndef WJR_MEMORY_AUTOMATIC_FREE_POOL_HPP__
#define WJR_MEMORY_AUTOMATIC_FREE_POOL_HPP__

#include <exception>
#include <limits>
#include <memory>

#include <wjr/container/intrusive_algorithms.hpp>
#include <wjr/memory/aligned_alloc.hpp>

namespace wjr {

/**
 * @brief Automatic free all memory when thread exit.
 * @details For example, it is possible to have a trivial destructor to thread_local
 * variables that require high performance. Such as stack allocator. This is usually used to
 * allocate larger blocks of memory.
 *
 * @note Allocation failure terminates the process.
 *
 */
class automatic_free_pool {
    struct chunk : intrusive::list_node<chunk> {};

public:
    static constexpr size_t aligned_header_size =
        align_up(sizeof(chunk), mem::default_new_alignment);

    automatic_free_pool() = default;
    ~automatic_free_pool() noexcept {
        intrusive::detail::dispose_nodes(head.next(), &head, [](chunk *node) noexcept {
            mem::_default_deallocate(node, std::nothrow);
        });
    }

    automatic_free_pool(const automatic_free_pool &) = delete;
    automatic_free_pool(automatic_free_pool &&other) noexcept {
        replace(&other.head, &head);
        other.head.init_self();
    }

    automatic_free_pool &operator=(const automatic_free_pool &) = delete;
    automatic_free_pool &operator=(automatic_free_pool &&other) noexcept {
        if (this != std::addressof(other)) {
            intrusive::detail::dispose_nodes(head.next(), &head, [](chunk *node) noexcept {
                mem::_default_deallocate(node, std::nothrow);
            });
            replace(&other.head, &head);
            other.head.init_self();
        }
        return *this;
    }

    /// @brief Release every tracked allocation, leaving the pool empty and reusable.
    /// @note Invalidates all outstanding allocation pointers. Does not destroy stored objects.
    void clear() noexcept {
        intrusive::clear_and_dispose(
            head, [](chunk *node) noexcept { mem::_default_deallocate(node, std::nothrow); });
    }

    /// @pre n + aligned_header_size is representable in size_t.
    WJR_MALLOC void *allocate(size_t n) noexcept {
        WJR_ASSERT_ASSUME_L2(n <= std::numeric_limits<size_t>::max() - aligned_header_size);
        auto *const raw = mem::_default_allocate(n + aligned_header_size, std::nothrow);
        if (WJR_UNLIKELY(raw == nullptr)) {
            std::terminate();
        }
        head.push_back(static_cast<chunk *>(raw));
        return static_cast<void *>(static_cast<std::byte *>(raw) + aligned_header_size);
    }

    static void deallocate(void *ptr) noexcept {
        auto *const node = static_cast<std::byte *>(ptr) - aligned_header_size;
        reinterpret_cast<chunk *>(node)->remove();
        mem::_default_deallocate(node, std::nothrow);
    }

private:
    chunk head;
};

} // namespace wjr

#endif // WJR_MEMORY_AUTOMATIC_FREE_POOL_HPP__