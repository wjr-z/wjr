#if defined(WJR_TEST_XSIMD)

    #include <algorithm>
    #include <array>
    #include <limits>
    #include <vector>

    #include <gtest/gtest.h>

    #include <wjr/simd/xsimd.hpp>

namespace backend = wjr::simd::xsimd_backend;

namespace {

template <typename T>
concept has_scan = requires(const T *src, T value) { backend::find_equal(src, value, 0); };

static_assert(has_scan<std::uint64_t>);
static_assert(has_scan<float>);
static_assert(!has_scan<bool>);
static_assert(!has_scan<long double>);
static_assert(!has_scan<std::array<int, 2>>);

template <typename T>
void check_predicates() {
    using mask = backend::mask<T>;
    constexpr auto lanes = backend::batch<T>::size;
    std::array<bool, lanes> values{};
    auto check = [&](std::size_t first, bool all) {
        const auto predicate = mask::load_unaligned(values.data());
        EXPECT_EQ(backend::any(predicate), first != lanes);
        EXPECT_EQ(backend::all(predicate), all);
        EXPECT_EQ(backend::first_true(predicate), first);
    };
    check(lanes, false);
    for (std::size_t i = 0; i < lanes; ++i) {
        values.fill(false);
        values[i] = true;
        check(i, lanes == 1);
        // The first lane wins even when later lanes also match.
        for (std::size_t j = i; j < lanes; ++j) {
            values[j] = true;
        }
        check(i, i == 0);
    }
}

template <typename T>
void check_scan() {
    for (std::size_t count = 0; count <= 129; ++count) {
        for (std::size_t offset = 0; offset < 64 / sizeof(T); ++offset) {
            // The logical end is the allocation end, making tail overreads visible to ASan.
            std::vector<T> storage(count + offset, T(7));
            const T *src = count == 0 ? nullptr : storage.data() + offset;
            EXPECT_EQ(backend::find_equal(src, T(42), count), count);
            for (std::size_t position = 0; position < count; ++position) {
                storage[offset + position] = T(42);
                EXPECT_EQ(backend::find_equal(src, T(42), count), position);
                storage[offset + position] = T(7);
            }
        }
    }
}

template <typename T>
void check_floating_scan() {
    const T nan = std::numeric_limits<T>::quiet_NaN();
    const T inf = std::numeric_limits<T>::infinity();
    std::array<T, 37> values{};
    values.fill(nan);
    values[11] = -T(0);
    values[25] = inf;
    values[33] = -inf;
    for (T needle : {nan, T(0), -T(0), inf, -inf, T(42)}) {
        const auto expected = std::find(values.begin(), values.end(), needle) - values.begin();
        EXPECT_EQ(backend::find_equal(values.data(), needle, values.size()),
                  static_cast<std::size_t>(expected));
    }
}

} // namespace

TEST(simd_xsimd, lane_predicates) {
    check_predicates<std::uint8_t>();
    check_predicates<std::uint16_t>();
    check_predicates<std::uint32_t>();
    check_predicates<std::uint64_t>();
    check_predicates<float>();
    check_predicates<double>();
}

TEST(simd_xsimd, bounded_integer_scan) {
    check_scan<std::uint8_t>();
    check_scan<std::uint16_t>();
    check_scan<std::uint32_t>();
    check_scan<std::uint64_t>();
}

TEST(simd_xsimd, floating_equality) {
    check_floating_scan<float>();
    check_floating_scan<double>();
}

#endif // WJR_TEST_XSIMD
