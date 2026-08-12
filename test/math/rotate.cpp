#include <gtest/gtest.h>

#include <wjr/math/rotate.hpp>

#include <type_traits>
#include <utility>

using namespace wjr;

namespace {

template <typename T, typename = void>
struct has_rotl : std::false_type {};

template <typename T>
struct has_rotl<T, std::void_t<decltype(rotl(std::declval<T>(), 0))>> : std::true_type {};

template <typename T, typename = void>
struct has_rotr : std::false_type {};

template <typename T>
struct has_rotr<T, std::void_t<decltype(rotr(std::declval<T>(), 0))>> : std::true_type {};

} // namespace

TEST(rotate, type_constraints) {
    static_assert(has_rotl<uint32_t>::value);
    static_assert(has_rotr<uint32_t>::value);
    static_assert(!has_rotl<int32_t>::value);
    static_assert(!has_rotr<int32_t>::value);
}

TEST(rotate, uint8_boundaries) {
    constexpr uint8_t value = 0x96;

    static_assert(rotl(value, 0) == value);
    static_assert(rotl(value, 8) == value);
    static_assert(rotl(value, 9) == rotl(value, 1));
    static_assert(rotr(value, -1) == rotl(value, 1));

    EXPECT_EQ(rotl(value, 1), uint8_t(0x2d));
    EXPECT_EQ(rotr(value, 1), uint8_t(0x4b));
}

TEST(rotate, wider_values) {
    constexpr uint32_t value32 = 0x12345678U;
    constexpr uint64_t value64 = 0x0123456789abcdefULL;

    EXPECT_EQ(rotl(value32, 4), 0x23456781U);
    EXPECT_EQ(rotr(value32, 4), 0x81234567U);
    EXPECT_EQ(rotl(value64, 12), 0x3456789abcdef012ULL);
    EXPECT_EQ(rotr(value64, 12), 0xdef0123456789abcULL);
}