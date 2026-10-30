#include "btree_focused.hpp"

#include <benchmark/benchmark.h>

namespace btree_focused {

enum class measurement { build_destroy, lower_bound, lower_bound_view, steady_mixed, ordered_scan };

template <typename S, typename Tree>
void measure(benchmark::State &state, S spec, measurement kind) {
    const workload<S> data(static_cast<size_t>(state.range(0)), spec);
    validate<S, Tree>(data);
    if (kind == measurement::build_destroy) {
        for (auto _ : state) {
            Tree tree;
            build<S>(tree, data);
            benchmark::DoNotOptimize(tree);
            benchmark::ClobberMemory();
        }
    } else if (kind == measurement::steady_mixed) {
        for (auto _ : state) {
            state.PauseTiming();
            {
                Tree tree;
                build<S>(tree, data);
                state.ResumeTiming();
                benchmark::DoNotOptimize(mixed<S>(tree, data));
                state.PauseTiming();
            }
            state.ResumeTiming();
        }
    } else {
        Tree tree;
        build<S>(tree, data);
        for (auto _ : state) {
            if (kind == measurement::ordered_scan) {
                benchmark::DoNotOptimize(scan<S>(tree));
            } else {
                uint64_t result = 0;
                for (auto id : data.queries) {
                    if constexpr (std::is_same_v<typename S::key_type, std::string>) {
                        if (kind == measurement::lower_bound_view) {
                            result += lookup<S>(tree, std::string_view(data.keys[id]));
                        } else {
                            result += lookup<S>(tree, data.keys[id]);
                        }
                    } else {
                        result += lookup<S>(tree, data.keys[id]);
                    }
                }
                benchmark::DoNotOptimize(result);
            }
        }
    }
    const auto operations =
        kind == measurement::steady_mixed ? data.mixed.size()
        : kind == measurement::lower_bound || kind == measurement::lower_bound_view
            ? data.queries.size()
            : data.build.size();
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(operations));
}

template <typename S, typename Tree>
void register_tree(const std::string &name, S spec) {
    for (auto [operation, kind] : {std::pair{"build_destroy", measurement::build_destroy},
                                   {"lower_bound_50", measurement::lower_bound},
                                   {"mixed_90_5_5", measurement::steady_mixed},
                                   {"scan", measurement::ordered_scan}}) {
        benchmark::RegisterBenchmark(
            (name + "/" + operation).c_str(),
            [=](benchmark::State &state) { measure<S, Tree>(state, spec, kind); })
            ->Arg(64)
            ->Arg(4096)
            ->Arg(262144)
            ->Repetitions(5)
            ->UseRealTime();
    }
    if constexpr (std::is_same_v<typename S::key_type, std::string>) {
        benchmark::RegisterBenchmark((name + "/lower_bound_view_50").c_str(),
                                     [=](benchmark::State &state) {
                                         measure<S, Tree>(state, spec,
                                                          measurement::lower_bound_view);
                                     })
            ->Arg(64)
            ->Arg(4096)
            ->Arg(262144)
            ->Repetitions(5)
            ->UseRealTime();
    }
}

template <typename S>
void register_scenario(const std::string &name, S spec) {
    register_tree<S, typename wjr_backend<S, std::allocator<char>, 16>::type>(name + "/wjr16",
                                                                              spec);
    register_tree<S, typename wjr_backend<S, std::allocator<char>, 32>::type>(name + "/wjr32",
                                                                              spec);
    register_tree<S, typename std_backend<S, std::allocator>::type>(name + "/std", spec);
#ifdef WJR_BENCHMARK_COMPACT
    if constexpr (std::is_same_v<typename S::key_type, uint64_t> &&
                  (std::is_same_v<typename S::mapped_type, uint64_t> ||
                   std::is_same_v<typename S::mapped_type, large_value>)) {
        register_tree<S,
                      wjr::experimental::compact_btree_map<uint64_t, typename S::mapped_type,
                                                           std::less<>, std::allocator<char>, 16>>(
            name + "/compact16", spec);
    }
#endif
#ifdef WJR_BENCHMARK_ABSEIL
    register_tree<S, typename absl_backend<S, std::allocator>::type>(name + "/absl", spec);
#endif
}

void register_all() {
    register_scenario("u64_set", scenario<uint64_t, void>{});
    register_scenario("u64_map", scenario<uint64_t, uint64_t>{});
    register_scenario("short_string_map", scenario<std::string, uint64_t>{12});
    register_scenario("long_string_map", scenario<std::string, uint64_t>{96});
    register_scenario("large_map", scenario<uint64_t, large_value>{});
}

} // namespace btree_focused

int main(int argc, char **argv) {
    btree_focused::register_all();
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv)) {
        return 1;
    }
    benchmark::AddCustomContext("wjr_compiler", WJR_BENCHMARK_COMPILER);
    benchmark::AddCustomContext("wjr_source", WJR_BENCHMARK_SOURCE);
    benchmark::AddCustomContext("wjr_flags", WJR_BENCHMARK_FLAGS);
    benchmark::AddCustomContext("wjr_cpu_target", WJR_BENCHMARK_CPU_TARGET);
    benchmark::AddCustomContext("trace_seed", std::to_string(btree_focused::seed));
    benchmark::AddCustomContext("trace_blocks", std::to_string(btree_focused::trace_blocks));
    benchmark::AddCustomContext("allocator", "std::allocator; no counters");
    benchmark::AddCustomContext("string_sizeof", std::to_string(sizeof(std::string)));
    benchmark::AddCustomContext("standard_library", btree_focused::standard_library());
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
}
