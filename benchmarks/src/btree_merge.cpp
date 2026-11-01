#include <wjr/container/btree_map.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <array>
#include <iostream>
#include <map>

#ifdef WJR_BENCHMARK_ABSEIL
    #include <absl/container/btree_map.h>
#endif
#ifndef WJR_MERGE_MEMORY
    #include <benchmark/benchmark.h>
#endif

namespace {
struct reference_less {
    bool operator()(uint64_t lhs, uint64_t rhs) const noexcept { return std::less<>{}(lhs, rhs); }
};

struct specification {
    const char *name;
    size_t source;
    size_t destination;
    uint64_t source_step;
    uint64_t destination_step;
    uint64_t source_base;
    uint64_t destination_base;
    size_t expected_conflicts;
};
constexpr specification cases[] = {{"empty", 4096, 0, 1, 1, 0, 0, 0},
                                   {"interleaved", 4096, 4096, 2, 2, 0, 1, 0},
                                   {"conflicting", 4096, 4096, 1, 1, 0, 0, 4096},
                                   {"sparse", 64, 4096, 64, 1, 32, 0, 64}};

template <typename Tree>
void fill(Tree &source, Tree &destination, specification spec) {
    for (size_t i = 0; i < spec.source; ++i)
        source.try_emplace(spec.source_base + i * spec.source_step);
    for (size_t i = 0; i < spec.destination; ++i)
        destination.try_emplace(spec.destination_base + i * spec.destination_step);
}

#ifdef WJR_MERGE_MEMORY
struct counts {
    size_t live = 0;
    size_t peak = 0;
    size_t allocations = 0;
    size_t deallocations = 0;
};
inline counts *active;
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
template <typename Tree>
void measure(const std::string &label, specification spec) {
    counts stats;
    active = &stats;
    size_t before, after, peak, allocations, deallocations;
    {
        Tree source, destination;
        fill(source, destination, spec);
        before = stats.live;
        stats.peak = stats.live;
        const auto old_allocations = stats.allocations;
        const auto old_deallocations = stats.deallocations;
        destination.merge(source);
        after = stats.live;
        peak = stats.peak;
        allocations = stats.allocations - old_allocations;
        deallocations = stats.deallocations - old_deallocations;
        if (source.size() != spec.expected_conflicts ||
            destination.size() != spec.source + spec.destination - spec.expected_conflicts)
            std::abort();
    }
    if (stats.live != 0 || stats.allocations != stats.deallocations)
        std::abort();
    std::cout << label << '/' << spec.name << ',' << before << ',' << after << ',' << peak << ','
              << allocations << ',' << deallocations << ',' << stats.live << '\n';
}
#else
template <typename T>
using allocator = std::allocator<T>;
template <typename Tree>
void measure(const std::string &label, specification spec) {
    Tree source, destination;
    fill(source, destination, spec);
    destination.merge(source);
    if (source.size() != spec.expected_conflicts ||
        destination.size() != spec.source + spec.destination - spec.expected_conflicts)
        std::abort();
    benchmark::RegisterBenchmark((label + '/' + spec.name).c_str(),
                                 [spec](benchmark::State &state) {
                                     for (auto _ : state) {
                                         state.PauseTiming();
                                         {
                                             Tree source, destination;
                                             fill(source, destination, spec);
                                             state.ResumeTiming();
                                             destination.merge(source);
                                             benchmark::DoNotOptimize(source);
                                             benchmark::DoNotOptimize(destination);
                                             benchmark::ClobberMemory();
                                             state.PauseTiming();
                                         }
                                         state.ResumeTiming();
                                     }
                                     state.SetItemsProcessed(state.iterations() * spec.source);
                                 })
        ->Iterations(32)
        ->Repetitions(5)
        ->UseRealTime();
}
#endif

template <typename Value>
void scenario(const std::string &label) {
    using A = allocator<char>;
    using pair = std::pair<const uint64_t, Value>;
    for (auto spec : cases) {
        measure<wjr::btree_map<uint64_t, Value, reference_less, A>>(label + "/legacy_reference",
                                                                    spec);
        measure<wjr::btree_map<uint64_t, Value, std::less<>, A>>(label + "/legacy_ordered", spec);
        measure<wjr::experimental::compact_btree_map<uint64_t, Value, reference_less, A>>(
            label + "/compact_reference", spec);
        measure<wjr::experimental::compact_btree_map<uint64_t, Value, std::less<>, A>>(
            label + "/compact_ordered", spec);
        measure<std::map<uint64_t, Value, std::less<>, allocator<pair>>>(label + "/std", spec);
#ifdef WJR_BENCHMARK_ABSEIL
        measure<absl::btree_map<uint64_t, Value, std::less<>, allocator<pair>>>(label + "/absl",
                                                                                spec);
#endif
    }
}
} // namespace

int main(int argc, char **argv) {
#ifdef WJR_MERGE_MEMORY
    (void)argc;
    (void)argv;
    std::cout
        << "case,before_requested,after_requested,peak_requested,merge_allocations,merge_deallocations,after_destroy\n";
#endif
    scenario<uint64_t>("u64");
    scenario<std::array<std::byte, 64>>("large");
#ifndef WJR_MERGE_MEMORY
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;
    benchmark::AddCustomContext("source", WJR_BENCHMARK_SOURCE);
    benchmark::AddCustomContext("flags", WJR_BENCHMARK_FLAGS);
    benchmark::AddCustomContext(
        "contract",
        "N16, ascending source; merge only timed; fill/destroy excluded; std allocator; fixed 32 iterations, 5 raw repetitions");
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
#endif
}
