#include <wjr/container/experimental/btree_bulk.hpp>

#include <array>
#include <iostream>
#include <map>
#include <vector>

#ifdef WJR_BENCHMARK_ABSEIL
    #include <absl/container/btree_map.h>
#endif
#ifndef WJR_BULK_MEMORY
    #include <benchmark/benchmark.h>
#endif

namespace {
template <typename Value>
auto input_values(size_t count) {
    std::vector<std::pair<uint64_t, Value>> input;
    for (size_t i = 0; i < count; ++i)
        input.emplace_back(i, Value{});
    return input;
}

template <bool Packed, typename Tree, typename Input>
Tree build(const Input &input) {
    if constexpr (Packed) {
        return wjr::experimental::make_btree_from_sorted_unique<Tree>(input.begin(), input.end());
    } else {
        Tree tree;
        if constexpr (requires { tree.insert_with_hint(input.begin(), input.end()); }) {
            tree.insert_with_hint(input.begin(), input.end());
        } else {
            for (const auto &value : input)
                tree.insert(tree.end(), value);
        }
        return tree;
    }
}

#ifdef WJR_BULK_MEMORY
struct counts {
    size_t live = 0;
    size_t peak = 0;
    size_t bytes = 0;
    size_t allocations = 0;
    size_t deallocations = 0;
};
inline counts *active;
template <typename T>
struct counter_allocator {
    using value_type = T;
    using is_always_equal = std::true_type;
    counter_allocator() = default;
    template <typename U>
    counter_allocator(const counter_allocator<U> &) noexcept {}
    T *allocate(size_t n) {
        auto *ptr = std::allocator<T>{}.allocate(n);
        active->live += n * sizeof(T);
        active->peak = std::max(active->peak, active->live);
        active->bytes += n * sizeof(T);
        ++active->allocations;
        return ptr;
    }
    void deallocate(T *ptr, size_t n) noexcept {
        active->live -= n * sizeof(T);
        ++active->deallocations;
        std::allocator<T>{}.deallocate(ptr, n);
    }
    template <typename U>
    bool operator==(const counter_allocator<U> &) const noexcept {
        return true;
    }
};
using allocator = counter_allocator<char>;
template <bool Packed, typename Tree>
void measure(const std::string &name, size_t n) {
    const auto input = input_values<typename Tree::mapped_type>(n);
    counts stats;
    active = &stats;
    size_t built;
    {
        auto tree = build<Packed, Tree>(input);
        if (tree.size() != n)
            std::abort();
        built = stats.live;
    }
    if (stats.live != 0 || stats.allocations != stats.deallocations)
        std::abort();
    std::cout << name << ',' << n << ',' << sizeof(Tree) << ',' << built << ',' << stats.peak << ','
              << stats.bytes << ',' << stats.allocations << ',' << stats.live << '\n';
    active = nullptr;
}
#else
using allocator = std::allocator<char>;
template <bool Packed, typename Tree>
void measure(const std::string &name, size_t n) {
    const auto input = input_values<typename Tree::mapped_type>(n);
    auto check = build<Packed, Tree>(input);
    if (check.size() != n)
        std::abort();
    size_t index = 0;
    for (const auto &value : check)
        if (value.first != index++ || value.second != typename Tree::mapped_type{})
            std::abort();
    benchmark::RegisterBenchmark((name + "/" + std::to_string(n)).c_str(),
                                 [input](benchmark::State &state) {
                                     for (auto _ : state) {
                                         auto tree = build<Packed, Tree>(input);
                                         benchmark::DoNotOptimize(tree);
                                         benchmark::ClobberMemory();
                                     }
                                     state.SetItemsProcessed(state.iterations() * input.size());
                                 })
        ->MinTime(0.02)
        ->Repetitions(5)
        ->UseRealTime();
}
#endif

template <typename Value>
void scenario(const std::string &label) {
    using legacy = wjr::btree_map<uint64_t, Value, std::less<>, allocator, 16>;
    using compact =
        wjr::experimental::compact_btree_map<uint64_t, Value, std::less<>, allocator, 16>;
    using pair = std::pair<const uint64_t, Value>;
    using value_allocator = typename std::allocator_traits<allocator>::template rebind_alloc<pair>;
    for (size_t n : {
#ifdef WJR_BULK_MEMORY
             0, 1, 2, 3, 16,
#endif
             17, 64, 4096}) {
        measure<false, legacy>(label + "/legacy_hint", n);
        measure<true, legacy>(label + "/legacy_packed", n);
        measure<false, compact>(label + "/compact_hint", n);
        measure<true, compact>(label + "/compact_packed", n);
        measure<false, std::map<uint64_t, Value, std::less<>, value_allocator>>(label + "/std_hint",
                                                                                n);
#ifdef WJR_BENCHMARK_ABSEIL
        measure<false, absl::btree_map<uint64_t, Value, std::less<>, value_allocator>>(
            label + "/absl_hint", n);
#endif
    }
}
} // namespace

int main(int argc, char **argv) {
#ifdef WJR_BULK_MEMORY
    (void)argc;
    (void)argv;
    std::cout
        << "backend,n,object_bytes,built_requested,peak_requested,total_requested,allocations,after_destroy\n";
#endif
    scenario<uint64_t>("u64");
    scenario<std::array<std::byte, 64>>("large");
#ifndef WJR_BULK_MEMORY
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;
    benchmark::AddCustomContext("source", WJR_BENCHMARK_SOURCE);
    benchmark::AddCustomContext("flags", WJR_BENCHMARK_FLAGS);
    benchmark::AddCustomContext(
        "contract",
        "ascending unique N16; input excluded; construct and destroy timed; std allocator; no counters");
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
#endif
}
