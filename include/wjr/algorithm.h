#pragma once
#ifndef __WJR_ALGORITHM_H__
#define __WJR_ALGORITHM_H__

#include <algorithm>
#include <limits>
#include <cstring>
#include <execution>
#include <wjr/math.h>
#include <wjr/simd/simd.h>

_WJR_BEGIN

template<typename _Iter, typename _Val,
	typename _Iter_value = wjr::iter_val_t<_Iter>>
using __Has_fast_find = std::conjunction<
	wjr::is_memory_compare<_Iter_value>,
#if defined(__HAS_SIMD_MEMCHR)
	std::disjunction<wjr::is_contiguous_iterator<_Iter>,
	wjr::is_reverse_contiguous_iterator<_Iter>>,
	wjr::is_any_index_of<sizeof(_Iter_value), 1, 2, 4>,
#else
	wjr::is_contiguous_iterator<_Iter>,
	wjr::is_any_index_of<sizeof(_Iter_value), sizeof(char), sizeof(wchar_t)>,
#endif
	std::is_integral<_Val>
>;

template<typename _Iter, typename _Val>
constexpr bool __Has_fast_find_v = __Has_fast_find<_Iter, _Val>::value;

struct _Find_fn {
	template<typename ExecutionPolicy, typename ForwardIt, typename T, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>, int> = 0>
	ForwardIt operator()(ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, const T& value) const {
		return std::find(std::forward<ExecutionPolicy>(policy), first, last, value);
	}

	template<typename _Iter, typename _Ty>
	WJR_CONSTEXPR _Iter operator()(_Iter _First, _Iter _Last, const _Ty& _Val) const {
		WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
			if constexpr (__Has_fast_find_v<_Iter, _Ty>) {
				using _Iter_value = typename std::iterator_traits<_Iter>::value_type;
				const auto n = _Last - _First;
				if (!in_range<std::decay_t<_Iter_value>>(_Val) || n == 0) {
					return _Last;
				}
#if defined(__HAS_SIMD_MEMCHR) // use simd::memchr
				using __value_t = std::conditional_t<
					sizeof(_Iter_value) == 1,
					uint8_t,
					std::conditional_t<
					sizeof(_Iter_value) == 2,
					uint16_t,
					uint32_t
					>
				>;
				if constexpr (wjr::is_contiguous_iterator_v<_Iter>) {
					const auto _Ptr_first = wjr::to_address(_First);
					const auto _Ptr_last = wjr::to_address(_Last - 1) + 1;
					auto __s = (const __value_t*)_Ptr_first;
					auto __e = (const __value_t*)_Ptr_last;
					return _First + (simd::memchr(__s, static_cast<__value_t>(_Val), n) - __s);
				}
				else {
					const auto _Ptr_first = wjr::to_address(_Last - 1);
					const auto _Ptr_last = wjr::to_address(_First) + 1;
					auto __s = (const __value_t*)_Ptr_first;
					auto __e = (const __value_t*)_Ptr_last;
					return _Last - (simd::memrchr(__s, static_cast<__value_t>(_Val),n) - __s);
				}
#else // use memchr or wmemchr
				const auto _Ptr_first = wjr::to_address(_First);
				const auto _Ptr_last = wjr::to_address(_Last - 1) + 1;
				if constexpr (sizeof(_Iter_value) == sizeof(char)) {
					return _First + (::memchr((const char*)_Ptr_first,
						(int)_Val, n) - (const char*)_Ptr_first);
				}
				else if constexpr (sizeof(_Iter_value) == sizeof(wchar_t)) {
					return _First + (::wmemchr((const wchar_t*)_Ptr_first,
						(wchar_t)_Val, n) - (const wchar_t*)_Ptr_first);
				}
#endif
			}
		WJR_IS_NOT_CONSTANT_EVALUATED_END
		return std::find(_First, _Last, _Val);
	}
};

constexpr _Find_fn find{};

struct _Find_if_fn {
	template<typename ExecutionPolicy, typename ForwardIt, typename UnaryPredicate, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>, int> = 0>
	ForwardIt operator()(ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, UnaryPredicate p) const {
		return std::find_if(std::forward<ExecutionPolicy>(policy), first, last, p);
	}

	template<typename _Iter, typename _Pr>
	WJR_CONSTEXPR _Iter operator()(_Iter _First, _Iter _Last, _Pr _Pred) const {
		return std::find_if(_First, _Last, _Pred);
	}
};

constexpr _Find_if_fn find_if{};

struct _Find_if_not_fn {
	template<typename ExecutionPolicy, typename ForwardIt, typename UnaryPredicate, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>, int> = 0>
	ForwardIt operator()(ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, UnaryPredicate p) const {
		return std::find_if_not(std::forward<ExecutionPolicy>(policy), first, last, p);
	}

	template<typename _Iter, typename _Pr>
	WJR_CONSTEXPR _Iter operator()(_Iter _First, _Iter _Last, _Pr _Pred) const {
		return std::find_if_not(_First, _Last, _Pred);
	}
};

constexpr _Find_if_not_fn find_if_not{};

struct _Count_fn {
	template<typename ExecutionPolicy, typename ForwardIt, typename T, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>, int> = 0>
	typename std::iterator_traits<ForwardIt>::difference_type 
		operator()(ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, const T& value) const {
		return std::count(std::forward<ExecutionPolicy>(policy), first, last, value);
	}

	template<typename _Iter, typename _Ty>
	WJR_CONSTEXPR typename std::iterator_traits<_Iter>::difference_type
		operator()(_Iter _First, _Iter _Last, const _Ty& _Val) const {
		return std::count(_First, _Last, _Val);
	}
};

constexpr _Count_fn count{};

struct _Count_if_fn {
	template<typename ExecutionPolicy, typename ForwardIt, typename UnaryPredicate, std::enable_if_t<
		std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>, int> = 0>
	typename std::iterator_traits<ForwardIt>::difference_type
		operator()(ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, UnaryPredicate p) const {
		return std::count_if(std::forward<ExecutionPolicy>(policy), first, last, p);
	}

	template<typename _Iter, typename _Pr>
	WJR_CONSTEXPR typename std::iterator_traits<_Iter>::difference_type
		operator()(_Iter _First, _Iter _Last, _Pr _Pred) const {
		return std::count_if(_First, _Last, _Pred);
	}
};

constexpr _Count_if_fn count_if{};

// First use simd::memcmp
// Then use memcmp
template<typename _Iter1, typename _Iter2, typename _Pred, 
	typename _Iter_value1 = wjr::iter_val_t<_Iter1>,
	typename _Iter_value2 = wjr::iter_val_t<_Iter2>>
using __has_fast_equal_helper = std::conjunction<
	std::disjunction<
	std::conjunction<
	wjr::is_contiguous_iterator<_Iter1>,
	wjr::is_contiguous_iterator<_Iter2>
	>,
	std::conjunction<
	wjr::is_reverse_contiguous_iterator<_Iter1>,
	wjr::is_reverse_contiguous_iterator<_Iter2>
	>
	>,
	wjr::is_memory_compare<_Iter_value1>,
	wjr::is_memory_compare<_Iter_value2>,
	std::bool_constant<sizeof(_Iter_value1) == sizeof(_Iter_value2)>,
	std::is_same<_Pred, std::equal_to<>>
>;

template<typename _Iter1, typename _Iter2, typename _Pred>
using __has_fast_equal = std::bool_constant<__has_fast_equal_helper<_Iter1, _Iter2, _Pred>::value>;

template<typename _Iter1, typename _Iter2, typename _Pred>
constexpr bool __has_fast_equal_v = __has_fast_equal<_Iter1, _Iter2, _Pred>::value;

// First use simd::memcmpeq
// Then use simd::memcmp
template<typename _Iter1, typename _Iter2, typename _Pred>
using __has_fast_mismatch =
#if defined(__HAS_SIMD_MEMCMP) // use simd::memcmp
	std::bool_constant<__has_fast_equal<_Iter1, _Iter2, _Pred>::value>;
#else // can't use memcmp
	std::false_type;
#endif

template<typename _Iter1, typename _Iter2, typename _Pred>
constexpr bool __has_fast_mismatch_v = __has_fast_mismatch<_Iter1, _Iter2, _Pred>::value;

struct _Mismatch_fn {
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

	template<typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR std::pair<_Iter1, _Iter2> operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) const {
		return this->operator()(_First1, _Last1, _First2, std::equal_to<>{});
	}

	template<typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR std::pair<_Iter1, _Iter2> operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) const {
		return this->operator()(_First1, _Last1, _First2, _Last2, std::equal_to<>{});
	}

	template<typename _Iter1, typename _Iter2, typename _Pred>
	WJR_CONSTEXPR std::pair<_Iter1, _Iter2> operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred) const {
		WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
			if constexpr (__has_fast_mismatch_v<_Iter1, _Iter2, _Pred>) {
				using _Iter_value = wjr::iter_val_t<_Iter1>;
				const auto n = std::distance(_First1, _Last1);
				if (n == 0) {
					return std::make_pair(_First1, _First2);
				}
				if constexpr (wjr::is_contiguous_iterator_v<_Iter1>) {
					const auto _Ptr_first1 = wjr::to_address(_First1);
					const auto _Ptr_first2 = wjr::to_address(_First2);

					auto __s0 = (const uint8_t*)(_Ptr_first1);
					auto __s1 = (const uint8_t*)(_Ptr_first2);

					auto p = simd::memcmp(__s0, __s1, n * sizeof(_Iter_value));
					auto dif = (p - __s0) / sizeof(_Iter_value);
					return std::make_pair(_First1 + dif, _First2 + dif);
				}
				else {
					const auto _Ptr_first1 = wjr::to_address(_Last1 - 1);
					const auto _Last2 = _First2 + n;
					const auto _Ptr_first2 = wjr::to_address(_Last2 - 1);
					
					auto __s0 = (const uint8_t*)(_Ptr_first1);
					auto __e0 = __s0 + n * sizeof(_Iter_value);
					auto __s1 = (const uint8_t*)(_Ptr_first2);

					auto p = simd::memrcmp(__s0, __s1, n * sizeof(_Iter_value));
					auto dif = (__e0 - p) / sizeof(_Iter_value);
					return std::make_pair(_First1 + dif, _First2 + dif);
				}
			}
		WJR_IS_NOT_CONSTANT_EVALUATED_END
		return std::mismatch(_First1, _Last1, _First2, pred);
	}

	template<typename _Iter1, typename _Iter2, typename _Pred>
	WJR_CONSTEXPR std::pair<_Iter1, _Iter2> operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) const {
		WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
			if constexpr (__has_fast_mismatch_v<_Iter1, _Iter2, _Pred>) {
				const auto n = _Last1 - _First1;
				const auto m = _Last2 - _First2;
				const auto _M = n < m ? n : m;
				return this->operator()(_First1, _First1 + _M, _First2, pred);
			}
		WJR_IS_NOT_CONSTANT_EVALUATED_END
		return std::mismatch(_First1, _Last1, _First2, _Last2, pred);
	}

};

constexpr _Mismatch_fn mismatch;

struct _Equal_fn {
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

	template<typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR bool operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) const {
		return this->operator()(_First1, _Last1, _First2, std::equal_to<>{});
	}

	template<typename _Iter1, typename _Iter2>
	WJR_CONSTEXPR bool operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) const {
		return this->operator()(_First1, _Last1, _First2, _Last2, std::equal_to<>{});
	}

	template<typename _Iter1, typename _Iter2, typename _Pred>
	WJR_CONSTEXPR bool operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred) const {
		WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
			if constexpr (__has_fast_equal_v<_Iter1, _Iter2, _Pred>) {
				using _Iter_value = wjr::iter_val_t<_Iter1>;
				const auto n = _Last1 - _First1;
				if (n == 0) {
					return true;
				}
				if constexpr (wjr::is_contiguous_iterator_v<_Iter1>) {
					const auto _Ptr_first1 = wjr::to_address(_First1);
					const auto _Ptr_last1 = wjr::to_address(_Last1 - 1) + 1;
					const auto _Ptr_first2 = wjr::to_address(_First2);
#if defined(__HAS_SIMD_MEMCMP) || defined(__HAS_SIMD_MEMCMPEQ)
					auto __s0 = (const uint8_t*)_Ptr_first1;
					auto __e0 = (const uint8_t*)_Ptr_last1;
					auto __s1 = (const uint8_t*)_Ptr_first2;
#if defined(__HAS_SIMD_MEMCMPEQ)
					return simd::memcmpeq(__s0, __s1, n * sizeof(_Iter_value));
#else
					return simd::memcmp(__s0, __s1, n * sizeof(_Iter_value)) == __e0;
#endif
#else // memcmp
					return memcmp(_Ptr_first1, _Ptr_first2,
						n * sizeof(_Iter_value)) == 0;
#endif
				}
				else {
					const auto _Ptr_first1 = wjr::to_address(_Last1 - 1);
					const auto _Ptr_last1 = _Ptr_first1 + n;
					const auto _Last2 = _First2 + n;
					const auto _Ptr_first2 = wjr::to_address(_Last2 - 1);
#if defined(__HAS_SIMD_MEMCMP)
					auto __s0 = (const uint8_t*)_Ptr_first1;
					auto __e0 = (const uint8_t*)_Ptr_last1;
					auto __s1 = (const uint8_t*)_Ptr_first2;
#if defined(__HAS_SIMD_MEMCMPEQ)
					return simd::memcmpeq(__s0, __s1, n * sizeof(_Iter_value));
#else
					return simd::memcmp(__s0, __s1, n * sizeof(_Iter_value)) == __e0;
#endif
#else
					return memcmp(_Ptr_first1, _Ptr_first2,
						n * sizeof(_Iter_value)) == 0;
#endif
				}
			}
		WJR_IS_NOT_CONSTANT_EVALUATED_END
		return std::equal(_First1, _Last1, _First2, pred);
	}

	template<typename _Iter1, typename _Iter2, typename _Pred>
	WJR_CONSTEXPR bool operator()(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) const {
		WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
			if constexpr (__has_fast_equal_v<_Iter1, _Iter2, _Pred>) {
				const auto n = std::distance(_First1, _Last1);
				const auto m = std::distance(_First2, _Last2);
				if (n != m) {
					return false;
				}
				return this->operator()(_First1, _Last1, _First2, pred);
			}
		WJR_IS_NOT_CONSTANT_EVALUATED_END
		return std::equal(_First1, _Last1, _First2, _Last2, pred);
	}

};

constexpr _Equal_fn equal{};

_WJR_END

#endif
