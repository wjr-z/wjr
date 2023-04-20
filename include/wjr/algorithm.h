#pragma once
#ifndef __WJR_ALGORITHM_H
#define __WJR_ALGORITHM_H

// note that there may be some ADL conflicts in the GCC standard library, 
// such as uninitialized_copy, but sometimes we rely on standard library 
// implementations(Because sometimes the standard library uses some optimizations, 
// when my optimizations fail, I use the optimization of the standard library), 
// so we use functors to avoid conflicts

#include <wjr/type_traits.h>

_WJR_BEGIN

template<typename _Iter, typename _Ty, typename _Pred>
WJR_CONSTEXPR20 _Iter do_find(_Iter _First, _Iter _Last, const _Ty& _val, _Pred pred);

template<typename _Iter, typename _Ty>
WJR_CONSTEXPR20 _Iter do_find(_Iter _First, _Iter _Last, const _Ty& _val);

template<typename _Iter, typename _Pr>
WJR_CONSTEXPR20 _Iter do_find_if(_Iter _First, _Iter _Last, _Pr _Pred);

template<typename _Iter, typename _Pr>
WJR_CONSTEXPR20 _Iter do_find_if_not(_Iter _First, _Iter _Last, _Pr _Pred);

template<typename _Iter, typename _Ty>
WJR_CONSTEXPR20 typename std::iterator_traits<_Iter>::difference_type
do_count(_Iter _First, _Iter _Last, const _Ty& _Val);

template<typename _Iter, typename _Pr>
WJR_CONSTEXPR20 typename std::iterator_traits<_Iter>::difference_type
do_count_if(_Iter _First, _Iter _Last, _Pr _Pred);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> do_mismatch(
	_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> do_mismatch(
	_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> do_mismatch(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> do_mismatch(
	_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 bool do_equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 bool do_equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 bool do_equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 bool do_equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 bool do_lexicographical_compare(
	_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 bool do_lexicographical_compare(
	_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 int do_compare(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 int do_compare(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2);

template<typename _Iter, typename _Val>
WJR_CONSTEXPR20 void do_fill(_Iter _First, _Iter _Last, const _Val& value);

template<typename _Iter, typename _Size, typename _Val>
WJR_CONSTEXPR20 _Iter do_fill_n(_Iter _First, _Size count, const _Val& value);

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output do_copy(_Input _First1, _Input _Last1, _Output _First2);

template<typename _Input, typename _Size, typename _Output>
WJR_CONSTEXPR20 _Output do_copy_n(_Input _First1, _Size count, _Output _First2);

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output do_copy_backward(_Input _First1, _Input _Last1, _Output _Last2);

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output do_move(_Input _First1, _Input _Last1, _Output _First2);

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output do_move_backward(_Input _First1, _Input _Last1, _Output _Last2);

template<typename _Iter, typename _Pred = std::equal_to<>>
class default_searcher;

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter1 do_search(_Iter1 _First1, _Iter1 _Last1,
	_Iter2 _First2, _Iter2 _Last2);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 _Iter1 do_search(_Iter1 _First1, _Iter1 _Last1,
	_Iter2 _First2, _Iter2 _Last2, _Pred pred);

template<typename _Iter, typename _Searcher>
WJR_CONSTEXPR20 _Iter do_search(_Iter _First, _Iter _Last, const _Searcher& _Srch);

template<typename _Iter, typename...Args, 
	std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int> = 0>
WJR_CONSTEXPR20 void do_construct_at(_Iter iter, Args&&... args);

template<typename _Iter, std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int> = 0>
WJR_CONSTEXPR20 void do_construct_at(_Iter iter, default_construct_tag);

template<typename _Iter, std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int> = 0>
WJR_CONSTEXPR20 void do_construct_at(_Iter iter, value_construct_tag);

template<typename Alloc, typename _Iter, typename...Args, 
	std::enable_if_t<!is_iterator_v<wjr::remove_cvref_t<Alloc>>, int> = 0>
WJR_CONSTEXPR20 void do_construct_at(Alloc& al, _Iter iter, Args&&...args);

template<typename _Iter>
WJR_CONSTEXPR20 void do_destroy_at(_Iter ptr);

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void do_destroy_at(Alloc& al, _Iter iter);

template<typename _Iter>
WJR_CONSTEXPR20 void do_destroy(_Iter _First, _Iter _Last);

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void do_destroy(Alloc& al, _Iter _First, _Iter _Last);

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 void do_destroy_n(_Iter _First, const _Diff n);

template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 void do_destroy_n(Alloc& al, _Iter _First, _Diff n);

template<typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_default_construct(_Iter _First, _Iter _Last);

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_default_construct(
	Alloc& al, _Iter _First, _Iter _Last);

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_default_construct_n(_Iter _First, _Diff n);

template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_default_construct_n(
	Alloc& al, _Iter _First, _Diff n);

template<typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_value_construct(_Iter _First, _Iter _Last);

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_value_construct(
	Alloc& al, _Iter _First, _Iter _Last);

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_value_construct_n(_Iter _First, _Diff n);

template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_value_construct_n(
	Alloc& al, _Iter _First, _Diff n);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_copy(_Iter1 _First, _Iter1 _Last, _Iter2 _Dest);

template<typename Alloc, typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_copy(
	Alloc& al, _Iter1 _First, _Iter1 _Last, _Iter2 _Dest);

template<typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_copy_n(_Iter1 _First, _Diff n, _Iter2 _Dest);

template<typename Alloc, typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_copy_n(Alloc& al, _Iter1 _First, _Diff n, _Iter2 _Dest);


template<typename _Iter, typename _Val>
WJR_CONSTEXPR20 void do_uninitialized_fill(_Iter _First, _Iter _Last, const _Val& val);

template<typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_fill(_Iter _First, _Iter _Last, default_construct_tag);

template<typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_fill(_Iter _First, _Iter _Last, value_construct_tag);

template<typename Alloc, typename _Iter, typename _Val>
WJR_CONSTEXPR20 void do_uninitialized_fill(Alloc& al, _Iter _First, _Iter _Last, const _Val& val);

template<typename _Iter, typename _Diff, typename _Val>
WJR_CONSTEXPR20 _Iter do_uninitialized_fill_n(_Iter _First, _Diff count, const _Val& val);

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_fill_n(_Iter _First, _Diff n, default_construct_tag);

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_fill_n(_Iter _First, _Diff n, value_construct_tag);

template<typename Alloc, typename _Iter, typename _Diff, typename _Val>
WJR_CONSTEXPR20 _Iter do_uninitialized_fill_n(Alloc& al, _Iter _First, _Diff n, const _Val& val);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_move(_Iter1 _First, _Iter1 _Last, _Iter2 _Dest);

template<typename Alloc, typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_move(Alloc& al, _Iter1 _First, _Iter1 _Last, _Iter2 _Dest);

template<typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> do_uninitialized_move_n(_Iter1 _First, _Diff n, _Iter2 _Dest);

template<typename Alloc, typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> do_uninitialized_move_n(
	Alloc& al, _Iter1 _First, _Diff n, _Iter2 _Dest);

template<typename _Iter, typename _Func>
WJR_CONSTEXPR20 void do_for_each(_Iter _First, _Iter _Last, _Func fn);

template<typename _Iter, typename _SizeT, typename _Func>
WJR_CONSTEXPR20 void do_for_each_n(_Iter _First, _SizeT n, _Func fn);

template<typename Alloc>
class temporary_allocator_value;

#define __WJR_REGISTER_ALGO_FUNCTOR(name)	                        \
struct name##_fn{	                                                \
	template<typename...Args>	                                    \
	WJR_CONSTEXPR20 decltype(auto) operator()(Args&&...args) const{	\
		return do_##name(std::forward<Args>(args)...);				\
	}			                                                    \
};	                                                                \
inline constexpr name##_fn name;

__WJR_REGISTER_ALGO_FUNCTOR(find);
__WJR_REGISTER_ALGO_FUNCTOR(find_if);
__WJR_REGISTER_ALGO_FUNCTOR(find_if_not);
__WJR_REGISTER_ALGO_FUNCTOR(count);
__WJR_REGISTER_ALGO_FUNCTOR(count_if);
__WJR_REGISTER_ALGO_FUNCTOR(mismatch);
__WJR_REGISTER_ALGO_FUNCTOR(equal);
__WJR_REGISTER_ALGO_FUNCTOR(lexicographical_compare);
__WJR_REGISTER_ALGO_FUNCTOR(compare);
__WJR_REGISTER_ALGO_FUNCTOR(fill);
__WJR_REGISTER_ALGO_FUNCTOR(fill_n);
__WJR_REGISTER_ALGO_FUNCTOR(copy);
__WJR_REGISTER_ALGO_FUNCTOR(copy_n);
__WJR_REGISTER_ALGO_FUNCTOR(copy_backward);
__WJR_REGISTER_ALGO_FUNCTOR(move);
__WJR_REGISTER_ALGO_FUNCTOR(move_backward);
__WJR_REGISTER_ALGO_FUNCTOR(search);
__WJR_REGISTER_ALGO_FUNCTOR(construct_at);
__WJR_REGISTER_ALGO_FUNCTOR(destroy_at);
__WJR_REGISTER_ALGO_FUNCTOR(destroy);
__WJR_REGISTER_ALGO_FUNCTOR(destroy_n);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_default_construct);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_default_construct_n);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_value_construct);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_value_construct_n);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_copy);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_copy_n);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_fill);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_fill_n);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_move);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_move_n);
__WJR_REGISTER_ALGO_FUNCTOR(for_each);
__WJR_REGISTER_ALGO_FUNCTOR(for_each_n);

#undef __WJR_REGISTER_ALGO_FUNCTOR

_WJR_END

#include <wjr/algorithm-inl.h>

#endif // __WJR_ALGORITHM_H