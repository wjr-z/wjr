#pragma once
#ifndef __WJR_STRING_FUNC_H
#define __WJR_STRING_FUNC_H

// functions for string encode

#include <wjr/math.h>
#include <wjr/error_handle.h>

_WJR_BEGIN


// NO_FLAGS :
// ALLOW_PREFIX :
//	ignore prefix
//	prefix :
//		base = 0  : 0[bBxX]?
//		base = 2  : 0[bB]
//		base = 8  : 0
//		base = 10 : 0[xX]
// ALLOW_LEADING_SPACE :
//	ignore leading space of string
//	disable this option would be faster
// ALLOW_TRAILING_SPACE
//	ignore space after sign
// ALLOW_LEADING_ZEROS
//	ignore leading zeros of digit
// It is best to know all flags during compilation 
// for optimization during compilation
/*
enum class conv_flags {
	NO_FLAGS = 0x00,
	ALLOW_PREFIX = 0x01,
	ALLOW_LEADING_SPACE = 0x02,
	ALLOW_TRAILING_SPACE = 0x04
};
*/

// fast integer conversion of strings with different encodings
// requires :
// 1. forward iterator
// 2. constexpr uint32_t todigit<base = 10[, idx = 0]>
// related to base
// return (digit(ch)) * (base ^ idx);
// if is a invalid digit, return 1u << 24;
// use this function to find last of digit
// 3. constexpr bool isspace
// use this function to skip white space
template<typename Traits>
class string_func {
public:
	using traits_type = Traits;
	using value_type = typename traits_type::value_type;

	constexpr static uint32_t invalid_digit = 1u << 24;

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isalnum(value_type ch);

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isalpha(value_type ch);

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool islower(value_type ch);

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isupper(value_type ch);

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isxdigit(value_type ch);

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isspace(value_type ch);

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static value_type tolower(value_type ch);

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static value_type toupper(value_type ch);

	// base <= 10, digit is ['0', '0' + base - 1]
	// base > 10, digit is ['0', '9'] and ['a', 'a' + (base - 10) - 1] and ['A', 'A' + (base - 10) - 1]
	template<unsigned int Base = 10>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static bool isdigit(value_type ch);

	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static bool isdigit(value_type ch, int Base);

	template<unsigned int Base = 10, int idx = 0>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static uint32_t todigit(value_type ch);

	// return todigit<36, idx>(ch)
	template<int idx = 0>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static uint32_t toalnum(value_type ch);

	// skip whit space
	template<typename _Iter>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static _Iter skipw(_Iter _First, _Iter _Last);

	// skip whit space at end
	template<typename _Iter>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static _Iter rskipw(_Iter _First, _Iter _Last);

	// skip leading zeros
	template<typename _Iter>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static _Iter skipz(_Iter _First, _Iter _Last);

	// skip digit
	template<unsigned int Base, typename _Iter>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static _Iter skipd(_Iter _First, _Iter _Last);

	template<typename _Iter>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static _Iter skipd(_Iter _First, _Iter _Last, int Base);

	template<typename T, typename _Iter>
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static T to_integral(
		_Iter _First, _Iter _Last,
		_Iter& _Pos, error_code& _Err, int base);

};

#define __CONV_OK_RET(ptr, ret)					\
		_Pos = ptr;								\
		return ret;
#define __CONV_EMPTY_RET				        \
		_Err = error_code::noconv;				\
		return static_cast<T>(0);
#define __CONV_NEXT								\
		if(is_unlikely(++_First == _Last)){	    \
			__CONV_EMPTY_RET					\
		}
#define __CONV_OVERFLOW_RET	                    \
		_Err = error_code::overflow;			\
		return max();
#define __CONV_UNDERFLOW_RET	                \
		_Err = error_code::underflow;			\
		return min();
#define __CONV_FLOW_RET	                        \
		if constexpr (std::is_unsigned_v<T>) {	\
			__CONV_OVERFLOW_RET;	            \
		}	                                    \
		else {	                                \
			if (_Is_p) {	                    \
				__CONV_OVERFLOW_RET;	        \
			}	                                \
			else {	                            \
				__CONV_UNDERFLOW_RET;	        \
			}	                                \
		}

// details of integral converison
template<typename T, typename Func, unsigned int base>
class __integral_conversion_details_helper {
public:
	using func_type = Func;
	using value_type = typename func_type::value_type;
	static_assert(base >= 2 && base <= 36 && sizeof(value_type) == 1, "");

	constexpr static unsigned int base0 = 1;
	constexpr static unsigned int base1 = base;
	constexpr static unsigned int base2 = base1 * base;
	constexpr static unsigned int base3 = base2 * base;
	constexpr static unsigned int base4 = base3 * base;

	constexpr static T min() { return std::numeric_limits<T>::min(); }
	constexpr static T max() { return std::numeric_limits<T>::max(); }

	using uT = std::make_unsigned_t<T>;

	constexpr static uT umax() { return std::max(make_unsigned_v(min()), make_unsigned_v(max())); }

	constexpr static unsigned int digits = base_width<base>(umax());

	template<int idx>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static unsigned int todigit(value_type ch) { 
		return func_type::template todigit<base, idx>(ch); 
	}

	template<typename _Iter>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static T work(
		_Iter _First, size_t n,
		error_code& _Err, bool _Is_p) {
		if (n > digits) {
			__CONV_FLOW_RET;
		}

		if (n == digits) {
			const auto __ret = check(_First);
			int ret = __ret.first;

			if constexpr (std::is_unsigned_v<T>) {

				if (ret < 0) {
					__CONV_FLOW_RET;
				}

				// if ret == 0
				// __ret.second == umax()
				// we can merge ret==0 and ret<0

				if (_Is_p) {
					return static_cast<T>(__ret.second);
				}

				return static_cast<T>(-__ret.second);
			}
			else {

				if (ret < 0) {
					__CONV_FLOW_RET;
				}

				// signed number, further judgment is needed
				// if is positive, and ret == 0
				// it's to say num == -min(), we know that -min() == max() + 1
				// so num is overflow

				if (ret == 0) {
					if (_Is_p) {
						__CONV_OVERFLOW_RET;
					}
					return min();
				}

				if (_Is_p) {
					return static_cast<T>(__ret.second);
				}

				return static_cast<T>(-__ret.second);
			}
		}

		uT uret = 0;

		if (n >= 4) {
			auto r0 = todigit<3>(*_First);
			++_First;
			auto r1 = todigit<2>(*_First);
			++_First;
			auto r2 = todigit<1>(*_First);
			++_First;
			auto r3 = todigit<0>(*_First);
			++_First;

			uret = r0 + r1 + r2 + r3;
			n -= 4;

			while (n >= 4) {
				uret *= base4;
				auto r0 = todigit<3>(*_First);
				++_First;
				auto r1 = todigit<2>(*_First);
				++_First;
				auto r2 = todigit<1>(*_First);
				++_First;
				auto r3 = todigit<0>(*_First);
				++_First;

				uret += r0 + r1 + r2 + r3;
				n -= 4;
			} while (n >= 4);

			switch (n) {
			case 3: {
				uret *= base3;
				auto r0 = todigit<2>(*_First);
				++_First;
				auto r1 = todigit<1>(*_First);
				++_First;
				auto r2 = todigit<0>(*_First);
				uret += r0 + r1 + r2;
				break;
			}
			case 2: {
				uret *= base2;
				auto r0 = todigit<1>(*_First);
				++_First;
				auto r1 = todigit<0>(*_First);
				uret += r0 + r1;
				break;
			}
			case 1: {
				uret *= base1;
				auto r0 = todigit<0>(*_First);
				uret += r0;
				break;
			}
			default: break;
			}
		}
		else {
			switch (n) {
			case 3: {
				auto r0 = todigit<2>(*_First);
				++_First;
				auto r1 = todigit<1>(*_First);
				++_First;
				auto r2 = todigit<0>(*_First);
				uret = r0 + r1 + r2;
				break;
			}
			case 2: {
				auto r0 = todigit<1>(*_First);
				++_First;
				auto r1 = todigit<0>(*_First);
				uret = r0 + r1;
				break;
			}
			case 1: {
				auto r0 = todigit<0>(*_First);
				uret = r0;
				break;
			}
			default: break;
			}
		}

		if (_Is_p) {
			return static_cast<T>(uret);
		}
		return static_cast<T>(-uret);
	}

private:

	using __power_type = std::conditional_t<
		sizeof(uT) <= sizeof(uint32_t), uint32_t, uT>;

	template<int idx, typename _Ty>
	WJR_INTRINSIC_CONSTEXPR static _Ty __mod(_Ty val) {
		if constexpr (idx < digits) {
			constexpr auto _Div = power<__power_type>(base, idx);
			return val % _Div;
		}
		else {
			return val;
		}
	}

	template<int idx, typename _Ty>
	WJR_INTRINSIC_CONSTEXPR static _Ty __div(_Ty val) {
		if constexpr (idx < digits) {
			constexpr auto _Div = power<__power_type>(base, idx);
			return val / _Div;
		}
		else {
			return 0;
		}
	}

	template<typename _Iter>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static std::pair<int, uT> check(_Iter _First) {
		uT ret = 0;
		int op = __check<digits, false>(_First, ret);
		return std::make_pair(op, ret);
	}

	template<size_t idx, bool below, typename _Iter>
	WJR_INTRINSIC_CONSTEXPR static int __check(_Iter _First, uT& ret) {

		if constexpr (idx >= 4) {
			constexpr auto _Val = __mod<4>(__div<idx - 4>(umax()));

			if constexpr (below && idx != digits) {
				ret *= base4;
			}

			auto r0 = todigit<3>(*_First);
			++_First;
			auto r1 = todigit<2>(*_First);
			++_First;
			auto r2 = todigit<1>(*_First);
			++_First;
			auto r3 = todigit<0>(*_First);
			++_First;

			auto sum = r0 + r1 + r2 + r3;

			if constexpr (!below) {
				if (sum > _Val) {
					return -1;
				}

				if (sum < _Val) {
					constexpr auto _Num = __div<idx>(umax()) * base4;
					ret = _Num + sum;
					__check<idx - 4, true>(_First, ret);
					return 1;
				}

				return __check<idx - 4, false>(_First, ret);
			}
			else {
				ret += sum;
				return 1;
			}
		}
		else if constexpr (idx == 3) {
			constexpr auto _Val = __mod<3>(umax());

			if constexpr (below) {
				ret *= base3;
			}

			auto r0 = todigit<2>(*_First);
			++_First;
			auto r1 = todigit<1>(*_First);
			++_First;
			auto r2 = todigit<0>(*_First);

			auto sum = r0 + r1 + r2;

			if constexpr (!below) {
				if (sum > _Val) {
					return -1;
				}

				constexpr auto _Num = __div<idx>(umax()) * base3;

				ret = _Num + sum;
				return sum == _Val ? 0 : 1;
			}
			else {
				ret += sum;
				return 1;
			}
		}
		else if constexpr (idx == 2) {
			constexpr auto _Val = __mod<2>(umax());

			if constexpr (below) {
				ret *= base2;
			}

			auto r0 = todigit<1>(*_First);
			++_First;
			auto r1 = todigit<0>(*_First);

			auto sum = r0 + r1;

			if constexpr (!below) {
				if (sum > _Val) {
					return -1;
				}

				constexpr auto _Num = __div<idx>(umax()) * base2;

				ret = _Num + sum;
				return sum == _Val ? 0 : 1;
			}
			else {
				ret += sum;
				return 1;
			}
		}
		else if constexpr (idx == 1) {
			constexpr auto _Val = __mod<1>(umax());

			if constexpr (below) {
				ret *= base1;
			}

			auto r0 = todigit<0>(*_First);

			auto sum = r0;

			if constexpr (!below) {
				if (sum > _Val) {
					return -1;
				}

				constexpr auto _Num = __div<idx>(umax()) * base1;

				ret = _Num + sum;
				return sum == _Val ? 0 : 1;
			}
			else {
				ret += sum;
				return 1;
			}
		}
		else {
			ret = umax();
			return 0;
		}
	}

};

template<typename T, typename Func>
class __integral_conversion_details_table_helper {
public:
	using func_type = Func;
	using value_type = typename func_type::value_type;

	constexpr static T min() { return std::numeric_limits<T>::min(); }
	constexpr static T max() { return std::numeric_limits<T>::max(); }

	using uT = std::make_unsigned_t<T>;

	constexpr static uT umax() { return std::max(make_unsigned_v(min()), make_unsigned_v(max())); }

	constexpr static unsigned int max_digits = base_width<2>(umax());

	template<unsigned int Base>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static unsigned int todigit(value_type ch) {
		return func_type::template todigit<Base, 0>(ch);
	}

	constexpr __integral_conversion_details_table_helper() 
		: m_base(), m_digits(0), m_max() {}

	constexpr void init(int _Base) {
		m_digits = base_width(_Base, umax());

		m_base[0] = _Base;
		m_base[1] = m_base[0] * _Base;
		m_base[2] = m_base[1] * _Base;
		m_base[3] = m_base[2] * _Base;

		auto B = umax();
		auto i = m_digits;
		if (i >= 4) {
			auto P = power(static_cast<uT>(m_base[0]), i - 4);
			do {
				auto q = B / P;
				B -= q * P;
				P /= m_base[3];
				m_max[i / 4] = q;
				i -= 4;
			} while (i >= 4);
		}

		m_max[0] = B;
	}

	WJR_INLINE_CONSTEXPR unsigned int get_digits() const { return m_digits; }

	template<int idx>
	WJR_INLINE_CONSTEXPR unsigned int get_base() const { return m_base[idx]; }

	// *_First must in range
	template<typename _Iter>
	WJR_INLINE_CONSTEXPR std::pair<int, uT> operator()(_Iter _First) const {
		bool below = false;
		auto i = m_digits;
		uT ret = 0;
		if (i >= 4) {
			do {
				auto r0 = todigit<36>(*_First) * m_base[2];
				++_First;
				auto r1 = todigit<36>(*_First) * m_base[1];
				++_First;
				auto r2 = todigit<36>(*_First) * m_base[0];
				++_First;
				auto r3 = todigit<36>(*_First);
				++_First;
				auto sum = r0 + r1 + r2 + r3;
				if (!below) {
					auto _Val = m_max[i / 4];
					if (sum > _Val) return std::make_pair(-1, 0);
					if (sum < _Val) below = true;
				}
				ret *= m_base[3];
				ret += sum;
				i -= 4;
			} while (i >= 4);
		}
		switch (i) {
		case 3: {
			auto r0 = todigit<36>(*_First) * m_base[1];
			++_First;
			auto r1 = todigit<36>(*_First) * m_base[0];
			++_First;
			auto r2 = todigit<36>(*_First);
			auto sum = r0 + r1 + r2;
			if (!below) {
				auto _Val = m_max[0];
				if (sum > _Val) return std::make_pair(-1, 0);
				if (sum < _Val) below = true;
			}
			ret *= m_base[2];
			ret += sum;
			return std::make_pair(below ? 1 : 0, ret);
		}
		case 2: {
			auto r0 = todigit<36>(*_First) * m_base[0];
			++_First;
			auto r1 = todigit<36>(*_First);
			auto sum = r0 + r1;
			if (!below) {
				auto _Val = m_max[0];
				if (sum > _Val) return std::make_pair(-1, 0);
				if (sum < _Val) below = true;
			}
			ret *= m_base[1];
			ret += sum;
			return std::make_pair(below ? 1 : 0, ret);
		}
		case 1: {
			auto r0 = todigit<36>(*_First);
			auto sum = r0;
			if (!below) {
				auto _Val = m_max[0];
				if (sum > _Val) return std::make_pair(-1, 0);
				if (sum < _Val) below = true;
			}
			ret *= m_base[0];
			ret += sum;
			return std::make_pair(below ? 1 : 0, ret);
		}
		case 0: {
			return std::make_pair(0, ret);
		}
		default: {
			unreachable();
			return std::make_pair(0, 0);
		}
		}
	}

private:

	unsigned int m_base[4];
	unsigned int m_digits;
	unsigned int m_max[max_digits / 4 + 1];
};

template<typename T, typename Func>
class __integral_conversion_details_table {
public:
	using helper_type = __integral_conversion_details_table_helper<T, Func>;
	constexpr __integral_conversion_details_table() : m_table() {
		for (int i = 2; i < 36; ++i)m_table[i - 2].init(i);
	}
	constexpr const helper_type& operator[](int idx) const { return m_table[idx - 2]; }
private:
	helper_type m_table[34];
};

template<typename T, typename Func>
constexpr static __integral_conversion_details_table<T, Func> __integral_conversion_details_table_v;

template<typename T, typename Func>
class __integral_conversion_details_helper<T, Func, -1u> {
public:
	using func_type = Func;
	using value_type = typename func_type::value_type;

	constexpr static T min() { return std::numeric_limits<T>::min(); }
	constexpr static T max() { return std::numeric_limits<T>::max(); }

	using uT = std::make_unsigned_t<T>;

	constexpr static uT umax() { return std::max(make_unsigned_v(min()), make_unsigned_v(max())); }

	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static unsigned int todigit(value_type ch) {
		return func_type::template todigit<36, 0>(ch);
	}

	template<typename _Iter>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static T work(
		_Iter _First, size_t n,
		error_code& _Err, int base, bool _Is_p) {

		WJR_ASSUME(base != 2 && base != 8 && base != 10 && base != 16);

		const auto& helper = __integral_conversion_details_table_v<T, func_type>[base];

		// check flow
		auto digits = helper.get_digits();

		// must flow
		if (n > digits) {
			__CONV_FLOW_RET;
		}

		// maybe flow, further testing
		if (n == digits) {

			const auto __ret = helper(_First);
			int ret = __ret.first;

			if constexpr (std::is_unsigned_v<T>) {

				if (ret < 0) {
					__CONV_FLOW_RET;
				}

				if (_Is_p) {
					return static_cast<T>(__ret.second);
				}

				return static_cast<T>(-__ret.second);
			}
			else {

				if (ret < 0) {
					__CONV_FLOW_RET;
				}

				if (ret == 0) {
					if (_Is_p) {
						__CONV_OVERFLOW_RET;
					}
					return helper.min();
				}

				if (_Is_p) {
					return static_cast<T>(__ret.second);
				}

				return static_cast<T>(-__ret.second);
			}
		}

		WJR_ASSUME(n < digits);

		// the result won't have error

		std::make_unsigned_t<T> uret = 0;

		auto multi1 = helper.template get_base<0>();
		auto multi2 = helper.template get_base<1>();
		auto multi3 = helper.template get_base<2>();
		auto multi4 = helper.template get_base<3>();

		for (; n >= 4; n -= 4) {
			uret *= multi4;
			auto r0 = todigit(*_First) * multi3;
			++_First;
			auto r1 = todigit(*_First) * multi2;
			++_First;
			auto r2 = todigit(*_First) * multi1;
			++_First;
			auto r3 = todigit(*_First);
			++_First;
			uret += r0 + r1 + r2 + r3;
		}

		switch (n) {
		case 3: {
			uret *= multi3;
			auto r0 = todigit(*_First) * multi2;
			++_First;
			auto r1 = todigit(*_First) * multi1;
			++_First;
			auto r2 = todigit(*_First);
			uret += r0 + r1 + r2;
			break;
		}
		case 2: {
			uret *= multi2;
			auto r0 = todigit(*_First) * multi1;
			++_First;
			auto r1 = todigit(*_First);
			uret += r0 + r1;
			break;
		}
		case 1: {
			uret *= multi1;
			auto r0 = todigit(*_First);
			uret += r0;
			break;
		}
		}

		if (_Is_p) {
			return static_cast<T>(uret);
		}

		return static_cast<T>(-uret);
	}
};

template<typename T, typename Func>
class integral_conversion_details {
public:
	using func_type = Func;
	using value_type = typename func_type::value_type;

	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static bool isspace(value_type ch) {
		return func_type::isspace(ch);
	}

	template<unsigned int Base, int idx>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static uint32_t todigit(value_type ch) {
		static_assert(Base >= 2 && Base <= 36 && idx >= 0 && idx < 4, "");
		return func_type::template todigit<Base, idx>(ch);
	}

	template<typename _Iter>
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static T work(
		_Iter _First, _Iter _Last,
		_Iter& _Pos, error_code& _Err, int base) {
		switch (base) {
		case 0: return __constexpr_work<0>(_First, _Last, _Pos, _Err);
		case 2: return __constexpr_work<2>(_First, _Last, _Pos, _Err);
		case 8: return __constexpr_work<8>(_First, _Last, _Pos, _Err);
		case 10: return __constexpr_work<10>(_First, _Last, _Pos, _Err);
		case 16: return __constexpr_work<16>(_First, _Last, _Pos, _Err);
		default: {
			if (is_unlikely(base < 2 || base > 36)) {
				_Err = error_code::noconv;
				return static_cast<T>(0);
			}
			return __runtime_work(_First, _Last, _Pos, _Err, base);
		}
		}
	}

private:

	template<unsigned int Base, typename _Iter>
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static T __constexpr_work(
		_Iter _First, _Iter _Last,
		_Iter& _Pos, error_code& _Err) {

		_Err = error_code::ok;

		// skip white space
		_First = func_type::skipw(_First, _Last);

		if (is_unlikely(_First == _Last)) {
			__CONV_EMPTY_RET;
		}

		bool _Is_p = true;

		// eat sign
		if (*_First == '+') {
			__CONV_NEXT;
		}
		else if (*_First == '-') {
			_Is_p = false;
			__CONV_NEXT;
		}

		// -1 : no prefix
		// 0 : length of prefix is 1
		// 1 : length of prefix is 2
		// notice that for base = 2, 0b is prefix, 0 is not prefix
		// but we will eat first 0, if the second char is not 'b'/'B'
		// we also set prefix = 0
		int prefix = -1;

		// Base = 0 prefix : 0[bBxX]?
		// base = 0 is not testing yet
		if constexpr (Base == 0 || Base == 2 || Base == 8 || Base == 16) {
			if constexpr (Base == 0) {
				if (*_First == '0') {
					if (is_unlikely(++_First == _Last)) {
						// treat 0 as a number instead of a prefix
						__CONV_OK_RET(_First, 0);
					}
					switch (*_First) {
					case 'x':
					case 'X': {
						if (is_unlikely(++_First == _Last)) {
							// treat 0 as a number instead of a prefix
							__CONV_OK_RET(_First - 1, 0);
						}
						return __work<16>(_First, _Last, _Pos, _Err, _Is_p, 1);
					}
					case 'b':
					case 'B': {
						if (is_unlikely(++_First == _Last)) {
							// treat 0 as a number instead of a prefix
							__CONV_OK_RET(_First - 1, 0);
						}
						return __work<2>(_First, _Last, _Pos, _Err, _Is_p, 1);
					}
					default: {
						return __work<8>(_First, _Last, _Pos, _Err, _Is_p, 0);
					}
					}
				}
				else {
					// base = 10
					return __work<10>(_First, _Last, _Pos, _Err, _Is_p, -1);
				}
			}
			else if constexpr (Base == 2) {
				if (*_First == '0') {
					if (is_unlikely(++_First == _Last)) {
						// treat 0 as a number instead of a prefix
						__CONV_OK_RET(_First, 0);
					}

					switch (*_First) {
					case 'b':
					case 'B': {
						if (is_unlikely(++_First == _Last)) {
							// treat 0 as a number instead of a prefix
							// ignore 'x'/'X'
							__CONV_OK_RET(_First - 1, 0);
						}
						prefix = 1;
						break;
					}
					default: {
						prefix = 0;
						break;
					}
					}
				}
			}
			else if constexpr (Base == 8) {
				if (*_First == '0') {
					if (is_unlikely(++_First == _Last)) {
						// treat 0 as a number instead of a prefix
						__CONV_OK_RET(_First, 0);
					}
					// 0 maybe is a prefix or just number 0
					// if there's no more digits after it, it wil be treat as number 0
					prefix = 0;
				}
			}
			else if constexpr (Base == 16) {
				if (*_First == '0') {
					if (is_unlikely(++_First == _Last)) {
						// treat 0 as a number instead of a prefix
						__CONV_OK_RET(_First, 0);
					}
					switch (*_First) {
					case 'x':
					case 'X': {
						if (is_unlikely(++_First == _Last)) {
							// treat 0 as a number instead of a prefix
							// ignore 'x'/'X'
							__CONV_OK_RET(_First - 1, 0);
						}
						prefix = 1;
						break;
					}
					default: {
						prefix = 0;
						break;
					}
					}
				}
			}
		}

		if constexpr (Base != 0) {
			return __work<Base>(_First, _Last, _Pos, _Err, _Is_p, prefix);
		}
		else {
			unreachable();
		}

	}

	template<typename _Iter>
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static T __runtime_work(
		_Iter _First, _Iter _Last,
		_Iter& _Pos, error_code& _Err, int base) {

		_Err = error_code::ok;

		// skip white space
		_First = func_type::skipw(_First, _Last);

		if (is_unlikely(_First == _Last)) {
			__CONV_EMPTY_RET;
		}

		bool _Is_p = true;

		// eat sign
		if (*_First == '+') {
			__CONV_NEXT;
		}
		else if (*_First == '-') {
			_Is_p = false;
			__CONV_NEXT;
		}

		return __work(_First, _Last, _Pos, _Err, base, _Is_p);
	}

	template<unsigned int Base, typename _Iter>
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static T __work(
		_Iter _First, _Iter _Last,
		_Iter& _Pos, error_code& _Err, bool _Is_p, int prefix) {

		_Last = func_type::template skipd<Base>(_First, _Last);

			// no digits
			if (_First == _Last) {
				// have a prefix 0
				if constexpr (Base == 2 || Base == 8 || Base == 16) {
					if (prefix != -1) {
						// treat 0 as a number instead of a prefix
						// for prefix = 0, we eat a '0'
						// for prefix = 0*, we need to set end_ptr = i - 1
						_Pos = _First - prefix;
						return static_cast<T>(0);
					}
				}
				// invalid digits
				__CONV_EMPTY_RET;
			}

			// _First != _Last

			_Pos = _Last;

			// remove leading zeros
			_First = func_type::skipz(_First, _Last);

			auto n = std::distance(_First, _Last);

			if (!n) {
				return static_cast<T>(0);
			}

			return __integral_conversion_details_helper<T, func_type, Base>
				::work(_First, n, _Err, _Is_p);
	}

	template<typename _Iter>
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static T __work(
		_Iter _First, _Iter _Last,
		_Iter& _Pos, error_code& _Err, int base, bool _Is_p) {

		_Last = func_type::template skipd(_First, _Last, base);

			// no digits
			if (_First == _Last) {
				// invalid digits
				__CONV_EMPTY_RET;
			}

			// _First != _Last

			_Pos = _Last;

			// remove leading zeros
			_First = func_type::skipz(_First, _Last);

			auto n = std::distance(_First, _Last);

			if (!n) {
				return static_cast<T>(0);
			}

			return __integral_conversion_details_helper<T, func_type, -1u>
				::work(_First, n, _Err, base, _Is_p);
	}

};

#undef __CONV_OK_RET
#undef __CONV_EMPTY_RET
#undef __CONV_NEXT
#undef __CONV_OVERFLOW_RET
#undef __CONV_UNDERFLOW_RET
#undef __CONV_FLOW_RET

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::isalnum(value_type ch) {
	return traits_type::isalnum(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::isalpha(value_type ch) {
	return traits_type::isalpha(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::islower(value_type ch) {
	return traits_type::islower(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::isupper(value_type ch) {
	return traits_type::isupper(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::isxdigit(value_type ch) {
	return traits_type::isxdigit(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::isspace(value_type ch) {
	return traits_type::isspace(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR typename string_func<Traits>::value_type 
string_func<Traits>::tolower(value_type ch) {
	return traits_type::tolower(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR typename string_func<Traits>::value_type 
string_func<Traits>::toupper(value_type ch) {
	return traits_type::toupper(ch);
}

template<typename Traits>
template<unsigned int Base>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR bool string_func<Traits>::isdigit(value_type ch) {
	return todigit<Base>(ch) != invalid_digit;
}

template<typename Traits>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR bool string_func<Traits>::isdigit(value_type ch, int Base) {
	return toalnum(ch) < Base;
}

template<typename Traits>
template<unsigned int Base, int idx>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR uint32_t string_func<Traits>::todigit(value_type ch) {
	return traits_type::template todigit<Base, idx>(ch);
}

template<typename Traits>
template<int idx>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR uint32_t string_func<Traits>::toalnum(value_type ch) {
	return todigit<36u, idx>(ch);
}

template<typename Traits>
template<typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR _Iter string_func<Traits>::skipw(_Iter _First, _Iter _Last) {
	while (_First != _Last && isspace(*_First)) ++_First;
	return _First;
}

// skip whit space at end
template<typename Traits>
template<typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR _Iter string_func<Traits>::rskipw(_Iter _First, _Iter _Last) {
	while (_First != _Last && isspace(*(_Last - 1))) --_Last;
	return _Last;
}

template<typename Traits>
template<typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR _Iter string_func<Traits>::skipz(_Iter _First, _Iter _Last) {
	while (_First != _Last && *_First == '0') ++_First;
	return _First;
}

// skip digit
template<typename Traits>
template<unsigned int Base, typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR _Iter string_func<Traits>::skipd(_Iter _First, _Iter _Last) {
	while (_First != _Last && isdigit<Base>(*_First)) ++_First;
	return _First;
}

template<typename Traits>
template<typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR _Iter string_func<Traits>
::skipd(_Iter _First, _Iter _Last, int Base) {
	while (_First != _Last && isdigit(*_First, Base)) ++_First;
	return _First;
}

template<typename Traits>
template<typename T, typename _Iter>
WJR_NODISCARD WJR_INLINE_CONSTEXPR T string_func<Traits>::to_integral(
	_Iter _First, _Iter _Last,
	_Iter& _Pos, error_code& _Err, int base) {
	return integral_conversion_details<T, string_func<Traits>>
		::template work(_First, _Last, _Pos, _Err, base);
}

_WJR_END

#endif // __WJR_STRING_FUNC_H