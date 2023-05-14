#ifndef __WJR_STRING_FUNC_H
#define __WJR_STRING_FUNC_H

// functions for string encode

#include <optional>


#include <wjr/math.h>
#include <wjr/string.h>

#include <wjr/double-conversion/double-conversion.h>
#include <wjr/fast_float/fast_float.h>

// encode class

// TODO :
// optimize conversion

_WJR_BEGIN

template<typename _Iter>
struct chars_convert_result {
	_Iter ptr;
	std::errc ec;
};

// common functions for encode
// such as from_integral, from_floating_point
// Actually, functions unrelated to encoding
class __base_encode_common {
public:
	template<typename T, typename Char>
	WJR_INLINE_CONSTEXPR static chars_convert_result<Char*> from_integral(
		Char* const first, Char* const last, T value, int base) noexcept;

	using from_f_mode = typename double_conversion::DoubleToStringConverter::DtoaMode;
	using from_f_flags = typename double_conversion::DoubleToStringConverter::Flags;

	using default_from_f_mode = std::integral_constant<
		from_f_mode,
		from_f_mode::SHORTEST
	>;

	using default_from_f_flags = std::integral_constant<
		from_f_flags,
		from_f_flags::NO_FLAGS
	>;

	template<typename T, typename Char,
		typename M = default_from_f_mode,
		typename F = default_from_f_flags>
	WJR_INLINE static chars_convert_result<Char*> from_floating_point(
		Char* const first, Char* const last, T value, int precision = 6, M m = M(), F _Flags = F()) noexcept;

private:
	inline constexpr static char charconv_lower[] =
	{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e',
	'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
	'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

	inline constexpr static char charconv_upper[] =
	{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E',
	'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
};

template<typename T, typename Char>
WJR_INLINE_CONSTEXPR chars_convert_result<Char*> __base_encode_common::from_integral(
	Char* const first, Char* const last, T value, int base) noexcept {
	using uT = std::make_unsigned_t<T>;

	auto uval = make_unsigned_v(value);

	if constexpr (std::is_signed_v<T>) {
		if (value < 0) {
			if (WJR_UNLIKELY(first == last)) {
				return { first, std::errc::value_too_large };
			}
			*first++ = '-';
			uval = static_cast<uT>(0 - uval);
		}
	}

	constexpr size_t _Buff_size = sizeof(uT) * 8;
	char _Buff[_Buff_size];
	char* const _Buff_end = _Buff + _Buff_size;
	char* _RNext = _Buff_end;

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

	size_t _Digits_written = _Buff_end - _RNext;

	if (static_cast<size_t>(last - first) < _Digits_written) {
		return { first, std::errc::value_too_large };
	}

	memcpy(first, _RNext, _Digits_written * sizeof(Char));
	return { first + _Digits_written, std::errc{} };
}

template<typename T, typename Char,
	typename M,
	typename F>
WJR_INLINE chars_convert_result<Char*> __base_encode_common::from_floating_point(
	Char* const first, Char* const last, T value, int precision, M m, F _Flags) noexcept {
	using double_conversion::DoubleToStringConverter;
	using double_conversion::StringBuilder;
	DoubleToStringConverter conv(
		_Flags,
		"Infinity",
		"NaN",
		'E',
		-6,
		21,
		6, // max leading padding zeros
		1); // max trailing padding zeros
	char buffer[256];
	StringBuilder builder(buffer, sizeof(buffer));
	const auto mode = m;
	switch (mode) {
	case DoubleToStringConverter::SHORTEST:
		conv.ToShortest(value, &builder);
		break;
	case DoubleToStringConverter::SHORTEST_SINGLE:
		conv.ToShortestSingle(static_cast<float>(value), &builder);
		break;
	case DoubleToStringConverter::FIXED:
		conv.ToFixed(value, precision, &builder);
		break;
	case DoubleToStringConverter::PRECISION:
	default:
		conv.ToPrecision(value, precision, &builder);
		break;
	}

	auto _Digits_written = static_cast<size_t>(builder.position());

	if (static_cast<size_t>(last - first) < _Digits_written) {
		return { first, std::errc::value_too_large };
	}

	memcpy(first, buffer, _Digits_written * sizeof(Char));
	return { first + _Digits_written, std::errc{} };
}

template<typename Derived>
class __base_string_modifier : public __base_string_view<Derived> {
	using _Mybase = __base_string_view<Derived>;
	template<typename T>
	constexpr static bool _Is_noptr_string_view_like_v = _Mybase::template _Is_noptr_string_view_like_v<T>;
	using Char = typename _Mybase::value_type;
	using Traits = typename _Mybase::traits_type;
public:
	using value_type = typename _Mybase::value_type;
	using traits_type = typename _Mybase::traits_type;
	using reference = value_type&;
	using pointer = value_type*;
	using iterator = pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using size_type = typename _Mybase::size_type;

	using _Mybase::_Mybase;
	using _Mybase::operator=;

	using _Mybase::data;
	using _Mybase::size;

	WJR_INLINE_CONSTEXPR __base_string_modifier(const basic_string_view<Char, Traits> other)
		: _Mybase(other) {}

	WJR_INLINE_CONSTEXPR operator basic_string_view<Char, Traits>() const noexcept {
		return basic_string_view<Char, Traits>(*this);
	}

	WJR_INLINE_CONSTEXPR pointer data() { return const_cast<pointer>(_Mybase::data()); }
	WJR_INLINE_CONSTEXPR iterator begin() { return const_cast<iterator>(_Mybase::begin()); }
	WJR_INLINE_CONSTEXPR iterator end() { return const_cast<iterator>(_Mybase::end()); }

	WJR_INLINE_CONSTEXPR reverse_iterator rbegin() {
		return reverse_iterator(end());
	}
	WJR_INLINE_CONSTEXPR reverse_iterator rend() {
		return reverse_iterator(begin());
	}
	WJR_INLINE_CONSTEXPR reference front() {
		return *begin();
	}
	WJR_INLINE_CONSTEXPR reference back() {
		return *rbegin();
	}

	WJR_INLINE_CONSTEXPR reference operator[](size_type n) {
		return data()[n];
	}

	template<typename otherChar, typename otherTraits>
	WJR_INLINE_CONSTEXPR20 Derived append(basic_string_view<otherChar, otherTraits> str) {
		static_assert(sizeof(otherChar) <= sizeof(Char), "");
		wjr::uninitialized_copy_n(str.begin(), str.size(), data());
		return Derived(data() + str.size(), size() - str.size());
	}

	template<typename otherChar>
	WJR_INLINE_CONSTEXPR20 Derived append(size_type n, otherChar ch) {
		static_assert(sizeof(otherChar) <= sizeof(Char), "");
		wjr::uninitialized_fill_n(data(), n, ch);
		return Derived(data() + n, size() - n);
	}

	template<typename otherChar>
	WJR_INLINE_CONSTEXPR20 Derived push_back(otherChar ch) {
		static_assert(sizeof(otherChar) <= sizeof(Char), "");
		wjr::construct_at(data(), ch);
		return Derived(data() + 1, size() - 1);
	}

	// set begin to '\0'
	WJR_INLINE_CONSTEXPR void finalize() {
		front() = value_type();
	}

	template<typename T>
	WJR_INLINE void from_integral(T value, pointer* pos = nullptr, int base = 10, std::errc* err = nullptr) noexcept;

	using from_f_mode = typename __base_encode_common::from_f_mode;
	using from_f_flags = typename __base_encode_common::from_f_flags;

	using default_from_f_mode = typename __base_encode_common::default_from_f_mode;
	using default_from_f_flags = typename __base_encode_common::default_from_f_flags;

	template<typename T, typename M = default_from_f_mode, typename F = default_from_f_flags>
	WJR_INLINE void from_floating_point(T value,
		pointer* pos = nullptr,
		std::errc* err = nullptr,
		int precision = 6,
		M m = M(), F f = F()) noexcept;

protected:
	using _Mybase::derived;
};

template<typename Derived>
template<typename T>
WJR_INLINE void __base_string_modifier<Derived>::from_integral(
	T value, pointer* pos, int base, std::errc* err) noexcept {
	auto ret = __base_encode_common::from_integral(begin(), end(), value, base);
	if (pos != nullptr)*pos = ret.ptr;
	if (err != nullptr)*err = ret.ec;
}

template<typename Derived>
template<typename T, typename M, typename F>
WJR_INLINE void __base_string_modifier<Derived>::from_floating_point(T value,
	pointer* pos,
	std::errc* err,
	int precision,
	M m, F f) noexcept {
	auto ret = __base_encode_common::from_floating_point(begin(), end(), value, precision, m, f);
	if (pos != nullptr)*pos = ret.ptr;
	if (err != nullptr)*err = ret.ec;
}

// this is a simple string modifier
template<typename Char, typename Traits = std::char_traits<Char>>
class basic_string_modifier : public __base_string_modifier<basic_string_modifier<Char, Traits>> {
	using _Mybase = __base_string_modifier<basic_string_modifier<Char, Traits>>;
public:
	using value_type = typename _Mybase::value_type;
	using pointer = typename _Mybase::pointer;
	using const_pointer = typename _Mybase::const_pointer;

	using _Mybase::_Mybase;
	using _Mybase::operator=;

	WJR_INLINE_CONSTEXPR basic_string_modifier(const basic_string_view<Char, Traits> other)
		: _Mybase(other) {}

	WJR_INLINE_CONSTEXPR operator basic_string_view<Char, Traits>() const noexcept {}

	WJR_INLINE_CONSTEXPR basic_string_modifier(pointer ptr, size_t n)
		: _Mybase(const_cast<const_pointer>(ptr), n) {}

	WJR_INLINE_CONSTEXPR basic_string_modifier(pointer ptr)
		: _Mybase(const_cast<const_pointer>(ptr)) {}

};

using string_modifier = basic_string_modifier<char>;
using wstring_modifier = basic_string_modifier<wchar_t>;
using u16string_modifier = basic_string_modifier<char16_t>;
using u32string_modifier = basic_string_modifier<char32_t>;

namespace __string_encode_traits {
	WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(do_skipw, do_skipw);
	WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(do_rskipw, do_rskipw);
	WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(do_skipz, do_skipz);
}

// CRTP for encode
template<typename EncodeTraits>
class __base_encode : public __base_encode_common {
	using _Mybase = __base_encode_common;
public:
	using traits_type = EncodeTraits;
	using encode_type = typename traits_type::encode_type;
	using char_type = typename traits_type::char_type;
	using value_type = typename traits_type::value_type;
	using pointer = typename traits_type::pointer;
	using const_pointer = typename traits_type::const_pointer;
	using iterator = typename traits_type::iterator;
	using const_iterator = typename traits_type::const_iterator;

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isalnum(value_type ch);

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isalpha(value_type ch);

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool islower(value_type ch);

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isupper(value_type ch);

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isxdigit(value_type ch);

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isspace(value_type ch);

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static value_type tolower(value_type ch);

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static value_type toupper(value_type ch);

	// valid digits : [0, 36)
	// invalid digits : 0xff
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static uint8_t todigit(value_type ch);

	// same as todigit
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static uint8_t toalnum(value_type ch);

	// base <= 10, digit is ['0', '0' + base - 1]
// base > 10, digit is ['0', '9'] and ['a', 'a' + (base - 10) - 1] and ['A', 'A' + (base - 10) - 1]
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static bool isdigit(value_type ch, int Base = 10);

	// skip whit space
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static const_iterator
		skipw(const_iterator first, const_iterator last);

	// skip whit space at end
	// can only support bidirectional iterators
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static
		auto rskipw(const_iterator first, const_iterator last)
		-> std::enable_if_t<is_random_iterator_v<const_iterator>, const_iterator>;

	// skip leading zeros
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static const_iterator skipz(const_iterator first, const_iterator last);

	enum to_i_flags {
		NO_FLAGS = 0,
		ALLOW_PREFIX = 0x01,
		ALLOW_TAIL = 0x02,
		ALLOW_SIGN = 0x04,
		ALLOW_LEADING_SPACE = 0x08,
		ALLOW_SPACE_AFTER_SIGN = 0x10,
		ALLOW_LEADING_ZEROS = 0x20,
	};

	// ALLOW_PREFIX:
	//  prefix : base = 2 : 0b, base = 8 : 0, base = 16 : 0x
	// ALLOW_LEADING_SPACE:
	//	leading space is allowed, but not allowed after sign
	// ALLOW_SPACE_AFTER_SIGN:
	//	leading space is allowed after sign
	// ALLOW_LEADING_ZEROS:
	//  allow mutiple leading zeros

	using default_to_i_flags = std::integral_constant<to_i_flags,
		static_cast<to_i_flags>(
			ALLOW_PREFIX
			| ALLOW_TAIL
			| ALLOW_SIGN
			| ALLOW_LEADING_SPACE
			| ALLOW_LEADING_ZEROS)>;

	template<typename T, typename F = default_to_i_flags>
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static chars_convert_result<const_iterator> to_integral(
		const_iterator first, const_iterator last, T& value,
		int base, F f = F()) noexcept;

private:
	using _Mybase::from_integral;
public:
	template<typename T>
	WJR_INLINE_CONSTEXPR20 static chars_convert_result<char_type*> from_integral(
		char_type* const first, char_type* const last, T value, int base) noexcept;

	using to_f_flags = fast_float::chars_format;

	using default_to_f_flags = std::integral_constant<
		to_f_flags,
		to_f_flags::general
	>;

	template<typename T, typename F = default_to_f_flags>
	WJR_NODISCARD WJR_INLINE static chars_convert_result<const_iterator> to_floating_point(
		const_iterator first, const_iterator last, T& value, F f = F()) noexcept;

	using from_f_mode = typename _Mybase::from_f_mode;
	using from_f_flags = typename _Mybase::from_f_flags;

	using default_from_f_mode = typename _Mybase::default_from_f_mode;
	using default_from_f_flags = typename _Mybase::default_from_f_flags;

private:
	using _Mybase::from_floating_point;
public:

	template<typename T,
		typename M = default_from_f_mode,
		typename F = default_from_f_flags>
	WJR_INLINE_CONSTEXPR20 static chars_convert_result<char_type*> from_floating_point(
		char_type* const first, char_type* const last, T value, int precision = 6, M m = M(), F _Flags = F()) noexcept;

	WJR_INLINE_CONSTEXPR static iterator make_iterator(pointer);
	WJR_INLINE_CONSTEXPR static const_iterator make_iterator(const_pointer);

	WJR_INLINE_CONSTEXPR static pointer make_pointer(iterator);
	WJR_INLINE_CONSTEXPR static const_pointer make_pointer(const_iterator);
};

template<typename T, typename Encode>
class integral_conversion_details;

template<typename T, typename Encode>
class floating_point_conversion_details;

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool __base_encode<Traits>::isalnum(value_type ch) {
	return encode_type::isalnum(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool __base_encode<Traits>::isalpha(value_type ch) {
	return encode_type::isalpha(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool __base_encode<Traits>::islower(value_type ch) {
	return encode_type::islower(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool __base_encode<Traits>::isupper(value_type ch) {
	return encode_type::isupper(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool __base_encode<Traits>::isxdigit(value_type ch) {
	return encode_type::isxdigit(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool __base_encode<Traits>::isspace(value_type ch) {
	return encode_type::isspace(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR typename __base_encode<Traits>::value_type
__base_encode<Traits>::tolower(value_type ch) {
	return encode_type::tolower(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR typename __base_encode<Traits>::value_type
__base_encode<Traits>::toupper(value_type ch) {
	return encode_type::toupper(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR uint8_t __base_encode<Traits>::todigit(value_type ch) {
	return encode_type::todigit(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR uint8_t __base_encode<Traits>::toalnum(value_type ch) {
	return todigit(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR bool __base_encode<Traits>::isdigit(value_type ch, int Base) {
	return toalnum(ch) < Base;
}

template<typename Traits>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR typename __base_encode<Traits>::const_iterator
__base_encode<Traits>::skipw(const_iterator first, const_iterator last) {
	if constexpr (__string_encode_traits::has_static_member_function_do_skipw_v<encode_type, const_iterator, const_iterator>) {
		return encode_type::do_skipw(first, last);
	}
	else {
		while (first != last && isspace(*first)) ++first;
		return first;
	}
}

// skip whit space at end
template<typename Traits>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR auto
__base_encode<Traits>::rskipw(const_iterator first, const_iterator last)
-> std::enable_if_t<is_random_iterator_v<const_iterator>, const_iterator> {
	if constexpr (__string_encode_traits::
		has_static_member_function_do_rskipw_v<encode_type, const_iterator, const_iterator>) {
		return encode_type::do_rskipw(first, last);
	}
	else {
		while (first != last && isspace(*(last - 1))) --last;
		return last;
	}
}

template<typename Traits>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR typename __base_encode<Traits>::const_iterator
__base_encode<Traits>::skipz(const_iterator first, const_iterator last) {
	if constexpr (__string_encode_traits::has_static_member_function_do_skipz_v<encode_type, const_iterator, const_iterator>) {
		return encode_type::do_skipz(first, last);
	}
	else {
		while (first != last && *first == '0') ++first;
		return first;
	}
}

template<typename Traits>
template<typename T, typename F>
WJR_NODISCARD WJR_INLINE_CONSTEXPR chars_convert_result<typename __base_encode<Traits>::const_iterator> 
	__base_encode<Traits>::to_integral(
	const_iterator first, const_iterator last, T& value, int base, F f) noexcept {
	return integral_conversion_details<T, __base_encode<Traits>>
		::template to(first, last, value, base, f);
}

template<typename Traits>
template<typename T>
WJR_INLINE_CONSTEXPR20 chars_convert_result<typename __base_encode<Traits>::char_type*> __base_encode<Traits>::from_integral(
	char_type* const first, char_type* const last, T value, int base) noexcept{
	return __base_encode<Traits>::from_integral(first, last, value, base);
}

template<typename Traits>
template<typename T, typename F>
WJR_NODISCARD WJR_INLINE chars_convert_result<typename __base_encode<Traits>::const_iterator> 
	__base_encode<Traits>::to_floating_point(
	const_iterator first, const_iterator last, T& value, F f) noexcept {
	static_assert(std::is_same_v<const_iterator, const char*> , "only support chars now");
	const auto f_ = f;
	auto ret = fast_float::from_chars(first, last, value, f_);
	return { ret.ptr, ret.ec };
}

template<typename Traits>
template<typename T,
	typename M,
	typename F>
WJR_INLINE_CONSTEXPR20 chars_convert_result<typename __base_encode<Traits>::char_type*> 
	__base_encode<Traits>::from_floating_point(
	char_type* const first, char_type* const last, T value,
	int precision, M m, F f) noexcept {
	return _Mybase::from_floating_point(first, last, value, precision, m, f);
}

template<typename Traits>
WJR_INLINE_CONSTEXPR typename __base_encode<Traits>::iterator
__base_encode<Traits>::make_iterator(pointer ptr) {
	return encode_type::make_iterator(ptr);
}

template<typename Traits>
WJR_INLINE_CONSTEXPR typename __base_encode<Traits>::const_iterator
__base_encode<Traits>::make_iterator(const_pointer ptr) {
	return encode_type::make_iterator(ptr);
}

template<typename Traits>
WJR_INLINE_CONSTEXPR typename __base_encode<Traits>::pointer
__base_encode<Traits>::make_pointer(iterator ptr) {
	return encode_type::make_pointer(ptr);
}

template<typename Traits>
WJR_INLINE_CONSTEXPR typename __base_encode<Traits>::const_pointer
__base_encode<Traits>::make_pointer(const_iterator ptr) {
	return encode_type::make_pointer(ptr);
}

template<typename T, typename Encode>
class integral_conversion_details {
public:
	using encode_type = Encode;
	using char_type = typename encode_type::char_type;
	using value_type = typename encode_type::value_type;
	using to_i_flags = typename encode_type::to_i_flags;
	using iterator = typename encode_type::iterator;
	using const_iterator = typename encode_type::const_iterator;

	template<typename F>
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static chars_convert_result<const_iterator> to(
		const_iterator first, const_iterator last, T & value, int base, F f) noexcept;

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

	template<typename F>
	WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static chars_convert_result<const_iterator> __to(
		const_iterator first, const_iterator last, T& value, int base, F f, const_iterator _Zero, bool _Is_minus) noexcept;

};

#define __CONV_EMPTY_RET								\
		return {first, std::errc::invalid_argument};
#define __CONV_NEXT								\
		if(WJR_UNLIKELY(++first == last)){	    \
			__CONV_EMPTY_RET					\
		}

template<typename T, typename Func>
template<typename F>
WJR_NODISCARD WJR_INLINE_CONSTEXPR chars_convert_result<typename integral_conversion_details<T, Func>::const_iterator>
	integral_conversion_details<T, Func>::to(
	const_iterator first, const_iterator last, T& value, int base, F f) noexcept{

	static_assert(std::is_integral_v<T>, "");

	// skip white space
	if (f & to_i_flags::ALLOW_LEADING_SPACE) {
		first = encode_type::skipw(first, last);
	}

	if (WJR_UNLIKELY(first == last)) {
		__CONV_EMPTY_RET;
	}

	bool _Is_minus = false;

	// eat sign
	if (f & to_i_flags::ALLOW_SIGN) {
		auto ch = *first;
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

	if (f & to_i_flags::ALLOW_SPACE_AFTER_SIGN) {
		first = encode_type::skipw(first, last);
		if (WJR_UNLIKELY(first == last)) {
			__CONV_EMPTY_RET;
		}
	}

	constexpr auto _B = encode_type::toalnum('B');
	constexpr auto _X = encode_type::toalnum('X');

	// pos of first zero
	// maybe is zero of prefix
	// if no digits after prefix, then use this zero of prefix as simple zero
	WJR_MAYBE_UNUSED auto _Zero = last;

	if (f & to_i_flags::ALLOW_PREFIX) {
		switch (base) {
		case 0: {
			if (*first == '0') {
				// eat '0'
				if (WJR_UNLIKELY(++first == last)) {
					value = static_cast<T>(0);
					return { first, std::errc{} };
				}
				_Zero = first;
				auto ch = encode_type::toalnum(*first);
				switch (ch) {
				case _B: {
					// eat 'b'/'B'
					if (WJR_UNLIKELY(++first == last)) {
						value = static_cast<T>(0);
						return { first, std::errc{} };
					}
					return __to(first, last, value, 2, _Zero, _Is_minus);
				}
				case _X: {
					// eat 'x'/'X'
					if (WJR_UNLIKELY(++first == last)) {
						value = static_cast<T>(0);
						return { first, std::errc{} };
					}
					return __to(first, last, value, 16, _Zero, _Is_minus);
				}
				default: {
					return __to(first, last, value, 8, _Zero, _Is_minus);
				}
				}
			}
			return __to(first, last, value, 10, _Zero, _Is_minus);
		}
		case 2: {
			if (*first == '0') {
				if (WJR_UNLIKELY(++first == last)) {
					value = static_cast<T>(0);
					return { first, std::errc{} };
				}
				_Zero = first;
				if (encode_type::toalnum(*first) == _B) {
					if (WJR_UNLIKELY(++first == last)) {
						value = static_cast<T>(0);
						return { _Zero, std::errc{} };
					}
				}
			}
			break;
		}
		case 8: {
			if (*first == '0') {
				if (WJR_UNLIKELY(++first == last)) {
					value = static_cast<T>(0);
					return { first, std::errc{} };
				}
				_Zero = first;
			}
			break;
		}
		case 16: {
			if (*first == '0') {
				if (WJR_UNLIKELY(++first == last)) {
					value = static_cast<T>(0);
					return { first, std::errc{} };
				}
				_Zero = first;
				if (encode_type::toalnum(*first) == _X) {
					if (WJR_UNLIKELY(++first == last)) {
						value = static_cast<T>(0);
						return { _Zero, std::errc{} };
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

	return __to(first, last, value, base, f, _Zero, _Is_minus);
}

template<typename T, typename Func>
template<typename F>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR chars_convert_result<typename integral_conversion_details<T, Func>::const_iterator> 
	integral_conversion_details<T, Func>::__to(
	const_iterator first, const_iterator last, T& value, int base, F f, const_iterator _Zero, bool _Is_minus) noexcept {

	auto _Next = first;

	if (f & to_i_flags::ALLOW_LEADING_ZEROS) {
		_Next = encode_type::skipz(first, last);
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

	for (; _Next != last; ++_Next) {
		auto ch = encode_type::toalnum(*_Next);

		if (ch >= base) break;

		if (uret < _Max1 || (uret == _Max1 && ch <= _Max2)) {
			uret *= base;
			uret += ch;
		}
		else overflowed = true;
	}

	// no digits after
	if (first == _Next) {
		// if we have a zero in prefix
		if (_Zero != last) {
			value = static_cast<T>(0);
			return { _Zero, std::errc{} };
		}
		// else 
		__CONV_EMPTY_RET;
	}

	if (overflowed) {
		return { _Next, std::errc::result_out_of_range };
	}

	if (_Is_minus) {
		uret = static_cast<uT>(0 - uret);
	}

	value = static_cast<T>(uret);
	return { _Next, std::errc{} };
}

#undef __CONV_EMPTY_RET
#undef __CONV_NEXT

template<typename T, typename Encode>
class floating_point_conversion_details {
public:
	using encode_type = Encode;
	using value_type = typename encode_type::value_type;
	using to_f_flags = typename encode_type::to_f_flags;
	using iterator = typename encode_type::iterator;
	using const_iterator = typename encode_type::const_iterator;
};

class ascii;

struct ascii_traits;

struct ascii_traits {
	using encode_type = ascii;
	using char_type = char;
	using value_type = char;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using iterator = pointer;
	using const_iterator = const_pointer;
};

struct __ascii_to_digit_table {

	constexpr static uint8_t invalid = 0xff;

	constexpr __ascii_to_digit_table() : table() {
		for (int i = 0; i < 256; ++i) {
			table[i] = invalid;
		}

		for (int i = '0'; i <= '9'; ++i) {
			table[i] = (i - '0');
		}

		for (int i = 'a'; i <= 'z'; ++i) {
			table[i] = (i - 'a') + 10;
		}

		for (int i = 'A'; i <= 'Z'; ++i) {
			table[i] = (i - 'A') + 10;
		}

	}

	WJR_ATTRIBUTE(CONST, CONSTEXPR) uint8_t operator[](uint8_t ch) const { return table[ch]; }

private:
	std::array<uint8_t, 256> table;
};

constexpr static __ascii_to_digit_table __ascii_to_digit_table_v = {};

class ascii : public __base_encode<ascii_traits> {
public:

	friend __base_encode<ascii_traits>;
	using _Mybase = __base_encode<ascii_traits>;
	using traits_type = ascii_traits;
	using char_type = typename traits_type::char_type;
	using value_type = typename traits_type::value_type;
	using pointer = typename traits_type::pointer;
	using const_pointer = typename traits_type::const_pointer;
	using iterator = typename traits_type::iterator;
	using const_iterator = typename traits_type::const_iterator;

	enum __char_code {
		__none = 0x00,
		__lower = 0x01,
		__upper = 0x02,
		__digit = 0x04,
		__xdigit = 0x08,
		__white = 0x10
	};

	static constexpr int __char_code_table[256] = {
	  __none,          __none,          __none,          __none,
	  __none,          __none,          __none,          __none,
	  __none,         __white,         __white,         __white,
	 __white,         __white,          __none,          __none,
	  __none,          __none,          __none,          __none,
	  __none,          __none,          __none,          __none,
	  __none,          __none,          __none,          __none,
	  __none,          __none,          __none,          __none,
	 __white,          __none,          __none,          __none,
	  __none,          __none,          __none,          __none,
	  __none,          __none,          __none,          __none,
	  __none,          __none,          __none,          __none,
__digit | __xdigit,__digit | __xdigit,__digit | __xdigit,__digit | __xdigit,
__digit | __xdigit,__digit | __xdigit,__digit | __xdigit,__digit | __xdigit,
__digit | __xdigit,__digit | __xdigit,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,__upper | __xdigit,__upper | __xdigit,__upper | __xdigit,
__upper | __xdigit,__upper | __xdigit,__upper | __xdigit,         __upper,
		 __upper,         __upper,         __upper,         __upper,
		 __upper,         __upper,         __upper,         __upper,
		 __upper,         __upper,         __upper,         __upper,
		 __upper,         __upper,         __upper,         __upper,
		 __upper,         __upper,         __upper,          __none,
		  __none,          __none,          __none,          __none,
		  __none,__lower | __xdigit,__lower | __xdigit,__lower | __xdigit,
__lower | __xdigit,__lower | __xdigit,__lower | __xdigit,         __lower,
		 __lower,         __lower,         __lower,         __lower,
		 __lower,         __lower,         __lower,         __lower,
		 __lower,         __lower,         __lower,         __lower,
		 __lower,         __lower,         __lower,         __lower,
		 __lower,         __lower,         __lower,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none,
		  __none,          __none,          __none,          __none
	};

	WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static bool isalnum(value_type ch) {
		return __char_code_table[make_unsigned_v(ch)] & (__lower | __upper | __digit);
	}

	WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static bool isalpha(value_type ch) {
		return __char_code_table[make_unsigned_v(ch)] & (__lower | __upper);
	}

	WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static bool islower(value_type ch) {
		return __char_code_table[make_unsigned_v(ch)] & __lower;
	}

	WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static bool isupper(value_type ch) {
		return __char_code_table[make_unsigned_v(ch)] & __upper;
	}

	WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static bool isdigit(value_type ch) {
		return __char_code_table[make_unsigned_v(ch)] & __digit;
	}

	WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static bool isxdigit(value_type ch) {
		return __char_code_table[make_unsigned_v(ch)] & __xdigit;
	}

	WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static bool isspace(value_type ch) {
		return __char_code_table[make_unsigned_v(ch)] & __white;
	}

	WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static value_type tolower(value_type ch) {
		return isupper(ch) ? ch + 'a' - 'A' : ch;
	}

	WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static value_type toupper(value_type ch) {
		return islower(ch) ? ch + 'A' - 'a' : ch;
	}

	// return -1 if is invalid
	WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static uint8_t todigit(char ch) {
		return __ascii_to_digit_table_v[ch];
	}

	WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) static const_iterator do_skipw(const_iterator s, const_iterator e) {
		return wjr::skip_whitespace(s, e);
	}

	WJR_INLINE_CONSTEXPR static iterator make_iterator(pointer ptr) { return ptr; }
	WJR_INLINE_CONSTEXPR static const_iterator make_iterator(const_pointer ptr) { return ptr; }

	WJR_INLINE_CONSTEXPR static pointer make_pointer(iterator ptr) { return ptr; }
	WJR_INLINE_CONSTEXPR static const_pointer make_pointer(const_iterator ptr) { return ptr; }

};

// TODO
// Encoding and decoding in previous github files
class utf8 {
public:
};

// encode may have difference iterator type
// and the encode_value_type is value_type of encode_iterator
template<typename Derived, typename Encode>
class __base_encode_view {
private:
	using _Mybase = Derived;
	using _Mytraits = __base_string_view_traits<Derived>;
	using Char = typename _Mytraits::value_type;
	using Traits = typename _Mytraits::traits_type;
public:
	using encode_type = Encode;
	using encode_iterator = typename encode_type::iterator;
	using encode_const_iterator = typename encode_type::const_iterator;
	using encode_value_type = typename encode_type::value_type;

	using pointer = typename _Mytraits::pointer;
	using const_pointer = typename _Mytraits::const_pointer;
	using iterator = typename _Mytraits::iterator;
	using const_iterator = typename _Mytraits::const_iterator;

#define _WJR_REGISTER_ENCODE_VIEW_DERIVED_FUNC(name, ret_type, type)	\
	WJR_INLINE_CONSTEXPR static ret_type name(type arg){return encode_type::name(arg);}

#define WJR_REGISTER_ENCODE_VIEW_DERIVED_FUNC(x) _WJR_REGISTER_ENCODE_VIEW_DERIVED_FUNC x

	WJR_MACRO_CALL(WJR_REGISTER_ENCODE_VIEW_DERIVED_FUNC, ,
		(isalnum, bool, encode_value_type),
		(isalpha, bool, encode_value_type),
		(islower, bool, encode_value_type),
		(isupper, bool, encode_value_type),
		(isxdigit, bool, encode_value_type),
		(isspace, bool, encode_value_type),
		(tolower, encode_value_type, encode_value_type),
		(toupper, encode_value_type, encode_value_type),
		(todigit, uint8_t, encode_value_type),
		(toalnum, uint8_t, encode_value_type),
		(isdigit, bool, encode_value_type),
		(make_iterator, encode_iterator, pointer),
		(make_iterator, encode_const_iterator, const_pointer),
		(make_pointer, pointer, encode_iterator),
		(make_pointer, const_pointer, encode_const_iterator)
	);

#undef WJR_REGISTER_ENCODE_VIEW_DERIVED_FUNC
#undef _WJR_REGISTER_ENCODE_VIEW_DERIVED_FUNC

	WJR_INLINE_CONSTEXPR encode_const_iterator encode_begin() const { return make_iterator(derived().begin()); }
	WJR_INLINE_CONSTEXPR encode_const_iterator encode_end() const { return make_iterator(derived().end()); }

	WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR) __base_encode_view ltrim() const;

	WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR)auto rtrim() const
		->std::enable_if_t<is_random_iterator_v<encode_const_iterator>, __base_encode_view>;
	WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR)auto trim() const
		->std::enable_if_t<is_random_iterator_v<encode_const_iterator>, __base_encode_view>;

	using to_i_flags = typename encode_type::to_i_flags;
	using default_to_i_flags = typename encode_type::default_to_i_flags;

	template<typename T, typename F = default_to_i_flags>
	WJR_ATTRIBUTE(NODISCARD, INLINE) T to_integral(
		const_iterator* _Pos = nullptr, int base = 10, std::errc* _Err = nullptr, F f = F()) const noexcept;


#define _WJR_REGISTER_STRING_TO_INTEGRAL_FUNC(T, name)													\
	template<typename F = default_to_i_flags>	                                                        \
	WJR_NODISCARD WJR_INLINE T to_##name(																\
	const_iterator* _Pos = nullptr, int base = 10, std::errc* _Err = nullptr, F f = F()) const noexcept{\
		return this->to_integral<T>(_Pos, base, _Err, f);	                                            \
	}
#define WJR_REGISTER_STRING_TO_INTEGRAL_FUNC(x) _WJR_REGISTER_STRING_TO_INTEGRAL_FUNC x

	WJR_MACRO_CALL(WJR_REGISTER_STRING_TO_INTEGRAL_FUNC, ,
		(char, char),
		(signed char, schar),
		(unsigned char, uchar),
		(short, short),
		(unsigned short, ushort),
		(int, int),
		(unsigned int, uint),
		(long, long),
		(unsigned long, ulong),
		(long long, llong),
		(unsigned long long, ullong)
	);

#undef WJR_REGISTER_STRING_TO_INTEGRAL_FUNC
#undef _WJR_REGISTER_STRING_TO_INTEGRAL_FUNC

	using to_f_flags = typename encode_type::to_f_flags;
	using default_to_f_flags = typename encode_type::default_to_f_flags;

	template<typename T, typename F = default_to_f_flags>
	WJR_ATTRIBUTE(NODISCARD, INLINE) T to_floating_point(
		const_iterator* _Pos = nullptr, std::errc* _Err = nullptr, F f = F()) const noexcept;

#define _WJR_REGISTER_STRING_TO_FLOATING_POINT_FUNC(T, name)	                                            \
	template<typename F = default_to_f_flags>	                                                            \
	WJR_NODISCARD WJR_INLINE T to_##name(																	\
	const_iterator* _Pos = nullptr, std::errc* _Err = nullptr, F f = F()) const noexcept{	                \
		return this->to_floating_point<T>(_Pos, _Err, f);	                                                \
	}
#define WJR_REGISTER_STRING_TO_FLOATING_POINT_FUNC(x) _WJR_REGISTER_STRING_TO_FLOATING_POINT_FUNC x

	WJR_MACRO_CALL(WJR_REGISTER_STRING_TO_FLOATING_POINT_FUNC, ,
		(float, float),
		(double, double)
	);

#undef WJR_REGISTER_STRING_TO_FLOATING_POINT_FUNC
#undef _WJR_REGISTER_STRING_TO_FLOATING_POINT_FUNC

protected:
	WJR_INTRINSIC_CONSTEXPR Derived& derived() noexcept { return static_cast<Derived&>(*this); }
	WJR_INTRINSIC_CONSTEXPR const Derived& derived() const noexcept { return static_cast<const Derived&>(*this); }
};


template<typename Derived, typename Encode>
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR) __base_encode_view<Derived, Encode>
__base_encode_view<Derived, Encode>::ltrim() const {
	return this->adjust_begin(encode_type::make_pointer(
		encode_type::skipw(encode_begin(), encode_end())));
}

template<typename Derived, typename Encode>
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR)
auto __base_encode_view<Derived, Encode>::rtrim() const
->std::enable_if_t<is_random_iterator_v<
	typename __base_encode_view<Derived, Encode>::encode_const_iterator>, __base_encode_view<Derived, Encode>> {
	return this->adjust_end(encode_type::make_pointer(
		encode_type::rskipw(encode_begin(), encode_end())));
}

template<typename Derived, typename Encode>
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR)
auto __base_encode_view<Derived, Encode>::trim() const
->std::enable_if_t<is_random_iterator_v<
	typename __base_encode_view<Derived, Encode>::encode_const_iterator>, __base_encode_view<Derived, Encode>> {
	return ltrim().rtrim();
}

template<typename Derived, typename Encode>
template<typename T, typename F>
WJR_ATTRIBUTE(NODISCARD, INLINE) T __base_encode_view<Derived, Encode>::to_integral(
	const_iterator* pos, int base, std::errc* err, F f) const noexcept {
	T value;
	auto ret = encode_type::template to_integral<T>(encode_begin(), encode_end(), value, base, f);
	if (pos != nullptr)*pos = ret.ptr;
	if (err != nullptr)*err = ret.ec;
	return value;
}

template<typename Derived, typename Encode>
template<typename T, typename F>
WJR_ATTRIBUTE(NODISCARD, INLINE) T __base_encode_view<Derived, Encode>::to_floating_point(
	const_iterator* pos, std::errc* err, F f) const noexcept {
	T value;
	auto ret = encode_type::template to_floating_point<T>(encode_begin(), encode_end(), value, f);
	if (pos != nullptr)*pos = ret.ptr;
	if (err != nullptr)*err = ret.ec;
	return value;
}

template<typename Char, typename Traits = std::char_traits<Char>>
class basic_encode_view;

template<>
class basic_encode_view<char, std::char_traits<char>>
	:
	public __base_string_view< basic_encode_view<char, std::char_traits<char>>>,
	public __base_encode_view<basic_encode_view<char, std::char_traits<char>>, ascii> {
	using _Mybase1 = __base_string_view< basic_encode_view<char, std::char_traits<char>>>;
	using _Mybase2 = __base_encode_view<basic_encode_view<char, std::char_traits<char>>, ascii>;
	using Char = char;
	using Traits = std::char_traits<char>;
public:

	using _Mybase1::_Mybase1;
	using _Mybase1::operator=;

	WJR_INLINE_CONSTEXPR basic_encode_view(const basic_string_view<Char, Traits> other)
		: _Mybase1(other) {}

	WJR_INLINE_CONSTEXPR operator basic_string_view<Char, Traits>() const noexcept {
		return basic_string_view<Char, Traits>(*this);
	}

};

template<typename Derived, typename Encode>
class __base_encode_modifier
	: public __base_string_modifier<Derived>,
	public __base_encode_view<Derived, Encode> {
	using _Mybase1 = __base_string_modifier<Derived>;
	using _Mybase2 = __base_encode_view<Derived, Encode>;
	using _Mytraits = __base_string_view_traits<Derived>;
	using Char = typename _Mytraits::value_type;
	using Traits = typename _Mytraits::traits_type;
public:
	using encode_type = typename _Mybase2::encode_type;

	using pointer = typename _Mybase1::pointer;
	using const_pointer = typename _Mybase1::const_pointer;
	using iterator = typename _Mybase1::iterator;
	using const_iterator = typename _Mybase1::const_iterator;

	using _Mybase1::_Mybase1;
	using _Mybase1::operator=;

	using _Mybase1::data;
	using _Mybase1::size;

	using _Mybase1::begin;
	using _Mybase1::end;

	WJR_INLINE_CONSTEXPR20 void tolower() noexcept;
	WJR_INLINE_CONSTEXPR20 void toupper() noexcept;

};

template<typename Derived, typename Encode>
WJR_INLINE_CONSTEXPR20 void __base_encode_modifier<Derived, Encode>::tolower() noexcept {
	auto first = make_iterator(begin());
	auto last = make_iterator(end());
	for (; first != last; ++first) {
		*first = encode_type::tolower(*first);
	}
}

template<typename Derived, typename Encode>
WJR_INLINE_CONSTEXPR20 void __base_encode_modifier<Derived, Encode>::toupper() noexcept {
	auto first = make_iterator(begin());
	auto last = make_iterator(end());
	for (; first != last; ++first) {
		*first = encode_type::toupper(*first);
	}
}

template<typename Char, typename Traits = std::char_traits<Char>>
class basic_encode_modifier;

template<>
class basic_encode_modifier<char, std::char_traits<char>>
	: public __base_encode_modifier<basic_encode_modifier<char, std::char_traits<char>>, ascii> {
	using _Mybase0 = __base_encode_modifier<basic_encode_modifier<char, std::char_traits<char>>, ascii>;
	using Char = typename _Mybase0::value_type;
	using Traits = typename _Mybase0::traits_type;
public:
	using _Mybase0::_Mybase0;
	using _Mybase0::operator=;

	WJR_INLINE_CONSTEXPR basic_encode_modifier(const basic_string_view<Char, Traits> other)
		: _Mybase0(other) {}

	WJR_INLINE_CONSTEXPR basic_encode_modifier(const basic_string_modifier<Char, Traits> other)
		: _Mybase0(other) {}

	WJR_INLINE_CONSTEXPR basic_encode_modifier(const basic_encode_view<Char, Traits> other)
		: _Mybase0(other) {}

	WJR_INLINE_CONSTEXPR operator basic_string_view<Char, Traits>() const noexcept {
		return basic_string_view<Char, Traits>(*this);
	}

	WJR_INLINE_CONSTEXPR operator basic_string_modifier<Char, Traits>() const noexcept {
		return basic_string_modifier<Char, Traits>(*this);
	}

	WJR_INLINE_CONSTEXPR operator basic_encode_view<Char, Traits>() const noexcept {
		return basic_encode_view<Char, Traits>(*this);
	}
};

using ascii_encode = ascii;
using ascii_view = basic_encode_view<char>;
using ascii_modifier = basic_encode_modifier<char>;

_WJR_END

#endif // __WJR_STRING_FUNC_H