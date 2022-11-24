#ifndef __WJR_MALLOCATOR_H
#define __WJR_MALLOCATOR_H

#include <cstdint>
#include <cstdlib>
#include <utility>
#include <memory>

#include <wjr/tag.h>

_WJR_BEGIN

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
		if (unlikely(0 == __result)) __result = _S_oom_malloc(__n);
		return __result;
	}

	static void deallocate(void* __p, size_t /* __n */) {
		free(__p);
	}

	static void* reallocate(void* __p, size_t /* old_sz */, size_t __new_sz) {
		void* __result = realloc(__p, __new_sz);
		if (unlikely(0 == __result)) __result = _S_oom_realloc(__p, __new_sz);
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
	if (unlikely(0 == base::start_free)) {
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

	constexpr basic_allocator() noexcept = default;
	constexpr basic_allocator(const basic_allocator&) noexcept = default;
	template <class _Other>
	constexpr explicit basic_allocator(const basic_allocator<_Other, threads>&) noexcept {}
	~basic_allocator() = default;
	basic_allocator& operator=(const basic_allocator&) noexcept = default;

	WJR_NODISCARD WJR_CONSTEXPR Ty* allocate() const noexcept {
		WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
			return static_cast<Ty*>(allocator_type::allocate(sizeof(Ty)));
		WJR_IS_NOT_CONSTANT_EVALUATED_END
			return std::allocator<Ty>().allocate(1);
	}

	WJR_NODISCARD WJR_CONSTEXPR Ty* allocate(size_t n) const noexcept {
		WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
			if (unlikely(0 == n)) {
				return nullptr;
			}
		return static_cast<Ty*>(allocator_type::allocate(sizeof(Ty) * n));
		WJR_IS_NOT_CONSTANT_EVALUATED_END
			return std::allocator<Ty>().allocate(n);
	}

	WJR_CONSTEXPR void deallocate(Ty* ptr) const noexcept {
		WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
			return allocator_type::deallocate(static_cast<void*>(ptr), sizeof(Ty));
		WJR_IS_NOT_CONSTANT_EVALUATED_END
			return std::allocator<Ty>().deallocate(ptr, 1);
	}

	WJR_CONSTEXPR void deallocate(Ty* ptr, size_t n) const noexcept {
		WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
			if (unlikely(0 == n)) return;
		return allocator_type::deallocate(static_cast<void*>(ptr), sizeof(Ty) * n);
		WJR_IS_NOT_CONSTANT_EVALUATED_END
			return std::allocator<Ty>().deallocate(ptr, n);
	}

	constexpr size_t max_size()const {
		return static_cast<size_t>(-1) / sizeof(Ty);
	}

	constexpr static size_t max_small_size() {
		return static_cast<size_t>(ALLOC_MAX_BYTES) / sizeof(Ty);
	}
};

template<typename T, typename U, bool t1, bool t2>
constexpr bool operator==(const basic_allocator<T, t1>&, const basic_allocator<U, t2>&) {
	return false;
}

template<typename T, typename U, bool threads>
constexpr bool operator==(const basic_allocator<T, threads>&, const basic_allocator<U, threads>&) {
	return true;
}

template<typename T, typename U, bool t1, bool t2>
constexpr bool operator!=(const basic_allocator<T, t1>&, const basic_allocator<U, t2>&) {
	return true;
}

template<typename T, typename U, bool threads>
constexpr bool operator!=(const basic_allocator<T, threads>&, const basic_allocator<U, threads>&) {
	return false;
}

template<typename T>
using tallocator = basic_allocator<T, true>;

template<typename T>
using allocator = basic_allocator<T, false>;

_WJR_END

#endif