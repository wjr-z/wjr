#ifndef __WJR_CIRCULAR_BUFFER_H
#error "This file should not be included directly. Include <wjr/circular_buffer.h> instead."
#endif // __WJR_CIRCULAR_BUFFER_H

// algorithm

_WJR_BEGIN

template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_copy(
	circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest) {
	const auto ptr = _First.ptr();
	const auto head_pos = _First.pos();
	const auto tail_pos = _Last.pos();
	const auto c = _First.capacity();
	if (tail_pos < c) {
		return wjr::uninitialized_copy(ptr + head_pos, ptr + tail_pos, _Dest);
	}
	return wjr::uninitialized_copy(
		ptr,
		ptr + tail_pos - c,
		wjr::uninitialized_copy(
			ptr + head_pos,
			ptr + c,
			_Dest
		)
	);
}

template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_copy(
	circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest) {
	return wjr::uninitialized_copy(_First.base(), _Last.base(), _Dest);
}

template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_copy(Alloc& al,
	circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest) {
	const auto ptr = _First.ptr();
	const auto head_pos = _First.pos();
	const auto tail_pos = _Last.pos();
	const auto c = _First.capacity();
	if (tail_pos < c) {
		return wjr::uninitialized_copy(al, ptr + head_pos, ptr + tail_pos, _Dest);
	}
	return wjr::uninitialized_copy(al,
		ptr,
		ptr + tail_pos - c,
		wjr::uninitialized_copy(al,
			ptr + head_pos,
			ptr + c,
			_Dest
		)
	);
}

template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_copy(Alloc& al,
	circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest) {
	return wjr::uninitialized_copy(al, _First.base(), _Last.base(), _Dest);
}

template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_move(
	circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest) {
	const auto ptr = _First.ptr();
	const auto head_pos = _First.pos();
	const auto tail_pos = _Last.pos();
	const auto c = _First.capacity();
	if (tail_pos < c) {
		return wjr::uninitialized_move(ptr + head_pos, ptr + tail_pos, _Dest);
	}
	return wjr::uninitialized_move(
		ptr,
		ptr + tail_pos - c,
		wjr::uninitialized_move(
			ptr + head_pos,
			ptr + c,
			_Dest
		)
	);
}

template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_move(
	circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest) {
	return wjr::uninitialized_move(_First.base(), _Last.base(), _Dest);
}

template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_move(Alloc& al,
	circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest) {
	const auto ptr = _First.ptr();
	const auto head_pos = _First.pos();
	const auto tail_pos = _Last.pos();
	const auto c = _First.capacity();
	if (tail_pos < c) {
		return wjr::uninitialized_move(al, ptr + head_pos, ptr + tail_pos, _Dest);
	}
	return wjr::uninitialized_move(al,
		ptr,
		ptr + tail_pos - c,
		wjr::uninitialized_move(al,
			ptr + head_pos,
			ptr + c,
			_Dest
		)
	);
}

template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_move(Alloc& al,
	circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest) {
	return wjr::uninitialized_move(al, _First.base(), _Last.base(), _Dest);
}

_WJR_END