#pragma once
#ifndef __WJR_CIRCULAR_BUFFER_H
#define __WJR_CIRCULAR_BUFFER_H

#include <wjr/type_traits.h>
#include <wjr/algorithm.h>
#include <wjr/compressed_pair.h>

_WJR_BEGIN

// TODO

template<typename T>
class circular_buffer_const_iterator {
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = const T*;
	using reference = const T&;

	WJR_CONSTEXPR20 circular_buffer_const_iterator() noexcept = default;
	WJR_CONSTEXPR20 circular_buffer_const_iterator(const circular_buffer_const_iterator&) noexcept = default;
	WJR_CONSTEXPR20 circular_buffer_const_iterator& 
		operator=(const circular_buffer_const_iterator&) noexcept = default;
	WJR_CONSTEXPR20 circular_buffer_const_iterator(pointer m_ptr, size_t m_pos, size_t m_capacity) noexcept
		: m_ptr(m_ptr), m_pos(m_pos), m_capacity(m_capacity) {}

	WJR_NODISCARD WJR_CONSTEXPR20 reference operator*() const noexcept {
		return m_ptr[offset()];
	}

	WJR_NODISCARD WJR_CONSTEXPR20 pointer operator->() const noexcept {
		return m_ptr + offset();
	}

	WJR_CONSTEXPR20 circular_buffer_const_iterator& operator++() noexcept {
		assume_no_add_overflow(m_pos, static_cast<size_t>(1));
		++m_pos;
		return *this;
	}

	WJR_CONSTEXPR20 circular_buffer_const_iterator operator++(int)noexcept {
		circular_buffer_const_iterator tmp = *this;
		++(*this);
		return tmp;
	}

	WJR_CONSTEXPR20 circular_buffer_const_iterator& operator--() noexcept {
		assume_no_sub_overflow(m_pos, static_cast<size_t>(1));
		--m_pos;
		return *this;
	}

	WJR_CONSTEXPR20 circular_buffer_const_iterator operator--(int)noexcept {
		circular_buffer_const_iterator tmp = *this;
		--(*this);
		return tmp;
	}

	WJR_CONSTEXPR20 circular_buffer_const_iterator& operator+=(const difference_type n) noexcept {
		assume_no_add_overflow(m_pos, static_cast<size_t>(n));
		m_pos += n;
		return *this;
	}

	WJR_CONSTEXPR20 circular_buffer_const_iterator operator+(const difference_type n) const noexcept {
		circular_buffer_const_iterator tmp = *this;
		tmp += n;
		return tmp;
	}

	WJR_CONSTEXPR20 circular_buffer_const_iterator& operator-=(const difference_type n) noexcept {
		(*this) += (-n);
		return *this;
	}

	WJR_CONSTEXPR20 circular_buffer_const_iterator operator-(const difference_type n) const noexcept {
		circular_buffer_const_iterator tmp = *this;
		tmp -= n;
		return tmp;
	}

	WJR_CONSTEXPR20 friend difference_type operator-(
		const circular_buffer_const_iterator& lhs, 
		const circular_buffer_const_iterator& rhs
		) noexcept {
		return lhs.m_pos - rhs.m_pos;
	}

	WJR_NODISCARD WJR_CONSTEXPR20 reference operator[](const difference_type n) const noexcept {
		return *(*this + n);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator==(
		const circular_buffer_const_iterator& lhs,
		const circular_buffer_const_iterator& rhs
		) noexcept {
		return lhs.m_pos == rhs.m_pos;
	}

	WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator!=(
		const circular_buffer_const_iterator& lhs,
		const circular_buffer_const_iterator& rhs
		) noexcept {
		return !(lhs == rhs);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator<(
		const circular_buffer_const_iterator& lhs,
		const circular_buffer_const_iterator& rhs
		) noexcept {
		return lhs.m_pos < rhs.m_pos;
	}

	WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator<=(
		const circular_buffer_const_iterator& lhs,
		const circular_buffer_const_iterator& rhs
		) noexcept {
		return !(rhs < lhs);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator>(
		const circular_buffer_const_iterator& lhs,
		const circular_buffer_const_iterator& rhs
		) noexcept {
		return rhs < lhs;
	}

	WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator>=(
		const circular_buffer_const_iterator& lhs,
		const circular_buffer_const_iterator& rhs
		) noexcept {
		return !(lhs < rhs);
	}

	WJR_CONSTEXPR20 pointer ptr() const noexcept {
		return m_ptr;
	}

	WJR_CONSTEXPR20 size_t pos() const noexcept {
		return m_pos;
	}

	WJR_CONSTEXPR20 size_t offset() const noexcept {
		return m_pos < m_capacity ? m_pos : m_pos - m_capacity;
	}

	WJR_CONSTEXPR20 size_t capacity() const noexcept {
		return m_capacity;
	}

private:
	pointer m_ptr = nullptr;
	size_t m_pos = 0;
	size_t m_capacity = 0;
};

template<typename T>
class circular_buffer_iterator : public circular_buffer_const_iterator<T> {
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = T*;
	using reference = T&;

	using _Mybase = circular_buffer_const_iterator<T>;

	WJR_CONSTEXPR20 circular_buffer_iterator() noexcept = default;
	WJR_CONSTEXPR20 circular_buffer_iterator(const circular_buffer_iterator&) noexcept = default;
	WJR_CONSTEXPR20 circular_buffer_iterator&
		operator=(const circular_buffer_iterator&) noexcept = default;
	WJR_CONSTEXPR20 circular_buffer_iterator(pointer m_ptr, size_t m_pos, size_t m_capacity) noexcept
		: _Mybase(m_ptr, m_pos, m_capacity) {}

	WJR_NODISCARD WJR_CONSTEXPR20 reference operator*() const noexcept {
		return const_cast<reference>(_Mybase::operator*());
	}

	WJR_NODISCARD WJR_CONSTEXPR20 pointer operator->() const noexcept {
		return const_cast<pointer>(_Mybase::operator->());
	}

	WJR_CONSTEXPR20 circular_buffer_iterator& operator++() noexcept {
		_Mybase::operator++();
		return *this;
	}

	WJR_CONSTEXPR20 circular_buffer_iterator operator++(int)noexcept {
		circular_buffer_iterator tmp = *this;
		++(*this);
		return tmp;
	}

	WJR_CONSTEXPR20 circular_buffer_iterator& operator--() noexcept {
		_Mybase::operator--();
		return *this;
	}

	WJR_CONSTEXPR20 circular_buffer_iterator operator--(int)noexcept {
		circular_buffer_iterator tmp = *this;
		--(*this);
		return tmp;
	}

	WJR_CONSTEXPR20 circular_buffer_iterator& operator+=(const difference_type n) noexcept {
		_Mybase::operator+=(n);
		return *this;
	}

	WJR_CONSTEXPR20 circular_buffer_iterator operator+(const difference_type n) const noexcept {
		circular_buffer_iterator tmp = *this;
		tmp += n;
		return tmp;
	}

	WJR_CONSTEXPR20 circular_buffer_iterator& operator-=(const difference_type n) noexcept {
		(*this) += (-n);
		return *this;
	}

	WJR_CONSTEXPR20 circular_buffer_iterator operator-(const difference_type n) const noexcept {
		circular_buffer_iterator tmp = *this;
		tmp -= n;
		return tmp;
	}

	WJR_CONSTEXPR20 friend difference_type operator-(
		const circular_buffer_iterator& lhs,
		const circular_buffer_iterator& rhs
		) noexcept {
		return lhs.pos() - rhs.pos();
	}

	WJR_NODISCARD WJR_CONSTEXPR20 reference operator[](const difference_type n) const noexcept {
		return *(*this + n);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator==(
		const circular_buffer_iterator& lhs,
		const circular_buffer_iterator& rhs
		) noexcept {
		return lhs.pos() == rhs.pos();
	}

	WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator!=(
		const circular_buffer_iterator& lhs,
		const circular_buffer_iterator& rhs
		) noexcept {
		return !(lhs == rhs);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator<(
		const circular_buffer_iterator& lhs,
		const circular_buffer_iterator& rhs
		) noexcept {
		return lhs.pos() < rhs.pos();
	}

	WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator<=(
		const circular_buffer_iterator& lhs,
		const circular_buffer_iterator& rhs
		) noexcept {
		return !(rhs < lhs);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator>(
		const circular_buffer_iterator& lhs,
		const circular_buffer_iterator& rhs
	) noexcept {
		return rhs < lhs;
	}

	WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator>=(
		const circular_buffer_iterator& lhs,
		const circular_buffer_iterator& rhs
		) noexcept {
		return !(lhs < rhs);
	}

	WJR_NODISCARD WJR_CONSTEXPR20 _Mybase& base() {
		return *this;
	}

	WJR_NODISCARD WJR_CONSTEXPR20 const _Mybase& base() const {
		return *this;
	}
};

// algorithm overload

template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_copy(
	circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest);

template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_copy(
	circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest);

template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_copy(Alloc& al,
	circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest);

template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_copy(Alloc& al,
	circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest);

template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_move(
	circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest);

template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_move(
	circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest);

template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_move(Alloc& al,
	circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest);

template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_move(Alloc& al,
	circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest);

template<typename T, typename Alloc = std::allocator<T>>
class circular_buffer {
	using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using _Alty_traits = std::allocator_traits<_Alty>;
public:
	using value_type = T;
	using allocator_type = Alloc;
	using size_type = typename _Alty_traits::size_type;
	using difference_type = typename _Alty_traits::difference_type;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = typename _Alty_traits::pointer;
	using const_pointer = typename _Alty_traits::const_pointer;
	using iterator = circular_buffer_iterator<T>;
	using const_iterator = circular_buffer_const_iterator<T>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
	struct __data {

		WJR_INTRINSIC_CONSTEXPR20 void reset() noexcept {
			_Myptr = nullptr;
			_Myhead = _Mytail = _Mysize = _Mycapacity = 0;
		}

		pointer _Myptr = nullptr; 
		size_type _Myhead = 0;
		size_type _Mytail = 0;
		size_type _Mysize = 0;
		size_type _Mycapacity = 0;
	};
public:
	WJR_CONSTEXPR20 circular_buffer() 
		noexcept(std::is_nothrow_default_constructible_v<_Alty>) = default;
	WJR_CONSTEXPR20 circular_buffer(const allocator_type& al)
		noexcept(std::is_nothrow_constructible_v<_Alty, const allocator_type&>)
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(al),
			std::forward_as_tuple()) {}

	template<typename _Alloc>
	WJR_CONSTEXPR20 circular_buffer(const circular_buffer& other, _Alloc&& _Al, disable_tag)
		noexcept(std::is_nothrow_constructible_v<_Alty, _Alloc&&>)
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(_Al),
			std::forward_as_tuple()){
		auto& al = getAllocator();
		auto& _Mydata = getData();
		const auto& _Otherdata = other.getData();
		if (_Otherdata._Mysize != 0) {
			const auto _Newcapacity = getGrowthCapacity(0, _Otherdata._Mysize);
			_Mydata._Myptr = _Alty_traits::allocate(al, _Newcapacity);
			_Mydata._Myhead = 0;
			_Mydata._Mytail = _Otherdata._Mysize;
			_Mydata._Mysize = _Otherdata._Mysize;
			_Mydata._Mycapacity = _Newcapacity;
			wjr::uninitialized_copy(other.begin(), other.end(), _Mydata._Myptr);
		}
	}

	WJR_CONSTEXPR20 circular_buffer(const circular_buffer& other)
		: circular_buffer(other, _Alty_traits::select_on_container_copy_construction(other.getAllocator()),
			disable_tag{}) {}
	
	WJR_CONSTEXPR20 circular_buffer(const circular_buffer& other, const allocator_type& al)
		: circular_buffer(other, al, disable_tag{}) {}
	
	WJR_CONSTEXPR20 circular_buffer(circular_buffer&& other)
		noexcept(std::is_nothrow_move_constructible_v<_Alty>)
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(std::move(other.getAllocator())),
			std::forward_as_tuple(other.getData())) {
		other.getData().reset();
	}

	WJR_CONSTEXPR20 circular_buffer& operator=(const circular_buffer&) = delete;

	WJR_CONSTEXPR20 circular_buffer& operator=(circular_buffer&&) = delete;

	template<typename...Args>
	WJR_INLINE_CONSTEXPR20 decltype(auto) emplace_back(Args&&...args) {
		auto& al = getAllocator();
		auto& _Mydata = getData();
		if (WJR_LIKELY(_Mydata._Mysize != _Mydata._Mycapacity)) {
			wjr::construct_at(al, _Mydata._Myptr + _Mydata._Mytail, std::forward<Args>(args)...);
			++_Mydata._Mytail;
			_Mydata._Mytail = _Mydata._Mytail == _Mydata._Mycapacity ? 0 : _Mydata._Mytail;
			++_Mydata._Mysize;
		}
		else {
			_M_realloc_insert_at_end(std::forward<Args>(args)...);
		}
		return back();
	}

	WJR_INLINE_CONSTEXPR20 void push_back(const value_type& val) { emplace_back(val); }
	WJR_INLINE_CONSTEXPR20 void push_back(value_type&& val) { emplace_back(std::move(val)); }

	template<typename...Args>
	WJR_INLINE_CONSTEXPR20 decltype(auto) emplac_front(Args&&...args) {
		auto& al = getAllocator();
		auto& _Mydata = getData();
		if (WJR_LIKELY(_Mydata._Mysize != _Mydata._Mycapacity)) {
			_Mydata._Myhead = _Mydata._Myhead == 0 ? _Mydata._Mycapacity : _Mydata._Myhead;
			--_Mydata._Myhead;
			wjr::construct_at(al, _Mydata._Myptr + _Mydata._Myhead, std::forward<Args>(args)...);
			++_Mydata._Mysize;
		}
		else {
			_M_realloc_insert_at_begin(std::forward<Args>(args)...);
		}
		return back();
	}

	WJR_INLINE_CONSTEXPR20 void push_front(const value_type& val) { emplace_front(val); }
	WJR_INLINE_CONSTEXPR20 void push_front(value_type&& val) { emplace_front(std::move(val)); }

	WJR_INTRINSIC_CONSTEXPR20 void pop_front() noexcept {
		auto& al = getAllocator();
		auto& _Mydata = getData();
		wjr::destroy_at(al, _Mydata._Myptr + _Mydata._Myhead);
		++_Mydata._Myhead;
		_Mydata._Myhead = _Mydata._Myhead == _Mydata._Mycapacity ? 0 : _Mydata._Myhead;
		--_Mydata._Mysize;
	}

	WJR_INTRINSIC_CONSTEXPR20 void pop_back() noexcept {
		auto& al = getAllocator();
		auto& _Mydata = getData();
		_Mydata._Mytail = _Mydata._Mytail == 0 ? _Mydata._Mycapacity : _Mydata._Mytail;
		--_Mydata._Mytail;
		wjr::destroy_at(al, _Mydata._Myptr + _Mydata._Mytail);
		--_Mydata._Mysize;
	}

	WJR_INTRINSIC_CONSTEXPR20 reference front() noexcept {
		const auto& _Mydata = getData();
		return _Mydata._Myptr[_Mydata._Myhead];
	}
	WJR_INTRINSIC_CONSTEXPR20 const_reference front() const noexcept {
		const auto& _Mydata = getData();
		return _Mydata._Myptr[_Mydata._Myhead];
	}

	WJR_INTRINSIC_CONSTEXPR20 reference back() noexcept {
		const auto& _Mydata = getData();
		const auto __offset = _Mydata._Mytail == 0 ? _Mydata._Mycapacity : _Mydata._Mytail;
		return _Mydata._Myptr[__offset - 1];
	}
	WJR_INTRINSIC_CONSTEXPR20 const_reference back() const noexcept {
		const auto& _Mydata = getData();
		const auto __offset = _Mydata._Mytail == 0 ? _Mydata._Mycapacity : _Mydata._Mytail;
		return _Mydata._Myptr[__offset - 1];
	}

	WJR_INTRINSIC_CONSTEXPR20 size_type size() const noexcept { return getData()._Mysize; }
	WJR_INTRINSIC_CONSTEXPR20 size_type capacity() const noexcept { return getData()._Mycapacity; }

	WJR_INTRINSIC_CONSTEXPR20 bool empty() const noexcept { return size() == 0; }

	WJR_CONSTEXPR20 void clear() {
		auto& al = getAllocator();
		auto& _Mydata = getData();
		if (_Mydata._Mysize) {
			wjr::destroy(al, begin(), end());
			_Mydata._Myhead = _Mydata._Mytail = _Mydata._Mysize = 0;
		}
	}

	WJR_INTRINSIC_CONSTEXPR20 iterator begin() {
		const auto& _Mydata = getData();
		return iterator(_Mydata._Myptr, _Mydata._Myhead, _Mydata._Mycapacity);
	}

	WJR_INTRINSIC_CONSTEXPR20 const_iterator begin() const {
		const auto& _Mydata = getData();
		return iterator(_Mydata._Myptr, _Mydata._Myhead, _Mydata._Mycapacity);
	}

	WJR_INTRINSIC_CONSTEXPR20 const_iterator cbegin() const {
		return begin();
	}

	WJR_INTRINSIC_CONSTEXPR20 iterator end() {
		return begin() + size();
	}

	WJR_INTRINSIC_CONSTEXPR20 const_iterator end() const {
		return begin() + size();
	}

	WJR_INTRINSIC_CONSTEXPR20 const_iterator cend() const {
		return end();
	}

	WJR_INTRINSIC_CONSTEXPR20 reverse_iterator rbegin() {
		return reverse_iterator(end());
	}

	WJR_INTRINSIC_CONSTEXPR20 const_reverse_iterator rbegin() const {
		return const_reverse_iterator(end());
	}

	WJR_INTRINSIC_CONSTEXPR20 const_reverse_iterator crbegin() const {
		return const_reverse_iterator(end());
	}

	WJR_INTRINSIC_CONSTEXPR20 reverse_iterator rend() {
		return reverse_iterator(begin());
	}

	WJR_INTRINSIC_CONSTEXPR20 const_reverse_iterator rend() const {
		return const_reverse_iterator(begin());
	}

	WJR_INTRINSIC_CONSTEXPR20 const_reverse_iterator crend() const {
		return const_reverse_iterator(begin());
	}

	WJR_INTRINSIC_CONSTEXPR20 reference operator[](size_type pos) {
		return *(begin() + pos);
	}

	WJR_INTRINSIC_CONSTEXPR20 const_reference operator[](size_type pos) const {
		return *(begin() + pos);
	}

	WJR_CONSTEXPR20 void tidy() {
		auto& al = getAllocator();
		auto& _Mydata = getData();
		if (_Mydata._Myptr) {
			clear();
			_Alty_traits::deallocate(al, _Mydata._Myptr, _Mydata._Mycapacity);
			_Mydata._Myptr = nullptr;
			_Mydata._Mycapacity = 0;
		}
	}

	WJR_INTRINSIC_CONSTEXPR20 _Alty& getAllocator() noexcept { return _Myval.first(); }
	WJR_INTRINSIC_CONSTEXPR20 const _Alty& getAllocator() const noexcept { return _Myval.first(); }
	WJR_INTRINSIC_CONSTEXPR20 __data& getData() noexcept { return _Myval.second(); }
	WJR_INTRINSIC_CONSTEXPR20 const __data& getData() const noexcept { return _Myval.second(); }

	template<typename _Iter, std::enable_if_t<is_iterator_v<_Iter>, int> = 0>
	WJR_CONSTEXPR20 circular_buffer& append(_Iter _First, _Iter _Last) {
		_M_range_append(_First, _Last, typename std::iterator_traits<_Iter>::iterator_category());
		return *this;
	}

private:

	WJR_CONSTEXPR20 static size_type getGrowthCapacity(
		const size_type _Oldcapacity, const size_type _Newsize) noexcept {
		WJR_ASSUME(_Newsize > _Oldcapacity);
		const auto _Newcapacity = _Oldcapacity + _Oldcapacity / 2;
		const auto _FixedCapacity = _Newcapacity < _Newsize ? _Newsize : _Newcapacity;
		const auto ret = _FixedCapacity < 32 ? ((_FixedCapacity + 3) & (-4)) :
			_FixedCapacity < 256 ? ((_FixedCapacity + 15) & (-16)) :
			((_FixedCapacity + 63) & (-64));
		WJR_ASSUME(ret >= _Newsize);
		return ret;
	}

	template<typename...Args>
	WJR_CONSTEXPR20 void _M_realloc_insert_at_begin(Args&&...args) {
		auto& al = getAllocator();
		auto& _Mydata = getData();
		const auto _Oldsize = _Mydata._Mysize;
		WJR_ASSUME(_Mydata._Mysize == _Mydata._Mycapacity);
		WJR_ASSUME(_Mydata._Myhead == _Mydata._Mytail);
		const auto _Newsize = _Oldsize + 1;
		const auto _Newcapacity = getGrowthCapacity(_Oldsize, _Newsize);

		auto _Newptr = _Alty_traits::allocate(al, _Newcapacity);
		wjr::construct_at(al, _Newptr, std::forward<Args>(args)...);

		wjr::uninitialized_move(al, begin(), begin() + capacity(), _Newptr + 1);

		tidy();

		_Mydata._Myptr = _Newptr;
		_Mydata._Myhead = 0;
		_Mydata._Mytail = _Mydata._Mysize = _Newsize;
		_Mydata._Mycapacity = _Newcapacity;
	}

	template<typename...Args>
	WJR_CONSTEXPR20 void _M_realloc_insert_at_end(Args&&...args) {
		auto& al = getAllocator();
		auto& _Mydata = getData();
		const auto _Oldsize = _Mydata._Mysize;
		WJR_ASSUME(_Mydata._Mysize == _Mydata._Mycapacity);
		WJR_ASSUME(_Mydata._Myhead == _Mydata._Mytail);
		const auto _Newsize = _Oldsize + 1;
		const auto _Newcapacity = getGrowthCapacity(_Oldsize, _Newsize);

		auto _Newptr = _Alty_traits::allocate(al, _Newcapacity);
		wjr::construct_at(al, _Newptr + _Oldsize, std::forward<Args>(args)...);

		wjr::uninitialized_move(al, begin(), begin() + capacity(), _Newptr);

		tidy();

		_Mydata._Myptr = _Newptr;
		_Mydata._Myhead = 0;
		_Mydata._Mytail = _Mydata._Mysize = _Newsize;
		_Mydata._Mycapacity = _Newcapacity;
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_range_append(iter _First, iter _Last, std::input_iterator_tag) {
		for (; _First != _Last; ++_First) {
			emplace_back(*_First);
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_range_append(iter _First, iter _Last, std::forward_iterator_tag) {
		if (_First != _Last) {
			auto& al = getAllocator();
			auto& _Mydata = getData();
			auto& _Myptr = _Mydata._Myptr;
			auto& _Myhead = _Mydata._Myhead;
			auto& _Mytail = _Mydata._Mytail;
			auto& _Mysize = _Mydata._Mysize;
			auto& _Mycapacity = _Mydata._Mycapacity;

			const auto n = static_cast<size_type>(std::distance(_First, _Last));
			const auto __rest = static_cast<size_type>(_Mycapacity - _Mysize);

			if (__rest >= n) {
				const auto __tail_rest = static_cast<size_type>(_Mycapacity - _Mytail);
				if (__tail_rest >= n) {
					wjr::uninitialized_copy(al, _First, _Last, _Myptr + _Mytail);
					_Mytail += n;
				}
				else {
					auto _Mid = _First;
					std::advance(_Mid, __tail_rest);
					wjr::uninitialized_copy_n(al, 
						_First, 
						__tail_rest, 
						_Myptr + _Mytail
					);
					wjr::uninitialized_copy(al,
						_Mid,
						_Last,
						_Myptr
					);
					_Mytail = n - __tail_rest;
				}
				_Mysize += n;
			}
			else {
				const auto _Newsize = _Mysize + n;
				const auto _Newcapacity = getGrowthCapacity(capacity(), _Newsize);

				auto _Newptr = _Alty_traits::allocate(al, _Newcapacity);

				wjr::uninitialized_copy(al, _First, _Last, _Newptr + _Mysize);
				wjr::uninitialized_move(al, begin(), end(), _Newptr);

				tidy();
				_Mydata._Myptr = _Newptr;
				_Mydata._Myhead = 0;
				_Mydata._Mytail = _Mydata._Mysize = _Newsize;
				_Mydata._Mycapacity = _Newcapacity;
			}
		}
	}

	compressed_pair<_Alty, __data> _Myval;
};

_WJR_END

#include <wjr/circular_buffer-inl.h>

#endif // __WJR_CIRCULAR_BUFFER_H