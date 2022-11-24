#pragma once
#ifndef __WJR_VIEW_H
#define __WJR_VIEW_H

#include <functional>

#include <wjr/type_traits.h>

_WJR_BEGIN

template<typename iter, std::enable_if_t<wjr::is_bidir_iter_v<iter>, int> = 0>
class view;

template<typename = void, typename...Args>
struct __can_make_view : std::false_type {};

template<typename...Args>
struct __can_make_view<std::void_t<decltype(view(std::declval<Args>()...))>, Args...> : std::true_type {};

template<typename...Args>
struct can_make_view : __can_make_view<void, Args...> {};

template<typename...Args>
inline constexpr bool can_make_view_v = can_make_view<Args...>::value;

class _Search_view_iterator_end {
};

template<typename iter, typename Searcher>
class _Search_view_iterator {
	using _Iter_traits = std::iterator_traits<iter>;
public:
	using value_type = typename _Iter_traits::value_type;
	using difference_type = typename _Iter_traits::difference_type;
	using size_type = std::conditional_t<std::is_integral_v<difference_type>,
		std::make_unsigned_t<difference_type>, difference_type>;
	constexpr _Search_view_iterator(view<iter> v, const Searcher& searcher) 
		: _M_view(v), _M_next(std::begin(v)), _M_searcher(searcher) {
		_First_find_next();
	}
	constexpr _Search_view_iterator& operator++() {
		_Find_next();
		return *this;
	}
	constexpr iter operator*() const {
		return std::begin(_M_view);
	}
	constexpr bool operator!=(const _Search_view_iterator_end&) const {
		return !_M_view.empty();
	}
private:
	constexpr void _First_find_next() {
		auto _Pair = _M_searcher(std::begin(_M_view), std::end(_M_view));
		_M_view = _M_view.subview(_Pair.first);
		_M_next = _Pair.second;
	}
	constexpr void _Find_next() {
		_M_view = _M_view.subview(_M_next);
		_First_find_next();
	}
	view<iter> _M_view;
	iter _M_next;
	const Searcher& _M_searcher;
};

template<typename iter, typename Searcher>
class _Search_view {
public:
	using iterator_category = std::forward_iterator_tag;
	using iterator = _Search_view_iterator<iter, Searcher>;
	using const_iterator = iterator;
	constexpr _Search_view(view<iter> v, Searcher&& searcher) 
		:_M_view(v), _M_searcher(std::forward<Searcher>(searcher)) {}
	constexpr _Search_view_iterator<iter, Searcher> begin() const {
		return _Search_view_iterator<iter, Searcher>(_M_view, _M_searcher);
	}
	constexpr _Search_view_iterator_end end() const {
		return {};
	}
	
private:
	view<iter> _M_view;
	std::conditional_t<std::is_rvalue_reference_v<Searcher&&>,
		Searcher, Searcher&> _M_searcher;
};

template<typename iter, typename Searcher>
_Search_view(view<iter>, Searcher&&)->_Search_view<iter, Searcher>;

template<typename iter>
class _View_base {
	using _Iter_traits = std::iterator_traits<iter>;
public:
	using value_type = typename _Iter_traits::value_type;
	using reference = typename _Iter_traits::reference;
	using difference_type = typename _Iter_traits::difference_type;
	using size_type = std::conditional_t<std::is_integral_v<difference_type>, 
		std::make_unsigned_t<difference_type>, difference_type>;
	using iterator = iter;
	using const_iterator = iter;
	using reverse_iterator = std::reverse_iterator<iter>;
	using const_reverse_iterator = std::reverse_iterator<iter>;
	using iterator_category = typename _Iter_traits::iterator_category;
	constexpr static size_type npos = -1;

	constexpr _View_base(const _View_base&) = default;
	constexpr _View_base(_View_base&&) = default;
	constexpr _View_base& operator=(const _View_base&) = default;
	constexpr _View_base& operator=(_View_base&&) = default;

	constexpr _View_base(iter _First, iter _Last) 
		noexcept(std::is_nothrow_constructible_v<iter>) 
		: _Myfirst(_First), _Mylast(_Last) {}

	template<typename _Container, std::enable_if_t<
		std::conjunction_v<
		std::has_global_function_begin<const _Container&>,
		std::has_global_function_end<const _Container&>,
		std::is_constructible<iter, decltype(std::begin(std::declval<const _Container&>()))>,
		std::is_constructible<iter, decltype(std::end(std::declval<const _Container&>()))>
		>, int> = 0>
	constexpr explicit _View_base(const _Container& _Cont) 
		noexcept(std::is_nothrow_constructible_v<iter, decltype(std::begin(_Cont))>&& 
		std::is_nothrow_constructible_v<iter, decltype(std::end(_Cont))>)
		: _Myfirst(std::begin(_Cont)), _Mylast(std::end(_Cont)) {}

	constexpr iter begin() const noexcept(std::is_nothrow_copy_constructible_v<iter>) {
		return _Myfirst;
	}

	constexpr iter end() const noexcept(std::is_nothrow_copy_constructible_v<iter>) {
		return _Mylast;
	}

	constexpr reverse_iterator rbegin() const noexcept(std::is_nothrow_copy_constructible_v<iter>) {
		return reverse_iterator(_Mylast);
	}

	constexpr reverse_iterator rend() const noexcept(std::is_nothrow_copy_constructible_v<iter>) {
		return reverse_iterator(_Myfirst);
	}

	constexpr decltype(auto) reverse_view() const noexcept(std::is_nothrow_copy_constructible_v<iter>) {
		return view<reverse_iterator>(_Mylast, _Myfirst);
	}

	constexpr decltype(auto) front() const noexcept(std::is_nothrow_copy_constructible_v<reference>) {
		return *_Myfirst;
	}

	constexpr decltype(auto) back() const noexcept(std::is_nothrow_copy_constructible_v<reference>) {
		return *std::prev(_Mylast);
	}

	constexpr view<iter> subview(iter _First, iter _Last) const noexcept(std::is_nothrow_copy_constructible_v<iter>) {
		return view<iter>(_First, _Last);
	}

	constexpr view<iter> subview(iter _First) const noexcept(std::is_nothrow_copy_constructible_v<iter>) {
		return view<iter>(_First, _Mylast);
	}

	constexpr bool empty() const noexcept {
		return _Myfirst == _Mylast;
	}

	template<typename _Pred>
	constexpr bool starts_with(const value_type& _Val, _Pred&& pred) const noexcept {
		return !empty() && std::forward<_Pred>(pred)(front(), _Val);
	}

	constexpr bool starts_with(const value_type& _Val) const noexcept {
		return starts_with(_Val, std::equal_to<>{});
	}

	template<typename _Iter, typename _Pred>
	constexpr bool starts_with(view<_Iter> _View, _Pred&& pred) const noexcept {
		return std::equal(begin(), end(), std::begin(_View), std::end(_View), std::forward<_Pred>(pred));
	}

	template<typename _Iter>
	constexpr bool starts_with(view<_Iter> _View) const noexcept {
		return starts_with(_View, std::equal_to<>{});
	}

	template<typename _Pred>
	constexpr bool ends_with(const value_type& _Val, _Pred&& pred)const noexcept {
		return !empty() && std::forward<_Pred>(pred)(back(), _Val);
	}

	constexpr bool ends_with(const value_type& _Val) const noexcept {
		return ends_with(_Val, std::equal_to<>{});
	}

	template<typename _Iter, typename _Pred>
	constexpr bool ends_with(view<_Iter> _View, _Pred&& pred) const noexcept {
		return std::equal(rbegin(), rend(), std::rbegin(_View), std::rend(_View), std::forward<_Pred>(pred));
	}

	template<typename _Iter>
	constexpr bool ends_with(view<_Iter> _View) const noexcept {
		return ends_with(_View, std::equal_to<>{});
	}
	
	constexpr iter find(const value_type& _Val) const noexcept {
		return std::find(begin(), end(), _Val);
	}

	constexpr iter rfind(const value_type& _Val) const noexcept {
		return std::find(rbegin(), rend(), _Val);
	}

	template<typename _Pred>
	constexpr iter find_if(const value_type& _Val, _Pred&& pred) const noexcept {
		return std::find_if(begin(), end(), [&](const auto& _Elem) {
			return std::forward<_Pred>(pred)(_Elem, _Val);
			});
	}

	template<typename _Pred>
	constexpr iter rfind_if(const value_type& _Val, _Pred&& pred)const noexcept {
		return std::find_if(rbegin(), rend(), [&](const auto& _Elem) {
			return std::forward<_Pred>(pred)(_Elem, _Val);
			});
	}

	template<typename _Iter, typename _Pred>
	constexpr iter search(view<_Iter> _View, _Pred&& pred) const noexcept {
		return std::search(begin(), end(),
			std::begin(_View), std::end(_View), std::forward<_Pred>(pred));
	}

	template<typename _Iter>
	constexpr iter search(view<_Iter> _View) const noexcept {
		return search(_View, std::equal_to<>{});
	}

	template<typename _Searcher, std::enable_if_t<
		!can_make_view_v<_Searcher&&>, int> = 0>
	constexpr iter search(_Searcher&& searcher) const noexcept {
		return std::search(begin(), end(), std::forward<_Searcher>(searcher));
	}

	template<typename _Iter, typename _Pred>
	constexpr decltype(auto) search_view(view<_Iter> _View, _Pred&& pred) const noexcept {
		return _Search_view(view<iter>(begin(), end()), 
			std::default_searcher(std::begin(_View), std::end(_View), std::forward<_Pred>(pred)));
	}

	template<typename _Iter>
	constexpr decltype(auto) search_view(view<_Iter> _View) const noexcept {
		return search_view(_View, std::equal_to<>{});
	}

	constexpr bool contains(const value_type& _Val) const noexcept {
		return find(_Val) != end();
	}

	template<typename _Pred>
	constexpr bool contains(const value_type& _Val, _Pred&& pred) const noexcept {
		return find_if(_Val, std::forward<_Pred>(pred)) != end();
	}

	template<typename _Iter>
	constexpr bool contains(view<_Iter> _View) const noexcept {
		return search(_View) != end();
	}

	template<typename _Iter, typename _Pred>
	constexpr bool contains(view<_Iter> _View, _Pred&& pred) const noexcept {
		return search(_View, std::forward<_Pred>(pred)) != end();
	}

	template<typename _Searcher>
	constexpr bool contains(_Searcher&& searcher) const noexcept {
		return search(std::forward<_Searcher>(searcher)) != end();
	}

private:
	iter _Myfirst;
	iter _Mylast;
};

// random_access_iterator
template<typename iter>
class _View_random_base : public _View_base<iter> {
	using _Base = _View_base<iter>;
public:
	using value_type = typename _Base::value_type;
	using reference = typename _Base::reference;
	using difference_type = typename _Base::difference_type;
	using size_type = typename _Base::size_type;
	using iterator = typename _Base::iterator;
	using const_iterator = typename _Base::const_iterator;
	using reverse_iterator = typename _Base::reverse_iterator;
	using const_reverse_iterator = typename _Base::const_reverse_iterator;

	using _Base::_Base;
	using _Base::subview;

	constexpr view<iter> subview(size_type _Off, size_type _Count) const {
		return view(_Base::begin() + _Off, _Base::begin() + _Off + _Count);
	}

	constexpr view<iter> subview(size_type _Off) const {
		return view(_Base::begin() + _Off, _Base::end());
	}

	constexpr size_type size() const noexcept {
		return static_cast<size_type>(_Base::end() - _Base::begin());
	}

	constexpr decltype(auto) operator[](size_type _Pos) const noexcept(std::is_nothrow_copy_constructible_v<reference>) {
		return *(_Base::begin() + _Pos);
	}

	constexpr decltype(auto) at(size_type _Pos) const {
		if (_Pos >= _Base::size()) {
			throw std::out_of_range("out of range");
		}
		return *(_Base::begin() + _Pos);
	}

	template<typename _Iter ,typename _Pred>
	constexpr bool ends_with(view<_Iter> _View, _Pred&& pred) const noexcept {
		if constexpr (wjr::is_random_iter_v<_Iter>) {
			size_t n = std::size(_View);
			size_t m = std::size(*this);
			if (n > m) {
				return false;
			}
			return std::equal(_Base::begin() + m - n, _Base::end(),
				std::begin(_View), std::end(_View), std::forward<_Pred>(pred));
		}
		else {
			return _Base::ends_with(_View, std::forward<_Pred>(pred));
		}
	}

	template<typename _Iter>
	constexpr bool ends_with(view<_Iter> _View) const noexcept {
		return ends_with(_View, std::equal_to<>{});
	}

};

// bidirectional_iterator
template<typename iter>
class _View_bidirectional_base : public _View_base<iter> {
	using _Base = _View_base<iter>;
public:
	using _Base::_Base;
};

template<typename iter>
using _Select_view_base = std::conditional_t<
	wjr::is_random_iter_v<iter>,
	_View_random_base<iter>,
	_View_bidirectional_base<iter>
>;

// only support random_access_iterator now
template<typename iter, std::enable_if_t<wjr::is_bidir_iter_v<iter>, int>>
class view : public _Select_view_base<iter> {
	using _Base = _Select_view_base<iter>;
public:
	using _Base::_Base;

};

template<typename iter>
view(iter, iter)->view<iter>;

template<typename _Container>
view(const _Container&)->view<decltype(std::begin(std::declval<const _Container&>()))>;

_WJR_END

#endif