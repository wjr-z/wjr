#ifndef WJR_ARCH_X86_SIMD_AVX_HPP__
#define WJR_ARCH_X86_SIMD_AVX_HPP__

#include <wjr/arch/x86/simd/sse.hpp>

namespace wjr {

struct avx {
    using mask_type = uint32_t;

#if WJR_HAS_SIMD(AVX)

    using float_type = __m256;
    using float_tag_type = __m256_t;
    using int_type = __m256i;
    using int_tag_type = __m256i_t;
    using double_type = __m256d;
    using double_tag_type = __m256d_t;

#endif // AVX

    constexpr static size_t width() noexcept;
    constexpr static mask_type mask() noexcept;

    WJR_INTRINSIC_CONSTEXPR static int clz_nz_epu64(mask_type x) noexcept;
    WJR_INTRINSIC_CONSTEXPR static int clz_nz(mask_type x, uint64_t) noexcept;

    WJR_INTRINSIC_CONSTEXPR static int ctz_nz_epu64(mask_type x) noexcept;
    WJR_INTRINSIC_CONSTEXPR static int ctz_nz(mask_type x, uint64_t) noexcept;

#if WJR_HAS_SIMD(AVX)

    WJR_INTRINSIC_INLINE static __m256i concat(__m128i a, __m128i b) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract_epi32(__m256i v) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static int64_t extract_epi64(__m256i v) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m256i v, int32_t) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static int64_t extract(__m256i v, int64_t) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m128i extract_si128(__m256i v) noexcept;

    WJR_INTRINSIC_INLINE static __m128i getlow(__m256i a) noexcept;

    WJR_INTRINSIC_INLINE static __m128i gethigh(__m256i a) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i insert_epi8(__m256i v, int8_t i) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i insert_epi16(__m256i v, int16_t i) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i insert_epi32(__m256i v, int32_t i) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i insert_epi64(__m256i v, int64_t i) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i insert_si128(__m256i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i load(const void *p) noexcept;
    WJR_INTRINSIC_INLINE static __m256i loadu(const void *p) noexcept;

    WJR_INTRINSIC_INLINE static __m256i ones() noexcept;

    WJR_INTRINSIC_INLINE static __m256i loadu_si16(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m256i loadu_si32(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m256i loadu_si48(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m256i loadu_si64(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m256i loadu_si80(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m256i loadu_si96(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m256i loadu_si112(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m256i loadu_si128(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m256i loadu_si144(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m256i loadu_si160(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m256i loadu_si176(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m256i loadu_si192(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m256i loadu_si208(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m256i loadu_si224(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m256i loadu_si240(const void *ptr) noexcept;
    WJR_INTRINSIC_INLINE static __m256i loadu_si256(const void *ptr) noexcept;

    WJR_INTRINSIC_INLINE static __m256i loadu_si16x(const void *ptr, int n) noexcept;

    WJR_INTRINSIC_INLINE static __m256i
    set_epi8(char e31, char e30, char e29, char e28, char e27, char e26, char e25, char e24,
             char e23, char e22, char e21, char e20, char e19, char e18, char e17, char e16,
             char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8, char e7,
             char e6, char e5, char e4, char e3, char e2, char e1, char e0) noexcept;

    WJR_INTRINSIC_INLINE static __m256i set_epi16(short e15, short e14, short e13, short e12,
                                                  short e11, short e10, short e9, short e8,
                                                  short e7, short e6, short e5, short e4, short e3,
                                                  short e2, short e1, short e0) noexcept;

    WJR_INTRINSIC_INLINE static __m256i set_epi32(int e7, int e6, int e5, int e4, int e3, int e2,
                                                  int e1, int e0) noexcept;

    WJR_INTRINSIC_INLINE static __m256i set_epi64x(long long e3, long long e2, long long e1,
                                                   long long e0) noexcept;

    WJR_INTRINSIC_INLINE static __m256i
    setr_epi8(char e31, char e30, char e29, char e28, char e27, char e26, char e25, char e24,
              char e23, char e22, char e21, char e20, char e19, char e18, char e17, char e16,
              char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8, char e7,
              char e6, char e5, char e4, char e3, char e2, char e1, char e0) noexcept;

    WJR_INTRINSIC_INLINE static __m256i setr_epi16(short e15, short e14, short e13, short e12,
                                                   short e11, short e10, short e9, short e8,
                                                   short e7, short e6, short e5, short e4, short e3,
                                                   short e2, short e1, short e0) noexcept;

    WJR_INTRINSIC_INLINE static __m256i setr_epi32(int e7, int e6, int e5, int e4, int e3, int e2,
                                                   int e1, int e0) noexcept;

    WJR_INTRINSIC_INLINE static __m256i setr_epi64x(long long e3, long long e2, long long e1,
                                                    long long e0) noexcept;

    WJR_INTRINSIC_INLINE static __m256i set1_epi8(int8_t a) noexcept;
    WJR_INTRINSIC_INLINE static __m256i set1_epi16(int16_t a) noexcept;
    WJR_INTRINSIC_INLINE static __m256i set1_epi32(int32_t a) noexcept;
    WJR_INTRINSIC_INLINE static __m256i set1_epi64(int64_t a) noexcept;

    WJR_INTRINSIC_INLINE static __m256i set1(int8_t a, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i set1(int16_t a, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i set1(int32_t a, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i set1(int64_t a, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i set1(uint8_t a, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i set1(uint16_t a, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i set1(uint32_t a, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i set1(uint64_t a, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i setmin_epi8() noexcept;
    WJR_INTRINSIC_INLINE static __m256i setmin_epi16() noexcept;
    WJR_INTRINSIC_INLINE static __m256i setmin_epi32() noexcept;
    WJR_INTRINSIC_INLINE static __m256i setmin_epi64() noexcept;

    WJR_INTRINSIC_INLINE static __m256i setmin(int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i setmin(int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i setmin(int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i setmin(int64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i setmax_epi8() noexcept;
    WJR_INTRINSIC_INLINE static __m256i setmax_epi16() noexcept;
    WJR_INTRINSIC_INLINE static __m256i setmax_epi32() noexcept;
    WJR_INTRINSIC_INLINE static __m256i setmax_epi64() noexcept;

    WJR_INTRINSIC_INLINE static __m256i setmax(int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i setmax(int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i setmax(int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i setmax(int64_t) noexcept;

    WJR_INTRINSIC_INLINE static void stream(void *p, __m256i a) noexcept;

    WJR_INTRINSIC_INLINE static void store(void *p, __m256i a) noexcept;
    WJR_INTRINSIC_INLINE static void storeu(void *p, __m256i a) noexcept;

    WJR_INTRINSIC_INLINE static int test_all_zeros(__m256i a) noexcept;

    WJR_INTRINSIC_INLINE static int testc(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static int testnzc(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static int testz(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i zeros() noexcept;

#endif // AVX

#if WJR_HAS_SIMD(AVX2)

    WJR_INTRINSIC_INLINE static __m256i And(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i AndNot(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i Or(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i Xor(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i Not(__m256i v) noexcept;

    WJR_INTRINSIC_INLINE static __m256i abs_epi8(__m256i v) noexcept;
    WJR_INTRINSIC_INLINE static __m256i abs_epi16(__m256i v) noexcept;
    WJR_INTRINSIC_INLINE static __m256i abs_epi32(__m256i v) noexcept;

    WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i add_epi8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i add_epi16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i add_epi32(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i add_epi64(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static uint8_t add_epu8(__m256i v) noexcept;
    WJR_INTRINSIC_INLINE static uint16_t add_epu16(__m256i v) noexcept;
    WJR_INTRINSIC_INLINE static uint32_t add_epu32(__m256i v) noexcept;
    WJR_INTRINSIC_INLINE static uint64_t add_epu64(__m256i v) noexcept;

    WJR_INTRINSIC_INLINE static int8_t add_epi8(__m256i v) noexcept;
    WJR_INTRINSIC_INLINE static int16_t add_epi16(__m256i v) noexcept;
    WJR_INTRINSIC_INLINE static int32_t add_epi32(__m256i v) noexcept;
    WJR_INTRINSIC_INLINE static int64_t add_epi64(__m256i v) noexcept;

    WJR_INTRINSIC_INLINE static int8_t add(__m256i v, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static int16_t add(__m256i v, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static int32_t add(__m256i v, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static int64_t add(__m256i v, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static uint8_t add(__m256i v, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static uint16_t add(__m256i v, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static uint32_t add(__m256i v, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static uint64_t add(__m256i v, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i adds_epi8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i adds_epi16(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i adds_epu8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i adds_epu16(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, uint16_t) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i alignr_epi16(__m256i a, __m256i b, int c) noexcept;
    WJR_INTRINSIC_INLINE static __m256i alignr_epi32(__m256i a, __m256i b, int c) noexcept;
    WJR_INTRINSIC_INLINE static __m256i alignr_epi64(__m256i a, __m256i b, int c) noexcept;

    WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i avg_epu8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i avg_epu16(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, uint16_t) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i blend_epi16(__m256i a, __m256i b) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i blend_epi32(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i blendv_epi8(__m256i a, __m256i b, __m256i mask) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i bslli_epi128(__m256i a) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i bsrli_epi128(__m256i a) noexcept;

    WJR_INTRINSIC_INLINE static __m256i cmpeq_epi8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpeq_epi16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpeq_epi32(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpeq_epi64(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i cmpge_epi8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpge_epi16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpge_epi32(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i cmpge_epu8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpge_epu16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpge_epu32(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i cmpgt_epi8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpgt_epi16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpgt_epi32(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpgt_epi64(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i cmpgt_epu8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpgt_epu16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpgt_epu32(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpgt_epu64(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i cmple_epi8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmple_epi16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmple_epi32(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i cmple_epu8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmple_epu16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmple_epu32(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i cmplt_epi8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmplt_epi16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmplt_epi32(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i cmplt_epu8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmplt_epu16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmplt_epu32(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i cmpne_epi8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpne_epi16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpne_epi32(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint32_t) noexcept;

    template <typename T>
    WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::equal_to<>, T) noexcept;
    template <typename T>
    WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::not_equal_to<>, T) noexcept;
    template <typename T>
    WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::greater<>, T) noexcept;
    template <typename T>
    WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::greater_equal<>, T) noexcept;
    template <typename T>
    WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::less<>, T) noexcept;
    template <typename T>
    WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::less_equal<>, T) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract_epi8(__m256i v) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract_epi16(__m256i v) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m256i v, int8_t) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static int extract(__m256i v, int16_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i hadd_epi16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i hadd_epi32(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i hadd(__m256i a, __m256i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i hadd(__m256i a, __m256i b, int32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i hadds_epi16(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i hsub_epi16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i hsub_epi32(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i hsub(__m256i a, __m256i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i hsub(__m256i a, __m256i b, int32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i hsubs_epi16(__m256i a, __m256i b) noexcept;

    template <typename T, WJR_REQUIRES(is_any_of_v<T, int8_t, int16_t, int32_t, int64_t, uint8_t,
                                                   uint16_t, uint32_t, uint64_t>)>
    WJR_INTRINSIC_INLINE static __m256i logical_and(__m256i a, __m256i b, T) noexcept;

    template <typename T, WJR_REQUIRES(is_any_of_v<T, int8_t, int16_t, int32_t, int64_t, uint8_t,
                                                   uint16_t, uint32_t, uint64_t>)>
    WJR_INTRINSIC_INLINE static __m256i logical_not(__m256i v, T) noexcept;

    template <typename T, WJR_REQUIRES(is_any_of_v<T, int8_t, int16_t, int32_t, int64_t, uint8_t,
                                                   uint16_t, uint32_t, uint64_t>)>
    WJR_INTRINSIC_INLINE static __m256i logical_or(__m256i a, __m256i b, T) noexcept;

    WJR_INTRINSIC_INLINE static __m256i madd_epi16(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i max_epi8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i max_epi16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i max_epi32(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i max_epu8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i max_epu16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i max_epu32(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static int8_t max_epi8(__m256i a) noexcept;
    WJR_INTRINSIC_INLINE static int16_t max_epi16(__m256i a) noexcept;
    WJR_INTRINSIC_INLINE static int32_t max_epi32(__m256i a) noexcept;
    WJR_INTRINSIC_INLINE static uint8_t max_epu8(__m256i a) noexcept;
    WJR_INTRINSIC_INLINE static uint16_t max_epu16(__m256i a) noexcept;
    WJR_INTRINSIC_INLINE static uint32_t max_epu32(__m256i a) noexcept;

    WJR_INTRINSIC_INLINE static int8_t max(__m256i a, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static int16_t max(__m256i a, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static int32_t max(__m256i a, int32_t) noexcept;

    WJR_INTRINSIC_INLINE static uint8_t max(__m256i a, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static uint16_t max(__m256i a, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static uint32_t max(__m256i a, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i min_epi8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i min_epi16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i min_epi32(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i min_epu8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i min_epu16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i min_epu32(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static int8_t min_epi8(__m256i a) noexcept;
    WJR_INTRINSIC_INLINE static int16_t min_epi16(__m256i a) noexcept;
    WJR_INTRINSIC_INLINE static int32_t min_epi32(__m256i a) noexcept;

    WJR_INTRINSIC_INLINE static uint8_t min_epu8(__m256i a) noexcept;
    WJR_INTRINSIC_INLINE static uint16_t min_epu16(__m256i a) noexcept;
    WJR_INTRINSIC_INLINE static uint32_t min_epu32(__m256i a) noexcept;

    WJR_INTRINSIC_INLINE static int8_t min(__m256i a, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static int16_t min(__m256i a, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static int32_t min(__m256i a, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static uint8_t min(__m256i a, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static uint16_t min(__m256i a, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static uint32_t min(__m256i a, uint32_t) noexcept;

    WJR_INTRINSIC_INLINE static mask_type movemask_epi8(__m256i a) noexcept;

    WJR_INTRINSIC_INLINE static __m256i mul_epi32(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i mul_epu32(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i mulhi_epi16(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i mulhi_epu16(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i mullo_epi16(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i packs_epi16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i packs_epi32(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i packus_epi16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i packus_epi32(__m256i a, __m256i b) noexcept;

    template <int imm>
    WJR_INTRINSIC_INLINE static __m256i shl(__m256i a) noexcept;

    template <int imm>
    WJR_INTRINSIC_INLINE static __m256i shr(__m256i a) noexcept;

    WJR_INTRINSIC_INLINE static __m256i shuffle_epi8(__m256i a, __m256i b) noexcept;
    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i shuffle_epi32(__m256i a) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i shufflehi_epi16(__m256i a) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i shufflelo_epi16(__m256i a) noexcept;

    WJR_INTRINSIC_INLINE static __m256i sll_epi16(__m256i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sll_epi32(__m256i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sll_epi64(__m256i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint64_t) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i slli(__m256i a) noexcept;
    WJR_INTRINSIC_INLINE static __m256i slli_epi16(__m256i a, int imm8) noexcept;
    WJR_INTRINSIC_INLINE static __m256i slli_epi32(__m256i a, int imm8) noexcept;
    WJR_INTRINSIC_INLINE static __m256i slli_epi64(__m256i a, int imm8) noexcept;

    WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i sra_epi16(__m256i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sra_epi32(__m256i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i sra(__m256i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sra(__m256i a, __m128i b, int32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i srai_epi16(__m256i a, int imm8) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srai_epi32(__m256i a, int imm8) noexcept;

    WJR_INTRINSIC_INLINE static __m256i srai(__m256i a, int imm8, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srai(__m256i a, int imm8, int32_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i stream_load(const void *p) noexcept;

    WJR_INTRINSIC_INLINE static __m256i srl_epi16(__m256i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srl_epi32(__m256i a, __m128i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srl_epi64(__m256i a, __m128i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint64_t) noexcept;

    template <int imm8>
    WJR_INTRINSIC_INLINE static __m256i srli(__m256i a) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srli_epi8(__m256i a, int imm8) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srli_epi16(__m256i a, int imm8) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srli_epi32(__m256i a, int imm8) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srli_epi64(__m256i a, int imm8) noexcept;

    WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i sub_epi8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sub_epi16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sub_epi32(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sub_epi64(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i subs_epi8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i subs_epi16(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i subs_epu8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i subs_epu16(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, uint16_t) noexcept;

    WJR_INTRINSIC_INLINE static int test_all_ones(__m256i a) noexcept;

    WJR_INTRINSIC_INLINE static __m256i unpackhi_epi8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpackhi_epi16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpackhi_epi32(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpackhi_epi64(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint64_t) noexcept;

    WJR_INTRINSIC_INLINE static __m256i unpacklo_epi8(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpacklo_epi16(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpacklo_epi32(__m256i a, __m256i b) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpacklo_epi64(__m256i a, __m256i b) noexcept;

    WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int32_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int64_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint8_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint16_t) noexcept;
    WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint32_t) noexcept;

#endif // AVX2
};

namespace avx_detail {
namespace {
#if WJR_HAS_SIMD(AVX2)

static const __m256i srli_epi8_mask[8] = {
    avx::set1_epi16(0xFFFF), avx::set1_epi16(0x7F7F), avx::set1_epi16(0x3F3F),
    avx::set1_epi16(0x1F1F), avx::set1_epi16(0xF0F),  avx::set1_epi16(0x707),
    avx::set1_epi16(0x303),  avx::set1_epi16(0x101),
};

#endif
} // namespace
} // namespace avx_detail

#if WJR_HAS_SIMD(AVX)

template <>
struct broadcast_fn<uint8_t, __m256i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256i operator()(uint8_t v) const noexcept {
        return _mm256_set1_epi8(v);
    }
};

template <>
struct broadcast_fn<uint16_t, __m256i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256i operator()(uint16_t v) const noexcept {
        return _mm256_set1_epi16(v);
    }
};

template <>
struct broadcast_fn<uint32_t, __m256i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256i operator()(uint32_t v) const noexcept {
        return _mm256_set1_epi32(v);
    }
};

template <>
struct broadcast_fn<uint64_t, __m256i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256i operator()(uint64_t v) const noexcept {
        return _mm256_set1_epi64x(v);
    }
};

template <>
struct broadcast_fn<__m256i_t, __m256i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256i operator()(__m256i v) const noexcept { return v; }
};

template <>
struct broadcast_fn<__m128i_t, __m256i_t> {
    WJR_CONST WJR_INTRINSIC_INLINE __m256i operator()(__m128i v) const noexcept {
    #if WJR_HAS_SIMD(AVX2)
        return _mm256_broadcastsi128_si256(v);
    #else
        return _mm256_insertf128_si256(_mm256_castsi128_si256(v), v, 1);
    #endif
    }
};

#endif // AVX

/*------------------------avx------------------------*/

constexpr size_t avx::width() noexcept { return 256; }

constexpr avx::mask_type avx::mask() noexcept { return 0xffffffff; }

constexpr int avx::clz_nz_epu64(mask_type mask) noexcept { return clz(mask); }
constexpr int avx::clz_nz(mask_type mask, uint64_t) noexcept { return clz_nz_epu64(mask); }

constexpr int avx::ctz_nz_epu64(mask_type mask) noexcept { return ctz(mask); }
constexpr int avx::ctz_nz(mask_type mask, uint64_t) noexcept { return ctz_nz_epu64(mask); }

#if WJR_HAS_SIMD(AVX)

__m256i avx::concat(__m128i a, __m128i b) noexcept {
    return insert_si128<1>(simd_cast<__m128i_t, __m256i_t>(a), b);
}

template <int imm8>
int avx::extract_epi32(__m256i v) noexcept {
    return _mm256_extract_epi32(v, imm8);
}

template <int imm8>
int64_t avx::extract_epi64(__m256i v) noexcept {
    return _mm256_extract_epi64(v, imm8);
}

template <int imm8>
int avx::extract(__m256i v, int32_t) noexcept {
    return extract_epi32<imm8>(v);
}

template <int imm8>
int64_t avx::extract(__m256i v, int64_t) noexcept {
    return extract_epi64<imm8>(v);
}

template <int imm8>
__m128i avx::extract_si128(__m256i v) noexcept {
    #if WJR_HAS_SIMD(AV2)
    return _mm256_extracti128_si256(v, imm8);
    #else
    return _mm256_extractf128_si256(v, imm8);
    #endif
}

__m128i avx::getlow(__m256i a) noexcept { return simd_cast<__m256i_t, __m128i_t>(a); }

__m128i avx::gethigh(__m256i a) noexcept { return extract_si128<1>(a); }

template <int imm8>
__m256i avx::insert_epi8(__m256i v, int8_t i) noexcept {
    return _mm256_insert_epi8(v, i, imm8);
}

template <int imm8>
__m256i avx::insert_epi16(__m256i v, int16_t i) noexcept {
    return _mm256_insert_epi16(v, i, imm8);
}

template <int imm8>
__m256i avx::insert_epi32(__m256i v, int32_t i) noexcept {
    return _mm256_insert_epi32(v, i, imm8);
}

template <int imm8>
__m256i avx::insert_epi64(__m256i v, int64_t i) noexcept {
    return _mm256_insert_epi64(v, i, imm8);
}

template <int imm8>
__m256i avx::insert_si128(__m256i a, __m128i b) noexcept {
    #if WJR_HAS_SIMD(AVX2)
    return _mm256_inserti128_si256(a, b, imm8);
    #else
    return _mm256_insertf128_si256(a, b, imm8);
    #endif
}

__m256i avx::load(const void *p) noexcept {
    return _mm256_load_si256(static_cast<const __m256i *>(p));
}
__m256i avx::loadu(const void *p) noexcept {
    return _mm256_loadu_si256(static_cast<const __m256i *>(p));
}

__m256i avx::ones() noexcept { return _mm256_set1_epi32(-1); }

__m256i avx::loadu_si16(const void *ptr) noexcept {
    return simd_cast<__m128i_t, __m256i_t>(sse::loadu_si16(ptr));
}

__m256i avx::loadu_si32(const void *ptr) noexcept {
    return simd_cast<__m128i_t, __m256i_t>(sse::loadu_si32(ptr));
}

__m256i avx::loadu_si48(const void *ptr) noexcept {
    return simd_cast<__m128i_t, __m256i_t>(sse::loadu_si48(ptr));
}

__m256i avx::loadu_si64(const void *ptr) noexcept {
    return simd_cast<__m128i_t, __m256i_t>(sse::loadu_si64(ptr));
}

__m256i avx::loadu_si80(const void *ptr) noexcept {
    return simd_cast<__m128i_t, __m256i_t>(sse::loadu_si80(ptr));
}

__m256i avx::loadu_si96(const void *ptr) noexcept {
    return simd_cast<__m128i_t, __m256i_t>(sse::loadu_si96(ptr));
}

__m256i avx::loadu_si112(const void *ptr) noexcept {
    return simd_cast<__m128i_t, __m256i_t>(sse::loadu_si112(ptr));
}

__m256i avx::loadu_si128(const void *ptr) noexcept {
    return simd_cast<__m128i_t, __m256i_t>(sse::loadu_si128(ptr));
}

__m256i avx::loadu_si144(const void *ptr) noexcept {
    return concat(sse::loadu_si128(ptr), sse::loadu_si16(static_cast<const char *>(ptr) + 16));
}

__m256i avx::loadu_si160(const void *ptr) noexcept {
    return concat(sse::loadu_si128(ptr), sse::loadu_si32(static_cast<const char *>(ptr) + 16));
}

__m256i avx::loadu_si176(const void *ptr) noexcept {
    return concat(sse::loadu_si128(ptr), sse::loadu_si48(static_cast<const char *>(ptr) + 16));
}

__m256i avx::loadu_si192(const void *ptr) noexcept {
    return concat(sse::loadu_si128(ptr), sse::loadu_si64(static_cast<const char *>(ptr) + 16));
}

__m256i avx::loadu_si208(const void *ptr) noexcept {
    return concat(sse::loadu_si128(ptr), sse::loadu_si80(static_cast<const char *>(ptr) + 16));
}

__m256i avx::loadu_si224(const void *ptr) noexcept {
    return concat(sse::loadu_si128(ptr), sse::loadu_si96(static_cast<const char *>(ptr) + 16));
}
__m256i avx::loadu_si240(const void *ptr) noexcept {
    return concat(sse::loadu_si128(ptr), sse::loadu_si112(static_cast<const char *>(ptr) + 16));
}

__m256i avx::loadu_si256(const void *ptr) noexcept {
    return loadu(static_cast<const __m256i *>(ptr));
}

__m256i avx::loadu_si16x(const void *ptr, int n) noexcept {
    switch (n) {
    case 0:
        return zeros();
    case 1:
        return loadu_si16(ptr);
    case 2:
        return loadu_si32(ptr);
    case 3:
        return loadu_si48(ptr);
    case 4:
        return loadu_si64(ptr);
    case 5:
        return loadu_si80(ptr);
    case 6:
        return loadu_si96(ptr);
    case 7:
        return loadu_si112(ptr);
    case 8:
        return loadu_si128(ptr);
    case 9:
        return loadu_si144(ptr);
    case 10:
        return loadu_si160(ptr);
    case 11:
        return loadu_si176(ptr);
    case 12:
        return loadu_si192(ptr);
    case 13:
        return loadu_si208(ptr);
    case 14:
        return loadu_si224(ptr);
    case 15:
        return loadu_si240(ptr);
    default:
        return loadu_si256(ptr);
    }
}

__m256i avx::set_epi8(char e31, char e30, char e29, char e28, char e27, char e26, char e25,
                      char e24, char e23, char e22, char e21, char e20, char e19, char e18,
                      char e17, char e16, char e15, char e14, char e13, char e12, char e11,
                      char e10, char e9, char e8, char e7, char e6, char e5, char e4, char e3,
                      char e2, char e1, char e0) noexcept {
    return _mm256_set_epi8(e31, e30, e29, e28, e27, e26, e25, e24, e23, e22, e21, e20, e19, e18,
                           e17, e16, e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2,
                           e1, e0);
}

__m256i avx::set_epi16(short e15, short e14, short e13, short e12, short e11, short e10, short e9,
                       short e8, short e7, short e6, short e5, short e4, short e3, short e2,
                       short e1, short e0) noexcept {
    return _mm256_set_epi16(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
}

__m256i avx::set_epi32(int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0) noexcept {
    return _mm256_set_epi32(e7, e6, e5, e4, e3, e2, e1, e0);
}

__m256i avx::set_epi64x(long long e3, long long e2, long long e1, long long e0) noexcept {
    return _mm256_set_epi64x(e3, e2, e1, e0);
}

__m256i avx::setr_epi8(char e31, char e30, char e29, char e28, char e27, char e26, char e25,
                       char e24, char e23, char e22, char e21, char e20, char e19, char e18,
                       char e17, char e16, char e15, char e14, char e13, char e12, char e11,
                       char e10, char e9, char e8, char e7, char e6, char e5, char e4, char e3,
                       char e2, char e1, char e0) noexcept {
    return _mm256_setr_epi8(e31, e30, e29, e28, e27, e26, e25, e24, e23, e22, e21, e20, e19, e18,
                            e17, e16, e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2,
                            e1, e0);
}

__m256i avx::setr_epi16(short e15, short e14, short e13, short e12, short e11, short e10, short e9,
                        short e8, short e7, short e6, short e5, short e4, short e3, short e2,
                        short e1, short e0) noexcept {
    return _mm256_setr_epi16(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
}

__m256i avx::setr_epi32(int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0) noexcept {
    return _mm256_setr_epi32(e7, e6, e5, e4, e3, e2, e1, e0);
}

__m256i avx::setr_epi64x(long long e3, long long e2, long long e1, long long e0) noexcept {
    return _mm256_setr_epi64x(e3, e2, e1, e0);
}

__m256i avx::set1_epi8(int8_t a) noexcept { return _mm256_set1_epi8(a); }
__m256i avx::set1_epi16(int16_t a) noexcept { return _mm256_set1_epi16(a); }
__m256i avx::set1_epi32(int32_t a) noexcept { return _mm256_set1_epi32(a); }
__m256i avx::set1_epi64(int64_t a) noexcept { return _mm256_set1_epi64x(a); }

__m256i avx::set1(int8_t a, int8_t) noexcept { return set1_epi8(a); }
__m256i avx::set1(int16_t a, int16_t) noexcept { return set1_epi16(a); }
__m256i avx::set1(int32_t a, int32_t) noexcept { return set1_epi32(a); }
__m256i avx::set1(int64_t a, int64_t) noexcept { return set1_epi64(a); }
__m256i avx::set1(uint8_t a, uint8_t) noexcept { return set1_epi8(a); }
__m256i avx::set1(uint16_t a, uint16_t) noexcept { return set1_epi16(a); }
__m256i avx::set1(uint32_t a, uint32_t) noexcept { return set1_epi32(a); }
__m256i avx::set1(uint64_t a, uint64_t) noexcept { return set1_epi64(a); }

__m256i avx::setmin_epi8() noexcept { return set1_epi8(0x80u); }
__m256i avx::setmin_epi16() noexcept { return set1_epi16(0x8000u); }
__m256i avx::setmin_epi32() noexcept { return set1_epi32(0x80000000u); }
__m256i avx::setmin_epi64() noexcept { return set1_epi64(0x8000000000000000ull); }

__m256i avx::setmin(int8_t) noexcept { return setmin_epi8(); }
__m256i avx::setmin(int16_t) noexcept { return setmin_epi16(); }
__m256i avx::setmin(int32_t) noexcept { return setmin_epi32(); }
__m256i avx::setmin(int64_t) noexcept { return setmin_epi64(); }

__m256i avx::setmax_epi8() noexcept { return set1_epi8(0x7f); }
__m256i avx::setmax_epi16() noexcept { return set1_epi16(0x7fff); }
__m256i avx::setmax_epi32() noexcept { return set1_epi32(0x7fffffff); }
__m256i avx::setmax_epi64() noexcept { return set1_epi64(0x7fffffffffffffff); }

__m256i avx::setmax(int8_t) noexcept { return setmax_epi8(); }
__m256i avx::setmax(int16_t) noexcept { return setmax_epi16(); }
__m256i avx::setmax(int32_t) noexcept { return setmax_epi32(); }
__m256i avx::setmax(int64_t) noexcept { return setmax_epi64(); }

void avx::stream(void *p, __m256i a) noexcept { _mm256_stream_si256(static_cast<__m256i *>(p), a); }
void avx::store(void *p, __m256i a) noexcept { _mm256_store_si256(static_cast<__m256i *>(p), a); }
void avx::storeu(void *p, __m256i a) noexcept { _mm256_storeu_si256(static_cast<__m256i *>(p), a); }

int avx::test_all_zeros(__m256i a) noexcept { return testz(a, a); }

int avx::testc(__m256i a, __m256i b) noexcept { return _mm256_testc_si256(a, b); }

int avx::testnzc(__m256i a, __m256i b) noexcept { return _mm256_testnzc_si256(a, b); }

int avx::testz(__m256i a, __m256i b) noexcept { return _mm256_testz_si256(a, b); }

__m256i avx::zeros() noexcept { return _mm256_setzero_si256(); }

#endif

#if WJR_HAS_SIMD(AVX2)

__m256i avx::And(__m256i a, __m256i b) noexcept { return _mm256_and_si256(a, b); }

__m256i avx::AndNot(__m256i a, __m256i b) noexcept { return _mm256_andnot_si256(a, b); }

__m256i avx::Or(__m256i a, __m256i b) noexcept { return _mm256_or_si256(a, b); }

__m256i avx::Xor(__m256i a, __m256i b) noexcept { return _mm256_xor_si256(a, b); }

__m256i avx::Not(__m256i v) noexcept { return _mm256_xor_si256(v, ones()); }

__m256i avx::abs_epi8(__m256i v) noexcept { return _mm256_abs_epi8(v); }
__m256i avx::abs_epi16(__m256i v) noexcept { return _mm256_abs_epi16(v); }
__m256i avx::abs_epi32(__m256i v) noexcept { return _mm256_abs_epi32(v); }

__m256i avx::abs(__m256i v, int8_t) noexcept { return abs_epi8(v); }
__m256i avx::abs(__m256i v, int16_t) noexcept { return abs_epi16(v); }
__m256i avx::abs(__m256i v, int32_t) noexcept { return abs_epi32(v); }
__m256i avx::abs(__m256i v, int64_t) noexcept { return abs_epi32(v); }

__m256i avx::add_epi8(__m256i a, __m256i b) noexcept { return _mm256_add_epi8(a, b); }
__m256i avx::add_epi16(__m256i a, __m256i b) noexcept { return _mm256_add_epi16(a, b); }
__m256i avx::add_epi32(__m256i a, __m256i b) noexcept { return _mm256_add_epi32(a, b); }
__m256i avx::add_epi64(__m256i a, __m256i b) noexcept { return _mm256_add_epi64(a, b); }

__m256i avx::add(__m256i a, __m256i b, int8_t) noexcept { return add_epi8(a, b); }
__m256i avx::add(__m256i a, __m256i b, int16_t) noexcept { return add_epi16(a, b); }
__m256i avx::add(__m256i a, __m256i b, int32_t) noexcept { return add_epi32(a, b); }
__m256i avx::add(__m256i a, __m256i b, int64_t) noexcept { return add_epi64(a, b); }
__m256i avx::add(__m256i a, __m256i b, uint8_t) noexcept { return add_epi8(a, b); }
__m256i avx::add(__m256i a, __m256i b, uint16_t) noexcept { return add_epi16(a, b); }
__m256i avx::add(__m256i a, __m256i b, uint32_t) noexcept { return add_epi32(a, b); }
__m256i avx::add(__m256i a, __m256i b, uint64_t) noexcept { return add_epi64(a, b); }

uint8_t avx::add_epu8(__m256i v) noexcept {
    return sse::add_epu8(sse::add_epi8(getlow(v), gethigh(v)));
}

uint16_t avx::add_epu16(__m256i v) noexcept {
    return sse::add_epu16(sse::add_epi16(getlow(v), gethigh(v)));
}

uint32_t avx::add_epu32(__m256i v) noexcept {
    return sse::add_epu32(sse::add_epi32(getlow(v), gethigh(v)));
}

uint64_t avx::add_epu64(__m256i v) noexcept {
    return sse::add_epu64(sse::add_epi64(getlow(v), gethigh(v)));
}

int8_t avx::add_epi8(__m256i v) noexcept { return add_epu8(v); }
int16_t avx::add_epi16(__m256i v) noexcept { return add_epu16(v); }
int32_t avx::add_epi32(__m256i v) noexcept { return add_epu32(v); }
int64_t avx::add_epi64(__m256i v) noexcept { return add_epu64(v); }

int8_t avx::add(__m256i v, int8_t) noexcept { return add_epi8(v); }
int16_t avx::add(__m256i v, int16_t) noexcept { return add_epi16(v); }
int32_t avx::add(__m256i v, int32_t) noexcept { return add_epi32(v); }
int64_t avx::add(__m256i v, int64_t) noexcept { return add_epi64(v); }
uint8_t avx::add(__m256i v, uint8_t) noexcept { return add_epu8(v); }
uint16_t avx::add(__m256i v, uint16_t) noexcept { return add_epu16(v); }
uint32_t avx::add(__m256i v, uint32_t) noexcept { return add_epu32(v); }
uint64_t avx::add(__m256i v, uint64_t) noexcept { return add_epu64(v); }

__m256i avx::adds_epi8(__m256i a, __m256i b) noexcept { return _mm256_adds_epi8(a, b); }
__m256i avx::adds_epi16(__m256i a, __m256i b) noexcept { return _mm256_adds_epi16(a, b); }

__m256i avx::adds_epu8(__m256i a, __m256i b) noexcept { return _mm256_adds_epu8(a, b); }
__m256i avx::adds_epu16(__m256i a, __m256i b) noexcept { return _mm256_adds_epu16(a, b); }

__m256i avx::adds(__m256i a, __m256i b, int8_t) noexcept { return adds_epi8(a, b); }
__m256i avx::adds(__m256i a, __m256i b, int16_t) noexcept { return adds_epi16(a, b); }
__m256i avx::adds(__m256i a, __m256i b, uint8_t) noexcept { return adds_epu8(a, b); }
__m256i avx::adds(__m256i a, __m256i b, uint16_t) noexcept { return adds_epu16(a, b); }

template <int imm8>
__m256i avx::alignr(__m256i a, __m256i b) noexcept {
    return _mm256_alignr_epi8(a, b, imm8);
}

__m256i avx::alignr_epi16(__m256i a, __m256i b, int c) noexcept {
    return Or(slli_epi16(a, 16 - c), srli_epi16(b, c));
}

__m256i avx::alignr_epi32(__m256i a, __m256i b, int c) noexcept {
    return Or(slli_epi32(a, 32 - c), srli_epi32(b, c));
}

__m256i avx::alignr_epi64(__m256i a, __m256i b, int c) noexcept {
    return Or(slli_epi64(a, 64 - c), srli_epi64(b, c));
}

__m256i avx::alignr(__m256i a, __m256i b, int c, int16_t) noexcept { return alignr_epi16(a, b, c); }

__m256i avx::alignr(__m256i a, __m256i b, int c, int32_t) noexcept { return alignr_epi32(a, b, c); }

__m256i avx::alignr(__m256i a, __m256i b, int c, int64_t) noexcept { return alignr_epi64(a, b, c); }

__m256i avx::alignr(__m256i a, __m256i b, int c, uint16_t) noexcept {
    return alignr_epi16(a, b, c);
}

__m256i avx::alignr(__m256i a, __m256i b, int c, uint32_t) noexcept {
    return alignr_epi32(a, b, c);
}

__m256i avx::alignr(__m256i a, __m256i b, int c, uint64_t) noexcept {
    return alignr_epi64(a, b, c);
}

__m256i avx::avg_epu8(__m256i a, __m256i b) noexcept { return _mm256_avg_epu8(a, b); }
__m256i avx::avg_epu16(__m256i a, __m256i b) noexcept { return _mm256_avg_epu16(a, b); }

__m256i avx::avg(__m256i a, __m256i b, int8_t) noexcept { return avg_epu8(a, b); }
__m256i avx::avg(__m256i a, __m256i b, int16_t) noexcept { return avg_epu16(a, b); }
__m256i avx::avg(__m256i a, __m256i b, uint8_t) noexcept { return avg_epu8(a, b); }
__m256i avx::avg(__m256i a, __m256i b, uint16_t) noexcept { return avg_epu16(a, b); }

template <int imm8>
__m256i avx::blend_epi16(__m256i a, __m256i b) noexcept {
    return _mm256_blend_epi16(a, b, imm8);
}

template <int imm8>
__m256i avx::blend_epi32(__m256i a, __m256i b) noexcept {
    return _mm256_blend_epi32(a, b, imm8);
}

__m256i avx::blendv_epi8(__m256i a, __m256i b, __m256i mask) noexcept {
    return _mm256_blendv_epi8(a, b, mask);
}

template <int imm8>
__m256i avx::bslli_epi128(__m256i a) noexcept {
    return _mm256_bslli_epi128(a, imm8);
}

template <int imm8>
__m256i avx::bsrli_epi128(__m256i a) noexcept {
    return _mm256_bsrli_epi128(a, imm8);
}

__m256i avx::cmpeq_epi8(__m256i a, __m256i b) noexcept { return _mm256_cmpeq_epi8(a, b); }
__m256i avx::cmpeq_epi16(__m256i a, __m256i b) noexcept { return _mm256_cmpeq_epi16(a, b); }
__m256i avx::cmpeq_epi32(__m256i a, __m256i b) noexcept { return _mm256_cmpeq_epi32(a, b); }
__m256i avx::cmpeq_epi64(__m256i a, __m256i b) noexcept { return _mm256_cmpeq_epi64(a, b); }

__m256i avx::cmpeq(__m256i a, __m256i b, int8_t) noexcept { return cmpeq_epi8(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, int16_t) noexcept { return cmpeq_epi16(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, int32_t) noexcept { return cmpeq_epi32(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, int64_t) noexcept { return cmpeq_epi64(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, uint8_t) noexcept { return cmpeq_epi8(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, uint16_t) noexcept { return cmpeq_epi16(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, uint32_t) noexcept { return cmpeq_epi32(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, uint64_t) noexcept { return cmpeq_epi64(a, b); }

__m256i avx::cmpge_epi8(__m256i a, __m256i b) noexcept {
    return cmpeq(min(a, b, int8_t()), b, uint8_t());
}

__m256i avx::cmpge_epi16(__m256i a, __m256i b) noexcept {
    return cmpeq(min(a, b, int16_t()), b, uint16_t());
}

__m256i avx::cmpge_epi32(__m256i a, __m256i b) noexcept {
    return cmpeq(min(a, b, int32_t()), b, uint8_t());
}

__m256i avx::cmpge_epu8(__m256i a, __m256i b) noexcept {
    return cmpeq(min(a, b, uint8_t()), b, uint8_t());
}

__m256i avx::cmpge_epu16(__m256i a, __m256i b) noexcept {
    return cmpeq(min(a, b, uint16_t()), b, uint16_t());
}

__m256i avx::cmpge_epu32(__m256i a, __m256i b) noexcept {
    return cmpeq(min(a, b, uint32_t()), b, uint32_t());
}

__m256i avx::cmpge(__m256i a, __m256i b, int8_t) noexcept { return cmpge_epi8(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, int16_t) noexcept { return cmpge_epi16(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, int32_t) noexcept { return cmpge_epi32(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, uint8_t) noexcept { return cmpge_epu8(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, uint16_t) noexcept { return cmpge_epu16(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, uint32_t) noexcept { return cmpge_epu32(a, b); }

__m256i avx::cmpgt_epi8(__m256i a, __m256i b) noexcept { return _mm256_cmpgt_epi8(a, b); }
__m256i avx::cmpgt_epi16(__m256i a, __m256i b) noexcept { return _mm256_cmpgt_epi16(a, b); }
__m256i avx::cmpgt_epi32(__m256i a, __m256i b) noexcept { return _mm256_cmpgt_epi32(a, b); }
__m256i avx::cmpgt_epi64(__m256i a, __m256i b) noexcept { return _mm256_cmpgt_epi64(a, b); }

__m256i avx::cmpgt_epu8(__m256i a, __m256i b) noexcept {
    return cmpgt_epi8(Xor(a, setmin_epi8()), Xor(b, setmin_epi8()));
}

__m256i avx::cmpgt_epu16(__m256i a, __m256i b) noexcept {
    return cmpgt_epi16(Xor(a, setmin_epi16()), Xor(b, setmin_epi16()));
}

__m256i avx::cmpgt_epu32(__m256i a, __m256i b) noexcept {
    return cmpgt_epi32(Xor(a, setmin_epi32()), Xor(b, setmin_epi32()));
}

__m256i avx::cmpgt_epu64(__m256i a, __m256i b) noexcept {
    return cmpgt_epi64(Xor(a, setmin_epi64()), Xor(b, setmin_epi64()));
}

__m256i avx::cmpgt(__m256i a, __m256i b, int8_t) noexcept { return cmpgt_epi8(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, int16_t) noexcept { return cmpgt_epi16(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, int32_t) noexcept { return cmpgt_epi32(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, int64_t) noexcept { return cmpgt_epi64(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, uint8_t) noexcept { return cmpgt_epu8(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, uint16_t) noexcept { return cmpgt_epu16(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, uint32_t) noexcept { return cmpgt_epu32(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, uint64_t) noexcept { return cmpgt_epu64(a, b); }

__m256i avx::cmple_epi8(__m256i a, __m256i b) noexcept { return cmpge_epi8(b, a); }

__m256i avx::cmple_epi16(__m256i a, __m256i b) noexcept { return cmpge_epi16(b, a); }

__m256i avx::cmple_epi32(__m256i a, __m256i b) noexcept { return cmpge_epi32(b, a); }

__m256i avx::cmple_epu8(__m256i a, __m256i b) noexcept { return cmpge_epu8(b, a); }

__m256i avx::cmple_epu16(__m256i a, __m256i b) noexcept { return cmpge_epu16(b, a); }

__m256i avx::cmple_epu32(__m256i a, __m256i b) noexcept { return cmpge_epu32(b, a); }

__m256i avx::cmple(__m256i a, __m256i b, int8_t) noexcept { return cmple_epi8(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, int16_t) noexcept { return cmple_epi16(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, int32_t) noexcept { return cmple_epi32(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, uint8_t) noexcept { return cmple_epu8(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, uint16_t) noexcept { return cmple_epu16(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, uint32_t) noexcept { return cmple_epu32(a, b); }

__m256i avx::cmplt_epi8(__m256i a, __m256i b) noexcept { return cmpgt_epi8(b, a); }
__m256i avx::cmplt_epi16(__m256i a, __m256i b) noexcept { return cmpgt_epi16(b, a); }
__m256i avx::cmplt_epi32(__m256i a, __m256i b) noexcept { return cmpgt_epi32(b, a); }

__m256i avx::cmplt_epu8(__m256i a, __m256i b) noexcept { return cmpgt_epu8(b, a); }
__m256i avx::cmplt_epu16(__m256i a, __m256i b) noexcept { return cmpgt_epu16(b, a); }
__m256i avx::cmplt_epu32(__m256i a, __m256i b) noexcept { return cmpgt_epu32(b, a); }

__m256i avx::cmplt(__m256i a, __m256i b, int8_t) noexcept { return cmplt_epi8(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, int16_t) noexcept { return cmplt_epi16(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, int32_t) noexcept { return cmplt_epi32(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, uint8_t) noexcept { return cmplt_epu8(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, uint16_t) noexcept { return cmplt_epu16(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, uint32_t) noexcept { return cmplt_epu32(a, b); }

__m256i avx::cmpne_epi8(__m256i a, __m256i b) noexcept { return Not(cmpeq_epi8(a, b)); }
__m256i avx::cmpne_epi16(__m256i a, __m256i b) noexcept { return Not(cmpeq_epi16(a, b)); }
__m256i avx::cmpne_epi32(__m256i a, __m256i b) noexcept { return Not(cmpeq_epi32(a, b)); }

__m256i avx::cmpne(__m256i a, __m256i b, int8_t) noexcept { return cmpne_epi8(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, int16_t) noexcept { return cmpne_epi16(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, int32_t) noexcept { return cmpne_epi32(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, uint8_t) noexcept { return cmpne_epi8(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, uint16_t) noexcept { return cmpne_epi16(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, uint32_t) noexcept { return cmpne_epi32(a, b); }

template <typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::equal_to<>, T) noexcept {
    return cmpeq(a, b, T());
}

template <typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::not_equal_to<>, T) noexcept {
    return cmpne(a, b, T());
}

template <typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::greater<>, T) noexcept {
    return cmpgt(a, b, T());
}

template <typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::greater_equal<>, T) noexcept {
    return cmpge(a, b, T());
}

template <typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::less<>, T) noexcept {
    return cmplt(a, b, T());
}

template <typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::less_equal<>, T) noexcept {
    return cmple(a, b, T());
}

template <int imm8>
int avx::extract_epi8(__m256i v) noexcept {
    return _mm256_extract_epi8(v, imm8);
}

template <int imm8>
int avx::extract_epi16(__m256i v) noexcept {
    return _mm256_extract_epi16(v, imm8);
}

template <int imm8>
int avx::extract(__m256i v, int8_t) noexcept {
    return extract_epi8<imm8>(v);
}

template <int imm8>
int avx::extract(__m256i v, int16_t) noexcept {
    return extract_epi16<imm8>(v);
}

__m256i avx::hadd_epi16(__m256i a, __m256i b) noexcept { return _mm256_hadd_epi16(a, b); }
__m256i avx::hadd_epi32(__m256i a, __m256i b) noexcept { return _mm256_hadd_epi32(a, b); }

__m256i avx::hadd(__m256i a, __m256i b, int16_t) noexcept { return hadd_epi16(a, b); }
__m256i avx::hadd(__m256i a, __m256i b, int32_t) noexcept { return hadd_epi32(a, b); }

__m256i avx::hadds_epi16(__m256i a, __m256i b) noexcept { return _mm256_hadds_epi16(a, b); }

__m256i avx::hsub_epi16(__m256i a, __m256i b) noexcept { return _mm256_hsub_epi16(a, b); }
__m256i avx::hsub_epi32(__m256i a, __m256i b) noexcept { return _mm256_hsub_epi32(a, b); }

__m256i avx::hsub(__m256i a, __m256i b, int16_t) noexcept { return hsub_epi16(a, b); }
__m256i avx::hsub(__m256i a, __m256i b, int32_t) noexcept { return hsub_epi32(a, b); }

__m256i avx::hsubs_epi16(__m256i a, __m256i b) noexcept { return _mm256_hsubs_epi16(a, b); }

template <typename T, WJR_REQUIRES_I(is_any_of_v<T, int8_t, int16_t, int32_t, int64_t, uint8_t,
                                                 uint16_t, uint32_t, uint64_t>)>
__m256i avx::logical_and(__m256i a, __m256i b, T) noexcept {
    return Not(Or(logical_not(a, T()), logical_not(b, T())));
}

template <typename T, WJR_REQUIRES_I(is_any_of_v<T, int8_t, int16_t, int32_t, int64_t, uint8_t,
                                                 uint16_t, uint32_t, uint64_t>)>
__m256i avx::logical_not(__m256i v, T) noexcept {
    auto Zero = zeros();
    return cmpeq(v, Zero, T());
}

template <typename T, WJR_REQUIRES_I(is_any_of_v<T, int8_t, int16_t, int32_t, int64_t, uint8_t,
                                                 uint16_t, uint32_t, uint64_t>)>
__m256i avx::logical_or(__m256i a, __m256i b, T) noexcept {
    return Not(logical_not(Or(a, b), T()));
}

__m256i avx::madd_epi16(__m256i a, __m256i b) noexcept { return _mm256_madd_epi16(a, b); }

__m256i avx::max_epi8(__m256i a, __m256i b) noexcept { return _mm256_max_epi8(a, b); }
__m256i avx::max_epi16(__m256i a, __m256i b) noexcept { return _mm256_max_epi16(a, b); }
__m256i avx::max_epi32(__m256i a, __m256i b) noexcept { return _mm256_max_epi32(a, b); }

__m256i avx::max_epu8(__m256i a, __m256i b) noexcept { return _mm256_max_epu8(a, b); }
__m256i avx::max_epu16(__m256i a, __m256i b) noexcept { return _mm256_max_epu16(a, b); }
__m256i avx::max_epu32(__m256i a, __m256i b) noexcept { return _mm256_max_epu32(a, b); }

__m256i avx::max(__m256i a, __m256i b, int8_t) noexcept { return max_epi8(a, b); }
__m256i avx::max(__m256i a, __m256i b, int16_t) noexcept { return max_epi16(a, b); }
__m256i avx::max(__m256i a, __m256i b, uint8_t) noexcept { return max_epu8(a, b); }
__m256i avx::max(__m256i a, __m256i b, uint16_t) noexcept { return max_epu16(a, b); }
__m256i avx::max(__m256i a, __m256i b, int32_t) noexcept { return max_epi32(a, b); }
__m256i avx::max(__m256i a, __m256i b, uint32_t) noexcept { return max_epu32(a, b); }

int8_t avx::max_epi8(__m256i a) noexcept {
    return sse::max_epi8(sse::max_epi8(getlow(a), gethigh(a)));
}

int16_t avx::max_epi16(__m256i a) noexcept {
    return sse::max_epi16(sse::max_epi16(getlow(a), gethigh(a)));
}

int32_t avx::max_epi32(__m256i a) noexcept {
    return sse::max_epi32(sse::max_epi32(getlow(a), gethigh(a)));
}

uint8_t avx::max_epu8(__m256i a) noexcept {
    return sse::max_epu8(sse::max_epu8(getlow(a), gethigh(a)));
}

uint16_t avx::max_epu16(__m256i a) noexcept {
    return sse::max_epu16(sse::max_epu16(getlow(a), gethigh(a)));
}

uint32_t avx::max_epu32(__m256i a) noexcept {
    return sse::max_epu32(sse::max_epu32(getlow(a), gethigh(a)));
}

int8_t avx::max(__m256i a, int8_t) noexcept { return max_epi8(a); }
int16_t avx::max(__m256i a, int16_t) noexcept { return max_epi16(a); }
int32_t avx::max(__m256i a, int32_t) noexcept { return max_epi32(a); }
uint8_t avx::max(__m256i a, uint8_t) noexcept { return max_epu8(a); }
uint16_t avx::max(__m256i a, uint16_t) noexcept { return max_epu16(a); }
uint32_t avx::max(__m256i a, uint32_t) noexcept { return max_epu32(a); }

__m256i avx::min_epi8(__m256i a, __m256i b) noexcept { return _mm256_min_epi8(a, b); }
__m256i avx::min_epi16(__m256i a, __m256i b) noexcept { return _mm256_min_epi16(a, b); }
__m256i avx::min_epi32(__m256i a, __m256i b) noexcept { return _mm256_min_epi32(a, b); }

__m256i avx::min_epu8(__m256i a, __m256i b) noexcept { return _mm256_min_epu8(a, b); }
__m256i avx::min_epu16(__m256i a, __m256i b) noexcept { return _mm256_min_epu16(a, b); }
__m256i avx::min_epu32(__m256i a, __m256i b) noexcept { return _mm256_min_epu32(a, b); }

__m256i avx::min(__m256i a, __m256i b, int8_t) noexcept { return min_epi8(a, b); }
__m256i avx::min(__m256i a, __m256i b, int16_t) noexcept { return min_epi16(a, b); }
__m256i avx::min(__m256i a, __m256i b, uint8_t) noexcept { return min_epu8(a, b); }
__m256i avx::min(__m256i a, __m256i b, uint16_t) noexcept { return min_epu16(a, b); }
__m256i avx::min(__m256i a, __m256i b, int32_t) noexcept { return min_epi32(a, b); }
__m256i avx::min(__m256i a, __m256i b, uint32_t) noexcept { return min_epu32(a, b); }

int8_t avx::min_epi8(__m256i a) noexcept {
    return sse::min_epi8(sse::min_epi8(getlow(a), gethigh(a)));
}

int16_t avx::min_epi16(__m256i a) noexcept {
    return sse::min_epi16(sse::min_epi16(getlow(a), gethigh(a)));
}

int32_t avx::min_epi32(__m256i a) noexcept {
    return sse::min_epi32(sse::min_epi32(getlow(a), gethigh(a)));
}

uint8_t avx::min_epu8(__m256i a) noexcept {
    return sse::min_epu8(sse::min_epu8(getlow(a), gethigh(a)));
}

uint16_t avx::min_epu16(__m256i a) noexcept {
    return sse::min_epu16(sse::min_epu16(getlow(a), gethigh(a)));
}

uint32_t avx::min_epu32(__m256i a) noexcept {
    return sse::min_epu32(sse::min_epu32(getlow(a), gethigh(a)));
}

int8_t avx::min(__m256i a, int8_t) noexcept { return min_epi8(a); }
int16_t avx::min(__m256i a, int16_t) noexcept { return min_epi16(a); }
int32_t avx::min(__m256i a, int32_t) noexcept { return min_epi32(a); }
uint8_t avx::min(__m256i a, uint8_t) noexcept { return min_epu8(a); }
uint16_t avx::min(__m256i a, uint16_t) noexcept { return min_epu16(a); }
uint32_t avx::min(__m256i a, uint32_t) noexcept { return min_epu32(a); }

avx::mask_type avx::movemask_epi8(__m256i a) noexcept { return _mm256_movemask_epi8(a); }

__m256i avx::mul_epi32(__m256i a, __m256i b) noexcept { return _mm256_mul_epi32(a, b); }
__m256i avx::mul_epu32(__m256i a, __m256i b) noexcept { return _mm256_mul_epu32(a, b); }

__m256i avx::mulhi_epi16(__m256i a, __m256i b) noexcept { return _mm256_mulhi_epi16(a, b); }

__m256i avx::mulhi_epu16(__m256i a, __m256i b) noexcept { return _mm256_mulhi_epu16(a, b); }

__m256i avx::mullo_epi16(__m256i a, __m256i b) noexcept { return _mm256_mullo_epi16(a, b); }

__m256i avx::packs_epi16(__m256i a, __m256i b) noexcept { return _mm256_packs_epi16(a, b); }
__m256i avx::packs_epi32(__m256i a, __m256i b) noexcept { return _mm256_packs_epi32(a, b); }

__m256i avx::packus_epi16(__m256i a, __m256i b) noexcept { return _mm256_packus_epi16(a, b); }
__m256i avx::packus_epi32(__m256i a, __m256i b) noexcept { return _mm256_packus_epi32(a, b); }

template <int imm>
__m256i avx::shl(__m256i a) noexcept {
    if constexpr (imm >= 64 * 3) {
        a = slli<8 * 3>(a);
        a = slli_epi64(a, imm - 64 * 3);
        return a;
    } else if constexpr (imm >= 64 * 2) {
        a = slli<8 * 2>(a);
        constexpr auto I = imm - 64 * 2;
        auto b = slli_epi64(a, I);
        auto c = slli<8>(a);
        c = srli_epi64(c, 64 - I);
        return Or(b, c);
    } else if constexpr (imm >= 64) {
        a = slli<8>(a);
        constexpr auto I = imm - 64;
        auto b = slli_epi64(a, I);
        auto c = slli<8>(a);
        c = srli_epi64(c, 64 - I);
        return Or(b, c);
    } else {
        auto b = slli_epi64(a, imm);
        auto c = slli<8>(a);
        c = srli_epi64(c, 64 - imm);
        return Or(b, c);
    }
}

template <int imm>
__m256i avx::shr(__m256i a) noexcept {
    if constexpr (imm >= 64 * 3) {
        a = srli<8 * 3>(a);
        a = srli_epi64(a, imm - 64 * 3);
        return a;
    } else if constexpr (imm >= 64 * 2) {
        a = srli<8 * 2>(a);
        constexpr auto I = imm - 64 * 2;
        auto b = srli_epi64(a, I);
        auto c = srli<8>(a);
        c = slli_epi64(c, 64 - I);
        return Or(b, c);
    } else if constexpr (imm >= 64) {
        a = srli<8>(a);
        constexpr auto I = imm - 64;
        auto b = srli_epi64(a, I);
        auto c = srli<8>(a);
        c = slli_epi64(c, 64 - I);
        return Or(b, c);
    } else {
        auto b = srli_epi64(a, imm);
        auto c = srli<8>(a);
        c = slli_epi64(c, 64 - imm);
        return Or(b, c);
    }
}

__m256i avx::shuffle_epi8(__m256i a, __m256i b) noexcept { return _mm256_shuffle_epi8(a, b); }

template <int imm8>
__m256i avx::shuffle_epi32(__m256i a) noexcept {
    return _mm256_shuffle_epi32(a, imm8);
}

template <int imm8>
__m256i avx::shufflehi_epi16(__m256i a) noexcept {
    return _mm256_shufflehi_epi16(a, imm8);
}

template <int imm8>
__m256i avx::shufflelo_epi16(__m256i a) noexcept {
    return _mm256_shufflelo_epi16(a, imm8);
}

__m256i avx::sll_epi16(__m256i a, __m128i b) noexcept { return _mm256_sll_epi16(a, b); }
__m256i avx::sll_epi32(__m256i a, __m128i b) noexcept { return _mm256_sll_epi32(a, b); }
__m256i avx::sll_epi64(__m256i a, __m128i b) noexcept { return _mm256_sll_epi64(a, b); }

__m256i avx::sll(__m256i a, __m128i b, int16_t) noexcept { return sll_epi16(a, b); }
__m256i avx::sll(__m256i a, __m128i b, int32_t) noexcept { return sll_epi32(a, b); }
__m256i avx::sll(__m256i a, __m128i b, int64_t) noexcept { return sll_epi64(a, b); }
__m256i avx::sll(__m256i a, __m128i b, uint16_t) noexcept { return sll_epi16(a, b); }
__m256i avx::sll(__m256i a, __m128i b, uint32_t) noexcept { return sll_epi32(a, b); }
__m256i avx::sll(__m256i a, __m128i b, uint64_t) noexcept { return sll_epi64(a, b); }

template <int imm8>
__m256i avx::slli(__m256i a) noexcept {
    return _mm256_slli_si256(a, imm8);
}
__m256i avx::slli_epi16(__m256i a, int imm8) noexcept { return _mm256_slli_epi16(a, imm8); }
__m256i avx::slli_epi32(__m256i a, int imm8) noexcept { return _mm256_slli_epi32(a, imm8); }
__m256i avx::slli_epi64(__m256i a, int imm8) noexcept { return _mm256_slli_epi64(a, imm8); }

__m256i avx::slli(__m256i a, int imm8, int16_t) noexcept { return slli_epi16(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, int32_t) noexcept { return slli_epi32(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, int64_t) noexcept { return slli_epi64(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, uint16_t) noexcept { return slli_epi16(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, uint32_t) noexcept { return slli_epi32(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, uint64_t) noexcept { return slli_epi64(a, imm8); }

__m256i avx::sra_epi16(__m256i a, __m128i b) noexcept { return _mm256_sra_epi16(a, b); }
__m256i avx::sra_epi32(__m256i a, __m128i b) noexcept { return _mm256_sra_epi32(a, b); }

__m256i avx::sra(__m256i a, __m128i b, int16_t) noexcept { return sra_epi16(a, b); }
__m256i avx::sra(__m256i a, __m128i b, int32_t) noexcept { return sra_epi32(a, b); }

__m256i avx::srai_epi16(__m256i a, int imm8) noexcept { return _mm256_srai_epi16(a, imm8); }
__m256i avx::srai_epi32(__m256i a, int imm8) noexcept { return _mm256_srai_epi32(a, imm8); }

__m256i avx::srai(__m256i a, int imm8, int16_t) noexcept { return srai_epi16(a, imm8); }
__m256i avx::srai(__m256i a, int imm8, int32_t) noexcept { return srai_epi32(a, imm8); }

__m256i avx::stream_load(const void *p) noexcept {
    return _mm256_stream_load_si256(static_cast<const __m256i *>(p));
}

__m256i avx::srl_epi16(__m256i a, __m128i b) noexcept { return _mm256_srl_epi16(a, b); }
__m256i avx::srl_epi32(__m256i a, __m128i b) noexcept { return _mm256_srl_epi32(a, b); }
__m256i avx::srl_epi64(__m256i a, __m128i b) noexcept { return _mm256_srl_epi64(a, b); }

__m256i avx::srl(__m256i a, __m128i b, int16_t) noexcept { return srl_epi16(a, b); }
__m256i avx::srl(__m256i a, __m128i b, int32_t) noexcept { return srl_epi32(a, b); }
__m256i avx::srl(__m256i a, __m128i b, int64_t) noexcept { return srl_epi64(a, b); }
__m256i avx::srl(__m256i a, __m128i b, uint16_t) noexcept { return srl_epi16(a, b); }
__m256i avx::srl(__m256i a, __m128i b, uint32_t) noexcept { return srl_epi32(a, b); }
__m256i avx::srl(__m256i a, __m128i b, uint64_t) noexcept { return srl_epi64(a, b); }

template <int imm8>
__m256i avx::srli(__m256i a) noexcept {
    return _mm256_srli_si256(a, imm8);
}

__m256i avx::srli_epi8(__m256i a, int imm8) noexcept {
    return And(srli_epi16(a, imm8), avx_detail::srli_epi8_mask[imm8]);
}
__m256i avx::srli_epi16(__m256i a, int imm8) noexcept { return _mm256_srli_epi16(a, imm8); }
__m256i avx::srli_epi32(__m256i a, int imm8) noexcept { return _mm256_srli_epi32(a, imm8); }
__m256i avx::srli_epi64(__m256i a, int imm8) noexcept { return _mm256_srli_epi64(a, imm8); }

__m256i avx::srli(__m256i a, int imm8, int8_t) noexcept { return srli_epi8(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, int16_t) noexcept { return srli_epi16(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, int32_t) noexcept { return srli_epi32(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, int64_t) noexcept { return srli_epi64(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, uint8_t) noexcept { return srli_epi8(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, uint16_t) noexcept { return srli_epi16(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, uint32_t) noexcept { return srli_epi32(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, uint64_t) noexcept { return srli_epi64(a, imm8); }

__m256i avx::sub_epi8(__m256i a, __m256i b) noexcept { return _mm256_sub_epi8(a, b); }
__m256i avx::sub_epi16(__m256i a, __m256i b) noexcept { return _mm256_sub_epi16(a, b); }
__m256i avx::sub_epi32(__m256i a, __m256i b) noexcept { return _mm256_sub_epi32(a, b); }
__m256i avx::sub_epi64(__m256i a, __m256i b) noexcept { return _mm256_sub_epi64(a, b); }

__m256i avx::sub(__m256i a, __m256i b, int8_t) noexcept { return sub_epi8(a, b); }
__m256i avx::sub(__m256i a, __m256i b, int16_t) noexcept { return sub_epi16(a, b); }
__m256i avx::sub(__m256i a, __m256i b, int32_t) noexcept { return sub_epi32(a, b); }
__m256i avx::sub(__m256i a, __m256i b, int64_t) noexcept { return sub_epi64(a, b); }
__m256i avx::sub(__m256i a, __m256i b, uint8_t) noexcept { return sub_epi8(a, b); }
__m256i avx::sub(__m256i a, __m256i b, uint16_t) noexcept { return sub_epi16(a, b); }
__m256i avx::sub(__m256i a, __m256i b, uint32_t) noexcept { return sub_epi32(a, b); }
__m256i avx::sub(__m256i a, __m256i b, uint64_t) noexcept { return sub_epi64(a, b); }

__m256i avx::subs_epi8(__m256i a, __m256i b) noexcept { return _mm256_subs_epi8(a, b); }
__m256i avx::subs_epi16(__m256i a, __m256i b) noexcept { return _mm256_subs_epi16(a, b); }

__m256i avx::subs_epu8(__m256i a, __m256i b) noexcept { return _mm256_subs_epu8(a, b); }
__m256i avx::subs_epu16(__m256i a, __m256i b) noexcept { return _mm256_subs_epu16(a, b); }

__m256i avx::subs(__m256i a, __m256i b, int8_t) noexcept { return subs_epi8(a, b); }
__m256i avx::subs(__m256i a, __m256i b, int16_t) noexcept { return subs_epi16(a, b); }
__m256i avx::subs(__m256i a, __m256i b, uint8_t) noexcept { return subs_epu8(a, b); }
__m256i avx::subs(__m256i a, __m256i b, uint16_t) noexcept { return subs_epu16(a, b); }

int avx::test_all_ones(__m256i a) noexcept { return testc(a, cmpeq_epi32(a, a)); }

__m256i avx::unpackhi_epi8(__m256i a, __m256i b) noexcept { return _mm256_unpackhi_epi8(a, b); }
__m256i avx::unpackhi_epi16(__m256i a, __m256i b) noexcept { return _mm256_unpackhi_epi16(a, b); }
__m256i avx::unpackhi_epi32(__m256i a, __m256i b) noexcept { return _mm256_unpackhi_epi32(a, b); }
__m256i avx::unpackhi_epi64(__m256i a, __m256i b) noexcept { return _mm256_unpackhi_epi64(a, b); }

__m256i avx::unpackhi(__m256i a, __m256i b, int8_t) noexcept { return unpackhi_epi8(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, int16_t) noexcept { return unpackhi_epi16(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, int32_t) noexcept { return unpackhi_epi32(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, int64_t) noexcept { return unpackhi_epi64(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, uint8_t) noexcept { return unpackhi_epi8(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, uint16_t) noexcept { return unpackhi_epi16(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, uint32_t) noexcept { return unpackhi_epi32(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, uint64_t) noexcept { return unpackhi_epi64(a, b); }

__m256i avx::unpacklo_epi8(__m256i a, __m256i b) noexcept { return _mm256_unpacklo_epi8(a, b); }
__m256i avx::unpacklo_epi16(__m256i a, __m256i b) noexcept { return _mm256_unpacklo_epi16(a, b); }
__m256i avx::unpacklo_epi32(__m256i a, __m256i b) noexcept { return _mm256_unpacklo_epi32(a, b); }
__m256i avx::unpacklo_epi64(__m256i a, __m256i b) noexcept { return _mm256_unpacklo_epi64(a, b); }

__m256i avx::unpacklo(__m256i a, __m256i b, int8_t) noexcept { return unpacklo_epi8(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, int16_t) noexcept { return unpacklo_epi16(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, int32_t) noexcept { return unpacklo_epi32(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, int64_t) noexcept { return unpacklo_epi64(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, uint8_t) noexcept { return unpacklo_epi8(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, uint16_t) noexcept { return unpacklo_epi16(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, uint32_t) noexcept { return unpacklo_epi32(a, b); }

#endif

} // namespace wjr

#endif // WJR_ARCH_X86_SIMD_AVX_HPP__