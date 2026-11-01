#include "word_policy.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <list>
#include <new>
#include <string>
#include <vector>

namespace memory_pass {
bool active = false;
std::size_t calls = 0, bytes = 0, frees = 0;
void *allocate(std::size_t n) {
    if (active) {
        ++calls;
        bytes += n;
    }
    return std::malloc(n ? n : 1);
}
void release(void *p) {
    if (active && p)
        ++frees;
    std::free(p);
}
void begin() {
    calls = bytes = frees = 0;
    active = true;
}
void end() { active = false; }
} // namespace memory_pass
void *operator new(std::size_t n) {
    if (auto *p = memory_pass::allocate(n))
        return p;
    throw std::bad_alloc();
}
void *operator new[](std::size_t n) { return ::operator new(n); }
void *operator new(std::size_t n, const std::nothrow_t &) noexcept {
    return memory_pass::allocate(n);
}
void *operator new[](std::size_t n, const std::nothrow_t &) noexcept {
    return memory_pass::allocate(n);
}
void operator delete(void *p) noexcept { memory_pass::release(p); }
void operator delete[](void *p) noexcept { memory_pass::release(p); }
void operator delete(void *p, std::size_t) noexcept { memory_pass::release(p); }
void operator delete[](void *p, std::size_t) noexcept { memory_pass::release(p); }
void operator delete(void *p, const std::nothrow_t &) noexcept { memory_pass::release(p); }
void operator delete[](void *p, const std::nothrow_t &) noexcept { memory_pass::release(p); }

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
static_assert(!wjr::charconv_detail::_is_fast_convert_iterator_v<output_sink>);
static_assert(sizeof(wjr::charconv_detail::fast_buffer_t<output_sink>) == 1);
static_assert(
    wjr::charconv_detail::is_fast_container_inserter_v<std::back_insert_iterator<std::string>> !=
    0);
static_assert(wjr::mem::default_new_alignment == 16);

std::size_t capacity(std::size_t n, unsigned base) {
    if (base == 2)
        return 64 * n;
    if (base == 4)
        return (64 * n + 1) / 2;
    if (base == 8)
        return (64 * n + 2) / 3;
    if (base == 16)
        return (64 * n + 3) / 4;
    if (base == 32)
        return (64 * n + 4) / 5;
    return (64 * n * 4 + 12) / 13;
}

std::size_t checking_limbs;
unsigned checking_base;
int checking_pattern;
void require(bool value, const char *message) {
    if (!value) {
        std::fprintf(stderr, "%s: limbs=%zu base=%u pattern=%d\n", message, checking_limbs,
                     checking_base, checking_pattern);
        std::abort();
    }
}

template <class Converter>
void check(std::size_t n, unsigned base, int pattern, Converter converter) {
    checking_limbs = n;
    checking_base = base;
    checking_pattern = pattern;
    const auto count = capacity(n, base);
    std::vector<uint64_t> input(n);
    for (std::size_t i = 0; i != n; ++i)
        input[i] = pattern == 0 ? UINT64_MAX : pattern == 1 ? 0 : 0x9e3779b97f4a7c15ULL * (i + 1);
    if (pattern == 1)
        input.back() = 1;
    const auto original = input;
    constexpr char canary = char(0xa5);
    std::vector<char> reference(count + 64, canary), actual(count + 64, canary);
    char *const expected_start = reference.data() + 32;
    char *const actual_start = actual.data() + 32;
    char *const expected_end =
        wjr::biginteger_to_chars(expected_start, input.data(), n, base, converter);
    const auto length = static_cast<std::size_t>(expected_end - expected_start);
    require(length <= count, "conversion exceeded byte capacity");
    require(
        std::all_of(reference.begin(), reference.begin() + 32, [](char x) { return x == canary; }),
        "prefix overstore");
    require(std::all_of(reference.begin() + 32 + count, reference.end(),
                        [](char x) { return x == canary; }),
            "suffix overstore");

    auto candidate = [&] {
        return wjr::biginteger_to_chars(output_sink{actual_start}, input.data(), n, base, converter)
            .current;
    };
    auto old = [&] {
        return wjr::_probe_word_public_to_chars(output_sink{actual_start}, input.data(), n, base,
                                                converter)
            .current;
    };
    for (int mode = 0; mode != 2; ++mode) {
        std::fill(actual.begin(), actual.end(), canary);
        char *end = mode == 0 ? old() : candidate();
        require(end - actual_start == static_cast<std::ptrdiff_t>(length),
                "generic length changed");
        require(std::equal(expected_start, expected_end, actual_start), "generic output changed");
        require(
            std::all_of(actual.begin(), actual.begin() + 32, [](char x) { return x == canary; }),
            "generic prefix overwrite");
        require(std::all_of(actual.begin() + 32 + count, actual.end(),
                            [](char x) { return x == canary; }),
                "generic suffix overwrite");
    }
    std::string text;
    text.reserve(count);
    wjr::biginteger_to_chars(std::back_inserter(text), input.data(), n, base, converter);
    require(text.size() == length && std::equal(text.begin(), text.end(), expected_start),
            "string fast path mismatch");
    require(input == original, "input limbs mutated");

    if constexpr (std::is_same_v<Converter, wjr::char_converter_t>) {
        if (pattern == 0) {
            for (int mode = 0; mode != 3; ++mode) {
                text.clear();
                memory_pass::begin();
                if (mode == 0)
                    old();
                else if (mode == 1)
                    candidate();
                else
                    wjr::biginteger_to_chars(std::back_inserter(text), input.data(), n, base,
                                             converter);
                memory_pass::end();
                std::printf("%zu,%u,%s,%zu,%zu,%zu,%zu\n", n, base,
                            mode == 0   ? "old-generic"
                            : mode == 1 ? "byte-generic"
                                        : "string-fast",
                            count, memory_pass::calls, memory_pass::bytes, memory_pass::frees);
            }
        }
    }
}

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::puts("limbs,base,mode,output_capacity_bytes,heap_calls,heap_bytes,heap_frees");
    for (const std::size_t n : {2, 3, 4, 8, 32, 64, 128, 256, 1024})
        for (const unsigned base : {2, 4, 8, 10, 16, 32})
            for (int pattern = 0; pattern != 3; ++pattern) {
                check(n, base, pattern, wjr::char_converter_t{});
                check(n, base, pattern, wjr::origin_converter_t{});
            }
}
