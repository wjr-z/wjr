#pragma once
#ifndef __WJR_STRING_FUNC_H
#define __WJR_STRING_FUNC_H

// functions for string encode

#include <optional>

#include <wjr/math.h>
#include <wjr/error_handle.h>
#include <wjr/cvar.h>

_WJR_BEGIN

namespace __string_func_traits {
	WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(__skipw, skipw);
	WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(__rskipw, rskipw);
	WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(__skipz, skipz);
	WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(__skipd, skipd);
}

template<typename Traits>
class string_func {
public:
	using traits_type = Traits;
	using encode_type = typename traits_type::encode_type;
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
		ALLOW_SPACE_AFTER_SIGN     = 0x10,
		ALLOW_LEADING_ZEROS        = 0x20,
	};

	// ALLOW_PREFIX:
	//  prefix : base = 2 : 0b, base = 8 : 0, base = 16 : 0x
	// ALLOW_LEADING_SPACE:
	//	leading space is allowed, but not allowed after sign
	// ALLOW_SPACE_AFTER_SIGN:
	//	leading space is allowed after sign
	// ALLOW_LEADING_ZEROS:
	//  allow mutiple leading zeros

	using default_to_integral_flags = std::integral_constant<flags, 
		static_cast<flags>(flags::ALLOW_PREFIX
				| flags::ALLOW_TAIL
				| flags::ALLOW_SIGN
				| flags::ALLOW_LEADING_SPACE
				| flags::ALLOW_LEADING_ZEROS)>;

	template<typename T, typename _Iter, typename F = default_to_integral_flags>
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static T to_integral(
		_Iter _First, _Iter _Last,
		_Iter& _Pos, int base, errc& _Err, F f = F()) noexcept;

	template<typename T, typename _Iter>
	WJR_INLINE_CONSTEXPR20 static void from_integral(
		T _Val, _Iter _First, _Iter _Last,
		_Iter& _Pos, int base, errc& _Err) noexcept;

	template<typename T, typename _Diff, typename _Iter>
	WJR_INLINE_CONSTEXPR20 static void from_integral(
		T _Val, _Iter _First, _Diff n,
		_Iter& _Pos, int base, errc& _Err) noexcept;

private:
};

template<typename T, typename Func>
class integral_conversion_details {
public:
	using func_type = Func;
	using value_type = typename func_type::value_type;
	using flags = typename func_type::flags;

	template<typename _Iter, typename F>
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static T to(
		_Iter _First, _Iter _Last,
		_Iter& _Pos, int base, errc& _Err, F f) noexcept;

	// copy of std::to_chars
	template <typename _Iter>
	WJR_INLINE_CONSTEXPR20 static void from(
		T _Val, _Iter _First, _Iter _Last,
		_Iter& _Pos, int base, errc& _Err) noexcept;

	template <typename _Iter, typename _Diff>
	WJR_INLINE_CONSTEXPR20 static void from(
		T _Val, _Iter _First, _Diff n,
		_Iter& _Pos, int base, errc& _Err) noexcept;

private:

	inline constexpr static char charconv_lower[] = 
	{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e',
	'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 
	'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

	inline constexpr static char charconv_upper[] =
	{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E',
	'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };

	using uT = std::make_unsigned_t<T>;

	constexpr static T min() { return std::numeric_limits<T>::min(); }
	constexpr static T max() { return std::numeric_limits<T>::max(); }

	constexpr static uT umax() { return std::max(make_unsigned_v(min()), make_unsigned_v(max())); }

	template<typename _Iter, typename F>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static T __to(
		_Iter _First, _Iter _Last,
		WJR_MAYBE_UNUSED _Iter& _Pos,
		int base, errc& _Err, F f, _Iter _Zero, bool _Is_minus) noexcept;

	WJR_INLINE_CONSTEXPR20 static void __from(
		uT uval, const char*& _RNext, int base) noexcept;

};

#define __CONV_EMPTY_RET				        \
		_Err = errc::noconv;					\
		return static_cast<T>(0);
#define __CONV_NEXT								\
		if(is_unlikely(++_First == _Last)){	    \
			__CONV_EMPTY_RET					\
		}

template<typename T, typename Func>
template<typename _Iter, typename F>
WJR_NODISCARD WJR_INLINE_CONSTEXPR T integral_conversion_details<T, Func>::to(
	_Iter _First, _Iter _Last,
	_Iter& _Pos, int base, errc& _Err, F f) noexcept {
	const auto _Flags = get_cvar(f);

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

	if (_Flags & flags::ALLOW_SPACE_AFTER_SIGN) {
		_First = func_type::skipw(_First, _Last);
		if (is_unlikely(_First == _Last)) {
			__CONV_EMPTY_RET;
		}
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
					return __to(_First, _Last, _Pos, 2, _Err, f, _Zero, _Is_minus);
				}
				case _X: {
					// eat 'x'/'X'
					if (is_unlikely(++_First == _Last)) {
						_Pos = _First;
						return static_cast<T>(0);
					}
					return __to(_First, _Last, _Pos, 16, _Err, f, _Zero, _Is_minus);
				}
				default: {
					return __to(_First, _Last, _Pos, 8, _Err, f, _Zero, _Is_minus);
				}
				}
			}
			return __to(_First, _Last, _Pos, 10, _Err, f, _Last, _Is_minus);
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

	return __to(_First, _Last, _Pos, base, _Err, f, _Zero, _Is_minus);
}

template<typename T, typename Func>
template<typename _Iter, typename F>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR T integral_conversion_details<T, Func>::__to(
	_Iter _First, _Iter _Last,
	WJR_MAYBE_UNUSED _Iter& _Pos,
	int base, errc& _Err, F f, _Iter _Zero, bool _Is_minus) noexcept {

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
		__CONV_EMPTY_RET;
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

	_Err = errc::ok;

	if (_Is_minus) {
		uret = static_cast<uT>(0 - uret);
	}

	return static_cast<T>(uret);
}

#undef __CONV_EMPTY_RET
#undef __CONV_NEXT

template<typename T, typename Func>
WJR_INLINE_CONSTEXPR20 void integral_conversion_details<T, Func>::__from(
	uT uval, const char*& _RNext, int base) noexcept {
	switch (base) {
	case 10:
	{ // Derived from _UIntegral_to_buff()
		// Performance note: Ryu's digit table should be faster here.
		constexpr bool _Use_chunks = sizeof(uT) > sizeof(size_t);

		if constexpr (_Use_chunks) { // For 64-bit numbers on 32-bit platforms, work in chunks to avoid 64-bit
			// divisions.
			while (uval > 0xFFFF'FFFFU) {
				// Performance note: Ryu's division toaround would be faster here.
				unsigned long _Chunk = static_cast<unsigned long>(uval % 1'000'000'000);
				uval = static_cast<uT>(uval / 1'000'000'000);

				for (int _Idx = 0; _Idx != 9; ++_Idx) {
					*--_RNext = static_cast<char>('0' + _Chunk % 10);
					_Chunk /= 10;
				}
			}
		}

		using _Truncated = std::conditional_t<_Use_chunks, unsigned long, uT>;

		_Truncated _Trunc = static_cast<_Truncated>(uval);

		do {
			*--_RNext = static_cast<char>('0' + _Trunc % 10);
			_Trunc /= 10;
		} while (_Trunc != 0);
		break;
	}

	case 2:
		do {
			*--_RNext = static_cast<char>('0' + (uval & 0b1));
			uval >>= 1;
		} while (uval != 0);
		break;

	case 4:
		do {
			*--_RNext = static_cast<char>('0' + (uval & 0b11));
			uval >>= 2;
		} while (uval != 0);
		break;

	case 8:
		do {
			*--_RNext = static_cast<char>('0' + (uval & 0b111));
			uval >>= 3;
		} while (uval != 0);
		break;

	case 16:
		do {
			*--_RNext = charconv_lower[uval & 0b1111];
			uval >>= 4;
		} while (uval != 0);
		break;

	case 32:
		do {
			*--_RNext = charconv_lower[uval & 0b11111];
			uval >>= 5;
		} while (uval != 0);
		break;

	case 3:
	case 5:
	case 6:
	case 7:
	case 9:
		do {
			*--_RNext = static_cast<char>('0' + uval % base);
			uval = static_cast<uT>(uval / base);
		} while (uval != 0);
		break;

	default:
		do {
			*--_RNext = charconv_lower[uval % base];
			uval = static_cast<uT>(uval / base);
		} while (uval != 0);
		break;
	}
}

template<typename T, typename Func>
template <typename _Iter>
WJR_INLINE_CONSTEXPR20 void integral_conversion_details<T, Func>::from(
	T _Val, _Iter _First, _Iter _Last,
	_Iter& _Pos, int base, errc& _Err) noexcept {

	auto uval = make_unsigned_v(_Val);

	if constexpr (std::is_signed_v<T>) {
		if (_Val < 0) {
			if (_First == _Last) {
				_Err = errc::buffer_too_small;
				return;
			}
			*_First = '-';
			++_First;
			uval = static_cast<uT>(0 - uval);
		}
	}

	constexpr size_t _Buff_size = sizeof(uT) * 8;
	value_type _Buff[_Buff_size];
	char* const _Buff_end = _Buff + _Buff_size;
	char* _RNext = _Buff_end;

	__from(uval, _RNext, base);

	ptrdiff_t _Digits_written = _Buff_end - _RNext;

	if constexpr (is_random_iterator_v<_Iter>) {
		const auto _Size = std::distance(_First, _Last);
		if (_Size < _Digits_written) {
			_Err = errc::buffer_too_small;
			return;
		}
		_First = wjr::copy_n(_RNext, _Digits_written, _First);
	}
	else {
		for (; _Digits_written && _First != _Last; ++_RNext, ++_First, --_Digits_written) {
			*_First = *_RNext;
		}
		if (_Digits_written) {
			_Err = errc::buffer_too_small;
			return;
		}
	}

	_Pos = _First;
	_Err = errc::ok;
	return;
}

template<typename T, typename Func>
template <typename _Iter, typename _Diff>
WJR_INLINE_CONSTEXPR20 void integral_conversion_details<T, Func>::from(
	T _Val, _Iter _First, _Diff n,
	_Iter& _Pos, int base, errc& _Err) noexcept {

	if (n <= 0) {
		_Err = errc::buffer_too_small;
		return;
	}

	auto uval = make_unsigned_v(_Val);

	if constexpr (std::is_signed_v<T>) {
		if (_Val < 0) {
			*_First = '-';
			++_First;
			--n;
			uval = static_cast<uT>(0 - uval);
		}
	}

	constexpr size_t _Buff_size = sizeof(uT) * 8;
	value_type _Buff[_Buff_size];
	char* const _Buff_end = _Buff + _Buff_size;
	char* _RNext = _Buff_end;

	__from(uval, _RNext, base);

	ptrdiff_t _Digits_written = _Buff_end - _RNext;

	if (n < _Digits_written) {
		_Err = errc::buffer_too_small;
		return;
	}

	_First = wjr::copy_n(_RNext, _Digits_written, _First);

	_Pos = _First;
	_Err = errc::ok;
	return;
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::isalnum(value_type ch) {
	return encode_type::isalnum(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::isalpha(value_type ch) {
	return encode_type::isalpha(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::islower(value_type ch) {
	return encode_type::islower(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::isupper(value_type ch) {
	return encode_type::isupper(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::isxdigit(value_type ch) {
	return encode_type::isxdigit(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::isspace(value_type ch) {
	return encode_type::isspace(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR typename string_func<Traits>::value_type
string_func<Traits>::tolower(value_type ch) {
	return encode_type::tolower(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR typename string_func<Traits>::value_type
string_func<Traits>::toupper(value_type ch) {
	return encode_type::toupper(ch);
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
	return encode_type::template todigit<Base>(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR uint32_t string_func<Traits>::toalnum(value_type ch) {
	return todigit<36>(ch);
}

template<typename Traits>
template<typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR _Iter string_func<Traits>::skipw(_Iter _First, _Iter _Last) {
	if constexpr (__string_func_traits::has_static_member_function_skipw_v<encode_type, _Iter, _Iter>) {
		return encode_type::skipw(_First, _Last);
	}
	else {
		while (_First != _Last && isspace(*_First)) ++_First;
		return _First;
	}
}

// skip whit space at end
template<typename Traits>
template<typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR _Iter string_func<Traits>::rskipw(_Iter _First, _Iter _Last) {
	if constexpr (__string_func_traits::has_static_member_function_rskipw_v<encode_type, _Iter, _Iter>) {
		return encode_type::rskipw(_First, _Last);
	}
	else {
		while (_First != _Last && isspace(*(_Last - 1))) --_Last;
		return _Last;
	}
}

template<typename Traits>
template<typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR _Iter string_func<Traits>::skipz(_Iter _First, _Iter _Last) {
	if constexpr (__string_func_traits::has_static_member_function_skipz_v<encode_type, _Iter, _Iter>) {
		return encode_type::skipz(_First, _Last);
	}
	else {
		while (_First != _Last && *_First == '0') ++_First;
		return _First;
	}
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
	if constexpr (__string_func_traits::has_static_member_function_skipz_v<encode_type, _Iter, _Iter, int>) {
		return encode_type::skipz(_First, _Last, Base);
	}
	else {
		while (_First != _Last && isdigit(*_First, Base)) ++_First;
		return _First;
	}
}

template<typename Traits>
template<typename T, typename _Iter, typename F>
WJR_NODISCARD WJR_INLINE_CONSTEXPR T string_func<Traits>::to_integral(
	_Iter _First, _Iter _Last,
	_Iter& _Pos, int base, errc& _Err, F f) noexcept {
	return integral_conversion_details<T, string_func<Traits>>
		::template to(_First, _Last, _Pos, base, _Err, f);
}

template<typename Traits>
template<typename T, typename _Iter>
WJR_INLINE_CONSTEXPR20 void string_func<Traits>::from_integral(
	T _Val, _Iter _First, _Iter _Last,
	_Iter& _Pos, int base, errc& _Err) noexcept {
	return integral_conversion_details<T, string_func<Traits>>
		::template from(_Val, _First, _Last, _Pos, base, _Err);
}

template<typename Traits>
template<typename T, typename _Diff, typename _Iter>
WJR_INLINE_CONSTEXPR20 void string_func<Traits>::from_integral(
	T _Val, _Iter _First, _Diff n,
	_Iter& _Pos, int base, errc& _Err) noexcept {
	return integral_conversion_details<T, string_func<Traits>>
		::template from(_Val, _First, n, _Pos, base, _Err);
}

_WJR_END

#endif // __WJR_STRING_FUNC_H