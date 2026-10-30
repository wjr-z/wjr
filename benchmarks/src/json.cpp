#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/json/lexer.hpp>

namespace {

std::string make_records(size_t count, unsigned layout) {
    std::mt19937 random(73);
    std::string input = "[";
    for (size_t i = 0; i < count; ++i) {
        if (i != 0) {
            input += ',';
        }
        const bool pretty = layout == 1 || (layout == 2 && (random() & 1));
        input +=
            pretty
                ? "\n  { \"id\" : 12345, \"text\" : \"hello world\\n\\\"\\\\ text\", \"items\" : "
                  "[ 1, 2, 3 ] }"
                : R"({"id":12345,"text":"hello world\n\"\\ text","items":[1,2,3]})";
    }
    input += ']';
    return input;
}

void BM_json_minify(benchmark::State &state) {
    const auto input =
        make_records(static_cast<size_t>(state.range(0)), static_cast<unsigned>(state.range(1)));
    std::vector<char> output(input.size());
    for (auto _ : state) {
        benchmark::DoNotOptimize(input.data());
        auto end = wjr::json::minify(output.data(), input.data(), input.data() + input.size());
        benchmark::DoNotOptimize(end);
        benchmark::ClobberMemory();
    }
    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(input.size()));
}

void minify_arguments(benchmark::internal::Benchmark *benchmark) {
    for (int64_t count : {1, 4, 16, 1024}) {
        for (int64_t layout = 0; layout != 3; ++layout) {
            benchmark->Args({count, layout});
        }
    }
    benchmark->ArgNames({"records", "layout"});
}

BENCHMARK(BM_json_minify)->Apply(minify_arguments);

void BM_json_minify_shuffled(benchmark::State &state) {
    std::vector<std::string> inputs;
    size_t bytes = 0;
    size_t max_size = 0;
    for (unsigned layout = 0; layout != 3; ++layout) {
        for (size_t count = 1; count < 128; count += 7) {
            inputs.push_back(make_records(count, layout));
            bytes += inputs.back().size();
            max_size = std::max(max_size, inputs.back().size());
        }
    }
    std::shuffle(inputs.begin(), inputs.end(), std::mt19937(0));
    std::vector<char> output(max_size);
    for (auto _ : state) {
        for (const auto &input : inputs) {
            benchmark::DoNotOptimize(input.data());
            auto end = wjr::json::minify(output.data(), input.data(), input.data() + input.size());
            benchmark::DoNotOptimize(end);
            benchmark::ClobberMemory();
        }
    }
    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(bytes));
}

BENCHMARK(BM_json_minify_shuffled);

} // namespace
