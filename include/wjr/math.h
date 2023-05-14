#ifndef __WJR_MATH_H
#define __WJR_MATH_H

#include <wjr/asm/asm.h>

_WJR_BEGIN

#ifdef __cpp_lib_bit_cast
using std::bit_cast;
#else
template <
	typename To,
	typename From,
	std::enable_if_t<
	sizeof(From) == sizeof(To) && std::is_trivially_copyable<To>::value&&
	std::is_trivially_copyable<From>::value,
	int> = 0>
WJR_INTRINSIC_CONSTEXPR20 To bit_cast(const From & src) noexcept;
#endif

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
constexpr bool has_single_bit(T x) noexcept;

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int countl_zero(T x) noexcept;

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int countl_one(T x) noexcept;

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int countr_zero(T x) noexcept;

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int countr_one(T x) noexcept;

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int bit_width(T x) noexcept;

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) T bit_floor(T x) noexcept;

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) T bit_ceil(T x) noexcept;

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int popcount(T x) noexcept;

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR20) T adc(T a, T b, T c, T& d) noexcept;

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR20) T sbb(T a, T b, T c, T& d) noexcept;

template<typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR20) T byteswap(T x) noexcept;

template<endian to = endian::native, typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR20) T endian_convert(T x, endian from = endian::native) noexcept;

template<endian from, endian to, typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR20) T endian_convert(T x) noexcept;

template<typename T, endian to = endian::native>
WJR_INTRINSIC_INLINE T read_bytes(const void* ptr) noexcept;

template<typename T, endian to = endian::native>
WJR_INTRINSIC_INLINE void write_bytes(void* ptr, T val) noexcept;

template< typename T, typename U >
constexpr bool cmp_equal(T t, U u) noexcept;

template< typename T, typename U >
constexpr bool cmp_not_equal(T t, U u) noexcept;

template< typename T, typename U >
constexpr bool cmp_less(T t, U u) noexcept;

template< typename T, typename U >
constexpr bool cmp_greater(T t, U u) noexcept;

template< typename T, typename U >
constexpr bool cmp_less_equal(T t, U u) noexcept;

template< typename T, typename U >
constexpr bool cmp_greater_equal(T t, U u) noexcept;

template< typename R, typename T>
constexpr bool in_range(T t) noexcept;

#define _WJR_USE_ASM_INT128(FUNC) using masm::FUNC;

WJR_MACRO_CALL(_WJR_USE_ASM_INT128, ,
	u64x64,
	u32x32,
	u16x16,
	u64x64hi,
	u64x64lo,
	u32x32hi,
	u32x32lo,
	u16x16hi,
	u16x16lo,
	u128d64t64,
	u64d32t32,
	u32d16t16,
	u16d8t8,
	s64x64hi,
	s64x64lo,
	s32x32hi,
	s32x32lo,
	s16x16hi,
	s16x16lo
);

#undef _WJR_USE_ASM_INT128

template<typename T, std::enable_if_t<is_integrals_v<T>, int> = 0>
WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR) T mul(T a, T b, T& hi);

template<typename T, std::enable_if_t<is_integrals_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) T mulhi(T a, T b);

template<typename T, std::enable_if_t<is_integrals_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) T mullo(T a, T b);

// calc a ^ b
// O(log2 b)
template<typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, CONST, INLINE, CONSTEXPR) T power(T a, unsigned int b);

template<typename T, unsigned int base>
struct base_digits {
	static_assert(std::is_integral_v<T>, "");
	constexpr static unsigned int value = []() {
		unsigned int ret = 0;
		auto _Max = std::numeric_limits<T>::max();
		while (_Max) {
			++ret;
			_Max /= base;
		}
		return ret;
	}();
};

template<typename T, unsigned int base>
inline constexpr unsigned int base_digits_v = base_digits<T, base>::value;

// calc ceil(log(a) / log(base))
// for a = 0, return 0
// for a = (1, base), return 1
// force constexpr, so when base is power of 2, the performance may be worse
template<unsigned int base, typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
inline constexpr unsigned int base_width(T a);

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
inline constexpr unsigned int base2_width(T a);

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
inline constexpr unsigned int base10_width(T a);

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
inline constexpr unsigned int base_width(unsigned int b, T a);

template<typename R, typename T>
struct broadcast_fn {};

// broadcast T to R
// for example, T = uint32_t, R = uint8_t
// broadcast<R, T>(0x12) = 0x12121212
template<typename R, typename T>
inline constexpr broadcast_fn<R, T> broadcast{};

_WJR_END

#include <wjr/math-impl.h>

#endif // __WJR_MATH_H