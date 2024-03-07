#ifndef WJR_MATH_DIVIDER_HPP__
#define WJR_MATH_DIVIDER_HPP__

#include <wjr/math/mul.hpp>

#if defined(WJR_X86)
#include <wjr/x86/divider.hpp>
#endif

namespace wjr {

namespace math_details {

inline constexpr std::array<uint16_t, 0x100> div2by1_u64_lookup = {
    0x7fd, 0x7f5, 0x7ed, 0x7e5, 0x7dd, 0x7d5, 0x7ce, 0x7c6, 0x7bf, 0x7b7, 0x7b0, 0x7a8,
    0x7a1, 0x79a, 0x792, 0x78b, 0x784, 0x77d, 0x776, 0x76f, 0x768, 0x761, 0x75b, 0x754,
    0x74d, 0x747, 0x740, 0x739, 0x733, 0x72c, 0x726, 0x720, 0x719, 0x713, 0x70d, 0x707,
    0x700, 0x6fa, 0x6f4, 0x6ee, 0x6e8, 0x6e2, 0x6dc, 0x6d6, 0x6d1, 0x6cb, 0x6c5, 0x6bf,
    0x6ba, 0x6b4, 0x6ae, 0x6a9, 0x6a3, 0x69e, 0x698, 0x693, 0x68d, 0x688, 0x683, 0x67d,
    0x678, 0x673, 0x66e, 0x669, 0x664, 0x65e, 0x659, 0x654, 0x64f, 0x64a, 0x645, 0x640,
    0x63c, 0x637, 0x632, 0x62d, 0x628, 0x624, 0x61f, 0x61a, 0x616, 0x611, 0x60c, 0x608,
    0x603, 0x5ff, 0x5fa, 0x5f6, 0x5f1, 0x5ed, 0x5e9, 0x5e4, 0x5e0, 0x5dc, 0x5d7, 0x5d3,
    0x5cf, 0x5cb, 0x5c6, 0x5c2, 0x5be, 0x5ba, 0x5b6, 0x5b2, 0x5ae, 0x5aa, 0x5a6, 0x5a2,
    0x59e, 0x59a, 0x596, 0x592, 0x58e, 0x58a, 0x586, 0x583, 0x57f, 0x57b, 0x577, 0x574,
    0x570, 0x56c, 0x568, 0x565, 0x561, 0x55e, 0x55a, 0x556, 0x553, 0x54f, 0x54c, 0x548,
    0x545, 0x541, 0x53e, 0x53a, 0x537, 0x534, 0x530, 0x52d, 0x52a, 0x526, 0x523, 0x520,
    0x51c, 0x519, 0x516, 0x513, 0x50f, 0x50c, 0x509, 0x506, 0x503, 0x500, 0x4fc, 0x4f9,
    0x4f6, 0x4f3, 0x4f0, 0x4ed, 0x4ea, 0x4e7, 0x4e4, 0x4e1, 0x4de, 0x4db, 0x4d8, 0x4d5,
    0x4d2, 0x4cf, 0x4cc, 0x4ca, 0x4c7, 0x4c4, 0x4c1, 0x4be, 0x4bb, 0x4b9, 0x4b6, 0x4b3,
    0x4b0, 0x4ad, 0x4ab, 0x4a8, 0x4a5, 0x4a3, 0x4a0, 0x49d, 0x49b, 0x498, 0x495, 0x493,
    0x490, 0x48d, 0x48b, 0x488, 0x486, 0x483, 0x481, 0x47e, 0x47c, 0x479, 0x477, 0x474,
    0x472, 0x46f, 0x46d, 0x46a, 0x468, 0x465, 0x463, 0x461, 0x45e, 0x45c, 0x459, 0x457,
    0x455, 0x452, 0x450, 0x44e, 0x44b, 0x449, 0x447, 0x444, 0x442, 0x440, 0x43e, 0x43b,
    0x439, 0x437, 0x435, 0x432, 0x430, 0x42e, 0x42c, 0x42a, 0x428, 0x425, 0x423, 0x421,
    0x41f, 0x41d, 0x41b, 0x419, 0x417, 0x414, 0x412, 0x410, 0x40e, 0x40c, 0x40a, 0x408,
    0x406, 0x404, 0x402, 0x400};

// invert of (2 * i + 1) mod 256
inline constexpr std::array<uint64_t, 0x80> divexact1_lookup = {
    0x01, 0xAB, 0xCD, 0xB7, 0x39, 0xA3, 0xC5, 0xEF, 0xF1, 0x1B, 0x3D, 0xA7, 0x29,
    0x13, 0x35, 0xDF, 0xE1, 0x8B, 0xAD, 0x97, 0x19, 0x83, 0xA5, 0xCF, 0xD1, 0xFB,
    0x1D, 0x87, 0x09, 0xF3, 0x15, 0xBF, 0xC1, 0x6B, 0x8D, 0x77, 0xF9, 0x63, 0x85,
    0xAF, 0xB1, 0xDB, 0xFD, 0x67, 0xE9, 0xD3, 0xF5, 0x9F, 0xA1, 0x4B, 0x6D, 0x57,
    0xD9, 0x43, 0x65, 0x8F, 0x91, 0xBB, 0xDD, 0x47, 0xC9, 0xB3, 0xD5, 0x7F, 0x81,
    0x2B, 0x4D, 0x37, 0xB9, 0x23, 0x45, 0x6F, 0x71, 0x9B, 0xBD, 0x27, 0xA9, 0x93,
    0xB5, 0x5F, 0x61, 0x0B, 0x2D, 0x17, 0x99, 0x03, 0x25, 0x4F, 0x51, 0x7B, 0x9D,
    0x07, 0x89, 0x73, 0x95, 0x3F, 0x41, 0xEB, 0x0D, 0xF7, 0x79, 0xE3, 0x05, 0x2F,
    0x31, 0x5B, 0x7D, 0xE7, 0x69, 0x53, 0x75, 0x1F, 0x21, 0xCB, 0xED, 0xD7, 0x59,
    0xC3, 0xE5, 0x0F, 0x11, 0x3B, 0x5D, 0xC7, 0x49, 0x33, 0x55, 0xFF};

} // namespace math_details

template <typename T>
class div2by1_divider {
public:
    static_assert(std::is_same_v<T, uint64_t>, "Currently only support uint64_t");

    div2by1_divider() = default;
    div2by1_divider(const div2by1_divider &) = default;
    div2by1_divider &operator=(const div2by1_divider &) = default;
    ~div2by1_divider() = default;

    WJR_INTRINSIC_CONSTEXPR_E explicit div2by1_divider(T _divisor) : m_divisor(_divisor) {
        initialize();
    }

    constexpr div2by1_divider(T _divisor, T _value, unsigned int _shift)
        : m_divisor(_divisor), m_value(_value), m_shift(_shift) {}

    constexpr T get_divisor() const { return m_divisor; }
    constexpr T get_value() const { return m_value; }
    constexpr unsigned int get_shift() const { return m_shift; }

    constexpr bool is_zero_or_single_bit() const { return m_divisor == (1ull << 63); }

    WJR_INTRINSIC_CONSTEXPR20 static T divide(T divisor, T value, T lo, T &hi) {
        WJR_ASSERT_ASSUME(__has_high_bit(divisor));

        if (WJR_BUILTIN_CONSTANT_P(lo == 0) && lo == 0) {
            return divide_lo0(divisor, value, lo, hi);
        }

        return basic_divide(divisor, value, lo, hi);
    }

    WJR_CONST WJR_CONSTEXPR_E static T reciprocal(T d) {
        WJR_ASSERT_ASSUME(__has_high_bit(d));

        uint64_t d40 = 0, d63 = 0;
        uint32_t v0 = 0;
        uint64_t v1 = 0, v2 = 0, v3 = 0, v4 = 0;
        uint64_t t0 = 0, t1 = 0;

        // 40 bit
        d40 = (d >> 24) + 1;
        // 63 bit
        d63 = (d + 1) >> 1;
        // 11 bit
        v0 = math_details::div2by1_u64_lookup[((d >> 55) - 0x100)];
        // 22 bit
        v1 = (v0 << 11) - (mullo<uint64_t>(mullo<uint32_t>(v0, v0), d40) >> 40) - 1;

        t1 = mulhi<uint64_t>(v1 << 17, (1ull << 60) - mullo<uint64_t>(v1, d40));
        // 35 bit
        v2 = (v1 << 13) + t1;

        t0 = 0 - mul<uint64_t>(v2, d63, t1);
        if (d & 1) {
            t0 += v2 >> 1;
        }

        v3 = (v2 << 31) + (mulhi<uint64_t>(t0, v2) >> 1);

        if (v3 == -1ull) {
            v4 = v3 - d * 2;
        } else {
            v4 = mulhi<uint64_t>(v3 + 1, d);
            v4 += d;
            v4 = v3 - v4;
        }

        return v4;
    }

private:
    // make sure m_shift/one_single_bit(divisor) can be inlined
    WJR_INTRINSIC_CONSTEXPR_E void initialize() {
        if (WJR_UNLIKELY(!__has_high_bit(m_divisor))) {
            m_shift = clz(m_divisor);
            m_divisor <<= m_shift;

            WJR_ASSUME(m_shift != 0);
        } else {
            WJR_ASSUME(m_shift == 0);
        }

        WJR_ASSUME(__has_high_bit(m_divisor));

        if (WJR_UNLIKELY(m_divisor == (1ull << 63))) {
            m_value = -1;
            return;
        }

        m_value = reciprocal(m_divisor);
    }

    WJR_INTRINSIC_CONSTEXPR static void fallback_div2by1_adjust(T rax, T div, T &r8,
                                                                T &rdx) {
        T r9 = r8 + div;
        bool f = r8 < rax;
        r8 = f ? r8 : r9;
        rdx += -1 + f;
    }

    // see fallback_div2by1_adjust
    WJR_INTRINSIC_CONSTEXPR20 static void div2by1_adjust(T rax, T div, T &r8, T &rdx) {
#if WJR_HAS_BUILTIN(ASM_DIV2BY1_ADJUST)
        if (is_constant_evaluated()) {
            return fallback_div2by1_adjust(rax, div, r8, rdx);
        }

        return asm_div2by1_adjust(rax, div, r8, rdx);
#else
        return fallback_div2by1_adjust(rax, div, r8, rdx);
#endif
    }

    WJR_INTRINSIC_CONSTEXPR20 static T basic_divide(T divisor, T value, T lo, T &hi) {
        T hi1 = hi + 1;

        T rax, rdx;

        rax = mul(hi, value, rdx);
        __addc_128(rax, rdx, rax, rdx, lo, hi1);

        lo -= mullo(rdx, divisor);

        div2by1_adjust(rax, divisor, lo, rdx);

        if (WJR_UNLIKELY(lo >= divisor)) {
            WJR_FORCE_BRANCH_BARRIER();
            lo -= divisor;
            ++rdx;
        }

        hi = lo;
        return rdx;
    }

    WJR_INTRINSIC_CONSTEXPR20 static T divide_lo0(T divisor, T value, T lo, T &hi) {
        WJR_ASSERT(lo == 0);

        T hi1 = hi + 1;

        T rax, rdx;

        rax = mul(hi, value, rdx);
        rdx += hi1;

        lo -= mullo(rdx, divisor);

        div2by1_adjust(rax, divisor, lo, rdx);

        hi = lo;
        return rdx;
    }

    T m_divisor = 0;
    // m_value = floor((B ^ 2 - 1) / m_divisor) - B
    T m_value = 0;
    unsigned int m_shift = 0;
};

template <typename T>
class div3by2_divider {
public:
    static_assert(std::is_same_v<T, uint64_t>, "");

    div3by2_divider() = default;
    div3by2_divider(const div3by2_divider &) = default;
    div3by2_divider &operator=(const div3by2_divider &) = default;
    ~div3by2_divider() = default;

    WJR_INTRINSIC_CONSTEXPR_E div3by2_divider(T d0, T d1)
        : m_divisor0(d0), m_divisor1(d1) {
        initialize();
    }

    WJR_INTRINSIC_CONSTEXPR div3by2_divider(T d0, T d1, T value, unsigned int shift)
        : m_divisor0(d0), m_divisor1(d1), m_value(value), m_shift(shift) {}

    constexpr T get_divisor0() const { return m_divisor0; }
    constexpr T get_divisor1() const { return m_divisor1; }
    constexpr T get_value() const { return m_value; }
    constexpr unsigned int get_shift() const { return m_shift; }

    WJR_INTRINSIC_CONSTEXPR20 static T divide(T divisor0, T divisor1, T value, T u0,
                                              T &u1, T &u2) {
        WJR_ASSERT_ASSUME(__has_high_bit(divisor1));

        T q1, q0;
        q0 = mul<T>(value, u2, q1);
        __addc_128(q0, q1, q0, q1, u1, u2);

        T r1, r0;
        r1 = u1 - mullo<T>(q1, divisor1);
        T t1;
        r0 = mul<T>(divisor0, q1, t1);

        __subc_128(r0, r1, u0, r1, r0, t1);
        __subc_128(r0, r1, r0, r1, divisor0, divisor1);
        ++q1;

        if (r1 >= q0) {
            --q1;
            __addc_128(r0, r1, r0, r1, divisor0, divisor1);
        }

        if (WJR_UNLIKELY(__less_equal_128(divisor0, divisor1, r0, r1))) {
            ++q1;
            __subc_128(r0, r1, r0, r1, divisor0, divisor1);
        }

        u1 = r0;
        u2 = r1;
        return q1;
    }

    WJR_CONST WJR_CONSTEXPR_E static T reciprocal(T d0, T d1) {
        WJR_ASSERT_ASSUME(__has_high_bit(d1));

        T v = div2by1_divider<T>::reciprocal(d1);
        T p = mullo<T>(d1, v);
        p += d0;
        if (p < d0) {
            --v;
            if (p >= d1) {
                --v;
                p -= d1;
            }
            p -= d1;
        }

        T t0 = 0, t1 = 0;
        t0 = mul<T>(d0, v, t1);
        p += t1;
        if (p < t1) {
            --v;
            if (__less_equal_128(d0, d1, t0, p)) {
                --v;
            }
        }
        
        return v;
    }

private:
    WJR_INTRINSIC_CONSTEXPR_E void initialize() {
        if (WJR_UNLIKELY(!__has_high_bit(m_divisor1))) {
            m_shift = clz(m_divisor1);
            m_divisor1 = shld(m_divisor1, m_divisor0, m_shift);
            m_divisor0 <<= m_shift;

            WJR_ASSUME(m_shift != 0);
        } else {
            WJR_ASSUME(m_shift == 0);
        }

        WJR_ASSUME(__has_high_bit(m_divisor1));

        m_value = reciprocal(m_divisor0, m_divisor1);
    }

    WJR_INTRINSIC_CONSTEXPR static void fallback_div3by2_adjust(T d1, T &p, T &v) {
        T q = p - d1;
        T f = p < d1;
        p = f ? p : q;
        v += -1 + f;
    }

    WJR_INTRINSIC_CONSTEXPR20 static void div3by2_adjust(T rax, T div, T &r8, T &rdx) {
#if WJR_HAS_BUILTIN(ASM_DIV3BY2_ADJUST)
        if (is_constant_evaluated()) {
            return fallback_div3by2_adjust(rax, div, r8, rdx);
        }

        return asm_div3by2_adjust(rax, div, r8, rdx);
#else
        return fallback_div3by2_adjust(rax, div, r8, rdx);
#endif
    }

    T m_divisor0 = 0;
    T m_divisor1 = 0;
    T m_value = 0;
    unsigned int m_shift = 0;
};

// divexact1_divider
// m_value is invert of m_divisor mod 2 ^ N
// m_divisor is odd
template <typename T>
class divexact1_divider {
public:
    static_assert(std::is_same_v<T, uint64_t>, "Currently only support uint64_t");

    divexact1_divider() = default;
    divexact1_divider(const divexact1_divider &) = default;
    divexact1_divider &operator=(const divexact1_divider &) = default;
    ~divexact1_divider() = default;

    WJR_INTRINSIC_CONSTEXPR_E explicit divexact1_divider(T divisor) : m_divisor(divisor) {
        initialize();
    }

    constexpr divexact1_divider(T divisor, T value, unsigned int shift)
        : m_divisor(divisor), m_value(value), m_shift(shift) {}

    constexpr T get_divisor() const { return m_divisor; }
    constexpr T get_value() const { return m_value; }
    constexpr unsigned int get_shift() const { return m_shift; }

    constexpr bool is_zero_or_single_bit() const { return m_divisor == 1; }

    WJR_CONST constexpr static T reciprocal(T divisor) {
        T inv = math_details::divexact1_lookup[(divisor & 0xFF) >> 1];
        inv = inv * (2 - inv * divisor);
        inv = inv * (2 - inv * divisor);
        inv = inv * (2 - inv * divisor);
        return inv;
    }

private:
    WJR_INTRINSIC_CONSTEXPR_E void initialize() {

        if (WJR_UNLIKELY(!(m_divisor & 1))) {
            m_shift = ctz(m_divisor);
            m_divisor >>= m_shift;

            WJR_ASSUME(m_shift != 0);
        } else {
            WJR_ASSUME(m_shift == 0);
        }

        WJR_ASSUME((m_divisor & 1) != 0);

        m_value = reciprocal(m_divisor);
    }

    T m_divisor = 0;
    T m_value = 0;
    unsigned int m_shift = 0;
};

} // namespace wjr

#endif // WJR_MATH_DIVIDER_HPP__