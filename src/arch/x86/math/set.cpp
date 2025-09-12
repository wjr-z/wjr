/**
 * @file set.cpp
 * @author wjr
 * @brief large size memory set
 * @version 0.1
 * @date 2025-01-09
 *
 * @copyright Copyright (c) 2025
 *
 * @todo 1. Use streaming stores for large memory set \n
 * @todo 2. Threshold for different SIMD and platform
 *
 */

#include <wjr/arch/x86/cpuinfo.hpp>
#include <wjr/arch/x86/math/set.hpp>
#include <wjr/math/rotate.hpp>

namespace wjr {

// NOLINTBEGIN

#if WJR_HAS_BUILTIN(SET_N)

    #if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM) || defined(WJR_COMPILER_MSVC)
        #define WJR_HAS_BUILTIN_REP_STOS WJR_HAS_DEF
    #endif

    #if WJR_HAS_BUILTIN(REP_STOS)

WJR_INTRINSIC_INLINE void rep_stos(uint8_t *s, uint8_t val, size_t n) {
        #if defined(WJR_COMPILER_MSVC)
    __stosb(reinterpret_cast<unsigned char *>(s), val, n);
        #else
    asm volatile("rep stosb" : "+D"(s), "+c"(n) : "a"(val) : "memory");
        #endif
}

WJR_INTRINSIC_INLINE void rep_stos(uint16_t *s, uint16_t val, size_t n) {
        #if defined(WJR_COMPILER_MSVC)
    __stosw(reinterpret_cast<unsigned short *>(s), val, n);
        #else
    asm volatile("rep stosw" : "+D"(s), "+c"(n) : "a"(val) : "memory");
        #endif
}

WJR_INTRINSIC_INLINE void rep_stos(uint32_t *s, uint32_t val, size_t n) {
        #if defined(WJR_COMPILER_MSVC)
    __stosd(reinterpret_cast<unsigned long *>(s), val, n);
        #else
    asm volatile("rep stos{l|d}" : "+D"(s), "+c"(n) : "a"(val) : "memory");
        #endif
}

WJR_INTRINSIC_INLINE void rep_stos(uint64_t *s, uint64_t val, size_t n) {
        #if defined(WJR_COMPILER_MSVC)
    __stosq(reinterpret_cast<unsigned long long *>(s), val, n);
        #else
    asm volatile("rep stosq" : "+D"(s), "+c"(n) : "a"(val) : "memory");
        #endif
}

    #endif

// NOLINTEND

namespace {
template <typename T>
struct set_threshold {};

    #define WJR_REGISTER_SET_HELPER(type)                                                          \
        template <>                                                                                \
        struct set_threshold<uint##type##_t> {                                                     \
            static size_t get() noexcept { return __x86_rep_stosb_threshold_u##type; }             \
        }

WJR_REGISTER_SET_HELPER(16);
WJR_REGISTER_SET_HELPER(32);
WJR_REGISTER_SET_HELPER(64);

    #undef WJR_REGISTER_SET_HELPER
} // namespace

template <typename simd, typename T>
WJR_HOT void large_builtin_set_n(T *dst, T val, size_t n) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;
    constexpr auto simd_width = simd::width();
    constexpr auto type_width = simd_width / nd;
    constexpr auto u8_width = simd_width / 8;

    static_assert(type_width * sizeof(T) == u8_width);

    auto y = simd::set1(val, T());

    do {
        if (WJR_UNLIKELY(n > type_width * 2)) {
            if (WJR_UNLIKELY(n > type_width * 4)) {
                break;
            }

            simd::storeu(dst, y);
            simd::storeu(dst + type_width, y);
            simd::storeu(dst + n - type_width * 2, y);
            simd::storeu(dst + n - type_width, y);
            return;
        }

        simd::storeu(dst, y);
        simd::storeu(dst + n - type_width, y);
        return;
    } while (0);

    WJR_ASSUME(n > type_width * 4);

    T *ps;
    T *pe = dst + n;

    #if WJR_HAS_BUILTIN(REP_STOS)
    if (WJR_UNLIKELY(n > set_threshold<T>::get())) {
        simd::storeu(dst, y);
        simd::storeu(pe - type_width, y);

        ps = reinterpret_cast<T *>((reinterpret_cast<uintptr_t>(dst) + u8_width) & -u8_width);
        pe = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(pe) & -u8_width);

        if constexpr (sizeof(T) != 1) {
            const uintptr_t mo = reinterpret_cast<uintptr_t>(dst) % sizeof(T);
            val = rotr(val, mo * 8);
            y = simd::set1(val, T());
        }

        simd::store(ps, y);

        if constexpr (!set_detail::is_avx) {
            simd::store(ps + type_width, y);
            simd::store(ps + type_width * 2, y);
            simd::store(pe - type_width * 3, y);
            simd::store(pe - type_width * 2, y);
        }

        simd::store(pe - type_width, y);

        ps = reinterpret_cast<T *>((reinterpret_cast<uintptr_t>(dst) + 64) & -64);
        pe = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(pe) & -64);
        rep_stos(ps, val, pe - ps);
        return;
    }
    #endif

    do {
        const uintptr_t ups = (reinterpret_cast<uintptr_t>(dst) + u8_width) & -u8_width;
        simd::storeu(dst, y);
        simd::storeu(pe - type_width, y);
        ps = reinterpret_cast<T *>(ups);

        if constexpr (sizeof(T) != 1) {
            const uintptr_t mo = reinterpret_cast<uintptr_t>(dst) % sizeof(T);
            y = simd::set1(rotr(val, mo * 8), T());
        }
    } while (0);

    if (WJR_LIKELY(ps <= pe - type_width * 4)) {
        do {
            simd::store(ps, y);
            simd::store(ps + type_width, y);
            simd::store(ps + type_width * 2, y);
            simd::store(ps + type_width * 3, y);

            ps += type_width * 4;
        } while (WJR_LIKELY(ps <= pe - type_width * 4));
    }

    ps = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(pe) & -u8_width);
    simd::store(ps - type_width * 3, y);
    simd::store(ps - type_width * 2, y);
    simd::store(ps - type_width, y);
}

template void large_builtin_set_n<set_detail::simd, uint16_t>(uint16_t *, uint16_t,
                                                              size_t) noexcept;
template void large_builtin_set_n<set_detail::simd, uint32_t>(uint32_t *, uint32_t,
                                                              size_t) noexcept;
template void large_builtin_set_n<set_detail::simd, uint64_t>(uint64_t *, uint64_t,
                                                              size_t) noexcept;

#endif

} // namespace wjr