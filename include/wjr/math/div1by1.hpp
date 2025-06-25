#ifndef WJR_MATH_DIV1BY1_HPP__
#define WJR_MATH_DIV1BY1_HPP__

#include <wjr/assert.hpp>
#include <wjr/math/div-impl.hpp>
#include <wjr/math/mul.hpp>

namespace wjr {

#pragma pack(push, 1)

template <typename T>
struct div1by1_uint_t {
    T magic;
    uint8_t more;
};

#pragma pack(pop)

enum {
    DIV1BY1_16_SHIFT_MASK = 0x1F,
    DIV1BY1_32_SHIFT_MASK = 0x1F,
    DIV1BY1_64_SHIFT_MASK = 0x3F,
    DIV1BY1_ADD_MARKER = 0x40,
};

template <typename T, branch type>
WJR_CONST WJR_INTRINSIC_INLINE div1by1_uint_t<T> div1by1_internal_uint_gen(T d) noexcept;

template <typename T, branch type>
WJR_CONST WJR_INTRINSIC_INLINE div1by1_uint_t<T> div1by1_uint_gen(T d) noexcept;

template <typename T, branch type>
WJR_CONST WJR_INTRINSIC_INLINE T div1by1_uint_do(T d, const div1by1_uint_t<T> &denom) noexcept;

template <typename T>
WJR_INTRINSIC_INLINE T div1by1_div_half(T hi, T lo, T den, T &rem) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;
    using U = uint_t<nd * 2>;
    const U n = (static_cast<U>(hi) << nd) | lo;
    const auto result = static_cast<T>(n / den);
    rem = n - result * static_cast<U>(den);
    return result;
}

template <>
WJR_INTRINSIC_INLINE uint64_t div1by1_div_half<uint64_t>(uint64_t hi, uint64_t lo, uint64_t den,
                                                         uint64_t &rem) noexcept {
    return div128by64to64(rem, lo, hi, den);
}

////////// UINT16

template <branch type, typename T>
WJR_CONST WJR_INTRINSIC_INLINE div1by1_uint_t<T> div1by1_internal_uint_gen(T d) noexcept {
    WJR_ASSERT(d != 0, "Can't divide by zero.");

    constexpr auto nd = std::numeric_limits<T>::digits;

    div1by1_uint_t<T> result;
    const uint8_t floor_log_2_d = static_cast<uint8_t>(nd - 1 - clz<T>(d));

    // Power of 2
    if (is_zero_or_single_bit(d)) {
        // We need to subtract 1 from the shift value in case of an unsigned
        // branchfree div1by1_divider because there is a hardcoded right shift
        // by 1 in its division algorithm. Because of this we also need to add
        // back 1 in its recovery algorithm.
        result.magic = 0;
        result.more = static_cast<uint8_t>(floor_log_2_d - (type == branch::free));
    } else {
        uint8_t more;
        T rem, proposed_m;
        proposed_m = div1by1_div_half<T>(static_cast<T>(1) << floor_log_2_d, 0, d, rem);

        WJR_ASSERT(rem > 0 && rem < d);
        const T e = d - rem;

        if (type == branch::full && (e < (static_cast<T>(1) << floor_log_2_d))) {
            more = floor_log_2_d;
        } else {
            proposed_m += proposed_m;
            const T twice_rem = rem + rem;
            if (twice_rem >= d || twice_rem < rem) {
                ++proposed_m;
            }

            more = floor_log_2_d | DIV1BY1_ADD_MARKER;
        }

        result.magic = 1 + proposed_m;
        result.more = more;
    }

    return result;
}

template <typename T>
struct div1by1_shift_mask {
    static const uint8_t value = 0;
};

#define WJR_REGISTER_SHIFT_MASK(nd)                                                                \
    template <>                                                                                    \
    struct div1by1_shift_mask<uint##nd##_t> {                                                      \
        static const uint8_t value = DIV1BY1_##nd##_SHIFT_MASK;                                    \
    }

WJR_REGISTER_SHIFT_MASK(16);
WJR_REGISTER_SHIFT_MASK(32);
WJR_REGISTER_SHIFT_MASK(64);

#undef WJR_REGISTER_SHIFT_MASK

template <typename T, branch type>
WJR_CONST WJR_INTRINSIC_INLINE div1by1_uint_t<T> div1by1_uint_gen(T d) noexcept {
    if constexpr (type == branch::full) {
        return div1by1_internal_uint_gen<type>(d);
    } else {
        WJR_ASSERT(d != 1, "The divisor cannot be 1");
        const auto ret = div1by1_internal_uint_gen<type>(d);
        return div1by1_uint_t<T>{ret.magic,
                                 static_cast<uint8_t>(ret.more & div1by1_shift_mask<T>::value)};
    }
}

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE T
div1by1_uint_branchfull_do_impl(T d, const div1by1_uint_t<T> &denom) noexcept {
    const T magic = denom.magic;
    const uint8_t more = denom.more;
    if (!magic) {
        return d >> more;
    } else {
        const T q = mulhi<T>(magic, d);
        if (more & DIV1BY1_ADD_MARKER) {
            const T t = ((d - q) >> 1) + q;
            return t >> (more & div1by1_shift_mask<T>::value);
        } else {
            return q >> more;
        }
    }
}

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE T
div1by1_uint_branchfree_do_impl(T d, const div1by1_uint_t<T> &denom) noexcept {
    const T q = mulhi<T>(denom.magic, d);
    const T t = ((d - q) >> 1) + q;
    return t >> denom.more;
}

template <typename T, branch type>
WJR_CONST WJR_INTRINSIC_INLINE T div1by1_uint_do(T d, const div1by1_uint_t<T> &denom) noexcept {
    if constexpr (type == branch::full) {
        return div1by1_uint_branchfull_do_impl(d, denom);
    } else {
        return div1by1_uint_branchfree_do_impl(d, denom);
    }
}

template <typename T, branch type = branch::free>
class div1by1_divider {
private:
    using dispatcher_t = div1by1_uint_t<T>;

public:
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(div1by1_divider);

    WJR_INTRINSIC_INLINE div1by1_divider(T d) { div = div1by1_uint_gen<T, type>(d); }

    WJR_INTRINSIC_INLINE T divide(T n) const { return div1by1_uint_do<T, type>(n, div); }

    WJR_CONST bool operator==(const div1by1_divider<T, type> &other) const {
        return div.magic == other.div.magic && div.more == other.more;
    }

    WJR_CONST bool operator!=(const div1by1_divider<T, type> &other) const {
        return !(*this == other);
    }

private:
    dispatcher_t div;
};

template <typename T, branch type>
WJR_INTRINSIC_INLINE T operator/(T n, const div1by1_divider<T, type> &div) {
    return div.divide(n);
}

template <typename T, branch type>
WJR_INTRINSIC_INLINE T &operator/=(T &n, const div1by1_divider<T, type> &div) {
    n = div.divide(n);
    return n;
}

} // namespace wjr

#endif // WJR_MATH_DIV1BY1_HPP__