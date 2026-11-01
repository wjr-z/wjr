#include <wjr/json/document.hpp>

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <gtest/gtest.h>

namespace {

using document = wjr::json::document;

/** @brief Make the returned value observable independently of exception checks. */
template <typename T>
WJR_NOINLINE void consume_conversion(const document &value) {
    volatile T converted = static_cast<T>(value);
    (void)converted;
}

/** @brief Discarding a checked conversion must still perform its validation. */
template <typename T>
WJR_NOINLINE void discard_conversion(const document &value) {
    (void)static_cast<T>(value);
}

template <typename T>
class json_numeric_conversion : public testing::Test {};

using arithmetic_types =
    testing::Types<signed char, unsigned char, short, unsigned short, int, unsigned int,
                   std::int64_t, std::uint64_t, float, double, long double>;
TYPED_TEST_SUITE(json_numeric_conversion, arithmetic_types);

TYPED_TEST(json_numeric_conversion, valid_number_kinds) {
    const document signed_value(-7);
    const document unsigned_value(13u);
    const document float_value(2.5);
    EXPECT_EQ(static_cast<TypeParam>(signed_value), static_cast<TypeParam>(-7));
    EXPECT_EQ(static_cast<TypeParam>(unsigned_value), static_cast<TypeParam>(13));
    EXPECT_EQ(static_cast<TypeParam>(float_value), static_cast<TypeParam>(2.5));
    consume_conversion<TypeParam>(signed_value);
    discard_conversion<TypeParam>(signed_value);
}

TEST(json_scalar_conversion, valid_boolean_and_null) {
    const document yes(true), no(false), null(nullptr);
    EXPECT_TRUE(static_cast<bool>(yes));
    EXPECT_FALSE(static_cast<bool>(no));
    EXPECT_EQ(static_cast<std::nullptr_t>(null), nullptr);
    consume_conversion<bool>(yes);
    discard_conversion<bool>(yes);
    consume_conversion<std::nullptr_t>(null);
    discard_conversion<std::nullptr_t>(null);
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
TYPED_TEST(json_numeric_conversion, invalid_type_used) {
    const document wrong(nullptr);
    EXPECT_THROW(
        {
            volatile TypeParam converted = static_cast<TypeParam>(wrong);
            (void)converted;
        },
        wjr::bad_expected_access<wjr::json::error_code>);
}

TYPED_TEST(json_numeric_conversion, invalid_type_discarded) {
    const document wrong(nullptr);
    EXPECT_THROW((void)static_cast<TypeParam>(wrong),
                 wjr::bad_expected_access<wjr::json::error_code>);
}

TEST(json_scalar_conversion, invalid_boolean_used) {
    const document wrong(7);
    EXPECT_THROW(
        {
            volatile bool converted = static_cast<bool>(wrong);
            (void)converted;
        },
        wjr::json::bad_document_access);
}

TEST(json_scalar_conversion, invalid_boolean_discarded) {
    const document wrong(7);
    EXPECT_THROW((void)static_cast<bool>(wrong), wjr::json::bad_document_access);
}

TEST(json_scalar_conversion, invalid_null_used) {
    const document wrong(7);
    EXPECT_THROW(
        {
            volatile std::nullptr_t converted = static_cast<std::nullptr_t>(wrong);
            (void)converted;
        },
        wjr::json::bad_document_access);
}

TEST(json_scalar_conversion, invalid_null_discarded) {
    const document wrong(7);
    EXPECT_THROW((void)static_cast<std::nullptr_t>(wrong), wjr::json::bad_document_access);
}
#endif

} // namespace
