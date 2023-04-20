#ifndef __WJR_ALGORITHM_H
#error  "This file should not be included directly. Include <wjr/algorithm.h> instead."
#endif //__WJR_ALGORITHM_H

#include <algorithm>
#include <memory>
#include <limits>
#include <cstring>

#include <wjr/compressed_pair.h>
#include <wjr/algo/algo.h>

_WJR_BEGIN

template<typename _Iter, typename _Val, typename _Pred,
	typename _Iter_value = iter_val_t<_Iter>>
	struct __has_fast_find : std::conjunction<
	is_contiguous_iter<_Iter>,
	std::conditional_t<
	wjr::is_reverse_iterator_v<_Iter>,
	algo::__has_fast_memrchr<_Iter_value, _Val, _Pred>,
	algo::__has_fast_memchr<_Iter_value, _Val, _Pred>
	>
	> {};

template<typename _Iter, typename _Val, typename _Pred>
constexpr bool __has_fast_find_v = __has_fast_find<_Iter, _Val, _Pred>::value;

// find

template<typename _Iter, typename _Ty, typename _Pred>
WJR_CONSTEXPR20 _Iter do_find(
	_Iter _First, _Iter _Last, const _Ty& _Val, _Pred pred) {
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

template<typename _Iter, typename _Ty>
WJR_CONSTEXPR20 _Iter do_find(_Iter _First, _Iter _Last, const _Ty& _Val) {
	return wjr::find(_First, _Last, _Val, std::equal_to<>{});
}

template<typename _Iter, typename _Pr>
WJR_CONSTEXPR20 _Iter do_find_if(_Iter _First, _Iter _Last, _Pr _Pred) {
	return std::find_if(_First, _Last, _Pred);
}

template<typename _Iter, typename _Pr>
WJR_CONSTEXPR20 _Iter do_find_if_not(_Iter _First, _Iter _Last, _Pr _Pred) {
	return std::find_if_not(_First, _Last, _Pred);
}

template<typename _Iter, typename _Val,
	typename _Iter_value = iter_val_t<_Iter>>
	struct __has_fast_count : std::conjunction<
	wjr::is_contiguous_iter<_Iter>,
	algo::__has_fast_memcnt<_Iter_value, _Val>
	> {};

template<typename _Iter, typename _Val>
constexpr bool __has_fast_count_v = __has_fast_count<_Iter, _Val>::value;

template<typename _Iter, typename _Ty>
WJR_CONSTEXPR20 typename std::iterator_traits<_Iter>::difference_type
do_count(_Iter _First, _Iter _Last, const _Ty& _Val) {
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

template<typename _Iter, typename _Pr>
WJR_CONSTEXPR20 typename std::iterator_traits<_Iter>::difference_type
do_count_if(_Iter _First, _Iter _Last, _Pr _Pred) {
	return std::count_if(_First, _Last, _Pred);
}

// First use algo::memcmp
// Then use memcmp
template<typename _Iter1, typename _Iter2, typename _Pred,
	typename _Iter_value1 = iter_val_t<_Iter1>,
	typename _Iter_value2 = iter_val_t<_Iter2>>
	struct __has_fast_equal_helper : std::conjunction<
	wjr::is_contiguous_iter<_Iter1>,
	wjr::is_contiguous_iter<_Iter2>,
	std::bool_constant<wjr::is_reverse_iterator_v<_Iter1> == wjr::is_reverse_iterator_v<_Iter2>>,
	algo::__has_fast_memcmp<_Iter_value1, _Iter_value2, _Pred>
	> {};

template<typename _Iter1, typename _Iter2, typename _Pred>
struct __has_fast_equal : std::bool_constant<__has_fast_equal_helper<_Iter1, _Iter2, _Pred>::value> {};

template<typename _Iter1, typename _Iter2, typename _Pred>
constexpr bool __has_fast_equal_v = __has_fast_equal<_Iter1, _Iter2, _Pred>::value;

template<typename _Iter1, typename _Iter2, typename _Pred,
	typename _Iter_value1 = iter_val_t<_Iter1>,
	typename _Iter_value2 = iter_val_t<_Iter2>>
	struct __has_fast_mismatch : std::conjunction <
	wjr::is_contiguous_iter<_Iter1>,
	wjr::is_contiguous_iter<_Iter2>,
	std::bool_constant<wjr::is_reverse_iterator_v<_Iter1> == wjr::is_reverse_iterator_v<_Iter2>>,
	std::conditional_t<
	wjr::is_reverse_iterator_v<_Iter1>,
	algo::__has_fast_memrmis<_Iter_value1, _Iter_value2, _Pred>,
	algo::__has_fast_memmis<_Iter_value1, _Iter_value2, _Pred>
	>
	> {};

template<typename _Iter1, typename _Iter2, typename _Pred>
constexpr bool __has_fast_mismatch_v = __has_fast_mismatch<_Iter1, _Iter2, _Pred>::value;

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> do_mismatch(
	_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred) {
	if (!wjr::is_constant_evaluated()) {
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
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> do_mismatch(
	_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) {
	if (!wjr::is_constant_evaluated()) {
		if constexpr (__has_fast_mismatch_v<_Iter1, _Iter2, _Pred>) {
			const auto n = _Last1 - _First1;
			const auto m = _Last2 - _First2;
			const auto _M = n < m ? n : m;
			return wjr::mismatch(_First1, _First1 + _M, _First2, pred);
		}
	}
	return std::mismatch(_First1, _Last1, _First2, _Last2, pred);
}

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> do_mismatch(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) {
	return wjr::mismatch(_First1, _Last1, _First2, std::equal_to<>{});
}

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> do_mismatch(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) {
	return wjr::mismatch(_First1, _Last1, _First2, _Last2, std::equal_to<>{});
}

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 bool do_equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred) {
	if (!wjr::is_constant_evaluated()) {
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
WJR_CONSTEXPR20 bool do_equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) {
	if (!wjr::is_constant_evaluated()) {
		if constexpr (__has_fast_equal_v<_Iter1, _Iter2, _Pred>) {
			const auto n = std::distance(_First1, _Last1);
			const auto m = std::distance(_First2, _Last2);
			if (n != m) { return false; }
			return wjr::equal(_First1, _Last1, _First2, pred);
		}
	}
	return std::equal(_First1, _Last1, _First2, _Last2, pred);
}

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 bool do_equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) {
	return wjr::equal(_First1, _Last1, _First2, std::equal_to<>{});
}

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 bool do_equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) {
	return wjr::equal(_First1, _Last1, _First2, _Last2, std::equal_to<>{});
}

template<typename _Iter1, typename _Iter2, typename _Pred,
	typename _Iter_value1 = iter_val_t<_Iter1>,
	typename _Iter_value2 = iter_val_t<_Iter2>>
	struct __has_fast_lexicographical_compare : std::conjunction<
	wjr::is_any_of<_Pred, std::less<>>,
#if defined(_WJR_FAST_MEMMIS)
	__has_fast_mismatch<_Iter1, _Iter2, _Pred>
#else
	wjr::is_contiguous_iter<_Iter1>,
	wjr::is_contiguous_iter<_Iter2>,
	std::bool_constant<wjr::is_reverse_iterator_v<_Iter1> == wjr::is_reverse_iterator_v<_Iter2>>,
	std::conditional_t<
	wjr::is_reverse_iterator_v<_Iter1>,
	std::false_type,
	is_memory_comparable<_Iter_value1, _Iter_value2, _Pred>
	>,
	std::is_unsigned<_Iter_value1>,
	std::is_unsigned<_Iter_value2>,
	wjr::is_any_index_of<sizeof(_Iter_value1), 1>
#endif // _WJR_FAST_MEMMIS
	> {};

template<typename _Iter1, typename _Iter2, typename _Pred>
constexpr bool __has_fast_lexicographical_compare_v = __has_fast_lexicographical_compare<_Iter1, _Iter2, _Pred>::value;

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 bool do_lexicographical_compare(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) {
	if (!wjr::is_constant_evaluated()) {
		if constexpr (__has_fast_lexicographical_compare_v<_Iter1, _Iter2, _Pred>) {
			const auto n = std::distance(_First1, _Last1);
			const auto m = std::distance(_First2, _Last2);
			auto cl = std::min(n, m);
#if defined(_WJR_FAST_MEMMIS)
			auto e = _First1 + cl;
			auto pos = wjr::mismatch(_First1, e, _First2, std::equal_to<>{}).first - _First1;
			if (pos != cl) {
				return pred(_First1[pos], _First2[pos]);
			}
#else
			auto first1 = get_address(_First1);
			auto first2 = get_address(_First2);
			auto f = ::memcmp(first1, first2, cl * sizeof(iter_val_t<_Iter1>));
			if (f != 0) {
				return f < 0;
			}
#endif // _WJR_FAST_MEMMIS
			return n < m;
		}
	}
	return std::lexicographical_compare(_First1, _Last1, _First2, _Last2, pred);
}

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 bool do_lexicographical_compare(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) {
	return wjr::lexicographical_compare(_First1, _Last1, _First2, _Last2, std::less<>{});
}

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 int do_compare(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) {
	if (!wjr::is_constant_evaluated()) {
		if constexpr (__has_fast_lexicographical_compare_v<_Iter1, _Iter2, _Pred>) {
			const auto n = std::distance(_First1, _Last1);
			const auto m = std::distance(_First2, _Last2);
			auto cl = std::min(n, m);
#if defined(_WJR_FAST_MEMMIS)
			auto e = _First1 + cl;
			auto pos = wjr::mismatch(_First1, e, _First2, std::equal_to<>{}).first - _First1;
			if (pos != cl) {
				return pred(_First1[pos], _First2[pos]) ? -1 : 1;
			}
#else
			auto first1 = get_address(_First1);
			auto first2 = get_address(_First2);
			auto f = ::memcmp(first1, first2, cl * sizeof(iter_val_t<_Iter1>));
			if (f != 0) {
				return f;
			}
#endif // _WJR_FAST_MEMMIS
			return n < m ? -1 : (n > m ? 1 : 0);
		}
	}
	for (; _First1 != _Last1 && _First2 != _Last2; ++_First1, ++_First2) {
		if (pred(*_First1, *_First2)) {
			return -1;
		}
		if (pred(*_First2, *_First1)) {
			return 1;
		}
	}
	return _First1 == _Last1 ? (_First2 == _Last2 ? 0 : -1) : 1;
}

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 int do_compare(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) {
	return wjr::compare(_First1, _Last1, _First2, _Last2, std::less<>{});
}

template<typename _Iter, typename _Val,
	typename _Iter_ref = iter_ref_t<_Iter>>
	struct __has_fast_fill : std::conjunction<
	is_contiguous_iter<_Iter>,
	algo::__has_fast_assign_memset<remove_cref_t<_Iter_ref>, add_cref_t<_Val>>
	> {};

template<typename _Iter, typename _Val>
constexpr bool __has_fast_fill_v = __has_fast_fill<_Iter, _Val>::value;

template<typename _Iter, typename _Val>
WJR_CONSTEXPR20 void do_fill(_Iter _First, _Iter _Last, const _Val& value) {
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

template<typename _Iter, typename _Size, typename _Val>
WJR_CONSTEXPR20 _Iter do_fill_n(_Iter _First, _Size count, const _Val& value) {
	if (!wjr::is_constant_evaluated()) {
		if constexpr (__has_fast_fill_v<_Iter, _Val>) {
			if (count <= 0) { return _First; }
			wjr::fill(_First, _First + count, value);
			return _First + count;
		}
	}
	return std::fill_n(_First, count, value);
}

template<typename _Input, typename _Output,
	typename _Input_ref = iter_ref_t<_Input>,
	typename _Output_ref = iter_ref_t<_Output>>
	struct __has_fast_copy : std::conjunction<
	is_contiguous_iter<_Input>,
	is_contiguous_iter<_Output>,
	std::bool_constant<wjr::is_reverse_iterator_v<_Input> == wjr::is_reverse_iterator_v<_Output>>,
	algo::__has_fast_assign_memcpy<remove_ref_t<_Output_ref>, _Input_ref>
	> {};

template<typename _Input, typename _Output>
constexpr bool __has_fast_copy_v = __has_fast_copy<_Input, _Output>::value;

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output do_copy(_Input _First1, _Input _Last1, _Output _First2) {
	static_assert(!std::is_const_v<iter_ref_t<_Output>>, "Output iterator must not be const");
	if (!wjr::is_constant_evaluated()) {
		if constexpr (__has_fast_copy_v<_Input, _Output>) {
			const auto n = std::distance(_First1, _Last1);
			if (is_unlikely(n == 0)) { return _First2; }
			if constexpr (!wjr::is_reverse_iterator_v<_Input>) {
				const auto first1 = wjr::get_address(_First1);
				const auto first2 = wjr::get_address(_First2);

				algo::assign_memcpy(first2, first1, n);
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


template<typename _Input, typename _Size, typename _Output>
WJR_CONSTEXPR20 _Output do_copy_n(_Input _First1, _Size count, _Output _First2) {
	if (!wjr::is_constant_evaluated()) {
		if constexpr (__has_fast_copy_v<_Input, _Output>) {
			if (count <= 0) { return _First2; }
			return wjr::copy(_First1, _First1 + count, _First2);
		}
	}
	return std::copy_n(_First1, count, _First2);
}

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output do_copy_backward(_Input _First1, _Input _Last1, _Output _Last2) {
	return wjr::copy(std::make_reverse_iterator(_Last1),
		std::make_reverse_iterator(_First1),
		std::make_reverse_iterator(_Last2)).base();
}

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output do_move(_Input _First1, _Input _Last1, _Output _First2) {
	return wjr::copy(std::make_move_iterator(_First1), std::make_move_iterator(_Last1), _First2);
}

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output do_move_backward(_Input _First1, _Input _Last1, _Output _Last2) {
	return wjr::copy_backward(std::make_move_iterator(_First1), std::make_move_iterator(_Last1), _Last2);
}

template<typename _Iter, typename _Pred>
class default_searcher {
public:
	WJR_CONSTEXPR20 default_searcher(_Iter _First, _Iter _Last, _Pred _Eq = _Pred())
		: m_pair(std::move(_Eq), std::move(std::make_pair(_First, _Last))) {
	}

	template <typename _Iter2>
	WJR_NODISCARD WJR_CONSTEXPR20 std::pair<_Iter2, _Iter2> operator()(
		_Iter2 _First, _Iter2 _Last) const {
		auto& pred = m_pair.first();
		auto _Myfirst = m_pair.second().first;
		auto _Mylast = m_pair.second().second;
		if constexpr (is_random_iterator_v<_Iter> && is_random_iterator_v<_Iter2>) {
			const auto n = std::distance(_First, _Last);
			const auto m = std::distance(_Myfirst, _Mylast);
			if (m == 0) {
				return std::make_pair(_First, _First);
			}
			if (n < m) {
				return std::make_pair(_Last, _Last);
			}
			const auto& _Val = *_Myfirst;
			auto _Mid = _Last - (m - 1);
			for (;; ++_First) {
				_First = wjr::find(_First, _Mid, _Val, pred);
				if (_First == _Mid) {
					return std::make_pair(_Last, _Last);
				}
				if (wjr::equal(_Myfirst, _Mylast, _First, pred)) {
					return std::make_pair(_First, _First + m);
				}
			}
		}
		else {
			for (;; ++_First) { // loop until match or end of a sequence
				auto _Mid = _First;
				for (auto _Mid2 = _Myfirst;; ++_Mid, (void) ++_Mid2) {
					if (_Mid2 == _Mylast) {
						return std::make_pair(_First, _Mid);
					}

					if (_Mid == _Last) {
						return std::make_pair(_Last, _Last);
					}

					if (!pred(*_Mid, *_Mid2)) {
						break;
					}
				}
			}
		}
	}
private:
	compressed_pair<_Pred, std::pair<_Iter, _Iter>> m_pair;
};

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter1 do_search(_Iter1 _First1, _Iter1 _Last1,
	_Iter2 _First2, _Iter2 _Last2) {
	return wjr::search(_First1, _Last1, _First2, _Last2, std::equal_to<>{});
}

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 _Iter1 do_search(_Iter1 _First1, _Iter1 _Last1,
	_Iter2 _First2, _Iter2 _Last2, _Pred pred) {
	return wjr::search(_First1, _Last1, wjr::default_searcher(_First2, _Last2, pred));
}

template<typename _Iter, typename _Searcher>
WJR_CONSTEXPR20 _Iter do_search(_Iter _First, _Iter _Last, const _Searcher& _Srch) {
	return _Srch(_First, _Last).first;
}

template<typename _Iter, typename...Args, 
	std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int>>
WJR_CONSTEXPR20 void do_construct_at(_Iter iter, Args&&... args) {
#if defined(WJR_CPP_20)
	std::construct_at(get_address(iter), std::forward<Args>(args)...);
#else
	using value_type = iter_val_t<_Iter>;
	(void)(::new (voidify(get_address(iter))) value_type(std::forward<Args>(args)...));
#endif // WJR_CPP_20
}

template<typename _Iter, std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int>>
WJR_CONSTEXPR20 void do_construct_at(_Iter iter, default_construct_tag) {
	using value_type = iter_val_t<_Iter>;
	if (!is_constant_evaluated()) {
		(void)(::new (voidify(get_address(iter))) value_type);
		return;
	}

	if constexpr (std::is_trivially_default_constructible_v<value_type>) {
		// do nothing
	}
	else {
		// use value_construct
		construct_at(iter, value_construct_tag());
	}
}

template<typename _Iter, std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int>>
WJR_CONSTEXPR20 void do_construct_at(_Iter iter, value_construct_tag) {
	wjr::construct_at(iter);
}

template<typename Alloc, typename _Iter, typename...Args, 
	std::enable_if_t<!is_iterator_v<wjr::remove_cvref_t<Alloc>>, int>>
WJR_CONSTEXPR20 void do_construct_at(Alloc& al, _Iter iter, Args&&...args) {
	using pointer = iter_address_t<_Iter>;
	if constexpr (is_default_allocator_construct_v<Alloc, pointer, Args...>) {
		wjr::construct_at(iter, std::forward<Args>(args)...);
	}
	else {
		if constexpr(sizeof...(Args) == 1){
			// allocator may not support default construct
			// do default construct as value construct
			if constexpr (is_any_of_v<default_construct_tag, Args...> 
				|| is_any_of_v<value_construct_tag, Args...>) {
				return construct_at(al, iter);
			}
		}
		std::allocator_traits<Alloc>::construct(al, get_address(iter), std::forward<Args>(args)...);
	}
}

template<typename _Iter>
WJR_CONSTEXPR20 void do_destroy_at(_Iter ptr) {
	using value_type = iter_val_t<_Iter>;
	if constexpr (!std::is_trivially_destructible_v<value_type>) {
		get_address(ptr)->~value_type();
	}
}

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void do_destroy_at(Alloc& al, _Iter iter) {
	if constexpr (is_default_allocator_destroy_v<Alloc, _Iter>) {
		wjr::destroy_at(iter);
	}
	else {
		std::allocator_traits<Alloc>::destroy(al, get_address(iter));
	}
}

template<typename _Iter>
WJR_CONSTEXPR20 void do_destroy(_Iter _First, _Iter _Last) {
	using value_type = iter_val_t<_Iter>;
	if constexpr (!std::is_trivially_destructible_v<value_type>) {
		for (; _First != _Last; ++_First) {
			wjr::destroy_at(_First);
		}
	}
}

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void do_destroy(Alloc& al, _Iter _First, _Iter _Last) {
	using value_type = iter_val_t<_Iter>;
	if constexpr (!(is_default_allocator_destroy_v<Alloc, _Iter>
		&& std::is_trivially_destructible_v<value_type>)) {
		for (; _First != _Last; ++_First) {
			wjr::destroy_at(al, _First);
		}
	}
}


template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 void do_destroy_n(_Iter _First, const _Diff n) {
	using value_type = iter_val_t<_Iter>;
	if constexpr (!std::is_trivially_destructible_v<value_type>) {
		for (; n > 0; ++_First, (void)--n) {
			wjr::destroy_at(_First);
		}
	}
}

template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 void do_destroy_n(Alloc& al, _Iter _First, _Diff n) {
	using value_type = iter_val_t<_Iter>;
	if constexpr (!(is_default_allocator_destroy_v<Alloc, _Iter> && std::is_trivially_destructible_v<value_type>)) {
		for (; n > 0; ++_First, (void)--n) {
			wjr::destroy_at(al, _First);
		}
	}
}

template<typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_default_construct(_Iter _First, _Iter _Last) {
	if (!wjr::is_constant_evaluated()) {
		return std::uninitialized_default_construct(_First, _Last);
	}
	using value_type = iter_val_t<_Iter>;
	if constexpr (std::is_trivially_default_constructible_v<value_type>) {
	}
	else {
		for (; _First != _Last; (void)++_First) {
			wjr::construct_at(_First, default_construct_tag());
		}
	}
	
}

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_default_construct(
	Alloc& al, _Iter _First, _Iter _Last) {
	if constexpr (is_default_allocator_construct_v<Alloc, _Iter, default_construct_tag>) {
		wjr::uninitialized_default_construct(_First, _Last);
	}
	else {
		for (; _First != _Last; (void)++_First) {
			wjr::construct_at(al, _First, default_construct_tag());
		}
	}
}

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_default_construct_n(_Iter _First, _Diff n) {
	if (!wjr::is_constant_evaluated()) {
		return std::uninitialized_default_construct_n(_First, n);
	}
	
	if (n <= 0) {
		return _First;
	}

	using value_type = iter_val_t<_Iter>;
	if constexpr(std::is_trivially_default_constructible_v<value_type>){
		std::advance(_First, n);
	}
	else {
		for (; n; ++_First, (void)--n) {
			wjr::construct_at(_First, default_construct_tag());
		}
	}
	return _First;

}

template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_default_construct_n(
	Alloc& al, _Iter _First, _Diff n) {
	if constexpr (is_default_allocator_construct_v<Alloc, _Iter, default_construct_tag>) {
		return wjr::uninitialized_default_construct_n(_First, n);
	}
	else {
		for (; n > 0; ++_First, (void)--n) {
			wjr::construct_at(al, _First, default_construct_tag{});
		}
		return _First;
	}
}

template<typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_value_construct(_Iter _First, _Iter _Last) {
	if (!wjr::is_constant_evaluated()) {
		return std::uninitialized_value_construct(_First, _Last);
	}
	for (; _First != _Last; ++_First) {
		wjr::construct_at(_First, value_construct_tag());
	}
}

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_value_construct(
	Alloc& al, _Iter _First, _Iter _Last) {
	if constexpr (is_default_allocator_construct_v<Alloc, _Iter, value_construct_tag>) {
		wjr::uninitialized_value_construct(_First, _Last);
	}
	else {
		for (; _First != _Last; (void)++_First) {
			wjr::construct_at(al, _First, value_construct_tag());
		}
	}
}

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_value_construct_n(_Iter _First, _Diff n) {
	if (!wjr::is_constant_evaluated()) {
		return std::uninitialized_value_construct_n(_First, n);
	}

	if (n <= 0) {
		return _First;
	}

	for (; n; ++_First, (void)--n) {
		wjr::construct_at(_First, value_construct_tag());
	}

	return _First;
}

template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_value_construct_n(
	Alloc& al, _Iter _First, _Diff n) {
	if constexpr (is_default_allocator_construct_v<Alloc, _Iter, value_construct_tag>) {
		return wjr::uninitialized_value_construct_n(_First, n);
	}
	else {
		for (; n > 0; ++_First, (void)--n) {
			wjr::construct_at(al, _First, value_construct_tag{});
		}
		return _First;
	}
}

template<typename _Input, typename _Output,
	typename _Input_ref = iter_ref_t<_Input>,
	typename _Output_ref = iter_ref_t<_Output>>
	struct __has_fast_uninitialized_copy : std::conjunction<
	is_contiguous_iter<_Input>,
	is_contiguous_iter<_Output>,
	std::bool_constant<wjr::is_reverse_iterator_v<_Input> == wjr::is_reverse_iterator_v<_Output>>,
	algo::__has_fast_construct_memcpy<remove_ref_t<_Output_ref>, _Input_ref>
	> {};

template<typename _Input, typename _Output>
constexpr bool __has_fast_uninitialized_copy_v = __has_fast_uninitialized_copy<_Input, _Output>::value;

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_copy(_Iter1 _First, _Iter1 _Last, _Iter2 _Dest) {
	if (!wjr::is_constant_evaluated()) {
		if constexpr (__has_fast_uninitialized_copy_v<_Iter1, _Iter2>) {
			const auto n = std::distance(_First, _Last);
			if (is_unlikely(n == 0)) { return _Dest; }
			if constexpr (!wjr::is_reverse_iterator_v<_Iter1>) {
				const auto first1 = wjr::get_address(_First);
				const auto first2 = wjr::get_address(_Dest);

				algo::construct_memcpy(first2, first1, n);
			}
			else {
				const auto first1 = wjr::get_address(_Last - 1);
				const auto _Last2 = _Dest + n;
				const auto first2 = wjr::get_address(_Last2 - 1);

				algo::construct_memmove(first2, first1, n);
			}
			return _Dest + n;
		}
		return std::uninitialized_copy(_First, _Last, _Dest);
	}

	for (; _First != _Last; ++_First, (void)++_Dest) {
		wjr::construct_at(_Dest, *_First);
	}

	return _Dest;
}

template<typename Alloc, typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_copy(
	Alloc& al, _Iter1 _First, _Iter1 _Last, _Iter2 _Dest) {
	if constexpr (is_default_allocator_construct_v<Alloc, _Iter2, decltype(*_First)>) {
		return wjr::uninitialized_copy(_First, _Last, _Dest);
	}
	else {
		for (; _First != _Last; ++_Dest, (void)++_First) {
			wjr::construct_at(al, _Dest, *_First);
		}
	}
}

template<typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_copy_n(_Iter1 _First, _Diff n, _Iter2 _Dest) {
	if (!wjr::is_constant_evaluated()) {
		if constexpr (__has_fast_uninitialized_copy_v<_Iter1, _Iter2>) {
			if (n <= 0) { return _Dest; }
			return wjr::uninitialized_copy(_First, _First + n, _Dest);
		}
		return std::uninitialized_copy_n(_First, n, _Dest);
	}

	if (n <= 0) {
		return _Dest;
	}

	for (; n; ++_First, (void)++_Dest, (void)--n) {
		wjr::construct_at(_Dest, *_First);
	}

	return _Dest;
}

template<typename Alloc, typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_copy_n(Alloc& al, _Iter1 _First, _Diff n, _Iter2 _Dest) {
	if constexpr (is_default_allocator_construct_v<Alloc, _Iter2, decltype(*_First)>) {
		return wjr::uninitialized_copy_n(_First, n, _Dest);
	}
	else {
		for (; n > 0; ++_First, (void)++_Dest, (void)--n) {
			wjr::construct_at(al, _Dest, *_First);
		}
		return _Dest;
	}
}

template<typename _Iter, typename _Val,
	typename _Iter_ref = iter_ref_t<_Iter>>
	struct __has_fast_uninitialized_fill : std::conjunction<
	is_contiguous_iter<_Iter>,
	algo::__has_fast_construct_memset<remove_cref_t<_Iter_ref>, add_cref_t<_Val>>
	> {};

template<typename _Iter, typename _Val>
constexpr bool __has_fast_uninitialized_fill_v = __has_fast_uninitialized_fill<_Iter, _Val>::value;

template<typename _Iter, typename _Val>
WJR_CONSTEXPR20 void do_uninitialized_fill(_Iter _First, _Iter _Last, const _Val& val) {
	if (!wjr::is_constant_evaluated()) {
		if constexpr (__has_fast_uninitialized_fill_v<_Iter, _Val>) {
			const auto n = static_cast<size_t>(std::distance(_First, _Last));
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
		return std::uninitialized_fill(_First, _Last, val);
	}

	for (; _First != _Last; ++_First) {
		wjr::construct_at(_First, val);
	}
}

template<typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_fill(_Iter _First, _Iter _Last, default_construct_tag) {
	wjr::uninitialized_default_construct(_First, _Last);
}

template<typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_fill(_Iter _First, _Iter _Last, value_construct_tag) {
	wjr::uninitialized_value_construct(_First, _Last);
}

template<typename Alloc, typename _Iter, typename _Val>
WJR_CONSTEXPR20 void do_uninitialized_fill(Alloc& al, _Iter _First, _Iter _Last, const _Val& val) {
	if constexpr (is_default_allocator_construct_v<Alloc, _Iter, _Val>) {
		wjr::uninitialized_fill(_First, _Last, val);
	}
	else {
		for (; _First != _Last; (void)++_First) {
			wjr::construct_at(al, _First, val);
		}
	}
}

template<typename _Iter, typename _Diff, typename _Val>
WJR_CONSTEXPR20 _Iter do_uninitialized_fill_n(_Iter _First, _Diff n, const _Val& val) {
	if (!wjr::is_constant_evaluated()) {
		if constexpr (__has_fast_uninitialized_fill_v<_Iter, _Val>) {
			if (n <= 0) { return _First; }
			wjr::uninitialized_fill(_First, _First + n, val);
			return _First + n;
		}
		return std::uninitialized_fill_n(_First, n, val);
	}

	if (n <= 0) {
		return _First;
	}

	for (; n; ++_First, (void)--n) {
		wjr::construct_at(_First, val);
	}

	return _First;
}

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_fill_n(_Iter _First, _Diff n, default_construct_tag) {
	return wjr::uninitialized_default_construct_n(_First, n);
}

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_fill_n(_Iter _First, _Diff n, value_construct_tag) {
	return wjr::uninitialized_value_construct_n(_First, n);
}

template<typename Alloc, typename _Iter, typename _Diff, typename _Val>
WJR_CONSTEXPR20 _Iter do_uninitialized_fill_n(Alloc& al, _Iter _First, _Diff n, const _Val& val) {
	if constexpr (is_default_allocator_construct_v<Alloc, _Iter, _Val>) {
		return wjr::uninitialized_fill_n(_First, n, val);
	}
	else {
		for (; n > 0; (void)++_First, --n) {
			wjr::construct_at(al, _First, val);
		}
	}
}

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_move(_Iter1 _First, _Iter1 _Last, _Iter2 _Dest) {
	return wjr::uninitialized_copy(
		std::make_move_iterator(_First),
		std::make_move_iterator(_Last),
		_Dest
	);
}

template<typename Alloc, typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_move(Alloc& al, _Iter1 _First, _Iter1 _Last, _Iter2 _Dest) {
	return wjr::uninitialized_copy(al,
		std::make_move_iterator(_First),
		std::make_move_iterator(_Last),
		_Dest
	);
}

template<typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> do_uninitialized_move_n(_Iter1 _First, _Diff n, _Iter2 _Dest) {
	if (!wjr::is_constant_evaluated()) {
		if constexpr (__has_fast_uninitialized_copy_v<_Iter1, std::move_iterator<_Iter1>>) {
			if (n <= 0) return std::make_pair(_First, _Dest);
			wjr::uninitialized_move(_First, _First + n, _Dest);
			return std::make_pair(_First + n, _Dest + n);
		}
		return std::uninitialized_move_n(_First, n, _Dest);
	}

	if (n <= 0) {
		return std::make_pair(_First, _Dest);
	}

	for (; n; ++_First, (void)++_Dest, (void)--n) {
		wjr::construct_at(_Dest, std::move(*_First));
	}

	return std::make_pair(_First, _Dest);
}

template<typename Alloc, typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> do_uninitialized_move_n(
	Alloc& al, _Iter1 _First, _Diff n, _Iter2 _Dest) {
	if constexpr (is_default_allocator_construct_v<Alloc, _Iter1, decltype(std::move(*_First))>) {
		return wjr::uninitialized_move_n(_First, n, _Dest);
	}
	else {
		for (; n > 0; ++_First, (void)++_Dest, (void)--n) {
			wjr::construct_at(al, _Dest, std::move(*_First));
		}
		return std::make_pair(_First, _Dest);
	}
}

template<typename _Iter, typename _Func>
WJR_CONSTEXPR20 void do_for_each(_Iter _First, _Iter _Last, _Func fn) {
	for (; _First != _Last; ++_First) {
		fn(*_First);
	}
}

template<typename _Iter, typename _SizeT, typename _Func>
WJR_CONSTEXPR20 void do_for_each_n(_Iter _First, _SizeT n, _Func fn) {
	if (n <= 0) {
		return;
	}

	for (; n; ++_First, (void)--n) {
		fn(*_First);
	}
}

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