#pragma once
#ifndef __WJR_VECTOR_H
#define __WJR_VECTOR_H

#pragma push_macro("new")
#undef new

#include <stdexcept>
#include <string>

#include <wjr/compressed_pair.h>
#include <wjr/algorithm.h>

_WJR_BEGIN

/*
* vector_data ->
* 1.vector_data() 
* 2.vector_data(_Al&al, const size_t _Newsize, const size_t _Newcapacity)
* 4.vector_data(const vector_data&) = delete
* 5.vector_data(vector_data&&) = delete
* 6.static void copyConstruct(_Al& al, const vector_data& _Src, vector_data& _Dest)
* 7.static void moveConstruct(_Al& al, vector_data&& _Src, vector_data& _Dest)
* 8.static void Swap(_Al& al, vector_data& _Left, vector_data& _Right) [noexcept] [optional]
* 9.static void Deallocate(_Al& al, vector_data& _Data)
* 10.static size_t getGrowthCapacity(const size_t _Oldcapacity, const size_t _Newsize) [optinal]
* 11.static void shrinkToFit(_Al&al, vector_data& _Data) [optional]
* 12.size_t size() const;
* 13.size_t capacity() const;
* 14.[const] _Ty* data() [const]
* 15.const _Ty* cdata() const
* 16.void set_size(const size_type);
* 17.void inc_size(const difference_type);
*/

// support small-size-optimize
// don't support copy-on-write yet

template<typename T, typename Alloc>
struct vector_data {
	using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using _Alty_traits = std::allocator_traits<_Alty>;
	using value_type = T;
	using pointer = typename _Alty_traits::pointer;
	using const_pointer = typename _Alty_traits::const_pointer;
	using size_type = typename _Alty_traits::size_type;
	using difference_type = typename _Alty_traits::difference_type;

	WJR_INTRINSIC_CONSTEXPR20 vector_data() = default;
	vector_data(const vector_data&) = delete;
	vector_data& operator=(const vector_data&) = delete;
	~vector_data() = default;

	WJR_CONSTEXPR20 vector_data(
		_Alty& al, 
		const size_type _Newsize,
		const size_type _Newcapacity,
		extend_tag)
		: _Myfirst(_Alty_traits::allocate(al, _Newcapacity)),
		_Mylast(_Myfirst + _Newsize),
		_Myend(_Myfirst + _Newcapacity) {
	}

	WJR_CONSTEXPR20 vector_data(
		_Alty& al,
		const size_type _Newsize,
		const size_type _Newcapacity)
		: _Myfirst(_Alty_traits::allocate(al, _Newcapacity)),
		_Mylast(_Myfirst + _Newsize),
		_Myend(_Myfirst + _Newcapacity) {
	}

	WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const vector_data& _Src, vector_data& _Dest) {
		{
			vector_data data(al, _Src.size(), _Src.size(), extend_tag{});
			_Dest._Myfirst = data._Myfirst;
			_Dest._Mylast = data._Mylast;
			_Dest._Myend = data._Myend;
		}
		wjr::uninitialized_copy(al, _Src._Myfirst, _Src._Mylast, _Dest._Myfirst);
	}

	WJR_INTRINSIC_CONSTEXPR20 static void moveConstruct(_Alty&, vector_data&& _Src, vector_data& _Dest) noexcept {
		_Dest._Myfirst = _Src._Myfirst;
		_Dest._Mylast = _Src._Mylast;
		_Dest._Myend = _Src._Myend;
		_Src._Myfirst = _Src._Mylast = _Src._Myend = nullptr;
	}

	WJR_CONSTEXPR20 static void Destroy(_Alty& al, vector_data& _Data) {
		wjr::destroy(al, _Data.data(), _Data.lastPtr());
	}

	WJR_CONSTEXPR20 static void Deallocate(_Alty& al, vector_data& _Data) {
		_Alty_traits::deallocate(al, _Data._Myfirst, _Data.capacity());
		_Data._Myfirst = _Data._Mylast = _Data._Myend = nullptr;
	}

	WJR_CONSTEXPR20 static void Tidy(_Alty& al, vector_data& _Data) {
		if (_Data._Myfirst != nullptr) {
			Destroy(al, _Data);
			Deallocate(al, _Data);
		}
	}

	WJR_CONSTEXPR20 static void shrinkToFit(_Alty& al, vector_data& _Data) noexcept {
		if (_Data._Mylast != _Data._Myend) {
			auto _Olddata = _Data._Myfirst;
			const auto _Oldsize = _Data.size();
			if (_Oldsize == 0) {
				Deallocate(al, _Data);
			}
			else {
				vector_data _Newdata(al, _Oldsize, _Oldsize);
				wjr::uninitialized_move_n(al, _Data.data(), _Oldsize, _Newdata.data());
				wjr::destroy_n(al, _Olddata, _Oldsize);
				Deallocate(al, _Data);
				moveConstruct(al, std::move(_Newdata), _Data);
			}
		}
	}

	WJR_INTRINSIC_CONSTEXPR20 size_type size() const noexcept {
		return static_cast<size_type>(_Mylast - _Myfirst);
	}

	WJR_INTRINSIC_CONSTEXPR20 size_type capacity() const noexcept {
		return static_cast<size_type>(_Myend - _Myfirst);
	}

	WJR_INTRINSIC_CONSTEXPR20 pointer data() noexcept {
		return _Myfirst;
	}

	WJR_INTRINSIC_CONSTEXPR20 const_pointer data() const noexcept {
		return _Myfirst;
	}

	WJR_INTRINSIC_CONSTEXPR20 pointer lastPtr() noexcept {
		return _Mylast;
	}

	WJR_INTRINSIC_CONSTEXPR20 const_pointer lastPtr() const noexcept {
		return _Mylast;
	}

	WJR_INTRINSIC_CONSTEXPR20 pointer endPtr() noexcept {
		return _Myend;
	}

	WJR_INTRINSIC_CONSTEXPR20 const_pointer endPtr() const noexcept {
		return _Myend;
	}

	WJR_INTRINSIC_CONSTEXPR20 void set_size(const size_type _Size) noexcept {
		_Mylast = _Myfirst + _Size;
	}

	WJR_INTRINSIC_CONSTEXPR20 void inc_size(const difference_type _Size) noexcept {
		_Mylast += _Size;
	}

	pointer _Myfirst = nullptr;
	pointer _Mylast = nullptr;
	pointer _Myend = nullptr;
};

// Not tested

template<typename T, size_t N, typename Alloc>
struct vector_static_data {
	using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using _Alty_traits = std::allocator_traits<_Alty>;
	using value_type = T;
	using pointer = typename _Alty_traits::pointer;
	using const_pointer = typename _Alty_traits::const_pointer;
	using size_type = typename _Alty_traits::size_type;
	using difference_type = typename _Alty_traits::difference_type;

	static_assert(N > 0, "N must be greater than 0");

	constexpr static size_t _MaxAlignment = std::max(alignof(T), alignof(size_type));
	constexpr static size_t _MaxMemroy = (sizeof(T) * N + _MaxAlignment - 1) & (~(_MaxAlignment - 1));
	constexpr static size_t max_capacity = _MaxMemroy / sizeof(T);
	static_assert(max_capacity >= N, "");

	vector_static_data() {}
	vector_static_data(const vector_static_data&) = delete;
	vector_static_data& operator=(const vector_static_data&) = delete;
	~vector_static_data() = default;

	WJR_CONSTEXPR20 static void _lengthError(const size_type _Newcapacity){
#if defined(_WJR_EXCEPTION)
		std::string str = "vector_static_data is too small to hold the requested data";
		str += "\n old capacity = " + std::to_string(max_capacity);
		str += "\n allocate new capacity = " + std::to_string(_Newcapacity);
		throw std::length_error(str);
#endif // _WJR_EXCEPTION
		unreachable();
	}

	WJR_CONSTEXPR20 vector_static_data(
		_Alty& al, 
		const size_type _Newsize,
		const size_type _Newcapacity,
		extend_tag) {
		_lengthError(_Newcapacity);
	}

	WJR_CONSTEXPR20 vector_static_data(
		_Alty& al,
		const size_type _Newsize,
		const size_type _Newcapacity) : _M_size(_Newsize) {
		if (_Newcapacity > max_capacity) {
			_lengthError(_Newcapacity);
		}
	}

	WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const vector_static_data& _Src, vector_static_data& _Dest) {
		wjr::uninitialized_copy_n(al, _Src.data(), _Src.size(), _Dest.data());
		_Dest.set_size(_Src.size());
	}

	WJR_CONSTEXPR20 static void moveConstruct(_Alty& al, vector_static_data&& _Src, vector_static_data& _Dest) {
		auto n = _Src.size();
		if (n != 0) {
			wjr::uninitialized_move_n(al, _Src.data(), n, _Dest.data());
			wjr::destroy_n(al, _Src.data(), n);
			_Src.set_size(0);
			_Dest.set_size(n);
		}
	}

	WJR_CONSTEXPR20 static void Destroy(_Alty& al, vector_static_data& _Data) {
		wjr::destroy_n(al, _Data.data(), _Data.size());
	}

	WJR_INTRINSIC_CONSTEXPR20 static void Deallocate(_Alty& al, vector_static_data& _Data) noexcept {
		_Data.set_size(0);
	}

	WJR_CONSTEXPR20 static void Tidy(_Alty& al, vector_static_data& _Data) {
		Destroy(al, _Data);
		Deallocate(al, _Data);
	}

	WJR_INTRINSIC_CONSTEXPR20 static void shrinkToFit(_Alty& al, vector_static_data& _Data) {
		// do nothing
	}

	WJR_INTRINSIC_CONSTEXPR20 size_type size() const noexcept {
		return _M_size;
	}

	WJR_INTRINSIC_CONSTEXPR20 size_type capacity() const noexcept {
		return max_capacity;
	}

	WJR_INTRINSIC_CONSTEXPR20 pointer data() noexcept {
		return reinterpret_cast<pointer>(_M_storage);
	}

	WJR_INTRINSIC_CONSTEXPR20 const_pointer data() const noexcept {
		return reinterpret_cast<const_pointer>(_M_storage);
	}

	WJR_INTRINSIC_CONSTEXPR20 pointer lastPtr() noexcept {
		return data() + size();
	}

	WJR_INTRINSIC_CONSTEXPR20 const_pointer lastPtr() const noexcept {
		return data() + size();
	}

	WJR_INTRINSIC_CONSTEXPR20 pointer endPtr() noexcept {
		return data() + capacity();
	}

	WJR_INTRINSIC_CONSTEXPR20 const_pointer endPtr() const noexcept {
		return data() + capacity();
	}

	WJR_INTRINSIC_CONSTEXPR20 void set_size(const size_type _Size) noexcept {
		_M_size = _Size;
	}

	WJR_INTRINSIC_CONSTEXPR20 void inc_size(const difference_type _Size) noexcept {
		_M_size += _Size;
	}

	size_type _M_size = 0;
	alignas(_MaxAlignment) uint8_t _M_storage[_MaxMemroy];
};


template<typename T, size_t N, typename Alloc>
struct vector_sso_data {
	using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using _Alty_traits = std::allocator_traits<_Alty>;
	using value_type = T;
	using pointer = typename _Alty_traits::pointer;
	using const_pointer = typename _Alty_traits::const_pointer;
	using size_type = typename _Alty_traits::size_type;
	using difference_type = typename _Alty_traits::difference_type;

	static_assert(N > 0, "N must be greater than 0");

	constexpr static size_t _MaxAlignment = std::max(alignof(T), 
		std::max(alignof(size_type), alignof(pointer)));
	constexpr static size_t _MaxMemroy = std::max(
		(sizeof(T) * N + _MaxAlignment - 1) & (~(_MaxAlignment - 1)),
		sizeof(size_type));
	constexpr static size_t max_capacity = _MaxMemroy / sizeof(T);
	static_assert(max_capacity >= N, "");

	WJR_INTRINSIC_CONSTEXPR20 vector_sso_data() {}
	vector_sso_data(const vector_sso_data&) = delete;
	vector_sso_data& operator=(const vector_sso_data&) = delete;
	~vector_sso_data() = default;

	WJR_CONSTEXPR20 vector_sso_data(
		_Alty& al,
		const size_type _Newsize,
		const size_type _Newcapacity,
		extend_tag)
		: _M_ptr(_Alty_traits::allocate(al, _Newcapacity)),
		_M_size(_Newsize), 
		_M_capacity(_Newcapacity) {
		WJR_ASSUME(_Newcapacity > max_capacity);
	}

	WJR_CONSTEXPR20 vector_sso_data(
		_Alty& al,
		const size_type _Newsize,
		const size_type _Newcapacity)
		: _M_size(_Newsize){
		if (_Newcapacity > max_capacity) {
			_M_ptr = _Alty_traits::allocate(al, _Newcapacity);
			_M_capacity = _Newcapacity;
		}
	}

	WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const vector_sso_data& _Src, vector_sso_data& _Dest) {
		const auto _Size = _Src._M_size;
		if (!_Src.is_small()) {
			vector_sso_data data(al, _Size, _Size, extend_tag{});
			_Dest._M_ptr = data._M_ptr;
			_Dest._M_capacity = data._M_capacity;
		}
		_Dest._M_size = _Size;
		wjr::uninitialized_copy_n(al, _Src.data(), _Src.size(), _Dest.data());
	}

	WJR_INTRINSIC_CONSTEXPR20 static void moveConstruct(_Alty& al, vector_sso_data&& _Src, vector_sso_data& _Dest) {
		const auto _Size = _Src._M_size;
		if(!_Src.is_small()){
			_Dest._M_ptr = _Src._M_ptr;
			_Dest._M_capacity = _Src._M_capacity;
		}
		else {
			WJR_ASSUME(_Size <= max_capacity);
			wjr::uninitialized_move_n(al, _Src.data(), _Size, _Dest.data());
			wjr::destroy_n(al, _Src.data(), _Size);
		}
		
		_Dest._M_size = _Size;

		_Src.reset_ptr();
		_Src._M_size = 0;
	}

	WJR_CONSTEXPR20 static void Destroy(_Alty& al, vector_sso_data& _Data) {
		wjr::destroy_n(al, _Data.data(), _Data.size());
	}

	WJR_CONSTEXPR20 static void Deallocate(_Alty& al, vector_sso_data& _Data) {
		if (!_Data.is_small()) {
			_Alty_traits::deallocate(al, _Data.data(), _Data.capacity());
		}
		_Data.reset_ptr();
		_Data._M_size = 0;
	}
	
	WJR_CONSTEXPR20 static void Tidy(_Alty& al, vector_sso_data& _Data) {
		Destroy(al, _Data);
		Deallocate(al, _Data);
	}

	WJR_INTRINSIC_CONSTEXPR20 static void shrinkToFit(_Alty& al, vector_sso_data& _Data) {
		if (!_Data.is_small()) {
			const auto _Oldsize = _Data.size();
			const auto _Oldcapacity = _Data.capacity();
			if (_Oldsize != _Oldcapacity) {
				if (_Oldsize <= max_capacity) {
					wjr::uninitialized_move_n(al, _Data.data(), _Oldsize, reinterpret_cast<pointer>(_Data._M_storage));
					wjr::destroy_n(al, _Data.data(), _Oldsize);
					_Alty_traits::deallocate(al, _Data.data(), _Oldcapacity);
					_Data.reset_ptr();
				}
				else {
					auto _Newdata = _Alty_traits::allocate(al, _Oldsize);
					wjr::uninitialized_move_n(al, _Data.data(), _Oldsize, _Newdata);
					wjr::destroy_n(al, _Data.data(), _Oldsize);
					_Alty_traits::deallocate(al, _Data.data(), _Oldcapacity);
					_Data._M_ptr = _Newdata;
					_Data._M_capacity = _Oldsize;
				}
				_Data._M_size = _Oldsize;
			}
		}
	}

	WJR_INTRINSIC_CONSTEXPR20 size_type size() const noexcept {
		return _M_size;
	}

	WJR_INTRINSIC_CONSTEXPR20 size_type capacity() const noexcept {
		return is_small() ? max_capacity : _M_capacity;
	}

	WJR_INTRINSIC_CONSTEXPR20 pointer data() noexcept {
		return _M_ptr;
	}

	WJR_INTRINSIC_CONSTEXPR20 const_pointer data() const noexcept {
		return _M_ptr;
	}

	WJR_INTRINSIC_CONSTEXPR20 pointer lastPtr() noexcept {
		return data() + size();
	}

	WJR_INTRINSIC_CONSTEXPR20 const_pointer lastPtr() const noexcept {
		return data() + size();
	}

	WJR_INTRINSIC_CONSTEXPR20 pointer endPtr() noexcept {
		return data() + capacity();
	}

	WJR_INTRINSIC_CONSTEXPR20 const_pointer endPtr() const noexcept {
		return data() + capacity();
	}

	WJR_INTRINSIC_CONSTEXPR20 void set_size(const size_type _Size) noexcept {
		_M_size = _Size;
	}

	WJR_INTRINSIC_CONSTEXPR20 void inc_size(const difference_type _Size) noexcept {
		_M_size += _Size;
	}

	WJR_INTRINSIC_CONSTEXPR20 bool is_small() const noexcept{
		const bool f = static_cast<const void*>(_M_ptr) == static_cast<const void*>(_M_storage);
		return f;
	}

	WJR_INTRINSIC_CONSTEXPR20 void reset_ptr() noexcept {
		_M_ptr = reinterpret_cast<pointer>(_M_storage);
	}

	pointer _M_ptr = reinterpret_cast<pointer>(_M_storage);
	size_type _M_size = 0;
	union {
		alignas(_MaxAlignment) uint8_t _M_storage[_MaxMemroy];
		alignas(_MaxAlignment) size_type _M_capacity;
	};
};

template<typename Char, size_t N, typename Alloc>
struct string_static_data : public vector_static_data<Char, N, Alloc> {
	using _Mybase = vector_static_data<Char, N, Alloc>;

	using _Alty = typename _Mybase::_Alty;
	using _Alty_traits = typename _Mybase::_Alty_traits;
	using value_type = typename _Mybase::value_type;
	using pointer = typename _Mybase::pointer;
	using const_pointer = typename _Mybase::const_pointer;
	using size_type = typename _Mybase::size_type;
	using difference_type = typename _Mybase::difference_type;

	// virtual max capacity = max capacity - 1
	constexpr static size_t max_capacity = _Mybase::max_capacity - 1;

	WJR_INTRINSIC_CONSTEXPR20 string_static_data() : _Mybase() {}
	string_static_data(const string_static_data&) = delete;
	string_static_data& operator=(const string_static_data&) = delete;
	~string_static_data() = default;

	WJR_CONSTEXPR20 string_static_data(
		_Alty& al,
		const size_type _Newsize,
		const size_type _Newcapacity,
		extend_tag)
		: _Mybase(al, _Newsize, _Newcapacity + 1, extend_tag{}) {
	}

	WJR_CONSTEXPR20 string_static_data(
		_Alty& al,
		const size_type _Newsize,
		const size_type _Newcapacity)
		: _Mybase(al, _Newsize, _Newcapacity + 1) {
	}

	WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const string_static_data& _Src, string_static_data& _Dest) {
		return _Mybase::copyConstruct(al, std::move(_Src), _Dest);
	}

	WJR_INTRINSIC_CONSTEXPR20 static void moveConstruct(_Alty& al, 
		string_static_data&& _Src, string_static_data& _Dest) {
		return _Mybase::moveConstruct(al, std::move(_Src), _Dest);
	}

	WJR_CONSTEXPR20 static void Destroy(_Alty& al, string_static_data& _Data) {
		return _Mybase::Destroy(al, _Data);
	}

	WJR_CONSTEXPR20 static void Deallocate(_Alty& al, string_static_data& _Data) {
		return _Mybase::Deallocate(al, _Data);
	}

	WJR_CONSTEXPR20 static void Tidy(_Alty& al, string_static_data& _Data) {
		return _Mybase::Tidy(al, _Data);
	}

	WJR_INTRINSIC_CONSTEXPR20 static void shrinkToFit(_Alty& al, string_static_data& _Data) {
		return _Mybase::shrinkToFit(al, _Data);
	}

	WJR_INTRINSIC_CONSTEXPR20 size_type capacity() const noexcept {
		return _Mybase::capacity() - 1;
	}

	WJR_INTRINSIC_CONSTEXPR20 pointer endPtr() noexcept {
		return _Mybase::data() + capacity();
	}

	WJR_INTRINSIC_CONSTEXPR20 const_pointer endPtr() const noexcept {
		return _Mybase::data() + capacity();
	}
};

template<typename Char, size_t N, typename Alloc>
struct string_sso_data : public vector_sso_data<Char, N, Alloc> {
	using _Mybase = vector_sso_data<Char, N, Alloc>;

	using _Alty = typename _Mybase::_Alty;
	using _Alty_traits = typename _Mybase::_Alty_traits;
	using value_type = typename _Mybase::value_type;
	using pointer = typename _Mybase::pointer;
	using const_pointer = typename _Mybase::const_pointer;
	using size_type = typename _Mybase::size_type;
	using difference_type = typename _Mybase::difference_type;

	// virtual max capacity = max capacity - 1
	constexpr static size_t max_capacity = _Mybase::max_capacity - 1;

	WJR_INTRINSIC_CONSTEXPR20 string_sso_data() : _Mybase() {}
	string_sso_data(const string_sso_data&) = delete;
	string_sso_data& operator=(const string_sso_data&) = delete;
	~string_sso_data() = default;

	WJR_CONSTEXPR20 string_sso_data(
		_Alty& al,
		const size_type _Newsize,
		const size_type _Newcapacity,
		extend_tag)
		: _Mybase(al, _Newsize, _Newcapacity + 1, extend_tag{}) {
	}

	WJR_CONSTEXPR20 string_sso_data(
		_Alty& al,
		const size_type _Newsize,
		const size_type _Newcapacity)
		: _Mybase(al, _Newsize, _Newcapacity + 1) {
	}

	WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const string_sso_data& _Src, string_sso_data& _Dest) {
		const auto _Size = _Src._M_size;
		if (!_Src.is_small()) {
			string_sso_data data(al, _Size, _Size, extend_tag{});
			_Dest._M_ptr = data._M_ptr;
			_Dest._M_capacity = data._M_capacity;
		}
		_Dest._M_size = _Size;
		wjr::uninitialized_copy_n(al, _Src.data(), _Src.size(), _Dest.data());
	}

	WJR_INTRINSIC_CONSTEXPR20 static void moveConstruct(_Alty& al, string_sso_data&& _Src, string_sso_data& _Dest) {
		return _Mybase::moveConstruct(al, std::move(_Src), _Dest);
	}

	WJR_CONSTEXPR20 static void Destroy(_Alty& al, string_sso_data& _Data) {
		return _Mybase::Destroy(al, _Data);
	}

	WJR_CONSTEXPR20 static void Deallocate(_Alty& al, string_sso_data& _Data) {
		return _Mybase::Deallocate(al, _Data);
	}

	WJR_CONSTEXPR20 static void Tidy(_Alty& al, string_sso_data& _Data) {
		return _Mybase::Tidy(al, _Data);
	}

	WJR_INTRINSIC_CONSTEXPR20 static void shrinkToFit(_Alty& al, string_sso_data& _Data) {
		if (!_Data.is_small()) {
			const auto _Oldsize = _Data.size();
			const auto _Oldcapacity = _Data.capacity();
			if (_Oldsize != _Oldcapacity) {
				if (_Oldsize <= max_capacity) {
					wjr::uninitialized_move_n(al, _Data.data(), _Oldsize, reinterpret_cast<pointer>(_Data._M_storage));
					wjr::destroy_n(al, _Data.data(), _Oldsize);
					_Alty_traits::deallocate(al, _Data.data(), _Oldcapacity + 1);
					_Data.reset_ptr();
				}
				else {
					auto _Newdata = _Alty_traits::allocate(al, _Oldsize + 1);
					wjr::uninitialized_move_n(al, _Data.data(), _Oldsize, _Newdata);
					wjr::destroy_n(al, _Data.data(), _Oldsize);
					_Alty_traits::deallocate(al, _Data.data(), _Oldcapacity + 1);
					_Data._M_ptr = _Newdata;
					_Data._M_capacity = _Oldsize + 1;
				}
				_Data._M_size = _Oldsize;
			}
		}
	}

	WJR_INTRINSIC_CONSTEXPR20 size_type capacity() const noexcept {
		return _Mybase::capacity() - 1;
	}

	WJR_INTRINSIC_CONSTEXPR20 pointer endPtr() noexcept {
		return _Mybase::data() + capacity();
	}

	WJR_INTRINSIC_CONSTEXPR20 const_pointer endPtr() const noexcept {
		return _Mybase::data() + capacity();
	}
};

namespace _Vector_helper {
	WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(Swap, Swap);
	WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(getGrowthCapacity, getGrowthCapacity);
}

template<typename T, typename Alloc = std::allocator<T>, typename Data = vector_data<T, Alloc>>
class vector {

public:

	using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using _Alty_traits = std::allocator_traits<_Alty>;

	static_assert(std::is_same_v<typename _Alty_traits::value_type, T>,
		"Alloc::value_type must be the same as T");

	using value_type = T;
	using allocator_type = _Alty;
	using size_type = typename _Alty_traits::size_type;
	using difference_type = typename _Alty_traits::difference_type;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = typename _Alty_traits::pointer;
	using const_pointer = typename _Alty_traits::const_pointer;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	using data_type = Data;

	static_assert(std::is_pointer_v<pointer>, "");
	static_assert(std::is_pointer_v<const_pointer>, "");

	WJR_CONSTEXPR20 vector()
		noexcept(std::is_nothrow_default_constructible_v<_Alty>
			&& std::is_nothrow_default_constructible_v<data_type>) : _Myval() {}

	WJR_CONSTEXPR20 explicit vector(const allocator_type& al)
		noexcept(std::is_nothrow_constructible_v<_Alty, const allocator_type&>
			&& std::is_nothrow_default_constructible_v<data_type>)
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(al),
			std::forward_as_tuple()) {}

	WJR_CONSTEXPR20 explicit vector(const size_type _Count, const allocator_type& al = allocator_type())
		: vector(al) {
		_M_construct_n(_Count, value_construct_tag());
	}

	WJR_CONSTEXPR20 vector(size_type _Count, const value_type& _Val, const allocator_type& al = allocator_type())
		: vector(al) {
		_M_construct_n(_Count, _Val);
	}

private:
	constexpr static bool _IsNoThrowCopyConstruct =
		noexcept(data_type::copyConstruct(std::declval<_Alty&>(), 
			std::declval<const data_type&>(), std::declval<data_type&>()));
	constexpr static bool _IsNoThrowMoveConstruct =
		noexcept(data_type::moveConstruct(std::declval<_Alty&>(), 
			std::declval<data_type&&>(), std::declval<data_type&>()));
public:

	template<typename _Alloc>
	WJR_CONSTEXPR20 vector(const vector& other, _Alloc&& al, disable_tag)
		noexcept(_IsNoThrowCopyConstruct
			&& std::is_nothrow_constructible_v<_Alty, _Alloc&&>)
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(std::forward<_Alloc>(al)),
			std::forward_as_tuple()) {
		copyConstruct(getAllocator(), other.getData(), getData());
	}

	WJR_CONSTEXPR20 vector(const vector& other)
		: vector(other, _Alty_traits::select_on_container_copy_construction(other.getAllocator()), 
			disable_tag{}) {}

	WJR_CONSTEXPR20 vector(const vector& other, const allocator_type& al)
		: vector(other, al, disable_tag{}) {}

	WJR_CONSTEXPR20 vector(vector&& other)
		noexcept(_IsNoThrowMoveConstruct
			&& std::is_nothrow_move_constructible_v<_Alty>)
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(std::move(other.getAllocator())),
			std::forward_as_tuple()) {
		moveConstruct(getAllocator(), std::move(other.getData()), getData());
	}

private:
	WJR_CONSTEXPR20 vector(vector&& other, const allocator_type& al, std::true_type) noexcept
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(al),
			std::forward_as_tuple()) {
		moveConstruct(getAllocator(), std::move(other.getData()), getData());
	}
	WJR_CONSTEXPR20 vector(vector&& other, const allocator_type& _Al, std::false_type)
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(_Al),
			std::forward_as_tuple()) {
		auto& al = getAllocator();
		if (al == other.getAllocator()) {
			moveConstruct(al, std::move(other.getData()), getData());
		}
		else {
			const auto _Size = other.size();
			if (_Size != 0) {
				const auto _Oldcapacity = capacity();
				if (_Oldcapacity < _Size) {
					data_type _Data(al, 0, _Size, extend_tag{});
					moveConstruct(al, std::move(_Data), getData());
				}
				set_size(_Size);
				wjr::uninitialized_move_n(al, other.data(), _Size, data());
			}
		}
	}
public:
	WJR_CONSTEXPR20 vector(vector&& other, const allocator_type& al)
		noexcept(noexcept(vector(std::declval<vector&&>(),
			std::declval<const allocator_type&>(), std::declval<typename _Alty_traits::is_always_equal>())))
		: vector(std::move(other), al, typename _Alty_traits::is_always_equal{}) {}

	template<typename _Iter, std::enable_if_t<wjr::is_iterator_v<_Iter>, int> = 0>
	WJR_CONSTEXPR20 vector(_Iter _First, _Iter _Last, const allocator_type& al = allocator_type())
		: vector(al) {
		_M_range_init(_First, _Last, typename std::iterator_traits<_Iter>::iterator_category());
	}

	WJR_CONSTEXPR20 vector(std::initializer_list<value_type> il, const allocator_type& al = allocator_type())
		: vector(il.begin(), il.end(), al) { }

	WJR_CONSTEXPR20 ~vector() noexcept {
		tidy();
	}

	WJR_CONSTEXPR20 vector& operator=(const vector& other) {

		if (is_likely(this != std::addressof(other))) {
			auto& al = getAllocator();
			auto& other_al = other.getAllocator();

			if constexpr (std::conjunction_v<typename _Alty_traits::propagate_on_container_copy_assignment
				, std::negation<typename _Alty_traits::is_always_equal>>) {
				if (al != other_al) {
					tidy();
				}
			}

			if constexpr (_Alty_traits::propagate_on_container_copy_assignment::value) {
				al = other_al;
			}

			assign(other.begin(), other.end());
		}

		return *this;
	}

	WJR_CONSTEXPR20 vector& operator=(vector&& other)
		noexcept(_Alty_traits::propagate_on_container_move_assignment::value
			|| _Alty_traits::is_always_equal::value) {

		using type = std::disjunction<typename _Alty_traits::propagate_on_container_move_assignment,
			typename _Alty_traits::is_always_equal>;

		if (is_likely(this != std::addressof(other))) {
			_M_move_assign(std::move(other), type());
		}

		return *this;
	}

	WJR_CONSTEXPR20 vector& operator=(std::initializer_list<value_type> il) {
		return assign(il);
	}

	WJR_CONSTEXPR20 vector& assign(size_type _Count, const value_type& _Val) {
		_M_fill_assign(_Count, _Val);
		return *this;
	}

	template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
	WJR_CONSTEXPR20 vector& assign(iter _First, iter _Last) {
		_M_range_assign(_First, _Last, typename std::iterator_traits<iter>::iterator_category());
		return *this;
	}

	WJR_CONSTEXPR20 vector& assign(std::initializer_list<value_type> il) {
		return assign(il.begin(), il.end());
	}

	WJR_INTRINSIC_CONSTEXPR20 iterator begin() noexcept {
		return data();
	}

	WJR_INTRINSIC_CONSTEXPR20 const_iterator begin() const noexcept {
		return data();
	}

	WJR_INTRINSIC_CONSTEXPR20 const_iterator cbegin() const noexcept {
		return data();
	}

	WJR_INTRINSIC_CONSTEXPR20 iterator end() noexcept {
		return lastPtr();
	}

	WJR_INTRINSIC_CONSTEXPR20 const_iterator end() const noexcept {
		return lastPtr();
	}

	WJR_INTRINSIC_CONSTEXPR20 const_iterator cend() const noexcept {
		return lastPtr();
	}

	WJR_INTRINSIC_CONSTEXPR20 reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}

	WJR_INTRINSIC_CONSTEXPR20 const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}

	WJR_INTRINSIC_CONSTEXPR20 const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(cend());
	}

	WJR_INTRINSIC_CONSTEXPR20 reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}

	WJR_INTRINSIC_CONSTEXPR20 const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}

	WJR_INTRINSIC_CONSTEXPR20 const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(cbegin());
	}

	WJR_INTRINSIC_CONSTEXPR20 size_type size() const noexcept {
		return getData().size();
	}

	WJR_CONSTEXPR20 void resize(const size_type _Newsize) {
		_M_resize(_Newsize, value_construct_tag());
	}

	WJR_CONSTEXPR20 void resize(const size_type _Newsize, const value_type& _Val) {
		_M_resize(_Newsize, _Val);
	}

	WJR_CONSTEXPR20 void shrink_to_fit() {
		shrinkToFit(getAllocator(), getData());
	}

	WJR_INTRINSIC_CONSTEXPR20 size_type capacity() const noexcept {
		return getData().capacity();
	}

	WJR_INTRINSIC_CONSTEXPR20 bool empty() const noexcept {
		return size() == 0;
	}

	WJR_CONSTEXPR20 void reserve(size_type n) {
		const auto _Oldcapacity = capacity();
		if (_Oldcapacity < n) {
			auto& al = getAllocator();

			const auto _Oldsize = size();
			const auto _Newcapacity = getGrowthCapacity(_Oldcapacity, n);

			data_type _Newdata(al, _Oldsize, _Newcapacity, extend_tag{});
			wjr::uninitialized_move_n(al, data(), _Oldsize, _Newdata.data());

			tidy();
			moveConstruct(al, std::move(_Newdata), getData());
		}
	}

	WJR_INTRINSIC_CONSTEXPR20 reference operator[](size_type _Pos) noexcept {
		return *(data() + _Pos);
	}

	WJR_INTRINSIC_CONSTEXPR20 const_reference operator[](size_type _Pos) const noexcept {
		return *(data() + _Pos);
	}

	WJR_INTRINSIC_CONSTEXPR20 reference at(size_type _Pos) {
		if (_Pos >= size()) {
			throw (std::out_of_range("wjr::vector::at"));
		}
		return (*this)[_Pos];
	}

	WJR_INTRINSIC_CONSTEXPR20 const_reference at(size_type _Pos) const {
		if (_Pos >= size()) {
			throw (std::out_of_range("wjr::vector::at"));
		}
		return (*this)[_Pos];
	}

	WJR_INTRINSIC_CONSTEXPR20 reference front() noexcept {
		return *data();
	}

	WJR_INTRINSIC_CONSTEXPR20 const_reference front() const noexcept {
		return *data();
	}

	WJR_INTRINSIC_CONSTEXPR20 reference back() noexcept {
		return *(lastPtr() - 1);
	}

	WJR_INTRINSIC_CONSTEXPR20 const_reference back() const noexcept {
		return *(lastPtr() - 1);
	}

	WJR_INTRINSIC_CONSTEXPR20 pointer data() noexcept {
		return getData().data();
	}

	WJR_INTRINSIC_CONSTEXPR20 const_pointer data() const noexcept {
		return getData().data();
	}

	WJR_INTRINSIC_CONSTEXPR20 const_pointer cdata() const noexcept {
		return getData().data();
	}

	template<typename...Args>
	WJR_INLINE_CONSTEXPR20 reference emplace_back(Args&&... args) {
		auto& al = getAllocator();
		const auto _Mylast = lastPtr();
		const auto _Myend = endPtr();
		if (_Mylast != _Myend) {
			wjr::construct_at(al, _Mylast, std::forward<Args>(args)...);
			inc_size(1);
		}
		else {
			_M_realloc_insert_at_end(std::forward<Args>(args)...);
		}
		return back();
	}

	WJR_INLINE_CONSTEXPR20 void push_back(const value_type& _Val) {
		emplace_back(_Val);
	}

	WJR_INLINE_CONSTEXPR20 void push_back(value_type&& _Val) {
		emplace_back(std::move(_Val));
	}

	WJR_INTRINSIC_CONSTEXPR20 void pop_back() noexcept {
		inc_size(-1);
		const pointer _Mylast = lastPtr();
		wjr::destroy_at(getAllocator(), _Mylast);
	}

	template<typename...Args>
	WJR_INLINE_CONSTEXPR20 iterator emplace(const_iterator _Where, Args&&... args) {
		return _M_emplace_aux(_Where, std::forward<Args>(args)...);
	}

	WJR_INLINE_CONSTEXPR20 iterator insert(const_iterator _Where, const value_type& _Val) {
		return emplace(_Where, _Val);
	}

	WJR_INLINE_CONSTEXPR20 iterator insert(const_iterator _Where, value_type&& _Val) {
		return emplace(_Where, std::move(_Val));
	}

	WJR_CONSTEXPR20 iterator insert(const_iterator _Where, std::initializer_list<value_type> il) {
		return insert(_Where, il.begin(), il.end());
	}

	WJR_CONSTEXPR20 iterator insert(const_iterator _Where, size_type _Count, const value_type& _Val) {
		const auto __old_pos = _Where - cbegin();
		_M_fill_insert(begin() + __old_pos, _Count, _Val);
		return begin() + __old_pos;
	}

	template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
	WJR_CONSTEXPR20 iterator insert(const_iterator _Where, iter _First, iter _Last) {
		const auto __old_pos = _Where - cbegin();
		_M_range_insert(begin() + __old_pos, _First, _Last, typename std::iterator_traits<iter>::iterator_category());
		return begin() + __old_pos;
	}

	WJR_INLINE_CONSTEXPR20 iterator erase(const_iterator _Where) {
		return _M_erase(begin() + (_Where - cbegin()));
	}

	WJR_INLINE_CONSTEXPR20 iterator erase(const_iterator _First, const_iterator _Last) {
		const auto __beg = begin();
		const auto __cbeg = cbegin();
		return _M_erase(__beg + (_First - __cbeg), __beg + (_Last - __cbeg));
	}

	WJR_INLINE_CONSTEXPR20 void swap(vector& _Right) noexcept {
		Swap(getAllocator(), getData(), _Right.getData());
		if constexpr (_Alty_traits::propagate_on_container_swap::value) {
			std::swap(getAllocator(), _Right.getAllocator());
		}
	}

	WJR_CONSTEXPR20 void clear() {
		_M_erase_at_end(data());
	}

	WJR_CONSTEXPR20 void tidy() noexcept {
		Tidy(getAllocator(), getData());
	}

	WJR_INTRINSIC_CONSTEXPR20 allocator_type& get_allocator() noexcept {
		return getAllocator();
	}
	WJR_INTRINSIC_CONSTEXPR20 const allocator_type& get_allocator() const noexcept {
		return getAllocator();
	}

	// non-standard extension functions

	/*------Internal management function------*/

	WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const data_type& _Src, data_type& _Dest) 
		noexcept(_IsNoThrowCopyConstruct) {
		data_type::copyConstruct(al, _Src, _Dest);
	}

	WJR_INLINE_CONSTEXPR20 static void moveConstruct(_Alty& al, data_type&& _Src, data_type& _Dest)
		noexcept(_IsNoThrowMoveConstruct) {
		data_type::moveConstruct(al, std::move(_Src), _Dest);
	}

	template<typename _D = data_type, std::enable_if_t<
		_Vector_helper::has_static_member_function_Swap_v<_D, _Alty&, _D&, _D&>, int> = 0>
	WJR_INLINE_CONSTEXPR20 static void Swap(_Alty& al, _D& _Left, _D& _Right)
		noexcept(noexcept(_D::Swap(al, _Left, _Right))) {
		return _D::Swap(al, _Left, _Right);
	}

	template<typename _D = Data, std::enable_if_t<
		!_Vector_helper::has_static_member_function_Swap_v<_D, _Alty&, _D&, _D&>, int> = 0>
	WJR_INLINE_CONSTEXPR20 static void Swap(_Alty& al, _D& _Left, _D& _Right)
		noexcept(
			noexcept(moveConstruct(std::declval<_Alty&>(), std::declval<_D&&>(), std::declval<_D&>()))
			&& std::is_nothrow_default_constructible_v<_D>) {
		_D _Tmp;
		moveConstruct(al, std::move(_Left), _Tmp);
		moveConstruct(al, std::move(_Right), _Left);
		moveConstruct(al, std::move(_Tmp), _Right);
	}

	WJR_INLINE_CONSTEXPR20 static void Destroy(_Alty& al, data_type& _Data) {
		data_type::Destroy(al, _Data);
	}

	WJR_CONSTEXPR20 static void Deallocate(_Alty& al, data_type& _Data) 
		noexcept(noexcept(data_type::Deallocate(al, _Data))) {
		data_type::Deallocate(al, _Data);
		WJR_ASSUME(_Data.size() == 0);
	}

	WJR_CONSTEXPR20 static void Tidy(_Alty& al, data_type& _Data) {
		data_type::Tidy(al, _Data);
		WJR_ASSUME(_Data.size() == 0);
	}

	template<typename _D = Data, std::enable_if_t<
		_Vector_helper::has_static_member_function_getGrowthCapacity_v<
		_D, const size_type, const size_type>, int> = 0>
	WJR_INTRINSIC_CONSTEXPR20 static size_type getGrowthCapacity(
		const size_type _Oldcapacity, const size_type _Newsize) noexcept {
		return Data::getGrowthCapacity(_Oldcapacity, _Newsize);
	}
	template<typename _D = Data, std::enable_if_t<
		!_Vector_helper::has_static_member_function_getGrowthCapacity_v<
		_D, const size_type, const size_type>, int> = 0>
	WJR_INTRINSIC_CONSTEXPR20 static size_type getGrowthCapacity(
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

	WJR_CONSTEXPR20 static void shrinkToFit(_Alty& al, data_type& _Data) {
		data_type::shrinkToFit(al, _Data);
	}

	WJR_INTRINSIC_CONSTEXPR20 _Alty& getAllocator() noexcept {
		return _Myval.first();
	}
	WJR_INTRINSIC_CONSTEXPR20 const _Alty& getAllocator() const noexcept {
		return _Myval.first();
	}

	WJR_INTRINSIC_CONSTEXPR20 data_type& getData() noexcept {
		return _Myval.second();
	}
	WJR_INTRINSIC_CONSTEXPR20 const data_type& getData() const noexcept {
		return _Myval.second();
	}

	WJR_INTRINSIC_CONSTEXPR20 pointer lastPtr() noexcept {
		return getData().lastPtr();
	}
	WJR_INTRINSIC_CONSTEXPR20 const_pointer lastPtr() const noexcept {
		return getData().lastPtr();
	}

	WJR_INTRINSIC_CONSTEXPR20 pointer endPtr() noexcept {
		return getData().endPtr();
	}
	WJR_INTRINSIC_CONSTEXPR20 const_pointer endPtr() const noexcept {
		return getData().endPtr();
	}

	WJR_INTRINSIC_CONSTEXPR20 void set_size(const size_type _Size) noexcept {
		getData().set_size(_Size);
	}

	WJR_INTRINSIC_CONSTEXPR20 void inc_size(const difference_type _Size) noexcept {
		getData().inc_size(_Size);
	}

	/*------External extension function------*/

	// n must less or equal 
	WJR_INLINE_CONSTEXPR20 vector& chop(size_t n) {
		_M_erase_at_end(end() - n);
		return *this;
	}

	// n <= size()
	WJR_INLINE_CONSTEXPR20 vector& truncate(size_t n) {
		return chop(size() - n);
	}
	
	/*------default construct/value_construct------*/
	
	WJR_CONSTEXPR20 vector(const size_type _Count, default_construct_tag, const allocator_type& al = allocator_type())
		: vector(al) {
		_M_construct_n(_Count, default_construct_tag());
	}

	WJR_CONSTEXPR20 vector(const size_type _Count, value_construct_tag, const allocator_type& al = allocator_type())
		: vector(al) {
		_M_construct_n(_Count, value_construct_tag());
	}

	WJR_CONSTEXPR20 void resize(const size_type _Newsize, default_construct_tag) {
		_M_resize(_Newsize, default_construct_tag());
	}

	WJR_CONSTEXPR20 void resize(const size_type _Newsize, value_construct_tag) {
		_M_resize(_Newsize, value_construct_tag());
	}

	WJR_CONSTEXPR20 void push_back(default_construct_tag) {
		emplace_back(default_construct_tag());
	}

	WJR_CONSTEXPR20 void push_back(value_construct_tag) {
		emplace_back(value_construct_tag());
	}

	WJR_CONSTEXPR20 vector& append(size_t n, default_construct_tag) {
		_M_append(n, default_construct_tag());
		return *this;
	}

	WJR_CONSTEXPR20 vector& append(size_t n, value_construct_tag) {
		_M_append(n, value_construct_tag());
		return *this;
	}

	/*------------------------------------------------------------*/

	WJR_INLINE_CONSTEXPR20 vector& append(const T& val) {
		emplace_back(val);
		return *this;
	}

	WJR_INLINE_CONSTEXPR20 vector& append(T&& val) {
		emplace_back(std::move(val));
		return *this;
	}

	template<typename _Iter, std::enable_if_t<is_iterator_v<_Iter>, int> = 0>
	WJR_CONSTEXPR20 vector& append(_Iter _First, _Iter _Last) {
		_M_range_append(_First, _Last, typename std::iterator_traits<_Iter>::iterator_category());
		return *this;
	}

	WJR_CONSTEXPR20 vector& append(size_t n, const T& val) {
		_M_append(n, val);
		return *this;
	}

	WJR_CONSTEXPR20 vector& append(std::initializer_list<value_type> il) {
		return append(il.begin(), il.end());
	}

	template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
	WJR_CONSTEXPR20 vector& replace(
		const_iterator _Oldfirst, const_iterator _Oldlast,
		iter _Newfirst, iter _Newlast) {
		const auto __offset1 = static_cast<size_type>(_Oldfirst - cbegin());
		const auto __offset2 = static_cast<size_type>(_Oldlast - cbegin());
		_M_range_replace(begin() + __offset1, begin() + __offset2, 
			_Newfirst, _Newlast, typename std::iterator_traits<iter>::iterator_category());
		return *this;
	}

	WJR_CONSTEXPR20 vector& replace(
		const_iterator _Oldfirst, const_iterator _Oldlast,
		const size_type _Count, const value_type& _Val
	) {
		const auto __offset1 = static_cast<size_type>(_Oldfirst - cbegin());
		const auto __offset2 = static_cast<size_type>(_Oldlast - cbegin());
		_M_fill_replace(begin() + __offset1, begin() + __offset2,
			_Count, _Val);
		return *this;
	}

private:

	template<typename...Args, std::enable_if_t<is_any_index_of_v<sizeof...(Args),1,2>, int> = 0>
	WJR_CONSTEXPR20 void _M_construct_n(const size_type _Count, Args&&... args) {
		if (_Count != 0) {
			auto& al = getAllocator();
			const auto _Oldcapacity = capacity();
			if (_Oldcapacity < _Count) {
				data_type _Newdata(al, 0, _Count, extend_tag{});
				moveConstruct(al, std::move(_Newdata), getData());
			}
			set_size(_Count);
			const pointer _Ptr = data();
			if constexpr (sizeof...(Args) == 1) {
				wjr::uninitialized_fill_n(al, _Ptr, _Count, std::forward<Args>(args)...);
			}
			else if constexpr (sizeof...(Args) == 2) {
				wjr::uninitialized_copy(al, std::forward<Args>(args)..., _Ptr);
			}
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_range_init(iter _First, iter _Last, std::input_iterator_tag) {
		for (; _First != _Last; ++_First) {
			emplace_back(*_First);
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_range_init(iter _First, iter _Last, std::forward_iterator_tag) {
		const auto _Count = static_cast<size_type>(std::distance(_First, _Last));
		_M_construct_n(_Count, _First, _Last);
	}

	WJR_CONSTEXPR20 void _M_erase_at_end(const pointer _Where) noexcept {
		const auto _Myfirst = data();
		const pointer _Mylast = lastPtr();
		wjr::destroy(getAllocator(), _Where, _Mylast);
		const auto __new_size = static_cast<size_type>(_Where - _Myfirst);
		set_size(__new_size);
	}

	WJR_CONSTEXPR20 iterator _M_erase(iterator _Where) noexcept {
		const pointer _Mylast = lastPtr();
		if (_Where + 1 != _Mylast) {
			wjr::move(_Where + 1, _Mylast, _Where);
		}
		wjr::destroy_at(getAllocator(), _Mylast - 1);
		inc_size(-1);
		return _Where;
	}

	WJR_CONSTEXPR20 iterator _M_erase(iterator _First, iterator _Last) noexcept {
		const pointer _Mylast = lastPtr();
		if (_First != _Last) {
			if (_Last != _Mylast) {
				wjr::move(_Last, _Mylast, _First);
			}
			_M_erase_at_end(_Mylast - (_Last - _First));
		}
		return _First;
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_range_insert(iterator _Where, iter _First, iter _Last, std::input_iterator_tag) {
		if (_Where == end()) {
			_M_range_append(_First, _Last, std::input_iterator_tag{});
		}
		else if (_First != _Last) {
			vector _Tmp(_First, _Last, getAllocator());
			insert(_Where, _Tmp.begin(), _Tmp.end());
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_range_insert(iterator _Where, iter _First, iter _Last, std::forward_iterator_tag) {
		if (_First != _Last) {
			auto& al = getAllocator();
			const pointer _Myfirst = data();
			const pointer _Mylast = lastPtr();
			const pointer _Myend = endPtr();

			const auto n = static_cast<size_type>(std::distance(_First, _Last));
			const auto __rest = static_cast<size_type>(_Myend - _Mylast);

			if (__rest >= n) {
				const auto __elements_after = static_cast<size_type>(_Mylast - _Where);
				auto __old_last = _Mylast;
				if (__elements_after > n) {
					wjr::uninitialized_move(al, _Mylast - n, _Mylast, _Mylast);
					wjr::move_backward(_Where, __old_last - n, __old_last);
					wjr::copy(_First, _Last, _Where);
				}
				else {
					auto _Mid = _First;
					std::advance(_Mid, __elements_after);
					wjr::uninitialized_copy(al, _Mid, _Last, _Mylast);
					wjr::uninitialized_move(al, _Where, __old_last, _Mylast + n - __elements_after);
					wjr::copy(_First, _Mid, _Where);
				}
				inc_size(n);
			}
			else {
				const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
				const auto __old_pos = static_cast<size_type>(_Where - _Myfirst);
				const auto _Newcapacity = getGrowthCapacity(capacity(), __old_size + n);
				data_type _Newdata(al, __old_size + n, _Newcapacity, extend_tag{});
				const pointer _Newfirst = _Newdata.data();

				wjr::uninitialized_copy(al, _First, _Last, _Newfirst + __old_pos);
				wjr::uninitialized_move(al, _Myfirst, _Where, _Newfirst);
				wjr::uninitialized_move(al, _Where, _Mylast, _Newfirst + __old_pos + n);

				tidy();
				moveConstruct(al, std::move(_Newdata), getData());
			}
		}
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
			const pointer _Myfirst = data();
			const pointer _Mylast = lastPtr();
			const pointer _Myend = endPtr();

			const auto n = static_cast<size_type>(std::distance(_First, _Last));
			const auto __rest = static_cast<size_type>(_Myend - _Mylast);

			if (__rest >= n) {
				wjr::uninitialized_copy(al, _First, _Last, _Mylast);
				inc_size(n);
			}
			else {
				const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
				const auto _Newcapacity = getGrowthCapacity(capacity(), __old_size + n);

				data_type _Newdata(al, __old_size + n, _Newcapacity, extend_tag{});
				const pointer _Newfirst = _Newdata.data();

				wjr::uninitialized_copy(al, _First, _Last, _Newfirst + __old_size);
				wjr::uninitialized_move(al, _Myfirst, _Mylast, _Newfirst);

				tidy();
				moveConstruct(al, std::move(_Newdata), getData());
			}
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_range_assign(iter _First, iter _Last, std::input_iterator_tag) {
		const pointer _Myfirst = data();
		const pointer _Mylast = lastPtr();

		pointer cur = _Myfirst;

		for (; _First != _Last && cur != _Mylast; ++_First, ++cur) {
			*cur = *_First;
		}

		if (_First == _Last) {
			_M_erase_at_end(cur);
		}
		else {
			_M_range_append(_First, _Last, std::input_iterator_tag{});
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_range_assign(iter _First, iter _Last, std::forward_iterator_tag) {
		auto _Count = static_cast<size_type>(std::distance(_First, _Last));
		auto& al = getAllocator();
		const pointer _Myfirst = data();
		const pointer _Mylast = lastPtr();

		if (_Count <= size()) {
			wjr::copy(_First, _Last, _Myfirst);
			_M_erase_at_end(_Myfirst + _Count);
		}
		else if (_Count <= capacity()) {
			auto _Mid = _First;
			std::advance(_Mid, size());
			wjr::copy(_First, _Mid, begin());
			wjr::uninitialized_copy(al, _Mid, _Last, _Mylast);
			set_size(_Count);
		}
		else {
			auto _Newcapacity = getGrowthCapacity(capacity(), _Count);
			data_type _Newdata(al, _Count, _Newcapacity, extend_tag{});
			const pointer _Newfirst = _Newdata.data();
			wjr::uninitialized_copy(al, _First, _Last, _Newfirst);

			tidy();
			moveConstruct(al, std::move(_Newdata), getData());
		}
	}

	WJR_CONSTEXPR20 void _M_fill_assign(size_type _Count, const value_type& _Val) {
		auto& al = getAllocator();
		if (_Count > capacity()) {
			tidy();
			data_type _Newdata(al, _Count, _Count, extend_tag{});
			moveConstruct(al, std::move(_Newdata), getData());
			wjr::uninitialized_fill_n(al, data(), _Count, _Val);
			return;
		}
		if (_Count > size()) {
			wjr::fill(begin(), end(), _Val);
			wjr::uninitialized_fill_n(al, end(), _Count - size(), _Val);
			set_size(_Count);
		}
		else {
			_M_erase_at_end(wjr::fill_n(begin(), _Count, _Val));
		}
	}

	WJR_CONSTEXPR20 void _M_move_assign(vector&& other, std::true_type) {
		auto& al = getAllocator();
		tidy();
		moveConstruct(al, std::move(other.getData()), getData());
		if constexpr (_Alty_traits::propagate_on_container_move_assignment::value) {
			al = std::move(other.getAllocator());
		}
	}

	WJR_CONSTEXPR20 void _M_move_assign(vector&& other, std::false_type) {
		if (getAllocator() == other.getAllocator()) {
			_M_move_assign(std::move(other), std::true_type{});
		}
		else {
			_M_range_assign(
				std::make_move_iterator(other.begin()),
				std::make_move_iterator(other.end()),
				std::random_access_iterator_tag{});
			clear();
		}
	}

	template<typename...Args>
	WJR_CONSTEXPR20 void _M_realloc_insert(iterator _Where, Args&&...args) {
		auto& al = getAllocator();
		const pointer _Myfirst = data();
		const pointer _Mylast = lastPtr();

		const auto __old_pos = static_cast<size_type>(_Where - _Myfirst);
		const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
		const auto __new_size = __old_size + 1;
		const auto _Newcapacity = getGrowthCapacity(__old_size, __new_size);

		data_type _Newdata(al, __new_size, _Newcapacity, extend_tag{});

		const pointer _Newfirst = _Newdata.data();
		const pointer _Newwhere = _Newfirst + __old_pos;

		wjr::construct_at(al, _Newwhere, std::forward<Args>(args)...);

		wjr::uninitialized_move(al, _Myfirst, _Where, _Newfirst);
		wjr::uninitialized_move(al, _Where, _Mylast, _Newwhere + 1);

		tidy();
		moveConstruct(al, std::move(_Newdata), getData());
	}

	template<typename...Args>
	WJR_CONSTEXPR20 void _M_realloc_insert_at_end(Args&&...args) {
		auto& al = getAllocator();
		const pointer _Myfirst = data();
		const pointer _Mylast = lastPtr();

		const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
		const auto __new_size = __old_size + 1;
		const auto _Newcapacity = getGrowthCapacity(__old_size, __new_size);

		data_type _Newdata(al, __new_size, _Newcapacity, extend_tag{});
		const pointer _Newfirst = _Newdata.data();

		const pointer _Newwhere = _Newfirst + __old_size;
		wjr::construct_at(al, _Newwhere, std::forward<Args>(args)...);

		wjr::uninitialized_move_n(al, _Myfirst, __old_size, _Newfirst);

		tidy();
		moveConstruct(al, std::move(_Newdata), getData());
	}

	WJR_CONSTEXPR20 void _M_fill_insert(iterator _Where, size_type n, const value_type& _Val) {
		if (n == 0) {
			return;
		}

		auto& al = getAllocator();
		const pointer _Myfirst = data();
		const pointer _Mylast = lastPtr();
		const pointer _Myend = endPtr();

		const auto __rest = static_cast<size_type>(_Myend - _Mylast);

		if (__rest >= n) {
			const temporary_allocator_value<Alloc> _Tmp(al, _Val);
			const auto& _Copy = _Tmp.value();
			const auto __elements_after = static_cast<size_type>(_Mylast - _Where);
			if (__elements_after > n) {
				wjr::uninitialized_move(al, _Mylast - n, _Mylast, _Mylast);
				wjr::move_backward(_Where, _Mylast - n, _Mylast);
				wjr::fill_n(_Where, n, _Copy);
			}
			else {
				wjr::uninitialized_fill_n(al, _Mylast, n - __elements_after, _Copy);
				wjr::uninitialized_move(al, _Where, _Mylast, _Where + n);
				wjr::fill(_Where, _Mylast, _Copy);
			}
			inc_size(n);
		}
		else {
			const auto _Newcapacity = getGrowthCapacity(capacity(), size() + n);
			data_type _Newdata(al, size() + n, _Newcapacity, extend_tag{});
			const pointer _Newfirst = _Newdata.data();

			const auto __old_pos = static_cast<size_type>(_Where - _Myfirst);

			wjr::uninitialized_fill_n(al, _Newfirst + __old_pos, n, _Val);
			wjr::uninitialized_move(al, _Myfirst, _Where, _Newfirst);
			wjr::uninitialized_move(al, _Where, _Mylast, _Newfirst + __old_pos + n);

			tidy();
			moveConstruct(al, std::move(_Newdata), getData());
		}
	}

	template<typename _Ty>
	WJR_CONSTEXPR20 void _M_resize(const size_type _Newsize, const _Ty& _Val) {
		const auto _Oldsize = size();
		if (_Newsize > _Oldsize) {
			_M_append(_Newsize - _Oldsize, _Val);
		}
		else if (_Newsize < _Oldsize) {
			_M_erase_at_end(data() + _Newsize);
		}
	}

	template<typename _Ty>
	WJR_CONSTEXPR20 void _M_append(size_type n, const _Ty& _Val) {
		auto& al = getAllocator();

		const auto _Oldsize = size();
		const auto _Oldcapacity = capacity();

		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + _Oldsize;

		const auto __rest = _Oldcapacity - _Oldsize;
		const auto _Newsize = _Oldsize + n;

		if (__rest >= n) {
			wjr::uninitialized_fill_n(al, _Mylast, n, _Val);
			set_size(_Newsize);
		}
		else {
			auto _Newcapacity = getGrowthCapacity(_Oldcapacity, _Newsize);
			data_type _Newdata(al, _Newsize, _Newcapacity, extend_tag{});
			const pointer _Newfirst = _Newdata.data();

			wjr::uninitialized_fill_n(al, _Newfirst + _Oldsize, n, _Val);
			wjr::uninitialized_move(al, _Myfirst, _Mylast, _Newfirst);

			tidy();
			moveConstruct(al, std::move(_Newdata), getData());
		}
	}

	template<typename Args>
	WJR_CONSTEXPR20 void _M_insert_aux(iterator _Where, Args&& args) {
		auto& al = getAllocator();
		const pointer _Mylast = lastPtr();

		wjr::construct_at(al, _Mylast, std::move(*(_Mylast - 1)));

		wjr::move_backward(_Where, _Mylast - 1, _Mylast);
		*_Where = std::forward<Args>(args);

		inc_size(1);
	}

	template<typename...Args>
	WJR_CONSTEXPR20 iterator _M_emplace_aux(const_iterator _Where, Args&&...args) {
		const auto __offset = _Where - cbegin();

		auto& al = getAllocator();
		const pointer _Mylast = lastPtr();
		const pointer _Myend = endPtr();

		if (_Mylast != _Myend) {
			if (_Where == _Mylast) {
				wjr::construct_at(al, _Mylast, std::forward<Args>(args)...);
				inc_size(1);
			}
			else {
				temporary_allocator_value<Alloc> _Tmp(al, std::forward<Args>(args)...);
				_M_insert_aux(begin() + __offset, std::move(_Tmp.value()));
			}
		}
		else {
			_M_realloc_insert(begin() + __offset, std::forward<Args>(args)...);
		}

		return begin() + __offset;
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_range_replace(
		iterator _Oldfirst, iterator _Oldlast,
		iter _Newfirst, iter _Newlast, std::input_iterator_tag
	) {
		for (; _Oldfirst != _Oldlast && _Newfirst != _Newlast; ++_Oldfirst, ++_Newfirst) {
			*_Oldfirst = *_Newfirst;
		}

		if (_Newfirst == _Newlast) {
			_M_erase_at_end(_Oldfirst, _Oldlast);
		}
		else {
			_M_range_insert(_Oldlast, _Newfirst, _Newlast, std::input_iterator_tag());
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_range_replace(
		iterator _Oldfirst, iterator _Oldlast,
		iter _Newfirst, iter _Newlast, std::forward_iterator_tag
	) {
		const auto __n = static_cast<size_type>(_Oldlast - _Oldfirst);
		const auto __m = static_cast<size_type>(std::distance(_Newfirst, _Newlast));

		if (__m <= __n) {
			erase(wjr::copy_n(_Newfirst, __m, _Oldfirst), _Oldlast);
		}
		else {
			const auto __delta = __m - __n;

			auto& al = getAllocator();
			const auto _Myfirst = data();
			const auto _Mylast = lastPtr();
			const auto _Myend = endPtr();
			
			const auto __rest = static_cast<size_type>(_Myend - _Mylast);

			if (__rest >= __delta) {
				const auto __elements_after = static_cast<size_type>(_Mylast - _Oldfirst);
				if (__elements_after > __m) {
					wjr::uninitialized_move(al, _Mylast - __delta, _Mylast, _Mylast);
					wjr::move_backward(_Oldlast, _Mylast - __delta, _Mylast);
					wjr::copy(_Newfirst, _Newlast, _Oldfirst);
				}
				else {
					auto _Mid = _Newfirst;
					std::advance(_Mid, __elements_after);
					wjr::uninitialized_copy(al, _Mid, _Newlast, _Mylast);
					wjr::uninitialized_move(al, _Oldlast, _Mylast, _Oldfirst + __m);
					wjr::copy(_Newfirst, _Mid, _Oldfirst);
				}
				inc_size(__delta);
			}
			else {
				const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
				const auto __old_pos = static_cast<size_type>(_Oldfirst - _Myfirst);
				const auto _Newcapacity = getGrowthCapacity(capacity(), __old_size + __delta);
				data_type _Newdata(al, __old_size + __delta, _Newcapacity, extend_tag{});
				const pointer _Ptr = _Newdata.data();

				wjr::uninitialized_copy(al, _Newfirst, _Newlast, _Ptr + __old_pos);
				wjr::uninitialized_move(al, _Myfirst, _Oldfirst, _Ptr);
				wjr::uninitialized_move(al, _Oldlast, _Mylast, _Ptr + __old_pos + __m);

				tidy();
				moveConstruct(al, std::move(_Newdata), getData());
			}
		}
	}

	WJR_CONSTEXPR20 void _M_fill_replace(
		iterator _Oldfirst, iterator _Oldlast, 
		size_type __m, const value_type& _Val) {

		const auto __n = static_cast<size_type>(_Oldlast - _Oldfirst);

		if (__m <= __n) {
			erase(wjr::fill_n(_Oldfirst, __m, _Val), _Oldlast);
		}
		else {
			const auto __delta = __m - __n;

			auto& al = getAllocator();
			const auto _Myfirst = data();
			const auto _Mylast = lastPtr();
			const auto _Myend = endPtr();

			const auto __rest = static_cast<size_type>(_Myend - _Mylast);

			if (__rest >= __delta) {
				const temporary_allocator_value<Alloc> _Tmp(al, _Val);
				const auto& _Copy = _Tmp.value();
				const auto __elements_after = static_cast<size_type>(_Mylast - _Oldfirst);
				if (__elements_after > __m) {
					wjr::uninitialized_move(al, _Mylast - __delta, _Mylast, _Mylast);
					wjr::move_backward(_Oldlast, _Mylast - __delta, _Mylast);
					wjr::fill_n(_Oldfirst, __m, _Copy);
				}
				else {
					wjr::uninitialized_fill_n(al, _Mylast, __m - __elements_after, _Copy);
					wjr::uninitialized_move(al, _Oldlast, _Mylast, _Oldfirst + __m);
					wjr::fill(_Oldfirst, _Mylast, _Copy);
				}
				inc_size(__delta);
			}
			else {
				const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
				const auto __old_pos = static_cast<size_type>(_Oldfirst - _Myfirst);
				const auto _Newcapacity = getGrowthCapacity(capacity(), __old_size + __delta);
				data_type _Newdata(al, __old_size + __delta, _Newcapacity, extend_tag{});
				const pointer _Ptr = _Newdata.data();

				wjr::uninitialized_fill_n(al, _Ptr + __old_pos, __m, _Val);
				wjr::uninitialized_move(al, _Myfirst, _Oldfirst, _Ptr);
				wjr::uninitialized_move(al, _Oldlast, _Mylast, _Ptr + __old_pos + __m);

				tidy();
				moveConstruct(al, std::move(_Newdata), getData());
			}
		}
	}

private:
	wjr::compressed_pair<_Alty, data_type> _Myval;
};

template<typename iter,
	typename v = typename std::iterator_traits<iter>::value_type,
	typename Alloc = std::allocator<v>,
	typename Data = vector_data<v, Alloc>,
	std::enable_if_t<is_iterator_v<iter>, int> = 0>
vector(iter, iter, Alloc = Alloc()) -> vector<v, Data>;

template<typename T, size_t N, typename Alloc = std::allocator<T>>
using static_vector = vector<T, Alloc, vector_static_data<T, N, Alloc>>;

template<typename T, size_t N, typename Alloc = std::allocator<T>>
using sso_vector = vector<T, Alloc, vector_sso_data<T, N, Alloc>>;

template<typename T, typename Alloc, typename Data>
bool operator==(const vector<T, Alloc, Data>& lhs, const vector<T, Alloc, Data>& rhs) {
	return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T, typename Alloc, typename Data>
bool operator!=(const vector<T, Alloc, Data>& lhs, const vector<T, Alloc, Data>& rhs) {
	return !(lhs == rhs);
}

template<typename T, typename Alloc, typename Data>
bool operator<(const vector<T, Alloc, Data>& lhs, const vector<T, Alloc, Data>& rhs) {
	return wjr::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename T, typename Alloc, typename Data>
bool operator>(const vector<T, Alloc, Data>& lhs, const vector<T, Alloc, Data>& rhs) {
	return rhs < lhs;
}

template<typename T, typename Alloc, typename Data>
bool operator<=(const vector<T, Alloc, Data>& lhs, const vector<T, Alloc, Data>& rhs) {
	return !(rhs < lhs);
}

template<typename T, typename Alloc, typename Data>
bool operator>=(const vector<T, Alloc, Data>& lhs, const vector<T, Alloc, Data>& rhs) {
	return !(lhs < rhs);
}

_WJR_END

namespace std {
	template<typename T, typename Alloc, typename Data>
	WJR_CONSTEXPR20 void swap(wjr::vector<T, Alloc, Data>& lhs, wjr::vector<T, Alloc, Data>& rhs)
		noexcept(noexcept(lhs.swap(rhs))) {
		lhs.swap(rhs);
	}

	template<typename T, typename Alloc, typename Data>
	WJR_CONSTEXPR20 typename wjr::vector<T, Alloc, Data>::size_type erase(wjr::vector<T, Alloc, Data>& c, const T& value) {
		auto it = std::remove(c.begin(), c.end(), value);
		const auto n = std::distance(it, c.end());
		c.erase(it, c.end());
		return n;
	}

	template<typename T, typename Alloc, typename Data, typename Pred>
	WJR_CONSTEXPR20 typename wjr::vector<T, Alloc, Data>::size_type erase_if(wjr::vector<T, Alloc, Data>& c, Pred pred) {
		auto it = std::remove_if(c.begin(), c.end(), pred);
		const auto n = std::distance(it, c.end());
		c.erase(it, c.end());
		return n;
	}

}

#pragma pop_macro("new")

#endif // !__WJR_VECTOR_H