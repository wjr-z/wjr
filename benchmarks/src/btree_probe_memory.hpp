#ifndef WJR_BENCHMARKS_BTREE_PROBE_MEMORY_HPP__
#define WJR_BENCHMARKS_BTREE_PROBE_MEMORY_HPP__

#include <algorithm>
#include <memory>

namespace btree_probe {
struct counts {
    size_t live = 0;
    size_t peak = 0;
    size_t allocations = 0;
    size_t deallocations = 0;
};
inline counts *active = nullptr;

/// @brief Keep the single-threaded probe counter live until all fixture owners are destroyed.
class pass {
public:
    explicit pass(counts &stats) noexcept { active = &stats; }
    pass(const pass &) = delete;
    pass &operator=(const pass &) = delete;
    ~pass() noexcept { active = nullptr; }
};

template <typename T>
struct allocator {
    using value_type = T;
    using is_always_equal = std::true_type;
    allocator() = default;
    template <typename U>
    allocator(const allocator<U> &) noexcept {}
    T *allocate(size_t n) {
        auto *ptr = std::allocator<T>{}.allocate(n);
        active->live += n * sizeof(T);
        active->peak = std::max(active->peak, active->live);
        ++active->allocations;
        return ptr;
    }
    void deallocate(T *ptr, size_t n) noexcept {
        active->live -= n * sizeof(T);
        ++active->deallocations;
        std::allocator<T>{}.deallocate(ptr, n);
    }
    template <typename U>
    bool operator==(const allocator<U> &) const noexcept {
        return true;
    }
};
} // namespace btree_probe

#endif // WJR_BENCHMARKS_BTREE_PROBE_MEMORY_HPP__
