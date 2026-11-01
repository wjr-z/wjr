#include <wjr/container/btree_map.hpp>

#include <utility>

#include <gtest/gtest.h>

namespace search_selection_test {
struct recursive_value;
struct disabled_complete_key {
    int value;
};
} // namespace search_selection_test

namespace wjr {
template <>
struct is_complete<search_selection_test::recursive_value> : std::false_type {};
template <>
struct is_complete<search_selection_test::disabled_complete_key> : std::false_type {};
} // namespace wjr

using recursive_pair = std::pair<const int, search_selection_test::recursive_value>;
static_assert(!wjr::btree_detail::is_inline_key_v<recursive_pair>);
static_assert(!wjr::btree_detail::is_inline_key_v<search_selection_test::recursive_value>);

namespace search_selection_test {
struct recursive_value {
    int value;
};
} // namespace search_selection_test

static_assert(!wjr::btree_detail::is_inline_key_v<search_selection_test::recursive_value>);
static_assert(!wjr::btree_detail::is_inline_key_v<recursive_pair>);
static_assert(std::is_trivial_v<search_selection_test::disabled_complete_key>);
static_assert(!wjr::btree_detail::is_inline_key_v<search_selection_test::disabled_complete_key>);
static_assert(wjr::btree_detail::is_inline_key<int, void>::value);
static_assert(!wjr::btree_detail::is_inline_key<int, int>::value);

namespace {
template <typename T>
concept defined_searcher = requires { sizeof(T); };

static_assert(defined_searcher<wjr::basic_btree_searcher_impl<16>>);
static_assert(
    std::same_as<wjr::basic_btree_searcher_impl<16>, wjr::basic_btree_searcher_impl<16, void>>);
static_assert(!defined_searcher<wjr::basic_btree_searcher_impl<16, int>>);
static_assert(sizeof(wjr::basic_btree_searcher_impl<16>) == 1);

template <unsigned N>
void check_positions() {
    for (unsigned size = 1; size <= N; ++size) {
        for (unsigned first = 0; first <= size; ++first) {
            bool bounded = true;
            const auto result = wjr::basic_btree_searcher_impl<N, void>::template search<1>(
                size, [&](unsigned index) {
                    bounded &= index < size;
                    return index >= first;
                });
            EXPECT_TRUE(bounded) << N << ':' << size << ':' << first;
            EXPECT_EQ(result, first) << N << ':' << size << ':' << first;
        }
    }
}

TEST(BtreeSearchSelection, EveryCapacityPartialNodeAndInsertionPosition) {
    []<size_t... I>(std::index_sequence<I...>) {
        (check_positions<I + 1>(), ...);
    }(std::make_index_sequence<64>{});
}
} // namespace
