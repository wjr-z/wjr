#include <cpuinfo_x86.h>
#include <wjr/arch/x86/math/set.hpp>

namespace wjr {

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
    asm volatile("rep stosd" : "+D"(s), "+c"(n) : "a"(val) : "memory");
    #endif
}

WJR_INTRINSIC_INLINE void rep_stos(uint64_t *s, uint64_t val, size_t n) {
    #if defined(WJR_COMPILER_MSVC)
    __stosq(reinterpret_cast<unsigned long long *>(s), val, n);
    #else
    asm volatile("rep stosq" : "+D"(s), "+c"(n) : "a"(val) : "memory");
    #endif
}

bool __x86_is_enhanced_rep = cpu_features::GetX86Info().features.erms != 0;
#else
inline constexpr bool __x86_is_enhanced_rep = false;
#endif

#ifndef WJR_X86_REP_STOSB_DEFAULT_THRESHOLD
    #define WJR_X86_REP_STOSB_DEFAULT_THRESHOLD ((size_t)(1) << 24)
#endif

inline constexpr size_t __x86_rep_stosb_default_threshold = WJR_X86_REP_STOSB_DEFAULT_THRESHOLD;

template <typename T>
size_t __x86_rep_stosb_threshold =
    __x86_is_enhanced_rep ? __x86_rep_stosb_default_threshold / sizeof(T)
                          : std::numeric_limits<size_t>::max();

template size_t __x86_rep_stosb_threshold<uint64_t>;

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
    } while (false);

    WJR_ASSUME(n > type_width * 4);

    T *ps;
    T *pe = dst + n;

#if WJR_HAS_BUILTIN(REP_STOS)
    if (WJR_UNLIKELY(n > __x86_rep_stosb_threshold<T>)) {
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
        // rep_stos(ps, val, pe - ps);
        rep_stos(reinterpret_cast<uint8_t *>(ps), val, (pe - ps) * sizeof(T));
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
    } while (false);

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

template void large_builtin_set_n<set_detail::simd, uint64_t>(uint64_t *, uint64_t,
                                                              size_t) noexcept;

} // namespace wjr