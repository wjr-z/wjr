#include <benchmark/benchmark.h>

#include <bit>
#include <string>
#include <vector>

#include <wjr/json/document.hpp>
#include <wjr/json/inplace_parser.hpp>

namespace {

using namespace wjr::json;

struct point {
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(point);
    WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(point, x, y)
    double x = 0;
    double y = 0;
};

struct record {
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(record);
    WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(record, id, sequence, price, enabled, name, position,
                                            samples, path)
    uint64_t id = 0;
    int64_t sequence = 0;
    double price = 0;
    bool enabled = false;
    std::string name;
    point position;
    std::vector<int64_t> samples;
    std::vector<point> path;
};

uint64_t fold(uint64_t sum, uint64_t value) { return (sum ^ value) * UINT64_C(1099511628211); }

template <typename T>
uint64_t checksum(const T &value) {
    uint64_t sum = UINT64_C(1469598103934665603);
    if constexpr (std::is_integral_v<T>) {
        return fold(sum, static_cast<uint64_t>(value));
    } else if constexpr (std::is_same_v<T, float>) {
        return fold(sum, std::bit_cast<uint32_t>(value));
    } else if constexpr (std::is_same_v<T, double>) {
        return fold(sum, std::bit_cast<uint64_t>(value));
    } else if constexpr (std::is_same_v<T, std::string>) {
        for (unsigned char ch : value) {
            sum = fold(sum, ch);
        }
        return fold(sum, value.size());
    } else if constexpr (requires { T::template wjr_json_fields<>(); }) {
        std::apply(
            [&](auto... field) {
                ((sum = fold(sum, checksum(value.*decltype(field)::member))), ...);
            },
            T::template wjr_json_fields<>());
        return sum;
    } else {
        for (const auto &element : value) {
            sum = fold(sum, checksum(element));
        }
        return fold(sum, value.size());
    }
}

std::string records_input(size_t count, size_t text_size, bool unknown) {
    std::string input = "[";
    for (size_t index = 0; index < count; ++index) {
        if (index != 0) {
            input += ',';
        }
        input += "{\"samples\":[1,-2,3,4,5,6,7,8],\"name\":\"" + std::string(text_size, 'x') +
                 "\\n\\u4e2d\",\"price\":1.25,\"position\":{\"y\":2.5,\"x\":1.5},"
                 "\"enabled\":true,\"id\":" +
                 std::to_string(index) +
                 ",\"sequence\":-7,\"path\":[{\"x\":1,\"y\":2},{\"y\":4,\"x\":3}]";
        if (unknown) {
            input += ",\"unknown\":{\"payload\":\"" + std::string(128, 'u') +
                     "\",\"items\":[1.25,null,true,{\"x\":\"\\u4e2d\"}]}";
        }
        input += '}';
    }
    input += ']';
    return input;
}

template <typename T, bool Direct>
void benchmark_parse(benchmark::State &state, const std::string &input) {
    ondemand_reader reader;
    reader.read(input);
    {
        auto direct = wjr::json::parse<T>(reader);
        reader.read(input);
        auto document = wjr::json::document::parse(reader);
        if (!direct || !document) {
            state.SkipWithError("Invalid parse fixture");
            return;
        }
        auto projected = value_to<T>(std::move(*document));
        if (wjr::json::document(*direct) != wjr::json::document(projected)) {
            state.SkipWithError("Typed output mismatch");
            return;
        }
    }
    for (auto _ : state) {
        reader.read(input);
        if constexpr (Direct) {
            auto value = wjr::json::parse<T>(reader);
            if (!value) {
                state.SkipWithError("Direct parse failure");
                break;
            }
            benchmark::DoNotOptimize(checksum(*value));
        } else {
            auto source = wjr::json::document::parse(reader);
            if (!source) {
                state.SkipWithError("Document parse failure");
                break;
            }
            auto value = value_to<T>(std::move(*source));
            benchmark::DoNotOptimize(checksum(value));
        }
    }
    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(input.size()));
}

template <bool Direct>
void BM_json_owned_records(benchmark::State &state) {
    const auto input = records_input(static_cast<size_t>(state.range(0)),
                                     static_cast<size_t>(state.range(1)), state.range(2) != 0);
    benchmark_parse<std::vector<record>, Direct>(state, input);
}

template <bool Direct, typename Number>
void BM_json_owned_numbers(benchmark::State &state) {
    std::string input = "[";
    for (int index = 0; index != 4096; ++index) {
        if (index != 0) {
            input += ',';
        }
        input += std::to_string(index - 2048);
        if constexpr (std::is_floating_point_v<Number>) {
            input += ".25";
        }
    }
    input += ']';
    benchmark_parse<std::vector<Number>, Direct>(state, input);
}

BENCHMARK_TEMPLATE(BM_json_owned_records, true)
    ->Args({128, 8, 0})
    ->Args({64, 256, 0})
    ->Args({64, 8, 1});
BENCHMARK_TEMPLATE(BM_json_owned_records, false)
    ->Args({128, 8, 0})
    ->Args({64, 256, 0})
    ->Args({64, 8, 1});
BENCHMARK_TEMPLATE(BM_json_owned_numbers, true, int64_t);
BENCHMARK_TEMPLATE(BM_json_owned_numbers, false, int64_t);
BENCHMARK_TEMPLATE(BM_json_owned_numbers, true, float);
BENCHMARK_TEMPLATE(BM_json_owned_numbers, false, float);

} // namespace
