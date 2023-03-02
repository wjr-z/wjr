#pragma once
#ifndef __WJR_VECTOR_H
#define __WJR_VECTOR_H

#pragma push_macro("new")
#undef new

#include <stdexcept>
#include <string>

#include <wjr/pair.h>
#include <wjr/algorithm.h>
#include <wjr/allocator.h>

_WJR_BEGIN

/*
* vector_data ->
* vector_data must be shallow copy in copy constructor and copy assignment operator, 
* but deep copy in copy_construct, move_construct can be shallow copy/deep copy
* 1.static vector_data Allocate(_Al& al, size_t _Size, size_t _Capacity)
* 2.static void copyConstruct(_Al& al, const vector_data& _Src, vector_data& _Dest)
* 3.static void moveConstruct(_Al& al, vector_data&& _Src, vector_data& _Dest)
* 5.static void Swap(_Al& al, vector_data& _Left, vector_data& _Right) [noexcept] [optional]
* 7.static void Deallocate(_Al& al, vector_data& _Data)
* 9.static size_t growthCapacity(const size_t _Oldcapacity, const size_t _Newsize) [optinal]
* 10.static void shrinkToFit(_Al&al, vector_data& _Data) [optional]
* 11.size_t size() const;
* 12.size_t capacity() const;
* 13.[const] _Ty* data() [const]
* 14.const _Ty* cdata() const
* 15._Ty* mutable_data() [TODO]
* 16.void set_size();
*/

/*
* vector_core -> 
* 1.vector_core() [noexcept] = default;
* 2.vector_core(const allocator_type& al) [noexcept];
* 3.template<typename _Alloc> vector_core(const vector_core& other, _Alloc&& al) [noexcept];
* 4.vector_core(vector_core&& other) [noexcept];
* 5.vector_core(vector_core&& other, const allocator_type& al) [noexcept];
* 6.vector_core& operator=(const vector_core&) = delete;
* 7.vector_core& operator=(vector_core&&) = delete;
* 8.~vector_core() [noexcept];
* 9.static vector_core Allocate(_Al& al, size_t _Size, size_t _Capacity);
* 10.static void copyConstruct(_Al& al, const vector_data& _Src, vector_data& _Dest)
* 11.static void moveConstruct(_Al& al, vector_data&& _Src, vector_data& _Dest)
* 14.static void Swap(_Al& al, vector_data& _Left, vector_data& _Right) [noexcept]
* 15.static void Destroy(_Al& al, vector_data& _Data);
* 16.static void Deallocate(_Al& al, vector_data& _Data);
* 17.static void Tidy(_Al& al, vector_data& _Data);
* 18.static size_t growthCapacity(const size_t _Oldcapacity, const size_t _Newsize);
* 19.static void shrinkToFit(_Al&al, vector_data& _Data);
* 20.void tidy() [noexcept];
* 21.void swap(vector_core& other) [noexcept];
* 22.size_t size() const;
* 23.size_t capacity() const;
* 24.[const] _Ty* data() [const]
* 25.const _Ty* cdata() const
* 26._Ty* mutable_data() [TODO]
* 27.void set_size();
* 28.[const] _Al& getAllocator() [const];
* 29.[const] vector_data& getData() [const];
*/

template<typename T, typename Alloc>
struct vector_data {
	using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using _Alty_traits = std::allocator_traits<_Alty>;
	using value_type = T;
	using pointer = typename _Alty_traits::pointer;
	using const_pointer = typename _Alty_traits::const_pointer;
	using size_type = typename _Alty_traits::size_type;
	using difference_type = typename _Alty_traits::difference_type;

	WJR_CONSTEXPR20 static vector_data Allocate(_Alty& al, size_type _Size, size_type _Capacity) {
		auto ptr = _Alty_traits::allocate(al, _Capacity);
		return { ptr, ptr + _Size, ptr + _Capacity };
	}

	WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const vector_data& _Src, vector_data& _Dest) {
		auto data = Allocate(al, _Src.size(), _Src.capacity());
		wjr::uninitialized_copy_a(_Src._Myfirst, _Src._Mylast, data._Myfirst, al);
		_Dest = data;
	}

	WJR_CONSTEXPR20 static void moveConstruct(_Alty& al, vector_data&& _Src, vector_data& _Dest) noexcept {
		_Dest = _Src;
		_Src._Myfirst = _Src._Mylast = _Src._Myend = nullptr;
	}

	WJR_CONSTEXPR20 static void Deallocate(_Alty& al, vector_data& _Data) {
		_Alty_traits::deallocate(al, _Data._Myfirst, _Data.capacity());
		_Data._Myfirst = _Data._Mylast = _Data._Myend = nullptr;
	}

	WJR_CONSTEXPR20 size_type size() const noexcept {
		return static_cast<size_type>(_Mylast - _Myfirst);
	}

	WJR_CONSTEXPR20 size_type capacity() const noexcept {
		return static_cast<size_type>(_Myend - _Myfirst);
	}

	WJR_CONSTEXPR20 pointer data() noexcept {
		return _Myfirst;
	}

	WJR_CONSTEXPR20 const_pointer data() const noexcept {
		return _Myfirst;
	}

	WJR_CONSTEXPR20 const_pointer cdata() const noexcept {
		return _Myfirst;
	}

	WJR_CONSTEXPR20 void set_size(size_type _Size) noexcept {
		_Mylast = _Myfirst + _Size;
	}

	pointer _Myfirst = nullptr;
	pointer _Mylast = nullptr;
	pointer _Myend = nullptr;
};


template<typename T, typename Alloc, size_t N>
struct vector_static_data {
	using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using _Alty_traits = std::allocator_traits<_Alty>;
	using value_type = T;
	using pointer = typename _Alty_traits::pointer;
	using const_pointer = typename _Alty_traits::const_pointer;
	using size_type = typename _Alty_traits::size_type;
	using difference_type = typename _Alty_traits::difference_type;

	static_assert(N > 0, "N must be greater than 0");

private:
	constexpr static size_t _MaxAlignment = std::max(alignof(T), std::max(alignof(size_type), alignof(max_align_t)));
	constexpr static size_t _MaxCapacity = ((sizeof(T) * N + _MaxAlignment - 1) & (~(_MaxAlignment - 1))) / sizeof(T);
public:

	WJR_CONSTEXPR20 static vector_static_data Allocate(_Alty& al, size_type _Size, size_type _Capacity) {
		std::string str = "vector_static_data is too small to hold the requested data";
		str += "\n old capacity = " + std::to_string(_MaxCapacity);
		str += "\n allocate new size = " + std::to_string(_Size);
		str += "\n allocate new capacity = " + std::to_string(_Capacity);
		throw std::length_error(str);
		unreachable();
		return vector_static_data{};
	}

	WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const vector_static_data& _Src, vector_static_data& _Dest) {
		wjr::uninitialized_copy_n_a(_Src.data(), _Src.size(), _Dest.data(), al);
		_Dest.set_size(_Src.size());
	}

	WJR_CONSTEXPR20 static void moveConstruct(_Alty& al, vector_static_data&& _Src, vector_static_data& _Dest) {
		auto n = _Src.size();
		if (n != 0) {
			wjr::uninitialized_move_n_a(_Src.data(), n, _Dest.data(), al);
			wjr::destroy_n_a(_Src.data(), n, al);
			_Src.set_size(0);
			_Dest.set_size(n);
		}
	}

	WJR_CONSTEXPR20 static void Deallocate(_Alty& al, vector_static_data& _Data) noexcept {
		// do nothing
	}

	WJR_CONSTEXPR20 static void shrinkToFit(_Alty& al, vector_static_data& _Data) {
#ifndef _WJR_NOEXCEPTION
		throw std::logic_error("vector_static_data does not support shrink_to_fit");
#endif // !_WJR_NOEXCEPTION
		unreachable();
	}

	WJR_CONSTEXPR20 size_type size() const noexcept {
		return _M_size;
	}

	WJR_CONSTEXPR20 size_type capacity() const noexcept {
		return _MaxCapacity;
	}

	WJR_CONSTEXPR20 pointer data() noexcept {
		return reinterpret_cast<pointer>(_M_storage);
	}

	WJR_CONSTEXPR20 const_pointer data() const noexcept {
		return reinterpret_cast<const_pointer>(_M_storage);
	}

	WJR_CONSTEXPR20 const_pointer cdata() const noexcept {
		return reinterpret_cast<const_pointer>(_M_storage);
	}

	WJR_CONSTEXPR20 void set_size(size_type _Size) noexcept {
		_M_size = _Size;
	}

	std::aligned_storage_t<sizeof(T), alignof(T)> _M_storage[_MaxCapacity];
	alignas(_MaxAlignment) size_type _M_size;
};


namespace _Vector_helper {
	REGISTER_HAS_STATIC_MEMBER_FUNCTION(Swap, Swap);
	REGISTER_HAS_STATIC_MEMBER_FUNCTION(growthCapacity, growthCapacity);
	REGISTER_HAS_STATIC_MEMBER_FUNCTION(shrinkToFit, shrinkToFit);
}

template<typename T, typename Alloc, typename Data = vector_data<T, Alloc>>
struct vector_core {
public:
	using _Alty = typename Data::_Alty;
	using _Alty_traits = typename Data::_Alty_traits;
	using value_type = typename Data::value_type;
	using pointer = typename Data::pointer;
	using const_pointer = typename Data::const_pointer;
	using size_type = typename Data::size_type;
	using difference_type = typename Data::difference_type;

	using allocator_type = Alloc;
	using reference = value_type&;
	using const_reference = const value_type&;

	using data_type = Data;

	WJR_CONSTEXPR20 vector_core() noexcept(std::is_nothrow_default_constructible_v<_Alty>) = default;

	WJR_CONSTEXPR20 vector_core(const allocator_type& al)
		noexcept(std::is_nothrow_constructible_v<_Alty, const allocator_type&>) :
		_Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(al),
			std::forward_as_tuple()) {}

	template<typename _Alloc>
	WJR_CONSTEXPR20 vector_core(const vector_core& other, _Alloc&& al)
		noexcept(std::is_nothrow_constructible_v<_Alty&, _Alloc&&> && 
			noexcept(copyConstruct(std::declval<_Alty&>(), std::declval<const data_type&>(), std::declval<data_type&>())))
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(std::forward<_Alloc>(al)),
			std::forward_as_tuple()) {
		copyConstruct(getAllocator(), other.getData(), getData());
	}

	WJR_CONSTEXPR20 vector_core(vector_core&& other)
		noexcept(std::is_nothrow_move_constructible_v<_Alty> &&
			noexcept(moveConstruct(std::declval<_Alty&>(), std::declval<data_type&&>(), std::declval<data_type&>())))
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(std::move(other.getAllocator())),
			std::forward_as_tuple()) {
		moveConstruct(getAllocator(), std::move(other.getData()), getData());
	}

private:
	WJR_CONSTEXPR20 vector_core(vector_core&& other, const allocator_type& al, std::true_type) noexcept
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(al),
			std::forward_as_tuple()) {
		moveConstruct(getAllocator(), std::move(other.getData()), getData());
	}
	WJR_CONSTEXPR20 vector_core(vector_core&& other, const allocator_type& _Al, std::false_type)
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(_Al),
			std::forward_as_tuple()) {
		auto& al = getAllocator();
		if (al == other.getAllocator()) {
			moveConstruct(al, std::move(other.getData()), getData());
		}
		else {
			const auto _Size = other.size();
			if (is_likely(_Size != 0)) {
				if (capacity() < _Size) {
					auto _Data = Allocate(al, _Size, _Size);
					moveConstruct(al, std::move(_Data), getData());
				}
				else {
					set_size(_Size);
				}
				wjr::uninitialized_move_n_a(other.data(), _Size, data(), al);
			}
		}
	}

public:
	WJR_CONSTEXPR20 vector_core(vector_core&& other, const allocator_type& al)
		noexcept(noexcept(vector_core(std::declval<vector_core&&>(),
			std::declval<const allocator_type&>(), std::declval<typename _Alty_traits::is_always_equal>())))
		: vector_core(std::move(other), al, typename _Alty_traits::is_always_equal{}) {}

	vector_core& operator=(const vector_core&) = delete;
	vector_core& operator=(vector_core&&) = delete;

	WJR_CONSTEXPR20 ~vector_core() noexcept {
		tidy();
	}

	WJR_CONSTEXPR20 static Data Allocate(_Alty& al, size_type _Count, size_type _Capacity)
		noexcept(noexcept(Data::Allocate(al, _Count, _Capacity))) {
		return Data::Allocate(al, _Count, _Capacity);
	}

	WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const Data& _Src, Data& _Dest)
		noexcept(noexcept(Data::copyConstruct(al, _Src, _Dest))) {
		return Data::copyConstruct(al, _Src, _Dest);
	}

	WJR_CONSTEXPR20 static void moveConstruct(_Alty& al, Data&& _Src, Data& _Dest)
		noexcept(noexcept(Data::moveConstruct(al, std::move(_Src), _Dest))) {
		return Data::moveConstruct(al, std::move(_Src), _Dest);
	}

	template<typename _D = Data, std::enable_if_t<
		_Vector_helper::has_static_member_function_Swap_v<_D, _Alty&, _D&, _D&>, int> = 0>
	WJR_CONSTEXPR20 static void Swap(_Alty& al, _D& _Left, _D& _Right)
		noexcept(noexcept(Data::Swap(al, _Left, _Right))) {
		return Data::Swap(al, _Left, _Right);
	}

	template<typename _D = Data, std::enable_if_t<
		!_Vector_helper::has_static_member_function_Swap_v<_D, _Alty&, _D&, _D&>, int> = 0>
	WJR_CONSTEXPR20 static void Swap(_Alty& al, _D& _Left, _D& _Right)
		noexcept(noexcept(moveConstruct(std::declval<_Alty&>(), std::declval<_D&&>(), std::declval<_D&>()))) {
		_D _Tmp;
		moveConstruct(al, std::move(_Left), _Tmp);
		moveConstruct(al, std::move(_Right), _Left);
		moveConstruct(al, std::move(_Tmp), _Right);
	}

	WJR_CONSTEXPR20 static void Destroy(_Alty& al, Data& _Data) noexcept {
		wjr::destroy_n_a(_Data.data(), _Data.size(), al);
	}

	WJR_CONSTEXPR20 static void Deallocate(_Alty& al, Data& _Data)
		noexcept(noexcept(Data::Deallocate(al, _Data))) {
		Data::Deallocate(al, _Data);
		assume(_Data.size() == 0);
	}

	WJR_CONSTEXPR20 static void Tidy(_Alty& al, Data& _Data) noexcept {
		Destroy(al, _Data);
		Deallocate(al, _Data);
		assume(_Data.size() == 0);
	}

	template<typename _D = Data, std::enable_if_t<
		_Vector_helper::has_static_member_function_growthCapacity_v<_D, const size_type, const size_type>, int> = 0>
	WJR_CONSTEXPR20 static size_type growthCapacity(const size_type _Oldcapacity, const size_type _Newsize) noexcept {
		return Data::growthCapacity(_Oldcapacity, _Newsize);
	}
	template<typename _D = Data, std::enable_if_t<
		!_Vector_helper::has_static_member_function_growthCapacity_v<_D, const size_type, const size_type>, int> = 0>
	WJR_CONSTEXPR20 static size_type growthCapacity(const size_type _Oldcapacity, const size_type _Newsize) noexcept {
		assume(_Newsize > _Oldcapacity);
		const auto _Newcapacity = _Oldcapacity + _Oldcapacity / 2;
		return _Newcapacity < _Newsize ? _Newsize : _Newcapacity;
	}

	template<typename _D = Data, std::enable_if_t<
		_Vector_helper::has_static_member_function_shrinkToFit_v<_D, _Alty&, _D&>, int> = 0>
	WJR_CONSTEXPR20 static void shrinkToFit(_Alty& al, _D& _Data) noexcept {
		return Data::shrinkToFit(al, _Data);
	}

	template<typename _D = Data, std::enable_if_t<
		!_Vector_helper::has_static_member_function_shrinkToFit_v<_D, _Alty&, _D&>, int> = 0>
	WJR_CONSTEXPR20 static void shrinkToFit(_Alty& al, _D& _Data) noexcept {
		auto _Oldsize = _Data.size();
		auto _Oldcapacity = _Data.capacity();
		if (_Oldsize != _Oldcapacity) {
			if(_Oldsize == 0){
				Tidy(al, _Data);
			}
			else {
				auto _Newdata = Allocate(al, _Oldsize, _Oldsize);
				wjr::uninitialized_move_n_a(_Data.data(), _Oldsize, _Newdata.data(), al);
				Tidy(al, _Data);
				moveConstruct(al, std::move(_Newdata), _Data);
			}
		}
	}

	WJR_CONSTEXPR20 void tidy() noexcept {
		Tidy(getAllocator(), getData());
	}

	WJR_CONSTEXPR20 void swap(vector_core& other) noexcept {
		Swap(getAllocator(), getData(), other.getData());
		if constexpr (_Alty_traits::propagate_on_container_swap::value) {
			std::swap(getAllocator(), other.getAllocator());
		}
	}

	WJR_CONSTEXPR20 size_type size() const noexcept {
		return getData().size();
	}

	WJR_CONSTEXPR20 size_type capacity() const noexcept {
		return getData().capacity();
	}

	WJR_CONSTEXPR20 pointer data() noexcept {
		return getData().data();
	}

	WJR_CONSTEXPR20 const_pointer data() const noexcept {
		return getData().data();
	}

	WJR_CONSTEXPR20 const_pointer cdata() const noexcept {
		return getData().cdata();
	}

	WJR_CONSTEXPR20 void set_size(size_type _Size) noexcept {
		getData().set_size(_Size);
	}

	WJR_CONSTEXPR20 _Alty& getAllocator() noexcept {
		return _Myval.first();
	}

	WJR_CONSTEXPR20 const _Alty& getAllocator() const noexcept {
		return _Myval.first();
	}

	WJR_CONSTEXPR20 Data& getData() noexcept {
		return _Myval.second();
	}

	WJR_CONSTEXPR20 const Data& getData() const noexcept {
		return _Myval.second();
	}

	wjr::pair<_Alty, Data> _Myval;
};

template<typename T, typename Alloc = std::allocator<T>, typename Core = vector_core<T, Alloc>>
class vector : private Core{

	using _Mybase = Core;

	using _Alty = typename Core::_Alty;
	using _Alty_traits = typename Core::_Alty_traits;

	static_assert(std::is_same_v<typename _Alty_traits::value_type, T>,
		"Alloc::value_type must be the same as T");

	static_assert(std::is_same_v<typename Core::allocator_type, _Alty>,
		"Core::allocator_type must be the same as _Alty");

public:

	using value_type = typename Core::value_type;
	using allocator_type = typename Core::allocator_type;
	using size_type = typename Core::size_type;
	using difference_type = typename Core::difference_type;
	using reference = typename Core::reference;
	using const_reference = typename Core::const_reference;
	using pointer = typename Core::pointer;
	using const_pointer = typename Core::const_pointer;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	using core_type = Core;
	using data_type = typename Core::data_type;

	WJR_CONSTEXPR20 vector() noexcept(std::is_nothrow_constructible_v<core_type>) = default;

	WJR_CONSTEXPR20 explicit vector(const allocator_type& al) noexcept
		: _Mybase(al) {}

	WJR_CONSTEXPR20 explicit vector(const size_type _Count, const allocator_type& al = allocator_type())
		: _Mybase(al) {
		_M_construct_n(_Count);
	}

	WJR_CONSTEXPR20 vector(size_type _Count, const value_type& _Val, const allocator_type& al = allocator_type())
		: _Mybase(al) {
		_M_construct_n(_Count, _Val);
	}

	WJR_CONSTEXPR20 vector(const vector& other)
		: _Mybase(other, _Alty_traits::select_on_container_copy_construction(other.getAllocator())) {}

	WJR_CONSTEXPR20 vector(vector&& other) noexcept = default;

	WJR_CONSTEXPR20 vector(const vector& other, const allocator_type& al)
		: _Mybase(other, al) {}

	WJR_CONSTEXPR20 vector(vector&& other, const allocator_type& al)
		noexcept(std::is_nothrow_constructible_v<core_type, core_type&&, const allocator_type&>)
		: _Mybase(std::move(other), al) {}

	template<typename _Iter, std::enable_if_t<wjr::is_iterator_v<_Iter>, int> = 0>
	WJR_CONSTEXPR20 vector(_Iter _First, _Iter _Last, const allocator_type& al = allocator_type())
		: _Mybase(al) {
		_M_range_init(_First, _Last, typename std::iterator_traits<_Iter>::iterator_category());
	}

	WJR_CONSTEXPR20 vector(std::initializer_list<value_type> _Ilist, const allocator_type& al = allocator_type())
		: _Mybase(al) {
		_M_range_init(_Ilist.begin(), _Ilist.end(), std::random_access_iterator_tag());
	}

	WJR_CONSTEXPR20 ~vector() noexcept = default;

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
			_M_move_assign(std::move(other), type{});
		}
		
		return *this;
	}

	WJR_CONSTEXPR20 vector& operator=(std::initializer_list<value_type> _Ilist) {
		_M_assign_aux(_Ilist.begin(), _Ilist.end(), std::random_access_iterator_tag());
		return *this;
	}

	WJR_CONSTEXPR20 vector& assign(size_type _Count, const value_type& _Val) {
		_M_fill_assign(_Count, _Val);
		return *this;
	}

	template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
	WJR_CONSTEXPR20 vector& assign(iter _First, iter _Last) {
		_M_assign_aux(_First, _Last, typename std::iterator_traits<iter>::iterator_category());
		return *this;
	}

	WJR_CONSTEXPR20 vector& assign(std::initializer_list<value_type> _Ilist) {
		_M_assign_aux(_Ilist.begin(), _Ilist.end(), std::random_access_iterator_tag());
		return *this;
	}


	WJR_CONSTEXPR20 iterator begin() noexcept {
		return data();
	}

	WJR_CONSTEXPR20 const_iterator begin() const noexcept {
		return data();
	}

	WJR_CONSTEXPR20 const_iterator cbegin() const noexcept {
		return data();
	}

	WJR_CONSTEXPR20 iterator end() noexcept {
		return begin() + size();
	}

	WJR_CONSTEXPR20 const_iterator end() const noexcept {
		return begin() + size();
	}

	WJR_CONSTEXPR20 const_iterator cend() const noexcept {
		return cbegin() + size();
	}

	WJR_CONSTEXPR20 reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}

	WJR_CONSTEXPR20 const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}

	WJR_CONSTEXPR20 const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(cend());
	}

	WJR_CONSTEXPR20 reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}

	WJR_CONSTEXPR20 const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}

	WJR_CONSTEXPR20 const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(cbegin());
	}

	WJR_CONSTEXPR20 size_type size() const noexcept {
		return _Mybase::size();
	}

	WJR_CONSTEXPR20 void resize(const size_type _Newsize) {
		auto& al = getAllocator();
		temporary_allocator_value<Alloc> _Tmp(al);
		resize(_Newsize, _Tmp.value());
	}

	WJR_CONSTEXPR20 void resize(const size_type _Newsize, const value_type& _Val) {
		const auto _Oldsize = size();
		if (_Newsize > _Oldsize) {
			_M_append(_Newsize - _Oldsize, _Val);
		}
		else if (_Newsize < _Oldsize) {
			_M_erase_at_end(data() + _Newsize);
		}
		assume(size() == _Newsize);
	}

	WJR_CONSTEXPR20 void shrink_to_fit() {
		core_type::shrinkToFit(getAllocator(), getData());
	}

	WJR_CONSTEXPR20 size_type capacity() const noexcept {
		return _Mybase::capacity();
	}

	WJR_CONSTEXPR20 bool empty() const noexcept {
		return size() == 0;
	}

	WJR_CONSTEXPR20 void reserve(size_type n) {
		const auto _Oldcapacity = capacity();
		if (_Oldcapacity < n) {
			auto& al = getAllocator();

			const auto _Oldsize = size();
			const auto _Newcapacity = getGrowthCapacity(_Oldcapacity, n);

			auto _Newdata = Allocate(al, _Oldsize, _Newcapacity);
			wjr::uninitialized_move_n_a(data(), _Oldsize, _Newdata.data(), al);

			tidy();
			moveConstruct(al, std::move(_Newdata), getData());
		}
	}

	WJR_CONSTEXPR20 reference operator[](size_type _Pos) noexcept {
		return *(data() + _Pos);
	}

	WJR_CONSTEXPR20 const_reference operator[](size_type _Pos) const noexcept {
		return *(data() + _Pos);
	}

	WJR_CONSTEXPR20 reference at(size_type _Pos) {
		if (_Pos >= size()) {
			throw (std::out_of_range("wjr::vector::at"));
		}
		return (*this)[_Pos];
	}

	WJR_CONSTEXPR20 const_reference at(size_type _Pos) const {
		if (_Pos >= size()) {
			throw (std::out_of_range("wjr::vector::at"));
		}
		return (*this)[_Pos];
	}

	WJR_CONSTEXPR20 reference front() noexcept {
		return *data();
	}

	WJR_CONSTEXPR20 const_reference front() const noexcept {
		return *data();
	}

	WJR_CONSTEXPR20 reference back() noexcept {
		return *(data() + size() - 1);
	}

	WJR_CONSTEXPR20 const_reference back() const noexcept {
		return *(data() + size() - 1);
	}

	WJR_CONSTEXPR20 pointer data() noexcept {
		return _Mybase::data();
	}

	WJR_CONSTEXPR20 const_pointer data() const noexcept {
		return _Mybase::data();
	}

	template<typename...Args>
	WJR_CONSTEXPR20 reference emplace_back(Args&&... args) {
		auto& al = getAllocator();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		const pointer _Myend = _Myfirst + capacity();
		if (_Mylast != _Myend) {
			_Alty_traits::construct(al, _Mylast, std::forward<Args>(args)...);
			set_size(size() + 1);
		}
		else {
			_M_realloc_insert_at_end(std::forward<Args>(args)...);
		}
		return back();
	}

	WJR_CONSTEXPR20 void push_back(const value_type& _Val) {
		emplace_back(_Val);
	}

	WJR_CONSTEXPR20 void push_back(value_type&& _Val) {
		emplace_back(std::move(_Val));
	}

	WJR_CONSTEXPR20 void pop_back() noexcept {
		set_size(size() - 1);
		const pointer _Mylast = data() + size();
		_Alty_traits::destroy(getAllocator(), _Mylast);
	}

	template<typename...Args>
	WJR_CONSTEXPR20 iterator emplace(const_iterator _Where, Args&&... args) {
		return _M_emplace_aux(_Where, std::forward<Args>(args)...);
	}

	WJR_CONSTEXPR20 iterator insert(const_iterator _Where, const value_type& _Val) {
		return emplace(_Where, _Val);
	}

	WJR_CONSTEXPR20 iterator insert(const_iterator _Where, value_type&& _Val) {
		return emplace(_Where, std::move(_Val));
	}

	WJR_CONSTEXPR20 iterator insert(const_iterator _Where, std::initializer_list<value_type> _Ilist) {
		const auto __old_pos = _Where - cbegin();
		_M_range_insert(begin() + __old_pos, _Ilist.begin(), _Ilist.end(), std::random_access_iterator_tag());
		return begin() + __old_pos;
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

	WJR_CONSTEXPR20 iterator erase(const_iterator _Where) {
		return _M_erase(begin() + (_Where - cbegin()));
	}

	WJR_CONSTEXPR20 iterator erase(const_iterator _First, const_iterator _Last) {
		const auto __beg = begin();
		const auto __cbeg = cbegin();
		return _M_erase(__beg + (_First - __cbeg), __beg + (_Last - __cbeg));
	}

	WJR_CONSTEXPR20 void swap(vector& _Right) noexcept {
		getCore().swap(_Right.getCore());
	}

	WJR_CONSTEXPR20 void clear() {
		_M_erase_at_end(data());
	}

	WJR_CONSTEXPR20 void tidy() noexcept {
		Tidy(getAllocator(), getData());
	}

	WJR_CONSTEXPR20 allocator_type& get_allocator() noexcept {
		return getAllocator();
	}
	WJR_CONSTEXPR20 const allocator_type& get_allocator() const noexcept {
		return getAllocator();
	}

	// unstandard functions

	WJR_CONSTEXPR20 static data_type Allocate(_Alty& al, size_type _Newsize, size_type _Newcapacity) {
		return core_type::Allocate(al, _Newsize, _Newcapacity);
	}

	WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const data_type& _Src, data_type& _Dest) {
		core_type::copyConstruct(al, _Src, _Dest);
	}

	WJR_CONSTEXPR20 static void moveConstruct(_Alty& al, data_type&& _Src, data_type& _Dest) {
		core_type::moveConstruct(al, std::move(_Src), _Dest);
	}

	WJR_CONSTEXPR20 static void Destroy(_Alty& al, data_type& _Data) {
		core_type::Destroy(al, _Data);
	}

	WJR_CONSTEXPR20 static void Deallocate(_Alty& al, data_type& _Data) noexcept {
		core_type::Deallocate(al, _Data);
	}

	WJR_CONSTEXPR20 static void Tidy(_Alty& al, data_type& _Data) noexcept {
		core_type::Tidy(al, _Data);
	}

	WJR_CONSTEXPR20 static size_type getGrowthCapacity(const size_type _Oldcapacity, size_type _Newsize) noexcept {
		return core_type::growthCapacity(_Oldcapacity, _Newsize);
	}

	WJR_CONSTEXPR20 core_type& getCore() noexcept {
		return *this;
	}
	WJR_CONSTEXPR20 const core_type& getCore() const noexcept {
		return *this;
	}

	WJR_CONSTEXPR20 _Alty& getAllocator() noexcept {
		return _Mybase::getAllocator();
	}
	WJR_CONSTEXPR20 const _Alty& getAllocator() const noexcept {
		return _Mybase::getAllocator();
	}

	WJR_CONSTEXPR20 data_type& getData() noexcept {
		return _Mybase::getData();
	}
	WJR_CONSTEXPR20 const data_type& getData() const noexcept {
		return _Mybase::getData();
	}

	WJR_CONSTEXPR20 void set_size(size_type _Newsize) noexcept {
		_Mybase::set_size(_Newsize);
	}

private:

	template<typename...Args, std::enable_if_t<sizeof...(Args) <= 2, int> = 0>
	WJR_CONSTEXPR20 void _M_construct_n(const size_type _Count, Args&&... args) {
		if (_Count != 0) {
			auto& al = getAllocator();
			if (capacity() < _Count) {
				auto _Newdata = Allocate(al, _Count, _Count);
				moveConstruct(al, std::move(_Newdata), getData());
			}
			else {
				set_size(_Count);
			}
			const pointer _Ptr = data();
			if constexpr (sizeof...(Args) == 0) {
				wjr::uninitialized_value_construct_n_a(_Ptr, _Count, al);
			}
			else if constexpr (sizeof...(Args) == 1) {
				wjr::uninitialized_fill_n_a(_Ptr, _Count, std::forward<Args>(args)..., al);
			}
			else if constexpr (sizeof...(Args) == 2) {
				wjr::uninitialized_copy_a(std::forward<Args>(args)..., _Ptr, al);
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

	WJR_CONSTEXPR20 void _M_erase_at_end(pointer _Where) noexcept {
		const auto _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		wjr::destroy_a(_Where, _Mylast, getAllocator());
		const auto __new_size = static_cast<size_type>(_Where - _Myfirst);
		set_size(__new_size);
	}

	WJR_CONSTEXPR20 iterator _M_erase(iterator _Where) noexcept {
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		if (_Where + 1 != _Mylast) {
			wjr::move(_Where + 1, _Mylast, _Where);
		}
		wjr::destroy_at_a(_Mylast - 1, getAllocator());
		set_size(size() - 1);
		return _Where;
	}

	WJR_CONSTEXPR20 iterator _M_erase(iterator _First, iterator _Last) noexcept {
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
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
			const pointer _Mylast = _Myfirst + size();
			const pointer _Myend = _Myfirst + capacity();

			const auto n = static_cast<size_type>(std::distance(_First, _Last));
			const auto __rest = static_cast<size_type>(_Myend - _Mylast);

			if (__rest >= n) {
				const auto __elements_after = static_cast<size_type>(_Mylast - _Where);
				auto __old_last = _Mylast;
				if (__elements_after > n) {
					wjr::uninitialized_move_a(_Mylast - n, _Mylast, _Mylast, al);
					wjr::move_backward(_Where, __old_last - n, __old_last);
					wjr::copy(_First, _Last, _Where);
				}
				else {
					auto _Mid = _First;
					std::advance(_Mid, __elements_after);
					wjr::uninitialized_copy_a(_Mid, _Last, _Mylast, al);
					wjr::uninitialized_move_a(_Where, __old_last, _Mylast + n - __elements_after, al);
					wjr::copy(_First, _Mid, _Where);
				}
				set_size(size() + n);
			}
			else {
				const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
				const auto __old_pos = static_cast<size_type>(_Where - _Myfirst);
				const auto _Newcapacity = getGrowthCapacity(capacity(), __old_size + n);
				auto _Newdata = Allocate(al, __old_size + n, _Newcapacity);
				const pointer _Newfirst = _Newdata.data();

				wjr::uninitialized_copy_a(_First, _Last, _Newfirst + __old_pos, al);
				wjr::uninitialized_move_a(_Myfirst, _Where, _Newfirst, al);
				wjr::uninitialized_move_a(_Where, _Mylast, _Newfirst + __old_pos + n, al);

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
			const pointer _Mylast = _Myfirst + size();
			const pointer _Myend = _Myfirst + capacity();

			const auto n = static_cast<size_type>(std::distance(_First, _Last));
			const auto __rest = static_cast<size_type>(_Myend - _Mylast);

			if (__rest >= n) {
				wjr::uninitialized_copy_a(_First, _Last, _Mylast, al);
				set_size(size() + n);
			}
			else {
				const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
				const auto _Newcapacity = getGrowthCapacity(capacity(), __old_size + n);

				auto _Newdata = Allocate(al, __old_size + n, _Newcapacity);
				const pointer _Newfirst = _Newdata.data();

				wjr::uninitialized_copy_a(_First, _Last, _Newfirst + __old_size, al);
				wjr::uninitialized_move_a(_Myfirst, _Mylast, _Newfirst, al);

				tidy();
				moveConstruct(al, std::move(_Newdata), getData());
			}
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_assign_aux(iter _First, iter _Last, std::input_iterator_tag) {
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();

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

	WJR_CONSTEXPR20 void _M_fill_assign(size_type _Count, const value_type& _Val) {
		auto& al = getAllocator();
		if (_Count > capacity()) {
			tidy();
			auto _Data = Allocate(al, _Count, _Count);
			moveConstruct(al, std::move(_Data), getData());
			wjr::uninitialized_fill_n_a(data(), _Count, _Val, al);
		}
		else if (_Count > size()) {
			wjr::fill(begin(), end(), _Val);
			wjr::uninitialized_fill_n_a(end(), _Count - size(), _Val, al);
			set_size(_Count);
		}
		else {
			_M_erase_at_end(wjr::fill_n(begin(), _Count, _Val));
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_assign_aux(iter _First, iter _Last, std::forward_iterator_tag) {
		auto _Count = static_cast<size_type>(std::distance(_First, _Last));
		auto& al = getAllocator();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();

		if (_Count <= size()) {
			wjr::copy(_First, _Last, _Myfirst);
			_M_erase_at_end(_Myfirst + _Count);
		}
		else if (_Count <= capacity()) {
			auto _Mid = _First;
			std::advance(_Mid, size());
			wjr::copy(_First, _Mid, begin());
			wjr::uninitialized_copy_a(_Mid, _Last, _Mylast, al);
			set_size(_Count);
		}
		else {
			auto _Newcapacity = getGrowthCapacity(capacity(), _Count);
			auto _Newdata = Allocate(al, _Count, _Newcapacity);
			const pointer _Newfirst = _Newdata.data();
			wjr::uninitialized_copy_a(_First, _Last, _Newfirst, al);

			tidy();
			moveConstruct(al, std::move(_Newdata), getData());
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
		if (getAllocator()== other.getAllocator()) {
			_M_move_assign(std::move(other), std::true_type{});
		}
		else {
			_M_assign_aux(
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
		const pointer _Mylast = _Myfirst + size();

		const auto __old_pos = static_cast<size_type>(_Where - _Myfirst);
		const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
		const auto __new_size = __old_size + 1;
		const auto _Newcapacity = getGrowthCapacity(__old_size, __new_size);

		auto _Newdata = Allocate(al, __new_size, _Newcapacity);

		const pointer _Newfirst = _Newdata.data();
		const pointer _Newwhere = _Newfirst + __old_pos;

		_Alty_traits::construct(al, _Newwhere, std::forward<Args>(args)...);

		wjr::uninitialized_move_a(_Myfirst, _Where, _Newfirst, al);
		wjr::uninitialized_move_a(_Where, _Mylast, _Newwhere + 1, al);

		tidy();
		moveConstruct(al, std::move(_Newdata), getData());
	}

	template<typename...Args>
	WJR_CONSTEXPR20 void _M_realloc_insert_at_end(Args&&...args) {
		auto& al = getAllocator();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();

		const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
		const auto __new_size = __old_size + 1;
		const auto _Newcapacity = getGrowthCapacity(__old_size, __new_size);

		auto _Newdata = Allocate(al, __new_size, _Newcapacity);
		const pointer _Newfirst = _Newdata.data();

		const pointer _Newwhere = _Newfirst + __old_size;
		_Alty_traits::construct(al, _Newwhere, std::forward<Args>(args)...);

		wjr::uninitialized_move_a(_Myfirst, _Mylast, _Newfirst, al);

		tidy();
		moveConstruct(al, std::move(_Newdata), getData());
	}

	WJR_CONSTEXPR20 void _M_fill_insert(iterator _Where, size_type n, const value_type& _Val) {
		if (n == 0) {
			return;
		}

		auto& al = getAllocator();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		const pointer _Myend = _Myfirst + capacity();

		const auto __rest = static_cast<size_type>(_Myend - _Mylast);

		if (__rest >= n) {
			temporary_allocator_value<Alloc> _Tmp(al, _Val);
			auto& _Copy = _Tmp.value();
			const auto __elements_after = static_cast<size_type>(_Mylast - _Where);
			if (__elements_after > n) {
				wjr::uninitialized_move_a(_Mylast - n, _Mylast, _Mylast, al);
				wjr::move_backward(_Where, _Mylast - n, _Mylast);
				wjr::fill_n(_Where, n, _Copy);
			}
			else {
				wjr::uninitialized_fill_n_a(_Mylast, n - __elements_after, _Copy, al);
				wjr::uninitialized_move_a(_Where, _Mylast, _Where + n, al);
				wjr::fill(_Where, _Mylast, _Copy);
			}
			set_size(size() + n);
		}
		else {
			const auto _Newcapacity = getGrowthCapacity(capacity(), size() + n);
			auto _Newdata = Allocate(al, size() + n, _Newcapacity);
			const pointer _Newfirst = _Newdata.data();

			const auto __old_pos = static_cast<size_type>(_Where - _Myfirst);

			wjr::uninitialized_fill_n_a(_Newfirst + __old_pos, n, _Val, al);
			wjr::uninitialized_move_a(_Myfirst, _Where, _Newfirst, al);
			wjr::uninitialized_move_a(_Where, _Mylast, _Newfirst + __old_pos + n, al);

			tidy();
			moveConstruct(al, std::move(_Newdata), getData());
		}
	}

	WJR_CONSTEXPR20 void _M_append(size_type n, const value_type& _Val) {
		auto& al = getAllocator();

		const auto _Oldsize = size();
		const auto _Oldcapacity = capacity();

		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + _Oldsize;
		const pointer _Myend = _Myfirst + _Oldcapacity;

		const auto __rest = _Oldcapacity - _Oldsize;
		const auto _Newsize = _Oldsize + n;

		if (__rest >= n) {
			wjr::uninitialized_fill_n_a(_Mylast, n, _Val, al);
			set_size(_Newsize);
		}
		else {
			auto _Newcapacity = getGrowthCapacity(_Oldcapacity, _Newsize);
			auto _Newdata = Allocate(al, _Newsize, _Newcapacity);
			const pointer _Newfirst = _Newdata.data();

			wjr::uninitialized_fill_n_a(_Newfirst + _Oldsize, n, _Val, al);
			wjr::uninitialized_move_a(_Myfirst, _Mylast, _Newfirst, al);

			tidy();
			moveConstruct(al, std::move(_Newdata), getData());
		}
	}

	template<typename Args>
	WJR_CONSTEXPR20 void _M_insert_aux(iterator _Where, Args&& args) {
		auto& al = getAllocator();
		const pointer _Mylast = data() + size();

		_Alty_traits::construct(al, _Mylast, std::move(*(_Mylast - 1)));

		wjr::move_backward(_Where, _Mylast - 1, _Mylast);
		*_Where = std::forward<Args>(args);

		set_size(size() + 1);
	}

	template<typename...Args>
	WJR_CONSTEXPR20 iterator _M_emplace_aux(const_iterator _Where, Args&&...args) {
		const auto __offset = _Where - cbegin();

		auto& al = getAllocator();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		const pointer _Myend = _Myfirst + capacity();

		if (_Mylast != _Myend) {
			if (_Where == _Mylast) {
				_Alty_traits::construct(al, _Mylast, std::forward<Args>(args)...);
				set_size(size() + 1);
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
	
};

template<typename iter,
	typename v = typename std::iterator_traits<iter>::value_type,
	typename Alloc = std::allocator<v>,
	typename Core = vector_core<v, Alloc>,
	std::enable_if_t<is_iterator_v<iter>, int> = 0>
vector(iter, iter, Alloc = Alloc())->vector<v, Core>;

template<typename T, size_t N, typename Alloc = std::allocator<T>>
using static_vector = vector<T, Alloc, vector_core<T, Alloc, vector_static_data<T, Alloc, N>>>;

template<typename T, typename Alloc, typename Core>
bool operator==(const vector<T, Alloc, Core>& lhs, const vector<T, Alloc, Core>& rhs) {
	return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T, typename Alloc, typename Core>
bool operator!=(const vector<T, Alloc, Core>& lhs, const vector<T, Alloc, Core>& rhs) {
	return !(lhs == rhs);
}

template<typename T, typename Alloc, typename Core>
bool operator<(const vector<T, Alloc, Core>& lhs, const vector<T, Alloc, Core>& rhs) {
	return wjr::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename T, typename Alloc, typename Core>
bool operator>(const vector<T, Alloc, Core>& lhs, const vector<T, Alloc, Core>& rhs) {
	return rhs < lhs;
}

template<typename T, typename Alloc, typename Core>
bool operator<=(const vector<T, Alloc, Core>& lhs, const vector<T, Alloc, Core>& rhs) {
	return !(rhs < lhs);
}

template<typename T, typename Alloc, typename Core>
bool operator>=(const vector<T, Alloc, Core>& lhs, const vector<T, Alloc, Core>& rhs) {
	return !(lhs < rhs);
}

//template<typename T, typename Alloc, size_t C = 0>
//using sso_vector = vector<T, Alloc, vector_sso_core<T, Alloc, C>>;

_WJR_END

namespace std {
	template<typename T, typename Alloc, typename Core>
	WJR_CONSTEXPR20 void swap(wjr::vector<T, Alloc, Core>& lhs, wjr::vector<T, Alloc, Core>& rhs)
		noexcept(noexcept(lhs.swap(rhs))) {
		lhs.swap(rhs);
	}

	template<typename T, typename Alloc, typename Core>
	WJR_CONSTEXPR20 typename wjr::vector<T, Alloc, Core>::size_type erase(wjr::vector<T, Alloc, Core>& c, const T& value) {
		auto it = std::remove(c.begin(), c.end(), value);
		const auto n = std::distance(it, c.end());
		c.erase(it, c.end());
		return n;
	}

	template<typename T, typename Alloc, typename Core, typename Pred>
	WJR_CONSTEXPR20 typename wjr::vector<T, Alloc, Core>::size_type erase_if(wjr::vector<T, Alloc, Core>& c, Pred pred) {
		auto it = std::remove_if(c.begin(), c.end(), pred);
		const auto n = std::distance(it, c.end());
		c.erase(it, c.end());
		return n;
	}

}

#pragma pop_macro("new")

#endif // !__WJR_VECTOR_H