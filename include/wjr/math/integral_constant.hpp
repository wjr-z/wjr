#ifndef WJR_MATH_INTEGRAL_CONSTANT_HPP__
#define WJR_MATH_INTEGRAL_CONSTANT_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename T, T val>
struct integral_constant {
    static constexpr T value = val;

    using value_type = T;
    using type = integral_constant;

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(integral_constant);

    constexpr integral_constant(std::integral_constant<T, val>) noexcept {}

    constexpr operator value_type() const noexcept { return value; }
    WJR_NODISCARD constexpr value_type operator()() const noexcept { return value; }

    WJR_NODISCARD constexpr operator std::integral_constant<T, val>() const noexcept {
        return {};
    }

    template <typename U, U u>
    constexpr auto operator+(integral_constant<U, u>) const noexcept {
        constexpr auto result = value + u;
        return integral_constant<decltype(result), result>{};
    }

    template <typename U, U u>
    constexpr auto operator-(integral_constant<U, u>) const noexcept {
        constexpr auto result = value - u;
        return integral_constant<decltype(result), result>{};
    }
};

namespace digits_literal_detail {

template <uint64_t Base>
WJR_CONST WJR_INTRINSIC_CONSTEXPR static uint32_t __fast_conv_4(uint32_t val) noexcept {
    constexpr uint32_t Base2 = Base * Base;

    constexpr uint32_t mul1 = 1 + (Base << 8);
    constexpr uint32_t mul2 = 1 + (Base2 << 16);

    val = ((val * mul1) >> 8) & 0x00FF00FF;
    return (val * mul2) >> 16;
}

template <uint64_t Base>
WJR_CONST WJR_INTRINSIC_CONSTEXPR static uint32_t __fast_conv_8(uint64_t val) noexcept {
    constexpr uint64_t Base2 = Base * Base;
    constexpr uint64_t Base4 = Base2 * Base2;

    constexpr uint64_t mul1 = 1 + (Base << 8);
    constexpr uint64_t mul2 = 1 + (Base2 << 16);
    constexpr uint64_t mul3 = 1 + (Base4 << 32);

    val = ((val * mul1) >> 8) & 0x00FF00FF00FF00FF;
    val = ((val * mul2) >> 16) & 0x0000FFFF0000FFFF;
    return (val * mul3) >> 32;
}

template <typename T>
struct parse_result {
    T result;
    T pow;
};

template <typename T>
WJR_CONST constexpr parse_result<T> __parse_impl() noexcept {
    return {0, 1};
}

template <typename T, char c0>
WJR_CONST constexpr parse_result<T> __parse_impl() noexcept {
    return {c0 - '0', 10};
}

template <typename T, char c0, char c1>
WJR_CONST constexpr parse_result<T> __parse_impl() noexcept {
    constexpr T result = (c0 - '0') * 10 + c1 - '0';
    return {result, 100};
}

template <typename T, char c0, char c1, char c2>
WJR_CONST constexpr parse_result<T> __parse_impl() noexcept {
    constexpr T result = (c0 - '0') * 100 + (c1 - '0') * 10 + c2 - '0';
    return {result, 1000};
}

template <typename T, char c0, char c1, char c2, char c3, char... Chars>
WJR_CONST constexpr parse_result<T> __parse_impl() noexcept {
    constexpr uint32_t mem = (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24)) - 0x30303030;
    constexpr uint32_t val = __fast_conv_4<10>(mem);
    constexpr auto result = __parse_impl<T, Chars...>();
    return {static_cast<T>(result.result * result.pow + val),
            static_cast<T>(result.pow * 10000)};
}

template <typename T, char c0, char c1, char c2, char c3, char c4, char c5, char c6,
          char c7, char... Chars>
WJR_CONST constexpr parse_result<T> __parse_impl() noexcept {
    constexpr uint64_t mem =
        (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24) | ((uint64_t)c4 << 32) |
         ((uint64_t)c5 << 40) | ((uint64_t)c6 << 48) | ((uint64_t)c7 << 56)) -
        0x3030303030303030;
    constexpr uint64_t val = __fast_conv_8<10>(mem);
    constexpr auto result = __parse_impl<T, Chars...>();
    return {static_cast<T>(result.result * result.pow + val),
            static_cast<T>(result.pow * 100000000)};
}

template <typename T, char... Chars>
WJR_CONST constexpr T parse() noexcept {
    return __parse_impl<T, Chars...>().result;
}

} // namespace digits_literal_detail

#define WJR_REGISTER_INTEGRAL_LITERALS(NAME, TYPE)                                       \
    template <char... Chars>                                                             \
    WJR_CONST WJR_INTRINSIC_CONSTEXPR auto operator"" _##NAME() noexcept                 \
        -> integral_constant<TYPE, digits_literal_detail::parse<TYPE, Chars...>()> {     \
        return {};                                                                       \
    }

namespace literals {

WJR_REGISTER_INTEGRAL_LITERALS(u, unsigned int);
WJR_REGISTER_INTEGRAL_LITERALS(ul, unsigned long);
WJR_REGISTER_INTEGRAL_LITERALS(ull, unsigned long long);
WJR_REGISTER_INTEGRAL_LITERALS(i, int);
WJR_REGISTER_INTEGRAL_LITERALS(l, long);
WJR_REGISTER_INTEGRAL_LITERALS(ll, long long);

WJR_REGISTER_INTEGRAL_LITERALS(i8, int8_t);
WJR_REGISTER_INTEGRAL_LITERALS(i16, int16_t);
WJR_REGISTER_INTEGRAL_LITERALS(i32, int32_t);
WJR_REGISTER_INTEGRAL_LITERALS(i64, int64_t);
WJR_REGISTER_INTEGRAL_LITERALS(u8, uint8_t);
WJR_REGISTER_INTEGRAL_LITERALS(u16, uint16_t);
WJR_REGISTER_INTEGRAL_LITERALS(u32, uint32_t);
WJR_REGISTER_INTEGRAL_LITERALS(u64, uint64_t);

WJR_REGISTER_INTEGRAL_LITERALS(zu, size_t);
WJR_REGISTER_INTEGRAL_LITERALS(z, ssize_t);

#undef WJR_REGISTER_INTEGRAL_LITERALS

} // namespace literals

using namespace literals;

} // namespace wjr

#endif // WJR_MATH_INTEGRAL_CONSTANT_HPP__