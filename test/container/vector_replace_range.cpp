#include <wjr/vector.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <memory>
#include <ranges>
#include <span>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace {

template <typename T, typename Range>
concept replaceable = requires(wjr::vector<T> &values, Range &&range) {
    {
        values.replace_range(values.cbegin(), values.cend(), std::forward<Range>(range))
    } -> std::same_as<wjr::vector<T> &>;
};
static_assert(replaceable<int, std::span<const int>>);
static_assert(!replaceable<int, std::array<void *, 2> &>);
static_assert(!replaceable<std::unique_ptr<int>, std::vector<std::unique_ptr<int>> &>);

TEST(vector_replace_range, counted_ranges_match_sequence_replacement) {
    const std::array source{10, 11, 12, 13, 14, 15, 16};
    for (std::size_t size = 0; size != 7; ++size) {
        for (std::size_t first = 0; first <= size; ++first) {
            for (std::size_t last = first; last <= size; ++last) {
                for (std::size_t count = 0; count <= source.size(); ++count) {
                    for (const bool reserve : {false, true}) {
                        SCOPED_TRACE(testing::Message() << size << "," << first << "," << last
                                                        << "," << count << "," << reserve);
                        wjr::vector<int> actual;
                        if (reserve)
                            actual.reserve(32);
                        for (std::size_t i = 0; i != size; ++i)
                            actual.push_back(static_cast<int>(i));
                        std::vector<int> expected(actual.begin(), actual.end());
                        expected.erase(expected.begin() + first, expected.begin() + last);
                        expected.insert(expected.begin() + first, source.begin(),
                                        source.begin() + count);
                        auto &result =
                            actual.replace_range(actual.cbegin() + first, actual.cbegin() + last,
                                                 std::span(source).first(count));
                        EXPECT_EQ(&result, &actual);
                        EXPECT_TRUE(std::ranges::equal(actual, expected));
                    }
                }
            }
        }
    }
}

TEST(vector_replace_range, unsized_noncommon_forward_view) {
    auto source =
        std::views::iota(10, 20) | std::views::take_while([](int value) { return value < 15; });
    static_assert(std::ranges::forward_range<decltype(source)>);
    static_assert(!std::ranges::common_range<decltype(source)>);
    static_assert(!std::ranges::sized_range<decltype(source)>);
    wjr::vector<int> values{0, 1, 2, 3};
    values.replace_range(values.cbegin() + 1, values.cbegin() + 3, source);
    EXPECT_TRUE(std::ranges::equal(values, std::array{0, 10, 11, 12, 13, 14, 3}));
}

TEST(vector_replace_range, single_pass_stream_preserves_suffix_for_all_lengths) {
    for (const auto &item : {std::pair{"", std::vector<int>{0, 3}},
                             {"10", std::vector<int>{0, 10, 3}},
                             {"10 11", std::vector<int>{0, 10, 11, 3}},
                             {"10 11 12 13", std::vector<int>{0, 10, 11, 12, 13, 3}}}) {
        for (const bool reserve : {false, true}) {
            wjr::vector<int> values{0, 1, 2, 3};
            if (reserve)
                values.reserve(16);
            std::istringstream stream(item.first);
            auto source = std::ranges::istream_view<int>(stream);
            static_assert(!std::copy_constructible<std::ranges::iterator_t<decltype(source)>>);
            values.replace_range(values.cbegin() + 1, values.cbegin() + 3, source);
            EXPECT_TRUE(std::ranges::equal(values, item.second));
        }
    }
}

TEST(vector_replace_range, exact_or_empty_input_does_not_move_the_untouched_suffix_to_itself) {
    wjr::vector<std::string> values{"first", "middle", "last"};
    std::istringstream empty;
    values.replace_range(values.cbegin() + 1, values.cbegin() + 1,
                         std::ranges::istream_view<std::string>(empty));
    EXPECT_TRUE(std::ranges::equal(values, std::array<std::string, 3>{"first", "middle", "last"}));
    std::istringstream one("new");
    values.replace_range(values.cbegin() + 1, values.cbegin() + 2,
                         std::ranges::istream_view<std::string>(one));
    EXPECT_TRUE(std::ranges::equal(values, std::array<std::string, 3>{"first", "new", "last"}));
}

TEST(vector_replace_range, moving_and_proxy_sources) {
    std::vector<std::unique_ptr<int>> source;
    for (int value : {10, 11, 12})
        source.push_back(std::make_unique<int>(value));
    wjr::vector<std::unique_ptr<int>> values;
    for (int value : {0, 1, 2})
        values.push_back(std::make_unique<int>(value));
    auto moving =
        source | std::views::transform([](auto &value) -> auto && { return std::move(value); });
    values.replace_range(values.cbegin() + 1, values.cbegin() + 2, moving);
    ASSERT_EQ(values.size(), 5);
    const std::array expected{0, 10, 11, 12, 2};
    for (std::size_t i = 0; i != expected.size(); ++i)
        EXPECT_EQ(*values[i], expected[i]);
    for (const auto &value : source)
        EXPECT_EQ(value, nullptr);

    const std::vector<bool> bits{true, false, true};
    wjr::vector<int> numbers{7, 8, 9};
    numbers.replace_range(numbers.cbegin() + 1, numbers.cbegin() + 2, bits);
    EXPECT_TRUE(std::ranges::equal(numbers, std::array{7, 1, 0, 1, 9}));
}

TEST(vector_replace_range, inplace_and_small_storage_reuse_capacity) {
    const std::array source{10, 11, 12, 13};
    wjr::inplace_vector<int, 8> inplace{0, 1, 2, 3};
    auto *const inplace_data = inplace.data();
    inplace.replace_range(inplace.cbegin() + 1, inplace.cbegin() + 3, source);
    EXPECT_EQ(inplace.data(), inplace_data);
    EXPECT_TRUE(std::ranges::equal(inplace, std::array{0, 10, 11, 12, 13, 3}));
    wjr::small_vector<int, 8> small{0, 1, 2, 3};
    auto *const small_data = small.data();
    small.replace_range(small.cbegin(), small.cbegin() + 3, std::span(source).first(1));
    EXPECT_EQ(small.data(), small_data);
    EXPECT_TRUE(std::ranges::equal(small, std::array{10, 3}));
}

} // namespace
