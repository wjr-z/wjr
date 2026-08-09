#include <gtest/gtest.h>

#include <string>
#include <utility>

#include <wjr/type_traits.hpp>

using namespace wjr;

namespace {

struct base {};
struct derived : base {};
struct not_swappable {
    not_swappable() = delete;
    not_swappable(const not_swappable &) = delete;
    not_swappable(not_swappable &&) = delete;
    not_swappable &operator=(const not_swappable &) = delete;
    not_swappable &operator=(not_swappable &&) = delete;
    ~not_swappable() = default;
};

static_assert(in_place_max.operator int() == std::numeric_limits<int>::max());
static_assert(in_place_min.operator int() == std::numeric_limits<int>::min());
static_assert(is_void_or_v<void, std::false_type>);
static_assert(!is_void_or_v<int, std::false_type>);
static_assert(
    std::is_same_v<multi_conditional_t<std::false_type, int, std::true_type, long, char>, long>);
static_assert(is_any_of_v<int, char, int, long>);
static_assert(!is_any_of_v<int, char, long>);
static_assert(std::is_same_v<remove_cvref_t<const int &>, int>);
static_assert(std::is_same_v<uint_t<32>, uint32_t>);
static_assert(std::is_same_v<int_t<64>, int64_t>);
static_assert(std::is_same_v<usint_t<32, true>, int32_t>);
static_assert(std::is_same_v<usint_t<32, false>, uint32_t>);
static_assert(is_nonbool_integral_v<int> && !is_nonbool_integral_v<bool>);
static_assert(is_unsigned_integral_v<unsigned> && !is_unsigned_integral_v<int>);
static_assert(is_signed_integral_v<int> && !is_signed_integral_v<unsigned>);
static_assert(is_nonbool_unsigned_integral_v<unsigned>);
static_assert(is_nonbool_signed_integral_v<int>);
static_assert(to_u8('A') == uint8_t('A'));
static_assert(to_char(uint8_t('A')) == 'A');
static_assert(std::is_same_v<add_restrict_t<int *>, int * WJR_RESTRICT>);
static_assert(is_swappable_v<int> && !is_swappable_v<not_swappable>);
static_assert(std::is_same_v<unref_wrapper_t<std::reference_wrapper<int>>, int &>);
static_assert(is_default_convertible_v<int>);
static_assert(get_place_index_v<std::in_place_index_t<3>> == 3);
static_assert(is_derived_from_v<derived, base>);
static_assert(!is_derived_from_v<base, derived>);
static_assert(is_convertible_to_v<int, long>);
static_assert(!is_convertible_to_v<std::string, int>);
static_assert(to_signed(uint32_t(3)) == 3);
static_assert(to_unsigned(int32_t(-1)) == UINT32_MAX);
static_assert(cmp_equal(-1, -1));
static_assert(!cmp_equal(-1, uint32_t(1)));
static_assert(cmp_less(-1, uint32_t(1)));
static_assert(cmp_greater(uint32_t(1), -1));
static_assert(cmp_less_equal(-1, uint32_t(1)));
static_assert(cmp_greater_equal(uint32_t(1), -1));
static_assert(in_range<int8_t>(int16_t(127)));
static_assert(!in_range<int8_t>(int16_t(128)));
static_assert(in_range<uint8_t>(int16_t(127)));
static_assert(!in_range<uint8_t>(int16_t(-1)));
static_assert(is_value_preserving_v<uint8_t, uint16_t>);
static_assert(!is_value_preserving_v<uint16_t, uint8_t>);
static_assert(is_value_preserving_or_int_v<int, uint8_t>);

} // namespace

TEST(type_traits, runtime_conversions) {
    EXPECT_EQ(static_cast<uint64_t>(in_place_max), UINT64_MAX);
    EXPECT_EQ(static_cast<int64_t>(in_place_min), INT64_MIN);
    EXPECT_EQ(to_u8(uint8_t(0x7f)), uint8_t(0x7f));
    EXPECT_EQ(to_char(uint8_t('w')), 'w');
    EXPECT_TRUE(cmp_not_equal(1, 2));
    EXPECT_TRUE(cmp_less_equal(1, 1));
    EXPECT_TRUE(cmp_greater_equal(2, 1));
}