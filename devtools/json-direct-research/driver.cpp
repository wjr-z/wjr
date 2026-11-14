#include "fixtures.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <new>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <sched.h>
#endif

namespace allocation {
bool enabled = false;
size_t calls = 0;
size_t bytes = 0;
size_t live = 0;
size_t peak = 0;
}

#ifdef COUNT_ALLOCATIONS
namespace {
struct header {
    void *allocation;
    size_t size;
    bool counted;
};
void *allocate(size_t size, size_t alignment) {
    alignment = std::max(alignment, alignof(std::max_align_t));
    if (size > SIZE_MAX - alignment - sizeof(header)) throw std::bad_alloc{};
    void *raw = std::malloc(std::max(size_t{1}, size) + alignment + sizeof(header));
    if (!raw) throw std::bad_alloc{};
    const auto address = (reinterpret_cast<uintptr_t>(raw) + sizeof(header) + alignment - 1) &
                         ~(static_cast<uintptr_t>(alignment) - 1);
    auto *metadata = reinterpret_cast<header *>(address) - 1;
    *metadata = {raw, size, allocation::enabled};
    if (metadata->counted) {
        ++allocation::calls;
        allocation::bytes += size;
        allocation::live += size;
        allocation::peak = std::max(allocation::peak, allocation::live);
    }
    return reinterpret_cast<void *>(address);
}
void release(void *pointer) noexcept {
    if (!pointer) return;
    auto *metadata = static_cast<header *>(pointer) - 1;
    if (metadata->counted) allocation::live -= metadata->size;
    std::free(metadata->allocation);
}
}
void *operator new(size_t size) { return allocate(size, alignof(std::max_align_t)); }
void *operator new[](size_t size) { return allocate(size, alignof(std::max_align_t)); }
void *operator new(size_t size, std::align_val_t alignment) {
    return allocate(size, static_cast<size_t>(alignment));
}
void *operator new[](size_t size, std::align_val_t alignment) {
    return allocate(size, static_cast<size_t>(alignment));
}
void operator delete(void *pointer) noexcept { release(pointer); }
void operator delete[](void *pointer) noexcept { release(pointer); }
void operator delete(void *pointer, size_t) noexcept { release(pointer); }
void operator delete[](void *pointer, size_t) noexcept { release(pointer); }
void operator delete(void *pointer, std::align_val_t) noexcept { release(pointer); }
void operator delete[](void *pointer, std::align_val_t) noexcept { release(pointer); }
void operator delete(void *pointer, size_t, std::align_val_t) noexcept { release(pointer); }
void operator delete[](void *pointer, size_t, std::align_val_t) noexcept { release(pointer); }
#endif

volatile uint64_t sink = 0;

void pin_cpu() {
#ifdef _WIN32
    if (!SetThreadAffinityMask(GetCurrentThread(), DWORD_PTR{1} << 2)) std::abort();
    std::cerr << "cpu=2\n";
#else
    cpu_set_t allowed;
    CPU_ZERO(&allowed);
    if (sched_getaffinity(0, sizeof(allowed), &allowed) != 0) std::abort();
    int selected = -1;
    for (int cpu = 0; cpu != CPU_SETSIZE; ++cpu) {
        if (CPU_ISSET(cpu, &allowed)) {
            selected = cpu;
            break;
        }
    }
    if (selected == -1) std::abort();
    cpu_set_t pinned;
    CPU_ZERO(&pinned);
    CPU_SET(selected, &pinned);
    if (sched_setaffinity(0, sizeof(pinned), &pinned) != 0) std::abort();
    std::cerr << "cpu=" << selected << '\n';
#endif
}

double measure(const probe::fixture &fixture, bool direct, size_t iterations,
               wjr::json::ondemand_reader &reader) {
    const auto parse = direct ? fixture.direct : fixture.reference;
    uint64_t sum = 0;
    const auto begin = std::chrono::steady_clock::now();
    for (size_t iteration = 0; iteration != iterations; ++iteration) {
        std::atomic_signal_fence(std::memory_order_seq_cst);
        reader.read(fixture.input);
        sum += parse(reader);
    }
    const auto end = std::chrono::steady_clock::now();
    sink = sum;
    return std::chrono::duration<double>(end - begin).count();
}

int main(int argc, char **argv) {
    pin_cpu();
    auto fixtures = probe::fixtures();
    const char* requested_fixture=std::getenv("JSON_FIXTURE");
    const char* requested_repeats=std::getenv("JSON_REPEATS");
    const int repeats=requested_repeats ? std::atoi(requested_repeats) : 7;
    if(repeats<1) std::abort();
    if (argc == 2) {
        std::filesystem::create_directories(argv[1]);
        for (const auto &fixture : fixtures) {
            std::ofstream file(std::filesystem::path(argv[1]) / (fixture.name + ".json"),
                               std::ios::binary);
            file.write(fixture.input.data(), static_cast<std::streamsize>(fixture.input.size()));
        }
    }
#ifdef COUNT_ALLOCATIONS
    std::cout << "fixture,backend,calls,requested_bytes,peak_live_bytes,final_live_bytes\n";
    for (const auto &fixture : fixtures) {
        if(requested_fixture && fixture.name!=requested_fixture) continue;
        wjr::json::ondemand_reader reader;
        reader.read(fixture.input);
        for (const bool direct : {false, true}) {
            allocation::calls = allocation::bytes = allocation::live = allocation::peak = 0;
            allocation::enabled = true;
            reader.read(fixture.input);
            sink = direct ? fixture.direct(reader) : fixture.reference(reader);
            allocation::enabled = false;
            if (allocation::live != 0) std::abort();
            std::cout << fixture.name << ',' << (direct ? "direct" : "dom") << ','
                      << allocation::calls << ',' << allocation::bytes << ',' << allocation::peak
                      << ',' << allocation::live << '\n';
        }
    }
#else
    std::cout << "fixture,bytes,input_hash,repeat,iterations,dom_seconds,direct_seconds,ratio\n";
    for (const auto &fixture : fixtures) {
        if(requested_fixture && fixture.name!=requested_fixture) continue;
        wjr::json::ondemand_reader reader;
        reader.read(fixture.input);
        if (fixture.direct(reader) != fixture.expected) std::abort();
        reader.read(fixture.input);
        if (fixture.reference(reader) != fixture.expected) std::abort();
        const auto calibration = measure(fixture, false, 32, reader);
        const size_t iterations = std::clamp(static_cast<size_t>(0.10 * 32 / calibration),
                                             size_t{8}, size_t{1} << 20);
        for (int repeat = 0; repeat != repeats; ++repeat) {
            double dom, direct;
            if (repeat % 2 == 0) {
                dom = measure(fixture, false, iterations, reader);
                direct = measure(fixture, true, iterations, reader);
            } else {
                direct = measure(fixture, true, iterations, reader);
                dom = measure(fixture, false, iterations, reader);
            }
            std::cout << fixture.name << ',' << fixture.input.size() << ','
                      << probe::checksum(fixture.input) << ',' << repeat << ',' << iterations
                      << ',' << dom << ',' << direct << ',' << dom / direct << '\n';
        }
    }
#endif
    return 0;
}
