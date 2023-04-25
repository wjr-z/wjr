#pragma once
#ifndef __WJR_MATH_H__
#define __WJR_MATH_H__

#include <string.h>

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
WJR_INTRINSIC_CONSTEXPR20 To bit_cast(const From & src) noexcept {
	static_assert(std::is_trivially_constructible_v<To>,
		"This implementation additionally requires destination type to be trivially constructible");
	To storage;
	if constexpr (is_any_index_of_v<sizeof(From), 1, 2, 4, 8>) {
		using value_type = uint_t<8 * sizeof(From)>;
		*reinterpret_cast<value_type*>(&storage) = *reinterpret_cast<const value_type*>(&src);
	}
	else {
		::memcpy(&storage, &src, sizeof(From));
	}
	return storage;
}

#endif

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
constexpr bool has_single_bit(T x) noexcept {
	return x != 0 && (x & (x - 1)) == 0;
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int countl_zero(T x) noexcept {
	return wjr::masm::clz(x);
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int countl_one(T x) noexcept {
	return wjr::countl_zero(static_cast<T>(~x));
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int countr_zero(T x) noexcept {
	return wjr::masm::ctz(x);
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int countr_one(T x) noexcept {
	return wjr::countr_zero(static_cast<T>(~x));
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int bit_width(T x) noexcept {
	return std::numeric_limits<T>::digits - wjr::countl_zero(x);
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) T bit_floor(T x) noexcept {
	if (x != 0) {
		return static_cast<T>(T{ 1 } << (wjr::bit_width(x) - 1));
	}
	return T{ 0 };
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) T bit_ceil(T x) noexcept {
	if (x <= 1) {
		return T{ 1 };
	}
	return static_cast<T>(T{ 1 } << wjr::bit_width(x - 1));
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int popcount(T x) noexcept {
	return wjr::masm::popcnt(x);
}

template<typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR20) T byteswap(T x) noexcept {
	using value_type = uint_t<8 * sizeof(T)>;
	return bit_cast<T>(wjr::masm::bswap(bit_cast<value_type>(x)));
}

template<endian to = endian::native, typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR20) T endian_convert(T x, endian from = endian::native) noexcept {
	if (from != to) {
		return byteswap(x);
	}
	return x;
}

template<endian from, endian to, typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR20) T endian_convert(T x) noexcept {
	if constexpr (from != to) {
		return byteswap(x);
	}
	return x;
}

//template<typename T, endian to = endian::native>
//WJR_INTRINSIC_CONSTEXPR20 void write_bytes

template<typename T, endian to = endian::native>
WJR_INTRINSIC_CONSTEXPR20 T read_bytes(const void* ptr) noexcept {
	T value = T{};
	memcpy(std::addressof(value), ptr, sizeof(T));
	return endian_convert<endian::little, to>(value);
}

template< typename T, typename U >
constexpr bool cmp_equal(T t, U u) noexcept {
	using UT = std::make_unsigned_t<T>;
	using UU = std::make_unsigned_t<U>;
	if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) {
		return t == u;
	}
	else if constexpr (std::is_signed_v<T>) {
		return t < 0 ? false : UT(t) == u;
	}
	else {
		return u < 0 ? false : t == UU(u);
	}
}

template< typename T, typename U >
constexpr bool cmp_not_equal(T t, U u) noexcept {
	return !cmp_equal(t, u);
}

template< typename T, typename U >
constexpr bool cmp_less(T t, U u) noexcept {
	using UT = std::make_unsigned_t<T>;
	using UU = std::make_unsigned_t<U>;
	if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) {
		return t < u;
	}
	else if constexpr (std::is_signed_v<T>) {
		return t < 0 ? true : UT(t) < u;
	}
	else {
		return u < 0 ? false : t < UU(u);
	}
}

template< typename T, typename U >
constexpr bool cmp_greater(T t, U u) noexcept {
	return wjr::cmp_less(u, t);
}

template< typename T, typename U >
constexpr bool cmp_less_equal(T t, U u) noexcept {
	return !wjr::cmp_greater(t, u);
}

template< typename T, typename U >
constexpr bool cmp_greater_equal(T t, U u) noexcept {
	return !wjr::cmp_less(t, u);
}

template< typename R, typename T>
constexpr bool in_range(T t) noexcept {
	return wjr::cmp_greater_equal(t, std::numeric_limits<R>::min()) &&
		wjr::cmp_less_equal(t, std::numeric_limits<R>::max());
}

// calc a ^ b
// O(log2 b)
template<typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, CONST, INLINE, CONSTEXPR) T power(T a, unsigned int b) {
	T ret = 1;
	while (b) {
		if (b & 1)ret *= a;
		a *= a;
		b >>= 1;
	}
	return ret;
}

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

template<unsigned int base, unsigned int SIZE>
struct __width_table {
	constexpr static unsigned int max_size = SIZE;
	static_assert(SIZE <= 2048, "");
	constexpr __width_table() : m_table() {
		m_table[0] = -1;
		for (unsigned int i = 1; i < SIZE; ++i) {
			m_table[i] = m_table[i / base] + 1;
		}
	}
	inline constexpr uint8_t operator[](unsigned int idx) const { return m_table[idx]; }
private:
	uint8_t m_table[SIZE];
};

template<unsigned int base>
struct __get_width_table_size {
	constexpr static unsigned int value = 0;
};

template<unsigned int base>
static constexpr __width_table<base, __get_width_table_size<base>::value> __width_table_v;

template<>
struct __get_width_table_size<2> {
	constexpr static unsigned int value = 512;
};

template<>
struct __get_width_table_size<3> {
	constexpr static unsigned int value = 729;
};

template<>
struct __get_width_table_size<5> {
	constexpr static unsigned int value = 625;
};

template<>
struct __get_width_table_size<6> {
	constexpr static unsigned int value = 1296;
};

template<>
struct __get_width_table_size<7> {
	constexpr static unsigned int value = 343;
};

template<>
struct __get_width_table_size<10> {
	constexpr static unsigned int value = 1000;
};

template<>
struct __get_width_table_size<11> {
	constexpr static unsigned int value = 121;
};

template<>
struct __get_width_table_size<12> {
	constexpr static unsigned int value = 144;
};

template<>
struct __get_width_table_size<13> {
	constexpr static unsigned int value = 169;
};

template<>
struct __get_width_table_size<14> {
	constexpr static unsigned int value = 196;
};

template<>
struct __get_width_table_size<15> {
	constexpr static unsigned int value = 225;
};

template<>
struct __get_width_table_size<17> {
	constexpr static unsigned int value = 289;
};

template<>
struct __get_width_table_size<18> {
	constexpr static unsigned int value = 324;
};

template<>
struct __get_width_table_size<19> {
	constexpr static unsigned int value = 361;
};

template<>
struct __get_width_table_size<20> {
	constexpr static unsigned int value = 400;
};

template<>
struct __get_width_table_size<21> {
	constexpr static unsigned int value = 441;
};

template<>
struct __get_width_table_size<22> {
	constexpr static unsigned int value = 484;
};

template<unsigned int base>
struct __get_width_table {
	constexpr static unsigned int value = -1;
};

template<>
struct __get_width_table<2> {
	constexpr static unsigned int value = 2;
};

template<>
struct __get_width_table<3> {
	constexpr static unsigned int value = 3;
};

template<>
struct __get_width_table<4> {
	constexpr static unsigned int value = 2;
};

template<>
struct __get_width_table<5> {
	constexpr static unsigned int value = 5;
};

template<>
struct __get_width_table<6> {
	constexpr static unsigned int value = 6;
};

template<>
struct __get_width_table<7> {
	constexpr static unsigned int value = 7;
};

template<>
struct __get_width_table<8> {
	constexpr static unsigned int value = 2;
};

template<>
struct __get_width_table<9> {
	constexpr static unsigned int value = 3;
};

template<>
struct __get_width_table<10> {
	constexpr static unsigned int value = 10;
};

template<>
struct __get_width_table<11> {
	constexpr static unsigned int value = 11;
};

template<>
struct __get_width_table<12> {
	constexpr static unsigned int value = 12;
};

template<>
struct __get_width_table<13> {
	constexpr static unsigned int value = 13;
};

template<>
struct __get_width_table<14> {
	constexpr static unsigned int value = 14;
};

template<>
struct __get_width_table<15> {
	constexpr static unsigned int value = 15;
};

template<>
struct __get_width_table<16> {
	constexpr static unsigned int value = 2;
};

template<>
struct __get_width_table<17> {
	constexpr static unsigned int value = 17;
};

template<>
struct __get_width_table<18> {
	constexpr static unsigned int value = 18;
};

template<>
struct __get_width_table<19> {
	constexpr static unsigned int value = 19;
};

template<>
struct __get_width_table<20> {
	constexpr static unsigned int value = 20;
};

template<>
struct __get_width_table<21> {
	constexpr static unsigned int value = 21;
};

template<>
struct __get_width_table<22> {
	constexpr static unsigned int value = 22;
};

template<>
struct __get_width_table<25> {
	constexpr static unsigned int value = 5;
};

template<unsigned int base>
inline constexpr unsigned int __get_width_table_v = __get_width_table<base>::value;

// calc depth of __width
template<unsigned int base, typename T, unsigned int digits>
inline constexpr unsigned int __width_depth() {
	// digits = 2, that's to say value < base ^ 2
	// we can just test if val >= base
	if constexpr (digits <= 2) {
		return 1;
	}
	else {
		constexpr auto mid_digits = (digits + 1) / 2;
		constexpr auto __table_index = __get_width_table_v<base>;
		if constexpr (__table_index != -1) {
			constexpr auto __table_size = __get_width_table_size<__table_index>::value;
			constexpr auto mid = power<T>(base, mid_digits);
			if constexpr (mid <= __table_size) {
				return 1;
			}
			else {
				return 1 + __width_depth<base, T, mid_digits>();
			}
		}
		else {
			return 1 + __width_depth<base, T, mid_digits>();
		}
	}
}

template<unsigned int base, typename T, unsigned int digits>
inline constexpr bool __width_end_with_table() {
	if constexpr (digits <= 2) {
		return false;
	}
	else {
		constexpr auto mid_digits = (digits + 1) / 2;
		constexpr auto __table_index = __get_width_table_v<base>;
		if constexpr (__table_index != -1) {
			constexpr auto __table_size = __get_width_table_size<__table_index>::value;
			constexpr auto mid = power<T>(base, mid_digits);
			if constexpr (mid <= __table_size) {
				return true;
			}
			else {
				return __width_end_with_table<base, T, mid_digits>();
			}
		}
		else {
			return __width_end_with_table<base, T, mid_digits>();
		}
	}
}

#define __WIDTH_WORK_GEN(x)	                                \
if constexpr(depth >= x){	                                \
	constexpr auto mid_digits##x = get_mid_digits(x);	    \
	constexpr auto mid##x = power<T>(base, mid_digits##x);	\
	if(a >= mid##x){	                                    \
		ret += mid_digits##x;	                            \
		a /= mid##x;		                                \
	}	                                                    \
}

// optimized
template<unsigned int base, typename T, unsigned int digits, unsigned int depth, bool use_table>
inline constexpr void __width(T a, unsigned int& ret) {
	constexpr auto get_mid_digits = [](int x) {
		unsigned int D = digits;
		while (x) {
			D = (D + 1) / 2;
			--x;
		}
		return D;
	};
	if constexpr (depth <= 4) {
		WJR_MACRO_CALL(__WIDTH_WORK_GEN, , 1, 2, 3, 4);
		if constexpr (use_table) {
			constexpr auto __mid_digits = get_mid_digits(depth);
			constexpr auto __mid = power<T>(base, __mid_digits);
			constexpr auto __table_index = __get_width_table_v<base>;
			constexpr auto __table_size = __get_width_table_size<__table_index>::value;
			//static_assert(__mid <= __table_size, "");
			constexpr auto p = []() {
				unsigned int ret = 0;
				unsigned int idx = base;
				while (idx != 1) {
					++ret;
					idx /= __table_index;
				}
				return ret;
			}();
			ret += (__width_table_v<__table_index>[static_cast<unsigned int>(a)] + p - 1) / p;
		}
	}
	else {
		WJR_MACRO_CALL(__WIDTH_WORK_GEN, , 1, 2, 3, 4);
		__width<base, T, get_mid_digits(4), depth - 4, use_table>(a, ret);
	}
}

#undef __WIDTH_WORK_GEN

// calc ceil(log(a) / log(base))
// for a = 0, return 0
// for a = (1, base), return 1
// force constexpr, so when base is power of 2, the performance may be worse
template<unsigned int base, typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
inline constexpr unsigned int base_width(T a) {
	constexpr auto __digits = base_digits_v<T, base>;
	constexpr auto __depth = __width_depth<base, T, __digits>();
	constexpr auto __use_table = __width_end_with_table<base, T, __digits>();
	if (WJR_UNLIKELY(!a)) return 0;
	unsigned int ret = 1;
	__width<base, T, __digits, __depth, __use_table>(a, ret);
	return ret;
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
inline constexpr unsigned int base2_width(T a) {
	return base_width<2>(a);
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
inline constexpr unsigned int base10_width(T a) {
	return base_width<10>(a);
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
inline constexpr unsigned int base_width(unsigned int b, T a) {
	// optimize for common cases
	switch (b) {
	case 2: return base_width<2>(a);
	case 3: return base_width<3>(a);
	case 4: return base_width<4>(a);
	case 5: return base_width<5>(a);
	case 6: return base_width<6>(a);
	case 7: return base_width<7>(a);
	case 8: return base_width<8>(a);
	case 9: return base_width<9>(a);
	case 10: return base_width<10>(a);
	case 16: return base_width<16>(a);
	case 25: return base_width<25>(a);
	default: {
		WJR_ASSUME(b != 0 && b != 1);
		unsigned int ret = 0;
		while (a) {
			++ret;
			a /= b;
		}
		return ret;
	}
	}
}

template<typename R, typename T>
struct broadcast_fn {};

template<typename R, typename T>
inline constexpr broadcast_fn<R, T> broadcast{};

template<>
struct broadcast_fn<uint8_t, uint8_t> {
	WJR_INTRINSIC_CONSTEXPR uint8_t operator()(uint8_t x)const {
		return x;
	}
};
template<>
struct broadcast_fn<uint16_t, uint16_t> {
	WJR_INTRINSIC_CONSTEXPR uint16_t operator()(uint16_t x)const {
		return x;
	}
};
template<>
struct broadcast_fn<uint32_t, uint32_t> {
	WJR_INTRINSIC_CONSTEXPR uint32_t operator()(uint32_t x)const {
		return x;
	}
};
template<>
struct broadcast_fn<uint64_t, uint64_t> {
	WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint64_t x)const {
		return x;
	}
};

template<>
struct broadcast_fn<uint16_t, uint8_t> {
	WJR_INTRINSIC_CONSTEXPR uint16_t operator()(uint8_t x)const {
		return static_cast<uint16_t>(static_cast<uint32_t>(x) | (static_cast<uint16_t>(x) << 8));
	}
};

template<>
struct broadcast_fn<uint32_t, uint16_t> {
	WJR_INTRINSIC_CONSTEXPR uint32_t operator()(uint16_t x)const {
		return x | (static_cast<uint32_t>(x) << 16);
	}
};
template<>
struct broadcast_fn<uint64_t, uint32_t> {
	WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint32_t x)const {
		return static_cast<uint64_t>(x) | (static_cast<uint64_t>(x) << 32);
	}
};

template<>
struct broadcast_fn<uint32_t, uint8_t> {
	WJR_INTRINSIC_CONSTEXPR uint32_t operator()(uint8_t x)const {
		return x * static_cast<uint32_t>(0x01010101u);
	}
};
template<>
struct broadcast_fn<uint64_t, uint16_t> {
	WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint16_t x)const {
		return x * static_cast<uint64_t>(0x0001000100010001ull);
	}
};

template<>
struct broadcast_fn<uint64_t, uint8_t> {
	WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint8_t x)const {
		return x * static_cast<uint64_t>(0x0101010101010101ull);
	}
};

_WJR_END

#endif // __WJR_MATH_H__