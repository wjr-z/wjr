#include <absl/container/btree_map.h>
#include <benchmark/benchmark.h>
#include <wjr/container/btree_map.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <array>
#include <cstdint>
#include <map>

template <typename Tree>
void fill(Tree &tree) {
    for (uint64_t i = 0; i != 4096; ++i)
        tree.try_emplace(i);
}

template <typename Tree>
void validate_erase_result(uint64_t first, uint64_t last) {
    Tree tree;
    fill(tree);
    tree.erase(tree.lower_bound(first), tree.lower_bound(last));
    if (tree.size() != 4096 - (last - first))
        throw std::runtime_error("range size mismatch");
    uint64_t expected = 0;
    for (const auto &value : tree) {
        if (expected == first)
            expected = last;
        if (value.first != expected++ || value.second != typename Tree::mapped_type{})
            throw std::runtime_error("range content mismatch");
    }
}

template <typename Tree>
void register_erase(const std::string &name) {
    for (auto [first, last] : {std::pair<uint64_t, uint64_t>{1024, 3072}, {1, 3071}}) {
        validate_erase_result<Tree>(first, last);
        const auto label = first == 1 ? "/front_partial" : "/middle";
        benchmark::RegisterBenchmark(
            (name + label).c_str(),
            [=](benchmark::State &state) {
                for (auto _ : state) {
                    state.PauseTiming();
                    {
                        Tree tree;
                        fill(tree);
                        state.ResumeTiming();
                        tree.erase(tree.lower_bound(first), tree.lower_bound(last));
                        benchmark::DoNotOptimize(tree);
                        benchmark::ClobberMemory();
                        state.PauseTiming();
                    }
                    state.ResumeTiming();
                }
                state.SetItemsProcessed(state.iterations() * (last - first));
            })
            ->Iterations(64)
            ->Repetitions(5)
            ->UseRealTime();
    }
}

template <typename Value>
void scenario(const std::string &name) {
    using A = std::allocator<char>;
    using R = wjr::btree_detail::range_erase_reference_less;
    register_erase<wjr::btree_map<uint64_t, Value, R, A, 16>>(name + "/legacy_reference");
    register_erase<wjr::btree_map<uint64_t, Value, std::less<>, A, 16>>(name + "/legacy_batch");
    register_erase<wjr::experimental::compact_btree_map<uint64_t, Value, R, A, 16>>(
        name + "/compact_reference");
    register_erase<wjr::experimental::compact_btree_map<uint64_t, Value, std::less<>, A, 16>>(
        name + "/compact_batch");
    register_erase<std::map<uint64_t, Value, std::less<>>>(name + "/std");
    register_erase<absl::btree_map<uint64_t, Value, std::less<>>>(name + "/absl");
}

int main(int argc, char **argv) {
    scenario<uint64_t>("u64");
    scenario<std::array<std::byte, 64>>("large");
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;
    benchmark::AddCustomContext("source", WJR_BENCHMARK_SOURCE);
    benchmark::AddCustomContext("flags", WJR_BENCHMARK_FLAGS);
    benchmark::AddCustomContext(
        "contract",
        "N4096 ascending build excluded; lower bounds+erase timed; 64 iterations/rep; no counters");
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
}
