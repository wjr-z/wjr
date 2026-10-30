#include "btree_focused.hpp"

#include <iostream>
#include <memory_resource>
#include <optional>

#ifndef WJR_POOL_MEMORY
    #include <benchmark/benchmark.h>
#endif

namespace btree_focused {

inline constexpr size_t pool_n = 4096;
inline constexpr size_t warm_cycles = 8;

template <typename S, typename Tree>
void validate_resource(const workload<S> &data) {
    std::pmr::unsynchronized_pool_resource pool;
    typename std_backend<S, std::allocator>::type expected;
    build<S>(expected, data);
    const auto expected_digest = mixed<S>(expected, data);
    for (size_t cycle = 0; cycle != warm_cycles; ++cycle) {
        Tree actual(std::less<>{}, typename Tree::allocator_type(&pool));
        build<S>(actual, data);
        if (mixed<S>(actual, data) != expected_digest || actual.size() != expected.size() ||
            !std::equal(actual.begin(), actual.end(), expected.begin(), expected.end())) {
            throw std::runtime_error("pooled final contents differ from std");
        }
        actual.clear();
        if (!actual.empty()) {
            throw std::runtime_error("clear failed");
        }
    }
}

template <typename S, typename Tree>
uint64_t cycle(std::pmr::memory_resource *resource, const workload<S> &data) {
    Tree tree(std::less<>{}, typename Tree::allocator_type(resource));
    build<S>(tree, data);
    const auto result = mixed<S>(tree, data);
    tree.clear();
    return result;
}

#ifndef WJR_POOL_MEMORY
template <typename S, typename Tree>
void measure_pool(benchmark::State &state, S spec, bool pooled, bool warm) {
    const workload<S> data(pool_n, spec);
    validate_resource<S, Tree>(data);
    const size_t cycles = warm ? warm_cycles : 1;
    if (warm && pooled) {
        std::pmr::unsynchronized_pool_resource pool;
        benchmark::DoNotOptimize(cycle<S, Tree>(&pool, data));
        for (auto _ : state) {
            for (size_t i = 0; i != cycles; ++i) {
                benchmark::DoNotOptimize(cycle<S, Tree>(&pool, data));
                benchmark::ClobberMemory();
            }
        }
    } else if (pooled) {
        for (auto _ : state) {
            std::pmr::unsynchronized_pool_resource pool;
            benchmark::DoNotOptimize(cycle<S, Tree>(&pool, data));
            benchmark::ClobberMemory();
        }
    } else {
        for (auto _ : state) {
            for (size_t i = 0; i != cycles; ++i) {
                benchmark::DoNotOptimize(cycle<S, Tree>(std::pmr::new_delete_resource(), data));
                benchmark::ClobberMemory();
            }
        }
    }
    state.SetItemsProcessed(state.iterations() * cycles * (data.build.size() + data.mixed.size()));
}

template <typename S, typename Tree>
void register_pool(const std::string &name, S spec) {
    for (bool pooled : {false, true}) {
        for (bool warm : {false, true}) {
            benchmark::RegisterBenchmark(
                (name + (pooled ? "/pool" : "/direct") + (warm ? "/warm8" : "/cold")).c_str(),
                [=](benchmark::State &state) { measure_pool<S, Tree>(state, spec, pooled, warm); })
                ->Repetitions(5)
                ->UseRealTime();
        }
    }
}
#else
struct stats {
    size_t live = 0, peak = 0, total = 0, allocations = 0, deallocations = 0;
};

class observed_resource final : public std::pmr::memory_resource {
public:
    explicit observed_resource(std::pmr::memory_resource *upstream) : upstream(upstream) {}
    stats counts;

private:
    void *do_allocate(size_t bytes, size_t alignment) override {
        void *ptr = upstream->allocate(bytes, alignment);
        counts.live += bytes;
        counts.peak = std::max(counts.peak, counts.live);
        counts.total += bytes;
        ++counts.allocations;
        return ptr;
    }
    void do_deallocate(void *ptr, size_t bytes, size_t alignment) override {
        counts.live -= bytes;
        ++counts.deallocations;
        upstream->deallocate(ptr, bytes, alignment);
    }
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
        return this == &other;
    }
    std::pmr::memory_resource *upstream;
};

template <typename S, typename Tree>
void memory_pool(const std::string &name, S spec) {
    const workload<S> data(pool_n, spec);
    validate_resource<S, Tree>(data);
    for (bool pooled : {false, true}) {
        for (bool warm : {false, true}) {
            observed_resource upstream(std::pmr::new_delete_resource());
            const size_t cycles = warm ? warm_cycles : 1;
            std::optional<std::pmr::unsynchronized_pool_resource> pool;
            if (pooled) {
                pool.emplace(&upstream);
            }
            observed_resource client(pooled ? static_cast<std::pmr::memory_resource *>(&*pool)
                                            : &upstream);
            size_t built = 0, mixed_live = 0, cleared = 0, retained = 0;
            for (size_t i = 0; i != cycles; ++i) {
                {
                    Tree tree(std::less<>{}, typename Tree::allocator_type(&client));
                    build<S>(tree, data);
                    built = client.counts.live;
                    mixed<S>(tree, data);
                    mixed_live = client.counts.live;
                    tree.clear();
                    cleared = client.counts.live;
                }
                if (client.counts.live != 0) {
                    throw std::runtime_error("client allocation leaked");
                }
                retained = upstream.counts.live;
            }
            if (pool) {
                pool->release();
            }
            if (upstream.counts.live != 0 ||
                client.counts.allocations != client.counts.deallocations ||
                upstream.counts.allocations != upstream.counts.deallocations) {
                throw std::runtime_error("resource release did not balance");
            }
            std::cout << name << ',' << (pooled ? "pool" : "direct") << ','
                      << (warm ? "warm8" : "cold") << ',' << cycles << ',' << sizeof(Tree) << ','
                      << built << ',' << mixed_live << ',' << cleared << ',' << client.counts.peak
                      << ',' << client.counts.total << ',' << client.counts.allocations << ','
                      << upstream.counts.peak << ',' << upstream.counts.total << ','
                      << upstream.counts.allocations << ',' << retained << ','
                      << upstream.counts.live << '\n';
        }
    }
}
#endif

template <typename S>
void pool_scenario(const std::string &name, S spec) {
    using wjr_tree = typename wjr_backend<S, std::pmr::polymorphic_allocator<char>, 16>::type;
    using std_tree = typename std_backend<S, std::pmr::polymorphic_allocator>::type;
    using absl_tree = typename absl_backend<S, std::pmr::polymorphic_allocator>::type;
#ifdef WJR_POOL_MEMORY
    memory_pool<S, wjr_tree>(name + "/wjr16", spec);
    memory_pool<S, std_tree>(name + "/std", spec);
    memory_pool<S, absl_tree>(name + "/absl", spec);
#else
    register_pool<S, wjr_tree>(name + "/wjr16", spec);
    register_pool<S, std_tree>(name + "/std", spec);
    register_pool<S, absl_tree>(name + "/absl", spec);
#endif
}

} // namespace btree_focused

int main(int argc, char **argv) {
    using namespace btree_focused;
#ifdef WJR_POOL_MEMORY
    (void)argc;
    (void)argv;
    std::cout << "case,resource,lifecycle,cycles,tree_size,build_live,mixed_live,clear_live,"
                 "client_peak,client_total,client_allocations,upstream_peak,upstream_total,"
                 "upstream_allocations,retained_after_destroy,after_release\n";
#endif
    pool_scenario("u64_map", scenario<uint64_t, uint64_t>{});
    pool_scenario("large_map", scenario<uint64_t, large_value>{});
#ifndef WJR_POOL_MEMORY
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv)) {
        return 1;
    }
    benchmark::AddCustomContext("wjr_source", WJR_BENCHMARK_SOURCE);
    benchmark::AddCustomContext("wjr_flags", WJR_BENCHMARK_FLAGS);
    benchmark::AddCustomContext("standard_library", standard_library());
    benchmark::AddCustomContext("trace_seed", std::to_string(seed));
    benchmark::AddCustomContext("n", std::to_string(pool_n));
    benchmark::AddCustomContext("pool_options", "standard-library defaults");
    benchmark::AddCustomContext("allocator", "polymorphic_allocator; no counting resources");
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
#endif
}
