#include <list>
#include <vector>

#include <wjr/format/fastfloat.hpp>
#include <wjr/vector.hpp>

namespace {

template <typename Value, typename Converter = wjr::char_converter_t>
concept integer_conversion = requires(char *out, const char *in, Value value, Converter conv) {
    wjr::to_chars(out, out, value, 10, conv);
    wjr::from_chars(in, in, value, 10, conv);
};

static_assert(integer_conversion<int8_t> && integer_conversion<uint8_t> &&
              integer_conversion<int16_t> && integer_conversion<uint16_t> &&
              integer_conversion<int32_t> && integer_conversion<uint32_t> &&
              integer_conversion<int64_t> && integer_conversion<uint64_t>);
static_assert(!integer_conversion<bool> && !integer_conversion<float> &&
              !integer_conversion<void *>);
static_assert(integer_conversion<uint64_t, wjr::origin_converter_t>);
static_assert(!integer_conversion<int64_t, wjr::origin_converter_t>);
static_assert(!integer_conversion<uint64_t, int>);

template <typename Iter>
concept fast_conversion = requires(Iter it, uint64_t value) {
    wjr::to_chars_backward_unchecked(it, value, 10);
    wjr::from_chars_unchecked(it, it, value, 10);
};

static_assert(fast_conversion<char *> && fast_conversion<uint8_t *> &&
              fast_conversion<std::vector<char>::iterator>);
static_assert(!fast_conversion<int *> && !fast_conversion<std::list<char>::iterator> &&
              !fast_conversion<std::back_insert_iterator<std::vector<char>>>);
static_assert(wjr::charconv_detail::fast_convert_iterator<std::move_iterator<char *>>);
static_assert(wjr::charconv_detail::fast_convert_iterator<const char *>);

template <typename Base>
concept dynamic_base = requires(char *out, Base base) { wjr::to_chars(out, out, 42, base); };
static_assert(dynamic_base<unsigned> && dynamic_base<int>);
static_assert(!dynamic_base<bool> && !dynamic_base<double>);

template <unsigned Base>
concept digit_block = requires(const char *input) {
    wjr::check_eight_digits<Base>(input);
    wjr::check_eight_origin_digits<Base>(input);
};
static_assert(digit_block<0> && digit_block<2> && digit_block<16> && !digit_block<17>);

template <typename T>
concept float_conversion = requires(const char *input, T value) {
    wjr::fastfloat::from_chars(input, input, value, wjr::chars_format::general);
};
static_assert(float_conversion<float> && float_conversion<double>);
static_assert(!float_conversion<long double> && !float_conversion<int>);

static_assert(wjr::charconv_detail::is_fast_container_inserter_v<
                  std::back_insert_iterator<wjr::vector<char>>> == 2);
static_assert(wjr::charconv_detail::is_fast_container_inserter_v<
                  std::back_insert_iterator<wjr::vector<int>>> == 0);
static_assert(wjr::charconv_detail::is_fast_container_inserter_v<
                  std::back_insert_iterator<std::vector<char>>> == 1);

} // namespace
