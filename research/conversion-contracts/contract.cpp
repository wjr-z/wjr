#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <gmp.h>
#include <map>
#include <vector>
#include <wjr/biginteger/detail/convert.hpp>

extern "C" decltype(mpn_get_str) __real___gmpn_get_str;
extern "C" decltype(mpn_set_str) __real___gmpn_set_str;

static std::map<std::pair<int, std::size_t>, std::vector<mp_limb_t>> formatting_inputs;
static std::map<std::pair<int, std::size_t>, std::vector<unsigned char>> parsing_inputs;
static std::size_t get_calls = 0, set_calls = 0;
static void require(bool value, const char *message) {
    if (!value) {
        std::fprintf(stderr, "CONTRACT ERROR: %s\n", message);
        std::abort();
    }
}

extern "C" auto __wrap___gmpn_get_str(unsigned char *digits, int base, mp_ptr input,
                                      mp_size_t count) -> decltype(mpn_get_str(digits, base, input,
                                                                               count)) {
    require(count > 0 && input[count - 1] != 0, "format input is not normalized");
    std::vector<mp_limb_t> original(input, input + count);
    auto [found, inserted] = formatting_inputs.emplace(std::make_pair(base, count), original);
    require(inserted || found->second == original, "format input changed between iterations");
    const auto length = __real___gmpn_get_str(digits, base, input, count);
    require(length <= static_cast<std::size_t>(count) * 64 + 1, "format output exceeds capacity");
    require(
        std::all_of(digits, digits + length, [base](unsigned char digit) { return digit < base; }),
        "format output is not raw digits");
    std::vector<uint8_t> expected(static_cast<std::size_t>(count) * 64 + 1);
    auto *end = wjr::biginteger_to_chars(expected.data(), original.data(), count, base,
                                         wjr::origin_converter_t{});
    auto *begin = digits;
    while (begin != digits + length && *begin == 0)
        ++begin;
    require(static_cast<std::size_t>(digits + length - begin) ==
                static_cast<std::size_t>(end - expected.data()),
            "format lengths disagree");
    require(std::equal(begin, digits + length, expected.data()),
            "WJR and GMP raw format output disagree");
    ++get_calls;
    return length;
}

extern "C" auto __wrap___gmpn_set_str(mp_ptr output, const unsigned char *digits, std::size_t count,
                                      int base) -> decltype(mpn_set_str(output, digits, count,
                                                                        base)) {
    require(count > 0 && digits[0] != 0, "parse input is not normalized");
    require(
        std::all_of(digits, digits + count, [base](unsigned char digit) { return digit < base; }),
        "raw digit outside base");
    std::vector<unsigned char> original(digits, digits + count);
    auto [found, inserted] = parsing_inputs.emplace(std::make_pair(base, count), original);
    require(inserted || found->second == original, "parse input changed between iterations");
    const auto length = __real___gmpn_set_str(output, digits, count, base);
    std::vector<uint64_t> expected((count * 4 + 63) / 64 + 1);
    auto *end = wjr::biginteger_from_chars(digits, digits + count, expected.data(), base,
                                           wjr::origin_converter_t{});
    require(static_cast<std::size_t>(length) == static_cast<std::size_t>(end - expected.data()),
            "parse lengths disagree");
    require(std::equal(output, output + length, expected.data()),
            "WJR and GMP parsed limbs disagree");
    require(std::equal(digits, digits + count, original.begin()), "parse input was mutated");
    ++set_calls;
    return length;
}

struct report_results {
    ~report_results() {
        std::fprintf(
            stderr,
            "CONTRACT OK GMP=%s get_calls=%zu set_calls=%zu format_cases=%zu parse_cases=%zu\n",
            gmp_version, get_calls, set_calls, formatting_inputs.size(), parsing_inputs.size());
    }
} results;
