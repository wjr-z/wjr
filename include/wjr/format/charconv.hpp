#ifndef WJR_FORMAT_CHARCONV_HPP__
#define WJR_FORMAT_CHARCONV_HPP__

#include <array>

#include <wjr/assert.hpp>
#include <wjr/container/generic/type_traits.hpp>
#include <wjr/format/charconv-impl.hpp>
#include <wjr/math/bit.hpp>
#include <wjr/math/mul.hpp>
#include <wjr/memory/copy.hpp>

#if defined(WJR_X86)
#include <wjr/x86/format/charconv.hpp>
#endif

namespace wjr {

static_assert(sizeof(char) == sizeof(uint8_t), "Not support currently.");

namespace convert_detail {

WJR_CONST constexpr bool __isspace(uint8_t ch) noexcept {
    return char_converter.from(ch) == 64;
}

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
 * @detail The iterator must be contiguous iterator and the value_type must be
 * trivial and sizeof(value_type) == 1. Cast to_address(iter) to uint8_t*(to_chars)/const
 * uint8_t*(from_chars) in fast_convert.
 *
 */
template <typename Iter>
inline constexpr bool __is_fast_convert_iterator_v =
    __is_fast_convert_iterator<Iter>::value;

template <typename Value, typename Converter>
struct __is_valid_converter : std::false_type {};

template <typename Value>
struct __is_valid_converter<Value, char_converter_t> : is_nonbool_integral<Value> {};

template <typename Value>
struct __is_valid_converter<Value, origin_converter_t>
    : is_nonbool_unsigned_integral<Value> {};

template <typename Value, typename Converter>
inline constexpr bool __is_valid_converter_v =
    __is_valid_converter<Value, Converter>::value;

WJR_REGISTER_HAS_TYPE(to_chars_fast_fn_fast_conv,
                      MyBase::__fast_conv(std::declval<void *>(),
                                          std::declval<Args>()...),
                      MyBase);

WJR_REGISTER_HAS_TYPE(from_chars_fast_fn_fast_conv,
                      MyBase::__fast_conv(std::declval<const void *>(),
                                          std::declval<Args>()...),
                      MyBase);

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
struct fast_buffer<Iter, std::enable_if_t<is_any_insert_iterator_v<Iter>>> {
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

} // namespace convert_detail

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

    WJR_CONST constexpr char operator[](unsigned int idx) const noexcept {
        return table[idx];
    }

    WJR_CONST constexpr const char *data() const noexcept { return table.data(); }

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
    WJR_INTRINSIC_INLINE static void __fast_conv(void *ptr, uint32_t val,
                                                 Converter) noexcept {
        const auto str = (char *)ptr;
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
    WJR_INTRINSIC_INLINE static void __fast_conv(void *ptr, uint32_t val,
                                                 Converter) noexcept {
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
                                                 Converter conv) noexcept {
        builtin_to_chars_unroll_8_fast<Base>(ptr, static_cast<uint32_t>(val), conv);
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
                                         Converter conv) const noexcept {
        if constexpr (convert_detail::has_to_chars_fast_fn_fast_conv_v<Mybase, uint32_t,
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
                                         Converter conv) const noexcept {
        if constexpr (convert_detail::has_to_chars_fast_fn_fast_conv_v<Mybase, uint32_t,
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
                                         Converter conv) const noexcept {
        if constexpr (convert_detail::has_to_chars_fast_fn_fast_conv_v<Mybase, uint64_t,
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
    WJR_CONST WJR_INTRINSIC_INLINE static uint32_t __fast_conv(uint32_t val) noexcept {
        return digits_literal_detail::__fast_conv_4<Base>(val);
    }

public:
    WJR_PURE WJR_INTRINSIC_INLINE static uint32_t __fast_conv(const void *ptr,
                                                              char_converter_t) noexcept {
        return __fast_conv(read_memory<uint32_t>(ptr) - 0x30303030u);
    }

    WJR_PURE WJR_INTRINSIC_INLINE static uint32_t
    __fast_conv(const void *ptr, origin_converter_t) noexcept {
        return __fast_conv(read_memory<uint32_t>(ptr));
    }
};

template <uint64_t Base>
class __from_chars_unroll_8_fast_fn_impl_base {
protected:
    WJR_CONST WJR_INTRINSIC_INLINE static uint32_t __fast_conv(uint64_t val) noexcept {
        return digits_literal_detail::__fast_conv_8<Base>(val);
    }

public:
    WJR_PURE WJR_INTRINSIC_INLINE static uint32_t
    __fast_conv(const void *ptr, WJR_MAYBE_UNUSED char_converter_t conv) noexcept {
#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_8_FAST)
        return builtin_from_chars_unroll_8_fast<Base>(ptr, conv);
#else
        return __fast_conv(read_memory<uint64_t>(ptr) - 0x3030303030303030ull);
#endif
    }

    WJR_PURE WJR_INTRINSIC_INLINE static uint32_t
    __fast_conv(const void *ptr, WJR_MAYBE_UNUSED origin_converter_t conv) noexcept {
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
                                                              Converter conv) noexcept {
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
                                                      Converter conv) const noexcept {
        if constexpr (convert_detail::has_from_chars_fast_fn_fast_conv_v<Mybase,
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
                                                      Converter conv) const noexcept {
        if constexpr (convert_detail::has_from_chars_fast_fn_fast_conv_v<Mybase,
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
                                                      Converter conv) const noexcept {
        if constexpr (convert_detail::has_from_chars_fast_fn_fast_conv_v<Mybase,
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
constexpr int fallback_count_digits10(UnsignedValue n) noexcept {
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

inline int builtin_count_digits10_u32(uint32_t n) noexcept {
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

inline int builtin_count_digits10_u64(uint64_t n) noexcept {
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
WJR_CONSTEXPR20 int count_digits10_impl(T n) noexcept {
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
    WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int operator()(T n) const noexcept {
        return bit_width(n);
    }
};

template <>
struct count_digits_fn<8> {
    template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
    WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int operator()(T n) const noexcept {
        return __ceil_div(to_unsigned(bit_width(n)), 3);
    }
};

template <>
struct count_digits_fn<16> {
    template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
    WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int operator()(T n) const noexcept {
        return __ceil_div(to_unsigned(bit_width(n)), 4);
    }
};

template <>
struct count_digits_fn<1> {
    template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
    WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int operator()(T n, int bits) const noexcept {
        return (bit_width(n) + bits - 1) / bits;
    }
};

template <>
struct count_digits_fn<10> {
    template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
    WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int operator()(T n) const noexcept {
        const int ret = count_digits10_impl(n);
        WJR_ASSUME(1 <= ret && ret <= std::numeric_limits<T>::digits10 + 1);
        return ret;
    }
};

template <typename Iter>
struct to_chars_result {
    Iter ptr;
    std::errc ec;

    friend bool operator==(const to_chars_result &lhs,
                           const to_chars_result &rhs) noexcept {
        return lhs.ptr == rhs.ptr && lhs.ec == rhs.ec;
    }

    constexpr explicit operator bool() const noexcept { return ec == std::errc{}; }
};

template <typename Iter = const char *>
struct from_chars_result {
    Iter ptr;
    std::errc ec;

    friend bool operator==(const from_chars_result &lhs,
                           const from_chars_result &rhs) noexcept {
        return lhs.ptr == rhs.ptr && lhs.ec == rhs.ec;
    }

    constexpr explicit operator bool() const noexcept { return ec == std::errc{}; }
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
public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    WJR_INTRINSIC_INLINE uint8_t *operator()(uint8_t *ptr, int n, UnsignedValue x,
                                             Converter conv) const noexcept {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;
        WJR_ASSERT_L2(x != 0);
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
};

template <>
class __unsigned_to_chars_backward_unchecked_fn<8> {
public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    WJR_INTRINSIC_INLINE uint8_t *operator()(uint8_t *ptr, int n, UnsignedValue x,
                                             Converter conv) const noexcept {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;
        WJR_ASSERT_L2(x != 0);
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
};

template <>
class __unsigned_to_chars_backward_unchecked_fn<16> {
public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    WJR_INTRINSIC_INLINE uint8_t *operator()(uint8_t *ptr, int n, UnsignedValue x,
                                             Converter conv) const noexcept {
        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;
        WJR_ASSERT_L2(x != 0);
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
};

template <>
class __unsigned_to_chars_backward_unchecked_fn<1> {
public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    WJR_INTRINSIC_INLINE uint8_t *operator()(uint8_t *ptr, int n, UnsignedValue x,
                                             int bits, Converter conv) const noexcept {
        WJR_ASSERT_L2(x != 0);
        WJR_ASSERT_ASSUME(1 <= n && n <= std::numeric_limits<UnsignedValue>::digits);

        const unsigned int mask = (1u << bits) - 1;

        do {
            *--ptr = conv.to(x & mask);
            x >>= bits;
            --n;
        } while (WJR_LIKELY(n != 0));

        return ptr;
    }
};

template <>
class __unsigned_to_chars_backward_unchecked_fn<10> {
public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    WJR_INTRINSIC_INLINE uint8_t *operator()(uint8_t *ptr, UnsignedValue val,
                                             Converter conv) const noexcept {
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
};

template <typename Value, typename IBase, typename Converter>
uint8_t *__fast_to_chars_backward_unchecked_impl(uint8_t *ptr, Value val, IBase ibase,
                                                 Converter conv) noexcept {
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
                                        Converter conv) noexcept {
    const auto __ptr = reinterpret_cast<uint8_t *>(wjr::to_address(first));
    const auto __end = __fast_to_chars_backward_unchecked_impl(__ptr, val, ibase, conv);
    return first + std::distance(__ptr, __end);
}

/**
 * @brief Convert an unsigned integer to a string in reverse order without checking
 * buf size.
 *
 * @detail Only use fast_convert mode.
 *
 */
template <typename Iter, typename Value, unsigned int IBase = 10,
          typename Converter = char_converter_t,
          WJR_REQUIRES(convert_detail::__is_fast_convert_iterator_v<Iter>
                           &&convert_detail::__is_valid_converter_v<Value, Converter>)>
Iter to_chars_backward_unchecked(Iter first, Value val,
                                 integral_constant<unsigned int, IBase> ic = {},
                                 Converter conv = {}) noexcept {
    return __to_chars_backward_unchecked_impl(first, val, ic, conv);
}

template <typename Iter, typename Value, typename Converter>
Iter to_chars_backward_unchecked_dynamic(Iter first, Value val, unsigned int base,
                                         Converter conv) noexcept {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return to_chars_backward_unchecked(first, val, 2_u, conv);
        }
        case 8: {
            return to_chars_backward_unchecked(first, val, 8_u, conv);
        }
        case 16: {
            return to_chars_backward_unchecked(first, val, 16_u, conv);
        }
        case 10: {
            return to_chars_backward_unchecked(first, val, 10_u, conv);
        }
        default: {
            break;
        }
        }
    }

    return __to_chars_backward_unchecked_impl(first, val, base, conv);
}

/**
 * @brief Convert an unsigned integer to a string in reverse order without checking
 * buf size.
 *
 *
 */
template <typename Iter, typename Value, typename IBase,
          typename Converter = char_converter_t,
          WJR_REQUIRES(convert_detail::__is_fast_convert_iterator_v<Iter>
                           &&convert_detail::__is_valid_converter_v<Value, Converter>
                               &&is_nonbool_integral_v<IBase>)>
Iter to_chars_backward_unchecked(Iter first, Value val, IBase base,
                                 Converter conv = {}) noexcept {
    return to_chars_backward_unchecked_dynamic(first, val,
                                               static_cast<unsigned int>(base), conv);
}

template <typename Value, typename IBase, typename Converter>
to_chars_result<uint8_t *> __fast_to_chars_impl(uint8_t *first, uint8_t *last, Value val,
                                                IBase ibase, Converter conv) noexcept {
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
    }
    }

#undef WJR_TO_CHARS_VALIDATE_IMPL
}

template <typename Iter, typename Value, typename IBase, typename Converter>
to_chars_result<Iter> __fallback_to_chars_impl(Iter first, Iter last, Value val,
                                               IBase ibase, Converter conv) noexcept {
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
        convert_detail::fast_buffer_t<Iter> buffer[TABLE + is_signed];                   \
        const auto __end = buffer + TABLE + is_signed;                                   \
        auto __ptr = (convert_detail::fast_buffer_t<Iter> *)                             \
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
        convert_detail::fast_buffer_t<Iter> buffer[TABLE];                               \
        const auto __end = buffer + TABLE;                                               \
        auto __ptr = (convert_detail::fast_buffer_t<Iter> *)                             \
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
        WJR_TO_CHARS_VALIDATE_IMPL(1, (base_2_table + 1) / 2, (n, uVal, bits));
    }
    case 10: {
        WJR_TO_CHARS_VALIDATE_IMPL(10, base_10_table, (uVal));
    }
    default: {
        WJR_UNREACHABLE();
    }
    }

#undef WJR_TO_CHARS_VALIDATE_IMPL
}

template <typename Iter, typename Value, typename IBase, typename Converter>
to_chars_result<Iter> __to_chars_impl(Iter first, Iter last, Value val, IBase ibase,
                                      Converter conv) noexcept {
    if constexpr (convert_detail::__is_fast_convert_iterator_v<Iter>) {
        const auto __first = reinterpret_cast<uint8_t *>(wjr::to_address(first));
        const auto __last = reinterpret_cast<uint8_t *>(wjr::to_address(last));
        const auto __result = __fast_to_chars_impl(__first, __last, val, ibase, conv);
        return {first + std::distance(__first, __result.ptr), __result.ec};
    } else {
        return __fallback_to_chars_impl(first, last, val, ibase, conv);
    }
}

template <typename Value, typename IBase, typename Converter>
uint8_t *__fast_to_chars_unchecked_impl(uint8_t *ptr, Value val, IBase ibase,
                                        Converter conv) noexcept {
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
    }
    }
}

template <typename Iter, typename Value, typename IBase, typename Converter>
Iter __fallback_to_chars_unchecked_impl(Iter ptr, Value val, IBase ibase,
                                        Converter conv) noexcept {
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
        convert_detail::is_fast_container_inserter_v<Iter>;                              \
    if constexpr (__fast_container_inserter_v != 0) {                                    \
        WJR_PP_BOOL_IF(WJR_PP_EQ(BASE, 10), const int n = count_digits<10>(uVal), );     \
        auto &cont = get_inserter_container(ptr);                                        \
        if constexpr (__fast_container_inserter_v == 1) {                                \
            resize(cont, cont.size() + n + sign);                                        \
        } else {                                                                         \
            append(cont, n + sign, dctor);                                               \
        }                                                                                \
        const auto __end = wjr::to_address(cont.data() + cont.size());                   \
        auto __ptr = (convert_detail::fast_buffer_t<Iter> *)                             \
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
        convert_detail::fast_buffer_t<Iter> buffer[TABLE + is_signed];                   \
        const auto __end = buffer + TABLE + is_signed;                                   \
        auto __ptr = (convert_detail::fast_buffer_t<Iter> *)                             \
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
        WJR_TO_CHARS_IMPL(1, (base_2_table + 1) / 2, (n, uVal, bits));
    }
    case 10: {
        WJR_TO_CHARS_IMPL(10, base_10_table, (uVal));
    }
    default: {
        WJR_UNREACHABLE();
    }
    }

#undef WJR_TO_CHARS_IMPL
}

template <typename Iter, typename Value, typename IBase, typename Converter>
Iter __to_chars_unchecked_impl(Iter ptr, Value val, IBase ibase,
                               Converter conv) noexcept {
    if constexpr (convert_detail::__is_fast_convert_iterator_v<Iter>) {
        const auto __ptr = reinterpret_cast<uint8_t *>(wjr::to_address(ptr));
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
template <typename Iter, typename Value, unsigned int IBase = 10,
          typename Converter = char_converter_t,
          WJR_REQUIRES(convert_detail::__is_valid_converter_v<Value, Converter>)>
to_chars_result<Iter> to_chars(Iter ptr, Iter last, Value val,
                               integral_constant<unsigned int, IBase> ic = {},
                               Converter conv = {}) noexcept {
    return __to_chars_impl(ptr, last, val, ic, conv);
}

template <typename Iter, typename Value, typename Converter>
to_chars_result<Iter> to_chars_dynamic(Iter ptr, Iter last, Value val, unsigned int base,
                                       Converter conv) noexcept {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return to_chars(ptr, last, val, 2_u, conv);
        }
        case 8: {
            return to_chars(ptr, last, val, 8_u, conv);
        }
        case 16: {
            return to_chars(ptr, last, val, 16_u, conv);
        }
        case 10: {
            return to_chars(ptr, last, val, 10_u, conv);
        }
        default: {
            break;
        }
        }
    }

    return __to_chars_impl(ptr, last, val, base, conv);
}

/**
 * @brief Convert an unsigned integer to a string with checking buf size.
 *
 * @return to_chars_result<Iter> If the conversion is successful, return {ans,
 * std::errc{}}. Otherwise, return {last, std::errc::value_too_large}.
 *
 */
template <typename Iter, typename Value, typename IBase,
          typename Converter = char_converter_t,
          WJR_REQUIRES(convert_detail::__is_valid_converter_v<Value, Converter>
                           &&is_nonbool_integral_v<IBase>)>
to_chars_result<Iter> to_chars(Iter ptr, Iter last, Value val, IBase base,
                               Converter conv = {}) noexcept {
    return to_chars_dynamic(ptr, last, val, static_cast<unsigned int>(base), conv);
}

/**
 * @brief Convert an unsigned integer to a string without checking buf size.
 *
 * @detail Iter can be any output iterator. Support fast_convert mode and fallback mode.
 * \n fast_convert mode : \n fast_convert mode is used when
 * __is_fast_convert_iterator_v<Iter> is true. \n caclulate the number of digits and
 * convert the integer to a string in reverse order. \n fallback mode : \n use buffer to
 * store the result and use @ref wjr::copy to copy the result to the output iterator. \n
 *
 */
template <typename Iter, typename Value, unsigned int IBase = 10,
          typename Converter = char_converter_t,
          WJR_REQUIRES(convert_detail::__is_valid_converter_v<Value, Converter>)>
Iter to_chars_unchecked(Iter ptr, Value val,
                        integral_constant<unsigned int, IBase> ic = {},
                        Converter conv = {}) noexcept {
    return __to_chars_unchecked_impl(ptr, val, ic, conv);
}

template <typename Iter, typename Value, typename Converter>
Iter to_chars_unchecked_dynamic(Iter ptr, Value val, unsigned int base,
                                Converter conv) noexcept {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return to_chars_unchecked(ptr, val, 2_u, conv);
        }
        case 8: {
            return to_chars_unchecked(ptr, val, 8_u, conv);
        }
        case 16: {
            return to_chars_unchecked(ptr, val, 16_u, conv);
        }
        case 10: {
            return to_chars_unchecked(ptr, val, 10_u, conv);
        }
        default: {
            break;
        }
        }
    }

    return __to_chars_unchecked_impl(ptr, val, base, conv);
}

/**
 * @brief Convert an unsigned integer to a string without checking buf size.
 *
 * @tparam Iter The iterator type. Must be random access iterator.
 * @tparam Value The value type. If Converter is origin_converter_t, Value must be
 * non-bool unsigned integral type. Otherwise, Value must be non-bool integral type.
 *
 */
template <typename Iter, typename Value, typename IBase,
          typename Converter = char_converter_t,
          WJR_REQUIRES(convert_detail::__is_valid_converter_v<Value, Converter>
                           &&is_nonbool_integral_v<IBase>)>
Iter to_chars_unchecked(Iter ptr, Value val, IBase base, Converter conv = {}) noexcept {
    return to_chars_unchecked_dynamic(ptr, val, static_cast<unsigned int>(base), conv);
}

template <uint64_t Base>
class __unsigned_from_chars_unchecked_fn {};

template <uint64_t Base>
inline constexpr __unsigned_from_chars_unchecked_fn<Base>
    __unsigned_from_chars_unchecked{};

template <>
class __unsigned_from_chars_unchecked_fn<2> {
public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    WJR_INTRINSIC_INLINE void operator()(const uint8_t *first, const uint8_t *last,
                                         UnsignedValue &val,
                                         Converter conv) const noexcept {
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
};

template <>
class __unsigned_from_chars_unchecked_fn<8> {
public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    WJR_INTRINSIC_INLINE void operator()(const uint8_t *first, const uint8_t *last,
                                         UnsignedValue &val,
                                         Converter conv) const noexcept {
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
};

template <>
class __unsigned_from_chars_unchecked_fn<16> {
public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    WJR_INTRINSIC_INLINE void operator()(const uint8_t *first, const uint8_t *last,
                                         UnsignedValue &val,
                                         Converter conv) const noexcept {
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
};

template <>
class __unsigned_from_chars_unchecked_fn<1> {};

template <>
class __unsigned_from_chars_unchecked_fn<10> {
public:
    template <typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    WJR_INTRINSIC_INLINE void operator()(const uint8_t *first, const uint8_t *last,
                                         UnsignedValue &val,
                                         Converter conv) const noexcept {
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
};

template <typename Value, typename IBase, typename Converter>
void __fast_from_chars_unchecked_impl(const uint8_t *first, const uint8_t *last,
                                      Value &val, IBase ibase, Converter conv) noexcept {
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

template <typename Iter, typename Value, typename IBase, typename Converter>
void __from_chars_unchecked_impl(Iter first, Iter last, Value &val, IBase ibase,
                                 Converter conv) noexcept {
    const auto __first = reinterpret_cast<const uint8_t *>(wjr::to_address(first));
    const auto __last = reinterpret_cast<const uint8_t *>(wjr::to_address(last));
    __fast_from_chars_unchecked_impl(__first, __last, val, ibase, conv);
}

template <typename Iter, typename Value, unsigned int IBase = 10,
          typename Converter = char_converter_t,
          WJR_REQUIRES(convert_detail::__is_fast_convert_iterator_v<Iter>
                           &&convert_detail::__is_valid_converter_v<Value, Converter>)>
void from_chars_unchecked(Iter first, Iter last, Value &val,
                          integral_constant<unsigned int, IBase> ic = {},
                          Converter conv = {}) noexcept {
    __from_chars_unchecked_impl(first, last, val, ic, conv);
}

template <typename Iter, typename Value, typename IBase, typename Converter>
void from_chars_unchecked_dynamic(Iter first, Iter last, Value &val, unsigned int base,
                                  Converter conv) noexcept {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            __from_chars_unchecked_impl(first, last, val, 2_u, conv);
            return;
        }
        case 8: {
            __from_chars_unchecked_impl(first, last, val, 8_u, conv);
            return;
        }
        case 16: {
            __from_chars_unchecked_impl(first, last, val, 16_u, conv);
            return;
        }
        case 10: {
            __from_chars_unchecked_impl(first, last, val, 10_u, conv);
            return;
        }
        default: {
            break;
        }
        }
    }

    __from_chars_unchecked_impl(first, last, val, base, conv);
}

template <typename Iter, typename Value, typename IBase,
          typename Converter = char_converter_t,
          WJR_REQUIRES(convert_detail::__is_fast_convert_iterator_v<Iter>
                           &&convert_detail::__is_valid_converter_v<Value, Converter>
                               &&is_nonbool_integral_v<IBase>)>
void from_chars_unchecked(Iter first, Iter last, Value &val, IBase base,
                          Converter conv = {}) noexcept {
    from_chars_unchecked_dynamic(first, last, val, static_cast<unsigned int>(base), conv);
}

template <typename T, T expe>
class result_type {
public:
    result_type() = default;
    result_type(const result_type &) = default;
    result_type(result_type &&) = default;
    result_type &operator=(const result_type &) = default;
    result_type &operator=(result_type &&) = default;
    ~result_type() = default;

    constexpr result_type(T result) noexcept : result(result) {}
    constexpr operator bool() const noexcept { return result == expe; }
    constexpr int pos() const noexcept { return ctz(result - expe) / 8; }

private:
    T result;
};

template <uint64_t Base>
struct check_four_digits_fn {};

template <uint64_t Base>
inline constexpr check_four_digits_fn<Base> check_four_digits{};

template <>
struct check_four_digits_fn<10> {
    WJR_INTRINSIC_INLINE result_type<uint32_t, 0x33'33'33'33>
    operator()(const uint8_t *first, char_converter_t) const noexcept {
        return get(first);
    }

    WJR_INTRINSIC_INLINE result_type<uint32_t, 0x00'00'00'00>
    operator()(const uint8_t *first, origin_converter_t) const noexcept {
        return get(first);
    }

private:
    WJR_INTRINSIC_INLINE static uint32_t get(const uint8_t *first) noexcept {
        constexpr auto high_mask = broadcast<uint8_t, uint32_t>(0xF0);
        constexpr auto low_added = broadcast<uint8_t, uint32_t>(0x06);
        const uint32_t x = read_memory<uint32_t>(first);
        return (x & high_mask) | (((x + low_added) & high_mask) >> 4);
    }
};

template <uint64_t Base>
struct __unsigned_from_chars_fn {};

/** @todo Can be optimized. */
template <uint64_t Base>
inline constexpr __unsigned_from_chars_fn<Base> __unsigned_from_chars{};

enum class __unsigned_from_chars_result {
    INVALID,
    SUCCESS,
    OUT_OF_RANGE,
};

template <>
struct __unsigned_from_chars_fn<2> {
    template <typename Value, typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    WJR_INTRINSIC_INLINE static __unsigned_from_chars_result
    get(const uint8_t *&first, const uint8_t *last, UnsignedValue &value,
        Converter conv) noexcept {

        constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;
        constexpr auto zero = conv.template to<2>(0);
        constexpr auto one = conv.template to<2>(1);

        if (WJR_UNLIKELY(first == last)) {
            return __unsigned_from_chars_result::INVALID;
        }

        uint8_t ch = first[0];

        if (ch == one) {
        } else if (ch == zero) {
            do {
                ++first;
                if (first == last) {
                    return __unsigned_from_chars_result::SUCCESS;
                }

                ch = first[0];
            } while (ch == zero);
            if (ch != one) {
                return __unsigned_from_chars_result::SUCCESS;
            }
        } else {
            return __unsigned_from_chars_result::INVALID;
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
        return idx <= nd ? __unsigned_from_chars_result::SUCCESS
                         : __unsigned_from_chars_result::OUT_OF_RANGE;
    }
};

template <>
struct __unsigned_from_chars_fn<10> {
    template <typename Value, typename UnsignedValue, typename Converter,
              WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    WJR_INTRINSIC_INLINE static __unsigned_from_chars_result
    get(const uint8_t *&first, const uint8_t *last, UnsignedValue &value,
        Converter conv) noexcept {
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

        if (WJR_UNLIKELY(first == last)) {
            return __unsigned_from_chars_result::INVALID;
        }

        uint8_t ch = *first;

        if (WJR_UNLIKELY(!__try_matches(ch))) {
            return __unsigned_from_chars_result::INVALID;
        }

        do {
            ++first;
            if (WJR_UNLIKELY(mul_overflow(value, 10, value) ||
                             add_overflow(value, ch, value))) {
                while (first != last && __matches(*first)) {
                    ++first;
                }

                return __unsigned_from_chars_result::OUT_OF_RANGE;
            }

            if (first == last) {
                break;
            }

            ch = *first;
        } while (__try_matches(ch));

        return __unsigned_from_chars_result::SUCCESS;
    }
};

template <typename Value, typename IBase, typename Converter>
from_chars_result<const uint8_t *>
__fast_from_chars_impl(const uint8_t *first, const uint8_t *last, Value &val, IBase ibase,
                       Converter conv) noexcept {
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
    __unsigned_from_chars_result valid;

    switch (base) {
    case 2: {
        valid = __unsigned_from_chars<2>.template get<Value>(first, last, uVal, conv);
        break;
    }
    case 10: {
        valid = __unsigned_from_chars<10>.template get<Value>(first, last, uVal, conv);
        break;
    }
    default: {
        return {first, std::errc::invalid_argument};
    }
    }

    if (WJR_UNLIKELY(valid == __unsigned_from_chars_result::INVALID)) {
        ret.ec = std::errc::invalid_argument;
    } else {
        ret.ptr = first;
        if (valid == __unsigned_from_chars_result::OUT_OF_RANGE) {
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

template <typename Value, typename IBase, typename Converter>
from_chars_result<const char *> __from_chars_impl(const char *first, const char *last,
                                                  Value &val, IBase ibase,
                                                  Converter conv) noexcept {
    const auto __first = reinterpret_cast<const uint8_t *>(first);
    const auto __last = reinterpret_cast<const uint8_t *>(last);
    const auto ret = __fast_from_chars_impl(__first, __last, val, ibase, conv);
    return {reinterpret_cast<const char *>(ret.ptr), ret.ec};
}

template <typename Value, unsigned int IBase = 10, typename Converter = char_converter_t,
          WJR_REQUIRES(convert_detail::__is_valid_converter_v<Value, Converter>)>
from_chars_result<const char *>
from_chars(const char *first, const char *last, Value &val,
           integral_constant<unsigned int, IBase> ic = {}, Converter conv = {}) noexcept {
    return __from_chars_impl(first, last, val, ic, conv);
}

template <typename Value, typename Converter,
          WJR_REQUIRES(convert_detail::__is_valid_converter_v<Value, Converter>)>
from_chars_result<const char *> from_chars_dynamic(const char *first, const char *last,
                                                   Value &val, unsigned int base,
                                                   Converter conv) noexcept {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return __from_chars_impl(first, last, val, 2_u, conv);
        }
        case 10: {
            return __from_chars_impl(first, last, val, 10_u, conv);
        }
        default: {
            break;
        }
        }
    }

    return __from_chars_impl(first, last, val, base, conv);
}

template <typename Value, typename IBase, typename Converter = char_converter_t,
          WJR_REQUIRES(convert_detail::__is_valid_converter_v<Value, Converter>
                           &&is_nonbool_integral_v<IBase>)>
from_chars_result<const char *> from_chars(const char *first, const char *last,
                                           Value &val, IBase base,
                                           Converter conv = {}) noexcept {
    return from_chars_dynamic(first, last, val, base, conv);
}

} // namespace wjr

#endif // WJR_FORMAT_CHARCONV_HPP__