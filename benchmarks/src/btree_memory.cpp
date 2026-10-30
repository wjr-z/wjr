#include "btree_focused.hpp"

#include <iostream>

namespace btree_focused {

struct allocation_stats {
    size_t live = 0;
    size_t peak = 0;
    size_t allocated = 0;
    size_t allocations = 0;
    size_t deallocations = 0;
};
// Only this single-threaded, separate executable uses these counters. Every
// rebound allocator shares the active pass; timing uses std::allocator instead.
inline allocation_stats *active_stats = nullptr;

template <typename T>
struct counting_allocator {
    using value_type = T;
    using is_always_equal = std::true_type;
    counting_allocator() = default;
    template <typename U>
    counting_allocator(const counting_allocator<U> &) noexcept {}
    T *allocate(size_t n) {
        T *result = std::allocator<T>{}.allocate(n);
        const auto bytes = n * sizeof(T);
        active_stats->live += bytes;
        active_stats->peak = std::max(active_stats->peak, active_stats->live);
        active_stats->allocated += bytes;
        ++active_stats->allocations;
        return result;
    }
    void deallocate(T *ptr, size_t n) noexcept {
        active_stats->live -= n * sizeof(T);
        ++active_stats->deallocations;
        std::allocator<T>{}.deallocate(ptr, n);
    }
    template <typename U>
    bool operator==(const counting_allocator<U> &) const noexcept {
        return true;
    }
};

template <typename S, typename Tree>
size_t owned_string_capacity(const Tree &tree) {
    size_t result = 0;
    if constexpr (std::is_same_v<typename S::key_type, std::string>) {
        for (const auto &value : tree) {
            const auto &key = get_key<S>(value);
            const auto storage = reinterpret_cast<uintptr_t>(key.data());
            const auto object = reinterpret_cast<uintptr_t>(&key);
            if (storage < object || storage >= object + sizeof(key)) {
                result += key.capacity() + 1;
            }
        }
    }
    return result;
}

template <typename S, typename Tree>
void memory_row(const std::string &name, S spec, size_t n) {
    const workload<S> data(n, spec);
    allocation_stats stats;
    active_stats = &stats;
    validate<S, Tree>(data);
    if (stats.live != 0) {
        throw std::runtime_error("validation leaked allocations");
    }
    stats = {};
    size_t built = 0, after_mixed = 0, strings = 0, after_clear = 0;
    {
        Tree tree;
        build<S>(tree, data);
        built = stats.live;
        mixed<S>(tree, data);
        after_mixed = stats.live;
        strings = owned_string_capacity<S>(tree);
        tree.clear();
        after_clear = stats.live;
    }
    active_stats = nullptr;
    if (stats.live != 0 || stats.allocations != stats.deallocations) {
        throw std::runtime_error("allocator did not balance");
    }
    std::cout << name << ',' << n << ',' << sizeof(Tree) << ',' << sizeof(typename Tree::iterator)
              << ',' << built << ',' << after_mixed << ',' << stats.peak << ',' << stats.allocated
              << ',' << stats.allocations << ',' << stats.deallocations << ',' << after_clear << ','
              << stats.live << ',' << strings << ",NA\n";
}

template <typename S>
void memory_scenario(const std::string &name, S spec) {
    for (size_t n : {0, 1, 2, 3, 64, 4096, 262144}) {
        memory_row<S, typename wjr_backend<S, counting_allocator<char>, 16>::type>(name + "/wjr16",
                                                                                   spec, n);
        memory_row<S, typename wjr_backend<S, counting_allocator<char>, 32>::type>(name + "/wjr32",
                                                                                   spec, n);
        memory_row<S, typename std_backend<S, counting_allocator>::type>(name + "/std", spec, n);
#ifdef WJR_BENCHMARK_ABSEIL
        memory_row<S, typename absl_backend<S, counting_allocator>::type>(name + "/absl", spec, n);
#endif
    }
}

} // namespace btree_focused

int main() {
    using namespace btree_focused;
    std::cout << "# source=" << WJR_BENCHMARK_SOURCE << " compiler=" << WJR_BENCHMARK_COMPILER
              << " stdlib=" << standard_library() << " flags=" << WJR_BENCHMARK_FLAGS
              << " string_sizeof=" << sizeof(std::string) << '\n';
    std::cout << "case,n,tree_size,iterator_size,build_live_requested,mixed_live_requested,"
                 "peak_requested,total_requested,allocations,deallocations,after_clear_requested,"
                 "after_destroy_requested,element_string_capacity_bytes,pool_reserved_bytes\n";
    memory_scenario("u64_set", scenario<uint64_t, void>{});
    memory_scenario("u64_map", scenario<uint64_t, uint64_t>{});
    memory_scenario("short_string_map", scenario<std::string, uint64_t>{12});
    memory_scenario("long_string_map", scenario<std::string, uint64_t>{96});
    memory_scenario("large_map", scenario<uint64_t, large_value>{});
}
