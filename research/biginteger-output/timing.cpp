#include "word_policy.hpp"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iterator>
#include <vector>

struct output_sink {
    using value_type = char;
    using difference_type = std::ptrdiff_t;
    using pointer = char *;
    using reference = char &;
    using iterator_category = std::output_iterator_tag;
    char *current;
    char &operator*() const { return *current; }
    output_sink &operator++() {
        ++current;
        return *this;
    }
    output_sink operator++(int) {
        auto old = *this;
        ++*this;
        return old;
    }
};

template <bool Old>
WJR_NOINLINE char *convert(char *out, const uint64_t *in, std::size_t n, unsigned base) {
    if constexpr (Old)
        return wjr::_probe_word_public_to_chars(output_sink{out}, in, n, base,
                                                wjr::char_converter_t{})
            .current;
    else
        return wjr::biginteger_to_chars(output_sink{out}, in, n, base).current;
}

volatile unsigned checksum;
int main() {
    using clock = std::chrono::steady_clock;
    std::puts("limbs,base,repetition,mode,ns_per_call");
    for (std::size_t n : {3, 64, 256, 1024}) {
        std::vector<uint64_t> input(n);
        for (std::size_t i = 0; i < n; ++i)
            input[i] = 0x9e3779b97f4a7c15ULL * (i + 1);
        input.back() |= uint64_t{1} << 63;
        std::vector<char> out(n * 64);
        for (unsigned base : {2, 4, 8, 10, 16, 32}) {
            const auto old_end = convert<true>(out.data(), input.data(), n, base);
            const auto length = old_end - out.data();
            const auto ref = out;
            if (convert<false>(out.data(), input.data(), n, base) - out.data() != length ||
                ref != out)
                return 2;
            std::size_t loops = 1;
            for (;;) {
                auto begin = clock::now();
                for (std::size_t i = 0; i < loops; ++i)
                    convert<false>(out.data(), input.data(), n, base);
                if (clock::now() - begin > std::chrono::milliseconds(3))
                    break;
                loops *= 2;
            }
            for (unsigned rep = 0; rep < 7; ++rep) {
                for (unsigned order = 0; order < 2; ++order) {
                    const bool old = (order ^ rep) & 1;
                    const auto fn = old ? convert<true> : convert<false>;
                    for (unsigned warm = 0; warm < 20; ++warm)
                        fn(out.data(), input.data(), n, base);
                    auto begin = clock::now();
                    for (std::size_t i = 0; i < loops; ++i) {
                        char *end = fn(out.data(), input.data(), n, base);
                        checksum = static_cast<unsigned char>(out[0]) +
                                   static_cast<unsigned char>(end[-1]);
                    }
                    const double ns =
                        std::chrono::duration<double, std::nano>(clock::now() - begin).count() /
                        loops;
                    std::printf("%zu,%u,%u,%s,%.3f\n", n, base, rep, old ? "word" : "byte", ns);
                }
            }
        }
    }
}
