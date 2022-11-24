#pragma once
#ifndef __WJR_VIEW_H
#define __WJR_VIEW_H

#include <functional>

#include <wjr/type_traits.h>

_WJR_BEGIN

template<typename iter, std::enable_if_t<wjr::is_forward_iter_v<iter> ,int> = 0>
class view;

template<typename = void, typename...Args>
struct __can_make_view : std::false_type {};

template<typename...Args>
struct __can_make_view<std::void_t<decltype(view(std::declval<Args>()...))>, Args...> : std::true_type {};

template<typename...Args>
struct can_make_view : __can_make_view<void, Args...> {};

template<typename...Args>
inline constexpr bool can_make_view_v = can_make_view<Args...>::value;

template<typename iter, typename ic>
class view_base;

template<typename iter>
class view_base<iter, std::forward_iterator_tag> {
	using _Iterator_traits = typename std::iterator_traits<iter>;
public:
	using value_type = typename _Iterator_traits::value_type;
	using reference = typename _Iterator_traits::reference;

	static_assert(
		!(std::is_reference_v<reference> && !std::is_const_v<std::remove_reference_t<reference>>),
		"reference type of iterator must be const or non-reference"
		);

	using iterator = iter;

	constexpr view_base(const view_base&) = default;
	constexpr view_base(view_base&&) = default;
	constexpr view_base& operator=(const view_base&) = default;
	constexpr view_base& operator=(view_base&&) = default;

	constexpr view_base(iter _First, iter _Last) 
		noexcept(std::is_nothrow_copy_constructible_v<iter>)
		: _Myfirst(_First), _Mylast(_Last) {}

	template<typename _Container, std::enable_if_t<
		std::conjunction_v<
		std::has_global_function_begin<const _Container&>,
		std::has_global_function_end<const _Container&>,
		std::is_constructible<iter, decltype(std::begin(std::declval<const _Container&>()))>,
		std::is_constructible<iter, decltype(std::end(std::declval<const _Container&>()))>
	>, int> = 0>
	constexpr view_base(const _Container& _Cont)
		noexcept(std::is_nothrow_constructible_v<iter>)
		: _Myfirst(std::begin(_Cont)), _Mylast(std::end(_Cont)) {}

	constexpr iterator begin() const noexcept {
		return _Myfirst;
	}

	constexpr iterator end() const noexcept {
		return _Mylast;
	}

	constexpr bool empty() const noexcept {
		return _Myfirst == _Mylast;
	}

	constexpr view<iter> subview(iter _First, iter _Last) const noexcept {
		return view<iter>(_First, _Last);
	}

	constexpr view<iter> subview(iter _First) const noexcept {
		return view<iter>(_First, _Mylast);
	}

	constexpr reference front() const noexcept {
		return *_Myfirst;
	}

	constexpr bool starts_with(reference _Val) const noexcept {
		return !empty() && front() == _Val;
	}

	template<typename _Pred>
	constexpr bool starts_with(view<iter> _View, _Pred&& pred) const noexcept {
		return std::equal(std::begin(_View), std::end(_View), begin(), end(), std::forward<_Pred>(pred));
	}

	constexpr bool starts_with(view<iter> _View) const noexcept {
		return starts_with(_View, std::equal_to<>());
	}

private:
	iter _Myfirst;
	iter _Mylast;
};

template<typename iter>
class view_base<iter, std::bidirectional_iterator_tag>
	: public view_base<iter, std::forward_iterator_tag> {
	using _Base = view_base<iter, std::forward_iterator_tag>;
public:
	using value_type = typename _Base::value_type;
	using reference = typename _Base::reference;

	using iterator = typename _Base::iterator;

	using reverse_iterator = std::reverse_iterator<iterator>;
	
	using _Base::_Base;
	using _Base::operator=;
	using _Base::begin;
	using _Base::end;
	using _Base::empty;
	using _Base::subview;
	using _Base::front;
	using _Base::starts_with;

	constexpr iterator rbegin() const noexcept {
		return std::reverse_iterator<iterator>(end());
	}

	constexpr iterator rend() const noexcept {
		return std::reverse_iterator<iterator>(begin());
	}

	constexpr reference back() const noexcept {
		return *(end() - 1);
	}

	constexpr bool ends_with(reference _Val) const noexcept {
		return !empty() && back() == _Val;
	}

	template<typename _Pred>
	constexpr bool ends_with(view<iter> _View, _Pred&& pred) const noexcept {
		return std::equal(std::rbegin(_View), std::rend(_View), rbegin(), rend(), std::forward<_Pred>(pred));
	}

	constexpr bool ends_with(view<iter> _View) const noexcept {
		return ends_with(_View, std::equal_to<>());
	}

};

template<typename iter>
class view_base<iter, std::random_access_iterator_tag> 
	: public view_base<iter, std::bidirectional_iterator_tag> {
	using _Base = view_base<iter, std::bidirectional_iterator_tag>;
public:
	using value_type = typename _Base::value_type;
	using reference = typename _Base::reference;
	using const_reference = typename _Base::const_reference;

	using iterator = typename _Base::iterator;
	using const_iterator = typename _Base::const_iterator;

	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	using _Base::_Base;
	using _Base::operator=;
	using _Base::begin;
	using _Base::end;
	using _Base::rbegin;
	using _Base::rend;
	using _Base::empty;
	using _Base::subview;
	using _Base::front;
	using _Base::back;
	using _Base::starts_with;
	using _Base::ends_with;

	constexpr view<iter> subview(size_t _Off, size_t _Count) const noexcept {
		return view<iter>(begin() + _Off, begin() + _Off + _Count);
	}

	constexpr view<iter> subview(size_t _Off) const noexcept {
		return view<iter>(begin() + _Off, end());
	}

};

// only support random_access_iterator now
template<typename iter, std::enable_if_t<wjr::is_forward_iter_v<iter>, int>>
class view : public view_base<iter, typename std::iterator_traits<iter>::iterator_category> {
	using _Base = view_base<iter, typename std::iterator_traits<iter>::iterator_category>;
public:
	using _Base::_Base;
	using _Base::operator=;

};

template<typename iter>
view(iter, iter)->view<iter>;

template<typename _Container>
view(const _Container&)->view<decltype(std::begin(std::declval<const _Container&>()))>;

_WJR_END

#endif