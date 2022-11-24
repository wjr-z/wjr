#pragma once
#ifndef __WJR_VECTOR_H
#define __WJR_VECTOR_H

#pragma push_macro("new")
#undef new

#include <algorithm>
#include <memory>

#include <wjr/pair.h>

_WJR_BEGIN

// constexpr20 core() [noexcept];
// constexpr20 core(const allocator_type& alloc) [noexcept];
// template<typename _Alloc>
// constexpr20 core(const core& other, const _Alloc& alloc);
// constexpr20 core(core&& other) [noexcept];
// constexpr20 core(core&& other, const allocator_type& alloc) [noexcept];
// 
// constexpr20 core& operator=(const core& other) = delete; // TODO;
// constexpr20 core& operator=(core&& other) [noexcept];
// constexpr20 void swap(core& other) [noexcept];
// 
// constexpr20 ~core() [noexcept];
// constexpr20 void tidy() [noexcept];
// 
// constexpr20 allocator_core allocate(size_type new_size, size_type new_capacity);
// constexpr20 void store(allocator_core&& allocator_core);
// 
// constexpr20 pointer data() [noexcept];
// constexpr20 const_pointer data() const [noexcept];
// constexpr20 size_type size() const [noexcept];
// constexpr20 size_type capacity() const [noexcept];
// 
// constexpr20 _Alty& Getal() [noexcept];
// constexpr20 const _Alty& Getal() const [noexcept];
// constexpr20 allocator_type& get_allocator() const [noexcept];
// constexpr20 const allocator_type& get_allocator() const [noexcept];
//
// TODO :
// template<typename iter, typename Tag>
// constexpr20 void copy(iter _First, iter _Last, allocator_core& core, size_type offset, Tag tag);
// template<typename iter, typename Tag>
// constexpr20 void copy_n(iter _First, size_type _Count, allocator_core& core, size_type offset, Tag tag);
// constexpr20 bool detach() [noexcept];
//
// TODO :
// Optional : COW

template<typename T, typename Alloc>
struct vector_allocator_core {
	using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using _Alty_traits = std::allocator_traits<_Alty>;
	using pointer = typename _Alty_traits::pointer;
	using const_pointer = typename _Alty_traits::const_pointer;
	using size_type = typename _Alty_traits::size_type;
	
	WJR_CONSTEXPR20 vector_allocator_core() noexcept = default;
	vector_allocator_core(const vector_allocator_core&) = delete;
	WJR_CONSTEXPR20 vector_allocator_core(vector_allocator_core&& other) noexcept
		: _Myfirst(other._Myfirst),
		_Mylast(other._Mylast),
		_Myend(other._Myend) {
		other._Myfirst = other._Mylast = other._Myend = nullptr;
	}
	vector_allocator_core& operator=(const vector_allocator_core&) = delete;
	WJR_CONSTEXPR20 vector_allocator_core& operator=(vector_allocator_core&& other) noexcept {
		_Myfirst = other._Myfirst;
		_Mylast = other._Mylast;
		_Myend = other._Myend;
		other._Myfirst = other._Mylast = other._Myend = nullptr;
		return *this;
	}
	// 	Do not release memory here
	WJR_CONSTEXPR20 ~vector_allocator_core() noexcept = default;
	WJR_CONSTEXPR20 pointer data() noexcept {
		return _Myfirst;
	}
	WJR_CONSTEXPR20 const_pointer data() const noexcept {
		return _Myfirst;
	}
	WJR_CONSTEXPR20 size_type size() const noexcept {
		return static_cast<size_type>(_Mylast - _Myfirst);
	}
	WJR_CONSTEXPR20 size_type capacity() const noexcept {
		return static_cast<size_type>(_Myend - _Myfirst);
	}
	pointer _Myfirst = nullptr;
	pointer _Mylast = nullptr;
	pointer _Myend = nullptr;
};

template<typename T, typename Alloc>
class vector_core {
private:
	using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using _Alty_traits = std::allocator_traits<_Alty>;

public:
	using value_type = T;
	using allocator_type = Alloc;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = typename _Alty_traits::pointer;
	using const_pointer = typename _Alty_traits::const_pointer;
	
	using allocator_core = vector_allocator_core<T, Alloc>;
	constexpr static size_type min_capacity = 0;

	WJR_CONSTEXPR20 vector_core() noexcept(std::is_nothrow_constructible_v<_Alty>) = default;

	WJR_CONSTEXPR20 explicit vector_core(const allocator_type& al)
		noexcept(std::is_nothrow_constructible_v<_Alty, const allocator_type&>)
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(al),
			std::forward_as_tuple()) {}

	template<typename _Alloc>
	WJR_CONSTEXPR20 vector_core(const vector_core& other, _Alloc&& al)
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(std::forward<_Alloc>(al)),
			std::forward_as_tuple()) {
		auto& Al = Getal();
		auto& _Mydata = _Myval.second();
		
		auto& _Myfirst = _Mydata._Myfirst;
		auto& _Mylast = _Mydata._Mylast;
		auto& _Myend = _Mydata._Myend;

		_Myfirst = _Alty_traits::allocate(Al, other.capacity());
		_Mylast = _Myfirst + other.size();
		_Myend = _Myfirst + other.capacity();
		
		wjr::uninitialized_copy_a(other._Myval.second()._Myfirst, other._Myval.second()._Mylast, _Myfirst, Al);
	}
	
	WJR_CONSTEXPR20 vector_core(vector_core&& other) noexcept = default;

private:
	WJR_CONSTEXPR20 vector_core(vector_core&& other, const allocator_type& al, std::true_type) noexcept
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(al),
			std::forward_as_tuple(std::move(other._Myval.second()))) {}
	WJR_CONSTEXPR20 vector_core(vector_core&& other, const allocator_type& al, std::false_type)
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(al),
			std::forward_as_tuple()) {
		auto& Al = Getal();
		if (Al == other.Getal()) {
			_Myval.second() = std::move(other._Myval.second());
		}
		else {
			const auto _Count = other.size();
			if (_Count != 0) {
				allocator_core _Newdata = allocate(_Count, _Count);
				wjr::uninitialized_move_n_a(other.data(), _Count, _Newdata.data(), Al);
				store(std::move(_Newdata));
			}
		}
	}
public:

	WJR_CONSTEXPR20 vector_core(vector_core&& other, const allocator_type& al) 
		noexcept(noexcept(vector_core(std::declval<vector_core&&>(),
			std::declval<const allocator_type&>(), std::declval<typename _Alty_traits::is_always_equal>())))
		: vector_core(std::move(other), al, typename _Alty_traits::is_always_equal{}) {}

	WJR_CONSTEXPR20 vector_core& operator=(const vector_core& other) = delete;
	
	// do not move allocator
	WJR_CONSTEXPR20 vector_core& operator=(vector_core&& other) noexcept {
		WASSERT_LEVEL_2(this != std::addressof(other));
		tidy();
		_Myval.second() = std::move(other._Myval.second());
		return *this;
	}

	// do not swap allocator
	WJR_CONSTEXPR20 void swap(vector_core& other) noexcept {
		WASSERT_LEVEL_2(this != std::addressof(other));
		std::swap(_Myval.second(), other._Myval.second());
	}

	WJR_CONSTEXPR20 ~vector_core() noexcept { tidy(); }

	WJR_CONSTEXPR20 allocator_core allocate(size_t _Size, size_t _Capacity) {
		auto& Al = Getal();
		allocator_core _Newdata;
		_Newdata._Myfirst = Al.allocate(_Capacity);
		_Newdata._Mylast = _Newdata._Myfirst + _Size;
		_Newdata._Myend = _Newdata._Myfirst + _Capacity;
		return _Newdata;
	}

	WJR_CONSTEXPR20 void store(allocator_core&& other) noexcept {
		tidy();
		_Myval.second() = std::move(other);
	}

	WJR_CONSTEXPR20 void tidy() noexcept {
		auto& Al = Getal();
		auto& _Mydata = _Myval.second();
		auto& _Myfirst = _Mydata._Myfirst;
		auto& _Mylast = _Mydata._Mylast;
		auto& _Myend = _Mydata._Myend;
		if (_Myfirst != nullptr) {
			wjr::destroy_a(_Myfirst, _Mylast, Al);
			_Alty_traits::deallocate(Al, _Myfirst, static_cast<size_type>(_Myend - _Myfirst));
			_Myfirst = nullptr;
			_Mylast = nullptr;
			_Myend = nullptr;
		}
	}

	WJR_CONSTEXPR20 _Alty& Getal() noexcept { return _Myval.first(); }
	WJR_CONSTEXPR20 const _Alty& Getal() const noexcept { return _Myval.first(); }
	WJR_CONSTEXPR20 Alloc& get_allocator() noexcept { return _Myval.first(); }
	WJR_CONSTEXPR20 Alloc& get_allocator() const noexcept { return _Myval.first(); }

	WJR_CONSTEXPR20 pointer data() noexcept {
		return _Myval.second().data();
	}

	WJR_CONSTEXPR20 const_pointer data() const noexcept {
		return _Myval.second().data();
	}

	WJR_CONSTEXPR20 size_type size() const noexcept {
		return _Myval.second().size();
	}

	WJR_CONSTEXPR20 size_type capacity() const noexcept{
		return _Myval.second().capacity();
	}

	WJR_CONSTEXPR20 void set_size(const size_type n) noexcept {
		auto& _Mydata = _Myval.second();
		_Mydata._Mylast = _Mydata._Myfirst + n;
	}

private:
	wjr::pair<_Alty, allocator_core> _Myval;
};

template<typename T, typename Alloc, size_t _Min_capacity = 0>
class vector_sso_core {
private:
	using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using _Alty_traits = std::allocator_traits<_Alty>;

public:
	using value_type = T;
	using allocator_type = Alloc;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = typename _Alty_traits::pointer;
	using const_pointer = typename _Alty_traits::const_pointer;

private:

	struct _Medium {
		pointer _M_data;
		size_type _M_size;
	};

	static constexpr size_type alignment_value =
		std::max(std::max(alignof(_Medium), alignof(value_type)), std::max(alignof(size_type), alignof(_Alty)));

	static constexpr size_type _Possiblle_elements = 
		std::max(sizeof(_Medium) / sizeof(value_type), _Min_capacity);
	
	static constexpr size_type sso_memory = std::max(
		sizeof(_Medium),
		(_Possiblle_elements * sizeof(value_type) + alignment_value - 1) & ~(alignment_value - 1)
	);

	static constexpr size_type sso_capacity = sso_memory / sizeof(value_type);

	static_assert(sso_memory >= sizeof(_Medium), 
		"sso_memory must be greater than or equal to sizeof(_Medium)");

	constexpr static bool sso_default_mem_copy_v =
		std::conjunction_v<
		std::bool_constant<sso_memory <= 6 * sizeof(_Medium)>,
		wjr::use_default_allocator_construct<_Alty, pointer, const value_type&>,
		std::is_trivially_copy_constructible<value_type>,
		std::is_trivially_copy_constructible<_Medium>
		>;

	constexpr static bool sso_default_mem_move_v = 
		std::conjunction_v<
		std::bool_constant<sso_memory <= 6 * sizeof(_Medium)>,
		wjr::use_default_allocator_construct<_Alty, pointer, value_type&&>,
		std::is_trivially_move_constructible<value_type>,
		std::is_trivially_copy_constructible<_Medium>
		>;

	struct _Val {

		WJR_CONSTEXPR20 _Val() noexcept {}
		WJR_CONSTEXPR20 _Val(const _Val&) = delete;
		WJR_CONSTEXPR20 _Val& operator=(const _Val&) = delete;
		WJR_CONSTEXPR20 ~_Val() noexcept {}
		WJR_CONSTEXPR20 bool is_small() const noexcept {
			return _M_capacity <= sso_capacity;
		}
		WJR_CONSTEXPR20 pointer data(wjr::tag::SMALL_TAG) noexcept {
			return reinterpret_cast<pointer>(_M_byte);
		}
		WJR_CONSTEXPR20 const_pointer data(wjr::tag::SMALL_TAG) const noexcept {
			return reinterpret_cast<const_pointer>(_M_byte);
		}
		WJR_CONSTEXPR20 pointer data(wjr::tag::MEDIUM_TAG) noexcept {
			return _M_medium._M_data;
		}
		WJR_CONSTEXPR20 const_pointer data(wjr::tag::MEDIUM_TAG) const noexcept {
			return _M_medium._M_data;
		}
		WJR_CONSTEXPR20 size_type size(wjr::tag::SMALL_TAG) const noexcept {
			return _M_capacity;
		}
		WJR_CONSTEXPR20 size_type size(wjr::tag::MEDIUM_TAG) const noexcept {
			return _M_medium._M_size;
		}
		WJR_CONSTEXPR20 size_type capacity(wjr::tag::SMALL_TAG) const noexcept {
			return sso_capacity;
		}
		WJR_CONSTEXPR20 size_type capacity(wjr::tag::MEDIUM_TAG) const noexcept {
			return _M_capacity;
		}
		WJR_CONSTEXPR20 pointer data() noexcept {
			if (is_small()) {
				return data(wjr::tag::SMALL);
			}
			else {
				return data(wjr::tag::MEDIUM);
			}
		}
		WJR_CONSTEXPR20 const_pointer data() const noexcept {
			if (is_small()) {
				return data(wjr::tag::SMALL);
			}
			else {
				return data(wjr::tag::MEDIUM);
			}
		}
		WJR_CONSTEXPR20 size_type size() const noexcept {
			if (is_small()) {
				return size(wjr::tag::SMALL);
			}
			else {
				return size(wjr::tag::MEDIUM);
			}
		}
		WJR_CONSTEXPR20 size_type capacity() const noexcept {
			if (is_small()) {
				return capacity(wjr::tag::SMALL);
			}
			else {
				return capacity(wjr::tag::MEDIUM);
			}
		}

		WJR_CONSTEXPR20 void set_size(size_type n, wjr::tag::SMALL_TAG) noexcept {
			_M_capacity = n;
		}
		WJR_CONSTEXPR20 void set_size(size_type n, wjr::tag::MEDIUM_TAG) noexcept {
			_M_medium._M_size = n;
		}

		WJR_CONSTEXPR20 void set_capacity(size_type n) noexcept {
			_M_capacity = n;
		}

		union {
			uint8_t _M_byte[sso_memory];
			_Medium _M_medium;
		};

		size_type _M_capacity = 0;
	};

public:

	using allocator_core = vector_allocator_core<T, Alloc>;
	constexpr static size_type min_capacity = sso_capacity;

	// Default constructor :
	// Can be used to implement different constructors, 
	// such as constructors with initial memory allocation not equal to 0
	WJR_CONSTEXPR20 vector_sso_core() noexcept(std::is_nothrow_constructible_v<_Alty>) = default;

	WJR_CONSTEXPR20 explicit vector_sso_core(const allocator_type& al)
		noexcept(std::is_nothrow_constructible_v<_Alty, const allocator_type&>)
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(al),
			std::forward_as_tuple()) {}

	template<typename _Alloc>
	WJR_CONSTEXPR20 vector_sso_core(const vector_sso_core& other, _Alloc&& al)
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(std::forward<_Alloc>(al)),
			std::forward_as_tuple()) {
		auto& Al = Getal();
		auto& _Mydata = _Myval.second();

		auto& _Newdata = other._Myval.second();
		auto _Newptr = _Newdata.data();
		auto _Newsize = _Newdata.size();
		auto _Newcapacity = _Newdata.capacity();
		if (_Newsize <= sso_capacity) {
			_Mydata.set_size(_Newsize, wjr::tag::SMALL);
			if constexpr (sso_default_mem_copy_v) {
				memcpy(_Mydata._M_byte, _Newdata._M_byte, sso_memory);
			}
			else {
				wjr::uninitialized_copy_n_a(
					_Newptr,
					_Newsize,
					_Mydata.data(wjr::tag::SMALL),
					Al);
			}
		}
		else {
			_Mydata._M_medium._M_data = _Alty_traits::allocate(Al, other.capacity());
			_Mydata.set_size(_Newsize, wjr::tag::MEDIUM);
			_Mydata.set_capacity(_Newcapacity);
			wjr::uninitialized_copy_n_a(
				_Newptr,
				_Newsize,
				_Mydata.data(wjr::tag::MEDIUM),
				Al);
		}
	}

private:
	WJR_CONSTEXPR20 void _M_move_assign(vector_sso_core&& other) noexcept {
		auto& _Mydata = _Myval.second();
		auto& _Newdata = other._Myval.second();
		if constexpr (sso_default_mem_move_v) {
			memcpy(_Mydata._M_byte, _Newdata._M_byte, sso_memory);
			_Mydata._M_capacity = _Newdata._M_capacity;
			if (_Newdata.is_small()) {
				wjr::destroy_n_a(_Newdata.data(wjr::tag::SMALL),
					_Newdata.size(wjr::tag::SMALL), Getal());
			}
			_Newdata._M_capacity = 0;
		}
		else {
			if (_Newdata.is_small()) {
				_Mydata.set_size(_Newdata.size(wjr::tag::SMALL), wjr::tag::SMALL);
				wjr::uninitialized_move_n_a(
					_Newdata.data(wjr::tag::SMALL),
					_Newdata.size(wjr::tag::SMALL),
					_Mydata.data(wjr::tag::SMALL),
					Getal());
				wjr::destroy_n_a(_Newdata.data(wjr::tag::SMALL),
					_Newdata.size(wjr::tag::SMALL), Getal());
				_Newdata.set_size(0, wjr::tag::SMALL);
			}
			else {
				_Mydata._M_medium._M_data = _Newdata._M_medium._M_data;
				_Mydata.set_size(_Newdata.size(), wjr::tag::MEDIUM);
				_Mydata.set_capacity(_Newdata.capacity());
				_Newdata._M_medium._M_data = nullptr;
				_Newdata.set_size(0, wjr::tag::MEDIUM);
				_Newdata.set_capacity(0);
			}
		}
	}
public:

	template<typename _Alloc>
	WJR_CONSTEXPR20 vector_sso_core(vector_sso_core&& other, _Alloc&& al) noexcept
		: _Myval(std::piecewise_construct_t{},
			std::forward_as_tuple(std::forward<_Alloc>(al)),
			std::forward_as_tuple()) {
		_M_move_assign(std::move(other));
	}

	WJR_CONSTEXPR20 vector_sso_core(vector_sso_core&& other) noexcept
		: vector_sso_core(std::move(other), std::move(other.Getal())) {}

	// Copy Assignment :
	// Should not be implemented
	WJR_CONSTEXPR20 vector_sso_core& operator=(const vector_sso_core& other) = delete;

	// Move Assignment :
	WJR_CONSTEXPR20 vector_sso_core& operator=(vector_sso_core&& other) noexcept {
		tidy();
		_M_move_assign(std::move(other));
		return *this;
	}

	WJR_CONSTEXPR20 void swap(vector_sso_core& other) noexcept {
		vector_sso_core _Tmp;
		_Tmp._M_move_assign(std::move(*this));
		this->_M_move_assign(std::move(other));
		other._M_move_assign(std::move(_Tmp));
	}

	WJR_CONSTEXPR20 ~vector_sso_core() noexcept { tidy(); }

	// allocate function : 
	// Used to implement different ways of allocating memory
	WJR_CONSTEXPR20 allocator_core allocate(size_t _Size, size_t _Capacity) {
		WASSERT_LEVEL_2(sso_capacity < _Capacity);
		auto& Al = Getal();
		allocator_core _Newdata;
		_Newdata._Myfirst = _Alty_traits::allocate(Al, _Capacity);
		_Newdata._Mylast = _Newdata._Myfirst + _Size;
		_Newdata._Myend = _Newdata._Myfirst + _Capacity;
		return _Newdata;
	}

	WJR_CONSTEXPR20 void store(allocator_core&& other) noexcept {
		tidy();
		auto& _Mydata = _Myval.second();
		_Mydata._M_medium._M_data = other._Myfirst;
		_Mydata._M_medium._M_size = other.size();
		_Mydata._M_capacity = other.capacity();
	}

	WJR_CONSTEXPR20 void tidy() noexcept {
		auto& Al = Getal();
		auto& _Mydata = _Myval.second();
		if (!_Mydata.is_small()) {
			wjr::destroy_n_a(_Mydata.data(wjr::tag::MEDIUM),
				_Mydata.size(wjr::tag::MEDIUM), Al);
			_Alty_traits::deallocate(Al, _Mydata.data(wjr::tag::MEDIUM), _Mydata.capacity(wjr::tag::MEDIUM));
		}
		else {
			wjr::destroy_n_a(_Mydata.data(wjr::tag::SMALL),
				_Mydata.size(wjr::tag::SMALL), Al);
		}
		// set to small and set size to zero
		_Mydata._M_capacity = 0;
	}

	WJR_CONSTEXPR20 _Alty& Getal() noexcept { return _Myval.first(); }
	WJR_CONSTEXPR20 const _Alty& Getal() const noexcept { return _Myval.first(); }
	WJR_CONSTEXPR20 Alloc& get_allocator() noexcept { return _Myval.first(); }
	WJR_CONSTEXPR20 Alloc& get_allocator() const noexcept { return _Myval.first(); }

	WJR_CONSTEXPR20 pointer data() noexcept {
		return _Myval.second().data();
	}

	WJR_CONSTEXPR20 const_pointer data() const noexcept {
		return _Myval.second().data();
	}

	WJR_CONSTEXPR20 size_type size() const noexcept {
		return _Myval.second().size();
	}

	WJR_CONSTEXPR20 size_type capacity() const noexcept {
		return _Myval.second().capacity();
	}

	WJR_CONSTEXPR20 void set_size(const size_type n) noexcept {
		auto& _Mydata = _Myval.second();
		if (_Mydata.is_small()) {
			_Mydata.set_size(n, wjr::tag::SMALL);
		}
		else {
			_Mydata.set_size(n, wjr::tag::MEDIUM);
		}
	}

private:
	wjr::pair<_Alty, _Val> _Myval;
};

// Only realize dynamic allocation of continuous memory
template<typename T, typename Alloc = std::allocator<T>, typename Core = vector_core<T, Alloc>>
class vector {
	using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using _Alty_traits = std::allocator_traits<_Alty>;

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
	using allocator_core = typename Core::allocator_core;
	constexpr static size_type min_capacity = Core::min_capacity;
	
	WJR_CONSTEXPR20 vector() noexcept(std::is_nothrow_constructible_v<core_type>) = default;

	WJR_CONSTEXPR20 explicit vector(const allocator_type& al) noexcept
		: _Myval(al) {}

	WJR_CONSTEXPR20 explicit vector(const size_type _Count, const allocator_type& al = allocator_type())
		: _Myval(al) {
		_M_construct_n(_Count);
	}

	WJR_CONSTEXPR20 vector(size_type _Count, const value_type& _Val, const allocator_type& al = allocator_type())
		: _Myval(al) {
		_M_construct_n(_Count, _Val);
	}

	WJR_CONSTEXPR20 vector(const vector& other)
		: _Myval(other._Myval, _Alty_traits::select_on_container_copy_construction(other.Getal())) {}

	WJR_CONSTEXPR20 vector(vector&& other) noexcept = default;

	WJR_CONSTEXPR20 vector(const vector& other, const allocator_type& al)
		: _Myval(other._Myval, al) {}

	WJR_CONSTEXPR20 vector(vector&& other, const allocator_type& al) 
		noexcept(std::is_nothrow_constructible_v<core_type, core_type&&, const allocator_type&>)
		: _Myval(std::move(other._Myval), al) {}

	template<typename _Iter, std::enable_if_t<wjr::is_iterator_v<_Iter>, int> = 0>
	WJR_CONSTEXPR20 vector(_Iter _First, _Iter _Last, const allocator_type& al = allocator_type())
		: _Myval(al) {
		_M_range_init(_First, _Last, typename std::iterator_traits<_Iter>::iterator_category());
	}

	WJR_CONSTEXPR20 vector(std::initializer_list<value_type> _Ilist, const allocator_type& al = allocator_type())
		: _Myval(al) {
		_M_range_init(_Ilist.begin(), _Ilist.end(), std::random_access_iterator_tag());
	}

	WJR_CONSTEXPR20 ~vector() noexcept = default;

	WJR_CONSTEXPR20 vector& operator=(const vector& other) {
		if (this == std::addressof(other)) {
			return *this;
		}

		auto& Al = Getal();
		auto& _R_al = other.Getal();

		if constexpr (std::conjunction_v<typename _Alty_traits::propagate_on_container_copy_assignment
			,std::negation<typename _Alty_traits::is_always_equal>>) {
			if (Al != _R_al) {
				tidy();
			}
		}
		
		if constexpr (_Alty_traits::propagate_on_container_copy_assignment::value) {
			Al = _R_al;
		}

		assign(other.begin(), other.end());
		return *this;
	}

	WJR_CONSTEXPR20 vector& operator=(vector&& other) 
		noexcept(_Alty_traits::propagate_on_container_move_assignment::value
			|| _Alty_traits::is_always_equal::value) {

		using type = std::disjunction<typename _Alty_traits::propagate_on_container_move_assignment,
			typename _Alty_traits::is_always_equal>;

		if (this == std::addressof(other)) {
			return *this;
		}
		
		_M_move_assign(std::move(other), type{});
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
		return _Myval.data();
	}

	WJR_CONSTEXPR20 const_iterator begin() const noexcept {
		return _Myval.data();
	}

	WJR_CONSTEXPR20 const_iterator cbegin() const noexcept {
		return _Myval.data();
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
		return _Myval.size();
	}

	WJR_CONSTEXPR20 void resize(const size_type _Newsize) {
		const auto _Oldsize = size();
		if (_Newsize > _Oldsize) {
			_M_append(wjr::tag::VALUE_CONSTRUCT, _Newsize - _Oldsize);
		}
		else if(_Newsize < _Oldsize) {
			_M_erase_at_end(data() + _Newsize);
		}
	}

	WJR_CONSTEXPR20 void resize(const size_type _Newsize, const value_type& _Val) {
		const auto _Oldsize = size();
		if (_Newsize > _Oldsize) {
			_M_append(wjr::tag::FILL, _Newsize - _Oldsize, _Val);
		}
		else if (_Newsize < _Oldsize) {
			_M_erase_at_end(data() + _Newsize);
		}
	}

	WJR_CONSTEXPR20 void shrink_to_fit() {
		_M_shrink_to_fit();
	}

	WJR_CONSTEXPR20 size_type capacity() const noexcept {
		return _Myval.capacity();
	}

	WJR_CONSTEXPR20 bool empty() const noexcept {
		return size() == 0;
	}

	WJR_CONSTEXPR20 void reserve(size_type n) {
		const auto _Oldcapacity = capacity();
		if (_Oldcapacity < n) {
			auto& Al = Getal();
			const pointer _Myfirst = data();

			const auto __old_size = size();
			const auto _Newcapacity = _M_calc_growth(_Oldcapacity, n);
			
			auto _Newdata = allocate(__old_size, _Newcapacity);
			const pointer _Newfirst = _Newdata.data();

			wjr::uninitialized_move_a(_Myfirst, _Myfirst + __old_size, _Newfirst, Al);
			
			store(std::move(_Newdata));
		}
	}

	WJR_CONSTEXPR20 reference operator[](size_type _Pos) {
		return *(data() + _Pos);
	}

	WJR_CONSTEXPR20 const_reference operator[](size_type _Pos) const {
		return *(data() + _Pos);
	}

	WJR_CONSTEXPR20 reference at(size_type _Pos) {
		if (_Pos >= size()) {
			_WJR_THROW(std::out_of_range("vector::at"));
		}
		return (*this)[_Pos];
	}

	WJR_CONSTEXPR20 const_reference at(size_type _Pos) const {
		if (_Pos >= size()) {
			_WJR_THROW(std::out_of_range("vector::at"));
		}
		return (*this)[_Pos];
	}

	WJR_CONSTEXPR20 reference front() {
		return *data();
	}
	
	WJR_CONSTEXPR20 const_reference front() const {
		return *data();
	}

	WJR_CONSTEXPR20 reference back() {
		return *(data() + size() - 1);
	}

	WJR_CONSTEXPR20 const_reference back() const {
		return *(data() + size() - 1);
	}

	WJR_CONSTEXPR20 pointer data() noexcept {
		return _Myval.data();
	}

	WJR_CONSTEXPR20 const_pointer data() const noexcept {
		return _Myval.data();
	}

	template<typename...Args>
	WJR_CONSTEXPR20 reference emplace_back(Args&&... args) {
		auto& Al = Getal();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		const pointer _Myend = _Myfirst + capacity();
		if (_Mylast != _Myend) {
			_Alty_traits::construct(Al, std::addressof(*_Mylast), std::forward<Args>(args)...);
			_Myval.set_size(size() + 1);
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

	WJR_CONSTEXPR20 void pop_back() {
		_Myval.set_size(size() - 1);
		const pointer _Mylast = data() + size();
		_Alty_traits::destroy(Getal(), _Mylast);
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
		std::swap(_Myval, _Right._Myval);
		if constexpr (_Alty_traits::propagate_on_container_swap::value) {
			std::swap(Getal(), _Right.Getal());
		}
	}

	WJR_CONSTEXPR20 void clear() {
		_M_erase_at_end(data());
	}

	// Non-standard extension function
	// 1. non-const algorithm
	// 2. const algorithm

	// use default construct instead of value construct
	WJR_CONSTEXPR20 vector(wjr::tag::DEFAULT_CONSTRUCT_TAG, const size_type _Count, const allocator_type& al = allocator_type())
		: _Myval(al) {
		_M_construct_reserve_n(_Count);
		wjr::uninitialized_default_construct_n_a(data(), _Count, Getal());
	}

	WJR_CONSTEXPR20 vector& append(size_type _Count, const value_type& _Val) {
		_M_fill_append(_Count, _Val);
		return *this;
	}

	WJR_CONSTEXPR20 vector& append(std::initializer_list<value_type> _Ilist) {
		_M_range_append(_Ilist.begin(), _Ilist.end(), std::random_access_iterator_tag());
		return *this;
	}

	template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
	WJR_CONSTEXPR20 vector& append(iter _First, iter _Last) {
		_M_range_append(_First, _Last, typename std::iterator_traits<iter>::iterator_category());
		return *this;
	}

	WJR_CONSTEXPR20 vector& append(const vector& _Right) {
		_M_range_append(_Right.begin(), _Right.end(), std::random_access_iterator_tag());
		return *this;
	}

	WJR_CONSTEXPR20 vector& append(vector&& _Right) {
		_M_range_append(
			std::make_move_iterator(_Right.begin()), 
			std::make_move_iterator(_Right.end()), 
			std::random_access_iterator_tag());
		return *this;
	}

	WJR_CONSTEXPR20 void chop(const size_type _Count) {
		const size_type _Oldsize = size();
		if (_Count >= _Oldsize) {
			clear();
		}
		else {
			_M_erase_at_end(data() + _Oldsize - _Count);
		}
	}

	//
	WJR_CONSTEXPR20 void tidy() noexcept {
		_Myval.tidy();
	}

	WJR_CONSTEXPR20 _Alty& Getal() noexcept {
		return _Myval.Getal();
	}

	WJR_CONSTEXPR20 const _Alty& Getal() const noexcept {
		return _Myval.Getal();
	}

	WJR_CONSTEXPR20 allocator_type& get_allocator() noexcept {
		return Getal();
	}

	WJR_CONSTEXPR20 const allocator_type& get_allocator() const noexcept {
		return Getal();
	}

	WJR_CONSTEXPR20 allocator_core allocate(size_type _Newsize, size_type _Newcapacity) {
		return _Myval.allocate(_Newsize, _Newcapacity);
	}

	WJR_CONSTEXPR20 void store(allocator_core&& _Core) {
		_Myval.store(std::move(_Core));
	}

	WJR_CONSTEXPR20 void set_size(size_type _Newsize) {
		_Myval.set_size(_Newsize);
	}

private:

	WJR_CONSTEXPR20 void _M_construct_reserve_n(const size_type _Count) {
		if constexpr (min_capacity == 0) {
			auto _Newdata = allocate(_Count, _Count);
			store(std::move(_Newdata));
		}
		else {
			WASSERT_LEVEL_2(capacity() == min_capacity);
			if (min_capacity < _Count) {
				auto _Newdata = allocate(_Count, _Count);
				store(std::move(_Newdata));
			}
			else {
				_Myval.set_size(_Count);
			}
		}
	}

	template<typename...Args, std::enable_if_t<sizeof...(Args) <= 2, int> = 0>
	WJR_CONSTEXPR20 void _M_construct_n(const size_type _Count, Args&&... args) {
		if (_Count != 0) {
			auto& Al = Getal();
			_M_construct_reserve_n(_Count);
			const pointer _Ptr = data();
			if constexpr (sizeof...(Args) == 0) {
				wjr::uninitialized_value_construct_n_a(_Ptr, _Count, Al);
			}
			else if constexpr(sizeof...(Args) == 1) {
				wjr::uninitialized_fill_n_a(_Ptr, _Count, std::forward<Args>(args)..., Al);
			}
			else if constexpr(sizeof...(Args) == 2) {
				wjr::uninitialized_copy_a(std::forward<Args>(args)..., _Ptr, Al);
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
		const auto __old_pos = static_cast<size_type>(_Where - begin());
		const pointer _Mylast = data() + size();
		wjr::destroy_a(_Where, _Mylast, Getal());
		_Myval.set_size(__old_pos);
	}

	WJR_CONSTEXPR20 iterator _M_erase(iterator _Where) noexcept {
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		if (_Where + 1 != _Mylast) {
			std::move(_Where + 1, _Mylast, _Where);
		}
		wjr::destroy_at_a(_Mylast - 1, Getal());
		_Myval.set_size(size() - 1);
		return _Where;
	}

	WJR_CONSTEXPR20 iterator _M_erase(iterator _First, iterator _Last) noexcept {
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		if (_First != _Last) {
			if (_Last != _Mylast) {
				std::move(_Last, _Mylast, _First);
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
		else if(_First != _Last) {
			vector _Tmp(_First, _Last, Getal());
			insert(_Where, _Tmp.begin(), _Tmp.end());
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_range_insert(iterator _Where, iter _First, iter _Last, std::forward_iterator_tag) {
		if (_First != _Last) {
			auto& Al = Getal();
			const pointer _Myfirst = data();
			const pointer _Mylast = _Myfirst + size();
			const pointer _Myend = _Myfirst + capacity();

			const auto n = static_cast<size_type>(std::distance(_First, _Last));
			const auto __rest = static_cast<size_type>(_Myend - _Mylast);

			if (__rest >= n) {
				const auto __elements_after = static_cast<size_type>(_Mylast - _Where);
				auto __old_last = _Mylast;
				if (__elements_after > n) {
					wjr::uninitialized_move_a(_Mylast - n, _Mylast, _Mylast, Al);
					std::move_backward(_Where, __old_last - n, __old_last);
					std::copy(_First, _Last, _Where);
				}
				else {
					auto _Mid = _First;
					std::advance(_Mid, __elements_after);
					wjr::uninitialized_copy_a(_Mid, _Last, _Mylast, Al);
					wjr::uninitialized_move_a(_Where, __old_last, _Mylast + n - __elements_after, Al);
					std::copy(_First, _Mid, _Where);
				}
				_Myval.set_size(size() + n);
			}
			else {
				const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
				const auto __old_pos = static_cast<size_type>(_Where - _Myfirst);
				const auto _Newcapacity = _M_calc_growth(capacity(), __old_size + n);
				auto _Newdata = allocate(__old_size + n, _Newcapacity);
				const pointer _Newfirst = _Newdata.data();
				
				wjr::uninitialized_copy_a(_First, _Last, _Newfirst + __old_pos, Al);
				wjr::uninitialized_move_a(_Myfirst, _Where, _Newfirst, Al);
				wjr::uninitialized_move_a(_Where, _Mylast, _Newfirst + __old_pos + n, Al);

				store(std::move(_Newdata));
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
			auto& Al = Getal();
			const pointer _Myfirst = data();
			const pointer _Mylast = _Myfirst + size();
			const pointer _Myend = _Myfirst + capacity();

			const auto n = static_cast<size_type>(std::distance(_First, _Last));
			const auto __rest = static_cast<size_type>(_Myend - _Mylast);

			if (__rest >= n) {
				wjr::uninitialized_copy_a(_First, _Last, _Mylast, Al);
				_Myval.set_size(size() + n);
			}
			else {
				const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
				const auto _Newcapacity = _M_calc_growth(capacity(), __old_size + n);
				
				auto _Newdata = allocate(__old_size + n, _Newcapacity);
				const pointer _Newfirst = _Newdata.data();

				wjr::uninitialized_copy_a(_First, _Last, _Newfirst + __old_size, Al);
				wjr::uninitialized_move_a(_Myfirst, _Mylast, _Newfirst, Al);

				store(std::move(_Newdata));
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
		if (_Count > capacity()) {
			vector _Tmp(_Count, _Val, Getal());
			_Myval = std::move(_Tmp._Myval);
		}
		else if (_Count > size()) {
			std::fill(begin(), end(), _Val);
			wjr::uninitialized_fill_n_a(end(), _Count - size(), _Val, Getal());
			_Myval.set_size(_Count);
		}
		else {
			_M_erase_at_end(std::fill_n(begin(), _Count, _Val));
		}
	}

	template<typename iter>
	WJR_CONSTEXPR20 void _M_assign_aux(iter _First, iter _Last, std::forward_iterator_tag) {
		auto _Count = static_cast<size_type>(std::distance(_First, _Last));
		auto& Al = Getal();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();

		if (_Count <= size()) {
			std::copy(_First, _Last, _Myfirst);
			_M_erase_at_end(_Myfirst + _Count);
		}
		else if (_Count <= capacity()) { 
			auto _Mid = _First;
			std::advance(_Mid, size());
			std::copy(_First, _Mid, begin());
			wjr::uninitialized_copy_a(_Mid, _Last, _Mylast, Al);
			_Myval.set_size(_Count);
		}
		else {
			auto _Newcapacity = _M_calc_growth(capacity(), _Count);
			auto _Newdata = allocate(_Count, _Newcapacity);
			const pointer _Newfirst = _Newdata.data();
			wjr::uninitialized_copy_a(_First, _Last, _Newfirst, Al);
			store(std::move(_Newdata));
		}
	}

	WJR_CONSTEXPR20 void _M_move_assign(vector&& other, std::true_type){
		_Myval = std::move(other._Myval);
		if constexpr (_Alty_traits::propagate_on_container_move_assignment::value) {
			Getal() = std::move(other.Getal());
		}
	}

	WJR_CONSTEXPR20 void _M_move_assign(vector&& other, std::false_type) {
		if (Getal() == other.Getal()) {
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

	WJR_CONSTEXPR20 static size_type _M_calc_growth(const size_type c, const size_type n) {
		return std::max(c + c, n);
	}

	template<typename...Args>
	WJR_CONSTEXPR20 void _M_realloc_insert(iterator _Where, Args&&...args) {
		auto& Al = Getal();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();

		const auto __old_pos = static_cast<size_type>(_Where - _Myfirst);
		const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
		const auto __new_size = __old_size + 1;
		const auto _Newcapacity = _M_calc_growth(__old_size, __new_size);

		auto _Newdata = allocate(__new_size, _Newcapacity);

		const pointer _Newfirst = _Newdata.data();
		const pointer _Newwhere = _Newfirst + __old_pos;
		
		_Alty_traits::construct(Al, _Newwhere, std::forward<Args>(args)...);

		wjr::uninitialized_move_a(_Myfirst, _Where, _Newfirst, Al);
		wjr::uninitialized_move_a(_Where, _Mylast, _Newwhere + 1, Al);
		
		store(std::move(_Newdata));
	}

	template<typename...Args>
	WJR_CONSTEXPR20 void _M_realloc_insert_at_end(Args&&...args) {
		auto& Al = Getal();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();

		const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
		const auto __new_size = __old_size + 1;
		const auto _Newcapacity = _M_calc_growth(__old_size, __new_size);

		auto _Newdata = allocate(__new_size, _Newcapacity);
		const pointer _Newfirst = _Newdata.data();

		const pointer _Newwhere = _Newfirst + __old_size;
		_Alty_traits::construct(Al, _Newwhere, std::forward<Args>(args)...);

		wjr::uninitialized_move_a(_Myfirst, _Mylast, _Newfirst, Al);

		store(std::move(_Newdata));
	}

	WJR_CONSTEXPR20 void _M_fill_insert(iterator _Where, size_type n, const value_type& _Val) {
		if (n == 0) {
			return;
		}
		
		auto& Al = Getal();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		const pointer _Myend = _Myfirst + capacity();

		const auto __rest = static_cast<size_type>(_Myend - _Mylast);
		
		if (__rest >= n) {
			temporary_allocator_value<Alloc> _Tmp(Al, _Val);
			auto& _Copy = _Tmp.value();
			const auto __elements_after = static_cast<size_type>(_Mylast - _Where);
			if (__elements_after > n) {
				wjr::uninitialized_move_a(_Mylast - n, _Mylast, _Mylast, Al);
				std::move_backward(_Where, _Mylast - n, _Mylast);
				std::fill_n(_Where, n, _Copy);
			}
			else {
				wjr::uninitialized_fill_n_a(_Mylast, n - __elements_after, _Copy, Al);
				wjr::uninitialized_move_a(_Where, _Mylast, _Where + n, Al);
				std::fill(_Where, _Mylast, _Copy);
			}
			_Myval.set_size(size() + n);
		}
		else {
			const auto _Newcapacity = _M_calc_growth(capacity(), size() + n);
			auto _Newdata = allocate(size() + n, _Newcapacity);
			const pointer _Newfirst = _Newdata.data();

			const auto __old_pos = static_cast<size_type>(_Where - _Myfirst);

			wjr::uninitialized_fill_n_a(_Newfirst + __old_pos, n, _Val, Al);
			wjr::uninitialized_move_a(_Myfirst, _Where, _Newfirst, Al);
			wjr::uninitialized_move_a(_Where, _Mylast, _Newfirst + __old_pos + n, Al);
			
			store(std::move(_Newdata));
		}
	}

	template<typename Tag, typename...Args, std::enable_if_t<wjr::tag::is_set_v<Tag>, int> = 0>
	WJR_CONSTEXPR20 void _M_append(Tag, size_type n, Args&&...args) {
		if (n == 0) {
			return;
		}

		auto& Al = Getal();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		const pointer _Myend = _Myfirst + capacity();

		const auto __rest = static_cast<size_type>(_Myend - _Mylast);
		if (__rest >= n) {
			wjr::uninitialized_n_a(Tag{}, _Mylast, n, std::forward<Args>(args)..., Al);
			_Myval.set_size(size() + n);
		}
		else {
			auto _Newcapacity = _M_calc_growth(capacity(), size() + n);
			auto _Newdata = allocate(size() + n, _Newcapacity);
			const pointer _Newfirst = _Newdata.data();

			wjr::uninitialized_n_a(Tag{}, _Newfirst + size(), n, std::forward<Args>(args)..., Al);
			wjr::uninitialized_move_a(_Myfirst, _Mylast, _Newfirst, Al);

			store(std::move(_Newdata));
		}
	}

	WJR_CONSTEXPR20 void _M_shrink_to_fit() {
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		const pointer _Myend = _Myfirst + capacity();

		if constexpr (min_capacity == 0) {
			if (_Mylast != _Myend) {
				if (_Myfirst == _Mylast) {
					tidy();
				}
				else {
					auto _Newcapacity = _Mylast - _Myfirst;
					auto _Newdata = allocate(size(), _Newcapacity);
					const pointer _Newfirst = _Newdata.data();
					wjr::uninitialized_move_a(_Myfirst, _Mylast, _Newfirst, Getal());
					store(std::move(_Newdata));
				}
			}
		}
		else {
			if (_Mylast != _Myend && size() > min_capacity) {
				if (_Myfirst == _Mylast) {
					tidy();
				}
				else {
					auto _Newcapacity = _Mylast - _Myfirst;
					auto _Newdata = allocate(size(), _Newcapacity);
					const pointer _Newfirst = _Newdata.data();
					wjr::uninitialized_move_a(_Myfirst, _Mylast, _Newfirst, Getal());
					store(std::move(_Newdata));
				}
			}
		}
	}

	template<typename Args>
	WJR_CONSTEXPR20 void _M_insert_aux(iterator _Where, Args&& args) {
		auto& Al = Getal();
		const pointer _Mylast = data() + size();

		_Alty_traits::construct(Al, _Mylast, std::move(*(_Mylast - 1)));

		std::move_backward(_Where, _Mylast - 1, _Mylast);
		*_Where = std::forward<Args>(args);
		
		_Myval.set_size(size() + 1);
	}

	template<typename...Args>
	WJR_CONSTEXPR20 iterator _M_emplace_aux(const_iterator _Where, Args&&...args) {
		const auto __offset = _Where - cbegin();
		
		auto& Al = Getal();
		const pointer _Myfirst = data();
		const pointer _Mylast = _Myfirst + size();
		const pointer _Myend = _Myfirst + capacity();

		if (_Mylast != _Myend) {
			if (_Where == _Mylast) {
				_Alty_traits::construct(Al, _Mylast, std::forward<Args>(args)...);
				_Myval.set_size(size() + 1);
			}
			else {
				temporary_allocator_value<Alloc> _Tmp(Al, std::forward<Args>(args)...);
				_M_insert_aux(begin() + __offset, std::move(_Tmp.value()));
			}
		}
		else {
			_M_realloc_insert(begin() + __offset, std::forward<Args>(args)...);
		}
		
		return begin() + __offset;
	}

	core_type _Myval;
};

template<typename iter, 
	typename v = typename std::iterator_traits<iter>::value_type, 
	typename Alloc = std::allocator<v>,
	typename Core = vector_core<v, Alloc>,
	std::enable_if_t<is_iterator_v<iter>, int> = 0>
vector(iter, iter, Alloc = Alloc())->vector<v, Core>;

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
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
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

template<typename T, typename Alloc, size_t C = 0>
using sso_vector = vector<T, Alloc, vector_sso_core<T, Alloc, C>>;

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