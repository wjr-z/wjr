#pragma once
#ifndef __WJR_ALGORITHM_H__
#define __WJR_ALGORITHM_H__

#include <algorithm>
#include <memory>
#include <limits>
#include <cstring>

#include <wjr/algo/algo.h>

_WJR_BEGIN

template<typename _Iter, typename _Val, typename _Pred,
	typename _Iter_value = iter_val_t<_Iter>>
struct __has_fast_find : std::conjunction<
	is_contiguous_iterator<_Iter>,
	std::conditional_t<
	wjr::is_reverse_iterator_v<_Iter>,
	algo::__has_fast_memrchr<_Iter_value, _Val, _Pred>,
	algo::__has_fast_memchr<_Iter_value, _Val, _Pred>
	>
>{};

template<typename _Iter, typename _Val, typename _Pred>
constexpr bool __has_fast_find_v = __has_fast_find<_Iter, _Val, _Pred>::value;

struct find_fn {

	template<typename _Iter, typename _Ty, typename _Pred = std::equal_to<>>
	WJR_CONSTEXPR20 _Iter operator()(
		_Iter _First, _Iter _Last, const _Ty& _Val, _Pred pred = _Pred()) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_find_v<_Iter, _Ty, _Pred>) {
				const auto n = std::distance(_First, _Last);
				if constexpr (!wjr::is_reverse_iterator_v<_Iter>) {
					const auto first = wjr::get_address(_First);
					return _First + (algo::memchr(first, _Val, n, pred) - first);
				}
#if defined(_WJR_FAST_MEMCHR) // use algo::memchr
				else {
					const auto first = wjr::get_address(_Last - 1);
					return _Last - (algo::memrchr(first, _Val, n, pred) - first);
				}
#endif // _WJR_FAST_MEMCHR
			}
		}
		if constexpr (std::is_same_v<_Pred, std::equal_to<>>) {
			return std::find(_First, _Last, _Val);
		}
		else {
			for (; _First != _Last; ++_First) {
				if (pred(*_First, _Val)) {
					return _First;
				}
			}
			return _Last;
		}
	}

};

constexpr find_fn find{};

struct find_if_fn {
	template<typename _Iter, typename _Pr>
	WJR_CONSTEXPR20 _Iter operator()(_Iter _First, _Iter _Last, _Pr _Pred) const {
		return std::find_if(_First, _Last, _Pred);
	}
};

constexpr find_if_fn find_if{};

struct find_if_not_fn {
	template<typename _Iter, typename _Pr>
	WJR_CONSTEXPR20 _Iter operator()(_Iter _First, _Iter _Last, _Pr _Pred) const {
		return std::find_if_not(_First, _Last, _Pred);
	}
};

constexpr find_if_not_fn find_if_not{};

template<typename _Iter, typename _Val,
	typename _Iter_value = iter_val_t<_Iter>>
struct __has_fast_count : std::conjunction<
	wjr::is_contiguous_iterator<_Iter>,
	algo::__has_fast_memcnt<_Iter_value, _Val>
>{};

template<typename _Iter, typename _Val>
constexpr bool __has_fast_count_v = __has_fast_count<_Iter, _Val>::value;

struct count_fn {

	template<typename _Iter, typename _Ty>
	WJR_CONSTEXPR20 typename std::iterator_traits<_Iter>::difference_type
		operator()(_Iter _First, _Iter _Last, const _Ty& _Val) const {
		if (!wjr::is_constant_evaluated()) {
#if defined(_WJR_FAST_MEMCNT)
			if constexpr (__has_fast_count_v<_Iter, _Ty>) {
				const auto n = _Last - _First;
				if constexpr (!wjr::is_reverse_iterator_v<_Iter>) {
					const auto first = wjr::get_address(_First);
					return algo::memcnt(first, _Val, n);
				}
				else {
					const auto first = wjr::get_address(_Last - 1);
					return algo::memcnt(first, _Val, n);
				}
			}
#endif // _WJR_FAST_MEMCNT
		}
		return std::count(_First, _Last, _Val);
	}
};

constexpr count_fn count{};

struct count_if_fn {

	template<typename _Iter, typename _Pr>
	WJR_CONSTEXPR20 typename std::iterator_traits<_Iter>::difference_type
		operator()(_Iter _First, _Iter _Last, _Pr _Pred) const {
		return std::count_if(_First, _Last, _Pred);
	}
};

constexpr count_if_fn count_if{};

// First use algo::memcmp
// Then use memcmp
template<typename _Iter1, typename _Iter2, typename _Pred, 
	typename _Iter_value1 = iter_val_t<_Iter1>,
	typename _Iter_value2 = iter_val_t<_Iter2>>
struct __has_fast_equal_helper : std::conjunction<
	wjr::is_contiguous_iterator<_Iter1>,
	wjr::is_contiguous_iterator<_Iter2>,
	std::bool_constant<wjr::is_reverse_iterator_v<_Iter1> == wjr::is_reverse_iterator_v<_Iter2>>,
	algo::__has_fast_memcmp<_Iter_value1, _Iter_value2, _Pred>
>{};

template<typename _Iter1, typename _Iter2, typename _Pred>
struct __has_fast_equal : std::bool_constant<__has_fast_equal_helper<_Iter1, _Iter2, _Pred>::value> {};

template<typename _Iter1, typename _Iter2, typename _Pred>
constexpr bool __has_fast_equal_v = __has_fast_equal<_Iter1, _Iter2, _Pred>::value;

template<typename _Iter1, typename _Iter2, typename _Pred,
	typename _Iter_value1 = iter_val_t<_Iter1>,
	typename _Iter_value2 = iter_val_t<_Iter2>>
struct __has_fast_mismatch : std::conjunction <
	wjr::is_contiguous_iterator<_Iter1>,
	wjr::is_contiguous_iterator<_Iter2>,
	std::bool_constant<wjr::is_reverse_iterator_v<_Iter1> == wjr::is_reverse_iterator_v<_Iter2>>,
	std::conditional_t<
	wjr::is_reverse_iterator_v<_Iter1>,
	algo::__has_fast_memrmis<_Iter_value1, _Iter_value2, _Pred>,
	algo::__has_fast_memmis<_Iter_value1, _Iter_value2, _Pred>
	>
>{};

template<typename _Iter1, typename _Iter2, typename _Pred>
constexpr bool __has_fast_mismatch_v = __has_fast_mismatch<_Iter1, _Iter2, _Pred>::value;

struct mismatch_fn {

	template<typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) const {
		return this->operator()(_First1, _Last1, _First2, std::equal_to<>{});
	}

	template<typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) const {
		return this->operator()(_First1, _Last1, _First2, _Last2, std::equal_to<>{});
	}

	template<typename _Iter1, typename _Iter2, typename _Pred>
	WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred) const {
		if(!wjr::is_constant_evaluated()){
#if defined(_WJR_FAST_MEMMIS)
			if constexpr (__has_fast_mismatch_v<_Iter1, _Iter2, _Pred>) {
				const auto n = std::distance(_First1, _Last1);
				if (is_unlikely(n == 0)) { return std::make_pair(_First1, _First2); }
				if constexpr (!wjr::is_reverse_iterator_v<_Iter1>) {
					const auto first1 = wjr::get_address(_First1);
					const auto first2 = wjr::get_address(_First2);

					auto pos = algo::memmis(first1, first2, n, pred) - first1;
					return std::make_pair(_First1 + pos, _First2 + pos);
				}
				else {
					const auto first1 = wjr::get_address(_Last1 - 1);
					const auto _Last2 = _First2 + n;
					const auto first2 = wjr::get_address(_Last2 - 1);

					auto pos = (first1 + n) - algo::memrmis(first1, first2, n, pred);
					return std::make_pair(_First1 + pos, _First2 + pos);
				}
			}
#endif // _WJR_FAST_MEMMIS
		}
		return std::mismatch(_First1, _Last1, _First2, pred);
	}

	template<typename _Iter1, typename _Iter2, typename _Pred>
	WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) const {
		if(!wjr::is_constant_evaluated()){
			if constexpr (__has_fast_mismatch_v<_Iter1, _Iter2, _Pred>) {
				const auto n = _Last1 - _First1;
				const auto m = _Last2 - _First2;
				const auto _M = n < m ? n : m;
				return this->operator()(_First1, _First1 + _M, _First2, pred);
			}
		}
		return std::mismatch(_First1, _Last1, _First2, _Last2, pred);
	}

};

constexpr mismatch_fn mismatch;

struct equal_fn {

	template<typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR20 bool operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) const {
		return this->operator()(_First1, _Last1, _First2, std::equal_to<>{});
	}

	template<typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR20 bool operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) const {
		return this->operator()(_First1, _Last1, _First2, _Last2, std::equal_to<>{});
	}

	template<typename _Iter1, typename _Iter2, typename _Pred>
	WJR_CONSTEXPR20 bool operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred) const {
		if(!wjr::is_constant_evaluated()){
			if constexpr (__has_fast_equal_v<_Iter1, _Iter2, _Pred>) {
				const auto n = _Last1 - _First1;
				if (is_unlikely(n == 0)) { return true; }
				if constexpr (!wjr::is_reverse_iterator_v<_Iter1>) {
					const auto first1 = wjr::get_address(_First1);
					const auto first2 = wjr::get_address(_First2);
					return algo::memcmp(first1, first2, n, pred);
				}
				else {
					const auto first1 = wjr::get_address(_Last1 - 1);
					const auto _Last2 = _First2 + n;
					const auto first2 = wjr::get_address(_Last2 - 1);
					return algo::memcmp(first1, first2, n, pred);
				}
			}
		}
		return std::equal(_First1, _Last1, _First2, pred);
	}

	template<typename _Iter1, typename _Iter2, typename _Pred>
	WJR_CONSTEXPR20 bool operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) const {
		if(!wjr::is_constant_evaluated()){
			if constexpr (__has_fast_equal_v<_Iter1, _Iter2, _Pred>) {
				const auto n = std::distance(_First1, _Last1);
				const auto m = std::distance(_First2, _Last2);
				if (n != m) { return false; }
				return this->operator()(_First1, _Last1, _First2, pred);
			}
		}
		return std::equal(_First1, _Last1, _First2, _Last2, pred);
	}

};

constexpr equal_fn equal{};

template<typename _Iter1, typename _Iter2, typename _Pred,
	typename _Iter_value1 = iter_val_t<_Iter1>,
	typename _Iter_value2 = iter_val_t<_Iter2>>
struct __has_fast_lexicographical_compare : std::conjunction<
	wjr::is_any_of<_Pred, std::less<>, std::not_equal_to<>>,
	__has_fast_mismatch<_Iter1, _Iter2, _Pred>
>{};

template<typename _Iter1, typename _Iter2, typename _Pred>
constexpr bool __has_fast_lexicographical_compare_v = __has_fast_lexicographical_compare<_Iter1, _Iter2, _Pred>::value;

struct lexicographical_compare_fn {
	template<typename _ExPolicy, typename _Iter1, typename _Iter2>
	bool operator()(_ExPolicy&& _Policy, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) const {
		return this->operator()(_Policy, _First1, _Last1, _First2, _Last2, std::less<>{});
	}

	template<typename _ExPolicy, typename _Iter1, typename _Iter2, typename _Pred>
	bool operator()(_ExPolicy&& _Policy, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) const {
		return std::lexicographical_compare(_Policy, _First1, _Last1, _First2, _Last2, pred);
	}

	template<typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR20 bool operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) const {
		return this->operator()(_First1, _Last1, _First2, _Last2, std::less<>{});
	}

	template<typename _Iter1, typename _Iter2, typename _Pred>
	WJR_CONSTEXPR20 bool operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_lexicographical_compare_v<_Iter1, _Iter2, _Pred>) {
				const auto n = std::distance(_First1, _Last1);
				const auto m = std::distance(_First2, _Last2);
				auto cl = std::min(n, m);
				auto e = _First1 + cl;
				auto pos = wjr::mismatch(_First1, e, _First2, std::equal_to<>{}).first - _First1;
				if (pos != cl) {
					if (pred(_First1 + pos, _First2 + pos)) {
						return true;
					}
					return false;
				}
				return n < m;
			}
		}
		return std::lexicographical_compare(_First1, _Last1, _First2, _Last2, pred);
	}
};

constexpr lexicographical_compare_fn lexicographical_compare{};

template<typename _Iter, typename _Val,
	typename _Iter_ref = iter_ref_t<_Iter>>
struct __has_fast_fill : std::conjunction<
	is_contiguous_iterator<_Iter>, 
	algo::__has_fast_assign_memset<remove_cref_t<_Iter_ref>, add_cref_t<_Val>>
>{};

template<typename _Iter, typename _Val>
constexpr bool __has_fast_fill_v = __has_fast_fill<_Iter, _Val>::value;

struct fill_fn {
	template<typename _Iter, typename _Val>
	WJR_CONSTEXPR20 void operator()(_Iter _First, _Iter _Last, const _Val& value) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_fill_v<_Iter, _Val>) {
				const auto n = std::distance(_First, _Last);
				if constexpr (!is_reverse_iterator_v<_Iter>) {
					const auto first = wjr::get_address(_First);
					algo::assign_memset(first, value, n);
				}
				else {
					if (is_unlikely(n == 0)) { return; }
					const auto first = wjr::get_address(_Last - 1);
					algo::assign_memset(first, value, n);
				}
				return;
			}
		}
		return std::fill(_First, _Last, value);
	}

};

constexpr fill_fn fill{};

struct fill_n_fn {
	template<typename _Iter, typename _Size, typename _Val>
	WJR_CONSTEXPR20 _Iter operator()(_Iter _First, _Size count, const _Val& value) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_fill_v<_Iter, _Val>) {
				if (count <= 0) { return _First; }
				wjr::fill(_First, _First + count, value);
				return _First + count;
			}
		}
		return std::fill_n(_First, count, value);
	}

};

constexpr fill_n_fn fill_n{};

template<typename _Input, typename _Output,
	typename _Input_ref = iter_ref_t<_Input>,
	typename _Output_ref = iter_ref_t<_Output>>
struct __has_fast_copy : std::conjunction<
	is_contiguous_iterator<_Input>,
	is_contiguous_iterator<_Output>,
	std::bool_constant<wjr::is_reverse_iterator_v<_Input> == wjr::is_reverse_iterator_v<_Output>>,
	algo::__has_fast_assign_memcpy<remove_ref_t<_Output_ref>, _Input_ref>
	> {};

template<typename _Input, typename _Output>
constexpr bool __has_fast_copy_v = __has_fast_copy<_Input, _Output>::value;

struct copy_fn {
	template<typename _Input, typename _Output>
	WJR_CONSTEXPR20 _Output operator()(_Input _First1, _Input _Last1, _Output _First2) const {
		static_assert(!std::is_const_v<iter_ref_t<_Output>>, "Output iterator must not be const");
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_copy_v<_Input, _Output>) {
				const auto n = std::distance(_First1, _Last1);
				if (is_unlikely(n == 0)) { return _First2; }
				if constexpr (!wjr::is_reverse_iterator_v<_Input>) {
					const auto first1 = wjr::get_address(_First1);
					const auto first2 = wjr::get_address(_First2);

					algo::assign_memmove(first2, first1, n);
				}
				else {
					const auto first1 = wjr::get_address(_Last1 - 1);
					const auto _Last2 = _First2 + n;
					const auto first2 = wjr::get_address(_Last2 - 1);

					algo::assign_memmove(first2, first1, n);
				}
				return _First2 + n;
			}
		}
		return std::copy(_First1, _Last1, _First2);
	}
};

constexpr copy_fn copy{};

struct copy_n_fn {
	template<typename _Input, typename _Size, typename _Output>
	WJR_CONSTEXPR20 _Output operator()(_Input _First1, _Size count, _Output _First2) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_copy_v<_Input, _Output>) {
				if (count <= 0) { return _First2; }
				return wjr::copy(_First1, _First1 + count, _First2);
			}
		}
		return std::copy_n(_First1, count, _First2);
	}
};

constexpr copy_n_fn copy_n{};

struct copy_backward_fn {
	template<typename _Input, typename _Output>
	WJR_CONSTEXPR20 _Output operator()(_Input _First1, _Input _Last1, _Output _Last2) const {
		return wjr::copy(std::make_reverse_iterator(_Last1),
			std::make_reverse_iterator(_First1),
			std::make_reverse_iterator(_Last2)).base();
	}
};

constexpr copy_backward_fn copy_backward{};

struct move_fn {
	template<typename _Input, typename _Output>
	WJR_CONSTEXPR20 _Output operator()(_Input _First1, _Input _Last1, _Output _First2) const {
		return wjr::copy(std::make_move_iterator(_First1), std::make_move_iterator(_Last1), _First2);
	}
};

constexpr move_fn move{};

struct move_backward_fn {
	template<typename _Input, typename _Output>
	WJR_CONSTEXPR20 _Output operator()(_Input _First1, _Input _Last1, _Output _Last2) const {
		return wjr::copy_backward(std::make_move_iterator(_First1), std::make_move_iterator(_Last1), _Last2);
	}
};

constexpr move_backward_fn move_backward{};

struct construct_at_fn {
	template<typename _Iter, typename...Args, std::enable_if_t<is_iterator_v<_Iter>, int> = 0>
	WJR_CONSTEXPR20 void operator()(_Iter iter, Args&&... args) const {
		using value_type = iter_val_t<_Iter>;
		::new (voidify(get_address(iter))) value_type(std::forward<Args>(args)...);
	}

	template<typename _Iter, std::enable_if_t<is_iterator_v<_Iter>, int> = 0>
	WJR_CONSTEXPR20 void operator()(_Iter iter, default_construct_tag) const {
		using value_type = iter_val_t<_Iter>;
		::new (voidify(get_address(iter))) value_type;
	}

	template<typename _Iter, std::enable_if_t<is_iterator_v<_Iter>, int> = 0>
	WJR_CONSTEXPR20 void operator()(_Iter iter, value_construct_tag) const {
		this->operator()(iter);
	}

	template<typename Alloc, typename _Iter, typename...Args, std::enable_if_t<!is_iterator_v<Alloc>, int> = 0>
	WJR_CONSTEXPR20 void operator()(Alloc& al, _Iter iter, Args&&...args) const {
		using pointer = iter_address_t<_Iter>;
		if constexpr (is_default_allocator_construct_v<Alloc, pointer, Args...>) {
			this->operator()(iter, std::forward<Args>(args)...);
		}
		else {
			std::allocator_traits<Alloc>::construct(al, get_address(iter), std::forward<Args>(args)...);
		}
	}

};

constexpr construct_at_fn construct_at;

struct destroy_at_fn {
	template<typename _Iter>
	WJR_CONSTEXPR20 void operator()(_Iter ptr) const {
		using value_type = iter_val_t<_Iter>;
		if constexpr (!std::is_trivially_destructible_v<value_type>) {
			get_address(ptr)->~value_type();
		}
	}

	template<typename Alloc, typename _Iter>
	WJR_CONSTEXPR20 void operator()(Alloc& al, _Iter iter) const {
		if constexpr (is_default_allocator_destroy_v<Alloc, _Iter>) {
			this->operator()(iter);
		}
		else {
			std::allocator_traits<Alloc>::destroy(al, get_address(iter));
		}
	}
	
};

constexpr destroy_at_fn destroy_at;

struct destroy_fn {
	template<typename _Iter>
	WJR_CONSTEXPR20 void operator()(_Iter _First, _Iter _Last) const {
		using value_type = iter_val_t<_Iter>;
		if constexpr (!std::is_trivially_destructible_v<value_type>) {
			for (; _First != _Last; ++_First) {
				wjr::destroy_at(_First);
			}
		}
	}

	template<typename Alloc, typename _Iter>
	WJR_CONSTEXPR20 void operator()(Alloc& al, _Iter _First, _Iter _Last) const {
		using value_type = iter_val_t<_Iter>;
		if constexpr (!(is_default_allocator_destroy_v<Alloc, _Iter> 
			&& std::is_trivially_destructible_v<value_type>)) {
			for (; _First != _Last; ++_First) {
				wjr::destroy_at(al, _First);
			}
		}
	}
};

constexpr destroy_fn destroy;

struct destroy_n_fn {
	template<typename _Iter, typename _Diff>
	WJR_CONSTEXPR20 void operator()(_Iter _First, const _Diff n) const {
		using value_type = iter_val_t<_Iter>;
		if constexpr (!std::is_trivially_destructible_v<value_type>) {
			for (; n > 0; (void)++_First, --n) {
				wjr::destroy_at(_First);
			}
		}
	}

	template<typename Alloc, typename _Iter, typename _Diff>
	WJR_CONSTEXPR20 void operator()(Alloc& al, _Iter _First, _Diff n) const {
		using value_type = iter_val_t<_Iter>;
		if constexpr (!(is_default_allocator_destroy_v<Alloc, _Iter> && std::is_trivially_destructible_v<value_type>)) {
			for (; n > 0; (void)++_First, --n) {
				wjr::destroy_at(al, _First);
			}
		}
	}
};

constexpr destroy_n_fn destroy_n;

struct uninitialized_default_construct_fn {

	template<typename _Iter>
	WJR_CONSTEXPR20 void operator()(_Iter _First, _Iter _Last) const {
		std::uninitialized_default_construct(_First, _Last);
	}

	template<typename Alloc, typename _Iter>
	WJR_CONSTEXPR20 void operator()(
		Alloc& al, _Iter _First, _Iter _Last) const {
		using value_type = iter_val_t<_Iter>;
		if constexpr (is_default_allocator_construct_v<Alloc, _Iter, default_construct_tag>) {
			this->operator()(_First, _Last);
		}
		else {
			for (; _First != _Last; (void)++_First) {
				wjr::construct_at(al, _First, default_construct_tag{});
			}
		}
	}
};

constexpr uninitialized_default_construct_fn uninitialized_default_construct;

struct uninitialized_default_construct_n_fn {
	template<typename _Iter, typename _Diff>
	WJR_CONSTEXPR20 _Iter operator()(_Iter _First, const _Diff n) const {
		return std::uninitialized_default_construct_n(_First, n);
	}

	template<typename Alloc, typename _Iter, typename _Diff>
	WJR_CONSTEXPR20 _Iter operator()(
		Alloc& al, _Iter _First, _Diff n) const {
		if constexpr (is_default_allocator_construct_v<Alloc, _Iter, default_construct_tag>) {
			return this->operator()(_First, n);
		}
		else {
			for (; n > 0; (void)++_First, --n) {
				wjr::construct_at(al, _First, default_construct_tag{});
			}
			return _First;
		}
	}
};

constexpr uninitialized_default_construct_n_fn uninitialized_default_construct_n;

struct uninitialized_value_construct_fn {
	template<typename _Iter>
	WJR_CONSTEXPR20 void operator()(_Iter _First, _Iter _Last) const {
		std::uninitialized_value_construct(_First, _Last);
	}

	template<typename Alloc, typename _Iter>
	WJR_CONSTEXPR20 void operator()(
		Alloc& al, _Iter _First, _Iter _Last) const {
		if constexpr (is_default_allocator_construct_v<Alloc, _Iter, value_construct_tag>) {
			this->operator()(_First, _Last);
		}
		else {
			for (; _First != _Last; (void)++_First) {
				wjr::construct_at(al, _First, value_construct_tag{});
			}
		}
	}
};

constexpr uninitialized_value_construct_fn uninitialized_value_construct;

struct uninitialized_value_construct_n_fn {
	template<typename _Iter, typename _Diff>
	WJR_CONSTEXPR20 _Iter operator()(_Iter _First, _Diff n) const {
		return std::uninitialized_value_construct_n(_First, n);
	}

	template<typename Alloc, typename _Iter, typename _Diff>
	WJR_CONSTEXPR20 _Iter operator()(
		Alloc& al, _Iter _First, _Diff n) const {
		if constexpr (is_default_allocator_construct_v<Alloc, _Iter, value_construct_tag>) {
			return this->operator()(_First, n);
		}
		else {
			for (; n > 0; (void)++_First, --n) {
				wjr::construct_at(al, _First, value_construct_tag{});
			}
			return _First;
		}
	}
};

constexpr uninitialized_value_construct_n_fn uninitialized_value_construct_n;

template<typename _Input, typename _Output,
	typename _Input_ref = iter_ref_t<_Input>,
	typename _Output_ref = iter_ref_t<_Output>>
struct __has_fast_uninitialized_copy : std::conjunction<
	is_contiguous_iterator<_Input>,
	is_contiguous_iterator<_Output>,
	std::bool_constant<wjr::is_reverse_iterator_v<_Input> == wjr::is_reverse_iterator_v<_Output>>,
	algo::__has_fast_construct_memcpy<remove_ref_t<_Output_ref>, _Input_ref>
	> {};

template<typename _Input, typename _Output>
constexpr bool __has_fast_uninitialized_copy_v = __has_fast_uninitialized_copy<_Input, _Output>::value;

struct uninitialized_copy_fn {
	template<typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR20 _Iter2 operator()(_Iter1 _First, _Iter1 _Last, _Iter2 _Dest) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_uninitialized_copy_v<_Iter1, _Iter2>) {
				const auto n = std::distance(_First, _Last);
				if (is_unlikely(n == 0)) { return _Dest; }
				if constexpr (!wjr::is_reverse_iterator_v<_Iter1>) {
					const auto first1 = wjr::get_address(_First);
					const auto first2 = wjr::get_address(_Dest);

					algo::construct_memmove(first2, first1, n);
				}
				else {
					const auto first1 = wjr::get_address(_Last - 1);
					const auto _Last2 = _Dest + n;
					const auto first2 = wjr::get_address(_Last2 - 1);

					algo::construct_memmove(first2, first1, n);
				}
				return _Dest + n;
			}
		}
		return std::uninitialized_copy(_First, _Last, _Dest);
	}

	template<typename Alloc, typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR20 _Iter2 operator()(
		Alloc& al, _Iter1 _First, _Iter1 _Last, _Iter2 _Dest) const {
		if constexpr (is_default_allocator_construct_v<Alloc, _Iter2, decltype(*_First)>) {
			return this->operator()(_First, _Last, _Dest);
		}
		else {
			for (; _First != _Last; ++_Dest, (void)++_First) {
				wjr::construct_at(al, _Dest, *_First);
			}
		}
	}
};

constexpr uninitialized_copy_fn uninitialized_copy;

struct uninitialized_copy_n_fn {
	template<typename _Iter1, typename _Diff, typename _Iter2>
	WJR_CONSTEXPR20 _Iter2 operator()(_Iter1 _First, _Diff n, _Iter2 _Dest) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_uninitialized_copy_v<_Iter1, _Iter2>) {
				if (n <= 0) { return _Dest; }
				return wjr::uninitialized_copy(_First, _First + n, _Dest);
			}
		}
		return std::uninitialized_copy_n(_First, n, _Dest);
	}

	template<typename Alloc, typename _Iter1, typename _Diff, typename _Iter2>
	WJR_CONSTEXPR20 _Iter2 operator()(Alloc& al, _Iter1 _First, _Diff n, _Iter2 _Dest) const {
		if constexpr (is_default_allocator_construct_v<Alloc, _Iter2, decltype(*_First)>) {
			return this->operator()(_First, n, _Dest);
		}
		else {
			for (; n > 0; ++_First, (void)++_Dest, --n) {
				wjr::construct_at(al, _Dest, *_First);
			}
			return _Dest;
		}
	}
};

constexpr uninitialized_copy_n_fn uninitialized_copy_n;

template<typename _Iter, typename _Val,
	typename _Iter_ref = iter_ref_t<_Iter>>
	struct __has_fast_uninitialized_fill : std::conjunction<
	is_contiguous_iterator<_Iter>,
	algo::__has_fast_construct_memset<remove_cref_t<_Iter_ref>, add_cref_t<_Val>>
	> {};

template<typename _Iter, typename _Val>
constexpr bool __has_fast_uninitialized_fill_v = __has_fast_uninitialized_fill<_Iter, _Val>::value;

struct uninitialized_fill_fn {
	template<typename _Iter, typename _Val>
	WJR_CONSTEXPR20 void operator()(_Iter _First, _Iter _Last, const _Val& val) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_uninitialized_fill_v<_Iter, _Val>) {
				const auto n = std::distance(_First, _Last);
				if constexpr (!is_reverse_iterator_v<_Iter>) {
					const auto first = wjr::get_address(_First);
					algo::construct_memset(first, val, n);
				}
				else {
					if (is_unlikely(n == 0)) { return; }
					const auto first = wjr::get_address(_Last - 1);
					algo::construct_memset(first, val, n);
				}
				return;
			}
		}
		return std::uninitialized_fill(_First, _Last, val);
	}

	template<typename _Iter>
	WJR_CONSTEXPR20 void operator()(_Iter _First, _Iter _Last, default_construct_tag) const {
		wjr::uninitialized_default_construct(_First, _Last);
	}

	template<typename _Iter>
	WJR_CONSTEXPR20 void operator()(_Iter _First, _Iter _Last, value_construct_tag) const {
		wjr::uninitialized_value_construct(_First, _Last);
	}

	template<typename Alloc, typename _Iter, typename _Val>
	WJR_CONSTEXPR20 void operator()(Alloc& al, _Iter _First, _Iter _Last, const _Val& val) const {
		if constexpr (is_default_allocator_construct_v<Alloc, _Iter, _Val>) {
			this->operator()(_First, _Last, val);
		}
		else {
			for (; _First != _Last; (void)++_First) {
				wjr::construct_at(al, _First, val);
			}
		}
	}
};

constexpr uninitialized_fill_fn uninitialized_fill;

struct uninitialized_fill_n_fn {
	template<typename _Iter, typename _Diff, typename _Val>
	WJR_CONSTEXPR20 _Iter operator()(_Iter _First, _Diff count, const _Val& val) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_uninitialized_fill_v<_Iter, _Val>) {
				if (count <= 0) { return _First; }
				wjr::uninitialized_fill(_First, _First + count, val);
				return _First + count;
			}
		}
		return std::uninitialized_fill_n(_First, count, val);
	}

	template<typename _Iter, typename _Diff>
	WJR_CONSTEXPR20 _Iter operator()(_Iter _First, _Diff n, default_construct_tag) const {
		return wjr::uninitialized_default_construct_n(_First, n);
	}

	template<typename _Iter, typename _Diff>
	WJR_CONSTEXPR20 _Iter operator()(_Iter _First, _Diff n, value_construct_tag) const {
		return wjr::uninitialized_value_construct_n(_First, n);
	}

	template<typename Alloc, typename _Iter, typename _Diff, typename _Val>
	WJR_CONSTEXPR20 _Iter operator()(Alloc& al, _Iter _First, _Diff n, const _Val& val) const {
		if constexpr (is_default_allocator_construct_v<Alloc, _Iter, _Val>) {
			return this->operator()(_First, n, val);
		}
		else {
			for (; n > 0; (void)++_First, --n) {
				wjr::construct_at(al, _First, val);
			}
		}
	}
};

constexpr uninitialized_fill_n_fn uninitialized_fill_n;

struct uninitialized_move_fn {
	template<typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR20 _Iter2 operator()(_Iter1 _First, _Iter1 _Last, _Iter2 _Dest) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_uninitialized_copy_v<_Iter1, std::move_iterator<_Iter1>>) {
				const auto n = std::distance(_First, _Last);
				if (is_unlikely(n == 0)) { return _Dest; }
				if constexpr (!wjr::is_reverse_iterator_v<_Iter1>) {
					const auto first1 = wjr::get_address(_First);
					const auto first2 = wjr::get_address(_Dest);

					algo::construct_memmove(first2, first1, n);
				}
				else {
					const auto first1 = wjr::get_address(_Last - 1);
					const auto _Last2 = _Dest + n;
					const auto first2 = wjr::get_address(_Last2 - 1);

					algo::construct_memmove(first2, first1, n);
				}
				return _Dest + n;
			}
		}
		return std::uninitialized_move(_First, _Last, _Dest);
	}

	template<typename Alloc, typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR20 _Iter2 operator()(Alloc& al, _Iter1 _First, _Iter1 _Last, _Iter2 _Dest) const {
		if constexpr (is_default_allocator_construct_v<Alloc, _Iter1, decltype(std::move(*_First))>) {
			return this->operator()(_First, _Last, _Dest);
		}
		else {
			for (; _First != _Last; ++_Dest, (void)++_First) {
				wjr::construct_at(al, _Dest, std::move(*_First));
			}
			return _Dest;
		}
	}
};

constexpr uninitialized_move_fn uninitialized_move;

struct uninitialized_move_n_fn {
	template<typename _Iter1, typename _Diff, typename _Iter2>
	WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> operator()(_Iter1 _First, _Diff n, _Iter2 _Dest) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_uninitialized_copy_v<_Iter1, std::move_iterator<_Iter1>>) {
				if (n <= 0) return std::make_pair(_First, _Dest);
				wjr::uninitialized_move(_First, _First + n, _Dest);
				return std::make_pair(_First + n, _Dest + n);
			}
		}
		return std::uninitialized_move_n(_First, n, _Dest);
	}

	template<typename Alloc, typename _Iter1, typename _Diff, typename _Iter2>
	WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> operator()(
		Alloc& al, _Iter1 _First, _Diff n, _Iter2 _Dest) const {
		if constexpr(is_default_allocator_construct_v<Alloc, _Iter1, decltype(std::move(*_First))>){
			return this->operator()(_First, n, _Dest);
		}
		else {
			for (; n > 0; ++_First, (void)++_Dest, --n) {
				wjr::construct_at(al, _Dest, std::move(*_First));
			}
			return std::make_pair(_First, _Dest);
		}
	}
};

constexpr uninitialized_move_n_fn uninitialized_move_n;

template<typename Alloc>
class temporary_allocator_value {
public:
	using value_type = alloc_value_t<Alloc>;
	using traits = std::allocator_traits<Alloc>;

	template<typename...Args>
	constexpr explicit temporary_allocator_value(Alloc& al, Args&&...args) noexcept
		: al(al) {
		traits::construct(al, get_ptr(), std::forward<Args>(args)...);
	}

	temporary_allocator_value(const temporary_allocator_value&) = delete;
	temporary_allocator_value& operator=(const temporary_allocator_value&) = delete;

	~temporary_allocator_value() {
		wjr::destroy_at(al, get_ptr());
	}

	constexpr value_type* get_ptr() {
		return reinterpret_cast<value_type*>(std::addressof(vl));
	}

	constexpr const value_type* get_ptr() const {
		return reinterpret_cast<const value_type*>(std::addressof(vl));
	}

	constexpr value_type& value() {
		return *get_ptr();
	}

	constexpr const value_type& value() const {
		return *get_ptr();
	}

private:

	Alloc& al;
	std::aligned_storage_t<sizeof(value_type), alignof(value_type)> vl;
};

_WJR_END

#endif
