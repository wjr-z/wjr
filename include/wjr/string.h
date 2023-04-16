
#pragma once
#ifndef __WJR_STRING_H
#define __WJR_STRING_H

#include <string_view>
#include <iostream>
#include <cctype>
#include <locale>
#include <cstring>
#include <charconv>
#include <functional>

#include <wjr/vector.h>
#include <wjr/math.h>

#include <wjr/string_func.h>

_WJR_BEGIN

/*---------------------------data for string---------------------------*/

// Please note that string always has a '\0' terminator, so the capacity of string
// should be the capacity of the container - 1, 
// by inheriting vector_sso_data and setting the virtual capacity to the actual capacity - 1
// All operations on string_data are performed only through the virtual capacity, 
// and the virtual capacity is converted to the actual capacity inside string_data
// virtual size is equal to actual size, so EOF needs to be handled manually
// virtual size = actual size <= virtual capacity = actual capacity - 1 < actual capacity

template<typename Char, size_t N, typename Alloc>
struct string_static_data : public vector_static_data<Char, N, Alloc> {
	using _Mybase = vector_static_data<Char, N, Alloc>;

	using _Alty = typename _Mybase::_Alty;
	using _Alty_traits = typename _Mybase::_Alty_traits;
	using value_type = typename _Mybase::value_type;
	using pointer = typename _Mybase::pointer;
	using const_pointer = typename _Mybase::const_pointer;
	using size_type = typename _Mybase::size_type;
	using difference_type = typename _Mybase::difference_type;

	// virtual max capacity = max capacity - 1
	constexpr static size_t max_capacity = _Mybase::max_capacity - 1;

	WJR_INLINE_CONSTEXPR20 string_static_data() : _Mybase() {}
	string_static_data(const string_static_data&) = delete;
	string_static_data& operator=(const string_static_data&) = delete;
	WJR_CONSTEXPR20 ~string_static_data() = default;

	WJR_CONSTEXPR20 string_static_data(
		_Alty& al,
		const size_type _Newsize,
		const size_type _Newcapacity,
		extend_tag)
		: _Mybase(al, _Newsize, _Newcapacity + 1, extend_tag{}) {
	}

	WJR_CONSTEXPR20 string_static_data(
		_Alty& al,
		const size_type _Newsize,
		const size_type _Newcapacity)
		: _Mybase(al, _Newsize, _Newcapacity + 1) {
	}

	WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const string_static_data& _Src, string_static_data& _Dest) {
		return _Mybase::copyConstruct(al, std::move(_Src), _Dest);
	}

	WJR_INLINE_CONSTEXPR20 static void moveConstruct(_Alty& al,
		string_static_data&& _Src, string_static_data& _Dest) {
		return _Mybase::moveConstruct(al, std::move(_Src), _Dest);
	}

	WJR_CONSTEXPR20 static void Deallocate(_Alty& al, string_static_data& _Data) {
		return _Mybase::Deallocate(al, _Data);
	}

	WJR_CONSTEXPR20 static void Tidy(_Alty& al, string_static_data& _Data) {
		return _Mybase::Tidy(al, _Data);
	}

	WJR_INLINE_CONSTEXPR20 static void shrinkToFit(_Alty& al, string_static_data& _Data) {
		return _Mybase::shrinkToFit(al, _Data);
	}

	WJR_INLINE_CONSTEXPR20 size_type capacity() const noexcept {
		return _Mybase::capacity() - 1;
	}

	WJR_INLINE_CONSTEXPR20 pointer endPtr() noexcept {
		return _Mybase::data() + capacity();
	}

	WJR_INLINE_CONSTEXPR20 const_pointer endPtr() const noexcept {
		return _Mybase::data() + capacity();
	}
};

template<typename Char, size_t N, typename Alloc>
struct string_sso_data : public vector_sso_data<Char, N, Alloc> {
	using _Mybase = vector_sso_data<Char, N, Alloc>;

	using _Alty = typename _Mybase::_Alty;
	using _Alty_traits = typename _Mybase::_Alty_traits;
	using value_type = typename _Mybase::value_type;
	using pointer = typename _Mybase::pointer;
	using const_pointer = typename _Mybase::const_pointer;
	using size_type = typename _Mybase::size_type;
	using difference_type = typename _Mybase::difference_type;

	// virtual max capacity = max capacity - 1
	constexpr static size_t max_capacity = _Mybase::max_capacity - 1;

	WJR_INLINE_CONSTEXPR20 string_sso_data() : _Mybase() {}
	string_sso_data(const string_sso_data&) = delete;
	string_sso_data& operator=(const string_sso_data&) = delete;
	WJR_CONSTEXPR20 ~string_sso_data() = default;

	WJR_CONSTEXPR20 string_sso_data(
		_Alty& al,
		const size_type _Newsize,
		const size_type _Newcapacity,
		extend_tag)
		: _Mybase(al, _Newsize, _Newcapacity + 1, extend_tag{}) {
	}

	WJR_CONSTEXPR20 string_sso_data(
		_Alty& al,
		const size_type _Newsize,
		const size_type _Newcapacity)
		: _Mybase(al, _Newsize, _Newcapacity + 1) {
	}

	WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const string_sso_data& _Src, string_sso_data& _Dest) {
		const auto _Size = _Src._M_size;
		if (!_Src.is_small()) {
			string_sso_data data(al, _Size, _Size, extend_tag{});
			_Dest._M_ptr = data._M_ptr;
			_Dest._M_capacity = data._M_capacity;
		}
		_Dest._M_size = _Size;
		wjr::uninitialized_copy_n(al, _Src.data(), _Src.size(), _Dest.data());
	}

	WJR_INLINE_CONSTEXPR20 static void moveConstruct(_Alty& al, string_sso_data&& _Src, string_sso_data& _Dest) {
		return _Mybase::moveConstruct(al, std::move(_Src), _Dest);
	}

	WJR_CONSTEXPR20 static void Deallocate(_Alty& al, string_sso_data& _Data) {
		return _Mybase::Deallocate(al, _Data);
	}

	WJR_CONSTEXPR20 static void Tidy(_Alty& al, string_sso_data& _Data) {
		return _Mybase::Tidy(al, _Data);
	}

	WJR_INLINE_CONSTEXPR20 static void shrinkToFit(_Alty& al, string_sso_data& _Data) {
		if (!_Data.is_small()) {
			const auto _Oldsize = _Data.size();
			const auto _Oldcapacity = _Data.capacity();
			if (_Oldsize != _Oldcapacity) {
				if (_Oldsize <= max_capacity) {
					wjr::uninitialized_move_n(al, _Data.data(), _Oldsize, reinterpret_cast<pointer>(_Data._M_storage));
					wjr::destroy_n(al, _Data.data(), _Oldsize);
					_Alty_traits::deallocate(al, _Data.data(), _Oldcapacity + 1);
					_Data.reset_ptr();
				}
				else {
					auto _Newdata = _Alty_traits::allocate(al, _Oldsize + 1);
					wjr::uninitialized_move_n(al, _Data.data(), _Oldsize, _Newdata);
					wjr::destroy_n(al, _Data.data(), _Oldsize);
					_Alty_traits::deallocate(al, _Data.data(), _Oldcapacity + 1);
					_Data._M_ptr = _Newdata;
					_Data._M_capacity = _Oldsize + 1;
				}
				_Data._M_size = _Oldsize;
			}
		}
	}

	WJR_INLINE_CONSTEXPR20 size_type capacity() const noexcept {
		return _Mybase::capacity() - 1;
	}

	WJR_INLINE_CONSTEXPR20 pointer endPtr() noexcept {
		return _Mybase::data() + capacity();
	}

	WJR_INLINE_CONSTEXPR20 const_pointer endPtr() const noexcept {
		return _Mybase::data() + capacity();
	}
};

template<typename Traits>
struct __traits_helper {
	using is_default_equal = std::false_type;
	using is_default_less = std::false_type;
};

template<typename Char>
struct __traits_helper<std::char_traits<Char>> {
	using is_default_equal = std::true_type;
	using is_default_less = std::true_type;
};

template<typename Derived>
struct __base_string_view_traits;

template<typename Char, typename Traits = std::char_traits<Char>>
class basic_string_view;

template<typename Char, typename Traits>
struct __base_string_view_traits<basic_string_view<Char, Traits>> {
	using _Derived = basic_string_view<Char, Traits>;

	using _Traits_helper = __traits_helper<Traits>;

	using _Std_view_type = std::basic_string_view<Char, Traits>;

	template<typename T>
	struct _Is_string_view_like : std::conjunction<
		std::is_convertible<const T&, _Derived>
	> {};

	template<typename T>
	constexpr static bool _Is_string_view_like_v = _Is_string_view_like<T>::value;

	template<typename T>
	struct _Is_noptr_std_string_view_like : std::conjunction<
		std::is_convertible<const T&, _Std_view_type>,
		std::negation<std::is_convertible<const T&, const Char*>>
	> {};

	template<typename T>
	constexpr static bool _Is_noptr_std_string_view_like_v = _Is_noptr_std_string_view_like<T>::value;

	template<typename T>
	struct _Is_noptr_string_view_like : std::conjunction<
		_Is_string_view_like<T>,
		std::negation<std::is_convertible<const T&, const Char*>>
	> {};

	template<typename T>
	constexpr static bool _Is_noptr_string_view_like_v = _Is_noptr_string_view_like<T>::value;

	using traits_type = Traits;
	using value_type = Char;
	using pointer = Char*;
	using const_pointer = const Char*;
	using reference = Char&;
	using const_reference = const Char&;
	using const_iterator = const Char*;
	using iterator = const_iterator;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = const_reverse_iterator;
	using size_type = size_t;
	using difference_type = ptrdiff_t;

	static constexpr auto npos = static_cast<size_type>(-1);
};

template<typename Derived>
class __base_string_view : public __base_string_view_traits<Derived> {
public:
	using _Mybase = __base_string_view_traits<Derived>;
	template<typename T>
	constexpr static bool _Is_noptr_string_view_like_v = _Mybase::template _Is_noptr_string_view_like_v<T>;

	using value_type = typename _Mybase::value_type;
	using size_type = typename _Mybase::size_type;

	constexpr static size_type npos = _Mybase::npos;
	
	WJR_INLINE_CONSTEXPR void swap(Derived& other) noexcept {
		const Derived tmp{ other };
		other = this->derived();
		this->derived() = tmp;
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR Derived substr(
		const size_type off = 0, size_type n = npos) const WJR_NOEXCEPT {
#if defined(_WJR_EXCEPTION)
		if (is_likely(off <= derived().size())) {
#endif // _WJR_EXCEPTION
			return nesubstr(off, n);
#if defined(_WJR_EXCEPTION)
		}
		throw std::out_of_range("out of range at basic_string_view::substr");
#endif // _WJR_EXCEPTION
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR Derived
		nesubstr(const size_type off = 0, size_type n = npos) const noexcept {
		n = std::min(n, this->derived().size() - off);
		return Derived(this->derived().data() + off, n);
	}

	// no exception view

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static Derived view(
		const value_type* s, const size_type n) noexcept {
		return Derived(s, n);
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static Derived view(
		const value_type* s) noexcept {
		return Derived(s);
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static Derived view(
		std::initializer_list<value_type> il) noexcept {
		return view(il.begin(), il.size());
	}

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static Derived view(const StringView& t) noexcept {
		return Derived(t);
	}

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static Derived view(const StringView& t,
		const size_type off, const size_type n = npos) noexcept {
		return view(t).nesubstr(off, n);
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR Derived view() const noexcept {
		return this->derived();
	}

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static Derived eview(const StringView& t,
		const size_type off, const size_type n = npos) WJR_NOEXCEPT {
		return view(t).substr(off, n);
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR Derived prefix(size_type n) const noexcept {
		n = std::min(n, this->derived().size());
		return Derived(this->derived().begin(), n);
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR Derived suffix(size_type n) const noexcept {
		n = std::min(n, this->derived().size());
		return Derived(this->derived().end() - n, n);
	}
protected:
	Derived& derived() { return static_cast<Derived&>(*this); }
	const Derived& derived() const { return static_cast<const Derived&>(*this); }
};

template<typename Char, typename Traits>
class basic_string_view : public __base_string_view<basic_string_view<Char, Traits>> {
public:
	using _Mybase = __base_string_view<basic_string_view<Char, Traits>>;
private:

	using _Traits_helper = typename _Mybase::_Traits_helper;

	using _Std_view_type = typename _Mybase::_Std_view_type;

	template<typename T>
	constexpr static bool _Is_string_view_like_v = _Mybase::template _Is_string_view_like_v<T>;

	template<typename T>
	constexpr static bool _Is_noptr_std_string_view_like_v = _Mybase::template _Is_noptr_std_string_view_like_v<T>;

	template<typename T>
	constexpr static bool _Is_noptr_string_view_like_v = _Mybase::template _Is_noptr_string_view_like_v<T>;

public:
	static_assert(!std::is_array_v<Char>&& std::is_trivial_v<Char>&& std::is_standard_layout_v<Char>,
		"The character type of basic_string_view must be a non-array trivial standard-layout type. See N4910 "
		"23.1 [strings.general]/1.");

	static_assert(std::is_same_v<Char, typename Traits::char_type>, "");

	using traits_type = typename _Mybase::traits_type;
	using value_type = typename _Mybase::value_type;
	using pointer = typename _Mybase::pointer;
	using const_pointer = typename _Mybase::const_pointer;
	using reference = typename _Mybase::reference;
	using const_reference = typename _Mybase::const_reference;
	using const_iterator = typename _Mybase::const_iterator;
	using iterator = typename _Mybase::iterator;
	using const_reverse_iterator = typename _Mybase::const_reverse_iterator;
	using reverse_iterator = typename _Mybase::reverse_iterator;
	using size_type = typename _Mybase::size_type;
	using difference_type = typename _Mybase::difference_type;

	constexpr static size_type npos = _Mybase::npos;

	using _Mybase::swap;
	using _Mybase::substr;
	using _Mybase::view;
	using _Mybase::eview;
	using _Mybase::prefix;
	using _Mybase::suffix;

private:

	constexpr static size_type traits_length(const value_type* s) {
		return traits_type::length(s);
	}
public:

	WJR_INLINE_CONSTEXPR basic_string_view() noexcept : _Mydata(), _Mysize(0) {}

	WJR_INLINE_CONSTEXPR basic_string_view(const basic_string_view&) noexcept = default;
	WJR_INLINE_CONSTEXPR basic_string_view& operator=(const basic_string_view&) noexcept = default;

	WJR_INLINE_CONSTEXPR basic_string_view(const const_pointer ptr)
		: _Mydata(ptr), _Mysize(traits_length(ptr)) {}

	WJR_INLINE_CONSTEXPR basic_string_view(
		const const_pointer ptr, const size_type n)
		: _Mydata(ptr), _Mysize(n) {}

private:
	WJR_INLINE_CONSTEXPR basic_string_view(_Std_view_type s, disable_tag) noexcept
		: _Mydata(s.data()), _Mysize(s.size()) {}
public:

	template<typename StringView, std::enable_if_t<_Is_noptr_std_string_view_like_v<StringView>, int> = 0>
	WJR_INLINE_CONSTEXPR basic_string_view(const StringView& t) noexcept
		: basic_string_view(_Std_view_type(t), disable_tag{}) {}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR operator _Std_view_type() const noexcept {
		return { data(), size() };
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR const_iterator begin() const noexcept {
		return const_iterator(_Mydata);
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR const_iterator end() const noexcept {
		return const_iterator(_Mydata + _Mysize);
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR const_iterator cbegin() const noexcept {
		return begin();
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR const_iterator cend() const noexcept {
		return end();
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator{ end() };
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator{ begin() };
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR const_reverse_iterator crbegin() const noexcept {
		return rbegin();
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR const_reverse_iterator crend() const noexcept {
		return rend();
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR size_type size() const noexcept {
		return _Mysize;
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR size_type length() const noexcept {
		return _Mysize;
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR bool empty() const noexcept {
		return _Mysize == 0;
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR const_pointer data() const noexcept {
		return _Mydata;
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR size_type max_size() const noexcept {
		return std::min(static_cast<size_t>(PTRDIFF_MAX), static_cast<size_t>(-1) / sizeof(Char));
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR const_reference operator[](const size_type off) const noexcept {
		return _Mydata[off];
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR const_reference at(const size_type off) const {
		if (off >= size()) {
			throw std::out_of_range("error at basic_string_view::at");
		}
		return _Mydata[off];
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR const_reference front() const noexcept {
		return _Mydata[0];
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR const_reference back() const noexcept {
		return _Mydata[_Mysize - 1];
	}

	WJR_INLINE_CONSTEXPR void remove_prefix(const size_type n) noexcept {
		_Mydata += n;
		_Mysize -= n;
	}

	WJR_INLINE_CONSTEXPR void remove_suffix(const size_type n) noexcept {
		_Mysize -= n;
	}

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_CONSTEXPR20 int compare(const StringView& t) const noexcept {
		const auto sv = view(t);
		if constexpr (_Traits_helper::is_default_equal::value) {
			if constexpr (std::is_same_v<Char, char>) {
				if (!wjr::is_constant_evaluated()) {
					return wjr::compare(
						reinterpret_cast<const uint8_t*>(begin()),
						reinterpret_cast<const uint8_t*>(end()),
						reinterpret_cast<const uint8_t*>(sv.begin()),
						reinterpret_cast<const uint8_t*>(sv.end())
					);
				}
				auto _First1 = begin();
				auto _Last1 = end();
				auto _First2 = sv.begin();
				auto _Last2 = sv.end();
				for (; _First1 != _Last1 && _First2 != _Last2; ++_First1, ++_First2) {
					if (*_First1 < *_First2) {
						return -1;
					}
					if (*_First2 < *_First1) {
						return 1;
					}
				}
				return _First1 == _Last1 ? (_First2 == _Last2 ? 0 : -1) : 1;
			}
			else {
				return wjr::compare(begin(), end(), sv.begin(), sv.end());
			}
		}
		else {
			const int ret = traits_type::compare(begin(), sv.begin(), std::min(size(), sv.size()));
			return ret != 0 ? ret : (size() < sv.size() ? -1 : 1);
		}
	}

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off, const size_type n, const StringView& t)
		const WJR_NOEXCEPT {
		return eview(*this, off, n).compare(t);
	}

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off1, const size_type n1, const StringView& t,
		const size_type off2, const size_type n2)
		const WJR_NOEXCEPT {
		return eview(*this, off1, n1).compare(eview(t, off2, n2));
	}

	WJR_NODISCARD WJR_CONSTEXPR20 int compare(const Char* const ptr) const {
		return compare(view(ptr));
	}

	WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off, const size_type n, const Char* const ptr) const {
		return eview(*this, off, n).compare(view(ptr));
	}

	WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off1, const size_type n1,
		const Char* const ptr, const size_type n2) const {
		return eview(*this, off1, n1).compare(eview(ptr, n2));
	}       

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_CONSTEXPR20 bool starts_with(const StringView& t) const noexcept {
		const auto sv = view(t);
		return prefix(sv.size()).equal(sv);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 bool starts_with(const Char ch) const noexcept {
		return !empty() && traits_type::eq(front(), ch);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 bool starts_with(const Char* const other) const {
		return starts_with(view(other));
	}

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_CONSTEXPR20 bool ends_with(const StringView& t) const noexcept {
		const auto sv = view(t);
		return suffix(sv.size()).equal(sv);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 bool ends_with(const Char ch) const noexcept {
		return !empty() && traits_type::eq(back(), ch);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 bool ends_with(const Char* const other) const {
		return ends_with(view(other));
	}

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_CONSTEXPR20 bool contains(const StringView& t) const noexcept {
		return find(t) != npos;
	}

	WJR_NODISCARD WJR_CONSTEXPR20 bool contains(const Char other) const noexcept {
		return find(other) != npos;
	}

	WJR_NODISCARD WJR_CONSTEXPR20 bool contains(const Char* const other) const {
		return find(other) != npos;
	}

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_CONSTEXPR20 size_type find(const StringView& t, const size_type off = 0) const noexcept {
		if (is_likely(off <= size())) {
			const auto sv1 = view(*this, off);
			const auto sv2 = view(t);
			if constexpr (_Traits_helper::is_default_equal::value) {
				auto iter = wjr::search(sv1.begin(), sv1.end(), sv2.begin(), sv2.end());
				return iter == end() ? npos : static_cast<size_type>(iter - begin());
			}
			else {
				auto iter = wjr::search(sv1.begin(), sv1.end(), sv2.begin(), sv2.end(),
					[](auto x, auto y) {
						return traits_type::eq(x, y);
					});
				return iter == end() ? npos : static_cast<size_type>(iter - begin());
			}
		}
		return npos;
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type find(const Char ch, const size_type off = 0) const noexcept {
		if (is_likely(off <= size())) {
			const auto sv = view(*this, off);
			if constexpr (_Traits_helper::is_default_equal::value) {
				auto iter = wjr::find(sv.begin(), sv.end(), ch);
				return iter == end() ? npos : static_cast<size_type>(iter - begin());
			}
			else {
				auto iter = traits_type::find(sv.begin(), sv.end(), ch);
				return iter == end() ? npos : static_cast<size_type>(iter - begin());
			}
		}
		return npos;
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type find(const Char* const ptr, const size_type off,
		const size_type n) const {
		return find(view(ptr, n), off);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type find(const Char* const ptr, const size_type off = 0)
		const {
		return find(view(ptr), off);
	}

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_CONSTEXPR20 size_type rfind(const StringView& t, const size_type off = npos) const noexcept {
		if (is_likely(size() != 0)) {
			const auto sv1 = view(*this, 0, std::min(off, size() - 1) + 1);
			const auto sv2 = view(t);
			if constexpr (_Traits_helper::is_default_equal::value) {
				auto iter = wjr::search(sv1.rbegin(), sv1.rend(),
					sv2.rbegin(), sv2.rend());
				return iter == rend() ? npos : static_cast<size_type>(rend() - iter) - sv2.size();
			}
			else {
				auto iter = wjr::search(sv1.rbegin(), sv1.rend(),
					sv2.rbegin(), sv2.rend(),
					[](auto x, auto y) {
						return traits_type::eq(x, y);
					});
				return iter == rend() ? npos : static_cast<size_type>(rend() - iter) - sv2.size();
			}
		}
		return npos;
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type rfind(const Char ch, const size_type off = npos) const noexcept {
		if (is_likely(size() != 0)) {
			const auto sv = view(*this, 0, std::min(off, size() - 1) + 1);
			if constexpr (_Traits_helper::is_default_equal::value) {
				auto iter = wjr::find(sv.rbegin(), sv.rend(), ch);
				return iter == rend() ? npos : static_cast<size_type>(rend() - iter) - 1;
			}
			else {
				auto iter = wjr::find(sv.rbegin(), sv.rend(), ch,
					[](auto x, auto y) {
						return traits_type::eq(x, y);
					});
				return iter == rend() ? npos : static_cast<size_type>(rend() - iter) - 1;
			}
		}
		return npos;
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type rfind(const Char* const ptr, const size_type off,
		const size_type n) const {
		return rfind(view(ptr, n), off);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type rfind(const Char* const ptr, const size_type off = npos)
		const {
		return rfind(view(ptr), off);
	}

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_of(const StringView& t,
		const size_type off = 0) const noexcept {
		auto sv = _Std_view_type(view(t));
		return _Std_view_type(*this).find_first_of(sv, off);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_of(const Char ch, const size_type off = 0) const noexcept {
		return find(ch, off);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_of(const Char* const ptr, const size_type off,
		const size_type n) const {
		return find_first_of(view(ptr, n), off);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_of(
		const Char* const ptr, const size_type off = 0) const {
		return find_first_of(view(ptr), off);
	}

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_of(const StringView& t,
		const size_type off = npos) const noexcept {
		const auto sv = _Std_view_type(view(t));
		return _Std_view_type(*this).find_last_of(sv, off);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_of(const Char ch, const size_type off = npos) const noexcept {
		return rfind(ch, off);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_of(const Char* const ptr, const size_type off,
		const size_type n) const {
		return find_last_of(view(ptr, n), off);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_of(
		const Char* const ptr, const size_type off = npos) const {
		return find_last_of(view(ptr), off);
	}

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_not_of(const StringView& t,
		const size_type off = 0) const noexcept {
		const auto sv = _Std_view_type(view(t));
		return _Std_view_type(*this).find_first_not_of(sv, off);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_not_of(const Char ch, const size_type off = 0) const noexcept {
		if (is_likely(off <= size())) {
			const auto sv = view(*this, off);
			if constexpr (_Traits_helper::is_default_equal::value) {
				auto iter = wjr::find(sv.begin(), sv.end(), ch, std::not_equal_to<>{});
				return iter == end() ? npos : static_cast<size_type>(iter - begin());
			}
			else {
				auto iter = wjr::find(sv.begin(), sv.end(), ch,
					[](auto x, auto y) {
						return !traits_type::eq(x, y);
					});
				return iter == end() ? npos : static_cast<size_type>(iter - begin());
			}
		}
		return npos;
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_not_of(const Char* const ptr, const size_type off,
		const size_type n) const {
		return find_first_not_of(view(ptr, n), off);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_not_of(
		const Char* const ptr, const size_type off = 0) const {
		return find_first_not_of(view(ptr), off);
	}

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_not_of(const StringView& t,
		const size_type off = npos) const noexcept {
		const auto sv = _Std_view_type(view(t));
		return _Std_view_type(*this).find_last_not_of(sv, off);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_not_of(const Char ch, const size_type off = npos) const noexcept {
		if (is_likely(size() != 0)) {
			const auto sv = view(*this, 0, std::min(off, size() - 1) + 1);
			if constexpr (_Traits_helper::is_default_equal::value) {
				auto iter = wjr::find(sv.rbegin(), sv.rend(), ch, std::not_equal_to<>{});
				return iter == rend() ? npos : static_cast<size_type>(rend() - iter) - 1;
			}
			else {
				auto iter = wjr::find(sv.rbegin(), sv.rend(), ch,
					[](auto x, auto y) {
						return !traits_type::eq(x, y);
					});
				return iter == rend() ? npos : static_cast<size_type>(rend() - iter) - 1;
			}
		}
		return npos;
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_not_of(const Char* const ptr, const size_type off,
		const size_type count) const {
		return find_last_not_of(view(ptr, count), off);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_not_of(
		const Char* const ptr, const size_type off = npos) const {
		return find_last_not_of(view(ptr), off);
	}

	// non-standard extension functions

	/*------External extension function------*/

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const StringView& t) const noexcept {
		const auto sv = view(t);
		if constexpr (_Traits_helper::is_default_equal::value) {
			return wjr::equal(begin(), end(), sv.begin(), sv.end());
		}
		else {
			return compare(sv) == 0;
		}
	}

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const size_type off, const size_type n, const StringView& t)
		const WJR_NOEXCEPT {
		return eview(*this, off, n).equal(view(t));
	}

	template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
	WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const size_type off1, const size_type n1, const StringView& t,
		const size_type off2, const size_type n2) const WJR_NOEXCEPT {
		return eview(*this, off1, n1).equal(eview(t, off2, n2));
	}

	WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const Char* const ptr) const {
		return equal(view(ptr));
	}

	WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const size_type off, const size_type n, const Char* const ptr) const {
		return equal(off, n, view(ptr));
	}

	WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const size_type off1, const size_type n1,
		const Char* const ptr, const size_type n2) const {
		return equal(off1, n1, view(ptr, n2));
	}

private:

	const_pointer _Mydata;
	size_type _Mysize;
};

template<typename Char, typename Traits>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator==(
	const basic_string_view<Char, Traits> lhs, 
	const basic_string_view<Char, Traits> rhs) noexcept {
	return lhs.equal(rhs);
}

template<typename Char, typename Traits, int = 1>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator==(
	const type_identity_t<basic_string_view<Char, Traits>> lhs,
	const basic_string_view<Char, Traits> rhs) noexcept {
	return lhs.equal(rhs);
}

template<typename Char, typename Traits, int = 2>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator==(
	const basic_string_view<Char, Traits> lhs,
	const type_identity_t<basic_string_view<Char, Traits>> rhs) noexcept {
	return lhs.equal(rhs);
}

template<typename Char, typename Traits>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator!=(
	const basic_string_view<Char, Traits> lhs, 
	const basic_string_view<Char, Traits> rhs) noexcept {
	return !(lhs == rhs);
}

template<typename Char, typename Traits, int = 1>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator!=(
	const type_identity_t<basic_string_view<Char, Traits>> lhs,
	const basic_string_view<Char, Traits> rhs) noexcept {
	return !(lhs == rhs);
}

template<typename Char, typename Traits, int = 2>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator!=(
	const basic_string_view<Char, Traits> lhs,
	const type_identity_t<basic_string_view<Char, Traits>> rhs) noexcept {
	return !(lhs == rhs);
}

template<typename Char, typename Traits>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<(
	const basic_string_view<Char, Traits> lhs, 
	const basic_string_view<Char, Traits> rhs) noexcept {
	return lhs.compare(rhs) < 0;
}

template<typename Char, typename Traits, int = 1>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<(
	const type_identity_t<basic_string_view<Char, Traits>> lhs,
	const basic_string_view<Char, Traits> rhs) noexcept {
	return lhs.compare(rhs) < 0;
}

template<typename Char, typename Traits, int = 2>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<(
	const basic_string_view<Char, Traits> lhs,
	const type_identity_t<basic_string_view<Char, Traits>> rhs) noexcept {
	return lhs.compare(rhs) < 0;
}

template<typename Char, typename Traits>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>(
	const basic_string_view<Char, Traits> lhs, 
	const basic_string_view<Char, Traits> rhs) noexcept {
	return rhs < lhs;
}

template<typename Char, typename Traits, int = 1>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>(
	const type_identity_t<basic_string_view<Char, Traits>> lhs,
	const basic_string_view<Char, Traits> rhs) noexcept {
	return rhs < lhs;
}

template<typename Char, typename Traits, int = 2>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>(
	const basic_string_view<Char, Traits> lhs,
	const type_identity_t<basic_string_view<Char, Traits>> rhs) noexcept {
	return rhs < lhs;
}

template<typename Char, typename Traits>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<=(
	const basic_string_view<Char, Traits> lhs, 
	const basic_string_view<Char, Traits> rhs) noexcept {
	return !(rhs < lhs);
}

template<typename Char, typename Traits, int = 1>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<=(
	const type_identity_t<basic_string_view<Char, Traits>> lhs,
	const basic_string_view<Char, Traits> rhs) noexcept {
	return !(rhs < lhs);
}

template<typename Char, typename Traits, int = 2>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<=(
	const basic_string_view<Char, Traits> lhs,
	const type_identity_t<basic_string_view<Char, Traits>> rhs) noexcept {
	return !(rhs < lhs);
}

template<typename Char, typename Traits>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>=(
	const basic_string_view<Char, Traits> lhs, 
	const basic_string_view<Char, Traits> rhs) noexcept {
	return !(lhs < rhs);
}

template<typename Char, typename Traits, int = 1>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>=(
	const type_identity_t<basic_string_view<Char, Traits>> lhs,
	const basic_string_view<Char> rhs) noexcept {
	return !(lhs < rhs);
}

template<typename Char, typename Traits, int = 2>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>=(
	const basic_string_view<Char, Traits> lhs,
	const type_identity_t<basic_string_view<Char, Traits>> rhs) noexcept {
	return !(lhs < rhs);
}

template<typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(
	std::basic_ostream<Char, Traits>& os, wjr::basic_string_view<Char, Traits> sv) {
	return (os << (std::basic_string_view<Char, Traits>)(sv));
}

using string_view = basic_string_view<char>;
#if defined(WJR_CHAR8_T)
using u8string_view = basic_string_view<char8_t>;
#endif // WJR_CHAR8_T
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;
using wstring_view = basic_string_view<wchar_t>;

template<
	typename Char,
	typename Traits = std::char_traits<Char>,
	typename Alloc = std::allocator<Char>,
	typename Data = string_sso_data<Char, std::max(size_t(1), size_t(16 / sizeof(Char))), Alloc>>
	class basic_string {
	public:
		using traits_type = Traits;
		using allocator_type = Alloc;

		using value_type = Char;
		using reference = value_type&;
		using const_reference = const value_type&;

		using size_type = size_t;
		using difference_type = size_t;

		using pointer = value_type*;
		using const_pointer = const value_type*;

		using iterator = pointer;
		using const_iterator = const_pointer;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		constexpr static size_type npos = static_cast<size_type>(-1);

		using vector_type = wjr::vector<Char, Alloc, Data>;
		using data_type = typename vector_type::data_type;
		using view_type = wjr::basic_string_view<Char, Traits>;

		using _Alty = typename vector_type::_Alty;
		using _Alty_traits = typename vector_type::_Alty_traits;

		static_assert(std::is_same_v<iterator, typename vector_type::iterator>, "");
		static_assert(std::is_same_v<const_iterator, typename vector_type::const_iterator>, "");
		static_assert(std::is_same_v<const_iterator, typename view_type::const_iterator>, "");
		static_assert(std::is_trivially_destructible_v<Char>, "");

	private:

		template<typename T>
		struct _Is_string_view_like : std::conjunction<
			std::is_convertible<const T&, view_type>
		> {};

		template<typename T>
		constexpr static bool _Is_string_view_like_v = _Is_string_view_like<T>::value;

		template<typename T>
		struct _Is_noptr_string_view_like : std::conjunction<
			_Is_string_view_like<T>,
			std::negation<std::is_convertible<const T&, const Char*>>
		> {};

		template<typename T>
		constexpr static bool _Is_noptr_string_view_like_v = _Is_noptr_string_view_like<T>::value;

		constexpr static size_type traits_length(const value_type* s) {
			return traits_type::length(s);
		}

	public:

		WJR_CONSTEXPR20 basic_string()
			noexcept(std::is_nothrow_default_constructible_v<vector_type>)
			: m_core() {
			set_end();
		}

		WJR_CONSTEXPR20 explicit basic_string(const allocator_type& al) noexcept
			: m_core(al) {
			set_end();
		}

		WJR_CONSTEXPR20 basic_string(const basic_string& other)
			: m_core(other.m_core) {
			set_end();
		}

		WJR_CONSTEXPR20 basic_string(const basic_string& other, const allocator_type& al)
			: m_core(other.m_core, al) {
			set_end();
		}

		WJR_CONSTEXPR20 basic_string(basic_string&& other)
			: m_core(std::move(other.m_core)) {
			set_end();
			other.set_end();
		}

		WJR_CONSTEXPR20 basic_string(basic_string&& other, const allocator_type& al)
			: m_core(std::move(other.m_core), al) {
			set_end();
			other.set_end();
		}

		template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
		WJR_CONSTEXPR20 basic_string(iter first, iter last, const allocator_type& al = allocator_type())
			: m_core(first, last, al) {
			set_end();
		}

		WJR_CONSTEXPR20 basic_string(const basic_string& other,
			const size_type off, const allocator_type& al = allocator_type())
			: basic_string(eview(other, off), al) {}

		WJR_CONSTEXPR20 basic_string(const basic_string& other,
			const size_type off, const size_type n, const allocator_type& al = allocator_type())
			: basic_string(eview(other, off, n), al) {}

		WJR_CONSTEXPR20 basic_string(const value_type* s, const size_type n, const allocator_type& al = allocator_type())
			: basic_string(view(s, n), al) {}

		WJR_CONSTEXPR20 basic_string(const value_type* s, const allocator_type& al = allocator_type())
			: basic_string(view(s), al) {}

		WJR_CONSTEXPR20 basic_string(std::initializer_list<value_type> il, const allocator_type& al = allocator_type())
			: basic_string(view(il), al) {}

	private:
		WJR_CONSTEXPR20 basic_string(view_type sv, const allocator_type& al, disable_tag)
			: basic_string(sv.data(), sv.data() + sv.size(), al) {}

		WJR_CONSTEXPR20 basic_string(
			view_type sv,
			const size_type off, const size_type n,
			const allocator_type& al, disable_tag)
			: basic_string(eview(sv, off, n), al) {}
	public:

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_CONSTEXPR20 explicit basic_string(const StringView& t, const allocator_type& al = allocator_type())
			:basic_string(view_type(t), al, disable_tag{}) {}

		template<typename StringView, std::enable_if_t<_Is_string_view_like_v<StringView>, int> = 0>
		explicit basic_string(const StringView& t,
			const size_type off, const size_type n,
			const allocator_type& al = allocator_type())
			: basic_string(view_type(t), off, n, al, disable_tag{}) {}

		WJR_CONSTEXPR20 explicit basic_string(const size_type n,
			const value_type c = value_type(), const allocator_type& al = allocator_type())
			: m_core(n, c, al) {
			set_end();
		}

		WJR_CONSTEXPR20 operator std::basic_string_view<Char, traits_type>() const noexcept {
			return { data(), size() };
		}

		WJR_CONSTEXPR20 ~basic_string() noexcept = default;

		WJR_CONSTEXPR20 basic_string& operator=(const basic_string& other) {

			if (is_likely(this != std::addressof(other))) {
				m_core = other.m_core;
				set_end();
			}

			return *this;
		}

		WJR_CONSTEXPR20 basic_string& operator=(basic_string&& other) noexcept {

			if (is_likely(this != std::addressof(other))) {
				m_core = std::move(other.m_core);
				set_end();
				other.set_end();
			}

			return *this;
		}

		WJR_CONSTEXPR20 basic_string& operator=(const value_type* str) {
			return assign(str);
		}

		WJR_CONSTEXPR20 basic_string& operator=(std::initializer_list<value_type> il) {
			return assign(il);
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_CONSTEXPR20 basic_string& operator=(const StringView& t) {
			return assign(t);
		}

		WJR_CONSTEXPR20 basic_string& operator=(const value_type c) {
			resize(1);
			*begin() = c;
			return *this;
		}

		WJR_INLINE_CONSTEXPR20 iterator begin() { return m_core.begin(); }
		WJR_INLINE_CONSTEXPR20 const_iterator begin()const { return m_core.begin(); }
		WJR_INLINE_CONSTEXPR20 const_iterator cbegin()const { return m_core.cbegin(); }

		WJR_INLINE_CONSTEXPR20 iterator end() { return m_core.end(); }
		WJR_INLINE_CONSTEXPR20 const_iterator end()const { return m_core.end(); }
		WJR_INLINE_CONSTEXPR20 const_iterator cend()const { return m_core.end(); }

		WJR_INLINE_CONSTEXPR20 reverse_iterator rbegin() { return reverse_iterator(end()); }
		WJR_INLINE_CONSTEXPR20 const_reverse_iterator rbegin()const { return const_reverse_iterator(end()); }
		WJR_INLINE_CONSTEXPR20 const_reverse_iterator crbegin()const { return const_reverse_iterator(cend()); }

		WJR_INLINE_CONSTEXPR20 reverse_iterator rend() { return reverse_iterator(begin()); }
		WJR_INLINE_CONSTEXPR20 const_reverse_iterator rend()const { return const_reverse_iterator(begin()); }
		WJR_INLINE_CONSTEXPR20 const_reverse_iterator crend()const { return const_reverse_iterator(begin()); }

		WJR_INLINE_CONSTEXPR20 reference front() { return m_core.front(); }
		WJR_INLINE_CONSTEXPR20 const_reference front()const { return m_core.front(); }

		WJR_INLINE_CONSTEXPR20 reference back() {
			return m_core.back();
		}

		WJR_INLINE_CONSTEXPR20 const_reference back()const {
			return m_core.back();
		}

		WJR_CONSTEXPR20 void resize(const size_type n) {
			m_core.resize(n);
			set_end();
		}

		WJR_CONSTEXPR20 void resize(const size_type n, const value_type c) {
			m_core.resize(n, c);
			set_end();
		}

		WJR_CONSTEXPR20 void reserve(const size_type c) {
			m_core.reserve(c);
			set_end();
		}

		WJR_CONSTEXPR20 void shrink_to_fit() {
			m_core.shrink_to_fit();
			set_end();
		}

		WJR_CONSTEXPR20 void clear() {
			resize(0);
		}

		WJR_INLINE_CONSTEXPR20 size_type size() const {
			return m_core.size();
		}

		WJR_INLINE_CONSTEXPR20 size_type length() const {
			return size();
		}

		WJR_INLINE_CONSTEXPR20 size_type max_size() const { return std::numeric_limits<size_type>::max(); }

		WJR_INLINE_CONSTEXPR20 size_type capacity() const {
			return m_core.capacity();
		}

		WJR_INLINE_CONSTEXPR20 reference operator[](const size_type index) {
			return m_core[index];
		}

		WJR_INLINE_CONSTEXPR20 const_reference operator[](const size_type index)const {
			return m_core[index];
		}

		WJR_INLINE_CONSTEXPR20 reference at(const size_type index) {
			return m_core.at(index);
		}

		WJR_INLINE_CONSTEXPR20 const_reference at(const size_type index)const {
			return m_core.at(index);
		}

		WJR_CONSTEXPR20 basic_string& operator+=(const basic_string& other) { return append(other); }
		WJR_CONSTEXPR20 basic_string& operator+=(const value_type* s) { return append(s); }
		WJR_CONSTEXPR20 basic_string& operator+=(std::initializer_list<value_type> il) {
			append(il);
			return *this;
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_CONSTEXPR20 basic_string& operator+=(const StringView& t) {
			return append(t);
		}

		WJR_CONSTEXPR20 basic_string& operator+=(const value_type c) { return append(c); }

		WJR_CONSTEXPR20 basic_string& append(const basic_string& other) {
			return append(view(other));
		}

		WJR_CONSTEXPR20 basic_string& append(
			const basic_string& other, const size_type off, const size_type n = npos) {
			return append(eview(other, off, n));
		}

		template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
		WJR_CONSTEXPR20 basic_string& append(iter first, iter last) {
			m_core.append(first, last);
			set_end();
			return *this;
		}

		WJR_CONSTEXPR20 basic_string& append(const value_type* s, size_type n) {
			return append(view(s, n));
		}

		WJR_CONSTEXPR20 basic_string& append(const value_type* s) {
			return append(view(s));
		}

		WJR_CONSTEXPR20 basic_string& append(std::initializer_list<value_type> il) {
			return append(view(il));
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_CONSTEXPR20 basic_string& append(const StringView& t) {
			const auto sv = view(t);
			return append(sv.data(), sv.data() + sv.size());
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_CONSTEXPR20 basic_string& append(const StringView& t,
			const size_type off, const size_type n = npos) {
			return apend(eview(t, off, n));
		}

		WJR_CONSTEXPR20 basic_string& append(const size_type n, const value_type c) {
			m_core.append(n, c);
			set_end();
			return *this;
		}

		WJR_CONSTEXPR20 basic_string& append(const value_type c) {
			push_back(c);
			return *this;
		}

		WJR_CONSTEXPR20 void push_back(const value_type c) {
			m_core.push_back(c);
			set_end();
		}

		WJR_CONSTEXPR20 void push_front(const value_type c) {
			prepend(c);
		}

		WJR_CONSTEXPR20 void pop_back() {
			m_core.pop_back();
			set_end();
		}

		WJR_CONSTEXPR20 basic_string& assign(const basic_string& other) {
			return (*this) = other;
		}

		WJR_CONSTEXPR20 basic_string& assign(basic_string&& other) noexcept {
			return (*this) = std::move(other);
		}

		WJR_CONSTEXPR20 basic_string& assign(const basic_string& other, const size_type off, const size_type n = npos) {
			return assign(eview(other, off, n));
		}

		template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
		WJR_CONSTEXPR20 basic_string& assign(iter first, iter last) {
			m_core.assign(first, last);
			set_end();
			return *this;
		}

		WJR_CONSTEXPR20 basic_string& assign(const value_type* s, const size_type n) {
			return assign(view(s, n));
		}

		WJR_CONSTEXPR20 basic_string& assign(const value_type* s) {
			return assign(view(s));
		}

		WJR_CONSTEXPR20 basic_string& assign(std::initializer_list<value_type> il) {
			return assign(view(il));
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_CONSTEXPR20 basic_string& assign(const StringView& t) {
			const auto sv = view(t);
			return assign(sv.data(), sv.data() + sv.size());
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_CONSTEXPR20 basic_string& assign(const StringView& t, const size_type off, const size_type n = npos) {
			return assign(eview(t, off, n));
		}

		WJR_CONSTEXPR20 basic_string& assign(const size_type n, const value_type c) {
			m_core.assign(n, c);
			set_end();
			return *this;
		}

		WJR_CONSTEXPR20 basic_string& insert(const size_type off, const basic_string& str) {
			return insert(off, view(str));
		}

		WJR_CONSTEXPR20 basic_string& insert(const size_type off1,
			const basic_string& str, const size_type off2, const size_type n = npos) {
			return insert(off1, eview(str, off2, n));
		}

		template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
		WJR_CONSTEXPR20 iterator insert(const_iterator _Where, iter first, iter last) {
			auto __old_pos = static_cast<size_type>(_Where - cbegin());
			m_core.insert(_Where, first, last);
			set_end();
			return begin() + __old_pos;
		}

		WJR_CONSTEXPR20 basic_string& insert(const size_type off, const value_type* s, const size_type n) {
			return insert(off, view(s, n));
		}

		WJR_CONSTEXPR20 basic_string& insert(const size_type off, const value_type* s) {
			return insert(off, view(s));
		}

		WJR_CONSTEXPR20 iterator insert(const_iterator _Where, const size_type n, const value_type c) {
			auto __old_pos = static_cast<size_type>(_Where - cbegin());
			m_core.insert(_Where, n, c);
			set_end();
			return begin() + __old_pos;
		}

		WJR_CONSTEXPR20 basic_string& insert(const size_type off, const size_type n, const value_type c) {
			(void)(eview(*this, off));
			insert(cbegin() + off, n, c);
			return *this;
		}

		WJR_CONSTEXPR20 iterator insert(const_iterator _Where, const value_type c) {
			auto __old_pos = static_cast<size_type>(_Where - cbegin());
			m_core.insert(_Where, c);
			set_end();
			return begin() + __old_pos;
		}

		WJR_CONSTEXPR20 iterator insert(const_iterator _Where, std::initializer_list<value_type> il) {
			return insert(_Where, il.begin(), il.end());
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_CONSTEXPR20 basic_string& insert(const size_type off, const StringView& t) {
			(void)(eview(*this, off));
			const auto sv = view(t);
			insert(cbegin() + off, sv.data(), sv.data() + sv.size());
			return *this;
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_CONSTEXPR20 basic_string& insert(const size_type off1,
			const StringView& t, const size_type off2, const size_type n = npos) {
			return insert(off1, view(t, off2, n));
		}

		WJR_CONSTEXPR20 iterator erase(const_iterator first, const_iterator last) {
			auto __old_pos = first - cbegin();
			m_core.erase(first, last);
			set_end();
			return begin() + __old_pos;
		}

		WJR_CONSTEXPR20 basic_string& erase(const size_type off = 0, size_type n = npos) {
			const auto sv = eview(*this, off, n);
			erase(sv.begin(), sv.end());
			return *this;
		}

		WJR_CONSTEXPR20 iterator erase(const_iterator _Where) {
			auto __old_pos = _Where - cbegin();
			m_core.erase(_Where);
			set_end();
			return begin() + __old_pos;
		}

		WJR_CONSTEXPR20 void swap(basic_string& other)noexcept {
			m_core.swap(other.m_core);
			set_end();
			other.set_end();
		}

		WJR_INLINE_CONSTEXPR20 const value_type* c_str() const { return m_core.data(); }
		WJR_INLINE_CONSTEXPR20 value_type* data() { return m_core.data(); }
		WJR_INLINE_CONSTEXPR20 const value_type* data() const { return m_core.data(); }

		WJR_INLINE_CONSTEXPR20 bool empty()const { return m_core.empty(); }

		WJR_NODISCARD WJR_CONSTEXPR20 int compare(const basic_string& other) const noexcept {
			return compare(view(other));
		}

		WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off1, const size_type n1, const basic_string& other) const {
			return compare(off1, n1, view(other));
		}

		WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off1, const size_type n1, const basic_string& other,
			const size_type off2, const size_type n2 = npos) const {
			return compare(off1, n1, view(other, off2, n2));
		}

		WJR_NODISCARD WJR_CONSTEXPR20 int compare(const value_type* s) const {
			return compare(view(s));
		}

		WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off1, const size_type n1, const value_type* s) const {
			return compare(off1, n1, view(s));
		}

		WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off1, const size_type n1, const value_type* s,
			const size_type n2) const {
			return compare(off1, n1, view(s, n2));
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_CONSTEXPR20 int compare(const StringView& t) const noexcept {
			return view().compare(view(t));
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off1, const size_type n1, const StringView& t) const {
			return view(*this, off1, n1).compare(view(t));
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off1, const size_type n1, const StringView& t,
			const size_type off2, const size_type n2 = npos) const {
			return compare(off1, n1, view(t, off2, n2));
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_CONSTEXPR20 bool starts_with(const StringView& t) const noexcept {
			return view().starts_with(view(t));
		}

		WJR_NODISCARD WJR_CONSTEXPR20 bool starts_with(const value_type ch) const noexcept {
			return view().starts_with(ch);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 bool starts_with(const value_type* const ptr) const {
			return view().starts_with(ptr);
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_CONSTEXPR20 bool ends_with(const StringView& t) const noexcept {
			return view().ends_with(view(t));
		}

		WJR_NODISCARD WJR_CONSTEXPR20 bool ends_with(const value_type ch) const noexcept {
			return view().ends_with(ch);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 bool ends_with(const value_type* const ptr) const {
			return view().ends_with(ptr);
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_CONSTEXPR20 bool contains(const StringView& t) const noexcept {
			return view().contains(view(t));
		}

		WJR_NODISCARD WJR_CONSTEXPR20 bool contains(const value_type ch) const noexcept {
			return view().contains(ch);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 bool contains(const value_type* const ptr) const {
			return view().contains(ptr);
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_CONSTEXPR20 size_type find(const StringView& t, const size_type off = 0) const noexcept {
			return view().find(view(t), off);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type find(const value_type ch, const size_type off = 0) const noexcept {
			return view().find(ch, off);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type find(const value_type* const ptr,
			const size_type off, const size_type n) const {
			return view().find(ptr, off, n);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type find(const value_type* const ptr, const size_type off = 0) const {
			return view().find(ptr, off);
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_CONSTEXPR20 size_type rfind(const StringView& t, const size_type off = npos) const noexcept {
			return view().rfind(view(t), off);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type rfind(const value_type ch, const size_type off = npos) const noexcept {
			return view().rfind(ch, off);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type rfind(const value_type* const ptr,
			const size_type off, const size_type n) const {
			return view().rfind(ptr, off, n);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type rfind(const value_type* const ptr, const size_type off = npos) const {
			return view().rfind(ptr, off);
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_of(
			const StringView& t, const size_type off = 0) const noexcept {
			return view().find_first_of(view(t), off);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_of(
			const value_type ch, const size_type off = 0) const noexcept {
			return view().find_first_of(ch, off);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_of(const value_type* const ptr,
			const size_type off, const size_type n) const {
			return view().find_first_of(ptr, off, n);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_of(
			const value_type* const ptr, const size_type off = 0) const {
			return view().find_first_of(ptr, off);
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_of(
			const StringView& t, const size_type off = npos) const noexcept {
			return view().find_last_of(view(t), off);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_of(
			const value_type ch, const size_type off = npos) const noexcept {
			return view().find_last_of(ch, off);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_of(const value_type* const ptr,
			const size_type off, const size_type n) const {
			return view().find_last_of(ptr, off, n);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_of(
			const value_type* const ptr, const size_type off = npos) const {
			return view().find_last_of(ptr, off);
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_not_of(
			const StringView& t, const size_type off = 0) const noexcept {
			return view().find_first_not_of(view(t), off);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_not_of(
			const value_type ch, const size_type off = 0) const noexcept {
			return view().find_first_not_of(ch, off);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_not_of(const value_type* const ptr,
			const size_type off, const size_type n) const {
			return view().find_first_not_of(ptr, off, n);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_not_of(
			const value_type* const ptr, const size_type off = 0) const {
			return view().find_first_not_of(ptr, off);
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_not_of(
			const StringView& t, const size_type off = npos) const noexcept {
			return view().find_last_not_of(view(t), off);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_not_of(
			const value_type ch, const size_type off = npos) const noexcept {
			return view().find_last_not_of(ch, off);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_not_of(const value_type* const ptr,
			const size_type off, const size_type n) const {
			return view().find_last_not_of(ptr, off, n);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_not_of(
			const value_type* const ptr, const size_type off = npos) const {
			return view().find_last_not_of(ptr, off);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 basic_string substr(
			const size_type off = 0, const size_type n = npos) const {
			return basic_string(eview(*this, off, n));
		}

		WJR_CONSTEXPR20 basic_string& replace(size_type off, size_type n,
			const basic_string& str) {
			return replace(off, n, view(str));
		}

		WJR_CONSTEXPR20 basic_string& replace(const_iterator first, const_iterator last,
			const basic_string& str) {
			return replace(first, last, view(str));
		}

		WJR_CONSTEXPR20 basic_string& replace(size_type off1, size_type n1,
			const basic_string& str,
			size_type off2, size_type n2 = npos) {
			return replace(off1, n1, view(str, off2, n2));
		}

		template<typename iter>
		WJR_CONSTEXPR20 basic_string& replace(const_iterator first, const_iterator last,
			iter first2, iter last2) {
			m_core.replace(first, last, first2, last2);
			set_end();
			return *this;
		}

		WJR_CONSTEXPR20 basic_string& replace(size_type off, size_type n1,
			const value_type* ptr, size_type n2) {
			const auto sv = view(*this, off, n1);
			return replace(sv.begin(), sv.end(), view(ptr, n2));
		}

		WJR_CONSTEXPR20 basic_string& replace(const_iterator first, const_iterator last,
			const value_type* ptr, size_type n2) {
			return replace(first, last, view(ptr, n2));
		}

		WJR_CONSTEXPR20 basic_string& replace(size_type off, size_type n1,
			const value_type* ptr) {
			return replace(off, n1, view(ptr));
		}

		WJR_CONSTEXPR20 basic_string& replace(const_iterator first, const_iterator last,
			const value_type* ptr) {
			return replace(first, last, view(ptr));
		}

		WJR_CONSTEXPR20 basic_string& replace(size_type off, size_type n1,
			size_type n2, value_type ch) {
			const auto sv = view(*this, off, n1);
			return replace(sv.begin(), sv.end(), n2, ch);
		}

		WJR_CONSTEXPR20 basic_string& replace(const_iterator first, const_iterator last,
			size_type n2, value_type ch) {
			m_core.replace(first, last, n2, ch);
			set_end();
			return *this;
		}

		WJR_CONSTEXPR20 basic_string& replace(const_iterator first, const_iterator last,
			std::initializer_list<value_type> il) {
			return replace(first, last, view(il));
		}

		template <typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_CONSTEXPR20 basic_string& replace(size_type off, size_type n,
			const StringView& t) {
			const auto sv = view(*this, off, n);
			return replace(sv.begin(), sv.end(), view(t));
		}

		template <typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_CONSTEXPR20 basic_string& replace(const_iterator first, const_iterator last,
			const StringView& t) {
			const auto sv = view(t);
			return replace(first, last, sv.begin(), sv.end());
		}

		template <typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_CONSTEXPR20 basic_string& replace(size_type off1, size_type n1, const StringView& t,
			size_type off2, size_type n2 = npos) {
			return replace(off1, n1, view(t, off2, n2));
		}

		WJR_CONSTEXPR20 allocator_type& get_allocator() {
			return m_core.get_allocator();
		}

		WJR_CONSTEXPR20 const allocator_type& get_allocator()const {
			return m_core.get_allocator();
		}

		// non-standard extension functions

		/*------Internal management function------*/

		WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const data_type& _Src, data_type& _Dest)
			noexcept(noexcept(vector_type::copyConstruct(al, _Src, _Dest))) {
			vector_type::copyConstruct(al, _Src, _Dest);
		}

		WJR_CONSTEXPR20 static void moveConstruct(_Alty& al, data_type&& _Src, data_type& _Dest)
			noexcept(noexcept(vector_type::moveConstruct(al, std::move(_Src), _Dest))) {
			vector_type::moveConstruct(al, std::move(_Src), _Dest);
		}

		WJR_CONSTEXPR20 static void Swap(_Alty& al, data_type& _Left, data_type& other)
			noexcept(noexcept(vector_type::Swap(al, _Left, other))) {
			vector_type::Swap(al, _Left, other);
		}

		WJR_CONSTEXPR20 static void Deallocate(_Alty& al, data_type& _Data)
			noexcept(noexcept(data_type::Deallocate(al, _Data))) {
			vector_type::Deallocate(al, _Data);
		}

		WJR_CONSTEXPR20 static void Tidy(_Alty& al, data_type& _Data) {
			vector_type::Tidy(al, _Data);
		}

		WJR_INLINE_CONSTEXPR20 static size_type getGrowthCapacity(
			const size_type _Oldcapacity, const size_type _Newsize) noexcept {
			return vector_type::getGrowthCapacity(_Oldcapacity, _Newsize);
		}

		WJR_CONSTEXPR20 static void shrinkToFit(_Alty& al, data_type& _Data) {
			vector_type::shrinkToFit(al, _Data);
		}

		WJR_INLINE_CONSTEXPR20 _Alty& getAllocator() noexcept {
			return m_core.getAllocator();
		}
		WJR_INLINE_CONSTEXPR20 const _Alty& getAllocator() const noexcept {
			return m_core.getAllocator();
		}

		WJR_INLINE_CONSTEXPR20 data_type& getData() noexcept {
			return m_core.getData();
		}
		WJR_INLINE_CONSTEXPR20 const data_type& getData() const noexcept {
			return m_core.getData();
		}

		WJR_INLINE_CONSTEXPR20 pointer lastPtr() noexcept {
			return getData().lastPtr();
		}
		WJR_INLINE_CONSTEXPR20 const_pointer lastPtr() const noexcept {
			return getData().lastPtr();
		}

		WJR_INLINE_CONSTEXPR20 pointer endPtr() noexcept {
			return getData().endPtr();
		}
		WJR_INLINE_CONSTEXPR20 const_pointer endPtr() const noexcept {
			return getData().endPtr();
		}

		WJR_INLINE_CONSTEXPR20 void set_size(const size_type _Size) noexcept {
			getData().set_size(_Size);
		}

		WJR_INLINE_CONSTEXPR20 void inc_size(const difference_type _Size) noexcept {
			getData().inc_size(_Size);
		}

		WJR_INLINE_CONSTEXPR20 void set_end() {
			wjr::construct_at(getAllocator(), lastPtr());
		}

		/*------External extension function------*/

		WJR_NODISCARD WJR_CONSTEXPR20 basic_string nesubstr(
			const size_type off = 0, const size_type n = npos) const {
			return basic_string(view(*this, off, n));
		}

		WJR_NODISCARD WJR_INLINE_CONSTEXPR20 static view_type view(const basic_string& str) noexcept {
			return view_type::view(str);
		}

		WJR_NODISCARD WJR_INLINE_CONSTEXPR20 static view_type view(const basic_string& str,
			const size_type off, const size_type n = npos) noexcept {
			return view_type::view(str, off, n);
		}

		WJR_NODISCARD WJR_INLINE_CONSTEXPR20 static view_type view(const value_type* s, const size_type n) noexcept {
			return view_type::view(s, n);
		}

		WJR_NODISCARD WJR_INLINE_CONSTEXPR20 static view_type view(const value_type* s) noexcept {
			return view_type::view(s);
		}

		WJR_NODISCARD WJR_INLINE_CONSTEXPR20 static view_type view(std::initializer_list<value_type> il) noexcept {
			return view_type::view(il);
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_INLINE_CONSTEXPR20 static view_type view(const StringView& t) noexcept {
			return view_type::view(t);
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_INLINE_CONSTEXPR20 static view_type view(const StringView& t,
			const size_type off, const size_type n = npos) noexcept {
			return view_type::view(t, off, n);
		}

		WJR_NODISCARD WJR_INLINE_CONSTEXPR20 view_type view() const noexcept {
			return view(*this);
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_INLINE_CONSTEXPR20 static view_type eview(const StringView& t,
			const size_type off, const size_type n = npos) WJR_NOEXCEPT {
			return view_type::eview(t, off, n);
		}

		WJR_CONSTEXPR20 basic_string& prepend(const basic_string& other) {
			return prepend(view(other));
		}

		WJR_CONSTEXPR20 basic_string& prepend(const basic_string& other, const size_type off, const size_type n = npos) {
			return prepend(view(other, off, n));
		}

		template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
		WJR_CONSTEXPR20 basic_string& prepend(iter first, iter last) {
			insert(begin(), first, last);
			return *this;
		}

		WJR_CONSTEXPR20 basic_string& prepend(const value_type* s, size_type n) {
			return prepend(view(s, n));
		}

		WJR_CONSTEXPR20 basic_string& prepend(const value_type* s) {
			return prepend(view(s));
		}

		WJR_CONSTEXPR20 basic_string& prepend(std::initializer_list<value_type> il) {
			return prepend(view(il));
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_CONSTEXPR20 basic_string& prepend(const StringView& t) {
			const auto sv = view_type(t);
			return prepend(sv.data(), sv.data() + sv.size());
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_CONSTEXPR20 basic_string& prepend(const StringView& t,
			const size_type off, const size_type n = npos) {
			return prepend(view(t, off, n));
		}

		WJR_CONSTEXPR20 basic_string& prepend(const size_type n, const value_type c) {
			insert(begin(), n, c);
			return *this;
		}

		WJR_CONSTEXPR20 basic_string& prepend(const value_type c) {
			m_core.emplace(m_core.begin(), c);
			set_end();
			return *this;
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const StringView& t) const noexcept {
			return view().equal(t);
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const size_type off, const size_type n, const StringView& t)
			const WJR_NOEXCEPT {
			return view().equal(off, n, t);
		}

		template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
		WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const size_type off1, const size_type n1, const StringView& t,
			const size_type off2, const size_type n2) const WJR_NOEXCEPT {
			return view().equal(off1, n1, t, off2, n2);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const Char* const ptr) const {
			return view().equal(ptr);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const size_type off, const size_type n, const Char* const ptr) const {
			return view().equal(off, n, ptr);
		}

		WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const size_type off1, const size_type n1,
			const Char* const ptr, const size_type n2) const {
			return view().equal(off1, n1, ptr, n2);
		}

		WJR_CONSTEXPR20 basic_string(const size_type _Count, default_construct_tag, const allocator_type& al = allocator_type())
			: m_core(_Count, default_construct_tag(), al) {
			set_end();
		}

		WJR_CONSTEXPR20 basic_string(const size_type _Count, value_construct_tag, const allocator_type& al = allocator_type())
			: m_core(_Count, value_construct_tag(), al) {
			set_end();
		}

		WJR_CONSTEXPR20 void resize(const size_type _Newsize, default_construct_tag) {
			m_core.resize(_Newsize, default_construct_tag());
			set_end();
		}

		WJR_CONSTEXPR20 void resize(const size_type _Newsize, value_construct_tag) {
			m_core.resize(_Newsize, value_construct_tag());
			set_end();
		}

		WJR_CONSTEXPR20 void push_back(default_construct_tag) {
			m_core.push_back(default_construct_tag());
			set_end();
		}

		WJR_CONSTEXPR20 void push_back(value_construct_tag) {
			m_core.push_back(value_construct_tag());
			set_end();
		}

		WJR_CONSTEXPR20 basic_string& append(const size_type n, default_construct_tag) {
			m_core.append(n, default_construct_tag());
			set_end();
			return *this;
		}

		WJR_CONSTEXPR20 basic_string& append(const size_type n, value_construct_tag) {
			m_core.append(n, value_construct_tag());
			set_end();
			return *this;
		}

		WJR_CONSTEXPR20 basic_string& chop(const size_type n) noexcept {
			m_core.chop(n);
			set_end();
			return *this;
		}

		WJR_CONSTEXPR20 basic_string& truncate(const size_type n) noexcept {
			m_core.truncate(n);
			set_end();
			return *this;
		}

		WJR_INLINE_CONSTEXPR20 void assume_total_capacity(const size_type n) const noexcept {
			m_core.assume_total_capacity(n);
		}

		WJR_INLINE_CONSTEXPR20 void assume_rest_capacity(const size_type n) const noexcept {
			m_core.assume_rest_capacity(n);
		}

	private:
		vector_type m_core;
};

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
	const basic_string<Char, Traits, Alloc, Data>& lhs,
	const basic_string<Char, Traits, Alloc, Data>& rhs
	) {
	basic_string<Char, Traits, Alloc, Data> ans;
	ans.reserve(lhs.size() + rhs.size());
	ans.assume_rest_capacity(lhs.size());
	ans.append(lhs);
	ans.assume_rest_capacity(rhs.size());
	ans.append(rhs);
	return ans;
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
	const basic_string<Char, Traits, Alloc, Data>& lhs,
	const Char* const rhs
	) {
	basic_string<Char, Traits, Alloc, Data> ans;
	const auto rhs_size = Traits::length(rhs);
	ans.reserve(lhs.size() + rhs_size);
	ans.assume_rest_capacity(lhs.size());
	ans.append(lhs);
	ans.assume_rest_capacity(rhs_size);
	ans.append(rhs, rhs_size);
	return ans;
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
	const basic_string<Char, Traits, Alloc, Data>& lhs,
	const Char rhs
	) {
	basic_string<Char, Traits, Alloc, Data> ans;
	ans.reserve(lhs.size() + 1);
	ans.assume_rest_capacity(lhs.size());
	ans.append(lhs);
	ans.assume_rest_capacity(1);
	ans.append(rhs);
	return ans;
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
	const Char* const lhs,
	const basic_string<Char, Traits, Alloc, Data>& rhs
	) {
	basic_string<Char, Traits, Alloc, Data> ans;
	const auto lhs_size = Traits::length(lhs);
	ans.reserve(lhs_size + rhs.size());
	ans.assume_rest_capacity(lhs_size);
	ans.append(lhs, lhs_size);
	ans.assume_rest_capacity(rhs.size());
	ans.append(rhs);
	return ans;
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
	const Char lhs,
	const basic_string<Char, Traits, Alloc, Data>& rhs
	) {
	basic_string<Char, Traits, Alloc, Data> ans;
	ans.reserve(rhs.size() + 1);
	ans.assume_rest_capacity(1);
	ans.append(lhs);
	ans.assume_rest_capacity(rhs.size());
	ans.append(rhs);
	return ans;
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
	basic_string<Char, Traits, Alloc, Data>&& lhs,
	basic_string<Char, Traits, Alloc, Data>&& rhs
	) {
	return std::move(lhs.append(rhs));
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
	basic_string<Char, Traits, Alloc, Data>&& lhs,
	const basic_string<Char, Traits, Alloc, Data>& rhs
	) {
	return std::move(lhs.append(rhs));
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
	const basic_string<Char, Traits, Alloc, Data>& lhs,
	basic_string<Char, Traits, Alloc, Data>&& rhs
	) {
	return std::move(rhs.prepend(lhs));
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
	basic_string<Char, Traits, Alloc, Data>&& lhs,
	const Char* const rhs
	) {
	return std::move(lhs.append(rhs));
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
	const Char* const lhs,
	basic_string<Char, Traits, Alloc, Data>&& rhs
	) {
	return std::move(rhs.prepend(lhs));
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
	basic_string<Char, Traits, Alloc, Data>&& lhs,
	const Char rhs
	) {
	return std::move(lhs.append(rhs));
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
	const Char lhs,
	basic_string<Char, Traits, Alloc, Data>&& rhs
	) {
	return std::move(rhs.prepend(lhs));
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator==(
	const basic_string<Char, Traits, Alloc, Data>& lhs,
	const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
	return lhs.equal(rhs);
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator==(
	const basic_string<Char, Traits, Alloc, Data>& lhs,
	const Char* rhs) noexcept {
	return lhs.equal(rhs);
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator==(
	const Char* lhs,
	const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
	return rhs == lhs;
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator!=(
	const basic_string<Char, Traits, Alloc, Data>& lhs,
	const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
	return !(lhs == rhs);
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator!=(
	const basic_string<Char, Traits, Alloc, Data>& lhs,
	const Char* rhs) noexcept {
	return !(lhs == rhs);
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator!=(
	const Char* lhs,
	const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
	return !(lhs == rhs);
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<(
	const basic_string<Char, Traits, Alloc, Data>& lhs,
	const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
	return lhs.compare(rhs) < 0;
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<(
	const basic_string<Char, Traits, Alloc, Data>& lhs,
	const Char* rhs) noexcept {
	return lhs.compare(rhs) < 0;
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<(
	const Char* lhs,
	const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
	return rhs.compare(lhs) > 0;
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>(
	const basic_string<Char, Traits, Alloc, Data>& lhs,
	const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
	return rhs < lhs;
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>(
	const basic_string<Char, Traits, Alloc, Data>& lhs,
	const Char* rhs) noexcept {
	return rhs < lhs;
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>(
	const Char* lhs,
	const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
	return rhs < lhs;
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<=(
	const basic_string<Char, Traits, Alloc, Data>& lhs,
	const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
	return !(rhs < lhs);
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<=(
	const basic_string<Char, Traits, Alloc, Data>& lhs,
	const Char* rhs) noexcept {
	return !(rhs < lhs);
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<=(
	const Char* lhs,
	const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
	return !(rhs < lhs);
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>=(
	const basic_string<Char, Traits, Alloc, Data>& lhs,
	const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
	return !(lhs < rhs);
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>=(
	const basic_string<Char, Traits, Alloc, Data>& lhs,
	const Char* rhs) noexcept {
	return !(lhs < rhs);
}

template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>=(
	const Char* lhs,
	const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
	return !(lhs < rhs);
}

template<typename Char, typename Traits, typename Alloc, typename Data>
std::basic_ostream<Char, Traits>& operator<<(
	std::basic_ostream<Char, Traits>& os,
	const basic_string<Char, Traits, Alloc, Data>& str) {
	return os << str.view();
}

template<typename Char, typename Traits, typename Alloc, typename Data>
std::basic_istream<Char, Traits>& operator>>(
	std::basic_istream<Char, Traits>& is,
	basic_string<Char, Traits, Alloc, Data>& str) {
	using IS = std::basic_istream<Char, Traits>;
	using size_type = typename basic_string<Char, Traits, Alloc, Data>::size_type;

	typename IS::iostate state = IS::goodbit;
	const typename IS::sentry sentry(is);

	bool changed = false;

	if (sentry) { // state okay, extract characters

		str.clear();

		size_type _Size;

		if (is.width() && static_cast<size_type>(is.width()) < str.max_size()) {
			_Size = static_cast<size_type>(is.width());
		}
		else {
			_Size = str.max_size();
		}

		typename Traits::int_type _Meta = is.rdbuf()->sgetc();

		for (; 0 < _Size; --_Size, _Meta = is.rdbuf()->snextc()) {
			if (Traits::eq_int_type(Traits::eof(), _Meta)) {
				state |= IS::eofbit;
				break;
			}
			if (isspace(Traits::to_char_type(_Meta))) {
				break;
			}
			str.push_back(Traits::to_char_type(_Meta));
			changed = true;
		}

	}

	is.width(0);

	if (!changed) {
		state |= IS::failbit;
	}

	is.setstate(state);

	return is;
}

using string = basic_string<char>;
#if defined(WJR_CHAR8_T)
using u8string = basic_string<char8_t>;
#endif // WJR_CHAR8_T
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;
using wstring = basic_string<wchar_t>;

template<typename Char, size_t N, typename Traits = std::char_traits<Char>, typename Alloc = std::allocator<Char>>
using static_basic_string = basic_string<Char, Traits, Alloc, string_static_data<Char, N, Alloc>>;

template<size_t N>
using static_string = static_basic_string<char, N>;
#if defined(WJR_CHAR8_T)
template<size_t N>
using static_u8string = static_basic_string<char8_t, N>;
#endif // WJR_CHAR8_T
template<size_t N>
using static_u16string = static_basic_string<char16_t, N>;
template<size_t N>
using static_u32string = static_basic_string<char32_t, N>;
template<size_t N>
using static_wstring = static_basic_string<wchar_t, N>;

namespace literals {

	WJR_NODISCARD WJR_INLINE_CONSTEXPR string_view operator""_sv(const char* str, size_t len) noexcept {
		return string_view(str, len);
	}
#if defined(WJR_CHAR8_T)
	WJR_NODISCARD WJR_INLINE_CONSTEXPR u8string_view operator""_sv(const char8_t* str, size_t len) noexcept {
		return u8string_view(str, len);
	}
#endif // WJR_CHAR8_T
	WJR_NODISCARD WJR_INLINE_CONSTEXPR u16string_view operator""_sv(const char16_t* str, size_t len) noexcept {
		return u16string_view(str, len);
	}
	WJR_NODISCARD WJR_INLINE_CONSTEXPR u32string_view operator""_sv(const char32_t* str, size_t len) noexcept {
		return u32string_view(str, len);
	}
	WJR_NODISCARD WJR_INLINE_CONSTEXPR wstring_view operator""_sv(const wchar_t* str, size_t len) noexcept {
		return wstring_view(str, len);
	}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR20 string operator""_s(const char* str, size_t len) noexcept {
		return string(str, len);
	}
#if defined(WJR_CHAR8_T)
	WJR_NODISCARD WJR_INLINE_CONSTEXPR20 u8string operator""_s(const char8_t* str, size_t len) noexcept {
		return u8string(str, len);
	}
#endif // WJR_CHAR8_T
	WJR_NODISCARD WJR_INLINE_CONSTEXPR20 u16string operator""_s(const char16_t* str, size_t len) noexcept {
		return u16string(str, len);
	}
	WJR_NODISCARD WJR_INLINE_CONSTEXPR20 u32string operator""_s(const char32_t* str, size_t len) noexcept {
		return u32string(str, len);
	}
	WJR_NODISCARD WJR_INLINE_CONSTEXPR20 wstring_view operator""_s(const wchar_t* str, size_t len) noexcept {
		return wstring(str, len);
	}

}

/*---------string elated function---------*/
template <typename Char, typename Traits, typename Alloc, typename Data>
std::basic_istream<Char, Traits>& getline(
	std::basic_istream<Char, Traits>&& is,
	basic_string<Char, Traits, Alloc, Data>& str, const Char delim) {
	// get characters into string, discard delimiter
	using IS = std::basic_istream<Char, Traits>;

	typename IS::iostate state = IS::goodbit;
	const typename IS::sentry sentry(is, true);

	bool changed = false;

	if (sentry) {
		str.clear();
		const typename Traits::int_type _Metadelim = Traits::to_int_type(delim);
		typename Traits::int_type _Meta = is.rdbuf()->sgetc();

		for (;; _Meta = is.rdbuf()->snextc()) {
			if (Traits::eq_int_type(Traits::eof(), _Meta)) { // end of file, quit
				state |= IS::eofbit;
				break;
			}
			else if (Traits::eq_int_type(_Meta, _Metadelim)) { // got a delimiter, discard it and quit
				changed = true;
				is.rdbuf()->sbumpc();
				break;
			}
			else if (str.max_size() <= str.size()) { // string too large, quit
				state |= IS::failbit;
				break;
			}
			else { // got a character, add it to string
				str.append(Traits::to_char_type(_Meta));
				changed = true;
			}
		}
	}

	if (!changed) {
		state |= IS::failbit;
	}

	is.setstate(state);
	return static_cast<IS&>(is);
}

template <typename Char, typename  Traits, typename Alloc, typename Data>
std::basic_istream<Char, Traits>& getline(
	std::basic_istream<Char, Traits>&& is, basic_string<Char, Traits, Alloc, Data>& str) {
	// get characters into string, discard newline
	return getline(std::move(is), str, is.widen('\n'));
}

template <typename Char, typename Traits, typename Alloc, typename Data>
std::basic_istream<Char, Traits>& getline(
	std::basic_istream<Char, Traits>& is, basic_string<Char, Traits, Alloc, Data>& str, const Char delim) {
	// get characters into string, discard delimiter
	return getline(std::move(is), str, delim);
}

template <typename Char, typename Traits, typename Alloc, typename Data>
std::basic_istream<Char, Traits>& getline(
	std::basic_istream<Char, Traits>& is, basic_string<Char, Traits, Alloc, Data>& str) {
	// get characters into string, discard newline
	return getline(std::move(is), str, is.widen('\n'));
}

/*---------------------------algorithm for string---------------------------*/


template<typename To, typename Char, typename From>
WJR_NODISCARD inline To str2i_helper(const Char* str, Char** str_end, From ans) {
	constexpr auto _Max = std::numeric_limits<To>::max();
	constexpr auto _Min = std::numeric_limits<To>::min();

	if constexpr (_Max == std::numeric_limits<From>::max()
		&& _Min == std::numeric_limits<From>::min()) {
		return ans;
	}
	else {
		if (str == *str_end) return static_cast<To>(0);
		int& errno_ref = errno;
		if (errno_ref == ERANGE) {
			return ans == std::numeric_limits<From>::max() ? _Max : _Min;
		}
		if (ans > _Max) {
			errno_ref = ERANGE;
			return _Max;
		}
		if (ans < _Min) {
			errno_ref = ERANGE;
			return _Min;
		}
		return static_cast<To>(ans);
	}
}

template<typename To, std::enable_if_t<std::is_integral_v<To>, int> = 0>
WJR_NODISCARD inline To str2i(const char* str, char** str_end, int base) {
	constexpr auto _Nd = std::numeric_limits<To>::digits;
	constexpr auto _Max = std::numeric_limits<To>::max();
	constexpr auto _Min = std::numeric_limits<To>::min();

	if constexpr (std::is_unsigned_v<To>) {
		constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
		if constexpr (_Nd <= _Nd_ul) {
			return str2i_helper<To>(str, str_end, std::strtoul(str, str_end, base));
		}
		else {
			return str2i_helper<To>(str, str_end, std::strtoull(str, str_end, base));
		}
	}
	else {
		constexpr auto _Nd_l = std::numeric_limits<long>::digits;
		if constexpr (_Nd <= _Nd_l) {
			return str2i_helper<To>(str, str_end, std::strtol(str, str_end, base));
		}
		else {
			return str2i_helper<To>(str, str_end, std::strtoll(str, str_end, base));
		}
	}
}

template<typename To, std::enable_if_t<std::is_floating_point_v<To>, int> = 0>
WJR_NODISCARD inline To str2f(const char* str, char** str_end) {
	if constexpr (std::is_same_v<To, float>) {
		return std::strtof(str, str_end);
	}
	else if constexpr (std::is_same_v<To, double>) {
		return std::strtod(str, str_end);
	}
	else {
		return std::strtold(str, str_end);
	}
}

template<typename To, std::enable_if_t<std::is_integral_v<To>, int> = 0>
WJR_NODISCARD inline To str2i(const wchar_t* str, wchar_t** str_end, int base) {
	constexpr auto _Nd = std::numeric_limits<To>::digits;
	constexpr auto _Max = std::numeric_limits<To>::max();
	constexpr auto _Min = std::numeric_limits<To>::min();

	if constexpr (std::is_unsigned_v<To>) {
		constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
		if constexpr (_Nd <= _Nd_ul) {
			return str2i_helper<To>(str, str_end, std::wcstoul(str, str_end, base));
		}
		else {
			return str2i_helper<To>(str, str_end, std::wcstoull(str, str_end, base));
		}
	}
	else {
		constexpr auto _Nd_l = std::numeric_limits<long>::digits;
		if constexpr (_Nd <= _Nd_l) {
			return str2i_helper<To>(str, str_end, std::wcstol(str, str_end, base));
		}
		else {
			return str2i_helper<To>(str, str_end, std::wcstoll(str, str_end, base));
		}
	}
}

template<typename To, std::enable_if_t<std::is_floating_point_v<To>, int> = 0>
WJR_NODISCARD inline To str2f(const wchar_t* str, wchar_t** str_end) {
	if constexpr (std::is_same_v<To, float>) {
		return std::wcstof(str, str_end);
	}
	else if constexpr (std::is_same_v<To, double>) {
		return std::wcstod(str, str_end);
	}
	else {
		return std::wcstold(str, str_end);
	}
}

template<typename Char>
WJR_NODISCARD inline int stoi(const basic_string<Char>& str, size_t* pos = nullptr, int base = 10) {
	int& errno_ref = errno;
	errno_ref = 0;
	const Char* ptr = str.data();
	Char* e = nullptr;
	const int ans = str2i<int>(ptr, &e, base);

#if defined(_WJR_EXCEPTION)

	if (ptr == e) {
		throw std::invalid_argument("invalid stoi argument");
	}

	if (errno_ref == ERANGE) {
		throw std::out_of_range("stoi argument out of range");
	}

#endif // _WJR_EXCEPTION

	if (pos) {
		*pos = static_cast<size_t>(e - ptr);
	}

	return ans;
}

template<typename Char>
WJR_NODISCARD inline long stol(const basic_string<Char>& str, size_t* pos = nullptr, int base = 10) {
	int& errno_ref = errno;
	errno_ref = 0;
	const Char* ptr = str.data();
	Char* e = nullptr;
	const long ans = str2i<long>(ptr, &e, base);

#if defined(_WJR_EXCEPTION)

	if (ptr == e) {
		throw std::invalid_argument("invalid stol argument");
	}

	if (errno_ref == ERANGE) {
		throw std::out_of_range("stol argument out of range");
	}

#endif // _WJR_EXCEPTION

	if (pos) {
		*pos = static_cast<size_t>(e - ptr);
	}

	return ans;
}

template<typename Char>
WJR_NODISCARD inline long long stoll(const basic_string<Char>& str, size_t* pos = nullptr, int base = 10) {
	int& errno_ref = errno;
	errno_ref = 0;
	const Char* ptr = str.data();
	Char* e = nullptr;
	const long long ans = str2i<long long>(ptr, &e, base);

#if defined(_WJR_EXCEPTION)

	if (ptr == e) {
		throw std::invalid_argument("invalid stoll argument");
	}

	if (errno_ref == ERANGE) {
		throw std::out_of_range("stoll argument out of range");
	}

#endif // _WJR_EXCEPTION

	if (pos) {
		*pos = static_cast<size_t>(e - ptr);
	}

	return ans;
}

template<typename Char>
WJR_NODISCARD inline unsigned long stoul(const basic_string<Char>& str, size_t* pos = nullptr, int base = 10) {
	int& errno_ref = errno;
	errno_ref = 0;
	const Char* ptr = str.data();
	Char* e = nullptr;
	const unsigned long ans = str2i<unsigned long>(ptr, &e, base);

#if defined(_WJR_EXCEPTION)

	if (ptr == e) {
		throw std::invalid_argument("invalid stoul argument");
	}

	if (errno_ref == ERANGE) {
		throw std::out_of_range("stoul argument out of range");
	}

#endif // _WJR_EXCEPTION

	if (pos) {
		*pos = static_cast<size_t>(e - ptr);
	}

	return ans;
}

template<typename Char>
WJR_NODISCARD inline unsigned long long stoull(const basic_string<Char>& str, size_t* pos = nullptr, int base = 10) {
	int& errno_ref = errno;
	errno_ref = 0;
	const Char* ptr = str.data();
	Char* e = nullptr;
	const unsigned long long ans = str2i<unsigned long long>(ptr, &e, base);

#if defined(_WJR_EXCEPTION)

	if (ptr == e) {
		throw std::invalid_argument("invalid stoull argument");
	}

	if (errno_ref == ERANGE) {
		throw std::out_of_range("stoull argument out of range");
	}

#endif // _WJR_EXCEPTION

	if (pos) {
		*pos = static_cast<size_t>(e - ptr);
	}

	return ans;
}

template<typename Char>
WJR_NODISCARD inline float stof(const basic_string<Char>& str, size_t* pos = nullptr) {
	int& errno_ref = errno;
	errno_ref = 0;
	const Char* ptr = str.data();
	Char* e = nullptr;
	const float ans = str2f<float>(ptr, &e);

#if defined(_WJR_EXCEPTION)

	if (ptr == e) {
		throw std::invalid_argument("invalid stof argument");
	}

	if (errno_ref == ERANGE) {
		throw std::out_of_range("stof argument out of range");
	}

#endif // _WJR_EXCEPTION

	if (pos) {
		*pos = static_cast<size_t>(e - ptr);
	}

	return ans;
}

template<typename Char>
WJR_NODISCARD inline double stod(const basic_string<Char>& str, size_t* pos = nullptr) {
	int& errno_ref = errno;
	errno_ref = 0;
	const Char* ptr = str.data();
	Char* e = nullptr;
	const double ans = str2f<double>(ptr, &e);

#if defined(_WJR_EXCEPTION)

	if (ptr == e) {
		throw std::invalid_argument("invalid stod argument");
	}

	if (errno_ref == ERANGE) {
		throw std::out_of_range("stod argument out of range");
	}

#endif // _WJR_EXCEPTION

	if (pos) {
		*pos = static_cast<size_t>(e - ptr);
	}

	return ans;
}

template<typename Char>
WJR_NODISCARD inline long double stold(const basic_string<Char>& str, size_t* pos = nullptr) {
	int& errno_ref = errno;
	errno_ref = 0;
	const Char* ptr = str.data();
	Char* e = nullptr;
	const long double ans = str2f<long double>(ptr, &e);

#if defined(_WJR_EXCEPTION)

	if (ptr == e) {
		throw std::invalid_argument("invalid stold argument");
	}

	if (errno_ref == ERANGE) {
		throw std::out_of_range("stold argument out of range");
	}

#endif // _WJR_EXCEPTION

	if (pos) {
		*pos = static_cast<size_t>(e - ptr);
	}

	return ans;
}

WJR_NODISCARD inline string to_string(int val) {
	char buf[12];
	const int len = snprintf(buf, sizeof(buf), "%d", val);
	return string(buf, static_cast<size_t>(len));
}

WJR_NODISCARD inline string to_string(long val) {
	char buf[24];
	const int len = snprintf(buf, sizeof(buf), "%ld", val);
	return string(buf, static_cast<size_t>(len));
}

WJR_NODISCARD inline string to_string(long long val) {
	char buf[24];
	const int len = snprintf(buf, sizeof(buf), "%lld", val);
	return string(buf, static_cast<size_t>(len));
}

WJR_NODISCARD inline string to_string(unsigned val) {
	char buf[12];
	const int len = snprintf(buf, sizeof(buf), "%u", val);
	return string(buf, static_cast<size_t>(len));
}

WJR_NODISCARD inline string to_string(unsigned long val) {
	char buf[24];
	const int len = snprintf(buf, sizeof(buf), "%lu", val);
	return string(buf, static_cast<size_t>(len));
}

WJR_NODISCARD inline string to_string(unsigned long long val) {
	char buf[24];
	const int len = snprintf(buf, sizeof(buf), "%llu", val);
	return string(buf, static_cast<size_t>(len));
}

WJR_NODISCARD inline string to_string(float val) {
	char buf[24];
	const int len = snprintf(buf, sizeof(buf), "%f", val);
	return string(buf, static_cast<size_t>(len));
}

WJR_NODISCARD inline string to_string(double val) {
	char buf[24];
	const int len = snprintf(buf, sizeof(buf), "%f", val);
	return string(buf, static_cast<size_t>(len));
}

WJR_NODISCARD inline string to_string(long double val) {
	char buf[24];
	const int len = snprintf(buf, sizeof(buf), "%Lf", val);
	return string(buf, static_cast<size_t>(len));
}

_WJR_END

namespace std {
	template<typename Char, typename Traits>
	struct hash<wjr::basic_string_view<Char, Traits>> {
		constexpr decltype(auto) operator()(const wjr::basic_string_view<Char, Traits>& str) const noexcept {
			return std::hash<std::basic_string_view<Char, Traits>>()(str);
		}
	};
	template<typename Char, typename Traits, typename Alloc, typename Data>
	struct hash<wjr::basic_string<Char, Traits, Alloc, Data>> {
		constexpr decltype(auto) operator()(const wjr::basic_string<Char, Traits, Alloc, Data>& str) const noexcept {
			return std::hash<std::basic_string_view<Char, Traits>>()(str);
		}
	};
}

_WJR_BEGIN

// encode functions
// the code writing specification is related to the function namespace, 
// which requires encoding auxiliary functions and auxiliary classes, 
// followed by defining corresponding traits and basic_string_view, implemented after all
 
namespace ascii {

	template<int Base, int idx = 0>
	struct __to_digit_table {

		constexpr static uint32_t invalid = 1u << 24;
		constexpr static uint32_t multi = power<uint32_t>(Base, idx);

		static_assert(Base >= 2 && Base <= 36, "");
		static_assert(idx >= 0 && idx < 4, "");

		constexpr __to_digit_table() : table() {
			for (int i = 0; i < 256; ++i) {
				table[i] = invalid;
			}

			{
				constexpr int _Max = '0' + (Base >= 10 ? 10 : Base) - 1;
				for (int i = '0'; i <= _Max; ++i) {
					table[i] = (i - '0') * multi;
				}
			}

			if constexpr (Base > 10) {
				constexpr int _Maxl = 'a' + (Base - 10) - 1;
				constexpr int _Maxu = _Maxl + ('A' - 'a');
				for (int i = 'a'; i <= _Maxl; ++i) {
					table[i] = ((i - 'a') + 10) * multi;
				}

				for (int i = 'A'; i <= _Maxu; ++i) {
					table[i] = ((i - 'A') + 10) * multi;
				}
			}

		}

		constexpr uint32_t operator[](uint8_t ch) const { return table[ch]; }

	private:
		uint32_t table[256];
	};

	template<int Base, int idx = 0>
	constexpr static __to_digit_table<Base, idx> __to_digit_table_v;

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

	class encode {
	public:

		using value_type = char;

		WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isalnum(char ch) {
			return __char_code_table[make_unsigned_v(ch)] & (__lower | __upper | __digit);
		}

		WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isalpha(char ch) {
			return __char_code_table[make_unsigned_v(ch)] & (__lower | __upper);
		}

		WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool islower(char ch) {
			return __char_code_table[make_unsigned_v(ch)] & __lower;
		}

		WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isupper(char ch) {
			return __char_code_table[make_unsigned_v(ch)] & __upper;
		}

		WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isdigit(char ch) {
			return __char_code_table[make_unsigned_v(ch)] & __digit;
		}

		WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isxdigit(char ch) {
			return __char_code_table[make_unsigned_v(ch)] & __xdigit;
		}

		WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isspace(char ch) {
			return __char_code_table[make_unsigned_v(ch)] & __white;
		}

		WJR_NODISCARD WJR_INLINE_CONSTEXPR static char tolower(char ch) {
			return isupper(ch) ? ch + 'a' - 'A' : ch;
		}

		WJR_NODISCARD WJR_INLINE_CONSTEXPR static char toupper(char ch) {
			return islower(ch) ? ch + 'A' - 'a' : ch;
		}

		// transfrom ch to digit
	// if ch is a valid digit, return digit(ch) * (Base ^ idx)
	// else return invalid
		template<unsigned int Base = 10, int idx = 0>
		WJR_NODISCARD WJR_INLINE_CONSTEXPR static uint32_t todigit(char ch) {
			return __to_digit_table_v<Base, idx>[ch];
		}

	};

	using func_type = string_func<encode>;

}

template<typename Traits = std::char_traits<char>>
struct ascii_traits : public Traits {};

template<typename Traits>
struct __traits_helper<ascii_traits<Traits>> : __traits_helper<Traits> {};

template<typename Traits>
class basic_string_view<char, ascii_traits<Traits>>
	: public basic_string_view<char, Traits>,
	public __base_string_view<basic_string_view<char, ascii_traits<Traits>>> {
private:
	using _Mybase1 = basic_string_view<char, Traits>;
	using _Mybase2 = __base_string_view<basic_string_view<char, ascii_traits<Traits>>>;
public:
	using _Mybase1::_Mybase1;
	using _Mybase1::operator=;
	using _Mybase1::begin;
	using _Mybase1::end;

	using _Mybase2::swap;
	using _Mybase2::substr;
	using _Mybase2::view;
	using _Mybase2::eview;
	using _Mybase2::prefix;
	using _Mybase2::suffix;

	using traits_type = ascii_traits<Traits>;
	using size_type = typename _Mybase1::size_type;
	using func_type = string_func<typename ascii::encode>;
	using flags = typename func_type::flags;

	basic_string_view(const _Mybase1& base) : _Mybase1(base) {}

	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isalnum(char ch);
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isalpha(char ch);
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool islower(char ch);
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isupper(char ch);
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isdigit(char ch);
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isxdigit(char ch);
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isspace(char ch);
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static char tolower(char ch);
	WJR_NODISCARD WJR_INLINE_CONSTEXPR static char toupper(char ch);

	WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_string_view ltrim() const;
	WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_string_view rtrim() const;
	WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_string_view trim() const;

	// support constexpr if str is constexpr
	template<typename T>
	WJR_NODISCARD WJR_INLINE_CONSTEXPR T to_integral(
		errc* err = nullptr, size_type* pos = nullptr, int base = 10) const;

	WJR_NODISCARD WJR_INLINE_CONSTEXPR int toi(
		errc* err = nullptr, size_type* pos = nullptr, int base = 10) const;

	WJR_NODISCARD WJR_INLINE_CONSTEXPR long tol(
		errc* err = nullptr, size_type* pos = nullptr, int base = 10) const;

	WJR_NODISCARD WJR_INLINE_CONSTEXPR long long toll(
		errc* err = nullptr, size_type* pos = nullptr, int base = 10) const;

	WJR_NODISCARD WJR_INLINE_CONSTEXPR unsigned int toui(
		errc* err = nullptr, size_type* pos = nullptr, int base = 10) const;

	WJR_NODISCARD WJR_INLINE_CONSTEXPR unsigned long toul(
		errc* err = nullptr, size_type* pos = nullptr, int base = 10) const;

	WJR_NODISCARD WJR_INLINE_CONSTEXPR unsigned long long toull(
		errc* err = nullptr, size_type* pos = nullptr, int base = 10) const;

};

using ascii_view = basic_string_view<char, ascii_traits<>>;

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool basic_string_view<char, ascii_traits<Traits>>::isalnum(char ch) {
	return func_type::isalnum(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool basic_string_view<char, ascii_traits<Traits>>::isalpha(char ch) {
	return func_type::isalpha(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool basic_string_view<char, ascii_traits<Traits>>::islower(char ch) {
	return func_type::islower(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool basic_string_view<char, ascii_traits<Traits>>::isupper(char ch) {
	return func_type::isupper(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool basic_string_view<char, ascii_traits<Traits>>::isdigit(char ch) {
	return func_type::isdigit(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool basic_string_view<char, ascii_traits<Traits>>::isxdigit(char ch) {
	return func_type::isxdigit(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool basic_string_view<char, ascii_traits<Traits>>::isspace(char ch) {
	return func_type::isspace(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR char basic_string_view<char, ascii_traits<Traits>>::tolower(char ch) {
	return func_type::tolower(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR char basic_string_view<char, ascii_traits<Traits>>::toupper(char ch) {
	return func_type::toupper(ch);
}

template<typename Traits>
WJR_INLINE_CONSTEXPR basic_string_view<char, ascii_traits<Traits>>
	basic_string_view<char, ascii_traits<Traits>>::ltrim() const {
	const char* s = begin();
	const char* e = end();
	s = func_type::skipw(s, e);
	return basic_string_view(s, e - s);
}

template<typename Traits>
WJR_INLINE_CONSTEXPR basic_string_view<char, ascii_traits<Traits>>
basic_string_view<char, ascii_traits<Traits>>::rtrim() const {
	const char* s = begin();
	const char* e = end();
	e = func_type::rskipw(s, e);
	return basic_string_view(s, e - s);
}

template<typename Traits>
WJR_INLINE_CONSTEXPR basic_string_view<char, ascii_traits<Traits>>
basic_string_view<char, ascii_traits<Traits>>::trim() const {
	return ltrim().rtrim();
}

template<typename Traits>
template<typename T>
WJR_NODISCARD WJR_INLINE_CONSTEXPR T basic_string_view<char, ascii_traits<Traits>>::to_integral(
	errc* err, size_type* pos, int base) const {
	using namespace enum_ops;

	constexpr auto _Flags =
		flags::ALLOW_PREFIX
		| flags::ALLOW_SIGN
		| flags::ALLOW_LEADING_SPACE
		| flags::ALLOW_LEADING_ZEROS;

	errc cc = errc::ok;
	const char* end_ptr = nullptr;
	T ret = func_type::to_integral<T>(
		std::integral_constant<flags, _Flags>(),
		begin(), end(), cc, end_ptr, base);

	if (err != nullptr) {
		*err = cc;
	}

	if (pos != nullptr) {
		if (cc == errc::noconv) {
			*pos = static_cast<size_type>(0);
		}
		else {
			*pos = static_cast<size_type>(end_ptr - begin());
		}
	}

	return ret;
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR int basic_string_view<char, ascii_traits<Traits>>::toi(
	errc* err, size_type* pos, int base) const {
	return to_integral<int>(err, pos, base);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR long basic_string_view<char, ascii_traits<Traits>>::tol(
	errc* err, size_type* pos, int base) const {
	return to_integral<long>(err, pos, base);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR long long basic_string_view<char, ascii_traits<Traits>>::toll(
	errc* err, size_type* pos, int base) const {
	return to_integral<long long>(err, pos, base);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR unsigned int basic_string_view<char, ascii_traits<Traits>>::toui(
	errc* err, size_type* pos, int base) const {
	return to_integral<unsigned int>(err, pos, base);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR unsigned long basic_string_view<char, ascii_traits<Traits>>::toul(
	errc* err, size_type* pos, int base) const {
	return to_integral<unsigned long>(err, pos, base);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR unsigned long long basic_string_view<char, ascii_traits<Traits>>::toull(
	errc* err, size_type* pos, int base) const {
	return to_integral<unsigned long long>(err, pos, base);
}

namespace utf8 {
	enum __char_code {
		__invalid_byte = 0x00,
		__one_byte = 0x01,
		__two_byte = 0x02,
		__three_byte = 0x03,
		__four_byte = 0x04,
		__tail_byte = 0x08,
	};

	static constexpr int __char_code_table[256] = {

		__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
		__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
		__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
		__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
		__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
		__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
		__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
		__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
		__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
		__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
		__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
		__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
		__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
		__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
		__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
		__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,

		__tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte,
		__tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte,
		__tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte,
		__tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte,
		__tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte,
		__tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte,
		__tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte,
		__tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte,

		__two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte,
		__two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte,
		__two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte,
		__two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte,

		__three_byte, __three_byte, __three_byte, __three_byte, __three_byte, __three_byte, __three_byte, __three_byte,
		__three_byte, __three_byte, __three_byte, __three_byte, __three_byte, __three_byte, __three_byte, __three_byte,

		__four_byte, __four_byte, __four_byte, __four_byte, __four_byte, __four_byte, __four_byte, __four_byte,

		__invalid_byte, __invalid_byte, __invalid_byte, __invalid_byte,
		__invalid_byte, __invalid_byte, __invalid_byte, __invalid_byte,
	};

	WJR_NODISCARD WJR_INLINE_CONSTEXPR20 int get_code(uint8_t c) {
		return __char_code_table[c];
	}

	// return - 1 if the length is insufficient
	// return - 2 if illegal
	template<typename iter1, typename iter2>
	WJR_NODISCARD WJR_CONSTEXPR20 uint32_t decode(iter1& _First, iter2 _Last) {
		if (is_unlikely(_First == _Last)) {
			return -1;
		}
		uint8_t c;
		c = *_First;
		++_First;
		int len = get_code(c) & 0x07;
		if (!len) {
			return -2;
		}

		switch (len) {
		case 4: {
			uint8_t t0, t1, t2;

			if (is_unlikely(_First == _Last)) {
				return -1;
			}
			t0 = *_First;
			++_First;
			if (get_code(t0) != __tail_byte) {
				return -2;
			}

			if (is_unlikely(_First == _Last)) {
				return -1;
			}
			t1 = *_First;
			++_First;
			if (get_code(t1) != __tail_byte) {
				return -2;
			}

			if (is_unlikely(_First == _Last)) {
				return -1;
			}
			t2 = *_First;
			++_First;
			if (get_code(t2) != __tail_byte) {
				return -2;
			}

			return ((c & 0x07) << 18)
				| ((t0 & 0x3f) << 12)
				| ((t1 & 0x3f) << 6)
				| (t2 & 0x3f);
		}
		case 3: {
			uint8_t t0, t1;

			if (is_unlikely(_First == _Last)) {
				return -1;
			}
			t0 = *_First;
			++_First;
			if (get_code(t0) != __tail_byte) {
				return -2;
			}

			if (is_unlikely(_First == _Last)) {
				return -1;
			}
			t1 = *_First;
			++_First;
			if (get_code(t1) != __tail_byte) {
				return -2;
			}

			return ((c & 0x0f) << 12)
				| ((t0 & 0x3f) << 6)
				| (t1 & 0x3f);
		}
		case 2: {
			uint8_t t0;

			if (is_unlikely(_First == _Last)) {
				return -1;
			}
			t0 = *_First;
			++_First;
			if (get_code(t0) != __tail_byte) {
				return -2;
			}

			return ((c & 0x1f) << 6)
				| (t0 & 0x3f);
		}
		case 1: {
			return c;
		}
		default: {
			WJR_UNREACHABLE;
		}
		}

	}

	template<typename iter1, typename iter2>
	WJR_NODISCARD WJR_CONSTEXPR20 uint32_t rdecode(iter1& _First, iter2 _Last) {
		if (is_unlikely(_First == _Last)) {
			return -1;
		}
		uint8_t c;
		c = *_First;
		++_First;
		int len = get_code(c) & 0x07;
		if (!len) {
			return -2;
		}

		switch (len) {
		case 4: {
			uint8_t t0, t1, t2;

			if (is_unlikely(_First == _Last)) {
				return -1;
			}
			t0 = *_First;
			++_First;
			if (get_code(t0) != __tail_byte) {
				return -2;
			}

			if (is_unlikely(_First == _Last)) {
				return -1;
			}
			t1 = *_First;
			++_First;
			if (get_code(t1) != __tail_byte) {
				return -2;
			}

			if (is_unlikely(_First == _Last)) {
				return -1;
			}
			t2 = *_First;
			++_First;
			if (get_code(t2) != __tail_byte) {
				return -2;
			}

			return ((c & 0x07) << 18)
				| ((t0 & 0x3f) << 12)
				| ((t1 & 0x3f) << 6)
				| (t2 & 0x3f);
		}
		case 3: {
			uint8_t t0, t1;

			if (is_unlikely(_First == _Last)) {
				return -1;
			}
			t0 = *_First;
			++_First;
			if (get_code(t0) != __tail_byte) {
				return -2;
			}

			if (is_unlikely(_First == _Last)) {
				return -1;
			}
			t1 = *_First;
			++_First;
			if (get_code(t1) != __tail_byte) {
				return -2;
			}

			return ((c & 0x0f) << 12)
				| ((t0 & 0x3f) << 6)
				| (t1 & 0x3f);
		}
		case 2: {
			uint8_t t0;

			if (is_unlikely(_First == _Last)) {
				return -1;
			}
			t0 = *_First;
			++_First;
			if (get_code(t0) != __tail_byte) {
				return -2;
			}

			return ((c & 0x1f) << 6)
				| (t0 & 0x3f);
		}
		case 1: {
			return c;
		}
		default: {
			WJR_UNREACHABLE;
		}
		}

	}

	// return - 2 if illegal
	// return 0 if legal
	template<typename iter>
	WJR_NODISCARD WJR_INLINE_CONSTEXPR20 uint32_t encode(uint32_t c, iter& _First) {
		if (c > 0x10ffff)return -2;
		if (c < 0x80) {
			*_First = static_cast<uint8_t>(c);
			++_First;
			return 0;
		}
		if (c < 0x800) {
			*_First = static_cast<uint8_t>(0xc0 | (c >> 6));
			++_First;
			*_First = static_cast<uint8_t>(0x80 | (c & 0x3f));
			++_First;
			return 0;
		}
		if (c < 0x10000) {
			*_First = static_cast<uint8_t>(0xe0 | (c >> 12));
			++_First;
			*_First = static_cast<uint8_t>(0x80 | ((c >> 6) & 0x3f));
			++_First;
			*_First = static_cast<uint8_t>(0x80 | (c & 0x3f));
			++_First;
			return 0;
		}
		*_First = static_cast<uint8_t>(0xf0 | (c >> 18));
		++_First;
		*_First = static_cast<uint8_t>(0x80 | ((c >> 12) & 0x3f));
		++_First;
		*_First = static_cast<uint8_t>(0x80 | ((c >> 6) & 0x3f));
		++_First;
		*_First = static_cast<uint8_t>(0x80 | (c & 0x3f));
		++_First;
		return 0;
	}

}

_WJR_END

#endif // __WJR_STRING_H
