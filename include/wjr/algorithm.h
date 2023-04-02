#pragma once
#ifndef __WJR_ALGORITHM_H
#define __WJR_ALGORITHM_H

#include <algorithm>
#include <memory>
#include <limits>
#include <cstring>

#include <wjr/type_traits.h>

_WJR_BEGIN

template<typename _Iter, typename _Ty, typename _Pred>
WJR_CONSTEXPR20 _Iter find(_Iter _First, _Iter _Last, const _Ty& _val, _Pred pred);

template<typename _Iter, typename _Ty>
WJR_CONSTEXPR20 _Iter find(_Iter _First, _Iter _Last, const _Ty& _val);

template<typename _Iter, typename _Pr>
WJR_CONSTEXPR20 _Iter find_if(_Iter _First, _Iter _Last, _Pr _Pred);

template<typename _Iter, typename _Pr>
WJR_CONSTEXPR20 _Iter find_if_not(_Iter _First, _Iter _Last, _Pr _Pred);

template<typename _Iter, typename _Ty>
WJR_CONSTEXPR20 typename std::iterator_traits<_Iter>::difference_type
count(_Iter _First, _Iter _Last, const _Ty& _Val);

template<typename _Iter, typename _Pr>
WJR_CONSTEXPR20 typename std::iterator_traits<_Iter>::difference_type
count_if(_Iter _First, _Iter _Last, _Pr _Pred);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> mismatch(
	_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> mismatch(
	_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> mismatch(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> mismatch(
	_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 bool equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 bool equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 bool equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 bool equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 bool lexicographical_compare(
	_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 bool lexicographical_compare(
	_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 int compare(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 int compare(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2);

template<typename _Iter, typename _Val>
WJR_CONSTEXPR20 void fill(_Iter _First, _Iter _Last, const _Val& value);

template<typename _Iter, typename _Size, typename _Val>
WJR_CONSTEXPR20 _Iter fill_n(_Iter _First, _Size count, const _Val& value);

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output copy(_Input _First1, _Input _Last1, _Output _First2);

template<typename _Input, typename _Size, typename _Output>
WJR_CONSTEXPR20 _Output copy_n(_Input _First1, _Size count, _Output _First2);

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output copy_backward(_Input _First1, _Input _Last1, _Output _Last2);

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output move(_Input _First1, _Input _Last1, _Output _First2);

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output move_backward(_Input _First1, _Input _Last1, _Output _Last2);

template<typename _Iter, typename _Pred = std::equal_to<>>
class default_searcher;

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter1 search(_Iter1 _First1, _Iter1 _Last1,
	_Iter2 _First2, _Iter2 _Last2);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 _Iter1 search(_Iter1 _First1, _Iter1 _Last1,
	_Iter2 _First2, _Iter2 _Last2, _Pred pred);

template<typename _Iter, typename _Searcher>
WJR_CONSTEXPR20 _Iter search(_Iter _First, _Iter _Last, const _Searcher& _Srch);

template<typename _Iter, typename...Args, std::enable_if_t<is_iterator_v<_Iter>, int> = 0>
WJR_CONSTEXPR20 void construct_at(_Iter iter, Args&&... args);

template<typename _Iter, std::enable_if_t<is_iterator_v<_Iter>, int> = 0>
WJR_CONSTEXPR20 void construct_at(_Iter iter, default_construct_tag);

template<typename _Iter, std::enable_if_t<is_iterator_v<_Iter>, int> = 0>
WJR_CONSTEXPR20 void construct_at(_Iter iter, value_construct_tag);

template<typename Alloc, typename _Iter, typename...Args, std::enable_if_t<!is_iterator_v<Alloc>, int> = 0>
WJR_CONSTEXPR20 void construct_at(Alloc& al, _Iter iter, Args&&...args);

template<typename _Iter>
WJR_CONSTEXPR20 void destroy_at(_Iter ptr);

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void destroy_at(Alloc& al, _Iter iter);

template<typename _Iter>
WJR_CONSTEXPR20 void destroy(_Iter _First, _Iter _Last);

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void destroy(Alloc& al, _Iter _First, _Iter _Last);

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 void destroy_n(_Iter _First, const _Diff n);

template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 void destroy_n(Alloc& al, _Iter _First, _Diff n);

template<typename _Iter>
WJR_CONSTEXPR20 void uninitialized_default_construct(_Iter _First, _Iter _Last);

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void uninitialized_default_construct(
	Alloc& al, _Iter _First, _Iter _Last);

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter uninitialized_default_construct_n(_Iter _First, const _Diff n);

template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter uninitialized_default_construct_n(
	Alloc& al, _Iter _First, _Diff n);

template<typename _Iter>
WJR_CONSTEXPR20 void uninitialized_value_construct(_Iter _First, _Iter _Last);

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void uninitialized_value_construct(
	Alloc& al, _Iter _First, _Iter _Last);

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter uninitialized_value_construct_n(_Iter _First, _Diff n);

template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter uninitialized_value_construct_n(
	Alloc& al, _Iter _First, _Diff n);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 uninitialized_copy(_Iter1 _First, _Iter1 _Last, _Iter2 _Dest);

template<typename Alloc, typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 uninitialized_copy(
	Alloc& al, _Iter1 _First, _Iter1 _Last, _Iter2 _Dest);

template<typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 uninitialized_copy_n(_Iter1 _First, _Diff n, _Iter2 _Dest);

template<typename Alloc, typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 uninitialized_copy_n(Alloc& al, _Iter1 _First, _Diff n, _Iter2 _Dest);


template<typename _Iter, typename _Val>
WJR_CONSTEXPR20 void uninitialized_fill(_Iter _First, _Iter _Last, const _Val& val);

template<typename _Iter>
WJR_CONSTEXPR20 void uninitialized_fill(_Iter _First, _Iter _Last, default_construct_tag);

template<typename _Iter>
WJR_CONSTEXPR20 void uninitialized_fill(_Iter _First, _Iter _Last, value_construct_tag);

template<typename Alloc, typename _Iter, typename _Val>
WJR_CONSTEXPR20 void uninitialized_fill(Alloc& al, _Iter _First, _Iter _Last, const _Val& val);

template<typename _Iter, typename _Diff, typename _Val>
WJR_CONSTEXPR20 _Iter uninitialized_fill_n(_Iter _First, _Diff count, const _Val& val);

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter uninitialized_fill_n(_Iter _First, _Diff n, default_construct_tag);

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter uninitialized_fill_n(_Iter _First, _Diff n, value_construct_tag);

template<typename Alloc, typename _Iter, typename _Diff, typename _Val>
WJR_CONSTEXPR20 _Iter uninitialized_fill_n(Alloc& al, _Iter _First, _Diff n, const _Val& val);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 uninitialized_move(_Iter1 _First, _Iter1 _Last, _Iter2 _Dest);

template<typename Alloc, typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 uninitialized_move(Alloc& al, _Iter1 _First, _Iter1 _Last, _Iter2 _Dest);

template<typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> uninitialized_move_n(_Iter1 _First, _Diff n, _Iter2 _Dest);

template<typename Alloc, typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> uninitialized_move_n(
	Alloc& al, _Iter1 _First, _Diff n, _Iter2 _Dest);

template<typename _Iter, typename _Func>
WJR_CONSTEXPR20 void for_each(_Iter _First, _Iter _Last, _Func fn);

template<typename _Iter, typename _SizeT, typename _Func>
WJR_CONSTEXPR20 void for_each_n(_Iter _First, _SizeT n, _Func fn);

template<typename Alloc>
class temporary_allocator_value;

_WJR_END

#include <wjr/algorithm-inl.h>

#endif // __WJR_ALGORITHM_H