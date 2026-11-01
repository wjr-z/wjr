#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>

namespace {

struct allocation_ledger {
    bool active = false;
    int64_t allocations = 0;
    int64_t total = 0;
    int64_t live = 0;
    int64_t peak = 0;
    int64_t expected_allocations = 0;
    int64_t expected_total = 0;
    int64_t expected_peak = 0;
} ledger;

template <typename T>
struct tracked_allocator {
    using value_type = T;

    T *allocate(size_t count) {
        auto *ptr = std::allocator<T>{}.allocate(count);
        if (ledger.active) {
            ++ledger.allocations;
            ledger.total += static_cast<int64_t>(count * sizeof(T));
            ledger.live += static_cast<int64_t>(count * sizeof(T));
            ledger.peak = std::max(ledger.peak, ledger.live);
        }
        return ptr;
    }

    void deallocate(T *ptr, size_t count) noexcept {
        if (ledger.active) {
            ledger.live -= static_cast<int64_t>(count * sizeof(T));
        }
        std::allocator<T>{}.deallocate(ptr, count);
    }
};

class recording_manager final : public benchmark::MemoryManager {
public:
    void Start() override {
        ledger = {};
        ledger.active = true;
    }

    void Stop(Result &result) override {
        if (ledger.allocations != ledger.expected_allocations ||
            ledger.total != ledger.expected_total || ledger.peak != ledger.expected_peak ||
            ledger.live != 0) {
            std::abort();
        }
        ledger.active = false;
        result.num_allocs = ledger.allocations;
        result.max_bytes_used = ledger.peak;
        result.total_allocated_bytes = ledger.total;
        result.net_heap_growth = ledger.live;
        ++verified_passes;
    }

    unsigned verified_passes = 0;
};

void tracked_vector(benchmark::State &state) {
    for (auto _ : state) {
        std::vector<uint64_t, tracked_allocator<uint64_t>> data(64, 7);
        benchmark::DoNotOptimize(data.data());
        benchmark::ClobberMemory();
    }
    if (ledger.active) {
        ledger.expected_allocations = state.iterations();
        ledger.expected_total = 512 * state.iterations();
        ledger.expected_peak = 512;
    }
}

void no_tracked_allocation(benchmark::State &state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(uint64_t{7});
    }
}

} // namespace

int main(int argc, char **argv) {
    recording_manager manager;
    benchmark::RegisterMemoryManager(&manager);
    benchmark::RegisterBenchmark("tracked_vector", tracked_vector)->Iterations(16)->Repetitions(5);
    benchmark::RegisterBenchmark("no_tracked_allocation", no_tracked_allocation)
        ->Iterations(16)
        ->Repetitions(5);
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    benchmark::RegisterMemoryManager(nullptr);
    return manager.verified_passes == 10 ? 0 : 1;
}
