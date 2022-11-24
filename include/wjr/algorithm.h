#pragma once
#ifndef __WJR_ALGORITHM_H__
#define __WJR_ALGORITHM_H__

#include <algorithm>
#include <limits>
#include <cstring>
#include <wjr/math.h>
#include <wjr/simd.h>

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
//	template<typename ExecutionPolicy, typename ForwardIt, typename T, std::enable_if_t<
//		std::is_execution_policy_v<std::decay_t<ExecutionPolicy>>, int> = 0>
//	ForwardIt operator()(ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, const T& value) const {
//		return std::find(std::forward<ExecutionPolicy>(policy), first, last, value);
//	}

	template<typename _Iter, typename _Ty>
	WJR_CONSTEXPR _Iter operator()(_Iter _First, _Iter _Last, const _Ty& _Val) const {

		WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
			if constexpr (__Has_fast_find_v<_Iter, _Ty>) {
				using _Iter_value = typename std::iterator_traits<_Iter>::value_type;
				if (!in_range<std::decay_t<_Iter_value>>(_Val) || _First == _Last) {
					return _Last;
				}
#if defined(__HAS_SIMD_MEMCHR)
				if constexpr (wjr::is_contiguous_iterator_v<_Iter>) {
					const auto _Ptr_first = wjr::to_address(_First);
					const auto _Ptr_last = wjr::to_address(_Last - 1) + 1;
					if constexpr (sizeof(_Iter_value) == 1) {
						return _First + (simd_memchr((const uint8_t*)_Ptr_first, 
							(const uint8_t*)_Ptr_last, static_cast<uint8_t>(_Val)) - 
							(const uint8_t*)_Ptr_first);
					}
					else if constexpr (sizeof(_Iter_value) == 2) {
						return _First + (simd_memchr((const uint16_t*)_Ptr_first,
							(const uint16_t*)_Ptr_last, static_cast<uint16_t>(_Val)) - 
							(const uint16_t*)_Ptr_first);
					}
					else if constexpr (sizeof(_Iter_value) == 4) {
						return _First + (simd_memchr((const uint32_t*)_Ptr_first,
							(const uint32_t*)_Ptr_last, static_cast<uint32_t>(_Val)) - 
							(const uint32_t*)_Ptr_first);
					}
				}
				else {
					const auto _Ptr_first = wjr::to_address(_Last - 1);
					const auto _Ptr_last = wjr::to_address(_First) + 1;
					if constexpr (sizeof(_Iter_value) == 1) {
						return _Last - (simd_memrchr((const uint8_t*)_Ptr_first,
							(const uint8_t*)_Ptr_last, static_cast<uint8_t>(_Val)) -
							(const uint8_t*)_Ptr_first);
					}
					else if constexpr (sizeof(_Iter_value) == 2) {
						return _Last - (simd_memrchr((const uint16_t*)_Ptr_first,
							(const uint16_t*)_Ptr_last, static_cast<uint16_t>(_Val)) -
							(const uint16_t*)_Ptr_first);
					}
					else if constexpr (sizeof(_Iter_value) == 4) {
						return _Last - (simd_memrchr((const uint32_t*)_Ptr_first,
							(const uint32_t*)_Ptr_last, static_cast<uint32_t>(_Val)) -
							(const uint32_t*)_Ptr_first);
					}
				}
#else
				const auto _Ptr_first = wjr::to_address(_First);
				const auto _Ptr_last = wjr::to_address(_Last - 1) + 1;
				if constexpr (sizeof(_Iter_value) == sizeof(char)) {
					return _First + (::memchr((const char*)_Ptr_first,
						(int)_Val, _Ptr_last - _Ptr_first) - (const char*)_Ptr_first);
				}
				else if constexpr (sizeof(_Iter_value) == sizeof(wchar_t)) {
					return _First + (::wmemchr((const wchar_t*)_Ptr_first,
						(wchar_t)_Val, _Ptr_last - _Ptr_first) - (const wchar_t*)_Ptr_first);
				}
#endif
			}
		WJR_IS_NOT_CONSTANT_EVALUATED_END
		return std::find(_First, _Last, _Val);
	}
};

constexpr _Find_fn find{};

// equal

/*
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
using __has_fast_equal = __has_fast_equal_helper<_Iter1, _Iter2, _Pred>;

template<typename _Iter1, typename _Iter2, typename _Pred>
constexpr bool __has_fast_equal_v = __has_fast_equal<_Iter1, _Iter2, _Pred>::value;

struct _Equal_fn {
//	template<typename _ExPolicy, typename _Iter1, typename _Iter2, std::enable_if_t<
//		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
//	bool operator()(_ExPolicy&& _Policy, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) const {
//		return std::equal(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2);
//	}
	
//	template<typename _ExPolicy, typename _Iter1, typename _Iter2, std::enable_if_t<
//		std::is_execution_policy_v<std::decay_t<_ExPolicy>>, int> = 0>
//	bool operator()(_ExPolicy&& _Policy, _Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) const {
//		return std::equal(std::forward<_ExPolicy>(_Policy), _First1, _Last1, _First2, _Last2);
//	}

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
				const size_t n = std::distance(_First1, _Last1);
				if (n == 0) {
					return true;
				}
				if constexpr (wjr::is_contiguous_iterator_v<_Iter1>) {
					const auto _Ptr_first1 = wjr::to_address(_First1);
					const auto _Ptr_last1 = wjr::to_address(_Last1 - 1) + 1;
					const auto _Ptr_first2 = wjr::to_address(_First2);
#if defined(HAS_SIMD_MEMCMP)
					return simd_memcmp((const uint8_t*)_Ptr_first1, 
						(const uint8_t*)_Ptr_last1, 
						(const uint8_t*)_Ptr_first2) == (const uint8_t*)_Ptr_last1;
#else
					return memcmp((const void*)_Ptr_first1,
						(const void*)_Ptr_first2,
						n * sizeof(_Iter_value)) == 0;
#endif
				}
				else {
					const auto _Ptr_first1 = wjr::to_address(_Last1 - 1);
					const auto _Ptr_last1 = wjr::to_address(_First1) + 1;
					const auto _Last2 = _First2 + n;
					const auto _Ptr_first2 = wjr::to_address(_Last2 - 1);
#if defined(HAS_SIMD_MEMCMP)
					return simd_memcmp((const uint8_t*)_Ptr_first1,
						(const uint8_t*)_Ptr_last1,
						(const uint8_t*)_Ptr_first2) == (const uint8_t*)_Ptr_last1;
#else
					return memcmp((const void*)_Ptr_first1, (const void*)_Ptr_first2,
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
				if (n == 0) {
					return true;
				}
				return this->operator()(_First1, _Last1, _First2, pred);
			}
		WJR_IS_NOT_CONSTANT_EVALUATED_END
		return std::equal(_First1, _Last1, _First2, _Last2, pred);
	}

};

constexpr _Equal_fn equal{};

*/

_WJR_END

#endif
