// #include "fast_float/fast_float.h"

#include <cstdint>
#include <ios>
#include <iostream>
#include <random>
#include <system_error>
#include <utility>

#include <gtest/gtest.h>

#include <wjr/format/fastfloat.hpp>

using namespace wjr;

#if !(defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW64__) || defined(sun) ||       \
      defined(__sun))

    #if defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW64__) || defined(sun) ||     \
        defined(__sun)
        // Anything at all that is related to cygwin, msys and so forth will
        // always use this fallback because we cannot rely on it behaving as normal
        // gcc.
        #include <locale>
        #include <sstream>
// workaround for CYGWIN
double cygwin_strtod_l(const char *start, char **end) {
    double d;
    std::stringstream ss;
    ss.imbue(std::locale::classic());
    ss << start;
    ss >> d;
    if (ss.fail()) {
        *end = nullptr;
    }
    if (ss.eof()) {
        ss.clear();
    }
    auto nread = ss.tellg();
    *end = const_cast<char *>(start) + nread;
    return d;
}
float cygwin_strtof_l(const char *start, char **end) {
    float d;
    std::stringstream ss;
    ss.imbue(std::locale::classic());
    ss << start;
    ss >> d;
    if (ss.fail()) {
        *end = nullptr;
    }
    if (ss.eof()) {
        ss.clear();
    }
    auto nread = ss.tellg();
    *end = const_cast<char *>(start) + nread;
    return d;
}
    #endif

class RandomEngine {
public:
    RandomEngine() = delete;
    RandomEngine(uint64_t new_seed) : wyhash64_x_(new_seed) {};
    uint64_t next() {
        // Adapted from https://github.com/wangyi-fudan/wyhash/blob/master/wyhash.h
        // Inspired from
        // https://github.com/lemire/testingRNG/blob/master/source/wyhash.h
        wyhash64_x_ += UINT64_C(0x60bee2bee120fc15);
        uint128_t tmp = mul64x64to128(wyhash64_x_, UINT64_C(0xa3b195354a39b70d));
        uint64_t m1 = (tmp.high) ^ tmp.low;
        tmp = mul64x64to128(m1, UINT64_C(0x1b03738712fad5c9));
        uint64_t m2 = (tmp.high) ^ tmp.low;
        return m2;
    }
    bool next_bool() { return (next() & 1) == 1; }
    int next_int() { return static_cast<int>(next()); }
    char next_char() { return static_cast<char>(next()); }
    double next_double() { return static_cast<double>(next()); }

    int next_ranged_int(int min, int max) { // min and max are included
        // Adapted from
        // https://lemire.me/blog/2019/06/06/nearly-divisionless-random-integer-generation-on-various-systems/
        /*  if (min == max) {
             return min;
        }*/
        uint64_t s = uint64_t(max - min + 1);
        uint64_t x = next();
        uint128_t m = mul64x64to128(x, s);
        uint64_t l = m.low;
        if (l < s) {
            uint64_t t = -s % s;
            while (l < t) {
                x = next();
                m = mul64x64to128(x, s);
                l = m.low;
            }
        }
        return int(m.high) + min;
    }
    int next_digit() { return next_ranged_int(0, 9); }

private:
    uint64_t wyhash64_x_;
};

static size_t build_random_string(RandomEngine &rand, char *buffer) {
    size_t pos{0};
    if (rand.next_bool()) {
        buffer[pos++] = '-';
    }
    int number_of_digits = rand.next_ranged_int(1, 1000);
    if (number_of_digits == 100) {
        // With low probability, we want to allow very long strings just to stress the
        // system.
        number_of_digits = rand.next_ranged_int(1, 2000);
    }
    int location_of_decimal_separator = rand.next_ranged_int(1, number_of_digits);
    for (size_t i = 0; i < size_t(number_of_digits); i++) {
        if (i == size_t(location_of_decimal_separator)) {
            buffer[pos++] = '.';
        }
        buffer[pos] = char(rand.next_digit() + '0');
        // We can have a leading zero only if location_of_decimal_separator = 1.
        while (i == 0 && 1 != size_t(location_of_decimal_separator) && buffer[pos] == '0') {
            buffer[pos] = char(rand.next_digit() + '0');
        }
        pos++;
    }
    if (rand.next_bool()) {
        if (rand.next_bool()) {
            buffer[pos++] = 'e';
        } else {
            buffer[pos++] = 'E';
        }
        if (rand.next_bool()) {
            buffer[pos++] = '-';
        } else {
            if (rand.next_bool()) {
                buffer[pos++] = '+';
            }
        }
        number_of_digits = rand.next_ranged_int(1, 3);
        for (size_t i = 0; i < size_t(number_of_digits); i++) {
            buffer[pos++] = char(rand.next_digit() + '0');
        }
    }
    buffer[pos] = '\0'; // null termination
    return pos;
}

static std::pair<double, bool> strtod_from_string(char *st) {
    double d;
    char *pr;
    #if defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW64__) || defined(sun) ||     \
        defined(__sun)
    d = cygwin_strtod_l(st, &pr);
    #elif defined(_WIN32)
    static _locale_t c_locale = _create_locale(LC_ALL, "C");
    d = _strtod_l(st, &pr, c_locale);
    #else
    static locale_t c_locale = newlocale(LC_ALL_MASK, "C", NULL);
    d = strtod_l(st, &pr, c_locale);
    #endif
    if (st == pr) {
        std::cerr << "strtod_l could not parse '" << st << std::endl;
        return std::make_pair(0, false);
    }
    return std::make_pair(d, true);
}

static std::pair<float, bool> strtof_from_string(char *st) {
    float d;
    char *pr;
    #if defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW64__) || defined(sun) ||     \
        defined(__sun)
    d = cygwin_strtof_l(st, &pr);
    #elif defined(_WIN32)
    static _locale_t c_locale = _create_locale(LC_ALL, "C");
    d = _strtof_l(st, &pr, c_locale);
    #else
    static locale_t c_locale = newlocale(LC_ALL_MASK, "C", NULL);
    d = strtof_l(st, &pr, c_locale);
    #endif
    if (st == pr) {
        std::cerr << "strtof_l could not parse '" << st << std::endl;
        return std::make_pair(0.0f, false);
    }
    return std::make_pair(d, true);
}

/**
 * We generate random strings and we try to parse them with both strtod/strtof,
 * and we verify that we get the same answer with with fast_float::from_chars.
 */
static bool tester(uint64_t seed, size_t volume) {
    char buffer[4096]; // large buffer (can't overflow)
    RandomEngine rand(seed);
    for (size_t i = 0; i < volume; i++) {
        size_t length = build_random_string(rand, buffer);
        std::pair<double, bool> expected_double = strtod_from_string(buffer);
        if (expected_double.second) {
            double result_value;
            auto result = from_chars(buffer, buffer + length, result_value);
            if (result.ec != std::errc() && result.ec != std::errc::result_out_of_range) {
                printf("parsing %.*s\n", int(length), buffer);
                std::cerr << " I could not parse " << std::endl;
                return false;
            }
            if (result.ptr != buffer + length) {
                printf("parsing %.*s\n", int(length), buffer);
                std::cerr << " Did not get to the end " << std::endl;
                return false;
            }
            if (result_value != expected_double.first) {
                printf("parsing %.*s\n", int(length), buffer);
                std::cerr << std::hexfloat << result_value << std::endl;
                std::cerr << std::hexfloat << expected_double.first << std::endl;
                std::cerr << " Mismatch " << std::endl;
                return false;
            }
        }
        std::pair<float, bool> expected_float = strtof_from_string(buffer);
        if (expected_float.second) {
            float result_value;
            auto result = from_chars(buffer, buffer + length, result_value);
            if (result.ec != std::errc() && result.ec != std::errc::result_out_of_range) {
                printf("parsing %.*s\n", int(length), buffer);
                std::cerr << " I could not parse " << std::endl;
                return false;
            }
            if (result.ptr != buffer + length) {
                printf("parsing %.*s\n", int(length), buffer);
                std::cerr << " Did not get to the end " << std::endl;
                return false;
            }
            if (result_value != expected_float.first) {
                printf("parsing %.*s\n", int(length), buffer);
                std::cerr << std::hexfloat << result_value << std::endl;
                std::cerr << std::hexfloat << expected_float.first << std::endl;
                std::cerr << " Mismatch " << std::endl;
                return false;
            }
        }
    }
    return true;
}

TEST(fastfloat, random_string) { WJR_CHECK(tester(std::random_device{}(), 200000)); }

#endif