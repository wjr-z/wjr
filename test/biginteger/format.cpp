#include <gtest/gtest.h>

#include <array>
#include <iomanip>
#include <limits>
#include <sstream>
#include <wjr/biginteger.hpp>

namespace {

struct bigint_format_traits : std::char_traits<char> {};

template <typename Traits>
std::string stream_bigint(const wjr::biginteger &value, std::ios_base::fmtflags base,
                          bool showpos = false) {
    std::basic_ostringstream<char, Traits> stream;
    stream.setf(base, std::ios_base::basefield);
    stream << std::showbase;
    if (showpos) {
        stream << std::showpos;
    }
    stream << value;
    const auto text = stream.str();
    return {text.data(), text.size()};
}

} // namespace

TEST(biginteger_format, fmt_integer_contract) {
    constexpr auto formats = std::to_array<const char *>({
        "{}",      "{:-}",     "{:+}",     "{: }",     "{:d}",    "{:b}",      "{:o}",   "{:x}",
        "{:#b}",   "{:#o}",    "{:#x}",    "{:+#x}",   "{: #o}",  "{:12}",     "{:012}", "{:+012}",
        "{: 012}", "{:#012x}", "{:#012o}", "{:#012b}", "{:>12}",  "{:<12}",    "{:^12}", "{:*>12}",
        "{:*<12}", "{:*^12}",  "{:>012}",  "{:<012}",  "{:^012}", "{:+#016x}",
    });
    for (const int64_t number :
         {int64_t{0}, int64_t{1}, int64_t{-1}, int64_t{255}, int64_t{-255},
          std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max()}) {
        const wjr::biginteger value(number);
        for (const auto *format : formats) {
            SCOPED_TRACE(format);
            SCOPED_TRACE(number);
            EXPECT_EQ(fmt::format(fmt::runtime(format), value),
                      fmt::format(fmt::runtime(format), number));
            EXPECT_EQ(value, number);
        }
    }
}

TEST(biginteger_format, dynamic_width_and_output_iterators) {
    const wjr::biginteger value(-255);
    for (const int width : {0, 3, 9, 24}) {
        EXPECT_EQ(fmt::format("{:{}}", value, width), fmt::format("{:{}}", -255, width));
        EXPECT_EQ(fmt::format("{:+#0{}x}", value, width), fmt::format("{:+#0{}x}", -255, width));
    }
    std::string output;
    fmt::format_to(std::back_inserter(output), "{:#012x}", value);
    EXPECT_EQ(output, fmt::format("{:#012x}", -255));
    std::array<char, 7> truncated{};
    const auto result = fmt::format_to_n(truncated.data(), truncated.size(), "{:#012x}", value);
    EXPECT_EQ(result.size, 12u);
    EXPECT_EQ(std::string(truncated.data(), truncated.size()), "-0x0000");
}

TEST(biginteger_format, multilimb_magnitude_and_storage_types) {
    wjr::biginteger value = (wjr::biginteger(1) << 128) - 1;
    value = -value;
    const std::string hex = "-0xffffffffffffffffffffffffffffffff";
    EXPECT_EQ(fmt::format("{:#x}", value), hex);
    EXPECT_EQ(fmt::format("{:#40x}", value), std::string(5, ' ') + hex);
    EXPECT_EQ(fmt::format("{:#040x}", value), "-0x00000ffffffffffffffffffffffffffffffff");
    EXPECT_EQ(stream_bigint<std::char_traits<char>>(value, std::ios_base::hex), hex);
    EXPECT_EQ(stream_bigint<bigint_format_traits>(value, std::ios_base::hex), hex);

    const wjr::inplace_biginteger<2> fixed(-255);
    EXPECT_EQ(fmt::format("{:+#012x}", fixed), fmt::format("{:+#012x}", -255));
    EXPECT_EQ(fmt::format("{:#x}", value), hex);
}

TEST(biginteger_format, stream_sign_before_base_prefix) {
    for (const auto base : {std::ios_base::dec, std::ios_base::oct, std::ios_base::hex}) {
        for (const bool showpos : {false, true}) {
            const std::string digits = base == std::ios_base::hex   ? "0xff"
                                       : base == std::ios_base::oct ? "0377"
                                                                    : "255";
            const wjr::biginteger negative(-255), positive(255);
            for (const auto *value : {&negative, &positive}) {
                const std::string sign = value->is_negate() ? "-" : showpos ? "+" : "";
                EXPECT_EQ(stream_bigint<std::char_traits<char>>(*value, base, showpos),
                          sign + digits);
                EXPECT_EQ(stream_bigint<bigint_format_traits>(*value, base, showpos),
                          sign + digits);
            }
        }
    }
}

TEST(biginteger_format, stream_padding_and_width_reset) {
    std::ostringstream stream;
    stream << std::hex << std::showbase << std::setfill('*') << std::setw(10)
           << wjr::biginteger(-255);
    EXPECT_EQ(stream.str(), "*****-0xff");
    EXPECT_EQ(stream.width(), 0);
    stream << wjr::biginteger(-1);
    EXPECT_EQ(stream.str(), "*****-0xff-0x1");
    stream.str("");
    stream << std::left << std::setw(10) << wjr::biginteger(-255);
    EXPECT_EQ(stream.str(), "-0xff*****");
}

TEST(biginteger_format, unsupported_presentations_are_rejected) {
    const wjr::biginteger value(65);
    for (const auto *format : {"{:X}", "{:B}", "{:c}", "{:L}", "{:f}", "{:.2}"}) {
        EXPECT_THROW((void)fmt::format(fmt::runtime(format), value), fmt::format_error);
    }
    EXPECT_THROW((void)fmt::format("{:{}}", value, -1), fmt::format_error);
}
