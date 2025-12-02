#ifndef WJR_UNITS_DETAIL_HPP__
#define WJR_UNITS_DETAIL_HPP__

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <functional>
#include <random>

#include <wjr/assert.hpp>

static std::mt19937_64 __mt_rand(time(0));
static auto mt_rand = std::ref(__mt_rand);

#define WJR_TESTS_NOT_OPT(x) (*(volatile decltype(x) *)&(x))

// Compatibility macros for doctest-style assertions
#define CHECK_EQ(a, b) CHECK((a) == (b))
#define CHECK_NE(a, b) CHECK((a) != (b))
#define CHECK_LT(a, b) CHECK((a) < (b))
#define CHECK_LE(a, b) CHECK((a) <= (b))
#define CHECK_GT(a, b) CHECK((a) > (b))
#define CHECK_GE(a, b) CHECK((a) >= (b))
// CHECK_FALSE and REQUIRE_FALSE are already defined by Catch2

#define REQUIRE_EQ(a, b) REQUIRE((a) == (b))
#define REQUIRE_NE(a, b) REQUIRE((a) != (b))
#define REQUIRE_LT(a, b) REQUIRE((a) < (b))
#define REQUIRE_LE(a, b) REQUIRE((a) <= (b))
#define REQUIRE_GT(a, b) REQUIRE((a) > (b))
#define REQUIRE_GE(a, b) REQUIRE((a) >= (b))

#endif // WJR_UNITS_DETAIL_HPP__