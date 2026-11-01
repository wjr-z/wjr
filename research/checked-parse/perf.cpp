#include "old_api.hpp"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <numeric>
#include <random>
#include <string>
#include <vector>

template <bool Old>
WJR_NOINLINE std::size_t parse(const std::string &input, wjr::biginteger &value, unsigned base) {
    const auto *first = input.data();
    const auto *last = first + input.size();
    if constexpr (Old)
        return wjr::biginteger_detail::_probe_old_from_chars_impl<true>(
                   first, last, wjr::biginteger_dispatcher(&value), base)
                   .ptr -
               first;
    else
        return wjr::from_chars(first, last, value, base).ptr - first;
}

struct sample {
    std::string text;
    unsigned base;
};
volatile std::size_t observed;
int main() {
    using clock = std::chrono::steady_clock;
    std::vector<sample> samples;
    for (unsigned base : {2, 8, 10, 16})
        for (std::size_t length : {2, 9, 17, 64, 256, 1024}) {
            std::string text(length, '1');
            for (std::size_t i = 0; i < length; ++i)
                text[i] = wjr::char_converter.to((i * 7 + 1) % base);
            samples.push_back({std::move(text), base});
        }
    wjr::biginteger value;
    for (const auto &item : samples) {
        auto old_size = parse<true>(item.text, value, item.base);
        const auto expected = value;
        auto new_size = parse<false>(item.text, value, item.base);
        if (old_size != item.text.size() || new_size != old_size || value != expected)
            return 2;
    }
    std::vector<std::size_t> order(samples.size());
    std::iota(order.begin(), order.end(), 0);
    std::shuffle(order.begin(), order.end(), std::mt19937(7));
    std::puts("base,length,repetition,mode,ns_per_call");
    for (std::size_t which = 0; which <= samples.size(); ++which) {
        const bool mixed = which == samples.size();
        const auto &fixed = samples[mixed ? 0 : which];
        std::size_t loops = 1;
        for (;;) {
            const auto begin = clock::now();
            for (std::size_t i = 0; i < loops; ++i) {
                const auto &item = mixed ? samples[order[i % order.size()]] : fixed;
                observed = parse<false>(item.text, value, item.base);
            }
            if (clock::now() - begin > std::chrono::milliseconds(3))
                break;
            loops *= 2;
        }
        for (unsigned repetition = 0; repetition < 7; ++repetition)
            for (unsigned position = 0; position < 2; ++position) {
                const bool old = (repetition ^ position) & 1;
                const auto fn = old ? parse<true> : parse<false>;
                for (unsigned warm = 0; warm < 32; ++warm)
                    fn(fixed.text, value, fixed.base);
                const auto begin = clock::now();
                for (std::size_t i = 0; i < loops; ++i) {
                    const auto &item = mixed ? samples[order[i % order.size()]] : fixed;
                    observed = fn(item.text, value, item.base);
                }
                const double ns =
                    std::chrono::duration<double, std::nano>(clock::now() - begin).count() / loops;
                std::printf("%u,%zu,%u,%s,%.3f\n", mixed ? 0 : fixed.base,
                            mixed ? 0 : fixed.text.size(), repetition, old ? "old" : "bounded", ns);
            }
    }
}
