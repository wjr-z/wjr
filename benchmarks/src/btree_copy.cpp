#include "btree_focused.hpp"
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <iostream>
#include <numeric>

#ifdef WJR_COPY_MEMORY
    #include "btree_probe_memory.hpp"
#else
    #include <benchmark/benchmark.h>
#endif

namespace {
constexpr size_t count = 4096;
constexpr size_t base_leaves = 455;
enum class operation {
    match0,
    match25,
    match50,
    match75,
    match100,
    late50,
    random_shape,
    size_mismatch,
    empty,
    leaf_to_inner
};

#ifdef WJR_COPY_MEMORY
template <typename T>
using allocator = btree_probe::allocator<T>;
#else
template <typename T>
using allocator = std::allocator<T>;
#endif

template <typename Tree>
struct fixture {
    operation op;
    static Tree build() {
        Tree tree;
        for (size_t i = 0; i < count; ++i)
            tree.try_emplace(i * 4);
        return tree;
    }
    Tree first = build();
    Tree second = first;
    Tree target = first;
    bool next_second = true;

    explicit fixture(operation requested) : op(requested) {
        if (op == operation::size_mismatch) {
            second.erase(4 * (count - 1));
        } else if (op == operation::random_shape) {
            second.clear();
            std::vector<uint64_t> keys(count);
            std::iota(keys.begin(), keys.end(), uint64_t{0});
            std::mt19937_64 random(btree_focused::seed);
            std::shuffle(keys.begin(), keys.end(), random);
            for (auto key : keys)
                second.try_emplace(key * 4);
        } else if (op != operation::empty && op != operation::leaf_to_inner) {
            const int percent = op == operation::match0                               ? 0
                                : op == operation::match25                            ? 25
                                : op == operation::match50 || op == operation::late50 ? 50
                                : op == operation::match75                            ? 75
                                                                                      : 100;
            const size_t changed = (base_leaves * (100 - percent) + 50) / 100;
            const size_t start = op == operation::late50 ? base_leaves - changed : 0;
            const size_t paired = changed % 2 ? changed - 3 : changed;
            for (size_t i = 0; i < paired; i += 2) {
                second.erase(4 * ((start + i) * 9 + 1));
                second.try_emplace(4 * ((start + i + 1) * 9 + 1) + 1);
            }
            if (paired != changed) {
                const size_t leaf = start + paired;
                second.erase(4 * (leaf * 9 + 1));
                second.erase(4 * ((leaf + 1) * 9 + 1));
                second.try_emplace(4 * ((leaf + 2) * 9 + 1) + 1);
                second.try_emplace(4 * ((leaf + 2) * 9 + 1) + 2);
            }
        }
    }
    bool prepare_needed() const { return op == operation::empty || op == operation::leaf_to_inner; }
    void prepare() {
        if (prepare_needed()) {
            target.clear();
            if (op == operation::leaf_to_inner)
                for (size_t i = 0; i < 16; ++i)
                    target.try_emplace(100000 + i);
        }
    }
    size_t run() {
        const auto &source = next_second ? second : first;
        next_second = !next_second;
        target = source;
        return source.size();
    }
    void validate() {
        prepare();
        run();
        if (!std::equal(target.begin(), target.end(), second.begin(), second.end()))
            std::abort();
        prepare();
        run();
        if (!std::equal(target.begin(), target.end(), first.begin(), first.end()))
            std::abort();
    }
};

template <typename Tree>
void measure(const std::string &name, operation op) {
#ifdef WJR_COPY_MEMORY
    btree_probe::counts stats;
    btree_probe::pass active(stats);
    {
        fixture<Tree> data(op);
        data.validate();
        for (const char *direction : {"to_variant", "to_base"}) {
            data.prepare();
            const auto before = stats.peak = stats.live;
            const auto old_allocations = stats.allocations, old_deallocations = stats.deallocations;
            const auto copied = data.run();
            std::cout << name << '/' << direction << ',' << copied << ',' << before << ','
                      << stats.live << ',' << stats.peak << ','
                      << stats.allocations - old_allocations << ','
                      << stats.deallocations - old_deallocations << '\n';
        }
    }
    if (stats.live || stats.allocations != stats.deallocations)
        std::abort();
#else
    {
        fixture<Tree> data(op);
        data.validate();
    }
    benchmark::RegisterBenchmark(name.c_str(),
                                 [op](benchmark::State &state) {
                                     fixture<Tree> data(op);
                                     size_t copied = 0;
                                     for (auto _ : state) {
                                         if (data.prepare_needed()) {
                                             state.PauseTiming();
                                             data.prepare();
                                             state.ResumeTiming();
                                         }
                                         copied += data.run();
                                         benchmark::DoNotOptimize(data.target);
                                         benchmark::ClobberMemory();
                                     }
                                     state.SetItemsProcessed(copied);
                                 })
        ->UseRealTime()
        ->Repetitions(5);
#endif
}

template <typename Tree>
void register_tree(const std::string &name) {
    const std::pair<const char *, operation> cases[] = {
        {"match0", operation::match0},
        {"match25", operation::match25},
        {"match50_early", operation::match50},
        {"match75", operation::match75},
        {"match100", operation::match100},
        {"match50_late", operation::late50},
        {"random_shape", operation::random_shape},
        {"size_mismatch", operation::size_mismatch},
        {"empty", operation::empty},
        {"leaf_to_inner", operation::leaf_to_inner}};
    for (const auto &[label, op] : cases)
        measure<Tree>(name + '/' + label + "/4096", op);
}
template <typename Mapped>
void register_value(const std::string &name) {
    using value = std::pair<const uint64_t, Mapped>;
    register_tree<wjr::btree_map<uint64_t, Mapped, std::less<>, allocator<char>, 16>>(name +
                                                                                      "/legacy");
    register_tree<
        wjr::experimental::compact_btree_map<uint64_t, Mapped, std::less<>, allocator<char>, 16>>(
        name + "/compact");
    register_tree<std::map<uint64_t, Mapped, std::less<>, allocator<value>>>(name + "/std");
#ifdef WJR_BENCHMARK_ABSEIL
    register_tree<absl::btree_map<uint64_t, Mapped, std::less<>, allocator<value>>>(name + "/absl");
#endif
}
} // namespace

int main(int argc, char **argv) {
#ifdef WJR_COPY_MEMORY
    std::cout
        << "case,copied,before_requested,after_requested,peak_requested,allocations,deallocations\n";
#else
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;
    benchmark::AddCustomContext("source", WJR_BENCHMARK_SOURCE);
    benchmark::AddCustomContext("flags", WJR_BENCHMARK_FLAGS);
    benchmark::AddCustomContext("compiler", WJR_BENCHMARK_COMPILER);
    benchmark::AddCustomContext("stl", btree_focused::standard_library());
    benchmark::AddCustomContext("trace_seed", std::to_string(btree_focused::seed));
#endif
    register_value<uint64_t>("u64");
    register_value<btree_focused::large_value>("large");
#ifndef WJR_COPY_MEMORY
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
#endif
}
