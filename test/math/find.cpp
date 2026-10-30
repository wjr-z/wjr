#include "../detail.hpp"

#include <array>

#include <wjr/math/find.hpp>

using namespace wjr;

namespace {
template <typename T>
constexpr bool constant_search() {
    std::array<T, 13> values{};
    std::array<T, 13> zeros{};
    std::array<T, 13> needles{};
    const T needle = static_cast<T>(-1);
    needles.fill(needle);
    values[3] = values[11] = needle;
    return find_n(values.data(), needle, values.size()) == 3 &&
           reverse_find_n(values.data(), needle, values.size()) == 12 &&
           find_not_n(values.data(), T{}, values.size()) == 3 &&
           reverse_find_not_n(values.data(), T{}, values.size()) == 12 &&
           find_n(values.data(), needles.data(), values.size()) == 3 &&
           reverse_find_n(values.data(), needles.data(), values.size()) == 12 &&
           find_not_n(values.data(), zeros.data(), values.size()) == 3 &&
           reverse_find_not_n(values.data(), zeros.data(), values.size()) == 12;
}

static_assert(constant_search<char>());
static_assert(constant_search<bool>());
static_assert(constant_search<int8_t>());
static_assert(constant_search<uint8_t>());
static_assert(constant_search<int16_t>());
static_assert(constant_search<uint16_t>());
static_assert(constant_search<int32_t>());
static_assert(constant_search<uint32_t>());
static_assert(constant_search<int64_t>());
static_assert(constant_search<uint64_t>());

template <typename T>
void check_search_boundaries() {
    const T needle = static_cast<T>(-1);
    std::array<T, 40> values{};
    std::array<T, 40> zeros{};
    std::array<T, 40> needles{};
    needles.fill(needle);
    for (size_t n = 0; n <= values.size(); ++n) {
        for (size_t pos = 0; pos <= n; ++pos) {
            SCOPED_TRACE(sizeof(T));
            SCOPED_TRACE(n);
            SCOPED_TRACE(pos);
            values.fill(T{});
            if (pos != n) {
                values[pos] = needle;
            }
            const auto reverse_pos = pos == n ? 0 : pos + 1;
            EXPECT_EQ(find_n(values.data(), needle, n), pos);
            EXPECT_EQ(reverse_find_n(values.data(), needle, n), reverse_pos);
            EXPECT_EQ(find_not_n(values.data(), T{}, n), pos);
            EXPECT_EQ(reverse_find_not_n(values.data(), T{}, n), reverse_pos);
            EXPECT_EQ(find_n(values.data(), needles.data(), n), pos);
            EXPECT_EQ(reverse_find_n(values.data(), needles.data(), n), reverse_pos);
            EXPECT_EQ(find_not_n(values.data(), zeros.data(), n), pos);
            EXPECT_EQ(reverse_find_not_n(values.data(), zeros.data(), n), reverse_pos);
        }
    }
}

} // namespace

TEST(math, find_integral_dispatch) {
    check_search_boundaries<char>();
    check_search_boundaries<bool>();
    check_search_boundaries<int8_t>();
    check_search_boundaries<uint8_t>();
    check_search_boundaries<int16_t>();
    check_search_boundaries<uint16_t>();
    check_search_boundaries<int32_t>();
    check_search_boundaries<uint32_t>();
    check_search_boundaries<int64_t>();
    check_search_boundaries<uint64_t>();
}
