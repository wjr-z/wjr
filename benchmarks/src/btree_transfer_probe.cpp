#include "btree_focused.hpp"
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <iostream>
#include <memory_resource>
#include <optional>

#ifndef WJR_TRANSFER_MEMORY
    #include <benchmark/benchmark.h>
#endif

namespace {
struct identity_resource : std::pmr::memory_resource {
    void *do_allocate(size_t n, size_t alignment) override {
        return std::pmr::new_delete_resource()->allocate(n, alignment);
    }
    void do_deallocate(void *p, size_t n, size_t alignment) override {
        std::pmr::new_delete_resource()->deallocate(p, n, alignment);
    }
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
        return this == &other;
    }
};

struct counts {
    size_t live = 0, peak = 0;
};

struct counting_resource : identity_resource {
    explicit counting_resource(counts &all) : all(all) {}
    counts &all;
    size_t live = 0, allocations = 0, deallocations = 0;
    void *do_allocate(size_t n, size_t alignment) override {
        auto *p = identity_resource::do_allocate(n, alignment);
        all.live += n;
        all.peak = std::max(all.peak, all.live);
        live += n;
        ++allocations;
        return p;
    }
    void do_deallocate(void *p, size_t n, size_t alignment) override {
        all.live -= n;
        live -= n;
        ++deallocations;
        identity_resource::do_deallocate(p, n, alignment);
    }
};

template <typename S, typename Tree>
void validate_transfer(const btree_focused::workload<S> &data) {
    identity_resource first, second;
    Tree source(typename Tree::key_compare{}, typename Tree::allocator_type(&first));
    btree_focused::build<S>(source, data);
    Tree target(std::move(source), typename Tree::allocator_type(&second));
    typename btree_focused::std_backend<S, std::allocator>::type expected;
    btree_focused::build<S>(expected, data);
    if (!source.empty() ||
        !std::equal(target.begin(), target.end(), expected.begin(), expected.end()))
        throw std::runtime_error("transfer oracle mismatch");
}

template <typename S, typename Tree>
void register_transfer(const std::string &name, S spec) {
    const btree_focused::workload<S> data(4096, spec);
    validate_transfer<S, Tree>(data);
#ifdef WJR_TRANSFER_MEMORY
    counts all;
    counting_resource first(all), second(all);
    size_t original = 0, peak = 0, after = 0, source_after = 0;
    {
        Tree source(typename Tree::key_compare{}, typename Tree::allocator_type(&first));
        btree_focused::build<S>(source, data);
        all.peak = original = all.live;
        Tree target(std::move(source), typename Tree::allocator_type(&second));
        peak = all.peak;
        after = second.live;
        source_after = first.live;
    }
    if (all.live || first.allocations != first.deallocations ||
        second.allocations != second.deallocations)
        throw std::runtime_error("unbalanced transfer resources");
    std::cout << name << ',' << original << ',' << peak << ',' << source_after << ',' << after
              << ',' << first.allocations << ',' << second.allocations << ',' << all.live << '\n';
#else
    benchmark::RegisterBenchmark(name.c_str(),
                                 [=](benchmark::State &state) {
                                     for (auto _ : state) {
                                         state.PauseTiming();
                                         {
                                             identity_resource first, second;
                                             Tree source(typename Tree::key_compare{},
                                                         typename Tree::allocator_type(&first));
                                             btree_focused::build<S>(source, data);
                                             std::optional<Tree> target;
                                             state.ResumeTiming();
                                             target.emplace(std::move(source),
                                                            typename Tree::allocator_type(&second));
                                             benchmark::DoNotOptimize(*target);
                                             benchmark::ClobberMemory();
                                             state.PauseTiming();
                                         }
                                         state.ResumeTiming();
                                     }
                                     state.SetItemsProcessed(state.iterations() * 4096);
                                 })
        ->Repetitions(5)
        ->UseRealTime();
#endif
}

template <typename V>
void scenario(const std::string &name) {
    using A = std::pmr::polymorphic_allocator<char>;
    using R = wjr::btree_detail::transfer_reference_less;
    using S = btree_focused::scenario<uint64_t, V>;
    register_transfer<S, wjr::btree_map<uint64_t, V, R, A, 16>>(name + "/legacy/reference", S{});
    register_transfer<S, wjr::btree_map<uint64_t, V, std::less<>, A, 16>>(name + "/legacy/stream",
                                                                          S{});
    register_transfer<S, wjr::experimental::compact_btree_map<uint64_t, V, R, A, 16>>(
        name + "/compact/reference", S{});
    register_transfer<S, wjr::experimental::compact_btree_map<uint64_t, V, std::less<>, A, 16>>(
        name + "/compact/stream", S{});
}
} // namespace

int main(int argc, char **argv) {
#ifdef WJR_TRANSFER_MEMORY
    (void)argc;
    (void)argv;
    std::cout << "case,source_before,transfer_peak,source_after,dest_after,source_allocations,"
                 "dest_allocations,after_destroy\n";
#endif
    scenario<uint64_t>("u64_map");
    scenario<btree_focused::large_value>("large_map");
#ifndef WJR_TRANSFER_MEMORY
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;
    benchmark::AddCustomContext("wjr_source", WJR_BENCHMARK_SOURCE);
    benchmark::AddCustomContext("wjr_flags", WJR_BENCHMARK_FLAGS);
    benchmark::AddCustomContext("standard_library", btree_focused::standard_library());
    benchmark::AddCustomContext("trace_seed", std::to_string(btree_focused::seed));
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
#endif
}
