#ifndef WJR_MATH_CONVERT_HPP__
#define WJR_MATH_CONVERT_HPP__

#include <system_error>

#include <wjr/math/bit.hpp>
#include <wjr/math/broadcast.hpp>
#include <wjr/math/convert-impl.hpp>
#include <wjr/math/div.hpp>
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

    WJR_CONST constexpr const char *data() const { return table.data(); }

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

namespace convert_details {

template <typename Enable, typename Base, typename Value, typename Converter,
          typename... Args>
struct __has_to_chars_fast_fn_fast_conv : std::false_type {};
template <typename Base, typename Value, typename Converter, typename... Args>
struct __has_to_chars_fast_fn_fast_conv<
    std::void_t<decltype(Base::__fast_conv(std::declval<void *>(), std::declval<Value>(),
                                           std::declval<Converter>()))>,
    Base, Value, Converter, Args...> : std::true_type {};
template <typename Base, typename Value, typename Converter, typename... Args>
struct has_to_chars_fast_fn_fast_conv
    : __has_to_chars_fast_fn_fast_conv<void, Base, Value, Converter, Args...> {};
template <typename Base, typename Value, typename Converter, typename... Args>
constexpr bool has_to_chars_fast_fn_fast_conv_v =
    has_to_chars_fast_fn_fast_conv<Base, Value, Converter, Args...>::value;

template <typename Enable, typename Base, typename Converter, typename... Args>
struct __has_from_chars_fast_fn_fast_conv : std::false_type {};
template <typename Base, typename Converter, typename... Args>
struct __has_from_chars_fast_fn_fast_conv<
    std::void_t<decltype(Base::__fast_conv(std::declval<const void *>(),
                                           std::declval<Converter>()))>,
    Base, Converter, Args...> : std::true_type {};
template <typename Base, typename Converter, typename... Args>
struct has_from_chars_fast_fn_fast_conv
    : __has_from_chars_fast_fn_fast_conv<void, Base, Converter, Args...> {};
template <typename Base, typename Converter, typename... Args>
constexpr bool has_from_chars_fast_fn_fast_conv_v =
    has_from_chars_fast_fn_fast_conv<Base, Converter, Args...>::value;

WJR_REGISTER_HAS_TYPE(from_chars_validate_fast_fn_fast_conv,
                      Base::__fast_conv(std::declval<const void *&>(),
                                        std::declval<Args>()...),
                      Base);

} // namespace convert_details

template <uint64_t Base>
class __to_chars_unroll_2_fast_fn_impl_base {
public:
    template <typename Converter>
    WJR_INTRINSIC_INLINE static void __fast_conv(void *ptr, uint32_t val, Converter) {
        auto str = (char *)ptr;
        if constexpr (Base * Base <= 16) {
            constexpr auto &table = __char_converter_table<Converter, Base, 4>;
            ::memcpy(str, table.data() + val * 4 + 2, 2);
        } else {
            constexpr auto &table = __char_converter_table<Converter, Base, 2>;
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
    template <typename Converter>
    WJR_INTRINSIC_INLINE static void __fast_conv(void *ptr, uint32_t val, Converter) {
        auto str = (char *)ptr;
        if constexpr (Base * Base <= 16) {
            constexpr auto &table = __char_converter_table<Converter, Base, 4>;
            ::memcpy(str, table.data() + val * 4, 4);
        } else {
            constexpr auto &table = __char_converter_table<Converter, Base, 2>;
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

template <>
class __to_chars_unroll_8_fast_fn_impl<10>
    : public __to_chars_unroll_8_fast_fn_impl_base<10> {};

template <uint64_t Base>
class __to_chars_unroll_2_fn : public __to_chars_unroll_2_fast_fn_impl<Base> {
    using Mybase = __to_chars_unroll_2_fast_fn_impl<Base>;

public:
    template <typename Iter, typename Converter>
    WJR_INTRINSIC_INLINE void operator()(Iter ptr, uint32_t val, Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      convert_details::has_to_chars_fast_fn_fast_conv_v<Mybase, uint32_t,
                                                                        Converter>) {
            Mybase::__fast_conv(to_address(ptr), val, conv);
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
    template <typename Iter, typename Converter>
    WJR_INTRINSIC_INLINE void operator()(Iter ptr, uint32_t val, Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      convert_details::has_to_chars_fast_fn_fast_conv_v<Mybase, uint32_t,
                                                                        Converter>) {
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
                      convert_details::has_to_chars_fast_fn_fast_conv_v<Mybase, uint64_t,
                                                                        Converter>) {
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
    WJR_PURE WJR_INTRINSIC_INLINE static uint32_t __fast_conv(const void *ptr,
                                                              origin_converter_t) {
#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_8_FAST)
        return builtin_from_chars_unroll_8_fast<Base>(ptr, origin_converter);
#else
        return __fast_conv(read_memory<uint64_t>(ptr));
#endif
    }

    WJR_PURE WJR_INTRINSIC_INLINE static uint32_t __fast_conv(const void *ptr,
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
                      convert_details::has_from_chars_fast_fn_fast_conv_v<Mybase,
                                                                          Converter>) {
            return Mybase::__fast_conv(to_address(first), conv);
        } else {
            uint64_t value = 0;
            value = conv.template from<Base>(*first++);
            value = value * Base + conv.template from<Base>(*first++);
            value = value * Base + conv.template from<Base>(*first++);
            return value * Base + conv.template from<Base>(*first++);
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
                      convert_details::has_from_chars_fast_fn_fast_conv_v<Mybase,
                                                                          Converter>) {
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
                      convert_details::has_from_chars_fast_fn_fast_conv_v<Mybase,
                                                                          Converter>) {
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

struct __from_chars_validate_result {
    bool ok;
    int offset;
};

template <uint64_t Base>
class __from_chars_validate_unroll_8_fast_fn_impl_base
    : private __from_chars_unroll_8_fast_fn_impl_base<Base> {

    WJR_INTRINSIC_INLINE static __from_chars_validate_result
    __fast_conv(uint64_t in, const void *&ptr, uint32_t &val) {
        constexpr uint64_t subt = broadcast<uint8_t, uint64_t>(Base);
        constexpr uint64_t mask = broadcast<uint8_t, uint64_t>(128);

        uint64_t xval = (in - subt) & mask;

        if (WJR_UNLIKELY(xval != mask)) {
            const auto offset = ctz((~xval) & mask);

            if (WJR_LIKELY(offset != 0)) {
                ptr = static_cast<const void *>(static_cast<const char *>(ptr) +
                                                offset / 8);
                in <<= 64 - offset;
                val = __from_chars_unroll_8_fast_fn_impl_base<Base>::__fast_conv(in);
            }

            return {false, offset / 8};
        }

        ptr = static_cast<const void *>(static_cast<const char *>(ptr) + 8);
        val = __from_chars_unroll_8_fast_fn_impl_base<Base>::__fast_conv(in);

        return {true, 0};
    }

public:
    // WJR_INTRINSIC_INLINE static __from_chars_validate_result
    // __fast_conv(const void *&ptr, uint32_t &val, origin_converter_t) {
    //     return __fast_conv(read_memory<uint64_t>(ptr), ptr, val);
    // }

    // WJR_INTRINSIC_INLINE static __from_chars_validate_result
    // __fast_conv(const void *&ptr, uint32_t &val, char_converter_t) {
    //     return __fast_conv(read_memory<uint64_t>(ptr) - 0x3030303030303030ull, ptr,
    //     val);
    // }
};

template <uint64_t Base>
class __from_chars_validate_unroll_8_fast_fn_impl {};

template <>
class __from_chars_validate_unroll_8_fast_fn_impl<2>
    : public __from_chars_validate_unroll_8_fast_fn_impl_base<2> {};

template <>
class __from_chars_validate_unroll_8_fast_fn_impl<8>
    : public __from_chars_validate_unroll_8_fast_fn_impl_base<8> {};

template <>
class __from_chars_validate_unroll_8_fast_fn_impl<10>
    : public __from_chars_validate_unroll_8_fast_fn_impl_base<10> {};

template <uint64_t Base>
class __from_chars_validate_unroll_8_fn
    : public __from_chars_validate_unroll_8_fast_fn_impl<Base> {
    using Mybase = __from_chars_validate_unroll_8_fast_fn_impl<Base>;

public:
    /**
     * @return 0 if sucessed \n
     * otherwise return offset of error
     *
     */
    template <typename Iter, typename Converter>
    WJR_INTRINSIC_INLINE __from_chars_validate_result operator()(Iter &first,
                                                                 uint32_t &val,
                                                                 Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      convert_details::has_from_chars_validate_fast_fn_fast_conv_v<
                          Mybase, uint32_t &, Converter>) {
            const void *ptr = to_address(first);
            const void *end = ptr;
            auto ret = Mybase::__fast_conv(end, val, conv);
            first += static_cast<const char *>(end) - static_cast<const char *>(ptr);
            return ret;
        } else {
            uint8_t ch = conv.from(*first);
            if (WJR_UNLIKELY(ch >= Base)) {
                return {false, 0};
            }

            val = ch;

            for (int i = 1; i < 8; ++i) {
                ch = conv.from(*++first);
                if (WJR_UNLIKELY(ch >= Base)) {
                    return {false, i};
                }

                val = val * Base + ch;
            }

            return {true, 0};
        }
    }

    template <typename Iter, typename Converter>
    WJR_INTRINSIC_INLINE void operator()(Iter &first, Converter conv) const {
        if constexpr (__is_fast_convert_iterator_v<Iter, Converter> &&
                      convert_details::has_from_chars_validate_fast_fn_fast_conv_v<
                          Mybase, Converter>) {
            const void *ptr = to_address(first);
            const void *end = ptr;
            Mybase::__fast_conv(end, conv);
            first += static_cast<const char *>(end) - static_cast<const char *>(ptr);
        } else {
            for (int i = 0; i < 8; ++i) {
                uint8_t ch = conv.from(*first);
                if (WJR_UNLIKELY(ch >= Base)) {
                    return;
                }

                ++first;
            }
        }
    }
};

template <uint64_t Base>
inline constexpr __from_chars_validate_unroll_8_fn<Base> __from_chars_validate_unroll_8{};

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

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR_E int count_digits10(T n) {
    int ret = count_digits10_impl(n);
    WJR_ASSUME(1 <= ret && ret <= std::numeric_limits<T>::digits10 + 1);
    return ret;
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR_E int count_digits_power_of_two(T n, int per_bit) {
    return (bit_width(n) + per_bit - 1) / per_bit;
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR_E int count_digits2(T n) {
    return bit_width(n);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR_E int count_digits8(T n) {
    return count_digits_power_of_two(n, 3);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR_E int count_digits16(T n) {
    return count_digits_power_of_two(n, 4);
}

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

template <typename Value, typename Converter>
struct __is_valid_chars_convert
    : std::disjunction<
          std::conjunction<std::is_same<remove_cvref_t<Converter>, char_converter_t>,
                           is_nonbool_integral<remove_cvref_t<Value>>>,
          std::conjunction<std::is_same<remove_cvref_t<Converter>, origin_converter_t>,
                           is_nonbool_unsigned_integral<remove_cvref_t<Value>>>> {};

template <typename Value, typename Converter>
inline constexpr bool __is_valid_chars_convert_v =
    __is_valid_chars_convert<Value, Converter>::value;

template <typename Iter, typename UnsignedValue, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
Iter __unsigned_to_chars_2_backward(Iter ptr, int n, UnsignedValue x,
                                    Converter conv = {}) {
    constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;
    WJR_ASSERT(x != 0);
    WJR_ASSERT_ASSUME(1 <= n && n <= nd);
    (void)(nd);

    if (WJR_UNLIKELY(n >= 4)) {
        do {
            ptr = std::prev(ptr, 4);
            __to_chars_unroll_4<2>(ptr, x & 0x0f, conv);
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
        ptr = std::prev(ptr, 2);
        __to_chars_unroll_2<2>(ptr, x, conv);
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

template <typename Iter, typename UnsignedValue, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
Iter __unsigned_to_chars_8_backward(Iter ptr, int n, UnsignedValue x,
                                    Converter conv = {}) {
    constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;
    WJR_ASSERT(x != 0);
    WJR_ASSERT_ASSUME(1 <= n && n <= (nd + 2) / 3);

    if constexpr (nd >= 16) {
        if (WJR_UNLIKELY(n >= 4)) {
            do {
                ptr = std::prev(ptr, 4);
                __to_chars_unroll_4<8>(ptr, x & 0x0fff, conv);
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
        ptr = std::prev(ptr, 2);
        __to_chars_unroll_2<8>(ptr, x, conv);
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

template <typename Iter, typename UnsignedValue, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
Iter __unsigned_to_chars_16_backward(Iter ptr, int n, UnsignedValue x,
                                     Converter conv = {}) {
    constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;
    WJR_ASSERT(x != 0);
    WJR_ASSERT_ASSUME(1 <= n && n <= (nd + 3) / 4);

    if constexpr (nd >= 16) {
        if (WJR_UNLIKELY(n >= 4)) {
            do {
                ptr = std::prev(ptr, 4);
                __to_chars_unroll_4<16>(ptr, x & 0xffff, conv);
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
        ptr = std::prev(ptr, 2);
        __to_chars_unroll_2<16>(ptr, x, conv);
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

template <typename Iter, typename UnsignedValue, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
Iter __unsigned_to_chars_power_of_two_backward(Iter ptr, int n, UnsignedValue x,
                                               int per_bit, Converter conv = {}) {
    WJR_ASSERT(x != 0);
    WJR_ASSERT_ASSUME(1 <= n && n <= std::numeric_limits<UnsignedValue>::digits);

    unsigned int mask = (1u << per_bit) - 1;

    do {
        *--ptr = conv.to(x & mask);
        x >>= per_bit;
        --n;
    } while (WJR_LIKELY(n != 0));

    return ptr;
}

template <typename Iter, typename UnsignedValue, typename Converter,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
Iter __unsigned_to_chars_10_backward(Iter ptr, UnsignedValue val, Converter conv) {
    WJR_ASSERT_ASSUME(val != 0);

    if (WJR_LIKELY(val >= 100)) {
        do {
            ptr = std::prev(ptr, 2);
            __to_chars_unroll_2<10>(ptr, val % 100, conv);
            val /= 100;
        } while (WJR_LIKELY(val >= 100));
    }

    if (val < 10) {
        *--ptr = conv.template to<10>(val);
        return ptr;
    }

    ptr = std::prev(ptr, 2);
    __to_chars_unroll_2<10>(ptr, val, conv);
    return ptr;
}

template <typename Iter, typename Value, typename IBase, typename Converter>
to_chars_result<Iter> __to_chars_backward_validate_impl(Iter ptr, Iter first, Value val,
                                                        IBase ibase, Converter conv) {
    if (WJR_UNLIKELY(val == 0)) {
        if (WJR_LIKELY(first != ptr)) {
            *--ptr = conv.template to<1>(0);
            return {ptr, std::errc{}};
        }

        return {ptr, std::errc::value_too_large};
    }

    auto uVal = to_unsigned(val);
    auto size = std::distance(first, ptr);
    bool sign = false;

    if constexpr (std::is_signed_v<Value>) {
        if (val < 0) {
            if (WJR_UNLIKELY(size <= 1)) {
                return {ptr, std::errc::value_too_large};
            }

            sign = true;
            uVal = -val;
            --size;
        }
    }

    const auto base = get_integral_constant_v(ibase);
    to_chars_result<Iter> ret;

    switch (base) {
    case 2: {
        int n = count_digits2(uVal);
        if (WJR_LIKELY(n <= size)) {
            ret = {__unsigned_to_chars_2_backward(ptr, n, uVal, conv), std::errc{}};
            break;
        }

        return {ptr, std::errc::value_too_large};
    }
    case 8: {
        int n = count_digits8(uVal);
        if (WJR_LIKELY(n <= size)) {
            ret = {__unsigned_to_chars_8_backward(ptr, n, uVal, conv), std::errc{}};
            break;
        }

        return {ptr, std::errc::value_too_large};
    }
    case 16: {
        int n = count_digits16(uVal);
        if (WJR_LIKELY(n <= size)) {
            ret = {__unsigned_to_chars_16_backward(ptr, n, uVal, conv), std::errc{}};
            break;
        }

        return {ptr, std::errc::value_too_large};
    }
    case 4:
    case 32: {
        int per_bit = base == 4 ? 2 : 5;
        int n = count_digits_power_of_two(uVal, per_bit);
        if (WJR_LIKELY(n <= size)) {
            ret = {__unsigned_to_chars_power_of_two_backward(ptr, n, uVal, per_bit, conv),
                   std::errc{}};
            break;
        }

        return {ptr, std::errc::value_too_large};
    }
    case 10: {
        int n = count_digits10(uVal);
        if (WJR_LIKELY(n <= size)) {
            ret = {__unsigned_to_chars_10_backward(ptr, uVal, conv), std::errc{}};
            break;
        }

        return {ptr, std::errc::value_too_large};
    }
    default: {
        WJR_UNREACHABLE();
        return {ptr, std::errc::invalid_argument};
    }
    }

    if constexpr (std::is_signed_v<Value>) {
        if (sign) {
            *--ret.ptr = '-';
        }
    }

    return ret;
}

template <typename Iter, typename Value, typename BaseType = unsigned int,
          BaseType IBase = 10, typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
to_chars_result<Iter>
to_chars_backward_validate(Iter ptr, Iter first, Value val,
                           std::integral_constant<BaseType, IBase> = {},
                           Converter conv = {}) {
    return __to_chars_backward_validate_impl(
        ptr, first, val, std::integral_constant<unsigned int, IBase>(), conv);
}

/**
 * @brief Convert an unsigned integer to a string in reverse order with checking buf size.
 *
 * @tparam Iter The iterator type. Must be random access iterator.
 * @tparam Value The value type. If Converter is origin_converter_t, Value must be
 * non-bool unsigned integral type. Otherwise, Value must be non-bool integral type.
 *
 * @return to_chars_result<Iter> If the conversion is successful, return {ans,
 * std::errc{}}. Otherwise, return {ptr, std::errc::value_too_large}.
 *
 */
template <typename Iter, typename Value, typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
to_chars_result<Iter> to_chars_backward_validate(Iter ptr, Iter first, Value val,
                                                 unsigned int base, Converter conv = {}) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return to_chars_backward_validate(
                ptr, first, val, std::integral_constant<unsigned int, 2>{}, conv);
        }
        case 8: {
            return to_chars_backward_validate(
                ptr, first, val, std::integral_constant<unsigned int, 8>{}, conv);
        }
        case 16: {
            return to_chars_backward_validate(
                ptr, first, val, std::integral_constant<unsigned int, 16>{}, conv);
        }
        case 10: {
            return to_chars_backward_validate(
                ptr, first, val, std::integral_constant<unsigned int, 10>{}, conv);
        }
        default: {
            break;
        }
        }
    }

    return __to_chars_backward_validate_impl(ptr, first, val, base, conv);
}

template <typename Iter, typename Value, typename IBase, typename Converter>
Iter __to_chars_backward_impl(Iter ptr, Value val, IBase ibase, Converter conv) {
    if (WJR_UNLIKELY(val == 0)) {
        *--ptr = conv.template to<1>(0);
        return ptr;
    }

    auto uVal = to_unsigned(val);
    bool sign = false;

    if constexpr (std::is_signed_v<Value>) {
        if (val < 0) {
            sign = true;
            uVal = -val;
        }
    }

    unsigned int base = get_integral_constant_v(ibase);

    switch (base) {
    case 2: {
        ptr = __unsigned_to_chars_2_backward(ptr, count_digits2(uVal), uVal, conv);
        break;
    }
    case 8: {
        ptr = __unsigned_to_chars_8_backward(ptr, count_digits8(uVal), uVal, conv);
        break;
    }
    case 16: {
        ptr = __unsigned_to_chars_16_backward(ptr, count_digits16(uVal), uVal, conv);
        break;
    }
    case 4:
    case 32: {
        int per_bit = base == 4 ? 2 : 5;
        ptr = __unsigned_to_chars_power_of_two_backward(
            ptr, count_digits_power_of_two(uVal, per_bit), uVal, per_bit, conv);
        break;
    }
    case 10: {
        ptr = __unsigned_to_chars_10_backward(ptr, uVal, conv);
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

template <typename Iter, typename Value, typename BaseType = unsigned int,
          BaseType IBase = 10, typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
Iter to_chars_backward(Iter ptr, Value val, std::integral_constant<BaseType, IBase> = {},
                       Converter conv = {}) {
    return __to_chars_backward_impl(ptr, val,
                                    std::integral_constant<unsigned int, IBase>(), conv);
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
template <typename Iter, typename Value, typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
Iter to_chars_backward(Iter ptr, Value val, unsigned int base, Converter conv = {}) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return to_chars_backward(ptr, val, std::integral_constant<unsigned int, 2>{},
                                     conv);
        }
        case 8: {
            return to_chars_backward(ptr, val, std::integral_constant<unsigned int, 8>{},
                                     conv);
        }
        case 16: {
            return to_chars_backward(ptr, val, std::integral_constant<unsigned int, 16>{},
                                     conv);
        }
        case 10: {
            return to_chars_backward(ptr, val, std::integral_constant<unsigned int, 10>{},
                                     conv);
        }
        default: {
            break;
        }
        }
    }

    return __to_chars_backward_impl(ptr, val, base, conv);
}

template <typename Iter, typename Value, typename IBase, typename Converter>
to_chars_result<Iter> __to_chars_validate_impl(Iter ptr, Iter last, Value val,
                                               IBase ibase, Converter conv) {
    if (WJR_UNLIKELY(val == 0)) {
        if (WJR_UNLIKELY(ptr == last)) {
            return {last, std::errc::value_too_large};
        }

        *ptr++ = conv.template to<1>(0);
        return {ptr, std::errc{}};
    }

    auto uVal = to_unsigned(val);

    if constexpr (std::is_signed_v<Value>) {
        if (val < 0) {
            if (WJR_UNLIKELY(ptr == last)) {
                return {last, std::errc::value_too_large};
            }

            *ptr++ = '-';
            uVal = -val;
        }
    }

    const auto base = get_integral_constant_v(ibase);
    auto size = std::distance(ptr, last);

    switch (base) {
    case 2: {
        int n = count_digits2(uVal);
        if (WJR_LIKELY(n <= size)) {
            ptr += n;
            (void)__unsigned_to_chars_2_backward(ptr, n, uVal, conv);
            return {ptr, std::errc{}};
        }

        return {last, std::errc::value_too_large};
    }
    case 8: {
        int n = count_digits8(uVal);
        if (WJR_LIKELY(n <= size)) {
            ptr += n;
            (void)__unsigned_to_chars_8_backward(ptr, n, uVal, conv);
            return {ptr, std::errc{}};
        }

        return {last, std::errc::value_too_large};
    }
    case 16: {
        int n = count_digits16(uVal);
        if (WJR_LIKELY(n <= size)) {
            ptr += n;
            (void)__unsigned_to_chars_16_backward(ptr, n, uVal, conv);
            return {ptr, std::errc{}};
        }

        return {last, std::errc::value_too_large};
    }
    case 4:
    case 32: {
        int per_bit = base == 4 ? 2 : 5;
        int n = count_digits_power_of_two(uVal, per_bit);
        if (WJR_LIKELY(n <= size)) {
            ptr += n;
            (void)__unsigned_to_chars_power_of_two_backward(ptr, n, uVal, per_bit, conv);
            return {ptr, std::errc{}};
        }

        return {last, std::errc::value_too_large};
    }
    case 10: {
        int n = count_digits10(uVal);
        if (WJR_LIKELY(n <= size)) {
            ptr += n;
            (void)__unsigned_to_chars_10_backward(ptr, uVal, conv);
            return {ptr, std::errc{}};
        }

        return {last, std::errc::value_too_large};
    }
    default: {
        WJR_UNREACHABLE();
        return {last, std::errc::invalid_argument};
    }
    }
}

template <typename Iter, typename Value, typename BaseType = unsigned int,
          BaseType IBase = 10, typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
to_chars_result<Iter> to_chars_validate(Iter ptr, Iter last, Value val,
                                        std::integral_constant<BaseType, IBase> = {},
                                        Converter conv = {}) {
    return __to_chars_validate_impl(ptr, last, val,
                                    std::integral_constant<unsigned int, IBase>(), conv);
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
template <typename Iter, typename Value, typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
to_chars_result<Iter> to_chars_validate(Iter ptr, Iter last, Value val, unsigned int base,
                                        Converter conv = {}) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return to_chars_validate(ptr, last, val,
                                     std::integral_constant<unsigned int, 2>{}, conv);
        }
        case 8: {
            return to_chars_validate(ptr, last, val,
                                     std::integral_constant<unsigned int, 8>{}, conv);
        }
        case 16: {
            return to_chars_validate(ptr, last, val,
                                     std::integral_constant<unsigned int, 16>{}, conv);
        }
        case 10: {
            return to_chars_validate(ptr, last, val,
                                     std::integral_constant<unsigned int, 10>{}, conv);
        }
        default: {
            break;
        }
        }
    }

    return __to_chars_validate_impl(ptr, last, val, base, conv);
}

template <typename Iter, typename Value, typename IBase, typename Converter>
Iter __to_chars_impl(Iter ptr, Value val, IBase ibase, Converter conv) {
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

    const auto base = get_integral_constant_v(ibase);

    switch (base) {
    case 2: {
        int n = count_digits2(uVal);
        ptr += n;
        (void)__unsigned_to_chars_2_backward(ptr, n, uVal, conv);
        return ptr;
    }
    case 8: {
        int n = count_digits8(uVal);
        ptr += n;
        (void)__unsigned_to_chars_8_backward(ptr, n, uVal, conv);
        return ptr;
    }
    case 16: {
        int n = count_digits16(uVal);
        ptr += n;
        (void)__unsigned_to_chars_16_backward(ptr, n, uVal, conv);
        return ptr;
    }
    case 4:
    case 32: {
        int per_bit = base == 4 ? 2 : 5;
        int n = count_digits_power_of_two(uVal, per_bit);
        ptr += n;
        (void)__unsigned_to_chars_power_of_two_backward(ptr, n, uVal, per_bit, conv);
        return ptr;
    }
    case 10: {
        int n = count_digits10(uVal);
        ptr += n;
        (void)__unsigned_to_chars_10_backward(ptr, uVal, conv);
        return ptr;
    }
    default: {
        WJR_UNREACHABLE();
        return ptr;
    }
    }
}

template <typename Iter, typename Value, typename BaseType = unsigned int,
          BaseType IBase = 10, typename Converter = char_converter_t,
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
Iter to_chars(Iter ptr, Value val, std::integral_constant<BaseType, IBase> = {},
              Converter conv = {}) {
    return __to_chars_impl(ptr, val, std::integral_constant<unsigned int, IBase>(), conv);
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
          std::enable_if_t<__is_valid_chars_convert_v<Value, Converter>, int> = 0>
Iter to_chars(Iter ptr, Value val, unsigned int base, Converter conv = {}) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            return to_chars(ptr, val, std::integral_constant<unsigned int, 2>{}, conv);
        }
        case 8: {
            return to_chars(ptr, val, std::integral_constant<unsigned int, 8>{}, conv);
        }
        case 16: {
            return to_chars(ptr, val, std::integral_constant<unsigned int, 16>{}, conv);
        }
        case 10: {
            return to_chars(ptr, val, std::integral_constant<unsigned int, 10>{}, conv);
        }
        default: {
            break;
        }
        }
    }

    return __to_chars_impl(ptr, val, base, conv);
}

template <typename Iter, typename Converter>
size_t __biginteger_to_chars_2_impl(Iter first, uint64_t *up, size_t n, Converter conv) {
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
            
            __to_chars_unroll_8<2>(first - 8, x & 0xff, conv);
            first -= 8;
            x >>= 8;
        }

        ++up;
        --n;
    } while (n);
    x = *up;

    (void)__unsigned_to_chars_2_backward(first, hbits, x, conv);
    return len;
}

template <typename Iter, typename Converter>
size_t __biginteger_to_chars_8_impl(Iter first, uint64_t *up, size_t n, Converter conv) {
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

template <typename Iter, typename Converter>
size_t __biginteger_to_chars_16_impl(Iter first, uint64_t *up, size_t n, Converter conv) {
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

        __to_chars_unroll_8<16>(first - 8, x & 0xffff'ffff, conv);
        __to_chars_unroll_8<16>(first - 16, x >> 32, conv);
        first -= 16;

        ++up;
        --n;
    } while (n);
    x = *up;

    (void)__unsigned_to_chars_16_backward(first, hbits, x, conv);

    return len;
}

template <typename Iter, typename Converter>
size_t __biginteger_to_chars_power_of_two_impl(Iter first, uint64_t *up, size_t n,
                                               unsigned int base, Converter conv) {
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
char *basecase_to_chars_10(char *buf, uint64_t *up, size_t n, Converter conv) {
    do {
        if (WJR_UNLIKELY(n == 1)) {
            return __unsigned_to_chars_10_backward(buf, up[0], conv);
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

template <typename Iter, typename Converter>
Iter basecase_to_chars(Iter first, size_t len, uint64_t *up, size_t n, unsigned int base,
                       Converter conv) {
    constexpr size_t buf_len = dc_bignum_to_chars_precompute_threshold * 64 * 7 / 11;
    char buf[buf_len];
    char *end = buf + buf_len;
    char *start;

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

template <typename Iter, typename Converter>
Iter __large_basecase_to_chars(Iter first, uint64_t *up, size_t n, unsigned int base,
                               Converter conv) {
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
 * @brief Use by `to_chars` to convert a large integer to a string.
 *
 * @note The length of the biginteger is greater than or equal to 2.
 */
template <typename Iter, typename Converter = char_converter_t,
          std::enable_if_t<is_any_of_v<remove_cvref_t<Converter>, char_converter_t,
                                       origin_converter_t>,
                           int> = 0>
Iter __biginteger_to_chars_impl(Iter first, uint64_t *up, size_t n,
                                unsigned int base = 10, Converter conv = {}) {
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

    return __large_basecase_to_chars(first, up, n, base, conv);
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
Iter biginteger_to_chars(Iter first, uint64_t *up, size_t n, unsigned int base = 10,
                         Converter conv = {}) {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));
    WJR_ASSERT_ASSUME(up[n - 1] != 0);

    if (n == 1) {
        return to_chars(first, up[0], base, conv);
    }

    return __biginteger_to_chars_impl(first, up, n, base, conv);
}

template <typename Iter, typename UnsignedValue, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
void __unsigned_from_chars_2(Iter first, Iter last, UnsignedValue &val, Converter conv) {
    constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;

    auto n = std::distance(first, last);
    WJR_ASSERT_ASSUME(1 <= n && n <= nd);

    UnsignedValue xval = 0;

    if constexpr (nd >= 16) {
        if (WJR_UNLIKELY(n >= 8)) {
            do {
                xval = (xval << 8) + __from_chars_unroll_8<2>(first, conv);
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
            val = __from_chars_unroll_8<2>(first, conv);
            first += 8;
            return;
        }
    }

    switch (n) {
    case 7: {
        xval = (xval << 1) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 6: {
        xval = (xval << 1) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 5: {
        xval = (xval << 1) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 4: {
        xval <<= 4;
        xval += __from_chars_unroll_4<2>(first, conv);
        first += 4;
        break;
    }
    case 3: {
        xval = (xval << 1) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 2: {
        xval = (xval << 1) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 1: {
        xval = (xval << 1) + conv.from(*first++);
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

    val = xval;
}

template <typename Iter, typename UnsignedValue, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
void __unsigned_from_chars_8(Iter first, Iter last, UnsignedValue &val,
                             Converter conv = {}) {
    constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;

    auto n = std::distance(first, last);
    WJR_ASSERT_ASSUME(1 <= n && n <= (nd + 2) / 3);

    UnsignedValue xval = 0;

    if constexpr (nd >= 32) {
        if (WJR_UNLIKELY(n >= 8)) {
            do {
                xval = (xval << 24) + __from_chars_unroll_8<8>(first, conv);
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
        xval = (xval << 3) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 6: {
        xval = (xval << 3) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 5: {
        xval = (xval << 3) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 4: {
        xval <<= 12;
        xval += __from_chars_unroll_4<8>(first, conv);
        first += 4;
        break;
    }
    case 3: {
        xval = (xval << 3) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 2: {
        xval = (xval << 3) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 1: {
        xval = (xval << 3) + conv.from(*first++);
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

    val = xval;
}

template <typename Iter, typename UnsignedValue, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
void __unsigned_from_chars_16(Iter first, Iter last, UnsignedValue &val,
                              Converter conv = {}) {
    constexpr auto nd = std::numeric_limits<UnsignedValue>::digits;

    auto n = std::distance(first, last);
    WJR_ASSERT_ASSUME(1 <= n && n <= (nd + 3) / 4);

    UnsignedValue xval = 0;

    if constexpr (nd >= 64) {
        if (WJR_UNLIKELY(n >= 8)) {
            do {
                xval = (xval << 32) + __from_chars_unroll_8<16>(first, conv);
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
            val = __from_chars_unroll_8<16>(first, conv);
            first += 8;
            return;
        }
    }

    switch (n) {
    case 7: {
        xval = (xval << 4) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 6: {
        xval = (xval << 4) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 5: {
        xval = (xval << 4) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 4: {
        xval <<= 16;
        xval += __from_chars_unroll_4<16>(first, conv);
        first += 4;
        break;
    }
    case 3: {
        xval = (xval << 4) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 2: {
        xval = (xval << 4) + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 1: {
        xval = (xval << 4) + conv.from(*first++);
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

    val = xval;
}

template <typename Iter, typename UnsignedValue, typename Converter,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
void __unsigned_from_chars_10(Iter first, Iter last, UnsignedValue &val, Converter conv) {
    constexpr auto nd = std::numeric_limits<UnsignedValue>::digits10 + 1;

    auto n = std::distance(first, last);
    WJR_ASSUME(1 <= n && n <= nd);

    UnsignedValue xval = 0;

    if constexpr (nd >= 8) {
        if (WJR_UNLIKELY(n >= 8)) {
            if (WJR_UNLIKELY(n >= 16)) {
                xval = __from_chars_unroll_16<10>(first, conv);
                first += 16;
                n -= 16;
            } else {
                xval = __from_chars_unroll_8<10>(first, conv);
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
        xval = xval * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 6: {
        xval = xval * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 5: {
        xval = xval * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 4: {
        xval = (xval * 10000) + __from_chars_unroll_4<10>(first, conv);
        break;
    }
    case 3: {
        xval = xval * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 2: {
        xval = xval * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 1: {
        xval = xval * 10 + conv.from(*first++);
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

template <typename Iter, typename UnsignedValue, typename Converter,
          std::enable_if_t<is_nonbool_unsigned_integral_v<UnsignedValue>, int> = 0>
bool __unsigned_from_chars_10_validate(Iter &first, Iter last, UnsignedValue &val,
                                       Converter conv) {
    constexpr auto nd = std::numeric_limits<UnsignedValue>::digits10 + 1;
    constexpr auto zero = conv.template to<1>(0);
    static_assert(nd < 24, "The number of digits of UnsignedValue must be less than 24.");

    while (first != last && *first == zero) {
        ++first;
    }

    UnsignedValue xval = 0;

    while (first != last) {
        uint8_t ch = conv.template from<10>(*first);
        if (WJR_UNLIKELY(ch >= 10)) {
            val = xval;
            return true;
        }

        ++first;
        if (WJR_UNLIKELY(mul_overflow(xval, 10, xval) || add_overflow(xval, ch, xval))) {
            while (first != last && conv.template from<10>(*first) < 10) {
                ++first;
            }

            return false;
        }
    }

    val = xval;
    return true;
}

template <typename Iter, typename Value, typename IBase,
          typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_integral_v<Value>, int> = 0>
void __from_chars_impl(Iter first, Iter last, Value &val, IBase ibase,
                       Converter conv = {}) {
    bool sign = false;

    if constexpr (std::is_signed_v<Value>) {
        WJR_ASSERT(first != last);

        if (*first == '-') {
            ++first;
            sign = true;
        }
    }

    std::make_unsigned_t<Value> uVal = 0;

    const auto base = get_integral_constant_v(ibase);

    switch (base) {
    case 2: {
        __unsigned_from_chars_2(first, last, uVal, conv);
        break;
    }
    case 8: {
        __unsigned_from_chars_8(first, last, uVal, conv);
        break;
    }
    case 16: {
        __unsigned_from_chars_16(first, last, uVal, conv);
        break;
    }
    case 10: {
        __unsigned_from_chars_10(first, last, uVal, conv);
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

template <typename Iter, typename Value, typename BaseType = unsigned int,
          BaseType IBase = 10, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_integral_v<Value>, int> = 0>
void from_chars(Iter first, Iter last, Value &val,
                std::integral_constant<BaseType, IBase> = {}, Converter conv = {}) {
    __from_chars_impl(first, last, val, std::integral_constant<unsigned int, IBase>(),
                      conv);
}

template <typename Iter, typename Value, typename Converter = char_converter_t,
          std::enable_if_t<is_nonbool_integral_v<Value>, int> = 0>
void from_chars(Iter first, Iter last, Value &val, unsigned int base,
                Converter conv = {}) {
    if (WJR_BUILTIN_CONSTANT_P(base)) {
        switch (base) {
        case 2: {
            __from_chars_impl(first, last, val, std::integral_constant<unsigned int, 2>(),
                              conv);
            return;
        }
        case 8: {
            __from_chars_impl(first, last, val, std::integral_constant<unsigned int, 8>(),
                              conv);
            return;
        }
        case 16: {
            __from_chars_impl(first, last, val,
                              std::integral_constant<unsigned int, 16>(), conv);
            return;
        }
        case 10: {
            __from_chars_impl(first, last, val,
                              std::integral_constant<unsigned int, 10>(), conv);
            return;
        }
        default: {
            break;
        }
        }
    }

    __from_chars_impl(first, last, val, base, conv);
}

template <typename Iter, typename Converter = char_converter_t>
size_t __biginteger_from_chars_2_impl(Iter first, size_t n, uint64_t *up,
                                      Converter conv = {}) {
    size_t hbits = (n - 1) % 64 + 1;
    size_t len = (n - 1) / 64 + 1;

    uint64_t x = 0;
    up += len;

    __unsigned_from_chars_2(first, first + hbits, x, conv);
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

template <typename Iter, typename Converter = char_converter_t>
size_t __biginteger_from_chars_8_impl(Iter first, size_t n, uint64_t *up,
                                      Converter conv = {}) {
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

    if (WJR_UNLIKELY(hbits == 0)) {
    } else {
        __unsigned_from_chars_8(first, first + hbits, x, conv);
        first += hbits;
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
size_t __biginteger_from_chars_16_impl(Iter first, size_t n, uint64_t *up,
                                       Converter conv = {}) {
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

    __unsigned_from_chars_16(first, first + hbits, x, conv);
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

template <typename Iter, typename Converter>
size_t basecase_from_chars_10(Iter first, size_t n, uint64_t *up, Converter conv) {
    uint64_t x = 0;

    if (n <= 19) {
        __unsigned_from_chars_10(first, first + n, x, conv);
        up[0] = x;

        return up[0] != 0;
    }

    size_t m = (n - 1) % 19 + 1;

    __unsigned_from_chars_10(first, first + m, x, conv);
    up[0] = x;

    first += m;
    n -= m;

    m = up[0] != 0;

    do {
        x = 0;

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
uint64_t *biginteger_from_chars(Iter first, Iter last, uint64_t *up,
                                unsigned int base = 10, Converter conv = {}) {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));
    size_t n = std::distance(first, last);

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
    size_t un = n / per_digits + 1;
    auto stk =
        static_cast<uint64_t *>(stkal.allocate((un * 16 / 5 + 192) * sizeof(uint64_t)));
    auto mpre = precompute_chars_convert(pre, un, base, stk);
    stk += un * 8 / 5 + 128;
    return up + dc_from_chars(first, n, up, mpre, stk, conv);
}

} // namespace wjr

#endif // WJR_MATH_CONVERT_HPP__