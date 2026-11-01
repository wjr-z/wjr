#include <wjr/json/array.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

struct allocation_counts {
    std::size_t calls = 0, bytes = 0, live = 0, peak = 0;
};

/** @brief Single-threaded instrumentation; only projected containers use this allocator. */
allocation_counts counts;

template <typename T>
struct counting_allocator {
    using value_type = T;
    counting_allocator() = default;
    template <typename U>
    counting_allocator(const counting_allocator<U> &) {}

    T *allocate(std::size_t n) {
        auto *pointer = std::allocator<T>{}.allocate(n);
        ++counts.calls;
        counts.bytes += n * sizeof(T);
        counts.live += n * sizeof(T);
        counts.peak = std::max(counts.peak, counts.live);
        return pointer;
    }
    void deallocate(T *pointer, std::size_t n) noexcept {
        counts.live -= n * sizeof(T);
        std::allocator<T>{}.deallocate(pointer, n);
    }
    bool operator==(const counting_allocator &) const = default;
};

template <typename T>
using tracked_vector = std::vector<T, counting_allocator<T>>;

struct summary {
    std::size_t points;
    double checksum;
};

template <typename Polygon>
summary project(const char *name, const wjr::json::document &coordinates) {
    counts = {};
    summary result{};
    std::size_t resident;
    {
        const auto polygon = wjr::json::value_to<Polygon>(coordinates);
        for (const auto &ring : polygon) {
            for (const auto &point : ring) {
                if (point.size() != 2) {
                    throw std::length_error("This example requires two-dimensional positions");
                }
                ++result.points;
                result.checksum += point[0] + point[1];
            }
        }
        resident = counts.live;
    }
    std::printf("%s,%zu,%zu,%zu,%zu,%zu,%zu,%.17g\n", name, result.points, counts.calls,
                counts.bytes, resident, counts.peak, counts.live, result.checksum);
    return result;
}

} // namespace

int main(int argc, char **argv) {
    if (argc != 2) {
        std::fprintf(stderr, "Usage: json_fixed_coordinates <two-dimensional-polygon.json>\n");
        return 1;
    }
    try {
        std::ifstream file(argv[1], std::ios::binary);
        if (!file) {
            throw std::runtime_error("Could not open input");
        }
        const std::string input((std::istreambuf_iterator<char>(file)), {});
        wjr::json::ondemand_reader reader;
        reader.read(input);
        const auto parsed = wjr::json::document::parse(reader);
        if (!parsed) {
            throw std::runtime_error("Could not parse input");
        }
        const auto &features = parsed->at("features").get<wjr::json::array_t>();
        if (features.size() != 1) {
            throw std::runtime_error("This example requires one Polygon feature");
        }
        const auto &geometry = features[0].at("geometry");
        if (wjr::json::value_to<std::string_view>(geometry.at("type")) != "Polygon") {
            throw std::runtime_error("This example requires Polygon geometry");
        }
        const auto &coordinates = geometry.at("coordinates");
        std::puts(
            "layout,points,allocations,requested_bytes,resident_bytes,peak_bytes,live_after,checksum");
        const auto reference =
            project<tracked_vector<tracked_vector<tracked_vector<double>>>>("vector", coordinates);
        const auto fixed =
            project<tracked_vector<tracked_vector<std::array<double, 2>>>>("array", coordinates);
        return reference.points == fixed.points && reference.checksum == fixed.checksum ? 0 : 2;
    } catch (const std::exception &error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
