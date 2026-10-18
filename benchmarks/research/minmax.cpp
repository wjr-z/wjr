#include <algorithm>
#include <array>
#include <bit>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string_view>
#include <vector>

#include "minmax_ops.hpp"

namespace research {
constexpr std::array patterns{"finite",          "ordered",          "equal-heavy", "signed-zero",
                              "sparse-left-nan", "sparse-right-nan", "mixed-nan"};
template <class T>
using bits_type = std::conditional_t<sizeof(T) == 4, uint32_t, uint64_t>;

template <class T>
bool equivalent(T left, T right) {
    return (std::isnan(left) && std::isnan(right)) ||
           std::bit_cast<bits_type<T>>(left) == std::bit_cast<bits_type<T>>(right);
}

template <bool Minimum, class T>
T oracle(T left, T right) {
    if (std::isnan(left))
        return left;
    if (std::isnan(right))
        return right;
    if (left == right)
        return left;
    if constexpr (Minimum)
        return left < right ? left : right;
    else
        return left > right ? left : right;
}

template <class T>
void fill(std::string_view pattern, std::vector<T> &left, std::vector<T> &right) {
    uint64_t state = 0x123456789abcdefULL;
    auto random = [&] {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        return static_cast<int>(state % 20001) - 10000;
    };
    const T nan = std::numeric_limits<T>::quiet_NaN();
    for (size_t i = 0; i != left.size(); ++i) {
        T a = random() * T(.125), b = random() * T(.125) + T(.0625);
        if (pattern == "ordered") {
            a = T(i % 997) * T(.25);
            b = a + T(.5);
        } else if (pattern == "equal-heavy") {
            a = T(i % 11) - 5;
            b = i % 5 ? a : a + 1;
        } else if (pattern == "signed-zero") {
            a = i & 1 ? -T(0) : T(0);
            b = i & 2 ? -T(0) : T(0);
        } else if (pattern == "sparse-left-nan") {
            if (i % 257 == 13)
                a = nan;
        } else if (pattern == "sparse-right-nan") {
            if (i % 257 == 13)
                b = nan;
        } else if (pattern == "mixed-nan") {
            if (i % 257 == 13 || i % 509 == 7)
                a = nan;
            if (i % 257 == 13 || i % 383 == 9)
                b = nan;
        }
        left[i] = a;
        right[i] = b;
    }
}

template <class T, bool Minimum>
size_t validate() {
    constexpr auto inf = std::numeric_limits<T>::infinity();
    constexpr auto nan = std::numeric_limits<T>::quiet_NaN();
    constexpr auto tiny = std::numeric_limits<T>::denorm_min();
    const std::array<T, 10> edge{-inf, -1, -tiny, -T(0), T(0), tiny, 1, inf, nan, -nan};
    std::vector<T> left, right;
    for (T a : edge)
        for (T b : edge) {
            left.push_back(a);
            right.push_back(b);
        }
    std::vector<T> output(left.size() + 2, T(1234));
    size_t checks = 0;
    auto check = [&](size_t count, const variant<T> &item) {
        item.function(output.data() + 1, left.data(), right.data(), count);
        if (output.front() != T(1234) || output[count + 1] != T(1234))
            std::abort();
        for (size_t i = 0; i != count; ++i)
            if (!equivalent(oracle<Minimum>(left[i], right[i]), output[i + 1])) {
                std::cerr << "oracle failed " << item.name << " dtype-bytes=" << sizeof(T)
                          << " count=" << count << " index=" << i << '\n';
                std::abort();
            }
        ++checks;
    };
    for (const auto &item : variants<T, Minimum>) {
        std::fill(output.begin(), output.end(), T(1234));
        check(left.size(), item);
    }
    for (size_t count : std::array<size_t, 10>{0, 1, 2, 3, 7, 16, 17, 31, 300, 8192}) {
        left.resize(count);
        right.resize(count);
        output.resize(count + 2);
        for (auto pattern : patterns) {
            fill(pattern, left, right);
            for (const auto &item : variants<T, Minimum>) {
                std::fill(output.begin(), output.end(), T(1234));
                check(count, item);
            }
        }
    }
    return checks;
}

template <class T>
double measure(kernel<T> function, T *out, const T *left, const T *right,
               const std::vector<size_t> &lengths, size_t repetitions) {
    const auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i != repetitions; ++i)
        for (auto count : lengths) {
            function(out, left, right, count);
            // Escape the payload, not addresses of mutable pointer variables.
            asm volatile("" : : "r"(out) : "memory");
        }
    return std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - start)
        .count();
}

template <class T, bool Minimum>
void benchmark() {
    constexpr const char *dtype = sizeof(T) == 4 ? "float" : "double";
    constexpr const char *op = Minimum ? "minimum" : "maximum";
    std::vector<T> left(8192), right(8192), output(8192);
    for (auto pattern : patterns) {
        fill(pattern, left, right);
        for (int mode = 0; mode != 4; ++mode) {
            const std::vector<size_t> lengths =
                mode == 0   ? std::vector<size_t>{16}
                : mode == 1 ? std::vector<size_t>{300}
                : mode == 2 ? std::vector<size_t>{8192}
                            : std::vector<size_t>{16, 300, 8192, 7, 33, 4096, 0, 1, 1024};
            size_t points = 0;
            for (auto count : lengths)
                points += count;
            std::array<size_t, variants<T, Minimum>.size()> repetitions{};
            for (size_t v = 0; v != repetitions.size(); ++v) {
                auto &count = repetitions[v];
                count = 1;
                while (measure(variants<T, Minimum>[v].function, output.data(), left.data(),
                               right.data(), lengths, count) < 2e6 &&
                       count < (size_t{1} << 20))
                    count *= 2;
            }
            for (size_t round = 0; round != 5; ++round)
                for (size_t v = 0; v != repetitions.size(); ++v) {
                    const auto index = (v + round * 3) % repetitions.size();
                    const auto &item = variants<T, Minimum>[index];
                    const auto elapsed = measure(item.function, output.data(), left.data(),
                                                 right.data(), lengths, repetitions[index]);
                    std::cout << dtype << ',' << op << ',' << (mode == 3 ? "mixed" : "fixed") << ','
                              << (mode == 3 ? 0 : lengths.front()) << ',' << pattern << ','
                              << item.name << ',' << round << ',' << repetitions[index] << ','
                              << elapsed << ',' << elapsed / (repetitions[index] * points) << ','
                              << elapsed / (repetitions[index] * lengths.size()) << '\n';
                }
        }
    }
}
} // namespace research

int main(int argc, char **argv) {
    const auto checks = research::validate<float, true>() + research::validate<float, false>() +
                        research::validate<double, true>() + research::validate<double, false>();
    std::cerr << "PASS " << checks
              << " actual API/hand oracle invocations, NaN class and non-NaN bits\n";
    if (argc < 2 || std::string_view(argv[1]) != "bench")
        return 0;
    std::cout
        << std::setprecision(12)
        << "dtype,operation,mode,count,pattern,variant,round,repetitions,total_ns,ns_element,ns_call\n";
    research::benchmark<float, true>();
    research::benchmark<float, false>();
    research::benchmark<double, true>();
    research::benchmark<double, false>();
}
