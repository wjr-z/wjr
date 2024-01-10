#ifndef WJR_MATH_DIVIDER_HPP__
#define WJR_MATH_DIVIDER_HPP__

#include <wjr/math/mul.hpp>
#include <wjr/math/shift.hpp>

namespace wjr {

// reference : https://ieeexplore.ieee.org/document/5487506
template <typename T>
class div2by1_divider {
public:
    static_assert(std::is_same_v<T, uint64_t>, "Currently only support uint64_t");

    constexpr div2by1_divider() = default;
    WJR_INTRINSIC_CONSTEXPR_E explicit div2by1_divider(T _divisor) : m_divisor(_divisor) {
        initialize();
    }
    constexpr div2by1_divider(T _divisor, T _value, T _shift)
        : m_divisor(_divisor), m_value(_value), m_shift(_shift) {}
    div2by1_divider(const div2by1_divider &) = default;
    div2by1_divider &operator=(const div2by1_divider &) = default;
    ~div2by1_divider() = default;

    constexpr T divisor() const { return m_divisor; }
    constexpr T value() const { return m_value; }
    constexpr T shift() const { return m_shift; }

    constexpr bool is_power_of_two() const { return m_divisor == (1ull << 63); }

private:
    // make sure m_shift/one_single_bit(divisor) can be inlined
    WJR_INTRINSIC_CONSTEXPR_E void initialize() {
        if (!(m_divisor >> 63)) {
            m_shift = clz(m_divisor);
        }

        m_divisor <<= m_shift;

        if (WJR_UNLIKELY(m_divisor == (1ull << 63))) {
            m_value = -1;
            return;
        }

        return large_initialize();
    }

    WJR_INTRINSIC_CONSTEXPR_E void large_initialize() {
        uint64_t d = m_divisor;
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

        m_value = v4;
    }

    T m_divisor = 0;
    // m_value = floor((B ^ 2 - 1) / m_divisor) - B
    T m_value = 0;
    unsigned int m_shift = 0;
};

} // namespace wjr

#endif // WJR_MATH_DIVIDER_HPP__