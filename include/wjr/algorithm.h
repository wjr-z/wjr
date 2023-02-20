#pragma once
#ifndef __WJR_ALGORITHM_H__
#define __WJR_ALGORITHM_H__

#include <algorithm>
#include <limits>
#include <cstring>

#include <wjr/algo/algo.h>

#if defined(WJR_HAS_EXECUTION)
#include <execution>
#endif // WJR_HAS_EXECUTION

_WJR_BEGIN

template<typename _Iter, typename _Val,
	typename _Iter_value = wjr::iter_val_t<_Iter>>
struct __has_fast_find : std::conjunction<
	is_contiguous_iterator<_Iter>,
	std::conditional_t<
	wjr::is_reverse_iterator_v<_Iter>,
	algo::__has_fast_memrchr<_Iter_value, _Val>,
	algo::__has_fast_memchr<_Iter_value, _Val>
	>
>{};

template<typename _Iter, typename _Val>
constexpr bool __has_fast_find_v = __has_fast_find<_Iter, _Val>::value;

struct _Find_fn {
	
#if defined(WJR_HAS_EXECUTION)
	template<typename ExecutionPolicy, typename ForwardIt, typename T, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>, int> = 0>
	ForwardIt operator()(ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, const T& value) const {
		return std::find(std::forward<ExecutionPolicy>(policy), first, last, value);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Iter, typename _Ty>
	WJR_CONSTEXPR20 _Iter operator()(
		_Iter _First, _Iter _Last, const _Ty& _Val) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_find_v<_Iter, _Ty>) {
				const auto n = std::distance(_First, _Last);
				if constexpr (!wjr::is_reverse_iterator_v<_Iter>) {
					const auto first = wjr::to_address(_First);
					return _First + (algo::memchr(first, _Val, n) - first);
				}
#if defined(__HAS_FAST_MEMCHR) // use algo::memchr
				else {
					const auto first = wjr::to_address(_Last - 1);
					return _Last - (algo::memrchr(first, _Val, n) - first);
				}
#endif // __HAS_FAST_MEMCHR
			}
		}
		return std::find(_First, _Last, _Val);
	}

};

constexpr _Find_fn find{};

struct _Find_if_fn {
	
#if defined(WJR_HAS_EXECUTION)
	template<typename ExecutionPolicy, typename ForwardIt, typename UnaryPredicate, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>, int> = 0>
	ForwardIt operator()(ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, UnaryPredicate p) const {
		return std::find_if(std::forward<ExecutionPolicy>(policy), first, last, p);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Iter, typename _Pr>
	WJR_CONSTEXPR20 _Iter operator()(_Iter _First, _Iter _Last, _Pr _Pred) const {
		return std::find_if(_First, _Last, _Pred);
	}
};

constexpr _Find_if_fn find_if{};

struct _Find_if_not_fn {
	
#if defined(WJR_HAS_EXECUTION)
	template<typename ExecutionPolicy, typename ForwardIt, typename UnaryPredicate, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>, int> = 0>
	ForwardIt operator()(ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, UnaryPredicate p) const {
		return std::find_if_not(std::forward<ExecutionPolicy>(policy), first, last, p);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Iter, typename _Pr>
	WJR_CONSTEXPR20 _Iter operator()(_Iter _First, _Iter _Last, _Pr _Pred) const {
		return std::find_if_not(_First, _Last, _Pred);
	}
};

constexpr _Find_if_not_fn find_if_not{};

template<typename _Iter, typename _Val,
	typename _Iter_value = wjr::iter_val_t<_Iter>>
struct __has_fast_count : std::conjunction<
	wjr::is_contiguous_iterator<_Iter>,
	algo::__has_fast_memcnt<_Iter_value, _Val>
>{};

template<typename _Iter, typename _Val>
constexpr bool __has_fast_count_v = __has_fast_count<_Iter, _Val>::value;

struct _Count_fn {

#if defined(WJR_HAS_EXECUTION)
	template<typename ExecutionPolicy, typename ForwardIt, typename T, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>, int> = 0>
	typename std::iterator_traits<ForwardIt>::difference_type 
		operator()(ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, const T& value) const {
		return std::count(std::forward<ExecutionPolicy>(policy), first, last, value);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Iter, typename _Ty>
	WJR_CONSTEXPR20 typename std::iterator_traits<_Iter>::difference_type
		operator()(_Iter _First, _Iter _Last, const _Ty& _Val) const {
		if (!wjr::is_constant_evaluated()) {
#if defined(__HAS_FAST_MEMCNT)
			if constexpr (__has_fast_count_v<_Iter, _Ty>) {
				const auto n = _Last - _First;
				if constexpr (!wjr::is_reverse_iterator_v<_Iter>) {
					const auto first = wjr::to_address(_First);
					return algo::memcnt(first, _Val, n);
				}
				else {
					const auto first = wjr::to_address(_Last - 1);
					return algo::memcnt(first, _Val, n);
				}
			}
#endif // __HAS_FAST_MEMCNT
		}
		return std::count(_First, _Last, _Val);
	}
};

constexpr _Count_fn count{};

struct _Count_if_fn {

#if defined(WJR_HAS_EXECUTION)
	template<typename ExecutionPolicy, typename ForwardIt, typename UnaryPredicate, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>, int> = 0>
	typename std::iterator_traits<ForwardIt>::difference_type
		operator()(ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, UnaryPredicate p) const {
		return std::count_if(std::forward<ExecutionPolicy>(policy), first, last, p);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Iter, typename _Pr>
	WJR_CONSTEXPR20 typename std::iterator_traits<_Iter>::difference_type
		operator()(_Iter _First, _Iter _Last, _Pr _Pred) const {
		return std::count_if(_First, _Last, _Pred);
	}
};

constexpr _Count_if_fn count_if{};

// First use algo::memcmp
// Then use memcmp
template<typename _Iter1, typename _Iter2, typename _Pred, 
	typename _Iter_value1 = wjr::iter_val_t<_Iter1>,
	typename _Iter_value2 = wjr::iter_val_t<_Iter2>>
struct __has_fast_equal_helper : std::conjunction<
	wjr::is_contiguous_iterator<_Iter1>,
	wjr::is_contiguous_iterator<_Iter2>,
	std::is_same<wjr::is_reverse_iterator<_Iter1>, wjr::is_reverse_iterator<_Iter2>>,
	algo::__has_fast_memcmp<_Iter_value1, _Iter_value2, _Pred>
>{};

template<typename _Iter1, typename _Iter2, typename _Pred>
struct __has_fast_equal : std::bool_constant<__has_fast_equal_helper<_Iter1, _Iter2, _Pred>::value> {};

template<typename _Iter1, typename _Iter2, typename _Pred>
constexpr bool __has_fast_equal_v = __has_fast_equal<_Iter1, _Iter2, _Pred>::value;

template<typename _Iter1, typename _Iter2, typename _Pred,
	typename _Iter_value1 = wjr::iter_val_t<_Iter1>,
	typename _Iter_value2 = wjr::iter_val_t<_Iter2>>
struct __has_fast_mismatch : std::conjunction <
	wjr::is_contiguous_iterator<_Iter1>,
	wjr::is_contiguous_iterator<_Iter2>,
	std::is_same<wjr::is_reverse_iterator<_Iter1>, wjr::is_reverse_iterator<_Iter2>>,
	std::conditional_t<
	wjr::is_reverse_iterator_v<_Iter1>,
	algo::__has_fast_memrmis<_Iter_value1, _Iter_value2, _Pred>,
	algo::__has_fast_memmis<_Iter_value1, _Iter_value2, _Pred>
	>
>{};

template<typename _Iter1, typename _Iter2, typename _Pred>
constexpr bool __has_fast_mismatch_v = __has_fast_mismatch<_Iter1, _Iter2, _Pred>::value;

struct _Mismatch_fn {

#if defined(WJR_HAS_EXECUTION)
	template<typename _ExPolicy, typename _Iter1, typename _Iter2, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
	std::pair<_Iter1, _Iter2> operator()(_ExPolicy&& _Policy, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) const {
		return std::mismatch(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2);
	}

	template<typename _ExPolicy, typename _Iter1, typename _Iter2, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
	std::pair<_Iter1, _Iter2> operator()(_ExPolicy&& _Policy, 
		_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) const {
		return std::mismatch(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2, _Last2);
	}

	template<typename _ExPolicy, typename _Iter1, typename _Iter2, typename _Pred, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
	std::pair<_Iter1, _Iter2> operator()(_ExPolicy&& _Policy, 
		_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred) const {
		return std::mismatch(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2, pred);
	}

	template<typename _ExPolicy, typename _Iter1, typename _Iter2, typename _Pred, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
	std::pair<_Iter1, _Iter2> operator()(_ExPolicy&& _Policy,
		_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) const {
		return std::mismatch(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2, _Last2, pred);
	}
#endif // WJR_HAS_EXECUTION

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
#if defined(__HAS_FAST_MEMMIS)
			if constexpr (__has_fast_mismatch_v<_Iter1, _Iter2, _Pred>) {
				const auto n = std::distance(_First1, _Last1);
				if (n == 0) { return std::make_pair(_First1, _First2); }
				if constexpr (!wjr::is_reverse_iterator_v<_Iter1>) {
					const auto first1 = wjr::to_address(_First1);
					const auto first2 = wjr::to_address(_First2);

					auto pos = algo::memmis(first1, first2, n, pred) - first1;
					return std::make_pair(_First1 + pos, _First2 + pos);
				}
				else {
					const auto first1 = wjr::to_address(_Last1 - 1);
					const auto _Last2 = _First2 + n;
					const auto first2 = wjr::to_address(_Last2 - 1);

					auto pos = (first1 + n) - algo::memrmis(first1, first2, n, pred);
					return std::make_pair(_First1 + pos, _First2 + pos);
				}
			}
#endif // __HAS_FAST_MEMMIS
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

constexpr _Mismatch_fn mismatch;

struct _Equal_fn {

#if defined(WJR_HAS_EXECUTION)
	template<typename _ExPolicy, typename _Iter1, typename _Iter2, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
	bool operator()(_ExPolicy&& _Policy, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) const {
		return std::equal(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2);
	}
	
	template<typename _ExPolicy, typename _Iter1, typename _Iter2, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
	bool operator()(_ExPolicy&& _Policy, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) const {
		return std::equal(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2, _Last2);
	}

	template<typename _ExPolicy, typename _Iter1, typename _Iter2, typename _Pred, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
	bool operator()(_ExPolicy&& _Policy, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred) const {
		return std::equal(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2, pred);
	}

	template<typename _ExPolicy, typename _Iter1, typename _Iter2, typename _Pred, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
	bool operator()(_ExPolicy&& _Policy, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) const {
		return std::equal(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2, _Last2, pred);
	}
#endif // WJR_HAS_EXECUTION

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
				if (n == 0) { return true; }
				if constexpr (!wjr::is_reverse_iterator_v<_Iter1>) {
					const auto first1 = wjr::to_address(_First1);
					const auto first2 = wjr::to_address(_First2);
					return algo::memcmp(first1, first2, n, pred);
				}
				else {
					const auto first1 = wjr::to_address(_Last1 - 1);
					const auto _Last2 = _First2 + n;
					const auto first2 = wjr::to_address(_Last2 - 1);
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

constexpr _Equal_fn equal{};

template<typename _Iter1, typename _Iter2, typename _Pred,
	typename _Iter_value1 = wjr::iter_val_t<_Iter1>,
	typename _Iter_value2 = wjr::iter_val_t<_Iter2>>
struct __has_fast_lexicographical_compare : std::conjunction<
	wjr::is_any_of<_Pred, std::less<>, std::not_equal_to<>>,
	__has_fast_mismatch<_Iter1, _Iter2, _Pred>
>{};

template<typename _Iter1, typename _Iter2, typename _Pred>
constexpr bool __has_fast_lexicographical_compare_v = __has_fast_lexicographical_compare<_Iter1, _Iter2, _Pred>::value;

struct _Lexicographical_compare_fn {

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

constexpr _Lexicographical_compare_fn lexicographical_compare{};

template<typename _Iter, typename _Val,
	typename _Iter_value = wjr::iter_val_t<_Iter>>
struct __has_fast_fill : std::conjunction<
	is_contiguous_iterator<_Iter>, 
	algo::__has_fast_memset<_Iter_value, _Val>
>{};

template<typename _Iter, typename _Val>
constexpr bool __has_fast_fill_v = __has_fast_fill<_Iter, _Val>::value;

struct _Fill_fn {

#if defined(WJR_HAS_EXECUTION)
	template<typename ExecutionPolicy, typename _Iter, typename _Val>
	void operator()(ExecutionPolicy&& policy,
		_Iter first, _Iter last, const _Val& value) const {
		return std::fill(policy, first, last, value);
	}
#endif // WJR_HAS_EXECUTION

	template<typename _Iter, typename _Val>
	WJR_CONSTEXPR20 void operator()(_Iter _First, _Iter _Last, const _Val& value) const {
		if (!wjr::is_constant_evaluated()) {
			if constexpr (__has_fast_fill_v<_Iter, _Val>) {
				const auto n = std::distance(_First, _Last);
				if constexpr (!is_reverse_iterator_v<_Iter>) {
					const auto first = wjr::to_address(_First);
					algo::memset(first, value, n);
				}
				else {
					if (n == 0) { return; }
					const auto first = wjr::to_address(_Last - 1);
					algo::memset(first, value, n);
				}
				return;
			}
		}
		return std::fill(_First, _Last, value);
	}

};

constexpr _Fill_fn fill{};

struct _Fill_n_fn {

#if defined(WJR_HAS_EXECUTION)
	template<typename ExecutionPolicy, typename _Iter, typename _Size, typename _Val>
	_Iter operator()(ExecutionPolicy&& policy,
		_Iter first, _Size count, const _Val& value) const {
		return std::fill_n(policy, first, count, value);
	}
#endif // WJR_HAS_EXECUTION

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

constexpr _Fill_n_fn fill_n{};

_WJR_END

#endif
