#include "btree_focused.hpp"
#include <wjr/container/experimental/compact_btree_map.hpp>

#ifdef WJR_INGEST_MEMORY
    #include "btree_probe_memory.hpp"
    #include <iostream>
#else
    #include <benchmark/benchmark.h>
#endif

namespace {
using namespace btree_focused;

#ifdef WJR_INGEST_MEMORY
template <typename T>
using allocator = btree_probe::allocator<T>;
#else
template <typename T>
using allocator = std::allocator<T>;
#endif

template <typename S, typename Tree>
void measure(const std::string &name, bool sorted) {
    workload<S> data(4096, S{});
    if (sorted)
        std::sort(data.build.begin(), data.build.end());
#ifdef WJR_INGEST_MEMORY
    btree_probe::counts stats;
    btree_probe::pass current(stats);
    validate<S, Tree>(data);
    stats = {};
    size_t built, updated, build_allocations, mixed_allocations;
    {
        Tree tree;
        build<S>(tree, data);
        built = stats.live;
        build_allocations = stats.allocations;
        mixed<S>(tree, data);
        updated = stats.live;
        mixed_allocations = stats.allocations - build_allocations;
    }
    if (stats.live != 0 || stats.allocations != stats.deallocations)
        std::abort();
    std::cout << name << ',' << sizeof(Tree) << ',' << built << ',' << updated << ',' << stats.peak
              << ',' << build_allocations << ',' << mixed_allocations << ',' << stats.deallocations
              << ',' << stats.live << '\n';
#else
    for (bool updates : {false, true}) {
        const auto label = name + (updates ? "/mixed_90_5_5" : "/build_destroy");
        benchmark::RegisterBenchmark(label.c_str(),
                                     [data, updates](benchmark::State &state) {
                                         validate<S, Tree>(data);
                                         for (auto _ : state) {
                                             if (updates) {
                                                 state.PauseTiming();
                                                 {
                                                     Tree tree;
                                                     build<S>(tree, data);
                                                     state.ResumeTiming();
                                                     benchmark::DoNotOptimize(mixed<S>(tree, data));
                                                     state.PauseTiming();
                                                 }
                                                 state.ResumeTiming();
                                             } else {
                                                 Tree tree;
                                                 build<S>(tree, data);
                                                 benchmark::DoNotOptimize(tree);
                                                 benchmark::ClobberMemory();
                                             }
                                         }
                                         state.SetItemsProcessed(
                                             state.iterations() *
                                             (updates ? data.mixed.size() : data.build.size()));
                                     })
            ->Repetitions(5)
            ->UseRealTime();
    }
#endif
}

template <typename Value>
void run_scenario(const std::string &label) {
    using S = btree_focused::scenario<uint64_t, Value>;
    using A = allocator<char>;
    for (bool sorted : {false, true}) {
        const auto prefix = label + (sorted ? "/sorted" : "/random");
        measure<S, wjr::btree_map<uint64_t, Value, std::less<>, A, 16>>(prefix + "/legacy", sorted);
        measure<S, wjr::experimental::compact_btree_map<uint64_t, Value, std::less<>, A, 16>>(
            prefix + "/compact", sorted);
        measure<S, typename std_backend<S, allocator>::type>(prefix + "/std", sorted);
#ifdef WJR_BENCHMARK_ABSEIL
        measure<S, typename absl_backend<S, allocator>::type>(prefix + "/absl", sorted);
#endif
    }
}
} // namespace

int main(int argc, char **argv) {
#ifdef WJR_INGEST_MEMORY
    (void)argc;
    (void)argv;
    std::cout
        << "case,object_bytes,build_requested,mixed_requested,peak_requested,build_allocations,mixed_allocations,total_deallocations,after_destroy\n";
#endif
    run_scenario<uint64_t>("u64");
    run_scenario<large_value>("large");
#ifndef WJR_INGEST_MEMORY
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;
    benchmark::AddCustomContext("source", WJR_BENCHMARK_SOURCE);
    benchmark::AddCustomContext("flags", WJR_BENCHMARK_FLAGS);
    benchmark::AddCustomContext(
        "contract",
        "N16;n4096;same shared seeded trace;sorted changes only build permutation;default allocator;build includes destroy;mixed excludes build/destroy");
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
#endif
}
