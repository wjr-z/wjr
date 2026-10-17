#if defined(WJR_DEBUG_LEVEL)
    #undef WJR_DEBUG_LEVEL
#endif
#define WJR_DEBUG_LEVEL 3

#if defined(WJR_MINIMAL_ASSERT)
    #undef WJR_MINIMAL_ASSERT
#endif
#if defined(WJR_LIGHT_ASSERT)
    #undef WJR_LIGHT_ASSERT
#endif
#if defined(WJR_ASSERT_TEST_MINIMAL)
    #define WJR_MINIMAL_ASSERT
#elif defined(WJR_ASSERT_TEST_LIGHT)
    #define WJR_LIGHT_ASSERT
#endif

#include <wjr/assert.hpp>

#include <cstdlib>
#include <exception>

namespace {
int condition_calls = 0;
int diagnostic_calls = 0;

int diagnostic() noexcept { return ++diagnostic_calls; }

constexpr int checked_levels(int count) {
    WJR_ASSERT_L0(++count == 1, diagnostic());
    WJR_ASSERT_L1(++count == 2, diagnostic());
    WJR_ASSERT_L2(++count == 3, diagnostic());
    WJR_ASSERT_L3(++count == 4, diagnostic());
    WJR_ASSERT_ASSUME_L0(++count == 5, diagnostic());
    WJR_ASSERT_ASSUME_L1(++count == 6, diagnostic());
    WJR_ASSERT_ASSUME_L2(++count == 7, diagnostic());
    WJR_ASSERT_ASSUME_L3(++count == 8, diagnostic());
    return count;
}

static_assert(checked_levels(0) == 8);

#if defined(WJR_MINIMAL_ASSERT)
struct unprintable {};
constexpr int check_without_formatting(int value) {
    WJR_CHECK(value >= 0, unprintable{});
    return value;
}
static_assert(check_without_formatting(7) == 7);
#endif
} // namespace

int main(int argc, char **argv) {
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    const int expected = std::atoi(argv[1]);
    if (checked_levels(expected) != 8 || diagnostic_calls != 0) {
        return EXIT_FAILURE;
    }
    std::set_terminate([] {
#if defined(WJR_MINIMAL_ASSERT)
        constexpr int expected_diagnostics = 0;
#else
        constexpr int expected_diagnostics = 1;
#endif
        std::_Exit(condition_calls == 1 && diagnostic_calls == expected_diagnostics ? EXIT_SUCCESS
                                                                                    : EXIT_FAILURE);
    });
    WJR_CHECK(++condition_calls == expected, "diagnostic=", diagnostic());
    return EXIT_FAILURE;
}
