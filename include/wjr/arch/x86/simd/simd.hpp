#ifndef WJR_ARCH_X86_SIMD_SIMD_HPP__
#define WJR_ARCH_X86_SIMD_SIMD_HPP__

#include <wjr/arch/x86/simd/avx.hpp>

namespace wjr {

#define WJR_REGISTER_X86_NORMAL_SIMD_FUNCTION(N, UNROLL2, UNROLL4, IS_UNROLL_8, ADVANCE, INIT,     \
                                              RET)                                                 \
    if (WJR_UNLIKELY(N <= 16)) {                                                                   \
        if (WJR_UNLIKELY(N <= 4)) {                                                                \
            UNROLL2(N - 2);                                                                        \
            return RET(N);                                                                         \
        }                                                                                          \
                                                                                                   \
        UNROLL2(2);                                                                                \
                                                                                                   \
        if (WJR_LIKELY(N > 8)) {                                                                   \
            UNROLL4(4);                                                                            \
                                                                                                   \
            if (N > 12) {                                                                          \
                UNROLL4(8);                                                                        \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        UNROLL4(N - 4);                                                                            \
        return RET(N);                                                                             \
    }                                                                                              \
                                                                                                   \
    N -= 3;                                                                                        \
    const size_t __rem = N & 7;                                                                    \
    N &= ~7;                                                                                       \
                                                                                                   \
    if (WJR_LIKELY(__rem >= 2)) {                                                                  \
        UNROLL4(2);                                                                                \
        UNROLL4(__rem - 1);                                                                        \
    } else {                                                                                       \
        UNROLL2(__rem + 1);                                                                        \
    }                                                                                              \
                                                                                                   \
    INIT;                                                                                          \
    WJR_PP_BOOL_IF(                                                                                \
        IS_UNROLL_8,                                                                               \
        if (N & 8) {                                                                               \
            UNROLL4(__rem + 3);                                                                    \
            UNROLL4(__rem + 7);                                                                    \
                                                                                                   \
            if (WJR_UNLIKELY(N == 8)) {                                                            \
                return RET(N + __rem + 3);                                                         \
            }                                                                                      \
                                                                                                   \
            ADVANCE(__rem + 11);                                                                   \
            N -= 8;                                                                                \
        } else {, ) \
            ADVANCE(__rem + 3);                                                                    \
    WJR_PP_BOOL_IF(IS_UNROLL_8,                                                                    \
        }, )

#define WJR_REGISTER_X86_NORMAL_REVERSE_SIMD_FUNCTION(N, UNROLL2, UNROLL4, IS_UNROLL_8, ADVANCE,   \
                                                      INIT, RET)                                   \
    if (WJR_UNLIKELY(N <= 16)) {                                                                   \
        if (WJR_UNLIKELY(N <= 4)) {                                                                \
            UNROLL2(0);                                                                            \
            return RET(0);                                                                         \
        }                                                                                          \
                                                                                                   \
        UNROLL2(N - 4);                                                                            \
                                                                                                   \
        if (WJR_LIKELY(N > 8)) {                                                                   \
            UNROLL4(N - 8);                                                                        \
                                                                                                   \
            if (N > 12) {                                                                          \
                UNROLL4(N - 12);                                                                   \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        UNROLL4(0);                                                                                \
        return RET(0);                                                                             \
    }                                                                                              \
                                                                                                   \
    N -= 3;                                                                                        \
    const size_t __rem = N & 7;                                                                    \
    N &= ~7;                                                                                       \
                                                                                                   \
    if (WJR_LIKELY(__rem >= 2)) {                                                                  \
        UNROLL4(N + __rem - 3);                                                                    \
        UNROLL4(N);                                                                                \
    } else {                                                                                       \
        UNROLL2(N);                                                                                \
    }                                                                                              \
                                                                                                   \
    INIT;                                                                                          \
    WJR_PP_BOOL_IF(                                                                                \
        IS_UNROLL_8,                                                                               \
        if (N & 8) {                                                                               \
            UNROLL4(N - 4);                                                                        \
            UNROLL4(N - 8);                                                                        \
                                                                                                   \
            if (WJR_UNLIKELY(N == 8)) {                                                            \
                return RET(0);                                                                     \
            }                                                                                      \
                                                                                                   \
            ADVANCE(N - 8);                                                                        \
            N -= 8;                                                                                \
        } else {, ) \
            ADVANCE(N);                                                                            \
    WJR_PP_BOOL_IF(IS_UNROLL_8,                                                                    \
        }, )

template <typename T, size_t N, typename Simd>
class __x86_simd_base {
    static constexpr size_t BitWidth = Simd::width();
    using int_type = typename Simd::int_type;
    using Mybase = fixed_size_simd<T, N>;

public:
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(__x86_simd_base);

    template <typename U, WJR_REQUIRES(is_value_preserving_or_int_v<U, T>)>
    __x86_simd_base(U value) noexcept : m_data(Simd::set1(value, U())) {}

    template <typename Flags = element_aligned_t>
    __x86_simd_base(const T *mem, Flags flags = {}) noexcept {
        copy_from(mem, flags);
    }

    void copy_from(const T *mem, element_aligned_t = {}) noexcept { m_data = Simd::loadu(mem); }

    void copy_from(const T *mem, vector_aligned_t) noexcept { m_data = Simd::load(mem); }

    void copy_to(T *mem, element_aligned_t = {}) noexcept { Simd::storeu(mem, m_data); }

    void copy_to(T *mem, vector_aligned_t) noexcept { Simd::store(mem, m_data); }

    Mybase &operator&=(const Mybase &other) noexcept {
        m_data = Simd::And(m_data, other.m_data);
        return static_cast<Mybase &>(*this);
    }

    friend Mybase operator&(const Mybase &lhs, const Mybase &rhs) noexcept {
        Mybase ret(lhs);
        ret &= rhs;
        return ret;
    }

    Mybase &operator|=(const Mybase &other) noexcept {
        m_data = Simd::Or(m_data, other.m_data);
        return static_cast<Mybase &>(*this);
    }

    friend Mybase operator|(const Mybase &lhs, const Mybase &rhs) noexcept {
        Mybase ret(lhs);
        ret |= rhs;
        return ret;
    }

    Mybase &operator^=(const Mybase &other) noexcept {
        m_data = Simd::Xor(m_data, other.m_data);
        return static_cast<Mybase &>(*this);
    }

    friend Mybase operator^(const Mybase &lhs, const Mybase &rhs) noexcept {
        Mybase ret(lhs);
        ret ^= rhs;
        return ret;
    }

private:
    int_type m_data;
};

#if WJR_HAS_SIMD(SSE2)
    #define WJR_HAS_SIMD_NATIVE_128BIT WJR_HAS_DEF

template <>
class simd<uint8_t, simd_abi::fixed_size<16>> : public __x86_simd_base<uint8_t, 16, sse> {
    using Mybase = __x86_simd_base<uint8_t, 16, sse>;

public:
    using Mybase::Mybase;
};

template <>
class simd<uint16_t, simd_abi::fixed_size<8>> : public __x86_simd_base<uint16_t, 8, sse> {
    using Mybase = __x86_simd_base<uint16_t, 8, sse>;

public:
    using Mybase::Mybase;
};

template <>
class simd<uint32_t, simd_abi::fixed_size<4>> : public __x86_simd_base<uint32_t, 4, sse> {
    using Mybase = __x86_simd_base<uint32_t, 4, sse>;

public:
    using Mybase::Mybase;
};

template <>
class simd<uint64_t, simd_abi::fixed_size<2>> : public __x86_simd_base<uint64_t, 2, sse> {
    using Mybase = __x86_simd_base<uint64_t, 2, sse>;

public:
    using Mybase::Mybase;
};

#endif

#if WJR_HAS_SIMD(AVX2)
    #define WJR_HAS_SIMD_NATIVE_256BIT WJR_HAS_DEF

template <>
class simd<uint8_t, simd_abi::fixed_size<32>> : public __x86_simd_base<uint8_t, 32, avx> {
    using Mybase = __x86_simd_base<uint8_t, 32, avx>;

public:
    using Mybase::Mybase;
};

template <>
class simd<uint16_t, simd_abi::fixed_size<16>> : public __x86_simd_base<uint16_t, 16, avx> {
    using Mybase = __x86_simd_base<uint16_t, 16, avx>;

public:
    using Mybase::Mybase;
};

template <>
class simd<uint32_t, simd_abi::fixed_size<8>> : public __x86_simd_base<uint32_t, 8, avx> {
    using Mybase = __x86_simd_base<uint32_t, 8, avx>;

public:
    using Mybase::Mybase;
};

template <>
class simd<uint64_t, simd_abi::fixed_size<4>> : public __x86_simd_base<uint64_t, 4, avx> {
    using Mybase = __x86_simd_base<uint64_t, 4, avx>;

public:
    using Mybase::Mybase;
};

#endif

} // namespace wjr

#endif // WJR_ARCH_X86_SIMD_SIMD_HPP__