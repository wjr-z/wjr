#include <wjr/container/btree_map.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

#ifdef WJR_BENCHMARK_JOIN
    #include <wjr/container/experimental/btree_join.hpp>
#endif

#include <array>
#include <iostream>
#include <map>

#ifdef WJR_BENCHMARK_ABSEIL
    #include <absl/container/btree_map.h>
#endif
#ifndef WJR_MERGE_MEMORY
    #include <benchmark/benchmark.h>
#else
    #include "btree_probe_memory.hpp"
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
#ifdef WJR_BENCHMARK_JOIN
constexpr specification cases[] = {{"leaf9", 9, 9, 1, 1, 8192, 0, 0},
                                   {"inner150", 150, 150, 1, 1, 8192, 0, 0},
                                   {"large4096", 4096, 4096, 1, 1, 8192, 0, 0},
                                   {"unequal", 64, 4096, 1, 1, 8192, 0, 0}};

template <typename Tree>
struct append_tree : Tree {
    using Tree::Tree;
    void merge(append_tree &source) { wjr::experimental::append_btree_disjoint(*this, source); }
};

template <typename Tree>
struct node_hint_tree : Tree {
    using Tree::Tree;
    void merge(node_hint_tree &source) {
        while (!source.empty())
            this->insert(this->end(), source.extract(source.begin()));
    }
};
#else
constexpr specification cases[] = {{"empty", 4096, 0, 1, 1, 0, 0, 0},
                                   {"interleaved", 4096, 4096, 2, 2, 0, 1, 0},
                                   {"conflicting", 4096, 4096, 1, 1, 0, 0, 4096},
                                   {"sparse", 64, 4096, 64, 1, 32, 0, 64}};
#endif

template <typename Tree>
void fill(Tree &source, Tree &destination, specification spec) {
    for (size_t i = 0; i < spec.source; ++i)
        source.try_emplace(spec.source_base + i * spec.source_step);
    for (size_t i = 0; i < spec.destination; ++i)
        destination.try_emplace(spec.destination_base + i * spec.destination_step);
}

#ifdef WJR_BENCHMARK_JOIN
template <typename Tree>
void validate(const Tree &destination, specification spec) {
    for (size_t i = 0; i < spec.source; ++i) {
        auto found = destination.find(spec.source_base + i * spec.source_step);
        if (found == destination.end() || found->second != typename Tree::mapped_type{})
            std::abort();
    }
    for (size_t i = 0; i < spec.destination; ++i) {
        auto found = destination.find(spec.destination_base + i * spec.destination_step);
        if (found == destination.end() || found->second != typename Tree::mapped_type{})
            std::abort();
    }
}
#endif

#ifdef WJR_MERGE_MEMORY
using btree_probe::counts;
template <typename T>
using allocator = btree_probe::allocator<T>;
template <typename Tree>
void measure(const std::string &label, specification spec) {
    counts stats;
    btree_probe::pass current(stats);
    size_t before, after, peak, allocations, deallocations;
    {
        Tree source, destination;
        fill(source, destination, spec);
        before = stats.live;
        stats.peak = stats.live;
        const auto old_allocations = stats.allocations;
        const auto old_deallocations = stats.deallocations;
        destination.merge(source);
    #ifdef WJR_BENCHMARK_JOIN
        validate(destination, spec);
    #endif
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
    #ifdef WJR_BENCHMARK_JOIN
    validate(destination, spec);
    #endif
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
#ifdef WJR_BENCHMARK_JOIN
        using legacy = wjr::btree_map<uint64_t, Value, std::less<>, A>;
        using compact = wjr::experimental::compact_btree_map<uint64_t, Value, std::less<>, A>;
        measure<legacy>(label + "/legacy_merge", spec);
        measure<append_tree<legacy>>(label + "/legacy_append", spec);
        measure<compact>(label + "/compact_merge", spec);
        measure<append_tree<compact>>(label + "/compact_append", spec);
        using std_tree = std::map<uint64_t, Value, std::less<>, allocator<pair>>;
        measure<std_tree>(label + "/std_merge", spec);
        measure<node_hint_tree<std_tree>>(label + "/std_node_hint", spec);
    #ifdef WJR_BENCHMARK_ABSEIL
        using absl_tree = absl::btree_map<uint64_t, Value, std::less<>, allocator<pair>>;
        measure<absl_tree>(label + "/absl_merge", spec);
        measure<node_hint_tree<absl_tree>>(label + "/absl_node_hint", spec);
    #endif
#else
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
