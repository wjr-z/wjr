#ifndef WJR_MATH_CONVERT_HPP__
#define WJR_MATH_CONVERT_HPP__

#include <wjr/math/clz.hpp>
#include <wjr/math/convert-impl.hpp>
#include <wjr/math/precompute-chars-convert.hpp>

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

// require operator() of Converter is constexpr
template <typename Converter, uint64_t Base, int Unroll>
class __char_converter_table_t {
    static constexpr uint64_t pw2 = Unroll == 2 ? Base * Base : Base * Base * Base * Base;

public:
    constexpr __char_converter_table_t() : table() {
        constexpr auto conv = Converter{};
        for (uint64_t i = 0, j = 0; i < pw2; ++i, j += Unroll) {
            int x = i;
            for (int k = Unroll - 1; ~k; --k) {
                table[j + k] = conv.to(x % Base);
                x /= Base;
            }
        }
    }

    WJR_CONST constexpr char operator[](unsigned int idx) const { return table[idx]; }

private:
    std::array<char, pw2 * Unroll> table;
};

template <typename Converter, uint64_t Base, int Unroll>
inline constexpr __char_converter_table_t<Converter, Base, Unroll>
    __char_converter_table = {};

template <typename Iter, typename Converter>
struct __is_fast_convert_iterator
    : std::conjunction<std::is_pointer<Iter>,
                       std::bool_constant<
                           sizeof(typename std::iterator_traits<Iter>::value_type) == 1>,
                       is_any_of<Converter, char_converter_t, origin_converter_t>> {};

template <typename Iter, typename Converter>
inline constexpr bool __is_fast_convert_iterator_v =
    __is_fast_convert_iterator<Iter, Converter>::value;

WJR_REGISTER_HAS_TYPE(__to_chars_fast_fn_fast_conv,
                      Base::__fast_conv(std::declval<void *>(), std::declval<Value>(),
                                        std::declval<Converter>()),
                      Base, Value, Converter);

WJR_REGISTER_HAS_TYPE(__from_chars_fast_fn_fast_conv,
                      Base::__fast_conv(std::declval<const void *>(),
                                        std::declval<Converter>()),
                      Base, Converter);

template <uint64_t Base>
class __to_chars_unroll_2_fast_fn_impl_base {
public:
    template <typename Converter>
    WJR_INTRINSIC_INLINE static void __fast_conv(void *ptr, uint32_t val,
                                                 Converter conv) {
        auto str = (char *)ptr;
        if constexpr (Base * Base <= 16) {
            constexpr auto &table = __char_converter_table<Converter, Base, 4>;
            str[0] = table[val * 4 + 2];
            str[1] = table[val * 4 + 3];
        } else {
            constexpr auto &table = __char_converter_table<Converter, Base, 2>;
            str[0] = table[val * 2];
            str[1] = table[val * 2 + 1];
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
                                                 Converter conv) {
        auto str = (char *)ptr;
        if constexpr (Base * Base <= 16) {
            constexpr auto &table = __char_converter_table<Converter, Base, 4>;
            str[0] = table[val * 4];
            str[1] = table[val * 4 + 1];
            str[2] = table[val * 4 + 2];
            str[3] = table[val * 4 + 3];
        } else {
            constexpr auto &table = __char_converter_table<Converter, Base, 2>;
            constexpr auto Base2 = Base * Base;
            uint32_t hi = val / Base2;
            uint32_t lo = val % Base2;

            str[0] = table[hi * 2];
            str[1] = table[hi * 2 + 1];
            str[2] = table[lo * 2];
            str[3] = table[lo * 2 + 1];
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
    WJR_INTRINSIC_INLINE static void __fast_conv(void *ptr, uint64_t val,
                                                 char_converter_t conv) {
        builtin_to_chars_unroll_8_fast<Base>(ptr, val, conv);
    }

    WJR_INTRINSIC_INLINE static void __fast_conv(void *ptr, uint64_t val,
                                                 origin_converter_t conv) {
        builtin_to_chars_unroll_8_fast<Base>(ptr, val, conv);
    }
#endif
};

template <uint64_t Base>
class __to_chars_unroll_8_fast_fn_impl {};

// template <>
// class __to_chars_unroll_8_fast_fn_impl<2>
//     : public __to_chars_unroll_8_fast_fn_impl_base<2> {};

// template <>
// class __to_chars_unroll_8_fast_fn_impl<8>
//     : public __to_chars_unroll_8_fast_fn_impl_base<8> {};

template <>
class __to_chars_unroll_8_fast_fn_impl<10>
    : public __to_chars_unroll_8_fast_fn_impl_base<10> {};

// template <>
// class __to_chars_unroll_8_fast_fn_impl<16>
//     : public __to_chars_unroll_8_fast_fn_impl_base<16> {};

template <uint64_t Base>
class __to_chars_unroll_2_fn : public __to_chars_unroll_2_fast_fn_impl<Base> {
    using Mybase = __to_chars_unroll_2_fast_fn_impl<Base>;

public:
    template <typename Iter, typename Converter>
    WJR_INTRINSIC_INLINE void operator()(Iter ptr, uint32_t val, Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      has___to_chars_fast_fn_fast_conv_v<Mybase, uint32_t, Converter>) {
            Mybase::__fast_conv(to_address(ptr), val, conv);
        } else {
            ptr[0] = conv.to(val / Base);
            ptr[1] = conv.to(val % Base);
        }
    }
};

template <uint64_t Base>
inline constexpr __to_chars_unroll_2_fn<Base> __to_chars_unroll_2{};

template <uint64_t Base>
class __to_chars_unroll_4_fn_impl : public __to_chars_unroll_4_fast_fn_impl<Base> {
    using Mybase = __to_chars_unroll_4_fast_fn_impl<Base>;

public:
    template <typename Iter, typename Converter>
    WJR_INTRINSIC_INLINE void operator()(Iter ptr, uint32_t val, Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      has___to_chars_fast_fn_fast_conv_v<Mybase, uint32_t, Converter>) {
            Mybase::__fast_conv(to_address(ptr), val, conv);
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
    template <typename Iter, typename Converter>
    WJR_INTRINSIC_INLINE void operator()(Iter ptr, uint64_t val, Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      has___to_chars_fast_fn_fast_conv_v<Mybase, uint64_t, Converter>) {
            Mybase::__fast_conv(to_address(ptr), val, conv);
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
                                                              origin_converter_t) {
        return __fast_conv(read_memory<uint32_t>(ptr));
    }

    WJR_PURE WJR_INTRINSIC_INLINE static uint32_t __fast_conv(const void *ptr,
                                                              char_converter_t) {
        return __fast_conv(read_memory<uint32_t>(ptr) - 0x30303030u);
    }
};

template <uint64_t Base>
class __from_chars_unroll_8_fast_fn_impl_base {
    WJR_CONST WJR_INTRINSIC_INLINE static uint64_t __fast_conv(uint64_t val) {
        constexpr uint64_t Base2 = Base * Base;
        constexpr uint64_t Base4 = Base2 * Base2;
        constexpr uint64_t Base6 = Base4 * Base2;

        constexpr uint64_t mask = 0x000000FF000000FF;
        constexpr uint64_t mul1 = Base2 + (Base6 << 32);
        constexpr uint64_t mul2 = 1 + (Base4 << 32);

        val = (val * Base) + (val >> 8);
        val = (((val & mask) * mul1) + (((val >> 16) & mask) * mul2)) >> 32;
        return uint32_t(val);
    }

public:
    WJR_PURE WJR_INTRINSIC_INLINE static uint64_t __fast_conv(const void *ptr,
                                                              origin_converter_t) {
#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_8_FAST)
        return builtin_from_chars_unroll_8_fast<Base>(ptr, origin_converter);
#else
        return __fast_conv(read_memory<uint64_t>(ptr));
#endif
    }

    WJR_PURE WJR_INTRINSIC_INLINE static uint64_t __fast_conv(const void *ptr,
                                                              char_converter_t) {
#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_8_FAST)
        return builtin_from_chars_unroll_8_fast<Base>(ptr, char_converter);
#else
        return __fast_conv(read_memory<uint64_t>(ptr) - 0x3030303030303030ull);
#endif
    }
};

template <uint64_t Base>
class __from_chars_unroll_16_fast_fn_impl_base {
#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_16_FAST)
public:
    WJR_PURE WJR_INTRINSIC_INLINE static uint64_t __fast_conv(const void *ptr,
                                                              origin_converter_t) {
        return builtin_from_chars_unroll_16_fast<Base>(ptr, origin_converter);
    }

    WJR_PURE WJR_INTRINSIC_INLINE static uint64_t __fast_conv(const void *ptr,
                                                              char_converter_t) {
        return builtin_from_chars_unroll_16_fast<Base>(ptr, char_converter);
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
    template <typename Iter, typename Converter>
    WJR_PURE WJR_INTRINSIC_INLINE uint64_t operator()(Iter first, Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      has___from_chars_fast_fn_fast_conv_v<Mybase, Converter>) {
            return Mybase::__fast_conv(to_address(first), conv);
        } else {
            uint64_t value = 0;
            value = conv.from(*first++);
            value = value * Base + conv.from(*first++);
            value = value * Base + conv.from(*first++);
            return value * Base + conv.from(*first++);
        }
    }
};

template <uint64_t Base>
inline constexpr __from_chars_unroll_4_fn<Base> __from_chars_unroll_4{};

template <uint64_t Base>
class __from_chars_unroll_8_fn : public __from_chars_unroll_8_fast_fn_impl<Base> {
    using Mybase = __from_chars_unroll_8_fast_fn_impl<Base>;

public:
    template <typename Iter, typename Converter>
    WJR_PURE WJR_INTRINSIC_INLINE uint64_t operator()(Iter first, Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      has___from_chars_fast_fn_fast_conv_v<Mybase, Converter>) {
            return Mybase::__fast_conv(to_address(first), conv);
        } else {
            constexpr uint64_t Base4 = Base * Base * Base * Base;
            return __from_chars_unroll_4<Base>(first, conv) * Base4 +
                   __from_chars_unroll_4<Base>(first + 4, conv);
        }
    }
};

template <uint64_t Base>
inline constexpr __from_chars_unroll_8_fn<Base> __from_chars_unroll_8{};

template <uint64_t Base>
class __from_chars_unroll_16_fn : public __from_chars_unroll_16_fast_fn_impl<Base> {
    using Mybase = __from_chars_unroll_16_fast_fn_impl<Base>;

public:
    template <typename Iter, typename Converter>
    WJR_PURE WJR_INTRINSIC_INLINE uint64_t operator()(Iter first, Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      has___from_chars_fast_fn_fast_conv_v<Mybase, Converter>) {
            return Mybase::__fast_conv(to_address(first), conv);
        } else {
            constexpr uint64_t Base4 = Base * Base * Base * Base;
            constexpr uint64_t Base8 = Base4 * Base4;
            return __from_chars_unroll_8<Base>(first, conv) * Base8 +
                   __from_chars_unroll_8<Base>(first + 8, conv);
        }
    }
};

template <uint64_t Base>
inline constexpr __from_chars_unroll_16_fn<Base> __from_chars_unroll_16{};

template <typename Iter, typename Converter = char_converter_t>
size_t to_chars_2(Iter first, uint64_t x, Converter conv = {}) {
    WJR_ASSERT(x != 0);

    int pc = clz(x);
    WJR_ASSUME(0 <= pc && pc <= 63);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);
    size_t len = hbits;
    first += len;

    if (WJR_LIKELY(hbits >= 8)) {
        do {
            __to_chars_unroll_8<2>(first - 8, x & 0xff, conv);
            first -= 8;
            x >>= 8;
            hbits -= 8;
        } while (WJR_LIKELY(hbits >= 8));
    }

    switch (hbits) {
    case 7: {
        *--first = conv.to(x & 1);
        x >>= 1;
        WJR_FALLTHROUGH;
    }
    case 6: {
        __to_chars_unroll_4<2>(first - 4, x & 0x0f, conv);
        __to_chars_unroll_2<2>(first - 6, x >> 4, conv);
        break;
    }
    case 5: {
        *--first = conv.to(x & 1);
        x >>= 1;
        WJR_FALLTHROUGH;
    }
    case 4: {
        __to_chars_unroll_4<2>(first - 4, x, conv);
        break;
    }
    case 3: {
        *--first = conv.to(x & 1);
        x >>= 1;
        WJR_FALLTHROUGH;
    }
    case 2: {
        __to_chars_unroll_2<2>(first - 2, x, conv);
        break;
    }
    case 1: {
        *--first = conv.to(x);
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

    return len;
}

template <typename Iter, typename Converter>
size_t __large_to_chars_2_impl(Iter first, uint64_t *up, size_t n, Converter conv) {
    WJR_ASSERT(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    WJR_ASSUME(0 <= pc && pc <= 63);
    int hbits = 64 - pc;
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

    if (WJR_LIKELY(hbits >= 8)) {
        do {
            __to_chars_unroll_8<2>(first - 8, x & 0xff, conv);
            first -= 8;
            x >>= 8;
            hbits -= 8;
        } while (WJR_LIKELY(hbits >= 8));
    }

    switch (hbits) {
    case 7: {
        *--first = conv.to(x & 1);
        x >>= 1;
        WJR_FALLTHROUGH;
    }
    case 6: {
        __to_chars_unroll_4<2>(first - 4, x & 0x0f, conv);
        __to_chars_unroll_2<2>(first - 6, x >> 4, conv);
        break;
    }
    case 5: {
        *--first = conv.to(x & 1);
        x >>= 1;
        WJR_FALLTHROUGH;
    }
    case 4: {
        __to_chars_unroll_4<2>(first - 4, x, conv);
        break;
    }
    case 3: {
        *--first = conv.to(x & 1);
        x >>= 1;
        WJR_FALLTHROUGH;
    }
    case 2: {
        __to_chars_unroll_2<2>(first - 2, x, conv);
        break;
    }
    case 1: {
        *--first = conv.to(x);
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

    return len;
}

template <typename Iter, typename Converter = char_converter_t>
size_t to_chars_8(Iter first, uint64_t x, Converter conv = {}) {
    WJR_ASSERT(x != 0);

    int pc = clz(x);
    WJR_ASSUME(0 <= pc && pc <= 63);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);
    size_t len = (hbits + 2) / 3;
    first += len;

    if (hbits + 2 >= 24) {
        do {
            __to_chars_unroll_8<8>(first - 8, x & 0xff'ffff, conv);
            first -= 8;
            x >>= 24;
            hbits -= 24;
        } while (hbits + 2 >= 24);
    }

    switch ((hbits + 2) / 3) {
    case 7: {
        *--first = conv.to(x & 0x07);
        x >>= 3;
        WJR_FALLTHROUGH;
    }
    case 6: {
        __to_chars_unroll_4<8>(first - 4, x & 0x0fff, conv);
        __to_chars_unroll_2<8>(first - 6, x >> 12, conv);
        break;
    }
    case 5: {
        *--first = conv.to(x & 0x07);
        x >>= 3;
        WJR_FALLTHROUGH;
    }
    case 4: {
        __to_chars_unroll_4<8>(first - 4, x, conv);
        break;
    }
    case 3: {
        *--first = conv.to(x & 0x07);
        x >>= 3;
        WJR_FALLTHROUGH;
    }
    case 2: {
        __to_chars_unroll_2<8>(first - 2, x, conv);
        break;
    }
    case 1: {
        *--first = conv.to(x);
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

    return len;
}

template <typename Iter, typename Converter>
size_t __large_to_chars_8_impl(Iter first, uint64_t *up, size_t n, Converter conv) {
    WJR_ASSERT(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    WJR_ASSUME(0 <= pc && pc <= 63);
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
            *--first = conv.to(x << 2 | last);
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

    if (hbits + 2 >= 24) {
        do {
            __to_chars_unroll_8<8>(first - 8, x & 0xff'ffff, conv);
            first -= 8;
            x >>= 24;
            hbits -= 24;
        } while (hbits + 2 >= 24);
    }

    switch ((hbits + 2) / 3) {
    case 7: {
        *--first = conv.to(x & 0x07);
        x >>= 3;
        WJR_FALLTHROUGH;
    }
    case 6: {
        __to_chars_unroll_4<8>(first - 4, x & 0x0fff, conv);
        __to_chars_unroll_2<8>(first - 6, x >> 12, conv);
        break;
    }
    case 5: {
        *--first = conv.to(x & 0x07);
        x >>= 3;
        WJR_FALLTHROUGH;
    }
    case 4: {
        __to_chars_unroll_4<8>(first - 4, x, conv);
        break;
    }
    case 3: {
        *--first = conv.to(x & 0x07);
        x >>= 3;
        WJR_FALLTHROUGH;
    }
    case 2: {
        __to_chars_unroll_2<8>(first - 2, x, conv);
        break;
    }
    case 1: {
        *--first = conv.to(x);
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

template <typename Iter, typename Converter = char_converter_t>
size_t to_chars_16(Iter first, uint64_t x, Converter conv = {}) {
    WJR_ASSERT(x != 0);

    int pc = clz(x);
    WJR_ASSUME(0 <= pc && pc <= 63);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);
    size_t len = (hbits + 3) / 4;
    first += len;

    if (WJR_LIKELY(hbits + 3 >= 32)) {
        __to_chars_unroll_8<16>(first - 8, x & 0xffff'ffff, conv);
        first -= 8;
        x >>= 32;
        hbits -= 32;

        if (hbits + 3 >= 32) {
            __to_chars_unroll_8<16>(first - 8, x, conv);
            hbits -= 32;
        }
    }

    switch ((hbits + 3) / 4) {
    case 7: {
        *--first = conv.to(x & 0x0f);
        x >>= 4;
        WJR_FALLTHROUGH;
    }
    case 6: {
        __to_chars_unroll_4<16>(first - 4, x & 0xffff, conv);
        __to_chars_unroll_2<16>(first - 6, x >> 16, conv);
        break;
    }
    case 5: {
        *--first = conv.to(x & 0x0f);
        x >>= 4;
        WJR_FALLTHROUGH;
    }
    case 4: {
        __to_chars_unroll_4<16>(first - 4, x, conv);
        break;
    }
    case 3: {
        *--first = conv.to(x & 0x0f);
        x >>= 4;
        WJR_FALLTHROUGH;
    }
    case 2: {
        __to_chars_unroll_2<16>(first - 2, x, conv);
        break;
    }
    case 1: {
        *--first = conv.to(x);
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

    return len;
}

template <typename Iter, typename Converter>
size_t __large_to_chars_16_impl(Iter first, uint64_t *up, size_t n, Converter conv) {
    WJR_ASSERT(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    WJR_ASSUME(0 <= pc && pc <= 63);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);
    size_t len = (hbits + 3) / 4 + 16 * n;
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

    if (WJR_LIKELY(hbits + 3 >= 32)) {
        __to_chars_unroll_8<16>(first - 8, x & 0xffff'ffff, conv);
        first -= 8;
        x >>= 32;
        hbits -= 32;

        if (hbits + 3 >= 32) {
            __to_chars_unroll_8<16>(first - 8, x, conv);
            hbits -= 32;
        }
    }

    switch ((hbits + 3) / 4) {
    case 7: {
        *--first = conv.to(x & 0x0f);
        x >>= 4;
        WJR_FALLTHROUGH;
    }
    case 6: {
        __to_chars_unroll_4<16>(first - 4, x & 0xffff, conv);
        __to_chars_unroll_2<16>(first - 6, x >> 16, conv);
        break;
    }
    case 5: {
        *--first = conv.to(x & 0x0f);
        x >>= 4;
        WJR_FALLTHROUGH;
    }
    case 4: {
        __to_chars_unroll_4<16>(first - 4, x, conv);
        break;
    }
    case 3: {
        *--first = conv.to(x & 0x0f);
        x >>= 4;
        WJR_FALLTHROUGH;
    }
    case 2: {
        __to_chars_unroll_2<16>(first - 2, x, conv);
        break;
    }
    case 1: {
        *--first = conv.to(x);
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

    return len;
}

template <typename Iter, typename Converter = char_converter_t>
size_t to_chars_power_of_two(Iter first, uint64_t x, unsigned int base,
                             Converter conv = {}) {
    WJR_ASSERT(x != 0);

    int per_bit = ctz(base);
    unsigned int mask = (1u << per_bit) - 1;

    int pc = clz(x);
    WJR_ASSUME(0 <= pc && pc <= 63);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);
    size_t len = (hbits + per_bit - 1) / per_bit;
    first += len;

    do {
        *--first = conv.to(x & mask);
        x >>= per_bit;
        hbits -= per_bit;
    } while (WJR_LIKELY(hbits > 0));

    return len;
}

template <typename Iter, typename Converter>
size_t __large_to_chars_power_of_two_impl(Iter first, uint64_t *up, size_t n,
                                          unsigned int base, Converter conv) {
    WJR_ASSERT(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    int per_bit = ctz(base);
    unsigned int mask = (1u << per_bit) - 1;

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    WJR_ASSUME(0 <= pc && pc <= 63);
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
            *--first = conv.to(val);
        } else {
            rest = 64;
        }

        do {
            *--first = conv.to(x & mask);
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
        x >>= per_bit;
        rest -= per_bit;
    } while (WJR_LIKELY(rest > 0));

DONE:
    return len;
}

template <typename Converter>
char *__backward_to_chars_10(char *buf, uint64_t val, Converter conv) {
    WJR_ASSERT_ASSUME(val != 0);

    if (WJR_LIKELY(val >= 10)) {
        if (WJR_LIKELY(val >= 1000'0000)) {
            do {
                __to_chars_unroll_8<10>(buf - 8, val % 1'0000'0000, conv);
                buf -= 8;
                val /= 1'0000'0000;
            } while (val >= 1000'0000);
        }

        if (WJR_LIKELY(val >= 10)) {
            do {
                __to_chars_unroll_2<10>(buf - 2, val % 100, conv);
                buf -= 2;
                val /= 100;
            } while (val >= 10);
        }

        if (!val) {
            return buf;
        }
    }

    *--buf = conv.to(val);

    return buf;
}

template <typename Converter>
char *basecase_to_chars_10(char *buf, uint64_t *up, size_t n, Converter conv) {
    constexpr auto &table = __char_converter_table<Converter, 10, 2>;
    constexpr auto unroll = [](auto &first, unsigned int v) {
        first[-1] = table[v * 2 + 1];
        first[-2] = table[v * 2];
        first -= 2;
    };

    do {
        if (WJR_UNLIKELY(n == 1)) {
            return __backward_to_chars_10(buf, up[0], conv);
        }

        uint64_t q, rem;

        q = div_qr_1_noshift(up, rem, up, n, div2by1_divider_noshift_of_big_base_10);
        n -= q == 0;
        if (q != 0) {
            up[n - 1] = q;
        }

        for (int i = 0; i < 9; ++i) {
            unroll(buf, rem % 100);
            rem /= 100;
        }

        *--buf = conv.to(rem);
    } while (n);

    return buf;
}

template <typename Iter, typename Converter>
Iter basecase_to_chars(Iter first, size_t len, uint64_t *up, size_t n, unsigned int base,
                       Converter conv) {
    constexpr size_t buf_len = dc_bignum_to_chars_precompute_threshold * 64 * 7 / 11;
    char buf[buf_len];
    char *start;

    if (WJR_LIKELY(base == 10)) {
        start = basecase_to_chars_10(buf + buf_len, up, n, conv);
    } else {
        start = buf + buf_len;
    }

    size_t rlen = buf + buf_len - start;
    if (len > rlen) {
        first = std::fill_n(first, len - rlen, conv.to(0));
    }

    return std::copy(start, buf + buf_len, first);
}

template <typename Iter, typename Converter>
Iter dc_to_chars(Iter first, size_t len, uint64_t *up, size_t n,
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

/**
 * @brief Convert a 64-bit integer to a string by a given base.
 */
template <typename Iter, typename Converter = char_converter_t,
          std::enable_if_t<is_any_of_v<remove_cvref_t<Converter>, char_converter_t,
                                       origin_converter_t>,
                           int> = 0>
Iter to_chars(Iter first, uint64_t x, unsigned int base = 10, Converter conv = {}) {
    switch (base) {
    case 2: {
        return first + to_chars_2(first, x, conv);
    }
    case 8: {
        return first + to_chars_8(first, x, conv);
    }
    case 16: {
        return first + to_chars_16(first, x, conv);
    }
    case 4:
    case 32: {
        return first + to_chars_power_of_two(first, x, base, conv);
    }
    default: {
        constexpr size_t buf_len = 41;
        char buf[buf_len];
        char *start;
        if (WJR_LIKELY(base == 10)) {
            start = __backward_to_chars_10(buf + buf_len, x, conv);
        } else {
            start = buf + buf_len;
        }
        return std::copy(start, buf + buf_len, first);
    }
    }
}

/**
 * @brief Use by `to_chars` to convert a large integer to a string.
 *
 * @note The length of the biginteger is greater than or equal to 2.
 */
template <typename Iter, typename Converter = char_converter_t,
          std::enable_if_t<is_any_of_v<remove_cvref_t<Converter>, char_converter_t,
                                       origin_converter_t>,
                           int> = 0>
Iter __large_to_chars_impl(Iter first, uint64_t *up, size_t n, unsigned int base = 10,
                           Converter conv = {}) {
    switch (base) {
    case 2: {
        return first + __large_to_chars_2_impl(first, up, n, conv);
    }
    case 8: {
        return first + __large_to_chars_8_impl(first, up, n, conv);
    }
    case 16: {
        return first + __large_to_chars_16_impl(first, up, n, conv);
    }
    case 4:
    case 32: {
        return first + __large_to_chars_power_of_two_impl(first, up, n, base, conv);
    }
    default: {
        break;
    }
    }

    if (WJR_LIKELY(n < dc_bignum_to_chars_precompute_threshold)) {
        uint64_t upbuf[dc_bignum_to_chars_precompute_threshold];
        std::copy_n(up, n, upbuf);
        return basecase_to_chars(first, 0, upbuf, n, base, conv);
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
    return dc_to_chars(first, 0, __up, n, mpre, stk, conv);
}

/**
 * @brief Convert a biginteger to a string by a given base.
 *
 * @tparam Iter Output iterator type
 * @tparam Converter char_converter_t or origin_converter_t. The default type is
 * char_converter_t.
 * @param[out] first Output iterator
 * @param[in] up Pointer to the biginteger
 * @param[in] n Length of the biginteger
 * @param[in] base Base of the output string. Range: `[2, 36]`,
 * Only support 10 and power of two currently.
 * @param[in] conv Converter, only support char_converter_t or origin_converter_t. The
 * default value is char_converter.
 * @return Output iterator after the conversion
 */
template <typename Iter, typename Converter = char_converter_t,
          std::enable_if_t<is_any_of_v<remove_cvref_t<Converter>, char_converter_t,
                                       origin_converter_t>,
                           int> = 0>
Iter to_chars(Iter first, uint64_t *up, size_t n, unsigned int base = 10,
              Converter conv = {}) {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));

    if (n == 1) {
        return to_chars(first, up[0], base, conv);
    }

    return __large_to_chars_impl(first, up, n, base, conv);
}

template <typename Iter, typename Converter = char_converter_t>
size_t from_chars_2(Iter first, size_t n, uint64_t *up, Converter conv = {}) {
    size_t hbits = (n - 1) % 64 + 1;
    size_t len = (n - 1) / 64 + 1;

    uint64_t x = 0;
    up += len;

    if (hbits >= 8) {
        if (hbits >= 16) {
            do {
                x = (x << 16) + __from_chars_unroll_16<2>(first, conv);
                first += 16;
                hbits -= 16;
            } while (WJR_LIKELY(hbits >= 16));
        }

        if (hbits >= 8) {
            x = (x << 8) + __from_chars_unroll_8<2>(first, conv);
            first += 8;
            hbits -= 8;
        }
    }

    switch (hbits) {
    case 7: {
        x = (x << 1) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 6: {
        x = (x << 1) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 5: {
        x = (x << 1) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 4: {
        x <<= 4;
        x += __from_chars_unroll_4<2>(first, conv);
        first += 4;
        break;
    }
    case 3: {
        x = (x << 1) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 2: {
        x = (x << 1) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 1: {
        x = (x << 1) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    default: {
        break;
    }
    }

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

template <typename Iter, typename Converter = char_converter_t>
size_t from_chars_8(Iter first, size_t n, uint64_t *up, Converter conv = {}) {
    size_t len = (n * 3 + 63) / 64;
    size_t lbits = (64 * (len - 1)) / 3;
    size_t rest = (64 * (len - 1)) % 3;
    size_t hbits = n - lbits - 1;

    auto unroll = [conv](uint64_t &x, auto &first) {
        auto x0 = conv.from(first[0]);
        auto x1 = conv.from(first[1]);
        auto x2 = conv.from(first[2]);
        auto x3 = conv.from(first[3]);

        x = x << 12 | x0 << 9 | x1 << 6 | x2 << 3 | x3;
        first += 4;
    };

    uint64_t x = 0;
    up += len;
    size_t idx = len - 1;

    if (hbits >= 8) {
        if (hbits >= 16) {
            do {
                x = (x << 48) + __from_chars_unroll_16<8>(first, conv);
                first += 16;
                hbits -= 16;
            } while (WJR_LIKELY(hbits >= 16));
        }

        if (hbits >= 8) {
            x = (x << 24) + __from_chars_unroll_8<8>(first, conv);
            first += 8;
            hbits -= 8;
        }
    }

    switch (hbits) {
    case 7: {
        x = (x << 3) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 6: {
        x = (x << 3) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 5: {
        x = (x << 3) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 4: {
        x <<= 12;
        x += __from_chars_unroll_4<8>(first, conv);
        first += 4;
        break;
    }
    case 3: {
        x = (x << 3) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 2: {
        x = (x << 3) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 1: {
        x = (x << 3) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    default: {
        break;
    }
    }

    uint64_t nx = conv.from(*first++);
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
                x = x << 3 | conv.from(*first++);
                uint64_t nx = conv.from(*first++);
                x = x << 1 | nx >> 2;
                *--up = x;
                x = nx & 3;
                rest = 2;
                break;
            }
            case 1: {
                x = x << 3 | conv.from(*first++);
                *--up = x;
                x = 0;
                rest = 0;
                break;
            }
            case 2: {
                uint64_t nx = conv.from(*first++);
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

template <typename Iter, typename Converter = char_converter_t>
size_t from_chars_16(Iter first, size_t n, uint64_t *up, Converter conv = {}) {
    size_t hbits = (n - 1) % 16 + 1;
    size_t len = (n - 1) / 16 + 1;

    auto unroll = [conv](uint64_t &x, auto &first) {
        auto x0 = conv.from(first[0]);
        auto x1 = conv.from(first[1]);
        auto x2 = conv.from(first[2]);
        auto x3 = conv.from(first[3]);

        x = x << 16 | x0 << 12 | x1 << 8 | x2 << 4 | x3;
        first += 4;
    };

    uint64_t x = 0;
    up += len;

    if (hbits > 4) {
        do {
            unroll(x, first);
            hbits -= 4;
        } while (WJR_LIKELY(hbits > 4));
    }

    switch (hbits) {
    case 4: {
        unroll(x, first);
        break;
    }
    case 3: {
        x = x << 4 | conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 2: {
        x = x << 4 | conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 1: {
        x = x << 4 | conv.from(*first++);
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

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

template <typename Iter, typename Converter>
void __from_chars_10(Iter first, size_t n, uint64_t &x, Converter conv) {
    x = 0;

    if (n >= 8) {
        if (n >= 16) {
            x = __from_chars_unroll_16<10>(first, conv);
            first += 16;
            n -= 16;
        } else {
            x = (x * 100000000) + __from_chars_unroll_8<10>(first, conv);
            first += 8;
            n -= 8;
        }
    }

    switch (n) {
    case 7: {
        x = x * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 6: {
        x = x * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 5: {
        x = x * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 4: {
        x = (x * 10000) + __from_chars_unroll_4<10>(first, conv);
        break;
    }
    case 3: {
        x = x * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 2: {
        x = x * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 1: {
        x = x * 10 + conv.from(*first++);
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

template <typename Iter, typename Converter>
size_t basecase_from_chars_10(Iter first, size_t n, uint64_t *up, Converter conv) {
    if (n <= 19) {
        __from_chars_10(first, n, up[0], conv);
        return up[0] != 0;
    }

    size_t m = (n - 1) % 19 + 1;

    __from_chars_10(first, m, up[0], conv);
    first += m;
    n -= m;

    m = up[0] != 0;

    do {
        uint64_t x = 0;

        x = __from_chars_unroll_16<10>(first, conv);
        first += 16;

        x = x * 10 + conv.from(*first++);
        x = x * 10 + conv.from(*first++);
        x = x * 10 + conv.from(*first++);

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

template <typename Iter, typename Converter>
size_t basecase_from_chars(Iter first, size_t n, uint64_t *up, unsigned int base,
                           Converter conv) {
    if (base == 10) {
        return basecase_from_chars_10(first, n, up, conv);
    } else {
        return 0;
    }
}

template <typename Iter, typename Converter>
size_t dc_from_chars(Iter first, size_t n, uint64_t *up, precompute_chars_convert_t *pre,
                     uint64_t *stk, Converter conv) {
    size_t lo = pre->digits_in_base;
    if (n <= lo) {
        if (n < dc_bignum_from_chars_threshold) {
            return basecase_from_chars(first, n, up, pre->base, conv);
        } else {
            return dc_from_chars(first, n, up, pre - 1, stk, conv);
        }
    }

    size_t hi = n - lo;
    size_t hn, ln;

    if (hi < dc_bignum_from_chars_threshold) {
        hn = basecase_from_chars(first, hi, stk, pre->base, conv);
    } else {
        hn = dc_from_chars(first, hi, stk, pre - (lo * 2 >= n), up, conv);
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

/**
 * @brief Convert a string to a biginteger by a given base.
 *
 * @tparam Iter Input iterator type
 * @tparam Converter char_converter_t or origin_converter_t. The default type is
 * char_converter_t.
 * @param[in] first Input iterator
 * @param[in] last Input iterator
 * @param[out] up Pointer to the biginteger
 * @param[in] base Base of the input string. Range: `[2, 36]`,
 * Only support 10 and power of two currently.
 * @param[in] conv Converter, only support char_converter_t or origin_converter_t. The
 * default value is char_converter.
 * @return uint64_t* Pointer after the conversion
 */
template <typename Iter, typename Converter = char_converter_t,
          std::enable_if_t<is_any_of_v<remove_cvref_t<Converter>, char_converter_t,
                                       origin_converter_t>,
                           int> = 0>
uint64_t *from_chars(Iter first, Iter last, uint64_t *up, unsigned int base = 10,
                     Converter conv = {}) {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));
    size_t n = std::distance(first, last);

    if (is_zero_or_single_bit(base)) {
        switch (base) {
        case 2: {
            return up + from_chars_2(first, n, up, conv);
        }
        case 8: {
            return up + from_chars_8(first, n, up, conv);
        }
        case 16: {
            return up + from_chars_16(first, n, up, conv);
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
    size_t un = n / per_digits + 1;
    auto stk =
        static_cast<uint64_t *>(stkal.allocate((un * 16 / 5 + 192) * sizeof(uint64_t)));
    auto mpre = precompute_chars_convert(pre, un, base, stk);
    stk += un * 8 / 5 + 128;
    return up + dc_from_chars(first, n, up, mpre, stk, conv);
}

} // namespace wjr

#endif // WJR_MATH_CONVERT_HPP__