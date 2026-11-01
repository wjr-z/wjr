#include "btree_focused.hpp"
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <iostream>

#ifdef WJR_COPY_MEMORY
    #include "btree_probe_memory.hpp"
#else
    #include <benchmark/benchmark.h>
#endif

namespace {
constexpr size_t count = 4096;
enum class operation { matched, shape_mismatch, size_mismatch };

#ifdef WJR_COPY_MEMORY
template <typename T>
using allocator = btree_probe::allocator<T>;
#else
template <typename T>
using allocator = std::allocator<T>;
#endif

template <typename Tree, operation Op>
struct fixture {
    static Tree build() {
        Tree tree;
        for (size_t i = 0; i < count; ++i)
            tree.try_emplace(i);
        return tree;
    }
    Tree first = build();
    Tree second = first;
    Tree target = first;
    bool next_second = true;

    fixture() {
        if constexpr (Op == operation::shape_mismatch) {
            second.erase(count - 16);
            second.try_emplace(count);
        } else if constexpr (Op == operation::size_mismatch) {
            second.erase(count - 1);
        }
    }
    size_t run() {
        if constexpr (Op == operation::matched) {
            target = first;
            return first.size();
        } else {
            const auto &source = next_second ? second : first;
            next_second = !next_second;
            target = source;
            return source.size();
        }
    }
    void validate() {
        run();
        if (!std::equal(target.begin(), target.end(), second.begin(), second.end()))
            std::abort();
        run();
        if (!std::equal(target.begin(), target.end(), first.begin(), first.end()))
            std::abort();
    }
};

template <typename Tree, operation Op>
void measure(const std::string &name) {
#ifdef WJR_COPY_MEMORY
    btree_probe::counts stats;
    btree_probe::pass active(stats);
    size_t before, after, peak, allocations, deallocations;
    {
        fixture<Tree, Op> data;
        data.validate();
        before = stats.peak = stats.live;
        const auto old_allocations = stats.allocations;
        const auto old_deallocations = stats.deallocations;
        data.run();
        after = stats.live;
        peak = stats.peak;
        allocations = stats.allocations - old_allocations;
        deallocations = stats.deallocations - old_deallocations;
    }
    if (stats.live || stats.allocations != stats.deallocations)
        std::abort();
    std::cout << name << ',' << before << ',' << after << ',' << peak << ',' << allocations << ','
              << deallocations << ',' << stats.live << '\n';
#else
    {
        fixture<Tree, Op> data;
        data.validate();
    }
    benchmark::RegisterBenchmark(name.c_str(),
                                 [](benchmark::State &state) {
                                     fixture<Tree, Op> data;
                                     size_t copied = 0;
                                     for (auto _ : state) {
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
    measure<Tree, operation::matched>(name + "/matched/4096");
    measure<Tree, operation::shape_mismatch>(name + "/shape_mismatch/4096");
    measure<Tree, operation::size_mismatch>(name + "/size_mismatch/4096");
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
        << "case,before_requested,after_requested,peak_requested,allocations,deallocations,after_destroy\n";
#else
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;
    benchmark::AddCustomContext("source", WJR_BENCHMARK_SOURCE);
    benchmark::AddCustomContext("flags", WJR_BENCHMARK_FLAGS);
    benchmark::AddCustomContext("compiler", WJR_BENCHMARK_COMPILER);
    benchmark::AddCustomContext("stl", btree_focused::standard_library());
#endif
    register_value<uint64_t>("u64");
    register_value<btree_focused::large_value>("large");
#ifndef WJR_COPY_MEMORY
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
#endif
}
