// libdivide.h - Optimized integer division
// https://libdivide.com
//
// Copyright (C) 2010 - 2022 ridiculous_fish, <libdivide@ridiculousfish.com>
// Copyright (C) 2016 - 2022 Kim Walisch, <kim.walisch@gmail.com>
//
// libdivide is dual-licensed under the Boost or zlib licenses.
// You may use libdivide under the terms of either of these.
// See LICENSE.txt for more details.

#ifndef WJR_LIBDIVIDE_HPP__
#define WJR_LIBDIVIDE_HPP__

#include <wjr/assert.hpp>
#include <wjr/math/div-impl.hpp>
#include <wjr/math/mul.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

// pack div1by1_divider structs to prevent compilers from padding.
// This reduces memory usage by up to 43% when using a large
// array of libdivide dividers and improves performance
// by up to 10% because of reduced memory bandwidth.
#pragma pack(push, 1)

template <typename T>
struct libdivide_uint_t_impl {
    T magic;
    uint8_t more;
};

template <typename T>
struct libdivide_uint_branchfree_t_impl {
    T magic;
    uint8_t more;
};

template <typename T, bool branchfree>
using libdivide_uint_t =
    std::conditional_t<branchfree, libdivide_uint_branchfree_t_impl<T>,
                       libdivide_uint_t_impl<T>>;

#pragma pack(pop)

// Explanation of the "more" field:
//
// * Bits 0-5 is the shift value (for shift path or mult path).
// * Bit 6 is the add indicator for mult path.
// * Bit 7 is set if the divisor is negative. We use bit 7 as the negative
//   divisor indicator so that we can efficiently use sign extension to
//   create a bitmask with all bits set to 1 (if the divisor is negative)
//   or 0 (if the divisor is positive).
//
// u32: [0-4] shift value
//      [5] ignored
//      [6] add indicator
//      magic number of 0 indicates shift path
//
// s32: [0-4] shift value
//      [5] ignored
//      [6] add indicator
//      [7] indicates negative divisor
//      magic number of 0 indicates shift path
//
// u64: [0-5] shift value
//      [6] add indicator
//      magic number of 0 indicates shift path
//
// s64: [0-5] shift value
//      [6] add indicator
//      [7] indicates negative divisor
//      magic number of 0 indicates shift path
//
// In s32 and s64 branchfree modes, the magic number is negated according to
// whether the divisor is negated. In branchfree strategy, it is not negated.

enum {
    LIBDIVIDE_16_SHIFT_MASK = 0x1F,
    LIBDIVIDE_32_SHIFT_MASK = 0x1F,
    LIBDIVIDE_64_SHIFT_MASK = 0x3F,
    LIBDIVIDE_ADD_MARKER = 0x40,
    LIBDIVIDE_NEGATIVE_DIVISOR = 0x80
};

template <typename T, bool branchfree>
WJR_CONST WJR_INTRINSIC_INLINE libdivide_uint_t<T, false>
libdivide_internal_uint_gen(T d) noexcept;

template <typename T, bool branchfree>
WJR_CONST WJR_INTRINSIC_INLINE libdivide_uint_t<T, branchfree>
libdivide_uint_gen(T d) noexcept;

template <typename T, bool branchfree>
WJR_CONST WJR_INTRINSIC_INLINE T
libdivide_uint_do(T d, const libdivide_uint_t<T, branchfree> &denom) noexcept;

//////// Internal Utility Functions

template <typename T>
WJR_INTRINSIC_INLINE T libdivide_div_half(T hi, T lo, T den, T &rem) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;
    using U = uint_t<nd * 2>;
    const U n = (static_cast<U>(hi) << nd) | lo;
    const auto result = static_cast<T>(n / den);
    rem = n - result * static_cast<U>(den);
    return result;
}

template <>
WJR_INTRINSIC_INLINE uint64_t libdivide_div_half<uint64_t>(uint64_t hi, uint64_t lo,
                                                           uint64_t den,
                                                           uint64_t &rem) noexcept {
    return div128by64to64(rem, lo, hi, den);
}

////////// UINT16

template <typename T, bool branchfree>
WJR_CONST WJR_INTRINSIC_INLINE libdivide_uint_t<T, false>
libdivide_internal_uint_gen(T d) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;

    WJR_ASSERT(d != 0, "div1by1_divider must be != 0");

    libdivide_uint_t<T, false> result;
    uint8_t floor_log_2_d = static_cast<uint8_t>(nd - 1 - clz<T>(d));

    // Power of 2
    if (is_zero_or_single_bit(d)) {
        // We need to subtract 1 from the shift value in case of an unsigned
        // branchfree div1by1_divider because there is a hardcoded right shift by 1
        // in its division algorithm. Because of this we also need to add back
        // 1 in its recovery algorithm.
        result.magic = 0;
        result.more = (uint8_t)(floor_log_2_d - (branchfree != 0));
    } else {
        uint8_t more;
        T rem, proposed_m;
        proposed_m = libdivide_div_half<T>(static_cast<T>(1) << floor_log_2_d, 0, d, rem);

        WJR_ASSERT(rem > 0 && rem < d);
        const T e = d - rem;

        // This power works if e < 2**floor_log_2_d.
        if (!branchfree && (e < (static_cast<T>(1) << floor_log_2_d))) {
            // This power works
            more = floor_log_2_d;
        } else {
            // We have to use the general 17-bit algorithm.  We need to compute
            // (2**power) / d. However, we already have (2**(power-1))/d and
            // its remainder.  By doubling both, and then correcting the
            // remainder, we can compute the larger division.
            // don't care about overflow here - in fact, we expect it
            proposed_m += proposed_m;
            const T twice_rem = rem + rem;
            if (twice_rem >= d || twice_rem < rem)
                proposed_m += 1;
            more = floor_log_2_d | LIBDIVIDE_ADD_MARKER;
        }
        result.magic = 1 + proposed_m;
        result.more = more;
        // result.more's shift should in general be ceil_log_2_d. But if we
        // used the smaller power, we subtract one from the shift because we're
        // using the smaller power. If we're using the larger power, we
        // subtract one from the shift because it's taken care of by the add
        // indicator. So floor_log_2_d happens to be correct in both cases.
    }

    return result;
}

template <typename T>
struct libdivide_shift_mask {
    static const uint8_t value = 0;
};

#define WJR_REGISTER_SHIFT_MASK(nd)                                                      \
    template <>                                                                          \
    struct libdivide_shift_mask<uint##nd##_t> {                                          \
        static const uint8_t value = LIBDIVIDE_##nd##_SHIFT_MASK;                        \
    }

WJR_REGISTER_SHIFT_MASK(16);
WJR_REGISTER_SHIFT_MASK(32);
WJR_REGISTER_SHIFT_MASK(64);

#undef WJR_REGISTER_SHIFT_MASK

template <typename T, bool branchfree>
WJR_CONST WJR_INTRINSIC_INLINE libdivide_uint_t<T, branchfree>
libdivide_uint_gen(T d) noexcept {
    if constexpr (!branchfree) {
        return libdivide_internal_uint_gen<T, false>(d);
    } else {
        WJR_ASSERT(d != 1, "div1by1_divider must be != 1");
        const auto tmp = libdivide_internal_uint_gen<T, true>(d);
        libdivide_uint_t<T, true> ret = {
            tmp.magic, (uint8_t)(tmp.more & libdivide_shift_mask<T>::value)};
        return ret;
    }
}

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE T
libdivide_uint_do_impl(T d, const libdivide_uint_t<T, false> &denom) noexcept {
    const T magic = denom.magic;
    const uint8_t more = denom.more;
    if (!magic) {
        return d >> more;
    } else {
        T q = mulhi<T>(magic, d);
        if (more & LIBDIVIDE_ADD_MARKER) {
            T t = ((d - q) >> 1) + q;
            return t >> (more & libdivide_shift_mask<T>::value);
        } else {
            // All upper bits are 0,
            // don't need to mask them off.
            return q >> more;
        }
    }
}

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE T
libdivide_uint_branchfree_do_impl(T d, const libdivide_uint_t<T, true> &denom) noexcept {
    const T q = mulhi<T>(denom.magic, d);
    const T t = ((d - q) >> 1) + q;
    return t >> denom.more;
}

template <typename T, bool branchfree>
WJR_CONST WJR_INTRINSIC_INLINE T
libdivide_uint_do(T d, const libdivide_uint_t<T, branchfree> &denom) noexcept {
    if constexpr (!branchfree) {
        return libdivide_uint_do_impl(d, denom);
    } else {
        return libdivide_uint_branchfree_do_impl(d, denom);
    }
}

/////////// C++ stuff

// This is the main div1by1_divider class for use by the user (C++ API).
// The actual division algorithm is selected using the dispatcher struct
// based on the integer width and algorithm template parameters.
template <typename T, bool branchfree = false>
class div1by1_divider {
private:
    using dispatcher_t = libdivide_uint_t<T, branchfree>;

public:
    // We leave the default constructor empty so that creating
    // an array of dividers and then initializing them
    // later doesn't slow us down.
    constexpr div1by1_divider() = default;
    constexpr div1by1_divider(const div1by1_divider &) = default;
    constexpr div1by1_divider(div1by1_divider &&) = default;
    constexpr div1by1_divider &operator=(const div1by1_divider &) = default;
    constexpr div1by1_divider &operator=(div1by1_divider &&) = default;
    ~div1by1_divider() = default;

    // Constructor that takes the divisor as a parameter
    WJR_INTRINSIC_INLINE div1by1_divider(T d) {
        div = libdivide_uint_gen<T, branchfree>(d);
    }

    // Divides n by the divisor
    WJR_INTRINSIC_INLINE T divide(T n) const { return libdivide_uint_do(n, div); }

    WJR_CONST bool operator==(const div1by1_divider<T, branchfree> &other) const {
        return div.denom.magic == other.denom.magic && div.denom.more == other.denom.more;
    }

    WJR_CONST bool operator!=(const div1by1_divider<T, branchfree> &other) const {
        return !(*this == other);
    }

private:
    // Storage for the actual divisor
    dispatcher_t div;
};

// Overload of operator / for scalar division
template <typename T, bool branchfree>
WJR_INTRINSIC_INLINE T operator/(T n, const div1by1_divider<T, branchfree> &div) {
    return div.divide(n);
}

// Overload of operator /= for scalar division
template <typename T, bool branchfree>
WJR_INTRINSIC_INLINE T &operator/=(T &n, const div1by1_divider<T, branchfree> &div) {
    n = div.divide(n);
    return n;
}

template <typename T>
using branchfree_divider = div1by1_divider<T, true>;

} // namespace wjr

#endif // WJR_LIBDIVIDE_HPP__
