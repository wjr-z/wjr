#pragma once
#ifndef __WJR_ALLOCATOR_H
#define __WJR_ALLOCATOR_H

#include <new>
#include <memory>

#include <wjr/type_traits.h>
#include <wjr/compressed_pair.h>

_WJR_BEGIN

template<typename _Ty>
struct __aligned_helper;

template<>
struct __aligned_helper<void> {
	constexpr static size_t value = std::max((size_t)(16), (size_t)(alignof(max_align_t)));
};

template<typename _Ty>
struct __aligned_helper {
	constexpr static size_t value = std::max((size_t)alignof(_Ty), __aligned_helper<void>::value);
};

#if defined(_DEBUG)
#define WJR_TEST_ALLOCATOR
#endif

#if defined(WJR_TEST_ALLOCATOR)
struct __test_allocator {
	void test()const;
	~__test_allocator();
	long long _Count = 0;
};
extern __test_allocator __test_allocator_instance;
#endif

template<typename _Ty, size_t _Al = __aligned_helper<_Ty>::value, size_t _Off = 0>
class aligned_allocator {
public:
	static_assert(!std::is_const_v<_Ty>, "The C++ Standard forbids containers of const elements "
		"because allocator<const T> is ill-formed.");
	static_assert(_Al >= 8, "The alignment must be greater than or equal to 8.");
	static_assert(_Off >= 0 && _Off < _Al,
		"The offset must be greater than or equal to 0 and less than the alignment.");
	static_assert((_Al & (_Al - 1)) == 0, "The alignment must be a power of 2.");

	using value_type = _Ty;

#if !defined(WJR_CPP_20)
	using pointer = _Ty*;
	using const_pointer = const _Ty*;

	using reference = _Ty&;
	using const_reference = const _Ty&;
#endif // !WJR_CPP_20

	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using propagate_on_container_move_assignment = std::true_type;
	using is_always_equal = std::true_type;

	template <typename _Other>
	struct rebind {
		using other = aligned_allocator<_Other, _Al, _Off>;
	};

#if !defined(WJR_CPP_20)
	WJR_NODISCARD _Ty* address(_Ty& _Val) const noexcept {
		return std::addressof(_Val);
	}

	WJR_NODISCARD const _Ty* address(const _Ty& _Val) const noexcept {
		return std::addressof(_Val);
	}
#endif // !WJR_CPP_20

	constexpr aligned_allocator() noexcept {}

	constexpr aligned_allocator(const aligned_allocator&) noexcept = default;
	template <typename _Other>
	constexpr aligned_allocator(const aligned_allocator<_Other, _Al, _Off>&) noexcept {}
	WJR_CONSTEXPR20 ~aligned_allocator() = default;
	WJR_CONSTEXPR20 aligned_allocator& operator=(const aligned_allocator&) = default;

	WJR_CONSTEXPR20 void deallocate(_Ty* const _Ptr, const size_t _Count) {
#if defined(WJR_TEST_ALLOCATOR)
		__test_allocator_instance._Count -= _Count * sizeof(_Ty) + _Off;
#endif
		WJR_ASSUME(reinterpret_cast<uintptr_t>(_Ptr) % _Al == _Off);
		auto ptr = reinterpret_cast<char*>(_Ptr) - _Off;
		::operator delete(ptr, _Count * sizeof(_Ty) + _Off, static_cast<std::align_val_t>(_Al));
	}

	WJR_NODISCARD WJR_CONSTEXPR20 WJR_MALLOC _Ty* allocate(const size_t _Count) {
#if defined(WJR_TEST_ALLOCATOR)
		__test_allocator_instance._Count += _Count * sizeof(_Ty) + _Off;
#endif
		auto ptr = static_cast<char*>(::operator new(_Count * sizeof(_Ty) + _Off, 
			static_cast<std::align_val_t>(_Al))) + _Off;
		WJR_ASSUME(reinterpret_cast<uintptr_t>(ptr) % _Al == _Off);
		return reinterpret_cast<_Ty*>(ptr);
	}

#if !defined(WJR_CPP_20)
	WJR_NODISCARD size_t max_size() const noexcept {
		return static_cast<size_t>(-1) / sizeof(_Ty);
	}
#endif // !WJR_CPP_20
};

template <size_t _Al, size_t _Off>
class aligned_allocator<void, _Al, _Off> {
public:
	using value_type = void;
#if !defined(WJR_CPP_20)
	using pointer = void*;
	using const_pointer = const void*;
#endif // !WJR_CPP_20

	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using propagate_on_container_move_assignment = std::true_type;
	using is_always_equal = std::true_type;

#if !defined(WJR_CPP_20)
	template <class _Other>
	struct rebind {
		using other = aligned_allocator<_Other>;
	};
#endif // !WJR_CPP_20
};

template <typename _Ty, typename _Other, size_t _Al, size_t _Off>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 bool operator==(
	const aligned_allocator<_Ty, _Al, _Off>&,
	const aligned_allocator<_Other, _Al, _Off>&) noexcept {
	return true;
}

template <typename _Ty, size_t _Al1, size_t _Off1, typename _Other, size_t _Al2, size_t _Off2>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 bool operator==(
	const aligned_allocator<_Ty, _Al1, _Off1>&,
	const aligned_allocator<_Other, _Al2, _Off2>&) noexcept {
	return false;
}

template <typename _Ty, size_t _Al1, size_t _Off1, typename _Other, size_t _Al2, size_t _Off2>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 bool operator!=(
	const aligned_allocator<_Ty, _Al1, _Off1>& lhs,
	const aligned_allocator<_Other, _Al2, _Off2>& rhs) noexcept {
	return !(lhs == rhs);
}

template<typename T, size_t _Al, size_t _Off>
struct is_default_allocator<aligned_allocator<T, _Al, _Off>> : std::true_type {};

template <int __inst>
class __malloc_alloc_template__ {
private:

	static void* _S_oom_malloc(size_t);
	static void* _S_oom_realloc(void*, size_t);

#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG
	static void (*__malloc_alloc_oom_handler)();
#endif

public:

	static void* allocate(size_t __n) {
		void* __result = malloc(__n);
		if (WJR_UNLIKELY(0 == __result)) __result = _S_oom_malloc(__n);
		return __result;
	}

	static void deallocate(void* __p, size_t /* __n */) {
		free(__p);
	}

	static void* reallocate(void* __p, size_t /* old_sz */, size_t __new_sz) {
		void* __result = realloc(__p, __new_sz);
		if (WJR_UNLIKELY(0 == __result)) __result = _S_oom_realloc(__p, __new_sz);
		return __result;
	}

	static void (*__set_malloc_handler(void (*__f)()))()
	{
		void (*__old)() = __malloc_alloc_oom_handler;
		__malloc_alloc_oom_handler = __f;
		return(__old);
	}
};

// malloc_alloc out-of-memory handling

#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG
template <int __inst>
void (*__malloc_alloc_template__<__inst>::__malloc_alloc_oom_handler)() = 0;
#endif

template <int __inst>
void*
__malloc_alloc_template__<__inst>::_S_oom_malloc(size_t __n) {
	void (*__my_malloc_handler)();
	void* __result;

	for (;;) {
		__my_malloc_handler = __malloc_alloc_oom_handler;
		(*__my_malloc_handler)();
		__result = malloc(__n);
		if (__result) return(__result);
	}
}

template <int __inst>
void* __malloc_alloc_template__<__inst>::_S_oom_realloc(void* __p, size_t __n) {
	void (*__my_malloc_handler)();
	void* __result;

	for (;;) {
		__my_malloc_handler = __malloc_alloc_oom_handler;

		(*__my_malloc_handler)();
		__result = realloc(__p, __n);
		if (__result) return(__result);
	}
}

typedef __malloc_alloc_template__<0> malloc_alloc;

enum { ALLOC_ALIGN = 8 };
enum { ALLOC_MAX_BYTES = 256 };
enum { ALLOC_NFRELISTS = ALLOC_MAX_BYTES / ALLOC_ALIGN };

template <bool threads, int inst>
class __default_alloc_template__;

template<bool threads>
class __basic_default_alloc_template__;

template<>
class __basic_default_alloc_template__<true> {
private:
	template<bool, int>
	friend class __default_alloc_template__;
	enum { __ALIGN = ALLOC_ALIGN };
	enum { __MAX_BYTES = ALLOC_MAX_BYTES };
	enum { __NFREELISTS = ALLOC_NFRELISTS };
	union obj {
		union obj* free_list_link;
		char client_data[1];
	};
	thread_local static obj* volatile free_list[__NFREELISTS];
	// Chunk allocation state.
	thread_local static char* start_free;
	thread_local static char* end_free;
	thread_local static size_t heap_size;
};

template<>
class __basic_default_alloc_template__<false> {
private:
	template<bool, int>
	friend class __default_alloc_template__;
	enum { __ALIGN = ALLOC_ALIGN };
	enum { __MAX_BYTES = ALLOC_MAX_BYTES };
	enum { __NFREELISTS = ALLOC_NFRELISTS };
	union obj {
		union obj* free_list_link;
		char client_data[1];
	};
	static obj* volatile free_list[__NFREELISTS];
	// Chunk allocation state.
	static char* start_free;
	static char* end_free;
	static size_t heap_size;
};

template <bool threads, int inst>
class __default_alloc_template__ {
private:
	using base = __basic_default_alloc_template__<threads>;
	using obj = typename base::obj;

	enum { __ALIGN = ALLOC_ALIGN };
	enum { __MAX_BYTES = ALLOC_MAX_BYTES };
	enum { __NFREELISTS = ALLOC_NFRELISTS };

	static inline size_t ROUND_UP(size_t bytes) {
		return (((bytes)+__ALIGN - 1) & ~(__ALIGN - 1));
	}

	static inline size_t FREELIST_INDEX(size_t bytes) {
		return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
	}

	// Returns an object of size n, and optionally adds to size n free list.
	static void* refill(size_t n) noexcept;

	// Allocates a chunk for nobjs of size "size".  nobjs may be reduced
	// if it is inconvenient to allocate the requested number.
	static char* chunk_alloc(size_t size, int& nobjs) noexcept;

public:
	static void* allocate(size_t n) noexcept //n must be > 0
	{
		if (n > (size_t)__MAX_BYTES) {
			return malloc_alloc::allocate(n);
		}
		obj* volatile* my_free_list = base::free_list + FREELIST_INDEX(n);
		obj* result = *my_free_list;
		if (result != nullptr) {
			*my_free_list = result->free_list_link;
			return result;
		}
		return refill(ROUND_UP(n));
	}

	static void deallocate(void* p, size_t n) noexcept //p may not be 0
	{

		if (n > (size_t)__MAX_BYTES) {
			malloc_alloc::deallocate(p, n);
			return;
		}

		obj* q = (obj*)p;
		obj* volatile* my_free_list = base::free_list + FREELIST_INDEX(n);
		q->free_list_link = *my_free_list;
		*my_free_list = q;
	}
};

//----------------------------------------------
// We allocate memory in large chunks in order to
// avoid fragmentingthe malloc heap too much.
// We assume that size is properly aligned.
// We hold the allocation lock.
//----------------------------------------------
template <bool threads, int inst>
char* __default_alloc_template__<threads, inst>::
chunk_alloc(size_t size, int& nobjs) noexcept {
	char* result;
	size_t total_bytes = size * nobjs;
	const auto bytes_left = static_cast<size_t>(base::end_free - base::start_free);

	if (bytes_left >= total_bytes) {
		result = base::start_free;
		base::start_free += total_bytes;
		return (result);
	}

	if (bytes_left >= size) {
		nobjs = static_cast<int>(bytes_left / size);
		total_bytes = size * nobjs;
		result = base::start_free;
		base::start_free += total_bytes;
		return (result);
	}

	const size_t bytes_to_get =
		2 * total_bytes + ROUND_UP(base::heap_size >> 4);
	// Try to make use of the left-over piece.
	if (bytes_left > 0) {
		obj* volatile* my_free_list =
			base::free_list + FREELIST_INDEX(bytes_left);

		((obj*)base::start_free)->free_list_link = *my_free_list;
		*my_free_list = (obj*)base::start_free;
	}
	base::start_free = (char*)malloc(bytes_to_get);
	if (WJR_UNLIKELY(0 == base::start_free)) {
		obj* volatile* my_free_list, * p;

		//Try to make do with what we have. That can't
		//hurt. We do not try smaller requests, since that tends
		//to result in disaster on multi-process machines.
		for (int i = static_cast<int>(size); i <= __MAX_BYTES; i += __ALIGN) {
			my_free_list = base::free_list + FREELIST_INDEX(i);
			p = *my_free_list;
			if (0 != p) {
				*my_free_list = p->free_list_link;
				base::start_free = (char*)p;
				base::end_free = base::start_free + i;
				return (chunk_alloc(size, nobjs));
				//Any leftover piece will eventually make it to the
				//right free list.
			}
		}
		base::end_free = 0; //In case of exception.
		base::start_free = (char*)malloc(bytes_to_get);
		//This should either throw an exception or
		//remedy the situation. Thus we assume it
		//succeeded.
	}
	base::heap_size += bytes_to_get;
	base::end_free = base::start_free + bytes_to_get;
	return (chunk_alloc(size, nobjs));
}

//----------------------------------------------
// Returns an object of size n, and optionally adds
// to size n free list.We assume that n is properly aligned.
// We hold the allocation lock.
//----------------------------------------------
template <bool threads, int inst>
void* __default_alloc_template__<threads, inst>::
refill(size_t n) noexcept {
	int nobjs = 20;
	char* chunk = chunk_alloc(n, nobjs);
	obj* current_obj;
	obj* next_obj;

	if (1 == nobjs) return (chunk);
	obj* volatile* my_free_list = base::free_list + FREELIST_INDEX(n);

	//Build free list in chunk
	obj* result = (obj*)chunk;

	*my_free_list = current_obj = (obj*)(chunk + n);
	nobjs -= 2;
	while (nobjs) {
		--nobjs;
		next_obj = (obj*)((char*)current_obj + n);
		current_obj->free_list_link = next_obj;
		current_obj = next_obj;
	}
	current_obj->free_list_link = 0;
	return (result);
}

// threads = false : Share one memory pool
// threads = true : Each thread shares one memory pool
template <typename Ty, bool threads>
class basic_allocator {
private:
	using allocator_type = __default_alloc_template__<threads, 0>;
public:

	using value_type = Ty;
	using reference = Ty&;
	using const_reference = const Ty&;
	using pointer = Ty*;
	using const_pointer = const Ty*;
	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using propagate_on_container_move_assignment = std::true_type;
	using is_always_equal = std::true_type;

	template <typename _Other>
	struct rebind {
		using other = basic_allocator<_Other, threads>;
	};

#if !defined(WJR_CPP_20)
	WJR_NODISCARD Ty* address(Ty& _Val) const noexcept {
		return std::addressof(_Val);
	}

	WJR_NODISCARD const Ty* address(const Ty& _Val) const noexcept {
		return std::addressof(_Val);
	}
#endif // !WJR_CPP_20

	constexpr basic_allocator() noexcept = default;
	constexpr basic_allocator(const basic_allocator&) noexcept = default;
	template <class _Other>
	constexpr explicit basic_allocator(const basic_allocator<_Other, threads>&) noexcept {}
	~basic_allocator() = default;
	basic_allocator& operator=(const basic_allocator&) noexcept = default;

	WJR_NODISCARD WJR_CONSTEXPR20 WJR_MALLOC Ty* allocate(size_t n) const noexcept {
#if defined(WJR_TEST_ALLOCATOR)
		__test_allocator_instance._Count += n;
#endif
		if (!is_constant_evaluated()) {
			if (WJR_UNLIKELY(0 == n)) {
				return nullptr;
			}
			return static_cast<Ty*>(allocator_type::allocate(sizeof(Ty) * n));
		}
		return std::allocator<Ty>().allocate(n);
	}

	WJR_CONSTEXPR20 void deallocate(Ty* ptr, size_t n) const noexcept {
#if defined(WJR_TEST_ALLOCATOR)
		__test_allocator_instance._Count -= n;
#endif
		if (!is_constant_evaluated()) {
			if (WJR_UNLIKELY(0 == n)) return;
			return allocator_type::deallocate(static_cast<void*>(ptr), sizeof(Ty) * n);
		}
		return std::allocator<Ty>().deallocate(ptr, n);
	}

	constexpr size_t max_size()const {
		return static_cast<size_t>(-1) / sizeof(Ty);
	}

};

template<typename T, typename U, bool t1, bool t2>
constexpr bool operator==(const basic_allocator<T, t1>&, const basic_allocator<U, t2>&) {
	return t1 == t2;
}

template<typename T, typename U, bool t1, bool t2>
constexpr bool operator!=(const basic_allocator<T, t1>&, const basic_allocator<U, t2>&) {
	return t1 != t2;
}

template<typename T, bool f>
struct is_default_allocator<basic_allocator<T, f>> : std::true_type {};

// non thread safe allocator
template<typename T>
using nsallocator = basic_allocator<T, false>;

template<typename T>
using sallocator = basic_allocator<T, true>;

template<typename T, typename Alloc>
class unique_ptr_with_allocator_delete 
	: private _Pair_wrapper<0, typename std::allocator_traits<Alloc>::template rebind_alloc<T>>{
	using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
	using _Mybase = _Pair_wrapper<0, _Alty>;
	using _Alty_traits = std::allocator_traits<Alloc>;
public:
	using allocator_type = Alloc;
	constexpr unique_ptr_with_allocator_delete() 
		noexcept(std::is_nothrow_default_constructible_v<_Alty>) : _Mybase() {}
	constexpr unique_ptr_with_allocator_delete(const allocator_type& _Al)
		noexcept(std::is_nothrow_constructible_v<_Alty, const allocator_type&>) : _Mybase(_Al) {}
	constexpr unique_ptr_with_allocator_delete(const unique_ptr_with_allocator_delete&) = default;
	constexpr unique_ptr_with_allocator_delete(unique_ptr_with_allocator_delete&&) = default;
	constexpr unique_ptr_with_allocator_delete& operator=(const unique_ptr_with_allocator_delete&) = default;
	constexpr unique_ptr_with_allocator_delete& operator=(unique_ptr_with_allocator_delete&&) = default;
	constexpr void operator()(T* ptr) {
		destroy_at(_Mybase::value(), ptr);
		_Mybase::value().deallocate(ptr, 1);
	}
};

template<typename T, typename Alloc>
class unique_ptr_with_allocator_delete<T[], Alloc> {
	using _Alty = typename std::allocator_traits<Alloc>::template rebind<T>;
	using _Alty_traits = std::allocator_traits<Alloc>;
public:
	using allocator_type = Alloc;
	constexpr unique_ptr_with_allocator_delete() = default;
	constexpr unique_ptr_with_allocator_delete(size_t n, const allocator_type& _Al = allocator_type())
		noexcept(std::is_nothrow_constructible_v<_Alty, const allocator_type&>)
		: m_value(std::piecewise_construct_t(),
			std::forward_as_tuple(_Al),
			std::forward_as_tuple(n)) {}
	constexpr unique_ptr_with_allocator_delete(const unique_ptr_with_allocator_delete&) = default;
	constexpr unique_ptr_with_allocator_delete(unique_ptr_with_allocator_delete&&) = default;
	constexpr unique_ptr_with_allocator_delete& operator=(const unique_ptr_with_allocator_delete&) = default;
	constexpr unique_ptr_with_allocator_delete& operator=(unique_ptr_with_allocator_delete&&) = default;
	constexpr void operator()(T* ptr) {
		destroy_n(m_value.first(), ptr, m_value.second());
		m_value.first().deallocate(ptr, m_value.second());
	}
private:
	compressed_pair<_Alty, size_t> m_value;
};

template<typename T>
using unique_ptr_with_nsallocator_delete = unique_ptr_with_allocator_delete<T, nsallocator<T>>;

template<typename T>
using unique_ptr_with_sallocator_delete = unique_ptr_with_allocator_delete<T, sallocator<T>>;

template<typename T>
using unique_ptr_with_nsallocator = std::unique_ptr<T, unique_ptr_with_nsallocator_delete<T>>;

template<typename T>
using unique_ptr_with_sallocator = std::unique_ptr<T, unique_ptr_with_sallocator_delete<T>>;

template<typename T, typename...Args, std::enable_if_t<!std::is_array_v<T>, int> = 0>
constexpr auto make_unique_with_nsallocator(Args&&...args) {
	auto al = nsallocator<T>();
	auto _Ptr = al.allocate(1);
	construct_at(al, _Ptr, std::forward<Args>(args)...);
	return unique_ptr_with_nsallocator<T>(_Ptr);
}

// notice that initialization will not occur
template<typename T, std::enable_if_t<std::is_array_v<T>&& std::extent_v<T> == 0, int> = 0>
constexpr auto make_unique_with_nsallocator(size_t n) {
	using elem = std::remove_extent_t<T>;
	auto al = nsallocator<elem>();
	auto _Ptr = al.allocate(n);
	uninitialized_default_construct_n(al, _Ptr, n);
	return unique_ptr_with_nsallocator<T>(_Ptr, unique_ptr_with_nsallocator_delete<T>(n));
}

template<typename T, typename...Args, std::enable_if_t<!std::is_array_v<T>, int> = 0>
constexpr auto make_unique_with_sallocator(Args&&...args) {
	auto al = sallocator<T>();
	auto _Ptr = al.allocate(1);
	construct_at(al, _Ptr, std::forward<Args>(args)...);
	return unique_ptr_with_sallocator<T>(_Ptr);
}

// notice that initialization will not occur
template<typename T, std::enable_if_t<std::is_array_v<T>&& std::extent_v<T> == 0, int> = 0>
constexpr auto make_unique_with_sallocator(size_t n) {
	using elem = std::remove_extent_t<T>;
	auto al = nsallocator<elem>();
	auto _Ptr = al.allocate(n);
	uninitialized_default_construct_n(al, _Ptr, n);
	return unique_ptr_with_sallocator<T>(_Ptr, unique_ptr_with_sallocator_delete<T>(n));
}

_WJR_END

#endif // __WJR_ALLOCATOR_H