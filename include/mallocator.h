#ifndef __WJR_MALLOCATOR_H
#define __WJR_MALLOCATOR_H

//#define ALLOCATOR_DEBUG
//#define __USE_THREADS

#ifdef ALLOCATOR_DEBUG
#include <cstdio>
#endif

#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <utility>
#include <type_traits>
#include <cstddef>

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
			if (0 == __result) __result = _S_oom_malloc(__n);
			return __result;
		}

		static void deallocate(void* __p, size_t /* __n */) {
			free(__p);
		}

		static void* reallocate(void* __p, size_t /* old_sz */, size_t __new_sz) {
			void* __result = realloc(__p, __new_sz);
			if (0 == __result) __result = _S_oom_realloc(__p, __new_sz);
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
			//if (0 == __my_malloc_handler) { __THROW_BAD_ALLOC; }
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
			//if (0 == __my_malloc_handler) { __THROW_BAD_ALLOC; }
			(*__my_malloc_handler)();
			__result = realloc(__p, __n);
			if (__result) return(__result);
		}
	}

	typedef __malloc_alloc_template__<0> malloc_alloc;

	enum {ALLOC_ALIGN = 8};
	enum {ALLOC_MAX_BYTES = 128};
	enum {ALLOC_NFRELISTS = ALLOC_MAX_BYTES / ALLOC_ALIGN};

	template <bool threads, int inst>
	class __default_alloc_template__;

	template<bool threads>
	class __basic_default_alloc_template__;

	template<>
	class __basic_default_alloc_template__<true> {
	private:
		template<bool,int>
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
			: allocated_size(0){

		}
		~allocator_debuger() {
			if (allocated_size == 0) {
				printf("allocator no problem\n");
			}
			if (allocated_size > 0) {
				printf("Some memory is not recycled\n");
			}
			if (allocated_size < 0) {
				printf("Recycled excess memory\n");
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
		static void* refill(size_t n);

		// Allocates a chunk for nobjs of size "size".  nobjs may be reduced
		// if it is inconvenient to allocate the requested number.
		static char* chunk_alloc(size_t size, int& nobjs);

	public:
		static void* allocate(size_t n) //n must be > 0
		{
			if (n > (size_t)__MAX_BYTES) {
				return malloc_alloc::allocate(n);
			}
		#ifdef ALLOCATOR_DEBUG
			allocator_debuger_ref.allocated_size += n;
		#endif
			obj* volatile* my_free_list = base::free_list + FREELIST_INDEX(n);
			obj* result = *my_free_list;
			if (result != nullptr) {
				*my_free_list = result->free_list_link;
				return result;
			}
			return refill(ROUND_UP(n));
		}

		static void deallocate(void* p, size_t n) //p may not be 0
		{
			obj* q = (obj*)p;

			if (n > (size_t)__MAX_BYTES) {
				free(p);
				return;
			}
		#ifdef ALLOCATOR_DEBUG
			allocator_debuger_ref.allocated_size -= n;
		#endif

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
	char*
		__default_alloc_template__<threads, inst>::
		chunk_alloc(size_t size, int& nobjs) {
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
		if (0 == base::start_free) {
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
		refill(size_t n) {
		int nobjs = 20;
		char* chunk = chunk_alloc(n, nobjs);
		obj* current_obj;
		obj* next_obj;

		if (1 == nobjs) return (chunk);
		obj* volatile* my_free_list = base::free_list + FREELIST_INDEX(n);

		//Build free list in chunk
		obj* result = (obj*)chunk;
		*my_free_list = next_obj = (obj*)(chunk + n);
		for (int i = 1; ; ++i) {
			current_obj = next_obj;
			next_obj = (obj*)((char*)next_obj + n);
			if (nobjs - 1 == i) {
				current_obj->free_list_link = 0;
				break;
			}
			current_obj->free_list_link = next_obj;
		}
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
			using other = basic_mallocator<_Other,threads>;
		};

		constexpr basic_mallocator() noexcept {}
		constexpr basic_mallocator(const basic_mallocator&) noexcept = default;
		template <class _Other>
		constexpr basic_mallocator(const basic_mallocator<_Other, threads>&) noexcept {}
		~basic_mallocator() = default;
		basic_mallocator& operator=(const basic_mallocator&) noexcept = default;

		Ty* allocate() {
			return static_cast<Ty*>(allocator_type::allocate(sizeof(Ty)));
		}

		Ty* allocate(size_t n) {
			return !n ? nullptr : static_cast<Ty*>(allocator_type::allocate(sizeof(Ty) * n));
		}

		void deallocate(Ty* ptr) {
			allocator_type::deallocate(static_cast<void*>(ptr), sizeof(Ty));
		}

		void deallocate(Ty* ptr, size_t n) {
			if (n == 0) return;
			allocator_type::deallocate(static_cast<void*>(ptr), sizeof(Ty) * n);
		}

		void construct(Ty* ptr) {
			new(ptr)Ty();
		}

		void construct(Ty* ptr, const Ty& value) {
			new(ptr)Ty(value);
		}

		void construct(Ty* ptr, Ty&& value) {
			new(ptr)Ty(std::forward<Ty>(value));
		}

		void destroy(Ty* ptr) {
			if constexpr (!std::is_trivially_destructible_v<Ty>) {
				ptr->~Ty();
			}
		}

		void destroy(Ty* first, Ty* last) {
			if constexpr (!std::is_trivially_destructible_v<Ty>) {
				for (; first != last; ++first) {
					first->~Ty();
				}
			}
		}

		size_t max_size()const {
			return static_cast<size_t>(-1) / sizeof(Ty);
		}

		constexpr static size_t max_small_size(){
			return static_cast<size_t>(ALLOC_MAX_BYTES) / sizeof(Ty);
		}

	};

	template<bool threads>
	class basic_mallocator<void,threads> {
	private:
		using allocator_type = __default_alloc_template__<threads, 0>;
	public:

		using value_type = void;
		//using reference = void&;
		//using const_reference = const void&;
		using pointer = void*;
		using const_pointer = const void*;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		using is_always_equal = std::true_type;

		template <typename _Other>
		struct rebind {
			using other = basic_mallocator<_Other, threads>;
		};

		constexpr basic_mallocator() noexcept {}
		constexpr basic_mallocator(const basic_mallocator&) noexcept = default;
		template <class _Other>
		constexpr basic_mallocator(const basic_mallocator<_Other, threads>&) noexcept {}
		~basic_mallocator() = default;
		basic_mallocator& operator=(const basic_mallocator&) noexcept = default;

		void* allocate() {
			return allocator_type::allocate(1);
		}

		void* allocate(size_t n) {
			return !n ? nullptr : allocator_type::allocate(n);
		}

		void deallocate(void * ptr) {
			allocator_type::deallocate(ptr, 1);
		}

		void deallocate(void * ptr, size_t n) {
			if (n == 0) return;
			allocator_type::deallocate(ptr, n);
		}

		/*
		void construct(void * ptr) {
			new(ptr)void();
		}

		void construct(void * ptr, const void & value) {
			new(ptr)void(value);
		}

		void construct(void * ptr, void && value) {
			new(ptr)void(std::forward<void>(value));
		}

		void destroy(void * ptr) {
			if constexpr (!std::is_trivially_destructible_v<void>) {
				ptr->~void();
			}
		}

		void destroy(void * first, void * last) {
			if constexpr (!std::is_trivially_destructible_v<void>) {
				for (; first != last; ++first) {
					first->~void();
				}
			}
		}
		*/

		size_t max_size()const {
			return static_cast<size_t>(-1);
		}

		constexpr static size_t max_small_size() {
			return static_cast<size_t>(ALLOC_MAX_BYTES);
		}
	};

	template<typename T,typename U,bool t1,bool t2>
	bool operator==(const basic_mallocator<T, t1>&, const basic_mallocator<U, t2>&) {
		return false;
	}

	template<typename T, typename U,bool threads>
	bool operator==(const basic_mallocator<T,threads>&, const basic_mallocator<U,threads>&) {
		return true;
	}

	template<typename T, typename U, bool t1, bool t2>
	bool operator!=(const basic_mallocator<T, t1>&, const basic_mallocator<U, t2>&) {
		return true;
	}

	template<typename T, typename U,bool threads>
	bool operator!=(const basic_mallocator<T,threads>&, const basic_mallocator<U,threads>&) {
		return false;
	}

	template<typename T>
	using tallocator = basic_mallocator<T,true>;

#ifndef __USE_THREADS
	template<typename T>
	using mallocator = basic_mallocator<T,false>;
#else 
	template<typename T>
	using mallocator = basic_mallocator<T,true>;
#endif

}

#endif