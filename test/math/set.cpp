#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/math/set.hpp>

namespace {

template <typename T>
void check_fill(size_t n, size_t offset, T value) {
    constexpr T sentinel = static_cast<T>(0xa5a5a5a5a5a5a5a5ULL);
    std::vector<T> storage(n + offset + 65, sentinel);
    auto expected = storage;
    std::fill_n(expected.begin() + offset, n, value);
    wjr::set_n(storage.data() + offset, value, n);
    EXPECT_EQ(storage, expected) << "n=" << n << ", offset=" << offset;
}

template <typename T>
void check_boundaries() {
    wjr::set_n<T>(nullptr, T(0), 0);
    constexpr std::array<T, 4> values = {T(0), std::numeric_limits<T>::max(),
                                         static_cast<T>(0x0123456789abcdefULL),
                                         static_cast<T>(T(1) << (sizeof(T) * 8 - 1))};
    for (size_t n = 0; n <= 257; ++n) {
        for (size_t offset = 0; offset < 64 / sizeof(T); ++offset) {
            for (T value : values) {
                check_fill(n, offset, value);
            }
        }
    }
    for (size_t n : {511, 512, 513, 1023, 1024, 1025, 4095, 4096, 4097}) {
        for (size_t offset = 0; offset < 64 / sizeof(T); ++offset) {
            check_fill(n, offset, values[2]);
        }
    }

    // Exact allocations make a SIMD tail overrun observable under ASan.
    for (size_t n = 1; n <= 129; ++n) {
        auto storage = std::make_unique<T[]>(n);
        wjr::set_n(storage.get(), values[2], n);
        for (size_t i = 0; i < n; ++i) {
            ASSERT_EQ(storage[i], values[2]) << "n=" << n << ", i=" << i;
        }
    }
}

} // namespace

TEST(set, uint16_boundaries) { check_boundaries<uint16_t>(); }
TEST(set, uint32_boundaries) { check_boundaries<uint32_t>(); }
TEST(set, uint64_boundaries) { check_boundaries<uint64_t>(); }
