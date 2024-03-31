#ifndef WJR_MATH_CONVERT_HPP__
#define WJR_MATH_CONVERT_HPP__

#include <array>
#include <system_error>

#include <wjr/assert.hpp>
#include <wjr/math/bit.hpp>
#include <wjr/math/broadcast.hpp>
#include <wjr/math/div.hpp>
#include <wjr/math/precompute-chars-convert.hpp>
#include <wjr/memory/copy.hpp>

#if defined(WJR_X86)
#include <wjr/x86/math/convert.hpp>
#endif

namespace wjr {

inline constexpr size_t dc_bignum_to_chars_threshold = WJR_DC_BIGNUM_TO_CHARS_THRESHOLD;
inline constexpr size_t dc_bignum_to_chars_precompute_threshold =
    WJR_DC_BIGNUM_TO_CHARS_THRESHOLD;

inline constexpr size_t dc_bignum_from_chars_threshold =
    WJR_DC_BIGNUM_FROM_CHARS_THRESHOLD;
inline constexpr size_t dc_bignum_from_chars_precompute_threshold =
    WJR_DC_BIGNUM_FROM_CHARS_PRECOMPUTE_THRESHOLD;

inline constexpr auto div2by1_divider_noshift_of_big_base_10 =
    div2by1_divider_noshift<uint64_t>(10'000'000'000'000'000'000ull,
                                      15'581'492'618'384'294'730ull);

namespace convert_details {

static constexpr std::array<uint8_t, 36> to_table = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b',
    'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

static constexpr std::array<uint8_t, 256> from_table = {
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   127, 127, 127, 127, 127, 127,
    127, 10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
    25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  127, 127, 127, 127, 127,
    127, 10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
    25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127};

template <uint64_t Base = 0>
WJR_CONST constexpr uint8_t to(uint8_t x) {
    if constexpr (Base == 0) {
        WJR_ASSERT_L2(x < 36);
    } else {
        WJR_ASSERT_L2(x < Base);
    }

    if constexpr (Base == 0 || Base > 10) {

        if (WJR_BUILTIN_CONSTANT_P(x < 10) && x < 10) {
            return x + '0';
        }

        return to_table[x];
    } else {
        return x + '0';
    }
}

template <uint64_t Base = 0>
WJR_CONST constexpr uint8_t from(uint8_t x) {
    if constexpr (Base == 0) {
        WJR_ASSERT_L2(from_table[x] < 36);
    } else {
        WJR_ASSERT_L2(from_table[x] < Base);
    }

    if constexpr (Base == 0 || Base > 10) {
        return from_table[x];
    } else {
        return x - '0';
    }
}

template <typename Enable, typename Base, typename Value, typename... Args>
struct __has_to_chars_fast_fn_fast_conv : std::false_type {};
template <typename Base, typename Value, typename... Args>
struct __has_to_chars_fast_fn_fast_conv<
    std::void_t<decltype(Base::__fast_conv(std::declval<void *>(),
                                           std::declval<Value>()))>,
    Base, Value, Args...> : std::true_type {};
template <typename Base, typename Value, typename... Args>
struct has_to_chars_fast_fn_fast_conv
    : __has_to_chars_fast_fn_fast_conv<void, Base, Value, Args...> {};
template <typename Base, typename Value, typename... Args>
constexpr bool has_to_chars_fast_fn_fast_conv_v =
    has_to_chars_fast_fn_fast_conv<Base, Value, Args...>::value;

template <typename Enable, typename Base, typename... Args>
struct __has_from_chars_fast_fn_fast_conv : std::false_type {};
template <typename Base, typename... Args>
struct __has_from_chars_fast_fn_fast_conv<
    std::void_t<decltype(Base::__fast_conv(std::declval<const void *>()))>, Base, Args...>
    : std::true_type {};
template <typename Base, typename... Args>
struct has_from_chars_fast_fn_fast_conv
    : __has_from_chars_fast_fn_fast_conv<void, Base, Args...> {};
template <typename Base, typename... Args>
constexpr bool has_from_chars_fast_fn_fast_conv_v =
    has_from_chars_fast_fn_fast_conv<Base, Args...>::value;

template <typename Enable, typename Base, typename... Args>
struct __has_from_chars_validate_fast_fn_fast_conv : std::false_type {};
template <typename Base, typename... Args>
struct __has_from_chars_validate_fast_fn_fast_conv<
    std::void_t<decltype(Base::__fast_conv(std::declval<const void *&>(),
                                           std::declval<Args>()...))>,
    Base, Args...> : std::true_type {};
template <typename Base, typename... Args>
struct has_from_chars_validate_fast_fn_fast_conv
    : __has_from_chars_validate_fast_fn_fast_conv<void, Base, Args...> {};
template <typename Base, typename... Args>
constexpr bool has_from_chars_validate_fast_fn_fast_conv_v =
    has_from_chars_validate_fast_fn_fast_conv<Base, Args...>::value;

} // namespace convert_details

// require operator() of Converter is constexpr
template <uint64_t Base, int Unroll>
class __char_converter_table_t {
    static constexpr uint64_t pw2 = Unroll == 2 ? Base * Base : Base * Base * Base * Base;

public:
    constexpr __char_converter_table_t() : table() {
        for (uint64_t i = 0, j = 0; i < pw2; ++i, j += Unroll) {
            int x = i;
            for (int k = Unroll - 1; ~k; --k) {
                table[j + k] = convert_details::to(x % Base);
                x /= Base;
            }
        }
    }

    WJR_CONST constexpr char operator[](unsigned int idx) const { return table[idx]; }

    WJR_CONST constexpr const char *data() const { return table.data(); }

private:
    std::array<char, pw2 * Unroll> table;
};

template <uint64_t Base, int Unroll>
inline constexpr __char_converter_table_t<Base, Unroll> __char_converter_table = {};

template <typename Iter, typename = void>
struct __is_fast_convert_iterator_helper : std::false_type {};

template <typename Iter>
struct __is_fast_convert_iterator_helper<
    Iter, std::enable_if_t<is_contiguous_iterator_v<Iter>, void>>
    : std::conjunction<std::is_trivially_copyable<iterator_value_t<Iter>>,
                       std::bool_constant<sizeof(iterator_value_t<Iter>) == 1>> {};

template <typename Iter>
struct __is_fast_convert_iterator : __is_fast_convert_iterator_helper<Iter> {};

/**
 * @brief Iterator concept that can fast convert to uint8_t *.
 *
 * @details The iterator must be contiguous iterator and the value_type must be
 * trivially copyable and sizeof(value_type) == 1.
 *
 */
template <typename Iter>
inline constexpr bool __is_fast_convert_iterator_v =
    __is_fast_convert_iterator<Iter>::value;

template <uint64_t Base>
class __to_chars_unroll_2_fast_fn_impl_base {
public:
    WJR_INTRINSIC_INLINE static void __fast_conv(void *ptr, uint32_t val) {
        auto str = (char *)ptr;
        if constexpr (Base * Base <= 16) {
            constexpr auto &table = __char_converter_table<Base, 4>;
            ::memcpy(str, table.data() + val * 4 + 2, 2);
        } else {
            constexpr auto &table = __char_converter_table<Base, 2>;
            ::memcpy(str, table.data() + val * 2, 2);
        }
    }
};

template <uint64_t Base>
class __to_chars_unroll_2_fast_fn_impl {};

template <>
class __to_chars_unroll_2_fast_fn_impl<2>
    : public __to_chars_unroll_2_fast_fn_impl_base<2> {};

template <>
class __to_chars_unroll_2_fast_fn_impl<8>
    : public __to_chars_unroll_2_fast_fn_impl_base<8> {};

template <>
class __to_chars_unroll_2_fast_fn_impl<10>
    : public __to_chars_unroll_2_fast_fn_impl_base<10> {};

template <>
class __to_chars_unroll_2_fast_fn_impl<16>
    : public __to_chars_unroll_2_fast_fn_impl_base<16> {};

template <uint64_t Base>
class __to_chars_unroll_4_fast_fn_impl_base {
public:
    WJR_INTRINSIC_INLINE static void __fast_conv(void *ptr, uint32_t val) {
        auto str = (char *)ptr;
        if constexpr (Base * Base <= 16) {
            constexpr auto &table = __char_converter_table<Base, 4>;
            ::memcpy(str, table.data() + val * 4, 4);
        } else {
            constexpr auto &table = __char_converter_table<Base, 2>;
            constexpr auto Base2 = Base * Base;
            uint32_t hi = val / Base2;
            uint32_t lo = val % Base2;

            ::memcpy(str, table.data() + hi * 2, 2);
            ::memcpy(str + 2, table.data() + lo * 2, 2);
        }
    }
};

template <uint64_t Base>
class __to_chars_unroll_4_fast_fn_impl {};

template <>
class __to_chars_unroll_4_fast_fn_impl<2>
    : public __to_chars_unroll_4_fast_fn_impl_base<2> {};

template <>
class __to_chars_unroll_4_fast_fn_impl<8>
    : public __to_chars_unroll_4_fast_fn_impl_base<8> {};

template <>
class __to_chars_unroll_4_fast_fn_impl<10>
    : public __to_chars_unroll_4_fast_fn_impl_base<10> {};

template <>
class __to_chars_unroll_4_fast_fn_impl<16>
    : public __to_chars_unroll_4_fast_fn_impl_base<16> {};

template <uint64_t Base>
class __to_chars_unroll_8_fast_fn_impl_base {
#if WJR_HAS_BUILTIN(TO_CHARS_UNROLL_8_FAST)
public:
    WJR_INTRINSIC_INLINE static void __fast_conv(void *ptr, uint64_t val) {
        builtin_to_chars_unroll_8_fast<Base>(ptr, val);
    }
#endif
};

template <uint64_t Base>
class __to_chars_unroll_8_fast_fn_impl {};

template <>
class __to_chars_unroll_8_fast_fn_impl<10>
    : public __to_chars_unroll_8_fast_fn_impl_base<10> {};

template <uint64_t Base>
class __to_chars_unroll_2_fn : public __to_chars_unroll_2_fast_fn_impl<Base> {
    using Mybase = __to_chars_unroll_2_fast_fn_impl<Base>;

public:
    WJR_INTRINSIC_INLINE void operator()(uint8_t *ptr, uint32_t val) const {
        if constexpr (convert_details::has_to_chars_fast_fn_fast_conv_v<Mybase,
                                                                        uint32_t>) {
            Mybase::__fast_conv(ptr, val);
        } else {
            ptr[0] = convert_details::to<Base>(val / Base);
            ptr[1] = convert_details::to<Base>(val % Base);
        }
    }
};

template <uint64_t Base>
inline constexpr __to_chars_unroll_2_fn<Base> __to_chars_unroll_2{};

template <uint64_t Base>
class __to_chars_unroll_4_fn_impl : public __to_chars_unroll_4_fast_fn_impl<Base> {
    using Mybase = __to_chars_unroll_4_fast_fn_impl<Base>;

public:
    WJR_INTRINSIC_INLINE void operator()(uint8_t *ptr, uint32_t val) const {
        if constexpr (convert_details::has_to_chars_fast_fn_fast_conv_v<Mybase,
                                                                        uint32_t>) {
            Mybase::__fast_conv(ptr, val);
        } else {
            constexpr auto Base2 = Base * Base;
            __to_chars_unroll_2<Base>(ptr, val / Base2);
            __to_chars_unroll_2<Base>(ptr + 2, val % Base2);
        }
    }
};

template <uint64_t Base>
inline constexpr __to_chars_unroll_4_fn_impl<Base> __to_chars_unroll_4{};

template <uint64_t Base>
class __to_chars_unroll_8_fn_impl : public __to_chars_unroll_8_fast_fn_impl<Base> {
    using Mybase = __to_chars_unroll_8_fast_fn_impl<Base>;

public:
    WJR_INTRINSIC_INLINE void operator()(uint8_t *ptr, uint64_t val) const {
        if constexpr (convert_details::has_to_chars_fast_fn_fast_conv_v<Mybase,
                                                                        uint64_t>) {
            Mybase::__fast_conv(to_address(ptr), val);
        } else {
            constexpr auto Base4 = Base * Base * Base * Base;
            __to_chars_unroll_4<Base>(ptr, val / Base4);
            __to_chars_unroll_4<Base>(ptr + 4, val % Base4);
        }
    }
};

template <uint64_t Base>
inline constexpr __to_chars_unroll_8_fn_impl<Base> __to_chars_unroll_8{};

template <uint64_t Base>
class __from_chars_unroll_4_fast_fn_impl_base {
    WJR_CONST WJR_INTRINSIC_INLINE static uint32_t __fast_conv(uint32_t val) {
        constexpr uint32_t Base2 = Base * Base;

        constexpr uint32_t mask = 0x00FF00FF;
        constexpr uint32_t mul = 1 + (Base2 << 16);
        val = (val * Base) + (val >> 8);
        val = ((val & mask) * mul) >> 16;
        return uint32_t(val);
    }

public:
    WJR_PURE WJR_INTRINSIC_INLINE static uint32_t __fast_conv(const void *ptr) {
        return __fast_conv(read_memory<uint32_t>(ptr) - 0x30303030u);
    }
};

template <uint64_t Base>
class __from_chars_unroll_8_fast_fn_impl_base {
protected:
    WJR_CONST WJR_INTRINSIC_INLINE static uint32_t __fast_conv(uint64_t val) {
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

public:
    WJR_PURE WJR_INTRINSIC_INLINE static uint32_t __fast_conv(const void *ptr) {
#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_8_FAST)
        return builtin_from_chars_unroll_8_fast<Base>(ptr);
#else
        return __fast_conv(read_memory<uint64_t>(ptr) - 0x3030303030303030ull);
#endif
    }
};

template <uint64_t Base>
class __from_chars_unroll_16_fast_fn_impl_base {
#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_16_FAST)
public:
    WJR_PURE WJR_INTRINSIC_INLINE static uint64_t __fast_conv(const void *ptr) {
        return builtin_from_chars_unroll_16_fast<Base>(ptr);
    }
#endif
};

template <uint64_t Base>
class __from_chars_unroll_4_fast_fn_impl {};

template <>
class __from_chars_unroll_4_fast_fn_impl<2>
    : public __from_chars_unroll_4_fast_fn_impl_base<2> {};

template <>
class __from_chars_unroll_4_fast_fn_impl<8>
    : public __from_chars_unroll_4_fast_fn_impl_base<8> {};

template <>
class __from_chars_unroll_4_fast_fn_impl<10>
    : public __from_chars_unroll_4_fast_fn_impl_base<10> {};

template <uint64_t Base>
class __from_chars_unroll_8_fast_fn_impl {};

template <>
class __from_chars_unroll_8_fast_fn_impl<2>
    : public __from_chars_unroll_8_fast_fn_impl_base<2> {};

template <>
class __from_chars_unroll_8_fast_fn_impl<8>
    : public __from_chars_unroll_8_fast_fn_impl_base<8> {};

template <>
class __from_chars_unroll_8_fast_fn_impl<10>
    : public __from_chars_unroll_8_fast_fn_impl_base<10> {};

template <uint64_t Base>
class __from_chars_unroll_16_fast_fn_impl {};

template <>
class __from_chars_unroll_16_fast_fn_impl<2>
    : public __from_chars_unroll_16_fast_fn_impl_base<2> {};

template <>
class __from_chars_unroll_16_fast_fn_impl<8>
    : public __from_chars_unroll_16_fast_fn_impl_base<8> {};

template <>
class __from_chars_unroll_16_fast_fn_impl<10>
    : public __from_chars_unroll_16_fast_fn_impl_base<10> {};

template <uint64_t Base>
class __from_chars_unroll_4_fn : public __from_chars_unroll_4_fast_fn_impl<Base> {
    using Mybase = __from_chars_unroll_4_fast_fn_impl<Base>;

public:
    WJR_PURE WJR_INTRINSIC_INLINE uint64_t operator()(const uint8_t *ptr) const {
        if constexpr (convert_details::has_from_chars_fast_fn_fast_conv_v<Mybase>) {
            return Mybase::__fast_conv(to_address(ptr));
        } else {
            uint64_t value = 0;
            value = convert_details::from<Base>(*ptr++);
            value = value * Base + convert_details::from<Base>(*ptr++);
            value = value * Base + convert_details::from<Base>(*ptr++);
            return value * Base + convert_details::from<Base>(*ptr++);
        }
    }
};

template <uint64_t Base>
inline constexpr __from_chars_unroll_4_fn<Base> __from_chars_unroll_4{};

template <uint64_t Base>
class __from_chars_unroll_8_fn : public __from_chars_unroll_8_fast_fn_impl<Base> {
    using Mybase = __from_chars_unroll_8_fast_fn_impl<Base>;

public:
    WJR_PURE WJR_INTRINSIC_INLINE uint64_t operator()(const uint8_t *ptr) const {
        if constexpr (convert_details::has_from_chars_fast_fn_fast_conv_v<Mybase>) {
            return Mybase::__fast_conv(ptr);
        } else {
            constexpr uint64_t Base4 = Base * Base * Base * Base;
            return __from_chars_unroll_4<Base>(ptr) * Base4 +
                   __from_chars_unroll_4<Base>(ptr + 4);
        }
    }
};

template <uint64_t Base>
inline constexpr __from_chars_unroll_8_fn<Base> __from_chars_unroll_8{};

template <uint64_t Base>
class __from_chars_unroll_16_fn : public __from_chars_unroll_16_fast_fn_impl<Base> {
    using Mybase = __from_chars_unroll_16_fast_fn_impl<Base>;

public:
    WJR_PURE WJR_INTRINSIC_INLINE uint64_t operator()(const uint8_t *ptr) const {
        if constexpr (convert_details::has_from_chars_fast_fn_fast_conv_v<Mybase>) {
            return Mybase::__fast_conv(ptr);
        } else {
            constexpr uint64_t Base4 = Base * Base * Base * Base;
            constexpr uint64_t Base8 = Base4 * Base4;
            return __from_chars_unroll_8<Base>(ptr) * Base8 +
                   __from_chars_unroll_8<Base>(ptr + 8);
        }
    }
};

template <uint64_t Base>
inline constexpr __from_chars_unroll_16_fn<Base> __from_chars_unroll_16{};

template <typename UnsignedValue>
constexpr int fallback_count_digits10(UnsignedValue n) {
    int count = 1;

    if (WJR_UNLIKELY(n >= 1000)) {
        do {
            n /= 10000;
            count += 4;
        } while (n >= 1000);

        if (n == 0) {
            return count;
        }
    }

    if (n < 10) {
        return count;
    }

    if (n < 100) {
        return count + 1;
    }

    return count + 3;
}

inline int builtin_count_digits10_u32(uint32_t n) {
#define WJR_INC(T) (((sizeof(#T) - 1ull) << 32) - T)
    static constexpr uint64_t table[] = {
        WJR_INC(0),          WJR_INC(0),          WJR_INC(0),          // 8
        WJR_INC(10),         WJR_INC(10),         WJR_INC(10),         // 64
        WJR_INC(100),        WJR_INC(100),        WJR_INC(100),        // 512
        WJR_INC(1000),       WJR_INC(1000),       WJR_INC(1000),       // 4096
        WJR_INC(10000),      WJR_INC(10000),      WJR_INC(10000),      // 32k
        WJR_INC(100000),     WJR_INC(100000),     WJR_INC(100000),     // 256k
        WJR_INC(1000000),    WJR_INC(1000000),    WJR_INC(1000000),    // 2048k
        WJR_INC(10000000),   WJR_INC(10000000),   WJR_INC(10000000),   // 16M
        WJR_INC(100000000),  WJR_INC(100000000),  WJR_INC(100000000),  // 128M
        WJR_INC(1000000000), WJR_INC(1000000000), WJR_INC(1000000000), // 1024M
        WJR_INC(1000000000), WJR_INC(1000000000)                       // 4B
    };
    auto inc = table[clz(n | 1) ^ 31];
    return static_cast<int>((n + inc) >> 32);
#undef WJR_INC
}

inline int builtin_count_digits10_u64(uint64_t n) {
#define WJR_POWERS_OF_10(factor)                                                         \
    factor * 10, (factor)*100, (factor)*1000, (factor)*10000, (factor)*100000,           \
        (factor)*1000000, (factor)*10000000, (factor)*100000000, (factor)*1000000000
    static constexpr uint8_t bsr2log10[] = {
        1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,
        6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9,  10, 10, 10,
        10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15,
        15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 19, 20};
    auto t = bsr2log10[clz(n | 1) ^ 63];
    static constexpr const uint64_t zero_or_powers_of_10[] = {
        0, 0, WJR_POWERS_OF_10(1U), WJR_POWERS_OF_10(1000000000ULL),
        10000000000000000000ULL};
    return t - (n < zero_or_powers_of_10[t]);
#undef WJR_POWERS_OF_10
}

template <typename T>
WJR_CONSTEXPR_E int count_digits10_impl(T n) {
    if (is_constant_evaluated() || WJR_BUILTIN_CONSTANT_P(n)) {
        return fallback_count_digits10(n);
    }

    if constexpr (sizeof(T) <= sizeof(uint32_t)) {
        return builtin_count_digits10_u32(static_cast<uint32_t>(n));
    } else {
        return builtin_count_digits10_u64(static_cast<uint64_t>(n));
    }
}

template <uint64_t Base>
struct count_digits_fn {};

template <uint64_t Base>
inline constexpr count_digits_fn<Base> count_digits{};

template <>
struct count_digits_fn<2> {
    template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
    WJR_CONST WJR_CONSTEXPR_E int operator()(T n) const {
        return bit_width(n);
    }
};

template <>
struct count_digits_fn<8> {
    template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
    WJR_CONST WJR_CONSTEXPR_E int operator()(T n) const {
        return (bit_width(n) + 2) / 3;
    }
};

template <>
struct count_digits_fn<16> {
    template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
    WJR_CONST WJR_CONSTEXPR_E int operator()(T n) const {
        return (bit_width(n) + 3) / 4;
    }
};

template <>
struct count_digits_fn<1> {
    template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
    WJR_CONST WJR_CONSTEXPR_E int operator()(T n, int per_bit) const {
        return (bit_width(n) + per_bit - 1) / per_bit;
    }
};

template <>
struct count_digits_fn<10> {
    template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
    WJR_CONST WJR_CONSTEXPR_E int operator()(T n) const {
        int ret = count_digits10_impl(n);
        WJR_ASSUME(1 <= ret && ret <= std::numeric_limits<T>::digits10 + 1);
        return ret;
    }
};

template <typename Iter>
struct to_chars_result {
    Iter ptr;
    std::errc ec;

    friend bool operator==(const to_chars_result &lhs, const to_chars_result &rhs) {
        return lhs.ptr == rhs.ptr && lhs.ec == rhs.ec;
    }

    constexpr explicit operator bool() const { return ec == std::errc{}; }
};

template <typename Iter>
struct from_chars_result {
    Iter ptr;
    std::errc ec;

    friend bool operator==(const from_chars_result &lhs, const from_chars_result &rhs) {
        return lhs.ptr == rhs.ptr && lhs.ec == rhs.ec;
    }

    constexpr explicit operator bool() const { return ec == std::errc{}; }
};

// Base :
// 0 : dynamic base
// 1 : base is power of two
template <uint64_t Base>
struct __unsigned_to_chars_backward_fn {};

template <uint64_t Base>
inline constexpr __unsigned_to_chars_backward_fn<Base> __unsigned_to_chars_backward{};

template <>
struct __unsigned_to_chars_backward_fn<2> {
    template <typename UnsignedValue,
              std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
    uint8_t *operator()(uint8_t *ptr, int n, UnsignedValue x) const {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;
        WJR_ASSERT(x != 0);
        WJR_ASSERT_ASSUME(1 <= n && n <= nd);
        (void)(nd);

        if (WJR_UNLIKELY(n >= 4)) {
            do {
                __to_chars_unroll_4<2>(ptr - 4, x & 0x0f);
                ptr -= 4;
                x >>= 4;
                n -= 4;
            } while (WJR_LIKELY(n >= 4));

            if (n == 0) {
                return ptr;
            }
        }

        switch (n) {
        case 3: {
            *--ptr = convert_details::to<2>(x & 1);
            x >>= 1;
            WJR_FALLTHROUGH;
        }
        case 2: {
            __to_chars_unroll_2<2>(ptr - 2, x);
            ptr -= 2;
            break;
        }
        case 1: {
            *--ptr = convert_details::to<2>(x);
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        return ptr;
    }
};

template <>
struct __unsigned_to_chars_backward_fn<8> {
    template <typename UnsignedValue,
              std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
    uint8_t *operator()(uint8_t *ptr, int n, UnsignedValue x) const {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;
        WJR_ASSERT(x != 0);
        WJR_ASSERT_ASSUME(1 <= n && n <= (nd + 2) / 3);

        if constexpr (nd >= 16) {
            if (WJR_UNLIKELY(n >= 4)) {
                do {
                    __to_chars_unroll_4<8>(ptr - 4, x & 0x0fff);
                    ptr -= 4;
                    x >>= 12;
                    n -= 4;
                } while (WJR_LIKELY(n >= 4));

                if (n == 0) {
                    return ptr;
                }
            }
        }

        switch (n) {
        case 3: {
            *--ptr = convert_details::to<8>(x & 0x07);
            x >>= 3;
            WJR_FALLTHROUGH;
        }
        case 2: {
            __to_chars_unroll_2<8>(ptr - 2, x);
            ptr -= 2;
            break;
        }
        case 1: {
            *--ptr = convert_details::to<8>(x);
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        return ptr;
    }
};

template <>
struct __unsigned_to_chars_backward_fn<16> {

    template <typename UnsignedValue,
              std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
    uint8_t *operator()(uint8_t *ptr, int n, UnsignedValue x) const {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;
        WJR_ASSERT(x != 0);
        WJR_ASSERT_ASSUME(1 <= n && n <= (nd + 3) / 4);

        if constexpr (nd >= 16) {
            if (WJR_UNLIKELY(n >= 4)) {
                do {
                    __to_chars_unroll_4<16>(ptr - 4, x & 0xffff);
                    ptr -= 4;
                    x >>= 16;
                    n -= 4;
                } while (WJR_LIKELY(n >= 4));

                if (n == 0) {
                    return ptr;
                }
            }
        }

        switch (n) {
        case 3: {
            *--ptr = convert_details::to(x & 0x0f);
            x >>= 4;
            WJR_FALLTHROUGH;
        }
        case 2: {
            __to_chars_unroll_2<16>(ptr - 2, x);
            ptr -= 2;
            break;
        }
        case 1: {
            *--ptr = convert_details::to(x);
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        return ptr;
    }
};

template <>
struct __unsigned_to_chars_backward_fn<1> {
    template <typename UnsignedValue,
              std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
    uint8_t *operator()(uint8_t *ptr, int n, UnsignedValue x, int per_bit) const {
        WJR_ASSERT(x != 0);
        WJR_ASSERT_ASSUME(1 <= n && n <= std::numeric_limits<UnsignedValue>::digits);

        unsigned int mask = (1u << per_bit) - 1;

        do {
            *--ptr = convert_details::to(x & mask);
            x >>= per_bit;
            --n;
        } while (WJR_LIKELY(n != 0));

        return ptr;
    }
};

template <>
struct __unsigned_to_chars_backward_fn<10> {
    template <typename UnsignedValue,
              std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
    uint8_t *operator()(uint8_t *ptr, UnsignedValue val) const {
        WJR_ASSERT_ASSUME(val != 0);

        if (WJR_LIKELY(val >= 100)) {
            do {
                __to_chars_unroll_2<10>(ptr - 2, val % 100);
                ptr -= 2;
                val /= 100;
            } while (WJR_LIKELY(val >= 100));
        }

        if (val < 10) {
            *--ptr = convert_details::to<10>(val);
            return ptr;
        }

        __to_chars_unroll_2<10>(ptr - 2, val);
        ptr -= 2;
        return ptr;
    }
};

template <typename Value, typename IBase>
uint8_t *__fast_to_chars_backward_impl(uint8_t *ptr, Value val, IBase ibase) {
    if (WJR_UNLIKELY(val == 0)) {
        *--ptr = '0';
        return ptr;
    }

    auto uVal = to_unsigned(val);
    int sign = 0;

    if constexpr (std::is_signed_v<Value>) {
        if (val < 0) {
            sign = 1;
            uVal = -val;
        }
    }

    const unsigned int base = ibase;

    switch (base) {
    case 2: {
        ptr = __unsigned_to_chars_backward<2>(ptr, count_digits<2>(uVal), uVal);
        break;
    }
    case 8: {
        ptr = __unsigned_to_chars_backward<8>(ptr, count_digits<8>(uVal), uVal);
        break;
    }
    case 16: {
        ptr = __unsigned_to_chars_backward<16>(ptr, count_digits<16>(uVal), uVal);
        break;
    }
    case 4:
    case 32: {
        int per_bit = base == 4 ? 2 : 5;
        ptr = __unsigned_to_chars_backward<1>(ptr, count_digits<1>(uVal, per_bit), uVal,
                                              per_bit);
        break;
    }
    case 10: {
        ptr = __unsigned_to_chars_backward<10>(ptr, uVal);
        break;
    }
    default: {
        WJR_UNREACHABLE();
        return ptr;
    }
    }

    if constexpr (std::is_signed_v<Value>) {
        if (sign) {
            *--ptr = '-';
        }
    }

    return ptr;
}

template <typename Iter, typename Value, typename IBase>
Iter __to_chars_backward_impl(Iter first, Value val, IBase ibase) {
    auto __ptr = reinterpret_cast<uint8_t *>(to_address(first));
    auto __end = __fast_to_chars_backward_impl(__ptr, val, ibase);
    return first + std::distance(__ptr, __end);
}

template <
    typename Iter, typename Value, typename BaseType = unsigned int, BaseType IBase = 10,
    std::enable_if_t<__is_fast_convert_iterator_v<Iter> && is_nonbool_integral_v<Value>,
                     int> = 0>
Iter to_chars_backward(Iter first, Value val,
                       std::integral_constant<BaseType, IBase> = {}) {
    return __to_chars_backward_impl(first, val,
                                    std::integral_constant<unsigned int, IBase>());
}

/**
 * @brief Convert an unsigned integer to a string in reverse order without checking
 * buf size.
 *
 * @tparam Iter The iterator type. Must be random access iterator.
 * @tparam Value The value type. If Converter is origin_converter_t, Value must be
 * non-bool unsigned integral type. Otherwise, Value must be non-bool integral type.
 *
 */
template <
    typename Iter, typename Value,
    std::enable_if_t<__is_fast_convert_iterator_v<Iter> && is_nonbool_integral_v<Value>,
                     int> = 0>
Iter to_chars_backward(Iter first, Value val, unsigned int base) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return to_chars_backward(first, val,
                                     std::integral_constant<unsigned int, 2>{});
        }
        case 8: {
            return to_chars_backward(first, val,
                                     std::integral_constant<unsigned int, 8>{});
        }
        case 16: {
            return to_chars_backward(first, val,
                                     std::integral_constant<unsigned int, 16>{});
        }
        case 10: {
            return to_chars_backward(first, val,
                                     std::integral_constant<unsigned int, 10>{});
        }
        default: {
            break;
        }
        }
    }

    return __to_chars_backward_impl(first, val, base);
}

template <typename Value, typename IBase>
to_chars_result<uint8_t *> __fast_to_chars_validate_impl(uint8_t *first, uint8_t *last,
                                                         Value val, IBase ibase) {
    if (WJR_UNLIKELY(val == 0)) {
        if (WJR_UNLIKELY(first == last)) {
            return {last, std::errc::value_too_large};
        }

        *first++ = '0';
        return {first, std::errc{}};
    }

    auto uVal = to_unsigned(val);

    if constexpr (std::is_signed_v<Value>) {
        if (val < 0) {
            if (WJR_UNLIKELY(first == last)) {
                return {last, std::errc::value_too_large};
            }

            *first++ = '-';
            uVal = -val;
        }
    }

    const unsigned int base = ibase;
    auto size = std::distance(first, last);

#define WJR_TO_CHARS_VALIDATE_IMPL(BASE, DIGITS, CALL)                                   \
    int n = count_digits<BASE> DIGITS;                                                   \
    if (WJR_LIKELY(n <= size)) {                                                         \
        first += n;                                                                      \
        (void)__unsigned_to_chars_backward<BASE>(first, WJR_PP_QUEUE_EXPAND(CALL));      \
        return {first, std::errc{}};                                                     \
    }                                                                                    \
    return { last, std::errc::value_too_large }

    switch (base) {
    case 2: {
        WJR_TO_CHARS_VALIDATE_IMPL(2, (uVal), (n, uVal));
    }
    case 8: {
        WJR_TO_CHARS_VALIDATE_IMPL(8, (uVal), (n, uVal));
    }
    case 16: {
        WJR_TO_CHARS_VALIDATE_IMPL(16, (uVal), (n, uVal));
    }
    case 4:
    case 32: {
        int per_bit = base == 4 ? 2 : 5;
        WJR_TO_CHARS_VALIDATE_IMPL(1, (uVal, per_bit), (n, uVal, per_bit));
    }
    case 10: {
        WJR_TO_CHARS_VALIDATE_IMPL(10, (uVal), (uVal));
    }
    default: {
        WJR_UNREACHABLE();
        return {last, std::errc::invalid_argument};
    }
    }

#undef WJR_TO_CHARS_VALIDATE_IMPL
}

template <typename Iter, typename Value, typename IBase>
to_chars_result<Iter> __fallback_to_chars_validate_impl(Iter first, Iter last, Value val,
                                                        IBase ibase) {
    constexpr auto is_signed = std::is_signed_v<Value>;
    constexpr auto base_2_table = std::numeric_limits<Value>::digits;
    constexpr auto base_10_table = std::numeric_limits<Value>::digits10 + 1;
    constexpr auto is_random_access = is_random_access_iterator_v<Iter>;

    if (WJR_UNLIKELY(val == 0)) {
        if (WJR_UNLIKELY(first == last)) {
            return {last, std::errc::value_too_large};
        }

        *first++ = '0';
        return {first, std::errc{}};
    }

    auto uVal = to_unsigned(val);
    int sign = 0;

    if constexpr (is_signed) {
        if constexpr (is_random_access) {
            if (val < 0) {
                if (WJR_UNLIKELY(first == last)) {
                    return {last, std::errc::value_too_large};
                }

                sign = 1;
                uVal = -val;
            }
        } else {
            if (val < 0) {
                if (WJR_UNLIKELY(first == last)) {
                    return {last, std::errc::value_too_large};
                }

                *first++ = '-';
                uVal = -val;
            }
        }
    }

    const unsigned int base = ibase;

#define WJR_TO_CHARS_VALIDATE_IMPL(BASE, TABLE, DIGITS, CALL)                            \
    int n = count_digits<BASE> DIGITS;                                                   \
                                                                                         \
    if constexpr (is_random_access) {                                                    \
        auto size = std::distance(first, last);                                          \
                                                                                         \
        if constexpr (is_signed) {                                                       \
            if (WJR_UNLIKELY(n + sign > size)) {                                         \
                return {last, std::errc::value_too_large};                               \
            }                                                                            \
        } else {                                                                         \
            if (WJR_UNLIKELY(n > size)) {                                                \
                return {last, std::errc::value_too_large};                               \
            }                                                                            \
        }                                                                                \
                                                                                         \
        uint8_t buffer[TABLE + is_signed];                                               \
        auto __ptr = __unsigned_to_chars_backward<BASE>(buffer + TABLE + is_signed,      \
                                                        WJR_PP_QUEUE_EXPAND(CALL));      \
                                                                                         \
        if constexpr (is_signed) {                                                       \
            if (sign) {                                                                  \
                *--__ptr = '-';                                                          \
                ++n;                                                                     \
            }                                                                            \
        }                                                                                \
                                                                                         \
        return wjr::copy_n(__ptr, n, first);                                             \
    } else {                                                                             \
        uint8_t buffer[TABLE];                                                           \
        auto __ptr = __unsigned_to_chars_backward<BASE>(buffer + TABLE,                  \
                                                        WJR_PP_QUEUE_EXPAND(CALL));      \
                                                                                         \
        do {                                                                             \
            if (WJR_UNLIKELY(first == last)) {                                           \
                return {last, std::errc::value_too_large};                               \
            }                                                                            \
                                                                                         \
            *first++ = *__ptr++;                                                         \
        } while (--n != 0);                                                              \
                                                                                         \
        return {first, std::errc{}};                                                     \
    }

    switch (base) {
    case 2: {
        WJR_TO_CHARS_VALIDATE_IMPL(2, base_2_table, (uVal), (n, uVal));
    }
    case 8: {
        WJR_TO_CHARS_VALIDATE_IMPL(8, (base_2_table + 2) / 3, (uVal), (n, uVal));
    }
    case 16: {
        WJR_TO_CHARS_VALIDATE_IMPL(16, (base_2_table + 3) / 4, (uVal), (n, uVal));
    }
    case 4:
    case 32: {
        int per_bit = base == 4 ? 2 : 5;
        WJR_TO_CHARS_VALIDATE_IMPL(1, (base_2_table + per_bit - 1) / per_bit,
                                   (uVal, per_bit), (n, uVal, per_bit));
    }
    case 10: {
        WJR_TO_CHARS_VALIDATE_IMPL(10, base_10_table, (uVal), (uVal));
    }
    default: {
        WJR_UNREACHABLE();
        return {last, std::errc::invalid_argument};
    }
    }

#undef WJR_TO_CHARS_VALIDATE_IMPL
}

template <typename Iter, typename Value, typename IBase>
to_chars_result<Iter> __to_chars_validate_impl(Iter first, Iter last, Value val,
                                               IBase ibase) {
    if constexpr (__is_fast_convert_iterator_v<Iter>) {
        auto __first = reinterpret_cast<uint8_t *>(to_address(first));
        auto __last = reinterpret_cast<uint8_t *>(to_address(last));
        auto __result = __fast_to_chars_validate_impl(__first, __last, val, ibase);
        return {first + std::distance(__first, __result.ptr), __result.ec};
    } else {
        return __fallback_to_chars_validate_impl(first, last, val, ibase);
    }
}

template <typename Value, typename IBase>
uint8_t *__fast_to_chars_impl(uint8_t *ptr, Value val, IBase ibase) {
    if (WJR_UNLIKELY(val == 0)) {
        *ptr++ = '0';
        return ptr;
    }

    auto uVal = to_unsigned(val);

    if constexpr (std::is_signed_v<Value>) {
        if (val < 0) {
            *ptr++ = '-';
            uVal = -val;
        }
    }

    const unsigned int base = ibase;

    switch (base) {
    case 2: {
        int n = count_digits<2>(uVal);
        ptr += n;
        (void)__unsigned_to_chars_backward<2>(ptr, n, uVal);
        return ptr;
    }
    case 8: {
        int n = count_digits<8>(uVal);
        ptr += n;
        (void)__unsigned_to_chars_backward<8>(ptr, n, uVal);
        return ptr;
    }
    case 16: {
        int n = count_digits<16>(uVal);
        ptr += n;
        (void)__unsigned_to_chars_backward<16>(ptr, n, uVal);
        return ptr;
    }
    case 4:
    case 32: {
        int per_bit = base == 4 ? 2 : 5;
        int n = count_digits<1>(uVal, per_bit);
        ptr += n;
        (void)__unsigned_to_chars_backward<1>(ptr, n, uVal, per_bit);
        return ptr;
    }
    case 10: {
        int n = count_digits<10>(uVal);
        ptr += n;
        (void)__unsigned_to_chars_backward<10>(ptr, uVal);
        return ptr;
    }
    default: {
        WJR_UNREACHABLE();
        return ptr;
    }
    }
}

template <typename Iter, typename Value, typename IBase>
Iter __fallback_to_chars_impl(Iter ptr, Value val, IBase ibase) {
    constexpr auto is_signed = std::is_signed_v<Value>;
    constexpr auto base_2_table = std::numeric_limits<Value>::digits;
    constexpr auto base_10_table = std::numeric_limits<Value>::digits10 + 1;

    if (WJR_UNLIKELY(val == 0)) {
        *ptr++ = '0';
        return ptr;
    }

    auto uVal = to_unsigned(val);
    int sign = 0;

    if constexpr (is_signed) {
        if (val < 0) {
            sign = 1;
            uVal = -val;
        }
    }

    const unsigned int base = ibase;

#define WJR_TO_CHARS_IMPL(BASE, TABLE, DIGITS, CALL)                                     \
    int n = count_digits<BASE> DIGITS;                                                   \
                                                                                         \
    uint8_t buffer[TABLE + is_signed];                                                   \
    auto __ptr = __unsigned_to_chars_backward<BASE>(buffer + TABLE + is_signed,          \
                                                    WJR_PP_QUEUE_EXPAND(CALL));          \
                                                                                         \
    if constexpr (is_signed) {                                                           \
        if (sign) {                                                                      \
            *--__ptr = '-';                                                              \
            ++n;                                                                         \
        }                                                                                \
    }                                                                                    \
                                                                                         \
    return wjr::copy_n(__ptr, n, ptr)

    switch (base) {
    case 2: {
        WJR_TO_CHARS_IMPL(2, base_2_table, (uVal), (n, uVal));
    }
    case 8: {
        WJR_TO_CHARS_IMPL(8, (base_2_table + 2) / 3, (uVal), (n, uVal));
    }
    case 16: {
        WJR_TO_CHARS_IMPL(16, (base_2_table + 3) / 4, (uVal), (n, uVal));
    }
    case 4:
    case 32: {
        int per_bit = base == 4 ? 2 : 5;
        WJR_TO_CHARS_IMPL(1, (base_2_table + per_bit - 1) / per_bit, (uVal, per_bit),
                          (n, uVal, per_bit));
    }
    case 10: {
        WJR_TO_CHARS_IMPL(10, base_10_table, (uVal), (uVal));
    }
    default: {
        WJR_UNREACHABLE();
        return ptr;
    }
    }

#undef WJR_TO_CHARS_IMPL
}

template <typename Iter, typename Value, typename IBase>
Iter __to_chars_impl(Iter ptr, Value val, IBase ibase) {
    if constexpr (__is_fast_convert_iterator_v<Iter>) {
        auto __ptr = reinterpret_cast<uint8_t *>(to_address(ptr));
        auto __result = __fast_to_chars_impl(__ptr, val, ibase);
        return ptr + std::distance(__ptr, __result);
    } else {
        return __fallback_to_chars_impl(ptr, val, ibase);
    }
}

template <typename Iter, typename Value, typename BaseType = unsigned int,
          BaseType IBase = 10, std::enable_if_t<is_nonbool_integral_v<Value>, int> = 0>
to_chars_result<Iter> to_chars_validate(Iter ptr, Iter last, Value val,
                                        std::integral_constant<BaseType, IBase> = {}) {
    return __to_chars_validate_impl(ptr, last, val,
                                    std::integral_constant<unsigned int, IBase>());
}

/**
 * @brief Convert an unsigned integer to a string with checking buf size.
 *
 * @tparam Iter The iterator type. Must be random access iterator.
 * @tparam Value The value type. If Converter is origin_converter_t, Value must be
 * non-bool unsigned integral type. Otherwise, Value must be non-bool integral type.
 *
 * @return to_chars_result<Iter> If the conversion is successful, return {ans,
 * std::errc{}}. Otherwise, return {last, std::errc::value_too_large}.
 *
 */
template <typename Iter, typename Value,
          std::enable_if_t<is_nonbool_integral_v<Value>, int> = 0>
to_chars_result<Iter> to_chars_validate(Iter ptr, Iter last, Value val,
                                        unsigned int base) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return to_chars_validate(ptr, last, val,
                                     std::integral_constant<unsigned int, 2>{});
        }
        case 8: {
            return to_chars_validate(ptr, last, val,
                                     std::integral_constant<unsigned int, 8>{});
        }
        case 16: {
            return to_chars_validate(ptr, last, val,
                                     std::integral_constant<unsigned int, 16>{});
        }
        case 10: {
            return to_chars_validate(ptr, last, val,
                                     std::integral_constant<unsigned int, 10>{});
        }
        default: {
            break;
        }
        }
    }

    return __to_chars_validate_impl(ptr, last, val, base);
}

template <typename Iter, typename Value, typename BaseType = unsigned int,
          BaseType IBase = 10, std::enable_if_t<is_nonbool_integral_v<Value>, int> = 0>
Iter to_chars(Iter ptr, Value val, std::integral_constant<BaseType, IBase> = {}) {
    return __to_chars_impl(ptr, val, std::integral_constant<unsigned int, IBase>());
}

/**
 * @brief Convert an unsigned integer to a string without checking buf size.
 *
 * @tparam Iter The iterator type. Must be random access iterator.
 * @tparam Value The value type. If Converter is origin_converter_t, Value must be
 * non-bool unsigned integral type. Otherwise, Value must be non-bool integral type.
 *
 */
template <typename Iter, typename Value,
          std::enable_if_t<is_nonbool_integral_v<Value>, int> = 0>
Iter to_chars(Iter ptr, Value val, unsigned int base) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return to_chars(ptr, val, std::integral_constant<unsigned int, 2>{});
        }
        case 8: {
            return to_chars(ptr, val, std::integral_constant<unsigned int, 8>{});
        }
        case 16: {
            return to_chars(ptr, val, std::integral_constant<unsigned int, 16>{});
        }
        case 10: {
            return to_chars(ptr, val, std::integral_constant<unsigned int, 10>{});
        }
        default: {
            break;
        }
        }
    }

    return __to_chars_impl(ptr, val, base);
}

inline size_t __biginteger_to_chars_2_impl(uint8_t *first, const uint64_t *up, size_t n) {
    WJR_ASSERT(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);

    size_t len = hbits + 64 * n;
    first += len;

    do {
        x = *up;

        for (int i = 0; i < 8; ++i) {
            __to_chars_unroll_8<2>(first - 8, x & 0xff);
            first -= 8;
            x >>= 8;
        }

        ++up;
        --n;
    } while (n);
    x = *up;

    (void)__unsigned_to_chars_backward<2>(first, hbits, x);
    return len;
}

inline size_t __biginteger_to_chars_8_impl(uint8_t *first, const uint64_t *up, size_t n) {
    WJR_ASSERT(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);

    size_t len = (hbits + 64 * n + 2) / 3;
    first += len;

    int rest = 0;
    unsigned int last = 0;

    do {
        x = *up;

        switch (rest) {
        case 0: {
            rest = 2;
            break;
        }
        case 2: {
            __to_chars_unroll_2<8>(first - 2, last | ((x & 0x03) << 4));
            first -= 2;
            x >>= 2;
            rest = 4;
            break;
        }
        case 4: {
            __to_chars_unroll_2<8>(first - 2, last | ((x & 0x0f) << 2));
            first -= 2;
            x >>= 4;
            rest = 0;
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        __to_chars_unroll_8<8>(first - 8, x & 0xff'ffff);
        x >>= 24;
        __to_chars_unroll_8<8>(first - 16, x & 0xff'ffff);
        x >>= 24;
        __to_chars_unroll_4<8>(first - 20, x & 0x0fff);
        x >>= 12;
        first -= 20;

        last = x;

        ++up;
        --n;
    } while (n);
    x = *up;

    switch (rest) {
    case 0: {
        break;
    }
    case 2: {
        __to_chars_unroll_2<8>(first - 2, last | ((x & 0x03) << 4));
        first -= 2;
        if (hbits <= 2) {
            goto DONE;
        }
        hbits -= 2;
        x >>= 2;
        break;
    }
    case 4: {
        if (WJR_UNLIKELY(hbits == 1)) {
            *--first = convert_details::to<8>(x << 2 | last);
            goto DONE;
        }

        __to_chars_unroll_2<8>(first - 2, last | ((x & 0x0f) << 2));
        first -= 2;
        if (hbits <= 4) {
            goto DONE;
        }
        hbits -= 4;
        x >>= 4;
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

    if (WJR_LIKELY(hbits + 2 >= 12)) {
        do {
            __to_chars_unroll_4<8>(first - 4, x & 0x0fff);
            first -= 4;
            x >>= 12;
            hbits -= 12;
        } while (WJR_LIKELY(hbits + 2 >= 12));
    }

    switch ((hbits + 2) / 3) {
    case 3: {
        *--first = convert_details::to<8>(x & 0x07);
        x >>= 3;
        WJR_FALLTHROUGH;
    }
    case 2: {
        __to_chars_unroll_2<8>(first - 2, x);
        break;
    }
    case 1: {
        *--first = convert_details::to<8>(x);
        WJR_FALLTHROUGH;
    }
    case 0: {
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

DONE:
    return len;
}

inline size_t __biginteger_to_chars_16_impl(uint8_t *first, const uint64_t *up,
                                            size_t n) {
    WJR_ASSERT(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);
    hbits = (hbits + 3) / 4;

    size_t len = hbits + 16 * n;
    first += len;

    do {
        x = *up;

        __to_chars_unroll_8<16>(first - 8, x & 0xffff'ffff);
        __to_chars_unroll_8<16>(first - 16, x >> 32);
        first -= 16;

        ++up;
        --n;
    } while (n);
    x = *up;

    (void)__unsigned_to_chars_backward<16>(first, hbits, x);

    return len;
}

inline size_t __biginteger_to_chars_power_of_two_impl(uint8_t *first, const uint64_t *up,
                                                      size_t n, unsigned int base) {
    WJR_ASSERT(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    int per_bit = ctz(base);
    unsigned int mask = (1u << per_bit) - 1;

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);

    size_t len = (hbits + 64 * n + per_bit - 1) / per_bit;
    first += len;

    int rest = 0;
    unsigned int last = 0;

    do {
        x = *up;

        if (rest) {
            int fix = per_bit - rest;
            unsigned int val = ((x & ((1u << fix) - 1)) << rest) | last;
            x >>= fix;
            rest = 64 - fix;
            *--first = convert_details::to(val);
        } else {
            rest = 64;
        }

        do {
            *--first = convert_details::to(x & mask);
            x >>= per_bit;
            rest -= per_bit;
        } while (rest >= per_bit);

        last = x;

        ++up;
        --n;
    } while (n);
    x = *up;

    WJR_ASSERT_ASSUME(rest < per_bit);

    if (WJR_UNLIKELY(rest != 0)) {
        int fix = per_bit - rest;
        unsigned int val = ((x & ((1u << fix) - 1)) << rest) | last;
        x >>= fix;
        *--first = convert_details::to(val);
        rest = hbits - fix;
        if (WJR_UNLIKELY(rest == 0)) {
            goto DONE;
        }
    } else {
        rest = hbits;
    }

    do {
        *--first = convert_details::to(x & mask);
        x >>= per_bit;
        rest -= per_bit;
    } while (WJR_LIKELY(rest > 0));

DONE:
    return len;
}

inline uint8_t *basecase_to_chars_10(uint8_t *buf, uint64_t *up, size_t n) {
    do {
        if (WJR_UNLIKELY(n == 1)) {
            return __unsigned_to_chars_backward<10>(buf, up[0]);
        }

        uint64_t q, rem;

        q = div_qr_1_noshift(up, rem, up, n, div2by1_divider_noshift_of_big_base_10);
        n -= q == 0;
        if (q != 0) {
            up[n - 1] = q;
        }

        __to_chars_unroll_8<10>(buf - 8, rem % 1'0000'0000);
        rem /= 1'0000'0000;
        __to_chars_unroll_8<10>(buf - 16, rem % 1'0000'0000);
        rem /= 1'0000'0000;
        __to_chars_unroll_2<10>(buf - 18, rem % 100);
        rem /= 100;
        buf[-19] = convert_details::to<10>(rem);

        buf -= 19;
    } while (n);

    return buf;
}

inline uint8_t *basecase_to_chars(uint8_t *first, size_t len, uint64_t *up, size_t n,
                                  unsigned int base) {
    constexpr size_t buf_len = dc_bignum_to_chars_precompute_threshold * 64 * 7 / 11;
    uint8_t buf[buf_len];
    uint8_t *end = buf + buf_len;
    uint8_t *start;

    if (WJR_LIKELY(base == 10)) {
        start = basecase_to_chars_10(end, up, n);
    } else {
        start = end;
    }

    size_t rlen = end - start;
    if (len > rlen) {
        first = std::fill_n(first, len - rlen, '0');
    }

    return std::copy(start, end, first);
}

inline uint8_t *dc_to_chars(uint8_t *first, size_t len, uint64_t *up, size_t n,
                            precompute_chars_convert_t *pre, uint64_t *stk) {
    WJR_ASSERT_ASSUME(n >= 1);
    if (n < dc_bignum_to_chars_threshold) {
        return basecase_to_chars(first, len, up, n, pre->base);
    } else {
        const auto pp = pre->ptr;
        const auto pn = pre->n;
        const auto ps = pre->shift;

        WJR_ASSERT((pn + ps) * 5 >= n * 2);

        if (n < pn + ps || (n == pn + ps && reverse_compare_n(up + ps, pp, pn) < 0)) {
            return dc_to_chars(first, len, up, n, pre - 1, stk);
        }

        const auto pd = pre->digits_in_base;
        auto qp = stk;

        div_qr_s(qp, up + ps, up + ps, n - ps, pp, pn);

        size_t qn = n - pn - ps;
        qn += qp[qn] != 0;

        if (len != 0) {
            len = len - pd;
        }

        pre -= qn * 2 <= n;

        first = dc_to_chars(first, len, qp, qn, pre, stk + qn);
        first = dc_to_chars(first, pd, up, pn + ps, pre, stk);
        return first;
    }
}

inline uint8_t *__biginteger_basecase_to_chars(uint8_t *first, const uint64_t *up,
                                               size_t n, unsigned int base) {
    if (WJR_LIKELY(n < dc_bignum_to_chars_precompute_threshold)) {
        uint64_t upbuf[dc_bignum_to_chars_precompute_threshold];
        std::copy_n(up, n, upbuf);
        return basecase_to_chars(first, 0, upbuf, n, base);
    }

    precompute_chars_convert_t pre[64 - 3];

    unique_stack_allocator stkal(math_details::stack_alloc);
    auto stk =
        static_cast<uint64_t *>(stkal.allocate((n * 18 / 5 + 192) * sizeof(uint64_t)));
    auto __up = stk;
    std::copy_n(up, n, __up);
    stk += n;
    auto mpre = precompute_chars_convert(pre, n, base, stk);
    stk += n * 8 / 5 + 128;
    return dc_to_chars(first, 0, __up, n, mpre, stk);
}

inline uint8_t *__biginteger_large_to_chars_impl(uint8_t *first, const uint64_t *up,
                                                 size_t n, unsigned int base = 10) {
    switch (base) {
    case 2: {
        return first + __biginteger_to_chars_2_impl(first, up, n);
    }
    case 8: {
        return first + __biginteger_to_chars_8_impl(first, up, n);
    }
    case 16: {
        return first + __biginteger_to_chars_16_impl(first, up, n);
    }
    case 4:
    case 32: {
        return first + __biginteger_to_chars_power_of_two_impl(first, up, n, base);
    }
    default: {
        break;
    }
    }

    return __biginteger_basecase_to_chars(first, up, n, base);
}

inline uint8_t *__biginteger_to_chars_impl(uint8_t *first, const uint64_t *up, size_t n,
                                           unsigned int base = 10) {
    if (n == 1) {
        return to_chars(first, up[0], base);
    }

    return __biginteger_large_to_chars_impl(first, up, n, base);
}

/**
 * @brief Convert a biginteger to a string by a given base.
 *
 * @tparam Iter Output iterator type
 * @param[out] first Output iterator
 * @param[in] up Pointer to the biginteger
 * @param[in] n Length of the biginteger
 * @param[in] base Base of the output string. Range: `[2, 36]`,
 * Only support 10 and power of two currently.
 * @return Output iterator after the conversion
 */
template <typename Iter, std::enable_if_t<__is_fast_convert_iterator_v<Iter>, int> = 0>
Iter biginteger_to_chars(Iter first, const uint64_t *up, size_t n,
                         unsigned int base = 10) {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));
    WJR_ASSERT_ASSUME(up[n - 1] != 0);

    auto __first = reinterpret_cast<uint8_t *>(to_address(first));
    auto ret = __biginteger_to_chars_impl(__first, up, n, base);
    return first + std::distance(__first, ret);
}

template <uint64_t Base>
struct __unsigned_from_chars_fn {};

template <uint64_t Base>
inline constexpr __unsigned_from_chars_fn<Base> __unsigned_from_chars{};

template <>
struct __unsigned_from_chars_fn<2> {
    template <typename UnsignedValue,
              std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
    void operator()(const uint8_t *first, const uint8_t *last, UnsignedValue &val) const {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;

        auto n = std::distance(first, last);
        WJR_ASSERT_ASSUME(1 <= n && n <= nd);

        UnsignedValue xval = 0;

        if constexpr (nd >= 16) {
            if (WJR_UNLIKELY(n >= 8)) {
                do {
                    xval = (xval << 8) + __from_chars_unroll_8<2>(first);
                    first += 8;
                    n -= 8;
                } while (WJR_LIKELY(n >= 8));

                if (n == 0) {
                    val = xval;
                    return;
                }
            }
        } else if constexpr (nd == 8) {
            if (WJR_UNLIKELY(n == 8)) {
                val = __from_chars_unroll_8<2>(first);
                first += 8;
                return;
            }
        }

        switch (n) {
        case 7: {
            xval = (xval << 1) + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 6: {
            xval = (xval << 1) + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 5: {
            xval = (xval << 1) + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 4: {
            xval <<= 4;
            xval += __from_chars_unroll_4<2>(first);
            first += 4;
            break;
        }
        case 3: {
            xval = (xval << 1) + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 2: {
            xval = (xval << 1) + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 1: {
            xval = (xval << 1) + convert_details::from(*first++);
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        val = xval;
    }
};

template <>
struct __unsigned_from_chars_fn<8> {
    template <typename UnsignedValue,
              std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
    void operator()(const uint8_t *first, const uint8_t *last, UnsignedValue &val) const {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;

        auto n = std::distance(first, last);
        WJR_ASSERT_ASSUME(1 <= n && n <= (nd + 2) / 3);

        UnsignedValue xval = 0;

        if constexpr (nd >= 32) {
            if (WJR_UNLIKELY(n >= 8)) {
                do {
                    xval = (xval << 24) + __from_chars_unroll_8<8>(first);
                    first += 8;
                    n -= 8;
                } while (WJR_LIKELY(n >= 8));

                if (n == 0) {
                    val = xval;
                    return;
                }
            }
        }

        switch (n) {
        case 7: {
            xval = (xval << 3) + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 6: {
            xval = (xval << 3) + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 5: {
            xval = (xval << 3) + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 4: {
            xval <<= 12;
            xval += __from_chars_unroll_4<8>(first);
            first += 4;
            break;
        }
        case 3: {
            xval = (xval << 3) + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 2: {
            xval = (xval << 3) + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 1: {
            xval = (xval << 3) + convert_details::from(*first++);
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        val = xval;
    }
};

template <>
struct __unsigned_from_chars_fn<16> {
    template <typename UnsignedValue,
              std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
    void operator()(const uint8_t *first, const uint8_t *last, UnsignedValue &val) const {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;

        auto n = std::distance(first, last);
        WJR_ASSERT_ASSUME(1 <= n && n <= (nd + 3) / 4);

        UnsignedValue xval = 0;

        if constexpr (nd >= 64) {
            if (WJR_UNLIKELY(n >= 8)) {
                do {
                    xval = (xval << 32) + __from_chars_unroll_8<16>(first);
                    first += 8;
                    n -= 8;
                } while (WJR_LIKELY(n >= 8));

                if (n == 0) {
                    val = xval;
                    return;
                }
            }
        } else if constexpr (nd == 32) {
            if (WJR_UNLIKELY(n == 8)) {
                val = __from_chars_unroll_8<16>(first);
                first += 8;
                return;
            }
        }

        switch (n) {
        case 7: {
            xval = (xval << 4) + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 6: {
            xval = (xval << 4) + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 5: {
            xval = (xval << 4) + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 4: {
            xval <<= 16;
            xval += __from_chars_unroll_4<16>(first);
            first += 4;
            break;
        }
        case 3: {
            xval = (xval << 4) + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 2: {
            xval = (xval << 4) + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 1: {
            xval = (xval << 4) + convert_details::from(*first++);
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        val = xval;
    }
};

template <>
struct __unsigned_from_chars_fn<1> {};

template <>
struct __unsigned_from_chars_fn<10> {
    template <typename UnsignedValue,
              std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
    void operator()(const uint8_t *first, const uint8_t *last, UnsignedValue &val) const {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits10 + 1;

        auto n = std::distance(first, last);
        WJR_ASSUME(1 <= n && n <= nd);

        UnsignedValue xval = 0;

        if constexpr (nd >= 8) {
            if (WJR_UNLIKELY(n >= 8)) {
                if (WJR_UNLIKELY(n >= 16)) {
                    xval = __from_chars_unroll_16<10>(first);
                    first += 16;
                    n -= 16;
                } else {
                    xval = __from_chars_unroll_8<10>(first);
                    first += 8;
                    n -= 8;
                }

                if (WJR_UNLIKELY(n == 0)) {
                    val = xval;
                    return;
                }
            }
        }

        switch (n) {
        case 7: {
            xval = xval * 10 + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 6: {
            xval = xval * 10 + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 5: {
            xval = xval * 10 + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 4: {
            xval = (xval * 10000) + __from_chars_unroll_4<10>(first);
            break;
        }
        case 3: {
            xval = xval * 10 + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 2: {
            xval = xval * 10 + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 1: {
            xval = xval * 10 + convert_details::from(*first++);
            WJR_FALLTHROUGH;
        }
        case 0: {
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        val = xval;
    }
};

template <typename Value, typename IBase,
          std::enable_if_t<is_nonbool_integral_v<Value>, int> = 0>
void __fast_from_chars_impl(const uint8_t *first, const uint8_t *last, Value &val,
                            IBase ibase) {
    int sign = 0;

    if constexpr (std::is_signed_v<Value>) {
        WJR_ASSERT(first != last);

        if (*first == '-') {
            ++first;
            sign = 1;
        }
    }

    std::make_unsigned_t<Value> uVal = 0;

    const unsigned int base = ibase;

    switch (base) {
    case 2: {
        __unsigned_from_chars<2>(first, last, uVal);
        break;
    }
    case 8: {
        __unsigned_from_chars<8>(first, last, uVal);
        break;
    }
    case 16: {
        __unsigned_from_chars<16>(first, last, uVal);
        break;
    }
    case 10: {
        __unsigned_from_chars<10>(first, last, uVal);
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

    if (sign) {
        val = -static_cast<Value>(uVal);
    } else {
        val = static_cast<Value>(uVal);
    }
}

template <typename Iter, typename Value, typename IBase,
          std::enable_if_t<is_nonbool_integral_v<Value>, int> = 0>
void __from_chars_impl(Iter first, Iter last, Value &val, IBase ibase) {
    auto __first = reinterpret_cast<const uint8_t *>(to_address(first));
    auto __last = reinterpret_cast<const uint8_t *>(to_address(last));
    __fast_from_chars_impl(__first, __last, val, ibase);
}

template <
    typename Iter, typename Value, typename BaseType = unsigned int, BaseType IBase = 10,
    std::enable_if_t<__is_fast_convert_iterator_v<Iter> && is_nonbool_integral_v<Value>,
                     int> = 0>
void from_chars(Iter first, Iter last, Value &val,
                std::integral_constant<BaseType, IBase> = {}) {
    __from_chars_impl(first, last, val, std::integral_constant<unsigned int, IBase>());
}

template <
    typename Iter, typename Value,
    std::enable_if_t<__is_fast_convert_iterator_v<Iter> && is_nonbool_integral_v<Value>,
                     int> = 0>
void from_chars(Iter first, Iter last, Value &val, unsigned int base) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            __from_chars_impl(first, last, val,
                              std::integral_constant<unsigned int, 2>());
            return;
        }
        case 8: {
            __from_chars_impl(first, last, val,
                              std::integral_constant<unsigned int, 8>());
            return;
        }
        case 16: {
            __from_chars_impl(first, last, val,
                              std::integral_constant<unsigned int, 16>());
            return;
        }
        case 10: {
            __from_chars_impl(first, last, val,
                              std::integral_constant<unsigned int, 10>());
            return;
        }
        default: {
            break;
        }
        }
    }

    __from_chars_impl(first, last, val, base);
}

inline size_t __biginteger_from_chars_2_impl(const uint8_t *first, size_t n,
                                             uint64_t *up) {
    size_t hbits = (n - 1) % 64 + 1;
    size_t len = (n - 1) / 64 + 1;

    uint64_t x = 0;
    up += len;

    __unsigned_from_chars<2>(first, first + hbits, x);
    first += hbits;

    *--up = x;

    size_t idx = len - 1;

    if (idx) {
        do {
            x = 0;

            for (int i = 0; i < 4; ++i) {
                x = (x << 16) + __from_chars_unroll_16<2>(first);
                first += 16;
            }

            *--up = x;
        } while (WJR_LIKELY(--idx));
    }

    return len;
}

inline size_t __biginteger_from_chars_8_impl(const uint8_t *first, size_t n,
                                             uint64_t *up) {
    size_t len = (n * 3 + 63) / 64;
    size_t lbits = (64 * (len - 1)) / 3;
    size_t rest = (64 * (len - 1)) % 3;
    size_t hbits = n - lbits - 1;

    auto unroll = [](uint64_t &x, auto &first) {
        auto x0 = convert_details::from(first[0]);
        auto x1 = convert_details::from(first[1]);
        auto x2 = convert_details::from(first[2]);
        auto x3 = convert_details::from(first[3]);

        x = x << 12 | x0 << 9 | x1 << 6 | x2 << 3 | x3;
        first += 4;
    };

    uint64_t x = 0;
    up += len;
    size_t idx = len - 1;

    if (WJR_UNLIKELY(hbits == 0)) {
    } else {
        __unsigned_from_chars<8>(first, first + hbits, x);
        first += hbits;
    }

    uint64_t nx = convert_details::from(*first++);
    switch (rest) {
    case 0: {
        *--up = x << 3 | nx;
        x = 0;
        break;
    }
    case 1: {
        x = x << 2 | nx >> 1;
        if (WJR_UNLIKELY(x == 0)) {
            --len;
        }

        *--up = x;
        x = nx & 1;
        break;
    }
    case 2: {
        x = x << 1 | nx >> 2;
        if (WJR_UNLIKELY(x == 0)) {
            --len;
        }
        *--up = x;
        x = nx & 3;
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

    if (idx) {
        do {
            for (int i = 0; i < 5; ++i) {
                unroll(x, first);
            }

            switch (rest) {
            case 0: {
                x = x << 3 | convert_details::from(*first++);
                uint64_t nx = convert_details::from(*first++);
                x = x << 1 | nx >> 2;
                *--up = x;
                x = nx & 3;
                rest = 2;
                break;
            }
            case 1: {
                x = x << 3 | convert_details::from(*first++);
                *--up = x;
                x = 0;
                rest = 0;
                break;
            }
            case 2: {
                uint64_t nx = convert_details::from(*first++);
                x = x << 2 | nx >> 1;
                *--up = x;
                x = nx & 1;
                rest = 1;
                break;
            }
            default: {
                WJR_UNREACHABLE();
                break;
            }
            }

        } while (WJR_LIKELY(--idx));
    }

    return len;
}

inline size_t __biginteger_from_chars_16_impl(const uint8_t *first, size_t n,
                                              uint64_t *up) {
    size_t hbits = (n - 1) % 16 + 1;
    size_t len = (n - 1) / 16 + 1;

    auto unroll = [](uint64_t &x, auto &first) {
        auto x0 = convert_details::from(first[0]);
        auto x1 = convert_details::from(first[1]);
        auto x2 = convert_details::from(first[2]);
        auto x3 = convert_details::from(first[3]);

        x = x << 16 | x0 << 12 | x1 << 8 | x2 << 4 | x3;
        first += 4;
    };

    uint64_t x = 0;
    up += len;

    __unsigned_from_chars<16>(first, first + hbits, x);
    first += hbits;

    *--up = x;

    size_t idx = len - 1;

    if (idx) {
        do {
            x = 0;

            for (int i = 0; i < 4; ++i) {
                unroll(x, first);
            }

            *--up = x;
        } while (WJR_LIKELY(--idx));
    }

    return len;
}

inline size_t basecase_from_chars_10(const uint8_t *first, size_t n, uint64_t *up) {
    uint64_t x = 0;

    if (n <= 19) {
        __unsigned_from_chars<10>(first, first + n, x);
        up[0] = x;

        return up[0] != 0;
    }

    size_t m = (n - 1) % 19 + 1;

    __unsigned_from_chars<10>(first, first + m, x);
    up[0] = x;

    first += m;
    n -= m;

    m = up[0] != 0;

    do {
        x = 0;

        x = __from_chars_unroll_16<10>(first);
        first += 16;

        x = x * 10 + convert_details::from(*first++);
        x = x * 10 + convert_details::from(*first++);
        x = x * 10 + convert_details::from(*first++);

        uint64_t cf;

        if (WJR_LIKELY(m == 0)) {
            cf = x;
        } else {
            cf = mul_1(up, up, m, div2by1_divider_noshift_of_big_base_10.get_divisor());
            cf += addc_1(up, up, m, x);
        }

        if (WJR_LIKELY(cf != 0)) {
            up[m++] = cf;
        }

        n -= 19;
    } while (WJR_LIKELY(n != 0));

    return m;
}

inline size_t basecase_from_chars(const uint8_t *first, size_t n, uint64_t *up,
                                  unsigned int base) {
    if (base == 10) {
        return basecase_from_chars_10(first, n, up);
    } else {
        return 0;
    }
}

inline size_t dc_from_chars(const uint8_t *first, size_t n, uint64_t *up,
                            precompute_chars_convert_t *pre, uint64_t *stk) {
    size_t lo = pre->digits_in_base;
    if (n <= lo) {
        if (n < dc_bignum_from_chars_threshold) {
            return basecase_from_chars(first, n, up, pre->base);
        } else {
            return dc_from_chars(first, n, up, pre - 1, stk);
        }
    }

    size_t hi = n - lo;
    size_t hn, ln;

    if (hi < dc_bignum_from_chars_threshold) {
        hn = basecase_from_chars(first, hi, stk, pre->base);
    } else {
        hn = dc_from_chars(first, hi, stk, pre - (lo * 2 >= n), up);
    }

    size_t ps = pre->shift;
    size_t pn = pre->n;

    if (WJR_LIKELY(hn != 0)) {
        if (pn >= hn) {
            mul_s(up + ps, pre->ptr, pn, stk, hn);
        } else {
            mul_s(up + ps, stk, hn, pre->ptr, pn);
        }
        set_n(up, 0, ps);
    }

    std::advance(first, hi);
    if (lo < dc_bignum_from_chars_threshold) {
        ln = basecase_from_chars(first, lo, stk, pre->base);
    } else {
        ln = dc_from_chars(first, lo, stk, pre - (lo * 2 >= n), stk + pn + ps + 1);
    }

    WJR_ASSERT(ps + pn + 1 >= ln);

    if (WJR_LIKELY(hn != 0)) {
        if (WJR_LIKELY(ln != 0)) {
            auto cf = addc_s(up, up, ps + pn + hn, stk, ln);
            WJR_ASSERT(cf == 0);
            (void)(cf);
        }

        n = ps + pn + hn;
        return n - (up[n - 1] == 0);
    }

    if (WJR_LIKELY(ln != 0)) {
        std::copy_n(stk, ln, up);
    }

    return ln;
}

inline uint64_t *__biginteger_from_chars_impl(const uint8_t *first, const uint8_t *last,
                                              uint64_t *up, unsigned int base = 10) {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));

    size_t n = std::distance(first, last);

    if (is_zero_or_single_bit(base)) {
        switch (base) {
        case 2: {
            return up + __biginteger_from_chars_2_impl(first, n, up);
        }
        case 8: {
            return up + __biginteger_from_chars_8_impl(first, n, up);
        }
        case 16: {
            return up + __biginteger_from_chars_16_impl(first, n, up);
        }
        default: {
            WJR_UNREACHABLE();
            return up;
        }
        }
    }

    if (WJR_LIKELY(n < dc_bignum_from_chars_precompute_threshold)) {
        return up + basecase_from_chars(first, n, up, base);
    }

    const auto per_digits = precompute_chars_convert_16n_ptr[base]->digits_in_one_base;

    precompute_chars_convert_t pre[64 - 3];

    unique_stack_allocator stkal(math_details::stack_alloc);
    size_t un = n / per_digits + 1;
    auto stk =
        static_cast<uint64_t *>(stkal.allocate((un * 16 / 5 + 192) * sizeof(uint64_t)));
    auto mpre = precompute_chars_convert(pre, un, base, stk);
    stk += un * 8 / 5 + 128;
    return up + dc_from_chars(first, n, up, mpre, stk);
}

/**
 * @brief Convert a string to a biginteger by a given base.
 *
 * @tparam Iter Input iterator type
 * @param[in] first Input iterator
 * @param[in] last Input iterator
 * @param[out] up Pointer to the biginteger
 * @param[in] base Base of the input string. Range: `[2, 36]`,
 * Only support 10 and power of two currently.
 * @return uint64_t* Pointer after the conversion
 */
template <typename Iter, std::enable_if_t<__is_fast_convert_iterator_v<Iter>, int> = 0>
uint64_t *biginteger_from_chars(Iter first, Iter last, uint64_t *up,
                                unsigned int base = 10) {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));

    auto __first = reinterpret_cast<uint8_t *>(to_address(first));
    auto __last = reinterpret_cast<uint8_t *>(to_address(last));

    return __biginteger_from_chars_impl(__first, __last, up, base);
}

} // namespace wjr

#endif // WJR_MATH_CONVERT_HPP__