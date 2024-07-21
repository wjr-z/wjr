#ifndef WJR_X86_MATH_SET_HPP__
#define WJR_X86_MATH_SET_HPP__

#include <cstring>

#include <wjr/x86/simd/simd.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE2)
#define WJR_HAS_BUILTIN_SET_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(SET_N)

template <typename T>
WJR_COLD void large_builtin_set_n(T *dst, T val, size_t n) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;
    constexpr auto is_avx = WJR_HAS_SIMD(AVX2);

    using simd = std::conditional_t<is_avx, avx, sse>;
    using simd_int = typename simd::int_type;
    constexpr auto simd_width = simd::width();
    constexpr auto type_width = simd_width / nd;
    constexpr auto u8_width = simd_width / 8;
    constexpr auto mask = u8_width * 4;

    WJR_ASSUME(n > type_width * 4);

    auto y = simd::set1(val, T());

    simd::storeu(dst, y);
    simd::storeu(dst + n - type_width, y);
    simd::storeu(dst + type_width, y);
    simd::storeu(dst + n - type_width * 2, y);
    simd::storeu(dst + type_width * 2, y);
    simd::storeu(dst + n - type_width * 3, y);
    simd::storeu(dst + type_width * 3, y);
    simd::storeu(dst + n - type_width * 4, y);

    uintptr_t ps = reinterpret_cast<uintptr_t>(dst);
    uintptr_t pe = reinterpret_cast<uintptr_t>(dst + n);

    ps += mask;
    ps &= -mask;
    pe &= -mask;

    if (WJR_UNLIKELY(ps == pe)) {
        return;
    }

    const uintptr_t mo = reinterpret_cast<uintptr_t>(dst) % sizeof(T);

    if (WJR_UNLIKELY(mo != 0)) {
        T stk[2] = {val, val};
        std::memcpy(&val, (char *)(stk) + mo, sizeof(T));
        y = simd::set1(val, T());
    }

    do {
        simd::store((simd_int *)(ps), y);
        simd::store((simd_int *)(ps + u8_width * 1), y);
        simd::store((simd_int *)(ps + u8_width * 2), y);
        simd::store((simd_int *)(ps + u8_width * 3), y);

        ps += u8_width * 4;
    } while (WJR_LIKELY(ps != pe));
    return;
}

template <typename T>
WJR_INTRINSIC_INLINE void builtin_set_n(T *dst, T val, size_t n) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;
    constexpr auto is_avx = WJR_HAS_SIMD(AVX2);

    using simd = std::conditional_t<is_avx, avx, sse>;
    constexpr auto simd_width = simd::width();
    constexpr auto type_width = simd_width / nd;

    constexpr auto sse_width = sse::width();
    constexpr auto sse_loop = sse_width / nd;

    if (WJR_UNLIKELY(n == 0)) {
        return;
    }

    if (WJR_BUILTIN_CONSTANT_P(n) && n <= 4) {
        for (size_t i = 0; i < n; ++i) {
            dst[i] = val;
        }

        return;
    }

    if (WJR_UNLIKELY(n > type_width * 2)) {

        if (WJR_UNLIKELY(n > type_width * 4)) {
            return large_builtin_set_n(dst, val, n);
        }

        auto y = simd::set1(val, T());

        simd::storeu(dst, y);
        simd::storeu(dst + type_width, y);
        simd::storeu(dst + n - type_width, y);
        simd::storeu(dst + n - type_width * 2, y);
        return;
    }

    const auto x = broadcast<T, uint64_t>(val);
    const auto y = broadcast<uint64_t, __m128i_t>(x);

    if (WJR_UNLIKELY(n <= sse_loop * 2)) {
        if (WJR_UNLIKELY(n >= sse_loop)) {
            sse::storeu(dst, y);
            sse::storeu(dst + n - sse_loop, y);
            return;
        }

        if constexpr (sse_loop == 2) {
            std::memcpy(dst, &x, 8);
            return;
        } else {
            if (WJR_UNLIKELY(n >= sse_loop / 2)) {
                std::memcpy(dst, &x, 8);
                std::memcpy(dst + n - sse_loop / 2, &x, 8);
                return;
            }
        }

        if constexpr (sse_loop >= 4) {
            if constexpr (sse_loop == 4) {
                std::memcpy(dst, &x, 4);
                return;
            } else {
                if (WJR_UNLIKELY(n >= sse_loop / 4)) {
                    std::memcpy(dst, &x, 4);
                    std::memcpy(dst + n - sse_loop / 4, &x, 4);
                    return;
                }
            }
        }

        if constexpr (sse_loop >= 8) {
            if constexpr (sse_loop == 8) {
                std::memcpy(dst, &x, 2);
                return;
            } else {
                if (WJR_UNLIKELY(n >= sse_loop / 8)) {
                    std::memcpy(dst, &x, 2);
                    std::memcpy(dst + n - sse_loop / 8, &x, 2);
                    return;
                }
            }
        }

        if constexpr (sse_loop >= 16) {
            if constexpr (sse_loop == 16) {
                std::memcpy(dst, &x, 1);
                return;
            } else {
                if (WJR_UNLIKELY(n >= sse_loop / 16)) {
                    std::memcpy(dst, &x, 16);
                    std::memcpy(dst + n - sse_loop / 16, &x, 16);
                    return;
                }
            }
        }

        return;
    }

#if WJR_HAS_SIMD(AVX2)
    if constexpr (is_avx) {
        auto z = broadcast<__m128i_t, __m256i_t>(y);
        avx::storeu(dst, z);
        avx::storeu(dst + n - type_width, z);
        return;
    }
#endif

    WJR_UNREACHABLE();
}

#endif

} // namespace wjr

#endif // WJR_X86_MATH_SET_HPP__