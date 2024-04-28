#ifndef WJR_MATH_CONVERT_HPP__
#define WJR_MATH_CONVERT_HPP__

#include <array>
#include <system_error>

#include <wjr/assert.hpp>
#include <wjr/container/generic/type_traits.hpp>
#include <wjr/math/bit.hpp>
#include <wjr/math/convert-impl.hpp>
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

WJR_CONST constexpr bool __isspace(uint8_t ch) { return char_converter.from(ch) == 64; }

template <typename T>
struct __is_fast_convert_value
    : std::conjunction<std::is_trivial<T>, std::bool_constant<sizeof(T) == 1>> {};

template <typename T>
inline constexpr bool __is_fast_convert_value_v = __is_fast_convert_value<T>::value;

template <typename Iter, typename = void>
struct __is_fast_convert_iterator_helper : std::false_type {};

template <typename Iter>
struct __is_fast_convert_iterator_helper<Iter,
                                         std::enable_if_t<is_contiguous_iterator_v<Iter>>>
    : __is_fast_convert_value<iterator_contiguous_value_t<Iter>> {};

template <typename Iter>
struct __is_fast_convert_iterator : __is_fast_convert_iterator_helper<Iter> {};

/**
 * @brief Iterator concept that can be used in fast_convert.
 *
 * @details The iterator must be contiguous iterator and the value_type must be
 * trivial and sizeof(value_type) == 1. Cast to_address(iter) to uint8_t*(to_chars)/const
 * uint8_t*(from_chars) in fast_convert.
 *
 */
template <typename Iter>
inline constexpr bool __is_fast_convert_iterator_v =
    __is_fast_convert_iterator<Iter>::value;

template <typename Value, typename Converter>
struct __is_valid_converter
    : std::disjunction<std::conjunction<std::is_same<Converter, char_converter_t>,
                                        is_nonbool_integral<Value>>,
                       std::conjunction<std::is_same<Converter, origin_converter_t>,
                                        is_nonbool_unsigned_integral<Value>>> {};

template <typename Value, typename Converter>
inline constexpr bool __is_valid_converter_v =
    __is_valid_converter<Value, Converter>::value;

WJR_REGISTER_HAS_TYPE(to_chars_fast_fn_fast_conv,
                      Base::__fast_conv(std::declval<void *>(), std::declval<Args>()...),
                      Base);

WJR_REGISTER_HAS_TYPE(from_chars_fast_fn_fast_conv,
                      Base::__fast_conv(std::declval<const void *>(),
                                        std::declval<Args>()...),
                      Base);

template <typename Iter, typename = void>
struct fast_buffer {
private:
    using value_type = iterator_value_t<Iter>;

public:
    using type =
        std::conditional_t<__is_fast_convert_value_v<value_type>, value_type, char>;
};

// back_inserter or inserter
template <typename Iter>
struct fast_buffer<Iter, std::void_t<decltype(std::declval<std::enable_if_t<
                                                  is_any_insert_iterator_v<Iter>>>())>> {
private:
    using value_type = typename Iter::container_type::value_type;

public:
    using type =
        std::conditional_t<__is_fast_convert_value_v<value_type>, value_type, char>;
};

template <typename Iter>
using fast_buffer_t = typename fast_buffer<Iter>::type;

template <typename Container>
struct __fast_container_inserter_test {
private:
    using traits_type = container_traits<Container>;

public:
    static constexpr int value =
        traits_type::is_trivially_contiguous_v &&
                has_container_resize_v<Container, size_t>
            ? (has_container_resize_v<Container, size_t, dctor_t> ? 2 : 1)
            : 0;

    static_assert(value != 2 || has_container_append_v<Container, size_t, dctor_t>, "");
};

template <typename Iter, typename = void>
struct __is_fast_container_inserter {
    static constexpr int value = 0;
};

template <typename Iter>
struct __is_fast_container_inserter<
    Iter,
    std::void_t<
        decltype(std::declval<std::enable_if_t<is_any_insert_iterator_v<Iter>>>(),
                 __fast_container_inserter_test<typename Iter::container_type>::value)>> {
private:
    using container_type = typename Iter::container_type;

public:
    static constexpr int value =
        __is_fast_convert_value_v<typename container_type::value_type> &&
                is_trivially_allocator_constructible_v<
                    typename container_type::allocator_type>
            ? __fast_container_inserter_test<container_type>::value
            : 0;
};

template <typename Iter>
inline constexpr int is_fast_container_inserter_v =
    __is_fast_container_inserter<Iter>::value;

} // namespace convert_details

// require operator() of Converter is constexpr
template <typename Converter, uint64_t Base, int Unroll>
class __char_converter_table_t {
    static constexpr uint64_t pw2 = Unroll == 2 ? Base * Base : Base * Base * Base * Base;

public:
    constexpr __char_converter_table_t() : table() {
        for (uint64_t i = 0, j = 0; i < pw2; ++i, j += Unroll) {
            int x = i;
            for (int k = Unroll - 1; ~k; --k) {
                table[j + k] = Converter::to(x % Base);
                x /= Base;
            }
        }
    }

    WJR_CONST constexpr char operator[](unsigned int idx) const { return table[idx]; }

    WJR_CONST constexpr const char *data() const { return table.data(); }

private:
    std::array<char, pw2 * Unroll> table;
};

template <typename Converter, uint64_t Base, int Unroll>
inline constexpr __char_converter_table_t<Converter, Base, Unroll>
    __char_converter_table{};

template <uint64_t Base>
class __to_chars_unroll_2_fast_fn_impl_base {
public:
    template <typename Converter>
    WJR_INTRINSIC_INLINE static void __fast_conv(void *ptr, uint32_t val, Converter) {
        auto str = (char *)ptr;
        if constexpr (Base * Base <= 16) {
            constexpr auto &table = __char_converter_table<Converter, Base, 4>;
            std::memcpy(str, table.data() + val * 4 + 2, 2);
        } else {
            constexpr auto &table = __char_converter_table<Converter, Base, 2>;
            std::memcpy(str, table.data() + val * 2, 2);
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
    template <typename Converter>
    WJR_INTRINSIC_INLINE static void __fast_conv(void *ptr, uint32_t val, Converter) {
        auto str = (char *)ptr;
        if constexpr (Base * Base <= 16) {
            constexpr auto &table = __char_converter_table<Converter, Base, 4>;
            std::memcpy(str, table.data() + val * 4, 4);
        } else {
            constexpr auto &table = __char_converter_table<Converter, Base, 2>;
            constexpr auto Base2 = Base * Base;
            const uint32_t hi = val / Base2;
            const uint32_t lo = val % Base2;

            std::memcpy(str, table.data() + hi * 2, 2);
            std::memcpy(str + 2, table.data() + lo * 2, 2);
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
    template <typename Converter>
    WJR_INTRINSIC_INLINE static void __fast_conv(void *ptr, uint64_t val,
                                                 Converter conv) {
        builtin_to_chars_unroll_8_fast<Base>(ptr, val, conv);
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
    template <typename Converter>
    WJR_INTRINSIC_INLINE void operator()(uint8_t *ptr, uint32_t val,
                                         Converter conv) const {
        if constexpr (convert_details::has_to_chars_fast_fn_fast_conv_v<Mybase, uint32_t,
                                                                        Converter>) {
            Mybase::__fast_conv(ptr, val, conv);
        } else {
            ptr[0] = conv.template to<Base>(val / Base);
            ptr[1] = conv.template to<Base>(val % Base);
        }
    }
};

template <uint64_t Base>
inline constexpr __to_chars_unroll_2_fn<Base> __to_chars_unroll_2{};

template <uint64_t Base>
class __to_chars_unroll_4_fn_impl : public __to_chars_unroll_4_fast_fn_impl<Base> {
    using Mybase = __to_chars_unroll_4_fast_fn_impl<Base>;

public:
    template <typename Converter>
    WJR_INTRINSIC_INLINE void operator()(uint8_t *ptr, uint32_t val,
                                         Converter conv) const {
        if constexpr (convert_details::has_to_chars_fast_fn_fast_conv_v<Mybase, uint32_t,
                                                                        Converter>) {
            Mybase::__fast_conv(ptr, val, conv);
        } else {
            constexpr auto Base2 = Base * Base;
            __to_chars_unroll_2<Base>(ptr, val / Base2, conv);
            __to_chars_unroll_2<Base>(ptr + 2, val % Base2, conv);
        }
    }
};

template <uint64_t Base>
inline constexpr __to_chars_unroll_4_fn_impl<Base> __to_chars_unroll_4{};

template <uint64_t Base>
class __to_chars_unroll_8_fn_impl : public __to_chars_unroll_8_fast_fn_impl<Base> {
    using Mybase = __to_chars_unroll_8_fast_fn_impl<Base>;

public:
    template <typename Converter>
    WJR_INTRINSIC_INLINE void operator()(uint8_t *ptr, uint64_t val,
                                         Converter conv) const {
        if constexpr (convert_details::has_to_chars_fast_fn_fast_conv_v<Mybase, uint64_t,
                                                                        Converter>) {
            Mybase::__fast_conv(ptr, val, conv);
        } else {
            constexpr auto Base4 = Base * Base * Base * Base;
            __to_chars_unroll_4<Base>(ptr, val / Base4, conv);
            __to_chars_unroll_4<Base>(ptr + 4, val % Base4, conv);
        }
    }
};

template <uint64_t Base>
inline constexpr __to_chars_unroll_8_fn_impl<Base> __to_chars_unroll_8{};

template <uint64_t Base>
class __from_chars_unroll_4_fast_fn_impl_base {
protected:
    WJR_CONST WJR_INTRINSIC_INLINE static uint32_t __fast_conv(uint32_t val) {
        constexpr uint32_t Base2 = Base * Base;

        constexpr uint32_t mask = 0x00FF00FF;
        constexpr uint32_t mul = 1 + (Base2 << 16);
        val = (val * Base) + (val >> 8);
        val = ((val & mask) * mul) >> 16;
        return uint32_t(val);
    }

public:
    WJR_PURE WJR_INTRINSIC_INLINE static uint32_t __fast_conv(const void *ptr,
                                                              char_converter_t) {
        return __fast_conv(read_memory<uint32_t>(ptr) - 0x30303030u);
    }

    WJR_PURE WJR_INTRINSIC_INLINE static uint32_t __fast_conv(const void *ptr,
                                                              origin_converter_t) {
        return __fast_conv(read_memory<uint32_t>(ptr));
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
    WJR_PURE WJR_INTRINSIC_INLINE static uint32_t
    __fast_conv(const void *ptr, WJR_MAYBE_UNUSED char_converter_t conv) {
#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_8_FAST)
        return builtin_from_chars_unroll_8_fast<Base>(ptr, conv);
#else
        return __fast_conv(read_memory<uint64_t>(ptr) - 0x3030303030303030ull);
#endif
    }

    WJR_PURE WJR_INTRINSIC_INLINE static uint32_t
    __fast_conv(const void *ptr, WJR_MAYBE_UNUSED origin_converter_t conv) {
#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_8_FAST)
        return builtin_from_chars_unroll_8_fast<Base>(ptr, conv);
#else
        return __fast_conv(read_memory<uint64_t>(ptr));
#endif
    }
};

template <uint64_t Base>
class __from_chars_unroll_16_fast_fn_impl_base {
#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_16_FAST)
public:
    template <typename Converter>
    WJR_PURE WJR_INTRINSIC_INLINE static uint64_t __fast_conv(const void *ptr,
                                                              Converter conv) {
        return builtin_from_chars_unroll_16_fast<Base>(ptr, conv);
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
    template <typename Converter>
    WJR_PURE WJR_INTRINSIC_INLINE uint64_t operator()(const uint8_t *ptr,
                                                      Converter conv) const {
        if constexpr (convert_details::has_from_chars_fast_fn_fast_conv_v<Mybase,
                                                                          Converter>) {
            return Mybase::__fast_conv(ptr, conv);
        } else {
            uint64_t value = 0;
            value = conv.template from<Base>(*ptr++);
            value = value * Base + conv.template from<Base>(*ptr++);
            value = value * Base + conv.template from<Base>(*ptr++);
            return value * Base + conv.template from<Base>(*ptr++);
        }
    }
};

template <uint64_t Base>
inline constexpr __from_chars_unroll_4_fn<Base> __from_chars_unroll_4{};

template <uint64_t Base>
class __from_chars_unroll_8_fn : public __from_chars_unroll_8_fast_fn_impl<Base> {
    using Mybase = __from_chars_unroll_8_fast_fn_impl<Base>;

public:
    template <typename Converter>
    WJR_PURE WJR_INTRINSIC_INLINE uint64_t operator()(const uint8_t *ptr,
                                                      Converter conv) const {
        if constexpr (convert_details::has_from_chars_fast_fn_fast_conv_v<Mybase,
                                                                          Converter>) {
            return Mybase::__fast_conv(ptr, conv);
        } else {
            constexpr uint64_t Base4 = Base * Base * Base * Base;
            return __from_chars_unroll_4<Base>(ptr, conv) * Base4 +
                   __from_chars_unroll_4<Base>(ptr + 4, conv);
        }
    }
};

template <uint64_t Base>
inline constexpr __from_chars_unroll_8_fn<Base> __from_chars_unroll_8{};

template <uint64_t Base>
class __from_chars_unroll_16_fn : public __from_chars_unroll_16_fast_fn_impl<Base> {
    using Mybase = __from_chars_unroll_16_fast_fn_impl<Base>;

public:
    template <typename Converter>
    WJR_PURE WJR_INTRINSIC_INLINE uint64_t operator()(const uint8_t *ptr,
                                                      Converter conv) const {
        if constexpr (convert_details::has_from_chars_fast_fn_fast_conv_v<Mybase,
                                                                          Converter>) {
            return Mybase::__fast_conv(ptr, conv);
        } else {
            constexpr uint64_t Base4 = Base * Base * Base * Base;
            constexpr uint64_t Base8 = Base4 * Base4;
            return __from_chars_unroll_8<Base>(ptr, conv) * Base8 +
                   __from_chars_unroll_8<Base>(ptr + 8, conv);
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
    const auto inc = table[clz(n | 1) ^ 31];
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
    const auto t = bsr2log10[clz(n | 1) ^ 63];
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
    template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
    WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int operator()(T n) const {
        return bit_width(n);
    }
};

template <>
struct count_digits_fn<8> {
    template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
    WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int operator()(T n) const {
        return __ceil_div(to_unsigned(bit_width(n)), 3);
    }
};

template <>
struct count_digits_fn<16> {
    template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
    WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int operator()(T n) const {
        return __ceil_div(to_unsigned(bit_width(n)), 4);
    }
};

template <>
struct count_digits_fn<1> {
    template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
    WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int operator()(T n, int bits) const {
        return (bit_width(n) + bits - 1) / bits;
    }
};

template <>
struct count_digits_fn<10> {
    template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
    WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int operator()(T n) const {
        const int ret = count_digits10_impl(n);
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

template <typename Iter = const char *>
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
class __unsigned_to_chars_backward_unchecked_fn {};

template <uint64_t Base>
inline constexpr __unsigned_to_chars_backward_unchecked_fn<Base>
    __unsigned_to_chars_backward_unchecked{};

template <>
class __unsigned_to_chars_backward_unchecked_fn<2> {
    template <typename UnsignedValue, typename Converter>
    static uint8_t *fn(uint8_t *ptr, int n, UnsignedValue x, Converter conv) {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;
        WJR_ASSERT_L1(x != 0);
        WJR_ASSERT_ASSUME(1 <= n && n <= nd);
        (void)(nd);

        if (WJR_UNLIKELY(n >= 4)) {
            do {
                __to_chars_unroll_4<2>(ptr - 4, x & 0x0f, conv);
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
            *--ptr = conv.template to<2>(x & 1);
            x >>= 1;
            WJR_FALLTHROUGH;
        }
        case 2: {
            __to_chars_unroll_2<2>(ptr - 2, x, conv);
            ptr -= 2;
            break;
        }
        case 1: {
            *--ptr = conv.template to<2>(x);
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        return ptr;
    }

public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    uint8_t *operator()(uint8_t *ptr, int n, UnsignedValue x, Converter conv) const {
        return fn(ptr, n, x, conv);
    }
};

template <>
class __unsigned_to_chars_backward_unchecked_fn<8> {
    template <typename UnsignedValue, typename Converter>
    static uint8_t *fn(uint8_t *ptr, int n, UnsignedValue x, Converter conv) {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;
        WJR_ASSERT_L1(x != 0);
        WJR_ASSERT_ASSUME(1 <= n && n <= (nd + 2) / 3);

        if constexpr (nd >= 16) {
            if (WJR_UNLIKELY(n >= 4)) {
                do {
                    __to_chars_unroll_4<8>(ptr - 4, x & 0x0fff, conv);
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
            *--ptr = conv.template to<8>(x & 0x07);
            x >>= 3;
            WJR_FALLTHROUGH;
        }
        case 2: {
            __to_chars_unroll_2<8>(ptr - 2, x, conv);
            ptr -= 2;
            break;
        }
        case 1: {
            *--ptr = conv.template to<8>(x);
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        return ptr;
    }

public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    uint8_t *operator()(uint8_t *ptr, int n, UnsignedValue x, Converter conv) const {
        return fn(ptr, n, x, conv);
    }
};

template <>
class __unsigned_to_chars_backward_unchecked_fn<16> {
    template <typename UnsignedValue, typename Converter>
    static uint8_t *fn(uint8_t *ptr, int n, UnsignedValue x, Converter conv) {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;
        WJR_ASSERT_L1(x != 0);
        WJR_ASSERT_ASSUME(1 <= n && n <= (nd + 3) / 4);

        if constexpr (nd >= 16) {
            if (WJR_UNLIKELY(n >= 4)) {
                do {
                    __to_chars_unroll_4<16>(ptr - 4, x & 0xffff, conv);
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
            *--ptr = conv.to(x & 0x0f);
            x >>= 4;
            WJR_FALLTHROUGH;
        }
        case 2: {
            __to_chars_unroll_2<16>(ptr - 2, x, conv);
            ptr -= 2;
            break;
        }
        case 1: {
            *--ptr = conv.to(x);
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        return ptr;
    }

public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    uint8_t *operator()(uint8_t *ptr, int n, UnsignedValue x, Converter conv) const {
        return fn(ptr, n, x, conv);
    }
};

template <>
class __unsigned_to_chars_backward_unchecked_fn<1> {
private:
    template <typename UnsignedValue, typename Converter>
    static uint8_t *fn(uint8_t *ptr, int n, UnsignedValue x, int bits, Converter conv) {
        WJR_ASSERT_L1(x != 0);
        WJR_ASSERT_ASSUME(1 <= n && n <= std::numeric_limits<UnsignedValue>::digits);

        const unsigned int mask = (1u << bits) - 1;

        do {
            *--ptr = conv.to(x & mask);
            x >>= bits;
            --n;
        } while (WJR_LIKELY(n != 0));

        return ptr;
    }

public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    uint8_t *operator()(uint8_t *ptr, int n, UnsignedValue x, int bits,
                        Converter conv) const {
        return fn(ptr, n, x, bits, conv);
    }
};

template <>
class __unsigned_to_chars_backward_unchecked_fn<10> {
private:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    static uint8_t *fn(uint8_t *ptr, UnsignedValue val, Converter conv) {
        WJR_ASSERT_ASSUME(val != 0);

        if (WJR_LIKELY(val >= 100)) {
            do {
                __to_chars_unroll_2<10>(ptr - 2, val % 100, conv);
                ptr -= 2;
                val /= 100;
            } while (WJR_LIKELY(val >= 100));
        }

        if (val < 10) {
            *--ptr = conv.template to<10>(val);
            return ptr;
        }

        __to_chars_unroll_2<10>(ptr - 2, val, conv);
        ptr -= 2;
        return ptr;
    }

public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    uint8_t *operator()(uint8_t *ptr, UnsignedValue val, Converter conv) const {
        return fn(ptr, val, conv);
    }
};

template <typename Value, typename IBase, typename Converter>
uint8_t *__fast_to_chars_backward_unchecked_impl(uint8_t *ptr, Value val, IBase ibase,
                                                 Converter conv) {
    if (WJR_UNLIKELY(val == 0)) {
        *--ptr = conv.template to<1>(0);
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
        ptr = __unsigned_to_chars_backward_unchecked<2>(ptr, count_digits<2>(uVal), uVal,
                                                        conv);
        break;
    }
    case 8: {
        ptr = __unsigned_to_chars_backward_unchecked<8>(ptr, count_digits<8>(uVal), uVal,
                                                        conv);
        break;
    }
    case 16: {
        ptr = __unsigned_to_chars_backward_unchecked<16>(ptr, count_digits<16>(uVal),
                                                         uVal, conv);
        break;
    }
    case 4:
    case 32: {
        const int bits = base == 4 ? 2 : 5;
        ptr = __unsigned_to_chars_backward_unchecked<1>(ptr, count_digits<1>(uVal, bits),
                                                        uVal, bits, conv);
        break;
    }
    case 10: {
        ptr = __unsigned_to_chars_backward_unchecked<10>(ptr, uVal, conv);
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

template <typename Iter, typename Value, typename IBase, typename Converter>
Iter __to_chars_backward_unchecked_impl(Iter first, Value val, IBase ibase,
                                        Converter conv) {
    const auto __ptr = reinterpret_cast<uint8_t *>((to_address)(first));
    const auto __end = __fast_to_chars_backward_unchecked_impl(__ptr, val, ibase, conv);
    return first + std::distance(__ptr, __end);
}

/**
 * @brief Convert an unsigned integer to a string in reverse order without checking
 * buf size.
 *
 * @details Only use fast_convert mode.
 *
 */
template <typename Iter, typename Value, typename BaseType = unsigned int,
          BaseType IBase = 10, typename Converter = char_converter_t,
          WJR_REQUIRES(convert_details::__is_fast_convert_iterator_v<Iter>
                           &&convert_details::__is_valid_converter_v<Value, Converter>)>
Iter to_chars_backward_unchecked(Iter first, Value val,
                                 std::integral_constant<BaseType, IBase> = {},
                                 Converter conv = {}) {
    return __to_chars_backward_unchecked_impl(
        first, val, std::integral_constant<unsigned int, IBase>(), conv);
}

/**
 * @brief Convert an unsigned integer to a string in reverse order without checking
 * buf size.
 *
 *
 */
template <typename Iter, typename Value, typename Converter = char_converter_t,
          WJR_REQUIRES(convert_details::__is_fast_convert_iterator_v<Iter>
                           &&convert_details::__is_valid_converter_v<Value, Converter>)>
Iter to_chars_backward_unchecked(Iter first, Value val, unsigned int base,
                                 Converter conv = {}) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return to_chars_backward_unchecked(
                first, val, std::integral_constant<unsigned int, 2>(), conv);
        }
        case 8: {
            return to_chars_backward_unchecked(
                first, val, std::integral_constant<unsigned int, 8>(), conv);
        }
        case 16: {
            return to_chars_backward_unchecked(
                first, val, std::integral_constant<unsigned int, 16>(), conv);
        }
        case 10: {
            return to_chars_backward_unchecked(
                first, val, std::integral_constant<unsigned int, 10>(), conv);
        }
        default: {
            break;
        }
        }
    }

    return __to_chars_backward_unchecked_impl(first, val, base, conv);
}

template <typename Value, typename IBase, typename Converter>
to_chars_result<uint8_t *> __fast_to_chars_impl(uint8_t *first, uint8_t *last, Value val,
                                                IBase ibase, Converter conv) {
    if (WJR_UNLIKELY(val == 0)) {
        if (WJR_UNLIKELY(first == last)) {
            return {last, std::errc::value_too_large};
        }

        *first++ = conv.template to<1>(0);
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
    const auto size = std::distance(first, last);

#define WJR_TO_CHARS_VALIDATE_IMPL(BASE, DIGITS, CALL)                                   \
    const int n = count_digits<BASE> DIGITS;                                             \
    if (WJR_LIKELY(n <= size)) {                                                         \
        first += n;                                                                      \
        (void)__unsigned_to_chars_backward_unchecked<BASE>(                              \
            first, WJR_PP_QUEUE_EXPAND(CALL), conv);                                     \
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
        const int bits = base == 4 ? 2 : 5;
        WJR_TO_CHARS_VALIDATE_IMPL(1, (uVal, bits), (n, uVal, bits));
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

template <typename Iter, typename Value, typename IBase, typename Converter>
to_chars_result<Iter> __fallback_to_chars_impl(Iter first, Iter last, Value val,
                                               IBase ibase, Converter conv) {
    constexpr auto is_signed = std::is_signed_v<Value>;
    constexpr auto base_2_table = std::numeric_limits<Value>::digits;
    constexpr auto base_10_table = std::numeric_limits<Value>::digits10 + 1;
    constexpr auto is_random_access = is_random_access_iterator_v<Iter>;

    if (WJR_UNLIKELY(val == 0)) {
        if (WJR_UNLIKELY(first == last)) {
            return {last, std::errc::value_too_large};
        }

        *first++ = conv.template to<1>(0);
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

#define WJR_TO_CHARS_VALIDATE_IMPL(BASE, TABLE, CALL)                                    \
    if constexpr (is_random_access) {                                                    \
        const auto size = std::distance(first, last);                                    \
        WJR_PP_QUEUE_EXPAND(                                                             \
            WJR_PP_BOOL_IF(WJR_PP_NE(BASE, 10),                                          \
                           (                                                             \
                               if constexpr (is_signed) {                                \
                                   if (WJR_UNLIKELY(n + sign > size)) {                  \
                                       return {last, std::errc::value_too_large};        \
                                   }                                                     \
                               } else {                                                  \
                                   if (WJR_UNLIKELY(n > size)) {                         \
                                       return {last, std::errc::value_too_large};        \
                                   }                                                     \
                               }),                                                       \
                           ()))                                                          \
                                                                                         \
        convert_details::fast_buffer_t<Iter> buffer[TABLE + is_signed];                  \
        const auto __end = buffer + TABLE + is_signed;                                   \
        auto __ptr = (convert_details::fast_buffer_t<Iter> *)                            \
            __unsigned_to_chars_backward_unchecked<BASE>(                                \
                (uint8_t *)__end, WJR_PP_QUEUE_EXPAND(CALL), conv);                      \
                                                                                         \
        WJR_PP_QUEUE_EXPAND(                                                             \
            WJR_PP_BOOL_IF(WJR_PP_EQ(BASE, 10),                                          \
                           (                                                             \
                               const auto n = __end - __ptr;                             \
                                                                                         \
                               if constexpr (is_signed) {                                \
                                   if (WJR_UNLIKELY(n + sign > size)) {                  \
                                       return {last, std::errc::value_too_large};        \
                                   }                                                     \
                               } else {                                                  \
                                   if (WJR_UNLIKELY(n > size)) {                         \
                                       return {last, std::errc::value_too_large};        \
                                   }                                                     \
                               }),                                                       \
                           ()))                                                          \
                                                                                         \
        if constexpr (is_signed) {                                                       \
            if (sign) {                                                                  \
                *--__ptr = '-';                                                          \
            }                                                                            \
        }                                                                                \
                                                                                         \
        return wjr::copy(__ptr, __end, first);                                           \
    } else {                                                                             \
        convert_details::fast_buffer_t<Iter> buffer[TABLE];                              \
        const auto __end = buffer + TABLE;                                               \
        auto __ptr = (convert_details::fast_buffer_t<Iter> *)                            \
            __unsigned_to_chars_backward_unchecked<BASE>(                                \
                (uint8_t *)__end, WJR_PP_QUEUE_EXPAND(CALL), conv);                      \
                                                                                         \
        do {                                                                             \
            if (WJR_UNLIKELY(first == last)) {                                           \
                return {last, std::errc::value_too_large};                               \
            }                                                                            \
                                                                                         \
            *first++ = *__ptr++;                                                         \
        } while (__ptr != __end);                                                        \
                                                                                         \
        return {first, std::errc{}};                                                     \
    }

    switch (base) {
    case 2: {
        const int n = count_digits<2>(uVal);
        WJR_TO_CHARS_VALIDATE_IMPL(2, base_2_table, (n, uVal));
    }
    case 8: {
        const int n = count_digits<8>(uVal);
        WJR_TO_CHARS_VALIDATE_IMPL(8, (base_2_table + 2) / 3, (n, uVal));
    }
    case 16: {
        const int n = count_digits<16>(uVal);
        WJR_TO_CHARS_VALIDATE_IMPL(16, (base_2_table + 3) / 4, (n, uVal));
    }
    case 4:
    case 32: {
        const int bits = base == 4 ? 2 : 5;
        const int n = count_digits<1>(uVal, bits);
        WJR_TO_CHARS_VALIDATE_IMPL(1, (base_2_table + bits - 1) / bits, (n, uVal, bits));
    }
    case 10: {
        WJR_TO_CHARS_VALIDATE_IMPL(10, base_10_table, (uVal));
    }
    default: {
        WJR_UNREACHABLE();
        return {last, std::errc::invalid_argument};
    }
    }

#undef WJR_TO_CHARS_VALIDATE_IMPL
}

template <typename Iter, typename Value, typename IBase, typename Converter>
to_chars_result<Iter> __to_chars_impl(Iter first, Iter last, Value val, IBase ibase,
                                      Converter conv) {
    if constexpr (convert_details::__is_fast_convert_iterator_v<Iter>) {
        const auto __first = reinterpret_cast<uint8_t *>((to_address)(first));
        const auto __last = reinterpret_cast<uint8_t *>((to_address)(last));
        const auto __result = __fast_to_chars_impl(__first, __last, val, ibase, conv);
        return {first + std::distance(__first, __result.ptr), __result.ec};
    } else {
        return __fallback_to_chars_impl(first, last, val, ibase, conv);
    }
}

template <typename Value, typename IBase, typename Converter>
uint8_t *__fast_to_chars_unchecked_impl(uint8_t *ptr, Value val, IBase ibase,
                                        Converter conv) {
    if (WJR_UNLIKELY(val == 0)) {
        *ptr++ = conv.template to<1>(0);
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
        const int n = count_digits<2>(uVal);
        ptr += n;
        (void)__unsigned_to_chars_backward_unchecked<2>(ptr, n, uVal, conv);
        return ptr;
    }
    case 8: {
        const int n = count_digits<8>(uVal);
        ptr += n;
        (void)__unsigned_to_chars_backward_unchecked<8>(ptr, n, uVal, conv);
        return ptr;
    }
    case 16: {
        const int n = count_digits<16>(uVal);
        ptr += n;
        (void)__unsigned_to_chars_backward_unchecked<16>(ptr, n, uVal, conv);
        return ptr;
    }
    case 4:
    case 32: {
        const int bits = base == 4 ? 2 : 5;
        const int n = count_digits<1>(uVal, bits);
        ptr += n;
        (void)__unsigned_to_chars_backward_unchecked<1>(ptr, n, uVal, bits, conv);
        return ptr;
    }
    case 10: {
        const int n = count_digits<10>(uVal);
        ptr += n;
        (void)__unsigned_to_chars_backward_unchecked<10>(ptr, uVal, conv);
        return ptr;
    }
    default: {
        WJR_UNREACHABLE();
        return ptr;
    }
    }
}

template <typename Iter, typename Value, typename IBase, typename Converter>
Iter __fallback_to_chars_unchecked_impl(Iter ptr, Value val, IBase ibase,
                                        Converter conv) {
    constexpr auto is_signed = std::is_signed_v<Value>;
    constexpr auto base_2_table = std::numeric_limits<Value>::digits;
    constexpr auto base_10_table = std::numeric_limits<Value>::digits10 + 1;

    if (WJR_UNLIKELY(val == 0)) {
        *ptr++ = conv.template to<1>(0);
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

#define WJR_TO_CHARS_IMPL(BASE, TABLE, CALL)                                             \
    constexpr auto __fast_container_inserter_v =                                         \
        convert_details::is_fast_container_inserter_v<Iter>;                             \
    if constexpr (__fast_container_inserter_v != 0) {                                    \
        WJR_PP_BOOL_IF(WJR_PP_EQ(BASE, 10), const int n = count_digits<10>(uVal), );     \
        auto &cont = get_inserter_container(ptr);                                        \
        if constexpr (__fast_container_inserter_v == 1) {                                \
            resize(cont, cont.size() + n + sign);                                        \
        } else {                                                                         \
            append(cont, n + sign, dctor);                                               \
        }                                                                                \
        const auto __end = (to_address)(cont.data() + cont.size());                      \
        auto __ptr = (convert_details::fast_buffer_t<Iter> *)                            \
            __unsigned_to_chars_backward_unchecked<BASE>(                                \
                (uint8_t *)__end, WJR_PP_QUEUE_EXPAND(CALL), conv);                      \
                                                                                         \
        if constexpr (is_signed) {                                                       \
            if (sign) {                                                                  \
                *--__ptr = '-';                                                          \
            }                                                                            \
        }                                                                                \
                                                                                         \
        return ptr;                                                                      \
    } else {                                                                             \
        convert_details::fast_buffer_t<Iter> buffer[TABLE + is_signed];                  \
        const auto __end = buffer + TABLE + is_signed;                                   \
        auto __ptr = (convert_details::fast_buffer_t<Iter> *)                            \
            __unsigned_to_chars_backward_unchecked<BASE>(                                \
                (uint8_t *)__end, WJR_PP_QUEUE_EXPAND(CALL), conv);                      \
                                                                                         \
        if constexpr (is_signed) {                                                       \
            if (sign) {                                                                  \
                *--__ptr = '-';                                                          \
            }                                                                            \
        }                                                                                \
                                                                                         \
        return wjr::copy(__ptr, __end, ptr);                                             \
    }

    switch (base) {
    case 2: {
        const int n = count_digits<2>(uVal);
        WJR_TO_CHARS_IMPL(2, base_2_table, (n, uVal));
    }
    case 8: {
        const int n = count_digits<8>(uVal);
        WJR_TO_CHARS_IMPL(8, (base_2_table + 2) / 3, (n, uVal));
    }
    case 16: {
        const int n = count_digits<16>(uVal);
        WJR_TO_CHARS_IMPL(16, (base_2_table + 3) / 4, (n, uVal));
    }
    case 4:
    case 32: {
        const int bits = base == 4 ? 2 : 5;
        const int n = count_digits<1>(uVal, bits);
        WJR_TO_CHARS_IMPL(1, (base_2_table + bits - 1) / bits, (n, uVal, bits));
    }
    case 10: {
        WJR_TO_CHARS_IMPL(10, base_10_table, (uVal));
    }
    default: {
        WJR_UNREACHABLE();
        return ptr;
    }
    }

#undef WJR_TO_CHARS_IMPL
}

template <typename Iter, typename Value, typename IBase, typename Converter>
Iter __to_chars_unchecked_impl(Iter ptr, Value val, IBase ibase, Converter conv) {
    if constexpr (convert_details::__is_fast_convert_iterator_v<Iter>) {
        const auto __ptr = reinterpret_cast<uint8_t *>((to_address)(ptr));
        const auto __result = __fast_to_chars_unchecked_impl(__ptr, val, ibase, conv);
        return ptr + std::distance(__ptr, __result);
    } else {
        return __fallback_to_chars_unchecked_impl(ptr, val, ibase, conv);
    }
}
/**
 * @brief Convert an unsigned integer to a string with checking buf size.
 *
 *
 * @return to_chars_result<Iter> If the conversion is successful, return {ans,
 * std::errc{}}. Otherwise, return {last, std::errc::value_too_large}.
 *
 */
template <typename Iter, typename Value, typename BaseType = unsigned int,
          BaseType IBase = 10, typename Converter = char_converter_t,
          WJR_REQUIRES(convert_details::__is_valid_converter_v<Value, Converter>)>
to_chars_result<Iter> to_chars(Iter ptr, Iter last, Value val,
                               std::integral_constant<BaseType, IBase> = {},
                               Converter conv = {}) {
    return __to_chars_impl(ptr, last, val, std::integral_constant<unsigned int, IBase>(),
                           conv);
}

/**
 * @brief Convert an unsigned integer to a string with checking buf size.
 *
 * @return to_chars_result<Iter> If the conversion is successful, return {ans,
 * std::errc{}}. Otherwise, return {last, std::errc::value_too_large}.
 *
 */
template <typename Iter, typename Value, typename Converter = char_converter_t,
          WJR_REQUIRES(convert_details::__is_valid_converter_v<Value, Converter>)>
to_chars_result<Iter> to_chars(Iter ptr, Iter last, Value val, unsigned int base,
                               Converter conv = {}) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return to_chars(ptr, last, val, std::integral_constant<unsigned int, 2>(),
                            conv);
        }
        case 8: {
            return to_chars(ptr, last, val, std::integral_constant<unsigned int, 8>(),
                            conv);
        }
        case 16: {
            return to_chars(ptr, last, val, std::integral_constant<unsigned int, 16>(),
                            conv);
        }
        case 10: {
            return to_chars(ptr, last, val, std::integral_constant<unsigned int, 10>(),
                            conv);
        }
        default: {
            break;
        }
        }
    }

    return __to_chars_impl(ptr, last, val, base, conv);
}

/**
 * @brief Convert an unsigned integer to a string without checking buf size.
 *
 * @details Iter can be any output iterator. Support fast_convert mode and fallback mode.
 * \n fast_convert mode : \n fast_convert mode is used when
 * __is_fast_convert_iterator_v<Iter> is true. \n caclulate the number of digits and
 * convert the integer to a string in reverse order. \n fallback mode : \n use buffer to
 * store the result and use @ref wjr::copy to copy the result to the output iterator. \n
 *
 */
template <typename Iter, typename Value, typename BaseType = unsigned int,
          BaseType IBase = 10, typename Converter = char_converter_t,
          WJR_REQUIRES(convert_details::__is_valid_converter_v<Value, Converter>)>
Iter to_chars_unchecked(Iter ptr, Value val, std::integral_constant<BaseType, IBase> = {},
                        Converter conv = {}) {
    return __to_chars_unchecked_impl(ptr, val,
                                     std::integral_constant<unsigned int, IBase>(), conv);
}

/**
 * @brief Convert an unsigned integer to a string without checking buf size.
 *
 * @tparam Iter The iterator type. Must be random access iterator.
 * @tparam Value The value type. If Converter is origin_converter_t, Value must be
 * non-bool unsigned integral type. Otherwise, Value must be non-bool integral type.
 *
 */
template <typename Iter, typename Value, typename Converter = char_converter_t,
          WJR_REQUIRES(convert_details::__is_valid_converter_v<Value, Converter>)>
Iter to_chars_unchecked(Iter ptr, Value val, unsigned int base, Converter conv = {}) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return to_chars_unchecked(ptr, val, std::integral_constant<unsigned int, 2>(),
                                      conv);
        }
        case 8: {
            return to_chars_unchecked(ptr, val, std::integral_constant<unsigned int, 8>(),
                                      conv);
        }
        case 16: {
            return to_chars_unchecked(ptr, val,
                                      std::integral_constant<unsigned int, 16>(), conv);
        }
        case 10: {
            return to_chars_unchecked(ptr, val,
                                      std::integral_constant<unsigned int, 10>(), conv);
        }
        default: {
            break;
        }
        }
    }

    return __to_chars_unchecked_impl(ptr, val, base, conv);
}

template <typename Converter>
size_t __biginteger_to_chars_2_impl(uint8_t *first, const uint64_t *up, size_t n,
                                    Converter conv) {
    WJR_ASSERT_L1(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    uint64_t x = up[n - 1];
    --n;
    const int pc = clz(x);
    const int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);

    size_t len = hbits + 64 * n;
    first += len;

    do {
        x = *up;

        for (int i = 0; i < 8; ++i) {
            __to_chars_unroll_8<2>(first - 8, x & 0xff, conv);
            first -= 8;
            x >>= 8;
        }

        ++up;
        --n;
    } while (n);
    x = *up;

    (void)__unsigned_to_chars_backward_unchecked<2>(first, hbits, x, conv);
    return len;
}

template <typename Converter>
size_t __biginteger_to_chars_8_impl(uint8_t *first, const uint64_t *up, size_t n,
                                    Converter conv) {
    WJR_ASSERT_L1(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    uint64_t x = up[n - 1];
    --n;
    const int pc = clz(x);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);

    const size_t len = (hbits + 64 * n + 2) / 3;
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
            __to_chars_unroll_2<8>(first - 2, last | ((x & 0x03) << 4), conv);
            first -= 2;
            x >>= 2;
            rest = 4;
            break;
        }
        case 4: {
            __to_chars_unroll_2<8>(first - 2, last | ((x & 0x0f) << 2), conv);
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

        __to_chars_unroll_8<8>(first - 8, x & 0xff'ffff, conv);
        x >>= 24;
        __to_chars_unroll_8<8>(first - 16, x & 0xff'ffff, conv);
        x >>= 24;
        __to_chars_unroll_4<8>(first - 20, x & 0x0fff, conv);
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
        __to_chars_unroll_2<8>(first - 2, last | ((x & 0x03) << 4), conv);
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
            *--first = conv.template to<8>(x << 2 | last);
            goto DONE;
        }

        __to_chars_unroll_2<8>(first - 2, last | ((x & 0x0f) << 2), conv);
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
            __to_chars_unroll_4<8>(first - 4, x & 0x0fff, conv);
            first -= 4;
            x >>= 12;
            hbits -= 12;
        } while (WJR_LIKELY(hbits + 2 >= 12));
    }

    switch ((hbits + 2) / 3) {
    case 3: {
        *--first = conv.template to<8>(x & 0x07);
        x >>= 3;
        WJR_FALLTHROUGH;
    }
    case 2: {
        __to_chars_unroll_2<8>(first - 2, x, conv);
        break;
    }
    case 1: {
        *--first = conv.template to<8>(x);
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

template <typename Converter>
size_t __biginteger_to_chars_16_impl(uint8_t *first, const uint64_t *up, size_t n,
                                     Converter conv) {
    WJR_ASSERT_L1(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    uint64_t x = up[n - 1];
    --n;
    const int pc = clz(x);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);
    hbits = (hbits + 3) / 4;

    const size_t len = hbits + 16 * n;
    first += len;

    do {
        x = *up;

        __to_chars_unroll_8<16>(first - 8, x & 0xffff'ffff, conv);
        __to_chars_unroll_8<16>(first - 16, x >> 32, conv);
        first -= 16;

        ++up;
        --n;
    } while (n);
    x = *up;

    (void)__unsigned_to_chars_backward_unchecked<16>(first, hbits, x, conv);

    return len;
}

template <typename Converter>
size_t __biginteger_to_chars_power_of_two_impl(uint8_t *first, const uint64_t *up,
                                               size_t n, unsigned int base,
                                               Converter conv) {
    WJR_ASSERT_L1(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    const int bits = ctz(base);
    const unsigned int mask = (1u << bits) - 1;

    uint64_t x = up[n - 1];
    --n;
    const int pc = clz(x);
    const int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);

    const size_t len = (hbits + 64 * n + bits - 1) / bits;
    first += len;

    int rest = 0;
    unsigned int last = 0;

    do {
        x = *up;

        if (rest) {
            int fix = bits - rest;
            unsigned int val = ((x & ((1u << fix) - 1)) << rest) | last;
            x >>= fix;
            rest = 64 - fix;
            *--first = conv.to(val);
        } else {
            rest = 64;
        }

        do {
            *--first = conv.to(x & mask);
            x >>= bits;
            rest -= bits;
        } while (rest >= bits);

        last = x;

        ++up;
        --n;
    } while (n);
    x = *up;

    WJR_ASSERT_ASSUME(rest < bits);

    if (WJR_UNLIKELY(rest != 0)) {
        int fix = bits - rest;
        unsigned int val = ((x & ((1u << fix) - 1)) << rest) | last;
        x >>= fix;
        *--first = conv.to(val);
        rest = hbits - fix;
        if (WJR_UNLIKELY(rest == 0)) {
            goto DONE;
        }
    } else {
        rest = hbits;
    }

    do {
        *--first = conv.to(x & mask);
        x >>= bits;
        rest -= bits;
    } while (WJR_LIKELY(rest > 0));

DONE:
    return len;
}

template <typename Converter>
uint8_t *basecase_to_chars_10(uint8_t *buf, uint64_t *up, size_t n, Converter conv) {
    do {
        if (WJR_UNLIKELY(n == 1)) {
            return __unsigned_to_chars_backward_unchecked<10>(buf, up[0], conv);
        }

        uint64_t q, rem;

        q = div_qr_1_noshift(up, rem, up, n, div2by1_divider_noshift_of_big_base_10);
        n -= q == 0;
        if (q != 0) {
            up[n - 1] = q;
        }

        __to_chars_unroll_8<10>(buf - 8, rem % 1'0000'0000, conv);
        rem /= 1'0000'0000;
        __to_chars_unroll_8<10>(buf - 16, rem % 1'0000'0000, conv);
        rem /= 1'0000'0000;
        __to_chars_unroll_2<10>(buf - 18, rem % 100, conv);
        rem /= 100;
        buf[-19] = conv.template to<10>(rem);

        buf -= 19;
    } while (n);

    return buf;
}

template <typename Converter>
uint8_t *basecase_to_chars(uint8_t *first, size_t len, uint64_t *up, size_t n,
                           unsigned int base, Converter conv) {
    constexpr size_t buf_len = dc_bignum_to_chars_precompute_threshold * 64 * 7 / 11;
    uint8_t buf[buf_len];
    uint8_t *end = buf + buf_len;
    uint8_t *start;

    if (WJR_LIKELY(base == 10)) {
        start = basecase_to_chars_10(end, up, n, conv);
    } else {
        start = end;
    }

    size_t rlen = end - start;
    if (len > rlen) {
        first = std::fill_n(first, len - rlen, conv.template to<1>(0));
    }

    return std::copy(start, end, first);
}

template <typename Converter>
uint8_t *dc_to_chars(uint8_t *first, size_t len, uint64_t *up, size_t n,
                     precompute_chars_convert_t *pre, uint64_t *stk, Converter conv) {
    WJR_ASSERT_ASSUME(n >= 1);
    if (n < dc_bignum_to_chars_threshold) {
        return basecase_to_chars(first, len, up, n, pre->base, conv);
    } else {
        const auto pp = pre->ptr;
        const auto pn = pre->n;
        const auto ps = pre->shift;

        WJR_ASSERT((pn + ps) * 5 >= n * 2);

        if (n < pn + ps || (n == pn + ps && reverse_compare_n(up + ps, pp, pn) < 0)) {
            return dc_to_chars(first, len, up, n, pre - 1, stk, conv);
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

        first = dc_to_chars(first, len, qp, qn, pre, stk + qn, conv);
        first = dc_to_chars(first, pd, up, pn + ps, pre, stk, conv);
        return first;
    }
}

template <typename Converter>
uint8_t *__biginteger_basecase_to_chars(uint8_t *first, const uint64_t *up, size_t n,
                                        unsigned int base, Converter conv) {
    if (WJR_LIKELY(n < dc_bignum_to_chars_precompute_threshold)) {
        uint64_t upbuf[dc_bignum_to_chars_precompute_threshold];
        std::copy_n(up, n, upbuf);
        return basecase_to_chars(first, 0, upbuf, n, base, conv);
    }

    precompute_chars_convert_t pre[64 - 3];

    unique_stack_allocator stkal(math_details::stack_alloc);
    auto stk =
        static_cast<uint64_t *>(stkal.allocate((n * 18 / 5 + 192) * sizeof(uint64_t)));
    const auto __up = stk;
    std::copy_n(up, n, __up);
    stk += n;
    const auto mpre = precompute_chars_convert(pre, n, base, stk);
    stk += n * 8 / 5 + 128;
    return dc_to_chars(first, 0, __up, n, mpre, stk, conv);
}

template <typename Converter>
uint8_t *__fast_biginteger_large_to_chars_impl(uint8_t *first, const uint64_t *up,
                                               size_t n, unsigned int base,
                                               Converter conv) {

    switch (base) {
    case 2: {
        return first + __biginteger_to_chars_2_impl(first, up, n, conv);
    }
    case 8: {
        return first + __biginteger_to_chars_8_impl(first, up, n, conv);
    }
    case 16: {
        return first + __biginteger_to_chars_16_impl(first, up, n, conv);
    }
    case 4:
    case 32: {
        return first + __biginteger_to_chars_power_of_two_impl(first, up, n, base, conv);
    }
    default: {
        break;
    }
    }

    return __biginteger_basecase_to_chars(first, up, n, base, conv);
}

template <typename Iter, typename Converter>
Iter __fallback_biginteger_large_to_chars_impl(Iter ptr, const uint64_t *up, size_t n,
                                               unsigned int base, Converter conv) {
#define WJR_BIGINTEGER_TO_CHARS_IMPL(BASE, NAME, TAIL, SIZE, CALL)                       \
    constexpr auto __fast_container_inserter_v =                                         \
        convert_details::is_fast_container_inserter_v<Iter>;                             \
    if constexpr (__fast_container_inserter_v != 0) {                                    \
        auto &cont = get_inserter_container(ptr);                                        \
        const auto __presize = cont.size();                                              \
        if constexpr (__fast_container_inserter_v == 1) {                                \
            resize(cont, __presize + SIZE);                                              \
        } else {                                                                         \
            append(cont, SIZE, dctor);                                                   \
        }                                                                                \
        const auto __ptr = (uint8_t *)(to_address)(cont.data()) + __presize;             \
        const auto __size = NAME(__ptr, WJR_PP_QUEUE_EXPAND(CALL), conv) TAIL;           \
        WJR_ASSERT((size_t)__size <= SIZE);                                              \
        if constexpr (__fast_container_inserter_v == 1) {                                \
            resize(cont, __presize + __size);                                            \
        } else {                                                                         \
            resize(cont, __presize + __size, wjr::dctor);                                \
        }                                                                                \
                                                                                         \
        return ptr;                                                                      \
    } else {                                                                             \
        unique_stack_allocator stkal(math_details::stack_alloc);                         \
        const auto __ptr = (uint8_t *)stkal.allocate(SIZE * sizeof(uint64_t));           \
        const auto __size = NAME(__ptr, WJR_PP_QUEUE_EXPAND(CALL), conv) TAIL;           \
                                                                                         \
        return wjr::copy_n((convert_details::fast_buffer_t<Iter> *)__ptr, __size, ptr);  \
    }

    switch (base) {
    case 2: {
        const size_t capacity = 64 * n;
        WJR_BIGINTEGER_TO_CHARS_IMPL(2, __biginteger_to_chars_2_impl, , capacity,
                                     (up, n));
    }
    case 8: {
        const size_t capacity = (64 * n + 2) / 3;
        WJR_BIGINTEGER_TO_CHARS_IMPL(8, __biginteger_to_chars_8_impl, , capacity,
                                     (up, n));
    }
    case 16: {
        const size_t capacity = (64 * n + 3) / 4;
        WJR_BIGINTEGER_TO_CHARS_IMPL(16, __biginteger_to_chars_16_impl, , capacity,
                                     (up, n));
    }
    case 4:
    case 32: {
        const int bits = base == 4 ? 2 : 5;
        const size_t capacity = (64 * n + bits - 1) / bits;
        WJR_BIGINTEGER_TO_CHARS_IMPL(base, __biginteger_to_chars_power_of_two_impl, ,
                                     capacity, (up, n, base));
    }
    default: {
        break;
    }
    }

    const size_t capacity = ((64 * n) * 4 + 12) / 13;
    WJR_BIGINTEGER_TO_CHARS_IMPL(base, __biginteger_basecase_to_chars, -__ptr, capacity,
                                 (up, n, base));

#undef WJR_BIGINTEGER_TO_CHARS_IMPL
}

template <typename Iter, typename Converter>
Iter __biginteger_to_chars_impl(Iter first, const uint64_t *up, size_t n,
                                unsigned int base, Converter conv) {
    if (n == 1) {
        return to_chars_unchecked(first, up[0], base, conv);
    }

    if constexpr (convert_details::__is_fast_convert_iterator_v<Iter>) {
        const auto __first = reinterpret_cast<uint8_t *>((to_address)(first));
        const auto __result =
            __fast_biginteger_large_to_chars_impl(__first, up, n, base, conv);
        return first + std::distance(__first, __result);
    } else {
        return __fallback_biginteger_large_to_chars_impl(first, up, n, base, conv);
    }
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
template <typename Iter, typename Converter = char_converter_t>
Iter biginteger_to_chars(Iter first, const uint64_t *up, size_t n, unsigned int base = 10,
                         Converter conv = {}) {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));
    WJR_ASSERT_ASSUME(up[n - 1] != 0);

    return __biginteger_to_chars_impl(first, up, n, base, conv);
}

template <uint64_t Base>
class __unsigned_from_chars_unchecked_fn {};

template <uint64_t Base>
inline constexpr __unsigned_from_chars_unchecked_fn<Base>
    __unsigned_from_chars_unchecked{};

template <>
class __unsigned_from_chars_unchecked_fn<2> {
    template <typename UnsignedValue, typename Converter>
    static void fn(const uint8_t *first, const uint8_t *last, UnsignedValue &val,
                   Converter conv) {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;

        auto n = std::distance(first, last);
        WJR_ASSERT_ASSUME(1 <= n && n <= nd);

        if constexpr (nd >= 16) {
            if (WJR_UNLIKELY(n >= 8)) {
                do {
                    val = (val << 8) + __from_chars_unroll_8<2>(first, conv);
                    first += 8;
                    n -= 8;
                } while (WJR_LIKELY(n >= 8));

                if (n == 0) {
                    return;
                }
            }
        } else if constexpr (nd == 8) {
            if (WJR_UNLIKELY(n == 8)) {
                val = __from_chars_unroll_8<2>(first, conv);
                first += 8;
                return;
            }
        }

        switch (n) {
        case 7: {
            val = (val << 1) + conv.template from<2>(*first++);
            WJR_FALLTHROUGH;
        }
        case 6: {
            val = (val << 1) + conv.template from<2>(*first++);
            WJR_FALLTHROUGH;
        }
        case 5: {
            val = (val << 1) + conv.template from<2>(*first++);
            WJR_FALLTHROUGH;
        }
        case 4: {
            val <<= 4;
            val += __from_chars_unroll_4<2>(first, conv);
            first += 4;
            break;
        }
        case 3: {
            val = (val << 1) + conv.template from<2>(*first++);
            WJR_FALLTHROUGH;
        }
        case 2: {
            val = (val << 1) + conv.template from<2>(*first++);
            WJR_FALLTHROUGH;
        }
        case 1: {
            val = (val << 1) + conv.template from<2>(*first++);
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }
    }

public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    void operator()(const uint8_t *first, const uint8_t *last, UnsignedValue &val,
                    Converter conv) const {
        return fn(first, last, val, conv);
    }
};

template <>
class __unsigned_from_chars_unchecked_fn<8> {
    template <typename UnsignedValue, typename Converter>
    static void fn(const uint8_t *first, const uint8_t *last, UnsignedValue &val,
                   Converter conv) {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;

        auto n = std::distance(first, last);
        WJR_ASSERT_ASSUME(1 <= n && n <= (nd + 2) / 3);

        if constexpr (nd >= 32) {
            if (WJR_UNLIKELY(n >= 8)) {
                do {
                    val = (val << 24) + __from_chars_unroll_8<8>(first, conv);
                    first += 8;
                    n -= 8;
                } while (WJR_LIKELY(n >= 8));

                if (n == 0) {
                    return;
                }
            }
        }

        switch (n) {
        case 7: {
            val = (val << 3) + conv.template from<8>(*first++);
            WJR_FALLTHROUGH;
        }
        case 6: {
            val = (val << 3) + conv.template from<8>(*first++);
            WJR_FALLTHROUGH;
        }
        case 5: {
            val = (val << 3) + conv.template from<8>(*first++);
            WJR_FALLTHROUGH;
        }
        case 4: {
            val <<= 12;
            val += __from_chars_unroll_4<8>(first, conv);
            first += 4;
            break;
        }
        case 3: {
            val = (val << 3) + conv.template from<8>(*first++);
            WJR_FALLTHROUGH;
        }
        case 2: {
            val = (val << 3) + conv.template from<8>(*first++);
            WJR_FALLTHROUGH;
        }
        case 1: {
            val = (val << 3) + conv.template from<8>(*first++);
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }
    }

public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    void operator()(const uint8_t *first, const uint8_t *last, UnsignedValue &val,
                    Converter conv) const {
        return fn(first, last, val, conv);
    }
};

template <>
class __unsigned_from_chars_unchecked_fn<16> {
    template <typename UnsignedValue, typename Converter>
    static void fn(const uint8_t *first, const uint8_t *last, UnsignedValue &val,
                   Converter conv) {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;

        auto n = std::distance(first, last);
        WJR_ASSERT_ASSUME(1 <= n && n <= (nd + 3) / 4);

        if constexpr (nd >= 64) {
            if (WJR_UNLIKELY(n >= 8)) {
                do {
                    val = (val << 32) + __from_chars_unroll_8<16>(first, conv);
                    first += 8;
                    n -= 8;
                } while (WJR_LIKELY(n >= 8));

                if (n == 0) {
                    return;
                }
            }
        } else if constexpr (nd == 32) {
            if (WJR_UNLIKELY(n == 8)) {
                val = __from_chars_unroll_8<16>(first, conv);
                first += 8;
                return;
            }
        }

        switch (n) {
        case 7: {
            val = (val << 4) + conv.template from<16>(*first++);
            WJR_FALLTHROUGH;
        }
        case 6: {
            val = (val << 4) + conv.template from<16>(*first++);
            WJR_FALLTHROUGH;
        }
        case 5: {
            val = (val << 4) + conv.template from<16>(*first++);
            WJR_FALLTHROUGH;
        }
        case 4: {
            val <<= 16;
            val += __from_chars_unroll_4<16>(first, conv);
            first += 4;
            break;
        }
        case 3: {
            val = (val << 4) + conv.template from<16>(*first++);
            WJR_FALLTHROUGH;
        }
        case 2: {
            val = (val << 4) + conv.template from<16>(*first++);
            WJR_FALLTHROUGH;
        }
        case 1: {
            val = (val << 4) + conv.template from<16>(*first++);
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }
    }

public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    void operator()(const uint8_t *first, const uint8_t *last, UnsignedValue &val,
                    Converter conv) const {
        return fn(first, last, val, conv);
    }
};

template <>
class __unsigned_from_chars_unchecked_fn<1> {};

template <>
class __unsigned_from_chars_unchecked_fn<10> {
    template <typename UnsignedValue, typename Converter>
    static void fn(const uint8_t *first, const uint8_t *last, UnsignedValue &val,
                   Converter conv) {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits10 + 1;

        auto n = std::distance(first, last);
        WJR_ASSUME(1 <= n && n <= nd);

        if constexpr (nd >= 8) {
            if (WJR_UNLIKELY(n >= 8)) {
                if (WJR_UNLIKELY(n >= 16)) {
                    val = __from_chars_unroll_16<10>(first, conv);
                    first += 16;
                    n -= 16;
                } else {
                    val = __from_chars_unroll_8<10>(first, conv);
                    first += 8;
                    n -= 8;
                }

                if (WJR_UNLIKELY(n == 0)) {
                    return;
                }
            }
        }

        switch (n) {
        case 7: {
            val = val * 10 + conv.template from<10>(*first++);
            WJR_FALLTHROUGH;
        }
        case 6: {
            val = val * 10 + conv.template from<10>(*first++);
            WJR_FALLTHROUGH;
        }
        case 5: {
            val = val * 10 + conv.template from<10>(*first++);
            WJR_FALLTHROUGH;
        }
        case 4: {
            val = (val * 10000) + __from_chars_unroll_4<10>(first, conv);
            break;
        }
        case 3: {
            val = val * 10 + conv.template from<10>(*first++);
            WJR_FALLTHROUGH;
        }
        case 2: {
            val = val * 10 + conv.template from<10>(*first++);
            WJR_FALLTHROUGH;
        }
        case 1: {
            val = val * 10 + conv.template from<10>(*first++);
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
    }

public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    void operator()(const uint8_t *first, const uint8_t *last, UnsignedValue &val,
                    Converter conv) const {
        return fn(first, last, val, conv);
    }
};

template <typename Value, typename IBase, typename Converter,
          WJR_REQUIRES(is_nonbool_integral_v<Value>)>
void __fast_from_chars_unchecked_impl(const uint8_t *first, const uint8_t *last,
                                      Value &val, IBase ibase, Converter conv) {
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
        __unsigned_from_chars_unchecked<2>(first, last, uVal, conv);
        break;
    }
    case 8: {
        __unsigned_from_chars_unchecked<8>(first, last, uVal, conv);
        break;
    }
    case 16: {
        __unsigned_from_chars_unchecked<16>(first, last, uVal, conv);
        break;
    }
    case 10: {
        __unsigned_from_chars_unchecked<10>(first, last, uVal, conv);
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

template <typename Iter, typename Value, typename IBase, typename Converter,
          WJR_REQUIRES(is_nonbool_integral_v<Value>)>
void __from_chars_unchecked_impl(Iter first, Iter last, Value &val, IBase ibase,
                                 Converter conv) {
    const auto __first = reinterpret_cast<const uint8_t *>((to_address)(first));
    const auto __last = reinterpret_cast<const uint8_t *>((to_address)(last));
    __fast_from_chars_unchecked_impl(__first, __last, val, ibase, conv);
}

template <typename Iter, typename Value, typename BaseType = unsigned int,
          BaseType IBase = 10, typename Converter = char_converter_t,
          WJR_REQUIRES(convert_details::__is_fast_convert_iterator_v<Iter>
                           &&convert_details::__is_valid_converter_v<Value, Converter>)>
void from_chars_unchecked(Iter first, Iter last, Value &val,
                          std::integral_constant<BaseType, IBase> = {},
                          Converter conv = {}) {
    __from_chars_unchecked_impl(first, last, val,
                                std::integral_constant<unsigned int, IBase>(), conv);
}

template <typename Iter, typename Value, typename Converter,
          WJR_REQUIRES(convert_details::__is_fast_convert_iterator_v<Iter>
                           &&convert_details::__is_valid_converter_v<Value, Converter>)>
void from_chars_unchecked(Iter first, Iter last, Value &val, unsigned int base,
                          Converter conv = {}) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            __from_chars_unchecked_impl(first, last, val,
                                        std::integral_constant<unsigned int, 2>(), conv);
            return;
        }
        case 8: {
            __from_chars_unchecked_impl(first, last, val,
                                        std::integral_constant<unsigned int, 8>(), conv);
            return;
        }
        case 16: {
            __from_chars_unchecked_impl(first, last, val,
                                        std::integral_constant<unsigned int, 16>(), conv);
            return;
        }
        case 10: {
            __from_chars_unchecked_impl(first, last, val,
                                        std::integral_constant<unsigned int, 10>(), conv);
            return;
        }
        default: {
            break;
        }
        }
    }

    __from_chars_unchecked_impl(first, last, val, base, conv);
}

template <uint64_t Base>
struct __unsigned_from_chars_fn {};

template <uint64_t Base>
inline constexpr __unsigned_from_chars_fn<Base> __unsigned_from_chars{};

template <>
struct __unsigned_from_chars_fn<2> {
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    int operator()(const uint8_t *&first, const uint8_t *last, UnsignedValue &value,
                   Converter conv) const {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;
        constexpr auto zero = conv.template to<2>(0);
        constexpr auto one = conv.template to<2>(1);

        if (first == last) {
            return 2;
        }

        uint8_t ch = first[0];

        if (ch == one) {
        } else if (ch == zero) {
            do {
                ++first;
                if (first == last) {
                    return 1;
                }

                ch = first[0];
            } while (ch == zero);
            if (ch != one) {
                return 1;
            }
        } else {
            return 2;
        }

        const size_t n = std::distance(first, last);
        ch = 1;
        size_t idx = 0;

        do {
            value = value << 1 | ch;
            ++idx;
            if (idx == n) {
                break;
            }

            ch = conv.template from<2>(first[idx]);
        } while (ch < 2);

        first += idx;
        return idx <= nd;
    }
};

template <>
struct __unsigned_from_chars_fn<10> {
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    int operator()(const uint8_t *&first, const uint8_t *last, UnsignedValue &value,
                   Converter conv) const {
        constexpr auto zero = conv.template to<10>(0);
        constexpr auto nine = conv.template to<10>(9);

        constexpr auto __try_matches = [](uint8_t &ch) {
            if constexpr (zero != 0) {
                return ch <= nine && !sub_overflow(ch, zero, ch);
            } else {
                return ch <= nine;
            }
        };

        constexpr auto __matches = [](uint8_t ch) {
            if constexpr (zero != 0) {
                return zero <= ch && ch <= nine;
            } else {
                return ch <= nine;
            }
        };

        if (first == last) {
            return 2;
        }

        uint8_t ch = *first;

        if (WJR_UNLIKELY(!__try_matches(ch))) {
            return 2;
        }

        do {
            ++first;
            if (WJR_UNLIKELY(mul_overflow(value, 10, value) ||
                             add_overflow(value, ch, value))) {
                while (first != last && __matches(*first)) {
                    ++first;
                }

                return 0;
            }

            if (first == last) {
                break;
            }

            ch = *first;
        } while (__try_matches(ch));

        return 1;
    }
};

template <typename Value, typename IBase, typename Converter,
          WJR_REQUIRES(is_nonbool_integral_v<Value>)>
from_chars_result<const uint8_t *> __fast_from_chars_impl(const uint8_t *first,
                                                          const uint8_t *last, Value &val,
                                                          IBase ibase, Converter conv) {
    constexpr auto is_signed = std::is_signed_v<Value>;

    int sign = 0;

    if constexpr (is_signed) {
        if (first != last && *first == '-') {
            ++first;
            sign = 1;
        }
    }

    using UnsignedValue = std::make_unsigned_t<Value>;

    UnsignedValue uVal = 0;

    const unsigned int base = ibase;
    from_chars_result<const uint8_t *> ret{first, std::errc{}};
    int valid;

    switch (base) {
    case 2: {
        valid = __unsigned_from_chars<2>(first, last, uVal, conv);
        break;
    }
    case 10: {
        valid = __unsigned_from_chars<10>(first, last, uVal, conv);
        break;
    }
    default: {
        return {first, std::errc::invalid_argument};
    }
    }

    if (WJR_UNLIKELY(valid == 2)) {
        ret.ec = std::errc::invalid_argument;
    } else {
        ret.ptr = first;
        if (!valid) {
            ret.ec = std::errc::result_out_of_range;
        } else {
            if constexpr (is_signed) {
                if (sign) {
                    if (uVal >
                        static_cast<UnsignedValue>(std::numeric_limits<Value>::min())) {
                        ret.ec = std::errc::result_out_of_range;
                    } else {
                        val = -static_cast<Value>(uVal);
                    }
                } else {
                    if (uVal >
                        static_cast<UnsignedValue>(std::numeric_limits<Value>::max())) {
                        ret.ec = std::errc::result_out_of_range;
                    } else {
                        val = static_cast<Value>(uVal);
                    }
                }
            } else {
                val = uVal;
            }
        }
    }

    return ret;
}

template <typename Value, typename IBase, typename Converter,
          WJR_REQUIRES(is_nonbool_integral_v<Value>)>
from_chars_result<const char *> __from_chars_impl(const char *first, const char *last,
                                                  Value &val, IBase ibase,
                                                  Converter conv) {
    const auto __first = reinterpret_cast<const uint8_t *>(first);
    const auto __last = reinterpret_cast<const uint8_t *>(last);
    const auto ret = __fast_from_chars_impl(__first, __last, val, ibase, conv);
    return {reinterpret_cast<const char *>(ret.ptr), ret.ec};
}

template <typename Value, typename BaseType = unsigned int, BaseType IBase = 10,
          typename Converter = char_converter_t,
          WJR_REQUIRES(convert_details::__is_valid_converter_v<Value, Converter>)>
from_chars_result<const char *>
from_chars(const char *first, const char *last, Value &val,
           std::integral_constant<BaseType, IBase> = {}, Converter conv = {}) {
    return __from_chars_impl(first, last, val,
                             std::integral_constant<unsigned int, IBase>(), conv);
}

template <typename Value, typename Converter = char_converter_t,
          WJR_REQUIRES(convert_details::__is_valid_converter_v<Value, Converter>)>
from_chars_result<const char *> from_chars(const char *first, const char *last,
                                           Value &val, unsigned int base,
                                           Converter conv = {}) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return __from_chars_impl(first, last, val,
                                     std::integral_constant<unsigned int, 2>(), conv);
        }
        case 10: {
            return __from_chars_impl(first, last, val,
                                     std::integral_constant<unsigned int, 10>(), conv);
        }
        default: {
            break;
        }
        }
    }

    return __from_chars_impl(first, last, val, base, conv);
}

template <typename Converter>
size_t __biginteger_from_chars_2_impl(const uint8_t *first, size_t n, uint64_t *up,
                                      Converter conv) {
    const size_t hbits = (n - 1) % 64 + 1;
    const size_t len = (n - 1) / 64 + 1;

    uint64_t x = 0;
    up += len;

    __unsigned_from_chars_unchecked<2>(first, first + hbits, x, conv);
    first += hbits;

    *--up = x;

    size_t idx = len - 1;

    if (idx) {
        do {
            x = 0;

            for (int i = 0; i < 4; ++i) {
                x = (x << 16) + __from_chars_unroll_16<2>(first, conv);
                first += 16;
            }

            *--up = x;
        } while (WJR_LIKELY(--idx));
    }

    return len;
}

template <typename Converter>
size_t __biginteger_from_chars_8_impl(const uint8_t *first, size_t n, uint64_t *up,
                                      Converter conv) {
    size_t len = (n * 3 + 63) / 64;
    size_t lbits = (64 * (len - 1)) / 3;
    size_t rest = (64 * (len - 1)) % 3;
    const size_t hbits = n - lbits - 1;

    auto unroll = [conv](uint64_t &x, auto &first) {
        auto x0 = conv.template from<8>(first[0]);
        auto x1 = conv.template from<8>(first[1]);
        auto x2 = conv.template from<8>(first[2]);
        auto x3 = conv.template from<8>(first[3]);

        x = x << 12 | x0 << 9 | x1 << 6 | x2 << 3 | x3;
        first += 4;
    };

    uint64_t x = 0;
    up += len;
    size_t idx = len - 1;

    if (WJR_UNLIKELY(hbits == 0)) {
    } else {
        __unsigned_from_chars_unchecked<8>(first, first + hbits, x, conv);
        first += hbits;
    }

    uint64_t nx = conv.template from<8>(*first++);
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
                x = x << 3 | conv.template from<8>(*first++);
                uint64_t nx = conv.template from<8>(*first++);
                x = x << 1 | nx >> 2;
                *--up = x;
                x = nx & 3;
                rest = 2;
                break;
            }
            case 1: {
                x = x << 3 | conv.template from<8>(*first++);
                *--up = x;
                x = 0;
                rest = 0;
                break;
            }
            case 2: {
                uint64_t nx = conv.template from<8>(*first++);
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

template <typename Converter>
size_t __biginteger_from_chars_16_impl(const uint8_t *first, size_t n, uint64_t *up,
                                       Converter conv) {
    const size_t hbits = (n - 1) % 16 + 1;
    size_t len = (n - 1) / 16 + 1;

    auto unroll = [conv](uint64_t &x, auto &first) {
        auto x0 = conv.template from<16>(first[0]);
        auto x1 = conv.template from<16>(first[1]);
        auto x2 = conv.template from<16>(first[2]);
        auto x3 = conv.template from<16>(first[3]);

        x = x << 16 | x0 << 12 | x1 << 8 | x2 << 4 | x3;
        first += 4;
    };

    uint64_t x = 0;
    up += len;

    __unsigned_from_chars_unchecked<16>(first, first + hbits, x, conv);
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

template <typename Converter>
size_t basecase_from_chars_10(const uint8_t *first, size_t n, uint64_t *up,
                              Converter conv) {
    uint64_t x = 0;

    if (n <= 19) {
        __unsigned_from_chars_unchecked<10>(first, first + n, x, conv);
        up[0] = x;

        return up[0] != 0;
    }

    size_t m = (n - 1) % 19 + 1;

    __unsigned_from_chars_unchecked<10>(first, first + m, x, conv);
    up[0] = x;

    first += m;
    n -= m;

    m = up[0] != 0;

    do {
        x = 0;

        x = __from_chars_unroll_16<10>(first, conv);
        first += 16;

        x = x * 10 + conv.template from<10>(*first++);
        x = x * 10 + conv.template from<10>(*first++);
        x = x * 10 + conv.template from<10>(*first++);

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

template <typename Converter>
size_t basecase_from_chars(const uint8_t *first, size_t n, uint64_t *up,
                           unsigned int base, Converter conv) {
    if (base == 10) {
        return basecase_from_chars_10(first, n, up, conv);
    } else {
        return 0;
    }
}

template <typename Converter>
size_t dc_from_chars(const uint8_t *first, size_t n, uint64_t *up,
                     precompute_chars_convert_t *pre, uint64_t *stk, Converter conv) {
    const size_t lo = pre->digits_in_base;
    if (n <= lo) {
        if (n < dc_bignum_from_chars_threshold) {
            return basecase_from_chars(first, n, up, pre->base, conv);
        } else {
            return dc_from_chars(first, n, up, pre - 1, stk, conv);
        }
    }

    const size_t hi = n - lo;
    size_t hn, ln;

    if (hi < dc_bignum_from_chars_threshold) {
        hn = basecase_from_chars(first, hi, stk, pre->base, conv);
    } else {
        hn = dc_from_chars(first, hi, stk, pre - (lo * 2 >= n), up, conv);
    }

    const size_t ps = pre->shift;
    const size_t pn = pre->n;

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
        ln = basecase_from_chars(first, lo, stk, pre->base, conv);
    } else {
        ln = dc_from_chars(first, lo, stk, pre - (lo * 2 >= n), stk + pn + ps + 1, conv);
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

template <typename Converter>
uint64_t *__biginteger_from_chars_impl(const uint8_t *first, const uint8_t *last,
                                       uint64_t *up, unsigned int base,
                                       Converter conv = {}) {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));

    const size_t n = std::distance(first, last);

    if (is_zero_or_single_bit(base)) {
        switch (base) {
        case 2: {
            return up + __biginteger_from_chars_2_impl(first, n, up, conv);
        }
        case 8: {
            return up + __biginteger_from_chars_8_impl(first, n, up, conv);
        }
        case 16: {
            return up + __biginteger_from_chars_16_impl(first, n, up, conv);
        }
        default: {
            WJR_UNREACHABLE();
            return up;
        }
        }
    }

    if (WJR_LIKELY(n < dc_bignum_from_chars_precompute_threshold)) {
        return up + basecase_from_chars(first, n, up, base, conv);
    }

    const auto per_digits = precompute_chars_convert_16n_ptr[base]->digits_in_one_base;

    precompute_chars_convert_t pre[64 - 3];

    unique_stack_allocator stkal(math_details::stack_alloc);
    const size_t un = n / per_digits + 1;
    auto stk =
        static_cast<uint64_t *>(stkal.allocate((un * 16 / 5 + 192) * sizeof(uint64_t)));
    const auto mpre = precompute_chars_convert(pre, un, base, stk);
    stk += un * 8 / 5 + 128;
    return up + dc_from_chars(first, n, up, mpre, stk, conv);
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
template <typename Iter, typename Converter = char_converter_t,
          WJR_REQUIRES(convert_details::__is_fast_convert_iterator_v<Iter>)>
uint64_t *biginteger_from_chars(Iter first, Iter last, uint64_t *up,
                                unsigned int base = 10, Converter conv = {}) {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));

    const auto __first = reinterpret_cast<const uint8_t *>((to_address)(first));
    const auto __last = reinterpret_cast<const uint8_t *>((to_address)(last));

    return __biginteger_from_chars_impl(__first, __last, up, base, conv);
}

} // namespace wjr

#endif // WJR_MATH_CONVERT_HPP__