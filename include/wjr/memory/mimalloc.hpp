#ifndef WJR_MEMORY_MIMALLOC_HPP__
#define WJR_MEMORY_MIMALLOC_HPP__

#include <cstddef>
#include <limits>
#include <memory_resource>
#include <type_traits>

#include <mimalloc.h>

#include <wjr/config.hpp>

namespace wjr {

namespace mimalloc_detail {
/// @brief Report allocation failure using the library's exception policy.
WJR_NORETURN WJR_COLD void allocation_failure();
} // namespace mimalloc_detail

/**
 * @brief Stateless allocator selecting mimalloc for an individual container.
 * @note No construct/destroy hooks interfere with ordinary allocator-traits relocation.
 * @pre Storage is freed through the same mimalloc runtime instance that allocated it.
 */
template <typename T>
class mimalloc_allocator {
public:
    using value_type = T;
    using is_always_equal = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;

    constexpr mimalloc_allocator() noexcept = default;
    template <typename U>
    constexpr mimalloc_allocator(const mimalloc_allocator<U> &) noexcept {}

    /// @brief Allocate uninitialized, correctly aligned storage for count elements.
    /// @throws std::bad_alloc On size overflow or backing allocation failure.
    WJR_NODISCARD WJR_MALLOC T *allocate(std::size_t count) {
        if (count > max_size()) {
            mimalloc_detail::allocation_failure();
        }
        void *pointer;
        if constexpr (alignof(T) <= alignof(std::max_align_t)) {
            pointer = mi_malloc(count * sizeof(T));
        } else {
            pointer = mi_malloc_aligned(count * sizeof(T), alignof(T));
        }
        if (pointer == nullptr) {
            mimalloc_detail::allocation_failure();
        }
        return static_cast<T *>(pointer);
    }

    /// @pre pointer and count describe an outstanding allocation from an equal allocator.
    void deallocate(T *pointer, std::size_t count) noexcept {
        if constexpr (alignof(T) <= alignof(std::max_align_t)) {
            mi_free_size(pointer, count * sizeof(T));
        } else {
            mi_free_size_aligned(pointer, count * sizeof(T), alignof(T));
        }
    }

    constexpr std::size_t max_size() const noexcept {
        return std::numeric_limits<std::size_t>::max() / sizeof(T);
    }
};

template <typename T, typename U>
constexpr bool operator==(const mimalloc_allocator<T> &, const mimalloc_allocator<U> &) noexcept {
    return true;
}

/**
 * @brief Thread-safe PMR adapter for mimalloc's ordinary allocation domain.
 * @pre This resource outlives its allocations' PMR owners, including moved-out values.
 * @note Resource identity defines equality. Destruction does not bulk-release allocations
 * or change the process/default PMR resource; underlying caches belong to mimalloc.
 */
class mimalloc_memory_resource final : public std::pmr::memory_resource {
public:
    mimalloc_memory_resource() noexcept = default;
    mimalloc_memory_resource(const mimalloc_memory_resource &) = delete;
    mimalloc_memory_resource &operator=(const mimalloc_memory_resource &) = delete;
    ~mimalloc_memory_resource() override = default;

private:
    void *do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void *pointer, std::size_t bytes, std::size_t alignment) noexcept override;
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override;
};

} // namespace wjr

#endif // WJR_MEMORY_MIMALLOC_HPP__
