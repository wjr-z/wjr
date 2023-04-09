#pragma once
#ifndef __WJR_STRING_FUNC_H
#define __WJR_STRING_FUNC_H

// functions for string encode

#include <optional>

#include <wjr/math.h>
#include <wjr/error_handle.h>
#include <wjr/cvar.h>

_WJR_BEGIN

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

	template<unsigned int Base = 10>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static uint32_t todigit(value_type ch);

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

	enum flags {
		NO_FLAGS                   = 0,
		ALLOW_PREFIX               = 0x01,
		ALLOW_TAIL                 = 0x02,
		ALLOW_SIGN                 = 0x04,
		ALLOW_LEADING_SPACE        = 0x08,
		ALLOW_TRAILING_SPACE       = 0x10,
		ALLOW_LEADING_ZEROS        = 0x20,
	};

	template<typename T, typename _Iter, typename F>
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static T to_integral(
		F f,
		_Iter _First, _Iter _Last,
		errc& _Err, _Iter& _Pos, int base);

};

#define __CONV_EMPTY_RET				        \
		_Err = errc::noconv;					\
		return static_cast<T>(0);
#define __CONV_NEXT								\
		if(is_unlikely(++_First == _Last)){	    \
			__CONV_EMPTY_RET					\
		}

template<typename T, typename Func>
class integral_conversion_details {
public:
	using func_type = Func;
	using value_type = typename func_type::value_type;
	using flags = typename func_type::flags;

	template<typename _Iter, typename F>
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static T work(
		F f,
		_Iter _First, _Iter _Last,
		errc& _Err, _Iter& _Pos, int base) {
		const auto _Flags = get_cvar(f);

		_Err = errc::ok;

		// skip white space
		if (_Flags & flags::ALLOW_LEADING_SPACE) {
			_First = func_type::skipw(_First, _Last);
		}

		if (is_unlikely(_First == _Last)) {
			__CONV_EMPTY_RET;
		}

		bool _Is_minus = false;

		// eat sign
		if (_Flags & flags::ALLOW_SIGN) {
			auto ch = *_First;
			switch (ch) {
			case '+': {
				__CONV_NEXT;
				break;
			}
			case '-': {
				_Is_minus = true;
				__CONV_NEXT;
				break;
			}
			}
		}

		if (_Flags & flags::ALLOW_TRAILING_SPACE) {
			_First = func_type::skipw(_First, _Last);
		}

		constexpr auto _B = func_type::toalnum('B');
		constexpr auto _X = func_type::toalnum('X');

		WJR_MAYBE_UNUSED auto _Zero = _Last;

		if (_Flags & flags::ALLOW_PREFIX) {
			switch (base) {
			case 0: {
				if (*_First == '0') {
					// eat '0'
					if (is_unlikely(++_First == _Last)) {
						_Pos = _First;
						return static_cast<T>(0);
					}
					_Zero = _First;
					auto ch = func_type::toalnum(*_First);
					switch (ch) {
					case _B: {
						// eat 'b'/'B'
						if (is_unlikely(++_First == _Last)) {
							_Pos = _First;
							return static_cast<T>(0);
						}
						return __work(f, _First, _Last, _Err, _Pos, 2, _Zero, _Is_minus);
					}
					case _X: {
						// eat 'x'/'X'
						if (is_unlikely(++_First == _Last)) {
							_Pos = _First;
							return static_cast<T>(0);
						}
						return __work(f, _First, _Last, _Err, _Pos, 16, _Zero, _Is_minus);
					}
					default: {
						return __work(f, _First, _Last, _Err, _Pos, 8, _Zero, _Is_minus);
					}
					}
				}
				return __work(f, _First, _Last, _Err, _Pos, 10, _Last, _Is_minus);
			}
			case 2: {
				if (*_First == '0') {
					if (is_unlikely(++_First == _Last)) {
						_Pos = _First;
						return static_cast<T>(0);
					}
					_Zero = _First;
					if (func_type::toalnum(*_First) == _B) {
						if (is_unlikely(++_First == _Last)) {
							_Pos = _Zero;
							return static_cast<T>(0);
						}
					}
				}
				break;
			}
			case 8: {
				if (*_First == '0') {
					if (is_unlikely(++_First == _Last)) {
						_Pos = _First;
						return static_cast<T>(0);
					}
					_Zero = _First;
				}
				break;
			}
			case 16: {
				if (*_First == '0') {
					if (is_unlikely(++_First == _Last)) {
						_Pos = _First;
						return static_cast<T>(0);
					}
					_Zero = _First;
					if (func_type::toalnum(*_First) == _X) {
						if (is_unlikely(++_First == _Last)) {
							_Pos = _Zero;
							return static_cast<T>(0);
						}
					}
				}
				break;
			}
			default: {
				break;
			}
			}
		}

		return __work(f, _First, _Last, _Err, _Pos, base, _Zero, _Is_minus);
	}

private:

	using uT = std::make_unsigned_t<T>;

	constexpr static T min() { return std::numeric_limits<T>::min(); }
	constexpr static T max() { return std::numeric_limits<T>::max(); }

	constexpr static uT umax() { return std::max(make_unsigned_v(min()), make_unsigned_v(max())); }

	template<typename _Iter, typename F>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static T __work(
		F f,
		_Iter _First, _Iter _Last,
		errc& _Err, WJR_MAYBE_UNUSED _Iter& _Pos, 
		int base, _Iter _Zero, bool _Is_minus) {

		const auto _Flags = get_cvar(f);

		auto _Next = _First;

		if (_Flags & flags::ALLOW_LEADING_ZEROS) {
			_Next = func_type::skipz(_First, _Last);
		}

		uT _Max1 = 0;
		uT _Max2 = 0;

		if constexpr (std::is_unsigned_v<T>) {
			_Max1 = umax() / base;
			_Max2 = umax() % base;
		}
		else {
			if (_Is_minus) {
				_Max1 = umax() / base;
				_Max2 = umax() % base;
			}
			else {
				constexpr auto __umax2 = umax() - 1;
				_Max1 = __umax2 / base;
				_Max2 = __umax2 % base;
			}
		}

		uT uret = 0;
		bool overflowed = false;

		for (; _Next != _Last; ++_Next) {
			auto ch = func_type::toalnum(*_Next);

			if (ch >= base) break;

			if (uret < _Max1 || (uret == _Max1 && ch <= _Max2)) {
				uret *= base;
				uret += ch;
			}
			else overflowed = true;
		}

		if (_First == _Next) {
			if (_Zero != _Last) {
				_Pos = _Zero;
				return static_cast<T>(uret);
			}
			_Err = errc::noconv;
			return static_cast<T>(0);
		}

		_Pos = _Next;

		if (overflowed) {
			if constexpr (std::is_unsigned_v<T>) {
				_Err = errc::overflow;
			}
			else {
				if (_Is_minus) {
					_Err = errc::underflow;
				}
				else {
					_Err = errc::overflow;
				}
			}
			return static_cast<T>(0);
		}

		if (_Is_minus) {
			uret = static_cast<uT>(0 - uret);
		}

		return static_cast<T>(uret);
	}

};

#undef __CONV_EMPTY_RET
#undef __CONV_NEXT

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
template<unsigned int Base>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR uint32_t string_func<Traits>::todigit(value_type ch) {
	return traits_type::template todigit<Base>(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR uint32_t string_func<Traits>::toalnum(value_type ch) {
	return todigit<36>(ch);
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
template<typename T, typename _Iter, typename F>
WJR_NODISCARD WJR_INLINE_CONSTEXPR T string_func<Traits>::to_integral(
	F f,
	_Iter _First, _Iter _Last,
	errc& _Err, _Iter& _Pos, int base) {
	return integral_conversion_details<T, string_func<Traits>>
		::template work(f, _First, _Last, _Err, _Pos, base);
}

_WJR_END

#endif // __WJR_STRING_FUNC_H