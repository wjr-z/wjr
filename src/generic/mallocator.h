#ifndef __WJR_MALLOCATOR_H
#define __WJR_MALLOCATOR_H

//#define ALLOCATOR_DEBUG

#ifdef ALLOCATOR_DEBUG
#include <cstdio>
#endif

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <type_traits>
#include <utility>
//#include "mtype_traits.h"
#include "mtag.h"

namespace wjr {
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

#ifdef ALLOCATOR_DEBUG
	struct allocator_debuger {
		allocator_debuger()
			: allocated_size(0) {
		}
		~allocator_debuger() {
			if (0 == allocated_size) {
				printf("allocator no problem\n");
				return;
			}
			if (allocated_size > 0) {
				printf("Some memory is not recycled\n");
				printf("size : %lld\n", allocated_size);
			}
			if (allocated_size < 0) {
				printf("Recycled excess memory\n");
				printf("size : %lld\n", allocated_size);
			}
		}
		long long allocated_size;
	};
	extern allocator_debuger allocator_debuger_ref;
#endif

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
#ifdef ALLOCATOR_DEBUG
			allocator_debuger_ref.allocated_size += n;
#endif
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
#ifdef ALLOCATOR_DEBUG
			allocator_debuger_ref.allocated_size -= n;
#endif

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

		* my_free_list = current_obj = (obj*)(chunk + n);
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
	class basic_mallocator {
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

		using is_always_equal = std::true_type;

		template <typename _Other>
		struct rebind {
			using other = basic_mallocator<_Other, threads>;
		};

		constexpr basic_mallocator() noexcept = default;
		constexpr basic_mallocator(const basic_mallocator&) noexcept = default;
		template <class _Other>
		constexpr explicit basic_mallocator(const basic_mallocator<_Other, threads>&) noexcept {}
		~basic_mallocator() = default;
		basic_mallocator& operator=(const basic_mallocator&) noexcept = default;

		[[nodiscard]] WJR_CONSTEXPR Ty* allocate() const noexcept {
			WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
				return static_cast<Ty*>(allocator_type::allocate(sizeof(Ty)));
			WJR_IS_NOT_CONSTANT_EVALUATED_END
			return std::allocator<Ty>().allocate(1);
		}

		[[nodiscard]] WJR_CONSTEXPR Ty* allocate(size_t n) const noexcept {
			WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
				if (unlikely(0 == n)) {
					return nullptr;
				}
			return static_cast<Ty*>(allocator_type::allocate(sizeof(Ty) * n));
			WJR_IS_NOT_CONSTANT_EVALUATED_END
			return std::allocator<Ty>().allocate(n);
		}

		WJR_CONSTEXPR void deallocate(Ty* ptr) const noexcept{
			WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
				return allocator_type::deallocate(static_cast<void*>(ptr), sizeof(Ty));
			WJR_IS_NOT_CONSTANT_EVALUATED_END
			return std::allocator<Ty>().deallocate(ptr, 1);
		}

		WJR_CONSTEXPR void deallocate(Ty* ptr, size_t n) const noexcept{
			WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
				if (unlikely(0 == n)) return;
				return allocator_type::deallocate(static_cast<void*>(ptr), sizeof(Ty) * n);
			WJR_IS_NOT_CONSTANT_EVALUATED_END
			return std::allocator<Ty>().deallocate(ptr, n);
		}

		constexpr void construct([[maybe_unused]]Ty* ptr, tag::uninitialized_tag) const{
			// don't do anything
		}

		constexpr void destroy(Ty* ptr)const noexcept{
			if constexpr(!std::is_trivially_destructible_v<Ty>) {
				ptr->~Ty();
			}
		}

		constexpr size_t max_size()const {
			return static_cast<size_t>(-1) / sizeof(Ty);
		}

		constexpr static size_t max_small_size() {
			return static_cast<size_t>(ALLOC_MAX_BYTES) / sizeof(Ty);
		}
	};

	template<typename T, typename U, bool t1, bool t2>
	constexpr bool operator==(const basic_mallocator<T, t1>&, const basic_mallocator<U, t2>&) {
		return false;
	}

	template<typename T, typename U, bool threads>
	constexpr bool operator==(const basic_mallocator<T, threads>&, const basic_mallocator<U, threads>&) {
		return true;
	}

	template<typename T, typename U, bool t1, bool t2>
	constexpr bool operator!=(const basic_mallocator<T, t1>&, const basic_mallocator<U, t2>&) {
		return true;
	}

	template<typename T, typename U, bool threads>
	constexpr bool operator!=(const basic_mallocator<T, threads>&, const basic_mallocator<U, threads>&) {
		return false;
	}

	template<typename T>
	using tallocator = basic_mallocator<T, true>;

#ifndef __USE_THREADS
	template<typename T>
	using mallocator = basic_mallocator<T, false>;
#else
	template<typename T>
	using mallocator = basic_mallocator<T, true>;
#endif

	template<typename Enable, typename Alloc, typename T, typename...Args>
	struct _Has_construct : std::false_type {};
	template<typename Alloc, typename T, typename...Args>
	struct _Has_construct<std::void_t<decltype(
		std::declval<Alloc&>().construct(std::declval<T>(), std::declval<Args>()...))>, T, Alloc, Args...> : std::true_type {};

	template<typename Alloc, typename T, typename...Args>
	struct mhas_construct : _Has_construct<void, Alloc, T, Args...> {};

	template<typename Alloc, typename T, typename...Args>
	constexpr static bool mhas_construct_v = mhas_construct<Alloc, T, Args...>::value;

	template<typename Alloc, typename T, typename = void>
	struct mhas_destroy : std::false_type {};

	template<typename Alloc, typename T>
	struct mhas_destroy<Alloc, T, std::void_t<
		decltype(std::declval<Alloc&>().destroy(std::declval<T>()))>> : std::true_type {};

	template<typename Alloc, typename T, typename...Args>
	constexpr static bool mhas_destroy_v = mhas_destroy<Alloc, T, Args...>::value;

	template<typename Alloc, typename T, typename...Args>
	void mconstruct(Alloc& al, T* ptr, Args&&...args) {
		if constexpr (mhas_construct_v<Alloc, T*, Args...>) {
			al.construct(ptr, std::forward<Args>(args)...);
		}
		else {
			new (ptr) T(std::forward<Args>(args)...);
		}
	}

	template<typename Alloc, typename T>
	void mdestroy(Alloc& al, T* ptr) {
		if constexpr (mhas_destroy_v<Alloc, T*>) {
			al.destroy(ptr);
		}
		else {
			ptr->~T();
		}
	}

	template<typename T, typename Alloc>
	struct _Make_allocate_unique : public forward_wrapper_t<Alloc> {
		using Base = forward_wrapper_t<Alloc>;
		template<typename...Args>
		constexpr explicit _Make_allocate_unique(Args&&...args) noexcept(std::is_nothrow_constructible_v<Base, Args...>)
			: Base(std::forward<Args>(args)...) {}
		template<typename U>
		constexpr _Make_allocate_unique& operator=(U&& _Val) noexcept(std::is_nothrow_assignable_v<Base, U&&>) {
			Base::operator=(std::forward<U>(_Val));
			return *this;
		}
		constexpr void operator()(T* ptr) noexcept {
			mdestroy(*this, ptr);
			static_cast<Alloc>(*this).deallocate(ptr, 1);
		}
	};

	template<typename T, typename Alloc>
	using mallocate_unique_ptr_t = std::unique_ptr<T, _Make_allocate_unique<T, Alloc>>;

	template<typename T, typename Alloc, typename...Args>
	constexpr mallocate_unique_ptr_t<T,Alloc> mallocate_unique(Alloc&& al, Args&&...args) {
		_Make_allocate_unique<T, Alloc> _Al(std::forward<Alloc>(al));
		T* ptr = static_cast<Alloc>(_Al).allocate(1);
		mconstruct(_Al, ptr, std::forward<Args>(args)...);
		return mallocate_unique_ptr_t<T,Alloc>(ptr, std::move(_Al));
	}
	
}

#endif