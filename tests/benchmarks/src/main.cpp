#include "detail.hpp"

#include <wjr/container/generic/bplus_tree.hpp>
#include <wjr/container/generic/vector.hpp>
#include <wjr/memory/memory_pool.hpp>

#include <numeric>

using namespace wjr;

struct xxx {
    bool operator()(int a, int b) const noexcept { return a < b; }
};

struct solution0 {
    using Traits = bplus_tree_traits<int, void, xxx, 16, true>;
    using Tree = basic_bplus_tree<Traits, memory_pool<int>>;

    solution0(int _n) : n(_n), vec(n), pos(n) { std::iota(pos.begin(), pos.end(), 0); }
    solution0(solution0 &&) = default;
    ~solution0() = default;

    void init() {
        for (auto it = tree.begin(); it != tree.end(); it = tree.erase(it))
            ;

        std::generate_n(std::begin(vec), n,
                        []() { return std::make_pair(mt_rand(), mt_rand()); });
        // std::sort(vec.begin(), vec.end(),
        //   [](auto x, auto y) { return x.first < y.first; });
        std::shuffle(pos.begin(), pos.end(), __mt_rand);
        for (auto it : vec) {
            tree.__emplace_multi(it.first);
        }
    }

    WJR_NOINLINE void run() {
        for (auto i : vec) {
            auto iter = tree.lower_bound(i.first);
            if (iter != tree.end()) {
                benchmark::DoNotOptimize(*iter);
            }
        }
    }

    int n;
    vector<std::pair<int, int>> vec;
    vector<int> pos;

    Tree tree;
};

struct solution1 {
    using Tree = std::multiset<int, xxx, memory_pool<int>>;

    solution1(int _n) : n(_n), vec(n), pos(n) { std::iota(pos.begin(), pos.end(), 0); }
    solution1(solution1 &&) = default;
    ~solution1() = default;

    void init() {
        tree.clear();
        std::generate_n(std::begin(vec), n,
                        []() { return std::make_pair(mt_rand(), mt_rand()); });
        // std::sort(vec.begin(), vec.end(),
        //   [](auto x, auto y) { return x.first < y.first; });
        std::shuffle(pos.begin(), pos.end(), __mt_rand);
        for (auto it : vec) {
            tree.emplace(it.first);
        }
        for (auto it : vec) {
            tree.emplace(it.first);
        }
    }

    WJR_NOINLINE void run() {
        for (auto i : vec) {
            auto iter = tree.lower_bound(i.first);
            if (iter != tree.end()) {
                benchmark::DoNotOptimize(*iter);
            }
        }
    }

    int n;
    vector<std::pair<int, int>> vec;
    vector<int> pos;

    Tree tree;
    Tree tree2;
};

static void map0(benchmark::State &state) {
    const auto n = state.range(0);
    const int N = 1e4 / n;

    vector<solution0> vec;
    for (int i = 0; i < N; ++i)
        vec.emplace_back(n);

    random_run(
        state,
        [&]() {
            for (auto &sol : vec)
                sol.init();
        },
        [&]() {
            for (auto &sol : vec)
                sol.run();
        },
        1);

    state.SetItemsProcessed(N * state.iterations());
}

static void map1(benchmark::State &state) {
    const auto n = state.range(0);
    const int N = 1e4 / n;

    vector<solution1> vec;
    for (int i = 0; i < N; ++i)
        vec.emplace_back(n);

    random_run(
        state,
        [&]() {
            for (auto &sol : vec)
                sol.init();
        },
        [&]() {
            for (auto &sol : vec)
                sol.run();
        },
        1);

    state.SetItemsProcessed(N * state.iterations());
}

BENCHMARK(map0)->RangeMultiplier(2)->Range(1, 1024);
BENCHMARK(map1)->RangeMultiplier(2)->Range(1, 1024);

BENCHMARK_MAIN();