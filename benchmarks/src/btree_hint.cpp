#include <algorithm>
#include <cstdint>
#include <map>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <vector>

#include <benchmark/benchmark.h>
#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

namespace {
enum class order { sorted, blocks, random };
constexpr uint32_t seed = 0x62b4e919;

template <bool Map>
auto make_input(size_t size, order distribution) {
    std::vector<uint64_t> keys(size);
    std::iota(keys.begin(), keys.end(), uint64_t(0));
    std::mt19937 random(seed);
    if (distribution == order::random)
        std::shuffle(keys.begin(), keys.end(), random);
    else if (distribution == order::blocks) {
        constexpr size_t block = 32;
        std::vector<size_t> permutation((size + block - 1) / block);
        std::iota(permutation.begin(), permutation.end(), size_t(0));
        std::shuffle(permutation.begin(), permutation.end(), random);
        keys.clear();
        for (size_t index : permutation)
            for (size_t key = index * block; key < std::min(size, (index + 1) * block); ++key)
                keys.push_back(key);
    }
    if constexpr (Map) {
        std::vector<std::pair<uint64_t, uint64_t>> values;
        values.reserve(size);
        for (uint64_t key : keys)
            values.emplace_back(key, key * 7 + 1);
        return values;
    } else
        return keys;
}

template <bool Range, class Tree, class Input>
void populate(Tree &tree, const Input &input) {
    if constexpr (Range)
        tree.insert(input.begin(), input.end());
    else
        for (const auto &value : input)
            tree.emplace(value);
}

template <bool Map, bool Range, class Tree>
void measure(benchmark::State &state, order distribution) {
    const auto input = make_input<Map>(static_cast<size_t>(state.range(0)), distribution);
    using reference_type = std::conditional_t<Map, std::map<uint64_t, uint64_t, std::less<>>,
                                              std::set<uint64_t, std::less<>>>;
    {
        const reference_type reference(input.begin(), input.end());
        Tree check;
        populate<Range>(check, input);
        if (check.size() != reference.size() ||
            !std::equal(check.begin(), check.end(), reference.begin())) {
            state.SkipWithError("hint range contents differ from std");
            return;
        }
    }
    for (auto _ : state) {
        Tree tree;
        populate<Range>(tree, input);
        benchmark::DoNotOptimize(tree.size());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}

template <bool Map, bool Range, class Tree>
void register_tree(const std::string &name) {
    for (auto [label, distribution] : {std::pair{"sorted", order::sorted},
                                       {"blocks32", order::blocks},
                                       {"random", order::random}}) {
        benchmark::RegisterBenchmark((name + '/' + label).c_str(),
                                     [distribution](benchmark::State &state) {
                                         measure<Map, Range, Tree>(state, distribution);
                                     })
            ->Arg(4096)
            ->Arg(32768)
            ->Repetitions(5)
            ->UseRealTime();
    }
}
} // namespace

int main(int argc, char **argv) {
    using set = wjr::btree_set<uint64_t, std::less<>>;
    using map = wjr::btree_map<uint64_t, uint64_t, std::less<>>;
    using compact = wjr::experimental::compact_btree_map<uint64_t, uint64_t, std::less<>>;
    register_tree<false, false, set>("set/wjr16/loop");
    register_tree<false, true, set>("set/wjr16/range");
    register_tree<true, false, map>("map/wjr16/loop");
    register_tree<true, true, map>("map/wjr16/range");
    register_tree<true, false, compact>("map/compact16/loop");
    register_tree<true, true, compact>("map/compact16/range");
    register_tree<false, true, std::set<uint64_t, std::less<>>>("set/std/range");
    register_tree<true, true, std::map<uint64_t, uint64_t, std::less<>>>("map/std/range");
    benchmark::AddCustomContext("wjr_source", WJR_BENCHMARK_SOURCE);
    benchmark::AddCustomContext("wjr_compiler", WJR_BENCHMARK_COMPILER);
    benchmark::AddCustomContext("wjr_flags", WJR_BENCHMARK_FLAGS);
    benchmark::AddCustomContext("wjr_cpu_target", WJR_BENCHMARK_CPU_TARGET);
    benchmark::AddCustomContext("trace_seed", std::to_string(seed));
    benchmark::AddCustomContext("allocator", "std::allocator; timing has no counters");
#ifdef _MSVC_STL_UPDATE
    benchmark::AddCustomContext("standard_library", "MSVC STL " +
                                                        std::to_string(_MSVC_STL_VERSION) + '.' +
                                                        std::to_string(_MSVC_STL_UPDATE));
#elif defined(__GLIBCXX__)
    benchmark::AddCustomContext("standard_library", "libstdc++ " + std::to_string(__GLIBCXX__));
#endif
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
}
