#ifndef WJR_SIMD_SIMD_CAST_HPP__
#define WJR_SIMD_SIMD_CAST_HPP__

#include <wjr/type_traits.hpp>

#define WJR_HAS_SIMD_SIMD WJR_HAS_DEF

#if defined(_MSC_VER)
/* Microsoft C/C++-compatible compiler */
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
/* GCC-compatible compiler, targeting x86/x86-64 */
#include <x86intrin.h>
#elif defined(__GNUC__) && defined(__ARM_NEON__)
/* GCC-compatible compiler, targeting ARM with NEON */
#error "Not support yet."
#include <arm_neon.h>
#else
#undef WJR_HAS_SIMD_SIMD
#endif

namespace wjr {

template <typename T, typename U>
struct simd_cast_fn;

template <typename T, typename U>
inline constexpr simd_cast_fn<T, U> simd_cast;

// simd type can't be directly used on template
template <typename T>
struct simd_wrapper {
    using type = T;
};

template <typename T>
using simd_wrapper_t = typename simd_wrapper<T>::type;

#define WJR_REGISTER_SIMD_TAG(simd_type)                                                 \
    struct simd_type##_t {                                                               \
        using type = simd_type;                                                          \
    }

#define WJR_GET_SIMD_TAG(type) WJR_GET_SIMD_TAG_I(type)
#define WJR_GET_SIMD_TAG_I(type) WJR_GET_SIMD_TAG_##type

#define WJR_GET_SIMD_TAG_int8_t int8_t
#define WJR_GET_SIMD_TAG_uint8_t uint8_t
#define WJR_GET_SIMD_TAG_int16_t int16_t
#define WJR_GET_SIMD_TAG_uint16_t uint16_t
#define WJR_GET_SIMD_TAG_int32_t int32_t
#define WJR_GET_SIMD_TAG_uint32_t uint32_t
#define WJR_GET_SIMD_TAG_int64_t int64_t
#define WJR_GET_SIMD_TAG_uint64_t uint64_t

#define WJR_GET_SIMD_TAG___m128 __m128_t
#define WJR_GET_SIMD_TAG___m128i __m128i_t
#define WJR_GET_SIMD_TAG___m128d __m128d_t

#define WJR_GET_SIMD_TAG___m256 __m256_t
#define WJR_GET_SIMD_TAG___m256i __m256i_t
#define WJR_GET_SIMD_TAG___m256d __m256d_t

#define WJR_REGISTER_SIMD_CAST(args)                                                     \
    WJR_PP_TRANSFORM_UNWRAP_PUT(args, WJR_REGISTER_SIMD_CAST_I_CALLER)

#define WJR_REGISTER_SIMD_CAST_I(From, To, func)                                         \
    (template <>                                                                         \
    struct simd_cast_fn<WJR_GET_SIMD_TAG(From), WJR_GET_SIMD_TAG(To)> {                  \
        To operator()(From v) const {                                                    \
            return func(v);                                                              \
    }                                                                                    \
    }                                                                                    \
    ;)

#define WJR_REGISTER_SIMD_CAST_I_CALLER(args) WJR_REGISTER_SIMD_CAST_I args

#if WJR_HAS_SIMD(SSE)

WJR_REGISTER_SIMD_TAG(__m128);

#endif // SSE

#if WJR_HAS_SIMD(SSE2)

WJR_REGISTER_SIMD_TAG(__m128i);
WJR_REGISTER_SIMD_TAG(__m128d);

WJR_REGISTER_SIMD_CAST(
    ((__m128, __m128i, _mm_castps_si128), (__m128, __m128d, _mm_castps_pd),
     (__m128i, __m128, _mm_castsi128_ps), (__m128i, __m128d, _mm_castsi128_pd),
     (__m128d, __m128, _mm_castpd_ps), (__m128d, __m128i, _mm_castpd_si128)));

WJR_REGISTER_SIMD_CAST(((int8_t, __m128i, _mm_cvtsi32_si128),
                        (uint8_t, __m128i, _mm_cvtsi32_si128),
                        (__m128i, int8_t, _mm_cvtsi128_si32),
                        (__m128i, uint8_t, _mm_cvtsi128_si32)));

WJR_REGISTER_SIMD_CAST(((int16_t, __m128i, _mm_cvtsi32_si128),
                        (uint16_t, __m128i, _mm_cvtsi32_si128),
                        (__m128i, int16_t, _mm_cvtsi128_si32),
                        (__m128i, uint16_t, _mm_cvtsi128_si32)));

WJR_REGISTER_SIMD_CAST(((int32_t, __m128i, _mm_cvtsi32_si128),
                        (uint32_t, __m128i, _mm_cvtsi32_si128),
                        (__m128i, int32_t, _mm_cvtsi128_si32),
                        (__m128i, uint32_t, _mm_cvtsi128_si32)));

WJR_REGISTER_SIMD_CAST(((int64_t, __m128i, _mm_cvtsi64_si128),
                        (uint64_t, __m128i, _mm_cvtsi64_si128),
                        (__m128i, int64_t, _mm_cvtsi128_si64),
                        (__m128i, uint64_t, _mm_cvtsi128_si64)));

#endif // SSE2

#if WJR_HAS_SIMD(AVX)

WJR_REGISTER_SIMD_TAG(__m256);
WJR_REGISTER_SIMD_TAG(__m256i);
WJR_REGISTER_SIMD_TAG(__m256d);

WJR_REGISTER_SIMD_CAST(
    ((__m256, __m256i, _mm256_castps_si256), (__m256, __m256d, _mm256_castps_pd),
     (__m256i, __m256, _mm256_castsi256_ps), (__m256i, __m256d, _mm256_castsi256_pd),
     (__m256d, __m256, _mm256_castpd_ps), (__m256d, __m256i, _mm256_castpd_si256),
     (__m128i, __m256i, _mm256_castsi128_si256),
     (__m256i, __m128i, _mm256_castsi256_si128)));

#define WJR_REGISTER_INT32_CVT2_AVX(x)                                                   \
    simd_cast<__m128i_t, __m256i_t>(simd_cast<uint32_t, __m128i_t>(x))
#define WJR_REGISTER_INT64_CVT2_AVX(x)                                                   \
    simd_cast<__m128i_t, __m256i_t>(simd_cast<uint64_t, __m128i_t>(x))

#define WJR_REGISTER_AVX_CVT2_INT32(x)                                                   \
    simd_cast<__m128i_t, uint32_t>(simd_cast<__m256i_t, __m128i_t>(x))
#define WJR_REGISTER_AVX_CVT2_INT64(x)                                                   \
    simd_cast<__m128i_t, uint64_t>(simd_cast<__m256i_t, __m128i_t>(x))

WJR_REGISTER_SIMD_CAST(((int8_t, __m256i, WJR_REGISTER_INT32_CVT2_AVX),
                        (uint8_t, __m256i, WJR_REGISTER_INT32_CVT2_AVX),
                        (__m256i, int8_t, WJR_REGISTER_AVX_CVT2_INT32),
                        (__m256i, uint8_t, WJR_REGISTER_AVX_CVT2_INT32)));

WJR_REGISTER_SIMD_CAST(((int16_t, __m256i, WJR_REGISTER_INT32_CVT2_AVX),
                        (uint16_t, __m256i, WJR_REGISTER_INT32_CVT2_AVX),
                        (__m256i, int16_t, WJR_REGISTER_AVX_CVT2_INT32),
                        (__m256i, uint16_t, WJR_REGISTER_AVX_CVT2_INT32)));

WJR_REGISTER_SIMD_CAST(((int32_t, __m256i, WJR_REGISTER_INT32_CVT2_AVX),
                        (uint32_t, __m256i, WJR_REGISTER_INT32_CVT2_AVX),
                        (__m256i, int32_t, WJR_REGISTER_AVX_CVT2_INT32),
                        (__m256i, uint32_t, WJR_REGISTER_AVX_CVT2_INT32)));

WJR_REGISTER_SIMD_CAST(((int64_t, __m256i, WJR_REGISTER_INT64_CVT2_AVX),
                        (uint64_t, __m256i, WJR_REGISTER_INT64_CVT2_AVX),
                        (__m256i, int64_t, WJR_REGISTER_AVX_CVT2_INT64),
                        (__m256i, uint64_t, WJR_REGISTER_AVX_CVT2_INT64)));

#undef WJR_REGISTER_AVX_CVT2_INT64
#undef WJR_REGISTER_AVX_CVT2_INT32

#undef WJR_REGISTER_INT64_CVT2_AVX
#undef WJR_REGISTER_INT32_CVT2_AVX

#endif // AVX

#undef WJR_REGISTER_SIMD_CAST_I_CALLER
#undef WJR_REGISTER_SIMD_CAST_I
#undef WJR_REGISTER_SIMD_CAST

#undef WJR_GET_SIMD_TAG_int8_t
#undef WJR_GET_SIMD_TAG_uint8_t
#undef WJR_GET_SIMD_TAG_int16_t
#undef WJR_GET_SIMD_TAG_uint16_t
#undef WJR_GET_SIMD_TAG_int32_t
#undef WJR_GET_SIMD_TAG_uint32_t
#undef WJR_GET_SIMD_TAG_int64_t
#undef WJR_GET_SIMD_TAG_uint64_t

#undef WJR_GET_SIMD_TAG___m128
#undef WJR_GET_SIMD_TAG___m128i
#undef WJR_GET_SIMD_TAG___m128d

#undef WJR_GET_SIMD_TAG___m256
#undef WJR_GET_SIMD_TAG___m256i
#undef WJR_GET_SIMD_TAG___m256d

#undef WJR_GET_SIMD_TAG_I
#undef WJR_GET_SIMD_TAG

#undef WJR_REGISTER_SIMD_TAG

} // namespace wjr

#endif // WJR_SIMD_SIMD_CAST_HPP__