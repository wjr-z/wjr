#include <wjr/container/btree_map.hpp>
#include <wjr/container/experimental/btree_join_range.hpp>
#include <wjr/container/experimental/btree_repack.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <array>
#include <iostream>
#include <map>
#include <vector>

#ifdef WJR_BENCHMARK_ABSEIL
    #include <absl/container/btree_map.h>
#endif
#ifdef WJR_SHARD_MEMORY
    #include "btree_probe_memory.hpp"
#else
    #include <benchmark/benchmark.h>
#endif

namespace {
constexpr size_t shard_count = 8;
constexpr size_t shard_size = 512;
enum class operation { merge, append, batch, batch_repack, node_hint };

#ifdef WJR_SHARD_MEMORY
template <typename T>
using allocator = btree_probe::allocator<T>;
#else
template <typename T>
using allocator = std::allocator<T>;
#endif

template <typename Tree, operation Op>
struct fixture {
    using vector_allocator =
        typename std::allocator_traits<typename Tree::allocator_type>::template rebind_alloc<Tree>;
    std::vector<Tree, vector_allocator> sources{shard_count};
    Tree destination;

    fixture() {
        for (size_t s = 0; s < shard_count; ++s)
            for (size_t i = 0; i < shard_size; ++i)
                sources[s].try_emplace(s * 1024 + i);
    }
    void run() {
        if constexpr (Op == operation::batch || Op == operation::batch_repack) {
            wjr::experimental::append_btree_disjoint(destination, sources.begin(), sources.end());
            if constexpr (Op == operation::batch_repack)
                wjr::experimental::repack_compact_btree(destination);
        } else {
            for (auto &source : sources) {
                if constexpr (Op == operation::append) {
                    wjr::experimental::append_btree_disjoint(destination, source);
                } else {
                    if (destination.empty()) {
                        destination.swap(source);
                    } else if constexpr (Op == operation::merge) {
                        destination.merge(source);
                    } else {
                        while (!source.empty())
                            destination.insert(destination.end(), source.extract(source.begin()));
                    }
                }
            }
        }
    }
    void validate() const {
        if (destination.size() != shard_count * shard_size)
            std::abort();
        for (const auto &source : sources)
            if (!source.empty())
                std::abort();
        for (size_t s = 0; s < shard_count; ++s)
            for (size_t i = 0; i < shard_size; ++i) {
                auto found = destination.find(s * 1024 + i);
                if (found == destination.end() || found->second != typename Tree::mapped_type{})
                    std::abort();
            }
    }
};

template <typename Tree, operation Op>
void measure(const std::string &name) {
#ifdef WJR_SHARD_MEMORY
    btree_probe::counts stats;
    btree_probe::pass current(stats);
    size_t before, after, peak, allocations, deallocations;
    {
        fixture<Tree, Op> data;
        before = stats.live;
        stats.peak = stats.live;
        const auto old_allocations = stats.allocations;
        const auto old_deallocations = stats.deallocations;
        data.run();
        data.validate();
        after = stats.live;
        peak = stats.peak;
        allocations = stats.allocations - old_allocations;
        deallocations = stats.deallocations - old_deallocations;
    }
    if (stats.live != 0 || stats.allocations != stats.deallocations)
        std::abort();
    std::cout << name << ',' << before << ',' << after << ',' << peak << ',' << allocations << ','
              << deallocations << ',' << stats.live << '\n';
#else
    {
        fixture<Tree, Op> data;
        data.run();
        data.validate();
    }
    benchmark::RegisterBenchmark(name.c_str(),
                                 [](benchmark::State &state) {
                                     for (auto _ : state) {
                                         state.PauseTiming();
                                         {
                                             fixture<Tree, Op> data;
                                             state.ResumeTiming();
                                             data.run();
                                             benchmark::DoNotOptimize(data);
                                             benchmark::ClobberMemory();
                                             state.PauseTiming();
                                         }
                                         state.ResumeTiming();
                                     }
                                     state.SetItemsProcessed(state.iterations() * shard_count *
                                                             shard_size);
                                 })
        ->Iterations(32)
        ->Repetitions(5)
        ->UseRealTime();
#endif
}

template <typename Value>
void scenario(const std::string &label) {
    using A = allocator<char>;
    using legacy = wjr::btree_map<uint64_t, Value, std::less<>, A, 16>;
    using compact = wjr::experimental::compact_btree_map<uint64_t, Value, std::less<>, A, 16>;
    measure<legacy, operation::merge>(label + "/legacy_merge");
    measure<legacy, operation::append>(label + "/legacy_append");
    measure<legacy, operation::batch>(label + "/legacy_batch");
    measure<compact, operation::merge>(label + "/compact_merge");
    measure<compact, operation::append>(label + "/compact_append");
    measure<compact, operation::batch>(label + "/compact_batch");
    measure<compact, operation::batch_repack>(label + "/compact_batch_repack");
    using pair = std::pair<const uint64_t, Value>;
    using std_tree = std::map<uint64_t, Value, std::less<>, allocator<pair>>;
    measure<std_tree, operation::merge>(label + "/std_swap_merge");
    measure<std_tree, operation::node_hint>(label + "/std_swap_node_hint");
#ifdef WJR_BENCHMARK_ABSEIL
    using absl_tree = absl::btree_map<uint64_t, Value, std::less<>, allocator<pair>>;
    measure<absl_tree, operation::merge>(label + "/absl_swap_merge");
    measure<absl_tree, operation::node_hint>(label + "/absl_swap_node_hint");
#endif
}
} // namespace

int main(int argc, char **argv) {
#ifdef WJR_SHARD_MEMORY
    (void)argc;
    (void)argv;
    std::cout
        << "case,before_requested,after_requested,peak_requested,allocations,deallocations,after_destroy\n";
#endif
    scenario<uint64_t>("u64");
    scenario<std::array<std::byte, 64>>("large");
#ifndef WJR_SHARD_MEMORY
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;
    benchmark::AddCustomContext("source", WJR_BENCHMARK_SOURCE);
    benchmark::AddCustomContext("flags", WJR_BENCHMARK_FLAGS);
    benchmark::AddCustomContext(
        "contract",
        "N16; eight512-key shards; reduction only; fixture construct/destroy excluded; std allocator; first-empty swap for std/Abseil;32iterations*5raw");
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
#endif
}
