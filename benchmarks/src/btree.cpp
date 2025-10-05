#include "detail.hpp"

#include <algorithm>
#include <map>
#include <random>
#include <unordered_map>

#include <wjr/container/btree_map.hpp>

using namespace wjr;

// ============================================================================
// Insert Benchmarks
// ============================================================================

// Sequential Insert - btree_map with different node sizes
template <size_t NodeSize>
static void BtreeMap_Insert_Sequential(benchmark::State &state) {
    const size_t n = state.range(0);

    for (auto _ : state) {
        state.PauseTiming();
        btree_map<int, int, std::less<>, std::allocator<char>, NodeSize> map;
        state.ResumeTiming();

        for (size_t i = 0; i < n; ++i) {
            map.emplace(i, i);
        }

        benchmark::DoNotOptimize(map);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Sequential Insert - std::map
static void StdMap_Insert_Sequential(benchmark::State &state) {
    const size_t n = state.range(0);

    for (auto _ : state) {
        state.PauseTiming();
        std::map<int, int> map;
        state.ResumeTiming();

        for (size_t i = 0; i < n; ++i) {
            map.emplace(i, i);
        }

        benchmark::DoNotOptimize(map);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Sequential Insert - std::unordered_map
static void UnorderedMap_Insert_Sequential(benchmark::State &state) {
    const size_t n = state.range(0);

    for (auto _ : state) {
        state.PauseTiming();
        std::unordered_map<int, int> map;
        map.reserve(n);
        state.ResumeTiming();

        for (size_t i = 0; i < n; ++i) {
            map.emplace(i, i);
        }

        benchmark::DoNotOptimize(map);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Random Insert - btree_map with different node sizes
template <size_t NodeSize>
static void BtreeMap_Insert_Random(benchmark::State &state) {
    const size_t n = state.range(0);

    // Pre-generate random data
    std::vector<int> data(n);
    for (size_t i = 0; i < n; ++i) {
        data[i] = i;
    }
    std::shuffle(data.begin(), data.end(), __mt_rand);

    for (auto _ : state) {
        state.PauseTiming();
        btree_map<int, int, std::less<>, std::allocator<char>, NodeSize> map;
        state.ResumeTiming();

        for (size_t i = 0; i < n; ++i) {
            map.emplace(data[i], data[i]);
        }

        benchmark::DoNotOptimize(map);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Random Insert - std::map
static void StdMap_Insert_Random(benchmark::State &state) {
    const size_t n = state.range(0);

    // Pre-generate random data
    std::vector<int> data(n);
    for (size_t i = 0; i < n; ++i) {
        data[i] = i;
    }
    std::shuffle(data.begin(), data.end(), __mt_rand);

    for (auto _ : state) {
        state.PauseTiming();
        std::map<int, int> map;
        state.ResumeTiming();

        for (size_t i = 0; i < n; ++i) {
            map.emplace(data[i], data[i]);
        }

        benchmark::DoNotOptimize(map);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Random Insert - std::unordered_map
static void UnorderedMap_Insert_Random(benchmark::State &state) {
    const size_t n = state.range(0);

    // Pre-generate random data
    std::vector<int> data(n);
    for (size_t i = 0; i < n; ++i) {
        data[i] = i;
    }
    std::shuffle(data.begin(), data.end(), __mt_rand);

    for (auto _ : state) {
        state.PauseTiming();
        std::unordered_map<int, int> map;
        map.reserve(n);
        state.ResumeTiming();

        for (size_t i = 0; i < n; ++i) {
            map.emplace(data[i], data[i]);
        }

        benchmark::DoNotOptimize(map);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// ============================================================================
// Lookup Benchmarks
// ============================================================================

// Sequential Lookup - btree_map with different node sizes
template <size_t NodeSize>
static void BtreeMap_Lookup_Sequential(benchmark::State &state) {
    const size_t n = state.range(0);

    btree_map<int, int, std::less<>, std::allocator<char>, NodeSize> map;
    for (size_t i = 0; i < n; ++i) {
        map.emplace(i, i);
    }

    for (auto _ : state) {
        for (size_t i = 0; i < n; ++i) {
            auto it = map.lower_bound(i);
            benchmark::DoNotOptimize(it);
        }
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Sequential Lookup - std::map
static void StdMap_Lookup_Sequential(benchmark::State &state) {
    const size_t n = state.range(0);

    std::map<int, int> map;
    for (size_t i = 0; i < n; ++i) {
        map.emplace(i, i);
    }

    for (auto _ : state) {
        for (size_t i = 0; i < n; ++i) {
            auto it = map.lower_bound(i);
            benchmark::DoNotOptimize(it);
        }
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Sequential Lookup - std::unordered_map
static void UnorderedMap_Lookup_Sequential(benchmark::State &state) {
    const size_t n = state.range(0);

    std::unordered_map<int, int> map;
    for (size_t i = 0; i < n; ++i) {
        map.emplace(i, i);
    }

    for (auto _ : state) {
        for (size_t i = 0; i < n; ++i) {
            auto it = map.find(i);
            benchmark::DoNotOptimize(it);
        }
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Random Lookup - btree_map with different node sizes
template <size_t NodeSize>
static void BtreeMap_Lookup_Random(benchmark::State &state) {
    const size_t n = state.range(0);

    btree_map<int, int, std::less<>, std::allocator<char>, NodeSize> map;
    for (size_t i = 0; i < n; ++i) {
        map.emplace(i, i);
    }

    // Pre-generate random lookup keys
    std::vector<int> keys(n);
    for (size_t i = 0; i < n; ++i) {
        keys[i] = i;
    }
    std::shuffle(keys.begin(), keys.end(), __mt_rand);

    for (auto _ : state) {
        for (size_t i = 0; i < n; ++i) {
            auto it = map.lower_bound(keys[i]);
            benchmark::DoNotOptimize(it);
        }
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Random Lookup - std::map
static void StdMap_Lookup_Random(benchmark::State &state) {
    const size_t n = state.range(0);

    std::map<int, int> map;
    for (size_t i = 0; i < n; ++i) {
        map.emplace(i, i);
    }

    // Pre-generate random lookup keys
    std::vector<int> keys(n);
    for (size_t i = 0; i < n; ++i) {
        keys[i] = i;
    }
    std::shuffle(keys.begin(), keys.end(), __mt_rand);

    for (auto _ : state) {
        for (size_t i = 0; i < n; ++i) {
            auto it = map.lower_bound(keys[i]);
            benchmark::DoNotOptimize(it);
        }
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Random Lookup - std::unordered_map
static void UnorderedMap_Lookup_Random(benchmark::State &state) {
    const size_t n = state.range(0);

    std::unordered_map<int, int> map;
    for (size_t i = 0; i < n; ++i) {
        map.emplace(i, i);
    }

    // Pre-generate random lookup keys
    std::vector<int> keys(n);
    for (size_t i = 0; i < n; ++i) {
        keys[i] = i;
    }
    std::shuffle(keys.begin(), keys.end(), __mt_rand);

    for (auto _ : state) {
        for (size_t i = 0; i < n; ++i) {
            auto it = map.find(keys[i]);
            benchmark::DoNotOptimize(it);
        }
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// ============================================================================
// Erase Benchmarks
// ============================================================================

// Sequential Erase - btree_map with different node sizes
template <size_t NodeSize>
static void BtreeMap_Erase_Sequential(benchmark::State &state) {
    const size_t n = state.range(0);

    for (auto _ : state) {
        state.PauseTiming();
        btree_map<int, int, std::less<>, std::allocator<char>, NodeSize> map;
        for (size_t i = 0; i < n; ++i) {
            map.emplace(i, i);
        }
        state.ResumeTiming();

        for (size_t i = 0; i < n; ++i) {
            auto it = map.lower_bound(i);
            if (it != map.end()) {
                map.erase(it);
            }
        }

        benchmark::DoNotOptimize(map);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Sequential Erase - std::map
static void StdMap_Erase_Sequential(benchmark::State &state) {
    const size_t n = state.range(0);

    for (auto _ : state) {
        state.PauseTiming();
        std::map<int, int> map;
        for (size_t i = 0; i < n; ++i) {
            map.emplace(i, i);
        }
        state.ResumeTiming();

        for (size_t i = 0; i < n; ++i) {
            auto it = map.lower_bound(i);
            if (it != map.end()) {
                map.erase(it);
            }
        }

        benchmark::DoNotOptimize(map);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Sequential Erase - std::unordered_map
static void UnorderedMap_Erase_Sequential(benchmark::State &state) {
    const size_t n = state.range(0);

    for (auto _ : state) {
        state.PauseTiming();
        std::unordered_map<int, int> map;
        for (size_t i = 0; i < n; ++i) {
            map.emplace(i, i);
        }
        state.ResumeTiming();

        for (size_t i = 0; i < n; ++i) {
            map.erase(i);
        }

        benchmark::DoNotOptimize(map);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Random Erase - btree_map with different node sizes
template <size_t NodeSize>
static void BtreeMap_Erase_Random(benchmark::State &state) {
    const size_t n = state.range(0);

    // Pre-generate random erase order
    std::vector<int> erase_order(n);
    for (size_t i = 0; i < n; ++i) {
        erase_order[i] = i;
    }
    std::shuffle(erase_order.begin(), erase_order.end(), __mt_rand);

    for (auto _ : state) {
        state.PauseTiming();
        btree_map<int, int, std::less<>, std::allocator<char>, NodeSize> map;
        for (size_t i = 0; i < n; ++i) {
            map.emplace(i, i);
        }
        state.ResumeTiming();

        for (size_t i = 0; i < n; ++i) {
            auto it = map.lower_bound(erase_order[i]);
            if (it != map.end() && it->first == erase_order[i]) {
                map.erase(it);
            }
        }

        benchmark::DoNotOptimize(map);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Random Erase - std::map
static void StdMap_Erase_Random(benchmark::State &state) {
    const size_t n = state.range(0);

    // Pre-generate random erase order
    std::vector<int> erase_order(n);
    for (size_t i = 0; i < n; ++i) {
        erase_order[i] = i;
    }
    std::shuffle(erase_order.begin(), erase_order.end(), __mt_rand);

    for (auto _ : state) {
        state.PauseTiming();
        std::map<int, int> map;
        for (size_t i = 0; i < n; ++i) {
            map.emplace(i, i);
        }
        state.ResumeTiming();

        for (size_t i = 0; i < n; ++i) {
            auto it = map.lower_bound(erase_order[i]);
            if (it != map.end() && it->first == erase_order[i]) {
                map.erase(it);
            }
        }

        benchmark::DoNotOptimize(map);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Random Erase - std::unordered_map
static void UnorderedMap_Erase_Random(benchmark::State &state) {
    const size_t n = state.range(0);

    // Pre-generate random erase order
    std::vector<int> erase_order(n);
    for (size_t i = 0; i < n; ++i) {
        erase_order[i] = i;
    }
    std::shuffle(erase_order.begin(), erase_order.end(), __mt_rand);

    for (auto _ : state) {
        state.PauseTiming();
        std::unordered_map<int, int> map;
        for (size_t i = 0; i < n; ++i) {
            map.emplace(i, i);
        }
        state.ResumeTiming();

        for (size_t i = 0; i < n; ++i) {
            map.erase(erase_order[i]);
        }

        benchmark::DoNotOptimize(map);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// ============================================================================
// Iteration Benchmarks
// ============================================================================

// Forward Iteration - btree_map with different node sizes
template <size_t NodeSize>
static void BtreeMap_Iteration_Forward(benchmark::State &state) {
    const size_t n = state.range(0);

    btree_map<int, int, std::less<>, std::allocator<char>, NodeSize> map;
    for (size_t i = 0; i < n; ++i) {
        map.emplace(i, i);
    }

    for (auto _ : state) {
        int sum = 0;
        for (const auto &[key, value] : map) {
            sum += value;
        }
        benchmark::DoNotOptimize(sum);
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Forward Iteration - std::map
static void StdMap_Iteration_Forward(benchmark::State &state) {
    const size_t n = state.range(0);

    std::map<int, int> map;
    for (size_t i = 0; i < n; ++i) {
        map.emplace(i, i);
    }

    for (auto _ : state) {
        int sum = 0;
        for (const auto &[key, value] : map) {
            sum += value;
        }
        benchmark::DoNotOptimize(sum);
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// Forward Iteration - std::unordered_map
static void UnorderedMap_Iteration_Forward(benchmark::State &state) {
    const size_t n = state.range(0);

    std::unordered_map<int, int> map;
    for (size_t i = 0; i < n; ++i) {
        map.emplace(i, i);
    }

    for (auto _ : state) {
        int sum = 0;
        for (const auto &[key, value] : map) {
            sum += value;
        }
        benchmark::DoNotOptimize(sum);
    }

    state.SetItemsProcessed(state.iterations() * n);
}

// ============================================================================
// Register Benchmarks
// ============================================================================

// Insert Sequential
BENCHMARK(BtreeMap_Insert_Sequential<8>)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);
BENCHMARK(BtreeMap_Insert_Sequential<16>)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);
BENCHMARK(StdMap_Insert_Sequential)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);
BENCHMARK(UnorderedMap_Insert_Sequential)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);

// Insert Random
BENCHMARK(BtreeMap_Insert_Random<8>)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);
BENCHMARK(BtreeMap_Insert_Random<16>)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);
BENCHMARK(StdMap_Insert_Random)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);
BENCHMARK(UnorderedMap_Insert_Random)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);

// Lookup Sequential
BENCHMARK(BtreeMap_Lookup_Sequential<8>)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);
BENCHMARK(BtreeMap_Lookup_Sequential<16>)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);
BENCHMARK(StdMap_Lookup_Sequential)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);
BENCHMARK(UnorderedMap_Lookup_Sequential)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);

// Lookup Random
BENCHMARK(BtreeMap_Lookup_Random<8>)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);
BENCHMARK(BtreeMap_Lookup_Random<16>)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);
BENCHMARK(StdMap_Lookup_Random)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);
BENCHMARK(UnorderedMap_Lookup_Random)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);

// Erase Sequential
BENCHMARK(BtreeMap_Erase_Sequential<8>)->Range(1 << 10, 1 << 16)->Unit(benchmark::kMicrosecond);
BENCHMARK(BtreeMap_Erase_Sequential<16>)->Range(1 << 10, 1 << 16)->Unit(benchmark::kMicrosecond);
BENCHMARK(StdMap_Erase_Sequential)->Range(1 << 10, 1 << 16)->Unit(benchmark::kMicrosecond);
BENCHMARK(UnorderedMap_Erase_Sequential)->Range(1 << 10, 1 << 16)->Unit(benchmark::kMicrosecond);

// Erase Random
BENCHMARK(BtreeMap_Erase_Random<8>)->Range(1 << 10, 1 << 16)->Unit(benchmark::kMicrosecond);
BENCHMARK(BtreeMap_Erase_Random<16>)->Range(1 << 10, 1 << 16)->Unit(benchmark::kMicrosecond);
BENCHMARK(StdMap_Erase_Random)->Range(1 << 10, 1 << 16)->Unit(benchmark::kMicrosecond);
BENCHMARK(UnorderedMap_Erase_Random)->Range(1 << 10, 1 << 16)->Unit(benchmark::kMicrosecond);

// Iteration
BENCHMARK(BtreeMap_Iteration_Forward<8>)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);
BENCHMARK(BtreeMap_Iteration_Forward<16>)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);
BENCHMARK(StdMap_Iteration_Forward)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);
BENCHMARK(UnorderedMap_Iteration_Forward)->Range(1 << 10, 1 << 18)->Unit(benchmark::kMicrosecond);
