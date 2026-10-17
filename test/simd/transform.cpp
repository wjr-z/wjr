#if defined(WJR_TEST_XSIMD)

    #include <algorithm>
    #include <array>
    #include <cmath>
    #include <limits>
    #include <memory>
    #include <stdexcept>

    #include <gtest/gtest.h>

    #include <wjr/simd/transform.hpp>

namespace {
namespace backend = wjr::simd::xsimd_backend;

struct increment {
    template <class V>
    auto operator()(V value) const noexcept {
        return value + V(1);
    }
};

struct add {
    template <class V>
    auto operator()(V lhs, V rhs) const noexcept {
        return lhs + rhs;
    }
};

struct potentially_throwing {
    template <class... V>
    auto operator()(V... values) const {
        return (values + ...);
    }
};

struct batch_only {
    backend::batch<float> operator()(backend::batch<float> value) const noexcept { return value; }
};

template <class T, class Operation>
concept has_unary_transform = requires(T *dst, const T *src, Operation &operation) {
    backend::transform_to(dst, src, 0, operation);
};
template <class T, class Operation>
concept has_binary_transform = requires(T *dst, const T *src, Operation &operation) {
    backend::transform_to(dst, src, src, 0, operation);
};

static_assert(has_unary_transform<float, increment> && has_binary_transform<double, add>);
static_assert(has_unary_transform<std::uint8_t, increment>);
static_assert(!has_unary_transform<bool, increment>);
static_assert(!has_unary_transform<long double, increment>);
static_assert(!has_binary_transform<bool, add>);
static_assert(!has_binary_transform<long double, add>);
static_assert(!has_unary_transform<float, batch_only>);
static_assert(noexcept(backend::transform_to<float>(nullptr, nullptr, 0, increment{})));
static_assert(noexcept(backend::transform_to<float>(nullptr, nullptr, nullptr, 0, add{})));
static_assert(!noexcept(backend::transform_to<float>(nullptr, nullptr, 0, potentially_throwing{})));
static_assert(!noexcept(backend::transform_to<float>(nullptr, nullptr, nullptr, 0,
                                                     potentially_throwing{})));

template <class T>
void check_transform() {
    constexpr auto lanes = backend::batch<T>::size;
    for (std::size_t count = 0; count <= 3 * lanes + 3; ++count) {
        for (std::size_t offset = 0; offset < 64 / sizeof(T); ++offset) {
            // The logical end equals the allocation end for ASan tail checks.
            auto lhs_storage = std::make_unique<T[]>(count + offset);
            auto rhs_storage = std::make_unique<T[]>(count + offset);
            auto dst_storage = std::make_unique<T[]>(count + offset);
            T *lhs = count ? lhs_storage.get() + offset : nullptr;
            T *rhs = count ? rhs_storage.get() + offset : nullptr;
            T *dst = count ? dst_storage.get() + offset : nullptr;
            for (std::size_t i = 0; i < count; ++i) {
                lhs[i] = T(i % 31);
                rhs[i] = T(i % 17);
            }
            backend::transform_to(dst, lhs, count, increment{});
            for (std::size_t i = 0; i < count; ++i) {
                EXPECT_EQ(dst[i], T(i % 31 + 1));
            }
            backend::transform_to(dst, lhs, rhs, count, add{});
            for (std::size_t i = 0; i < count; ++i) {
                EXPECT_EQ(dst[i], T(i % 31 + i % 17));
            }
            backend::transform_to(lhs, lhs, count, increment{});
            backend::transform_to(lhs, lhs, rhs, count, add{});
            backend::transform_to(rhs, lhs, rhs, count, add{});
            backend::transform_to(dst, dst, dst, count, add{});
            for (std::size_t i = 0; i < count; ++i) {
                EXPECT_EQ(lhs[i], T(i % 31 + 1 + i % 17));
                EXPECT_EQ(rhs[i], T(i % 31 + 1 + 2 * (i % 17)));
                EXPECT_EQ(dst[i], T(2 * (i % 31 + i % 17)));
            }
        }
    }
}

struct counted_increment {
    std::size_t batches = 0;
    std::size_t scalars = 0;
    counted_increment() = default;
    counted_increment(const counted_increment &) = delete;
    backend::batch<float> operator()(backend::batch<float> value) & noexcept {
        ++batches;
        return value + backend::batch<float>(1);
    }
    float operator()(const float &value) & noexcept {
        ++scalars;
        return value + 1;
    }
};

struct clip {
    template <class V>
    V operator()(V value) const noexcept {
        if constexpr (std::is_arithmetic_v<V>) {
            return value < V(-1) ? V(-1) : value > V(1) ? V(1) : value;
        } else {
            return xsimd::select(value < V(-1), V(-1), xsimd::select(value > V(1), V(1), value));
        }
    }
};

struct scalar_conversion {
    operator float() const noexcept(false) { return 1; }
};
struct converts_scalar {
    backend::batch<float> operator()(backend::batch<float> value) const noexcept { return value; }
    scalar_conversion operator()(const float &) const noexcept { return {}; }
};
static_assert(!noexcept(backend::transform_to<float>(nullptr, nullptr, 0, converts_scalar{})));

    #if !defined(WJR_DISABLE_EXCEPTIONS)
struct throwing_increment {
    std::size_t calls = 0;
    std::size_t fail_at;
    template <class V, class... Rest>
    auto operator()(V value, Rest... rest) {
        if (calls++ == fail_at) {
            throw std::runtime_error("operation failed");
        }
        return (value + ... + rest) + V(1);
    }
};
    #endif
} // namespace

TEST(simd_transform, bounded_tails_and_exact_in_place) {
    check_transform<std::uint8_t>();
    check_transform<std::uint16_t>();
    check_transform<std::uint32_t>();
    check_transform<std::uint64_t>();
    check_transform<float>();
    check_transform<double>();
}

TEST(simd_transform, same_noncopyable_operation_and_empty_null_buffers) {
    counted_increment operation;
    backend::transform_to<float>(nullptr, nullptr, 0, operation);
    EXPECT_EQ(operation.batches, 0u);
    EXPECT_EQ(operation.scalars, 0u);
    std::size_t binary_calls = 0;
    auto binary = [&binary_calls](auto lhs, auto rhs) {
        ++binary_calls;
        return lhs + rhs;
    };
    backend::transform_to<float>(nullptr, nullptr, nullptr, 0, binary);
    EXPECT_EQ(binary_calls, 0u);
    constexpr auto lanes = backend::batch<float>::size;
    std::array<float, 2 * lanes + 1> src{};
    std::array<float, 2 * lanes + 1> dst{};
    backend::transform_to(dst.data(), src.data(), src.size(), operation);
    EXPECT_EQ(operation.batches, 2u);
    EXPECT_EQ(operation.scalars, 1u);
    EXPECT_TRUE(std::all_of(dst.begin(), dst.end(), [](float value) { return value == 1; }));
}

template <class T>
void check_floating_transform() {
    constexpr auto lanes = backend::batch<T>::size;
    const std::array<T, 7> values = {std::numeric_limits<T>::quiet_NaN(),
                                     std::numeric_limits<T>::infinity(),
                                     -std::numeric_limits<T>::infinity(),
                                     T(0),
                                     -T(0),
                                     T(0.25),
                                     T(-0.5)};
    std::array<T, 3 * lanes + 1> src{};
    std::array<T, 3 * lanes + 1> dst{};
    for (std::size_t phase = 0; phase < values.size(); ++phase) {
        for (std::size_t i = 0; i < src.size(); ++i) {
            src[i] = values[(i + phase) % values.size()];
        }
        backend::transform_to(dst.data(), src.data(), src.size(), clip{});
        for (std::size_t i = 0; i < src.size(); ++i) {
            const auto expected = clip{}(src[i]);
            if (std::isnan(expected)) {
                EXPECT_TRUE(std::isnan(dst[i]));
            } else {
                EXPECT_EQ(dst[i], expected);
                if (expected == 0) {
                    EXPECT_EQ(std::signbit(dst[i]), std::signbit(expected));
                }
            }
        }
    }
}

TEST(simd_transform, floating_nan_infinities_and_signed_zero) {
    check_floating_transform<float>();
    check_floating_transform<double>();
}

    #if !defined(WJR_DISABLE_EXCEPTIONS)
TEST(simd_transform, throwing_operation_preserves_completed_prefix) {
    constexpr auto lanes = backend::batch<float>::size;
    std::array<float, 2 * lanes + 1> src{};
    std::array<float, 2 * lanes + 1> dst{};
    for (bool binary : {false, true}) {
        for (std::size_t fail_at = 0; fail_at < 3; ++fail_at) {
            dst.fill(-1);
            throwing_increment operation{0, fail_at};
            if (binary) {
                EXPECT_THROW(backend::transform_to(dst.data(), src.data(), src.data(), src.size(),
                                                   operation),
                             std::runtime_error);
            } else {
                EXPECT_THROW(backend::transform_to(dst.data(), src.data(), src.size(), operation),
                             std::runtime_error);
            }
            for (std::size_t i = 0; i < dst.size(); ++i) {
                EXPECT_EQ(dst[i], i < fail_at * lanes ? 1 : -1);
            }
        }
    }
}
    #endif

#endif // WJR_TEST_XSIMD
