#ifndef WJR_SIMD_SIMD_CAST_HPP__
#define WJR_SIMD_SIMD_CAST_HPP__

#include <wjr/type_traits.hpp>

#define WJR_HAS_SIMD_SIMD WJR_HAS_DEF

#if defined(_MSC_VER)
/* Microsoft C/C++-compatible compiler */
#include <intrin.h>
#elif defined(__GNUC__)
/* GCC-compatible compiler, targeting x86/x86-64 */
#include <x86intrin.h>
#else
#undef WJR_HAS_SIMD_SIMD
#endif

namespace wjr {

template <typename From, typename To>
struct simd_cast_fn;

template <typename From, typename To>
inline constexpr simd_cast_fn<From, To> simd_cast;

// simd type can't be directly used on template
template <typename T>
struct simd_wrapper {
    using type = T;
};

template <typename T>
using simd_wrapper_t = typename simd_wrapper<T>::type;

#if WJR_HAS_SIMD(SSE)

struct __m128_t {
    using type = __m128;
};

#endif // SSE

#if WJR_HAS_SIMD(SSE2)

struct __m128i_t {
    using type = __m128i;
};
struct __m128d_t {
    using type = __m128d;
};

template <>
struct simd_cast_fn<__m128_t, __m128i_t> {
    __m128i operator()(__m128 v) const { return _mm_castps_si128(v); }
};
template <>
struct simd_cast_fn<__m128_t, __m128d_t> {
    __m128d operator()(__m128 v) const { return _mm_castps_pd(v); }
};
template <>
struct simd_cast_fn<__m128i_t, __m128_t> {
    __m128 operator()(__m128i v) const { return _mm_castsi128_ps(v); }
};
template <>
struct simd_cast_fn<__m128i_t, __m128d_t> {
    __m128d operator()(__m128i v) const { return _mm_castsi128_pd(v); }
};
template <>
struct simd_cast_fn<__m128d_t, __m128_t> {
    __m128 operator()(__m128d v) const { return _mm_castpd_ps(v); }
};
template <>
struct simd_cast_fn<__m128d_t, __m128i_t> {
    __m128i operator()(__m128d v) const { return _mm_castpd_si128(v); }
};

template <>
struct simd_cast_fn<int8_t, __m128i_t> {
    __m128i operator()(int8_t v) const { return _mm_cvtsi32_si128(v); }
};
template <>
struct simd_cast_fn<uint8_t, __m128i_t> {
    __m128i operator()(uint8_t v) const { return _mm_cvtsi32_si128(v); }
};
template <>
struct simd_cast_fn<__m128i_t, int8_t> {
    int8_t operator()(__m128i v) const { return _mm_cvtsi128_si32(v); }
};
template <>
struct simd_cast_fn<__m128i_t, uint8_t> {
    uint8_t operator()(__m128i v) const { return _mm_cvtsi128_si32(v); }
};

template <>
struct simd_cast_fn<int16_t, __m128i_t> {
    __m128i operator()(int16_t v) const { return _mm_cvtsi32_si128(v); }
};
template <>
struct simd_cast_fn<uint16_t, __m128i_t> {
    __m128i operator()(uint16_t v) const { return _mm_cvtsi32_si128(v); }
};
template <>
struct simd_cast_fn<__m128i_t, int16_t> {
    int16_t operator()(__m128i v) const { return _mm_cvtsi128_si32(v); }
};
template <>
struct simd_cast_fn<__m128i_t, uint16_t> {
    uint16_t operator()(__m128i v) const { return _mm_cvtsi128_si32(v); }
};

template <>
struct simd_cast_fn<int32_t, __m128i_t> {
    __m128i operator()(int32_t v) const { return _mm_cvtsi32_si128(v); }
};
template <>
struct simd_cast_fn<uint32_t, __m128i_t> {
    __m128i operator()(uint32_t v) const { return _mm_cvtsi32_si128(v); }
};
template <>
struct simd_cast_fn<__m128i_t, int32_t> {
    int32_t operator()(__m128i v) const { return _mm_cvtsi128_si32(v); }
};
template <>
struct simd_cast_fn<__m128i_t, uint32_t> {
    uint32_t operator()(__m128i v) const { return _mm_cvtsi128_si32(v); }
};

template <>
struct simd_cast_fn<int64_t, __m128i_t> {
    __m128i operator()(int64_t v) const { return _mm_cvtsi64_si128(v); }
};
template <>
struct simd_cast_fn<uint64_t, __m128i_t> {
    __m128i operator()(uint64_t v) const { return _mm_cvtsi64_si128(v); }
};
template <>
struct simd_cast_fn<__m128i_t, int64_t> {
    int64_t operator()(__m128i v) const { return _mm_cvtsi128_si64(v); }
};
template <>
struct simd_cast_fn<__m128i_t, uint64_t> {
    uint64_t operator()(__m128i v) const { return _mm_cvtsi128_si64(v); }
};

#endif // SSE2

#if WJR_HAS_SIMD(AVX)

struct __m256_t {
    using type = __m256;
};
struct __m256i_t {
    using type = __m256i;
};
struct __m256d_t {
    using type = __m256d;
};

template <>
struct simd_cast_fn<__m256_t, __m256i_t> {
    __m256i operator()(__m256 v) const { return _mm256_castps_si256(v); }
};
template <>
struct simd_cast_fn<__m256_t, __m256d_t> {
    __m256d operator()(__m256 v) const { return _mm256_castps_pd(v); }
};
template <>
struct simd_cast_fn<__m256i_t, __m256_t> {
    __m256 operator()(__m256i v) const { return _mm256_castsi256_ps(v); }
};
template <>
struct simd_cast_fn<__m256i_t, __m256d_t> {
    __m256d operator()(__m256i v) const { return _mm256_castsi256_pd(v); }
};
template <>
struct simd_cast_fn<__m256d_t, __m256_t> {
    __m256 operator()(__m256d v) const { return _mm256_castpd_ps(v); }
};
template <>
struct simd_cast_fn<__m256d_t, __m256i_t> {
    __m256i operator()(__m256d v) const { return _mm256_castpd_si256(v); }
};
template <>
struct simd_cast_fn<__m128i_t, __m256i_t> {
    __m256i operator()(__m128i v) const { return _mm256_castsi128_si256(v); }
};
template <>
struct simd_cast_fn<__m256i_t, __m128i_t> {
    __m128i operator()(__m256i v) const { return _mm256_castsi256_si128(v); }
};

template <>
struct simd_cast_fn<int8_t, __m256i_t> {
    __m256i operator()(int8_t v) const {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint32_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<uint8_t, __m256i_t> {
    __m256i operator()(uint8_t v) const {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint32_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<__m256i_t, int8_t> {
    int8_t operator()(__m256i v) const {
        return simd_cast<__m128i_t, uint32_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<__m256i_t, uint8_t> {
    uint8_t operator()(__m256i v) const {
        return simd_cast<__m128i_t, uint32_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<int16_t, __m256i_t> {
    __m256i operator()(int16_t v) const {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint32_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<uint16_t, __m256i_t> {
    __m256i operator()(uint16_t v) const {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint32_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<__m256i_t, int16_t> {
    int16_t operator()(__m256i v) const {
        return simd_cast<__m128i_t, uint32_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<__m256i_t, uint16_t> {
    uint16_t operator()(__m256i v) const {
        return simd_cast<__m128i_t, uint32_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<int32_t, __m256i_t> {
    __m256i operator()(int32_t v) const {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint32_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<uint32_t, __m256i_t> {
    __m256i operator()(uint32_t v) const {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint32_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<__m256i_t, int32_t> {
    int32_t operator()(__m256i v) const {
        return simd_cast<__m128i_t, uint32_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<__m256i_t, uint32_t> {
    uint32_t operator()(__m256i v) const {
        return simd_cast<__m128i_t, uint32_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};

template <>
struct simd_cast_fn<int64_t, __m256i_t> {
    __m256i operator()(int64_t v) const {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint64_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<uint64_t, __m256i_t> {
    __m256i operator()(uint64_t v) const {
        return simd_cast<__m128i_t, __m256i_t>(simd_cast<uint64_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<__m256i_t, int64_t> {
    int64_t operator()(__m256i v) const {
        return simd_cast<__m128i_t, uint64_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};
template <>
struct simd_cast_fn<__m256i_t, uint64_t> {
    uint64_t operator()(__m256i v) const {
        return simd_cast<__m128i_t, uint64_t>(simd_cast<__m256i_t, __m128i_t>(v));
    }
};

#endif // AVX

} // namespace wjr

#endif // WJR_SIMD_SIMD_CAST_HPP__