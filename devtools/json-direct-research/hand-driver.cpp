#define main paired_driver_main
#include "driver.cpp"
#undef main
#include "hand-reference.hpp"

double strong_measure(strong_reference::parser& parser, size_t iterations) {
    uint64_t sum = 0;
    auto begin = std::chrono::steady_clock::now();
    for (size_t i = 0; i != iterations; ++i) {
        std::atomic_signal_fence(std::memory_order_seq_cst);
        sum += parser.run();
    }
    auto end = std::chrono::steady_clock::now();
    sink = sum;
    return std::chrono::duration<double>(end - begin).count();
}
int main() {
    pin_cpu();
    auto fixtures = probe::fixtures();
#ifdef COUNT_ALLOCATIONS
    std::cout << "fixture,backend,calls,requested_bytes,peak_live_bytes,final_live_bytes\n";
#else
    std::cout << "fixture,bytes,input_hash,repeat,iterations,dom_seconds,direct_seconds,hand_seconds,direct_over_hand\n";
#endif
    for (auto& fixture : fixtures) {
        if (fixture.name.find("records-") != 0) continue;
        strong_reference::parser parser(fixture.input);
        if (parser.run() != fixture.expected) std::abort();
        wjr::json::ondemand_reader reader;
        reader.read(fixture.input);
#ifdef COUNT_ALLOCATIONS
        allocation::calls = allocation::bytes = allocation::live = allocation::peak = 0;
        allocation::enabled = true;
        sink = parser.run();
        allocation::enabled = false;
        if (allocation::live != 0) std::abort();
        std::cout << fixture.name << ",hand," << allocation::calls << ',' << allocation::bytes << ',' << allocation::peak << ',' << allocation::live << '\n';
#else
        const auto calibration = measure(fixture, false, 32, reader);
        const size_t iterations = std::clamp(static_cast<size_t>(0.10 * 32 / calibration), size_t{8}, size_t{1} << 20);
        for (int repeat = 0; repeat != 7; ++repeat) {
            double dom, direct, strong;
            if (repeat % 2 == 0) {
                dom = measure(fixture, false, iterations, reader);
                direct = measure(fixture, true, iterations, reader);
                strong = strong_measure(parser, iterations);
            } else {
                strong = strong_measure(parser, iterations);
                direct = measure(fixture, true, iterations, reader);
                dom = measure(fixture, false, iterations, reader);
            }
            std::cout << fixture.name << ',' << fixture.input.size() << ',' << probe::checksum(fixture.input) << ',' << repeat << ',' << iterations << ',' << dom << ',' << direct << ',' << strong << ',' << strong / direct << '\n';
        }
#endif
    }
}
