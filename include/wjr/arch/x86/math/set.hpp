#ifndef WJR_ARCH_X86_MATH_SET_HPP__
#define WJR_ARCH_X86_MATH_SET_HPP__

#include <cstring>

#include <wjr/arch/x86/simd/simd.hpp>

namespace wjr {

#if WJR_HAS_SIMD(SSE2)
    #define WJR_HAS_BUILTIN_SET_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(SET_N)

template <typename simd, typename T>
void large_builtin_set_n(T *dst, typename simd::int_type y, size_t n) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;
    constexpr auto simd_width = simd::width();
    constexpr auto type_width = simd_width / nd;
    constexpr auto u8_width = simd_width / 8;
    constexpr auto mask = u8_width * 4;

    WJR_ASSUME(n > type_width * 4);

    simd::storeu(dst, y);
    simd::storeu(dst + n - type_width, y);
    simd::storeu(dst + type_width, y);
    simd::storeu(dst + n - type_width * 2, y);
    simd::storeu(dst + type_width * 2, y);
    simd::storeu(dst + n - type_width * 3, y);
    simd::storeu(dst + type_width * 3, y);
    simd::storeu(dst + n - type_width * 4, y);

    const uintptr_t updst = reinterpret_cast<uintptr_t>(dst);
    T *ps;
    T *pe;

    {
        uintptr_t ups = updst;
        uintptr_t upe = reinterpret_cast<uintptr_t>(dst + n);

        ups += mask;
        ups &= -mask;
        upe &= -mask;

        ps = reinterpret_cast<T *>(ups);
        pe = reinterpret_cast<T *>(upe);
    }

    if (WJR_UNLIKELY(ps == pe)) {
        return;
    }

    if (const uintptr_t mo = updst % sizeof(T); mo != 0) {
        y = simd::Or(
            simd::srl(y, simd_cast<uint8_t, __m128i_t>(mo * 8), T()),
            simd::sll(y, simd_cast<uint8_t, __m128i_t>(std::numeric_limits<T>::digits - mo * 8),
                      T()));
    }

    do {
        simd::store(ps, y);
        simd::store(ps + type_width, y);
        simd::store(ps + type_width * 2, y);
        simd::store(ps + type_width * 3, y);

        ps += type_width * 4;
    } while (WJR_LIKELY(ps != pe));
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

    if (WJR_BUILTIN_CONSTANT_P_TRUE(n <= 4)) {
        for (size_t i = 0; i < n; ++i) {
            dst[i] = val;
        }

        return;
    }

    if (WJR_UNLIKELY(n == 0)) {
        return;
    }

    if (WJR_LIKELY(n >= type_width)) {
        const auto y = simd::set1(val, T());
        if (WJR_UNLIKELY(n > type_width * 2)) {
            if (WJR_UNLIKELY(n > type_width * 4)) {
                return large_builtin_set_n<simd>(dst, y, n);
            }

            simd::storeu(dst, y);
            simd::storeu(dst + type_width, y);
            simd::storeu(dst + n - type_width, y);
            simd::storeu(dst + n - type_width * 2, y);
            return;
        }

        simd::storeu(dst, y);
        simd::storeu(dst + n - type_width, y);
        return;
    }

    const auto y = sse::set1(val, T());
    const auto x = simd_cast<typename sse::int_tag_type, uint64_t>(y);

    if constexpr (is_avx) {
        if (WJR_UNLIKELY(n >= sse_loop)) {
            sse::storeu(dst, y);
            sse::storeu(dst + n - sse_loop, y);
            return;
        }
    }

    if constexpr (sse_loop == 2) {
        builtin_memcpy(dst, &x, 8);
        return;
    } else {
        if (WJR_UNLIKELY(n >= sse_loop / 2)) {
            builtin_memcpy(dst, &x, 8);
            builtin_memcpy(dst + n - sse_loop / 2, &x, 8);
            return;
        }
    }

    if (WJR_UNLIKELY(n >= sse_loop)) {
        sse::storeu(dst, y);
        sse::storeu(dst + n - sse_loop, y);
        return;
    }

    if constexpr (sse_loop == 2) {
        builtin_memcpy(dst, &x, 8);
        return;
    } else {
        if (WJR_UNLIKELY(n >= sse_loop / 2)) {
            builtin_memcpy(dst, &x, 8);
            builtin_memcpy(dst + n - sse_loop / 2, &x, 8);
            return;
        }
    }

    if constexpr (sse_loop >= 4) {
        if constexpr (sse_loop == 4) {
            builtin_memcpy(dst, &x, 4);
            return;
        } else {
            if (WJR_UNLIKELY(n >= sse_loop / 4)) {
                builtin_memcpy(dst, &x, 4);
                builtin_memcpy(dst + n - sse_loop / 4, &x, 4);
                return;
            }
        }
    }

    if constexpr (sse_loop >= 8) {
        if constexpr (sse_loop == 8) {
            builtin_memcpy(dst, &x, 2);
            return;
        } else {
            if (WJR_UNLIKELY(n >= sse_loop / 8)) {
                builtin_memcpy(dst, &x, 2);
                builtin_memcpy(dst + n - sse_loop / 8, &x, 2);
                return;
            }
        }
    }

    if constexpr (sse_loop >= 16) {
        if constexpr (sse_loop == 16) {
            builtin_memcpy(dst, &x, 1);
            return;
        } else {
            if (WJR_UNLIKELY(n >= sse_loop / 16)) {
                builtin_memcpy(dst, &x, 16);
                builtin_memcpy(dst + n - sse_loop / 16, &x, 16);
                return;
            }
        }
    }
}

#endif

} // namespace wjr

#endif // WJR_ARCH_X86_MATH_SET_HPP__