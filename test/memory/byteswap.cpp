#include <gtest/gtest.h>

#include <wjr/memory/detail.hpp>

namespace {

template <typename T, typename = void>
struct has_byteswap : std::false_type {};

template <typename T>
struct has_byteswap<T, std::void_t<decltype(wjr::byteswap(std::declval<T>()))>> : std::true_type {};

enum class unsigned_enum : unsigned {};

static_assert(has_byteswap<uint8_t>::value);
static_assert(has_byteswap<uint16_t>::value);
static_assert(has_byteswap<uint32_t>::value);
static_assert(has_byteswap<uint64_t>::value);
static_assert(!has_byteswap<bool>::value);
static_assert(!has_byteswap<int>::value);
static_assert(!has_byteswap<double>::value);
static_assert(!has_byteswap<unsigned_enum>::value);
static_assert(std::is_same_v<decltype(wjr::byteswap(uint8_t{})), uint8_t>);
static_assert(std::is_same_v<decltype(wjr::byteswap(uint16_t{})), uint16_t>);
static_assert(std::is_same_v<decltype(wjr::byteswap(uint32_t{})), uint32_t>);
static_assert(std::is_same_v<decltype(wjr::byteswap(uint64_t{})), uint64_t>);
static_assert(noexcept(wjr::byteswap(uint64_t{})));

#if defined(WJR_CPP_20)
static_assert(wjr::byteswap(uint8_t{0xab}) == uint8_t{0xab});
static_assert(wjr::byteswap(uint16_t{0x0123}) == uint16_t{0x2301});
static_assert(wjr::byteswap(uint32_t{0x01234567}) == uint32_t{0x67452301});
static_assert(wjr::byteswap(uint64_t{0x0123456789abcdef}) == uint64_t{0xefcdab8967452301});
static_assert(wjr::byteswap(UINT64_MAX) == UINT64_MAX);
static_assert(wjr::byteswap(uint64_t{0}) == 0);
#endif

template <typename T>
void check_byteswap(T value, T expected) {
    // Keep the runtime path distinct from constant folding.
    volatile T input = value;
    EXPECT_EQ(wjr::byteswap(input), expected);
}

} // namespace

TEST(byteswap, byte_values) {
    for (unsigned value = 0; value <= UINT8_MAX; ++value) {
        check_byteswap(static_cast<uint8_t>(value), static_cast<uint8_t>(value));
    }
}

TEST(byteswap, multi_byte_values) {
    check_byteswap(uint16_t{0}, uint16_t{0});
    check_byteswap(uint16_t{UINT16_MAX}, uint16_t{UINT16_MAX});
    check_byteswap(uint16_t{1}, uint16_t{0x0100});
    check_byteswap(uint16_t{0x0123}, uint16_t{0x2301});
    check_byteswap(uint32_t{0}, uint32_t{0});
    check_byteswap(uint32_t{UINT32_MAX}, uint32_t{UINT32_MAX});
    check_byteswap(uint32_t{1}, uint32_t{0x01000000});
    check_byteswap(uint32_t{0x01234567}, uint32_t{0x67452301});
    check_byteswap(uint64_t{0}, uint64_t{0});
    check_byteswap(uint64_t{UINT64_MAX}, uint64_t{UINT64_MAX});
    check_byteswap(uint64_t{1}, uint64_t{0x0100000000000000});
    check_byteswap(uint64_t{0x0123456789abcdef}, uint64_t{0xefcdab8967452301});
}
