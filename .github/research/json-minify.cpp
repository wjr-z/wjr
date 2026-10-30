#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <random>
#include <string>
#include <vector>
#include <wjr/json/lexer.hpp>
#ifdef WITH_SIMDJSON
    #include "simdjson.hpp"
#endif

namespace wjr::json {
char *reference_minify(char *, const char *, const char *) noexcept;
}

using kernel = char *(*)(char *, const char *, const char *) noexcept;

std::string scalar(const std::string &input) {
    std::string output;
    bool quoted = false;
    bool escaped = false;
    for (char ch : input) {
        if (quoted || (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n')) {
            output += ch;
        }
        if (quoted && escaped) {
            escaped = false;
        } else if (quoted && ch == '\\') {
            escaped = true;
        } else if (ch == '"') {
            quoted = !quoted;
        }
    }
    return output;
}

struct dataset {
    std::string name;
    std::vector<std::string> inputs;
};

std::string records(unsigned mode, size_t count) {
    std::mt19937 rng(73);
    std::string out = "[";
    for (size_t i = 0; i < count; ++i) {
        if (i)
            out += ',';
        const bool pretty = mode == 1 || (mode == 2 && (rng() & 1));
        out +=
            pretty
                ? "\n  { \"id\" : 12345, \"text\" : \"hello world\\n\\\"\\\\ text\", \"items\" : [ 1, 2, 3 ] }"
                : R"({"id":12345,"text":"hello world\n\"\\ text","items":[1,2,3]})";
    }
    return out + ']';
}

bool verify(const std::string &input, bool valid = false) {
    const auto n = input.size();
    auto source = std::make_unique<char[]>(std::max(n, size_t{1}));
    auto output = std::make_unique<char[]>(std::max(n, size_t{1}));
    std::copy(input.begin(), input.end(), source.get());
    const auto old_end = wjr::json::reference_minify(output.get(), source.get(), source.get() + n);
    const auto expected = valid ? scalar(input) : std::string(output.get(), old_end - output.get());
    auto end = wjr::json::minify(output.get(), source.get(), source.get() + n);
    if (end - output.get() != expected.size() ||
        std::string(output.get(), expected.size()) != expected)
        return false;
    end = wjr::json::minify(source.get(), source.get(), source.get() + n);
    return end - source.get() == expected.size() &&
           std::string(source.get(), expected.size()) == expected;
}

volatile size_t sink = 0;

void measure(const dataset &data, kernel fn, const char *name) {
    std::vector<char> output;
    size_t bytes = 0;
    for (const auto &in : data.inputs) {
        output.resize(std::max(output.size(), in.size() + 64));
        bytes += in.size();
    }
    const size_t iterations = std::max<size_t>(100, (256 * 1024 * 1024) / bytes);
    size_t result = 0;
    const auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        for (const auto &in : data.inputs) {
            const auto end = fn(output.data(), in.data(), in.data() + in.size());
            result += end - output.data();
        }
    }
    const auto seconds =
        std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    sink = result;
    std::cout << data.name << ',' << name << ',' << bytes * iterations / seconds / 1e9 << '\n';
}

#ifdef WITH_SIMDJSON
char *mature(char *dst, const char *first, const char *last) noexcept {
    size_t n;
    if (simdjson::minify(first, last - first, dst, n))
        std::abort();
    return dst + n;
}
#endif

int main(int argc, char **argv) {
    if (argc == 1) {
        std::mt19937 rng(42);
        const char alphabet[] = "abc \\t\n\r\"\\n\\u1234";
        for (size_t n = 0; n < 1024; ++n) {
            for (size_t j = 0; j < 16; ++j) {
                std::string in(n, 'a');
                for (auto &ch : in)
                    ch = alphabet[rng() % (sizeof(alphabet) - 1)];
                if (!verify(in))
                    return 1;
            }
        }
        for (unsigned mode = 0; mode < 3; ++mode)
            if (!verify(records(mode, 100), true))
                return 2;
        return 0;
    }
#ifdef WITH_SIMDJSON
    #if WJR_HAS_SIMD(AVX2)
    simdjson::get_active_implementation() = simdjson::get_available_implementations()["haswell"];
    #else
    simdjson::get_active_implementation() = simdjson::get_available_implementations()["westmere"];
    #endif
    std::cerr << "simdjson=" << simdjson::get_active_implementation()->name() << '\n';
#endif
    std::vector<dataset> all;
    for (unsigned mode = 0; mode < 3; ++mode) {
        for (size_t n : {size_t{1}, size_t{4}, size_t{16}, size_t{1024}}) {
            all.push_back(
                {std::to_string(mode) + "_records_" + std::to_string(n), {records(mode, n)}});
        }
    }
    dataset mixed{"shuffled", {}};
    for (unsigned mode = 0; mode < 3; ++mode) {
        for (size_t n = 1; n < 128; n += 7)
            mixed.inputs.push_back(records(mode, n));
    }
    std::shuffle(mixed.inputs.begin(), mixed.inputs.end(), std::mt19937(0));
    all.push_back(std::move(mixed));
    std::ifstream file("test/data/success/twitter.json");
    if (file) {
        const std::string twitter((std::istreambuf_iterator<char>(file)), {});
        all.push_back({"twitter_pretty", {twitter}});
        all.push_back({"twitter_compact", {scalar(twitter)}});
    }
    for (const auto &data : all) {
        for (const auto &input : data.inputs) {
            if (!verify(input, true))
                return 3;
#ifdef WITH_SIMDJSON
            std::vector<char> output(input.size() + 64);
            const auto end = mature(output.data(), input.data(), input.data() + input.size());
            if (std::string(output.data(), end - output.data()) != scalar(input))
                return 4;
#endif
        }
        if (argc > 2) {
            measure(data, wjr::json::minify, "candidate");
            measure(data, wjr::json::reference_minify, "reference");
        } else {
            measure(data, wjr::json::reference_minify, "reference");
            measure(data, wjr::json::minify, "candidate");
        }
#ifdef WITH_SIMDJSON
        measure(data, mature, "simdjson");
#endif
    }
}
