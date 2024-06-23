#ifndef WJR_MATH_INTEGRAL_CONSTANT_HPP__
#define WJR_MATH_INTEGRAL_CONSTANT_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename T, T val>
struct integral_constant {
    static constexpr T value = val;

    using value_type = T;
    using type = integral_constant;

    constexpr integral_constant() noexcept = default;
    constexpr integral_constant(const integral_constant &) noexcept = default;
    constexpr integral_constant(integral_constant &&) noexcept = default;
    constexpr integral_constant &operator=(const integral_constant &) noexcept = default;
    constexpr integral_constant &operator=(integral_constant &&) noexcept = default;
    ~integral_constant() noexcept = default;

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

namespace digits_literal_details {

template <uint64_t Base>
WJR_CONST WJR_INTRINSIC_CONSTEXPR static uint32_t __fast_conv_4(uint32_t val) noexcept {
    constexpr uint32_t Base2 = Base * Base;

    constexpr uint32_t mask = 0x00FF00FF;
    constexpr uint32_t mul = 1 + (Base2 << 16);
    val = (val * Base) + (val >> 8);
    val = ((val & mask) * mul) >> 16;
    return uint32_t(val);
}

template <uint64_t Base>
WJR_CONST WJR_INTRINSIC_CONSTEXPR static uint32_t __fast_conv_8(uint64_t val) noexcept {
    constexpr uint64_t Base2 = Base * Base;
    constexpr uint64_t Base4 = Base2 * Base2;
    constexpr uint64_t Base6 = Base4 * Base2;

    constexpr uint64_t mask = 0x000000FF000000FF;
    constexpr uint64_t mul1 = Base2 + (Base6 << 32);
    constexpr uint64_t mul2 = 1 + (Base4 << 32);

    val = (val * Base) + (val >> 8);
    val = (((val & mask) * mul1) + (((val >> 16) & mask) * mul2)) >> 32;
    return val;
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

} // namespace digits_literal_details

#define WJR_REGISTER_INTEGRAL_LITERAL(NAME, TYPE)                                        \
    template <char... Chars>                                                             \
    WJR_CONST WJR_INTRINSIC_CONSTEXPR auto operator"" _##NAME() noexcept                 \
        -> integral_constant<TYPE, digits_literal_details::parse<TYPE, Chars...>()> {    \
        return {};                                                                       \
    }

WJR_REGISTER_INTEGRAL_LITERAL(u, unsigned int);
WJR_REGISTER_INTEGRAL_LITERAL(ul, unsigned long);
WJR_REGISTER_INTEGRAL_LITERAL(ull, unsigned long long);
WJR_REGISTER_INTEGRAL_LITERAL(i, int);
WJR_REGISTER_INTEGRAL_LITERAL(l, long);
WJR_REGISTER_INTEGRAL_LITERAL(ll, long long);

WJR_REGISTER_INTEGRAL_LITERAL(i8, int8_t);
WJR_REGISTER_INTEGRAL_LITERAL(i16, int16_t);
WJR_REGISTER_INTEGRAL_LITERAL(i32, int32_t);
WJR_REGISTER_INTEGRAL_LITERAL(i64, int64_t);
WJR_REGISTER_INTEGRAL_LITERAL(u8, uint8_t);
WJR_REGISTER_INTEGRAL_LITERAL(u16, uint16_t);
WJR_REGISTER_INTEGRAL_LITERAL(u32, uint32_t);
WJR_REGISTER_INTEGRAL_LITERAL(u64, uint64_t);

WJR_REGISTER_INTEGRAL_LITERAL(zu, size_t);
WJR_REGISTER_INTEGRAL_LITERAL(z, ssize_t);

#undef WJR_REGISTER_INTEGRAL_LITERAL

} // namespace wjr

#endif // WJR_MATH_INTEGRAL_CONSTANT_HPP__