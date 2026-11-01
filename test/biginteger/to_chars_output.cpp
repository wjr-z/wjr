#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <deque>
#include <iterator>
#include <list>
#include <string>
#include <vector>

#include <wjr/biginteger/detail/convert.hpp>

namespace {

template <typename Converter>
void check_power_of_two_output(Converter converter) {
    constexpr std::size_t max_limbs = 7;
    constexpr std::size_t guard_size = 16;
    constexpr uint8_t canary = 0xa5;
    const std::string prefix = "prefix:";

    for (std::size_t count = 2; count <= max_limbs; ++count) {
        for (unsigned int base : {2U, 4U, 8U, 16U, 32U}) {
            const auto bits = std::countr_zero(base);
            for (unsigned int high_bits : {1U, 2U, 3U, 4U, 5U, 31U, 32U, 63U, 64U}) {
                SCOPED_TRACE(::testing::Message() << "limbs=" << count << " base=" << base
                                                  << " high_bits=" << high_bits);
                std::array<uint64_t, max_limbs> input{};
                input[count - 1] = uint64_t{1} << (high_bits - 1);
                const auto bit_length = (count - 1) * 64 + high_bits;
                const auto length = (bit_length + bits - 1) / bits;
                const auto capacity = (count * 64 + bits - 1) / bits;
                std::vector<uint8_t> expected(length, converter.to(0));
                expected.front() = converter.to(1U << ((bit_length - 1) % bits));

                alignas(16) std::array<uint8_t, max_limbs * 64 + 2 * guard_size> guarded;
                guarded.fill(canary);
                auto *const first = guarded.data() + guard_size;
                auto *const last =
                    wjr::biginteger_to_chars(first, input.data(), count, base, converter);
                ASSERT_EQ(static_cast<std::size_t>(last - first), length);
                EXPECT_TRUE(std::equal(expected.begin(), expected.end(), first));
                EXPECT_TRUE(std::all_of(guarded.begin(), guarded.begin() + guard_size,
                                        [](uint8_t value) { return value == canary; }));
                EXPECT_TRUE(std::all_of(guarded.begin() + guard_size + capacity, guarded.end(),
                                        [](uint8_t value) { return value == canary; }));

                std::string text = prefix;
                text.reserve(prefix.size() + capacity);
                wjr::biginteger_to_chars(std::back_inserter(text), input.data(), count, base,
                                         converter);
                ASSERT_EQ(text.size(), prefix.size() + length);
                EXPECT_EQ(text.substr(0, prefix.size()), prefix);
                EXPECT_TRUE(
                    std::equal(expected.begin(), expected.end(), text.begin() + prefix.size()));

                std::list<uint8_t> output{canary};
                wjr::biginteger_to_chars(std::back_inserter(output), input.data(), count, base,
                                         converter);
                ASSERT_EQ(output.size(), length + 1);
                EXPECT_EQ(output.front(), canary);
                EXPECT_TRUE(
                    std::equal(expected.begin(), expected.end(), std::next(output.begin())));
            }
        }
    }
}

static_assert(wjr::charconv_detail::is_fast_container_inserter_v<
                  std::back_insert_iterator<std::list<uint8_t>>> == 0);
static_assert(
    sizeof(wjr::charconv_detail::fast_buffer_t<std::back_insert_iterator<std::list<uint32_t>>>) ==
    1);

template <typename Converter>
void check_generic_output(Converter converter) {
    const std::string prefix = "prefix:";
    for (const std::size_t count : {2, 64, 256, 1024}) {
        std::vector<uint64_t> input(count);
        for (std::size_t i = 0; i != count; ++i) {
            input[i] = 0x9e3779b97f4a7c15ULL * (i + 1);
        }
        input.back() |= uint64_t{1} << 63;
        const auto original = input;

        for (const unsigned int base : {2, 4, 8, 10, 16, 32}) {
            SCOPED_TRACE(::testing::Message() << "limbs=" << count << " base=" << base);
            std::vector<uint8_t> expected(count * 64);
            const auto end =
                wjr::biginteger_to_chars(expected.data(), input.data(), count, base, converter);
            expected.resize(static_cast<std::size_t>(end - expected.data()));

            std::deque<uint8_t> bytes{0xa5};
            wjr::biginteger_to_chars(std::back_inserter(bytes), input.data(), count, base,
                                     converter);
            ASSERT_EQ(bytes.size(), expected.size() + 1);
            EXPECT_EQ(bytes.front(), 0xa5);
            EXPECT_TRUE(std::equal(expected.begin(), expected.end(), std::next(bytes.begin())));

            std::vector<uint32_t> wide{0x12345678};
            wjr::biginteger_to_chars(std::back_inserter(wide), input.data(), count, base,
                                     converter);
            ASSERT_EQ(wide.size(), expected.size() + 1);
            EXPECT_EQ(wide.front(), 0x12345678);
            EXPECT_TRUE(std::equal(expected.begin(), expected.end(), std::next(wide.begin())));

            std::string text = prefix;
            wjr::biginteger_to_chars(std::back_inserter(text), input.data(), count, base,
                                     converter);
            ASSERT_EQ(text.size(), prefix.size() + expected.size());
            EXPECT_EQ(text.substr(0, prefix.size()), prefix);
            EXPECT_TRUE(std::equal(expected.begin(), expected.end(), text.begin() + prefix.size()));
            EXPECT_EQ(input, original);
        }
    }
}

} // namespace

TEST(biginteger_to_chars_output, power_of_two_preserves_bounds_and_append_prefix) {
    check_power_of_two_output(wjr::char_converter_t{});
    check_power_of_two_output(wjr::origin_converter_t{});
}

TEST(biginteger_to_chars_output, generic_and_fast_output_agree_for_both_converters) {
    check_generic_output(wjr::char_converter_t{});
    check_generic_output(wjr::origin_converter_t{});
}
