#ifndef WJR_MALLOCATOR_H
#define WJR_MALLOCATOR_H

#include <cstdlib>
#include <cassert>
#include <utility>
#include <type_traits>

#include <mutex>

namespace wjr {

#define ALLOCATOR_DEBUG

#ifndef _DEBUG
#undef ALLOCATOR_DEBUG
#endif

	template <int __inst>
	class __malloc_alloc_template {

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
	void (*__malloc_alloc_template<__inst>::__malloc_alloc_oom_handler)() = 0;
#endif

	template <int __inst>
	void*
		__malloc_alloc_template<__inst>::_S_oom_malloc(size_t __n) {
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
	void* __malloc_alloc_template<__inst>::_S_oom_realloc(void* __p, size_t __n) {
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

	typedef __malloc_alloc_template<0> malloc_alloc;

	enum {ALLOC_ALIGN = 8};
	enum {ALLOC_MAX_BYTES = 128};
	enum {ALLOC_NFRELISTS = ALLOC_MAX_BYTES / ALLOC_ALIGN};

	template <bool threads, int inst>
	class __default_alloc_template {
	private:

		enum { __ALIGN = ALLOC_ALIGN };
		enum { __MAX_BYTES = ALLOC_MAX_BYTES };
		enum { __NFREELISTS = ALLOC_NFRELISTS };

		static inline size_t ROUND_UP(size_t bytes) {
			return (((bytes)+__ALIGN - 1) & ~(__ALIGN - 1));
		}

	private:
		union obj {
			union obj* free_list_link; 
			char client_data[1]; 
		};
	private:
		static obj* volatile free_list[__NFREELISTS];

		static inline size_t FREELIST_INDEX(size_t bytes) {
			return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
		}

		// Returns an object of size n, and optionally adds to size n free list.
		static void* refill(size_t n);

		// Allocates a chunk for nobjs of size "size".  nobjs may be reduced
		// if it is inconvenient to allocate the requested number.
		static char* chunk_alloc(size_t size, int& nobjs);

		// Chunk allocation state.
		static char* start_free;
		static char* end_free;
		static size_t heap_size;

		static std::mutex allocator_mutex;

	#ifdef ALLOCATOR_DEBUG
		struct allocator_size_count {
			size_t count;
			allocator_size_count()
				: count(0) {

			}
			~allocator_size_count() {
				assert(count == 0);
			}
		};
		static allocator_size_count S;
	#endif

	public:
		static void* allocate(size_t n) //n must be > 0
		{
			if (n > (size_t)__MAX_BYTES) {
			#ifdef ALLOCATOR_DEBUG
				S.count += n;
			#endif
				return malloc_alloc::allocate(n);
			}
		#ifdef ALLOCATOR_DEBUG
			S.count += n;
		#endif
			if constexpr (threads) {
				obj* volatile* my_free_list = free_list + FREELIST_INDEX(n);
				std::unique_lock<std::mutex> _mutex(allocator_mutex);
				obj* result = *my_free_list;
				if (result != nullptr) {
					*my_free_list = result->free_list_link;
					return result;
				}
				return refill(ROUND_UP(n));
			}
			else {
				obj* volatile* my_free_list = free_list + FREELIST_INDEX(n);
				obj* result = *my_free_list;
				if (result != nullptr) {
					*my_free_list = result->free_list_link;
					return result;
				}
				return refill(ROUND_UP(n));
			}
		}

		static void deallocate(void* p, size_t n) //p may not be 0
		{
		#ifdef ALLOCATOR_DEBUG
			S.count -= n;
		#endif
			obj* q = (obj*)p;

			if (n > (size_t)__MAX_BYTES) {
				free(p);
				return;
			}
			if constexpr (threads) {
				obj* volatile* my_free_list = free_list + FREELIST_INDEX(n);
				std::unique_lock<std::mutex> _mutex(allocator_mutex);
				q->free_list_link = *my_free_list;
				*my_free_list = q;
			}
			else {
				obj* volatile* my_free_list = free_list + FREELIST_INDEX(n);
				q->free_list_link = *my_free_list;
				*my_free_list = q;
			}

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
		__default_alloc_template<threads, inst>::
		chunk_alloc(size_t size, int& nobjs) {
		char* result;
		size_t total_bytes = size * nobjs;
		const auto bytes_left = static_cast<size_t>(end_free - start_free);

		if (bytes_left >= total_bytes) { 
			result = start_free;
			start_free += total_bytes;
			return (result);
		}
		if (bytes_left >= size) {
			nobjs = static_cast<int>(bytes_left / size);
			total_bytes = size * nobjs;
			result = start_free;
			start_free += total_bytes;
			return (result);
		}
		const size_t bytes_to_get =
			2 * total_bytes + ROUND_UP(heap_size >> 4);
		// Try to make use of the left-over piece.
		if (bytes_left > 0) {
			obj* volatile* my_free_list =
				free_list + FREELIST_INDEX(bytes_left);

			((obj*)start_free)->free_list_link = *my_free_list;
			*my_free_list = (obj*)start_free;
		}
		start_free = (char*)malloc(bytes_to_get);
		if (0 == start_free) {
			obj* volatile* my_free_list, * p;

			//Try to make do with what we have. That can't
			//hurt. We do not try smaller requests, since that tends
			//to result in disaster on multi-process machines.
			for (int i = static_cast<int>(size); i <= __MAX_BYTES; i += __ALIGN) {
				my_free_list = free_list + FREELIST_INDEX(i);
				p = *my_free_list;
				if (0 != p) {
					*my_free_list = p->free_list_link;
					start_free = (char*)p;
					end_free = start_free + i;
					return (chunk_alloc(size, nobjs));
					//Any leftover piece will eventually make it to the
					//right free list.
				}
			}
			end_free = 0; //In case of exception.
			start_free = (char*)malloc(bytes_to_get);
			//This should either throw an exception or
			//remedy the situation. Thus we assume it
			//succeeded.
		}
		heap_size += bytes_to_get;
		end_free = start_free + bytes_to_get;
		return (chunk_alloc(size, nobjs));
	}

	//----------------------------------------------
	// Returns an object of size n, and optionally adds
	// to size n free list.We assume that n is properly aligned.
	// We hold the allocation lock.
	//----------------------------------------------
	template <bool threads, int inst>
	void* __default_alloc_template<threads, inst>::
		refill(size_t n) {
		int nobjs = 20;
		char* chunk = chunk_alloc(n, nobjs);
		obj* current_obj;
		obj* next_obj;

		if (1 == nobjs) return (chunk);
		obj* volatile* my_free_list = free_list + FREELIST_INDEX(n);

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

	//----------------------------------------------
	template <bool threads, int inst>
	char* __default_alloc_template<threads, inst>::start_free = 0;

	template <bool threads, int inst>
	char* __default_alloc_template<threads, inst>::end_free = 0;

	template <bool threads, int inst>
	size_t __default_alloc_template<threads, inst>::heap_size = 0;

	template <bool threads, int inst>
	typename __default_alloc_template<threads, inst>::obj* volatile
		__default_alloc_template<threads, inst>::free_list[__NFREELISTS]
		= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

	template<bool threads, int inst>
	std::mutex __default_alloc_template<threads, inst>::allocator_mutex;

	template<bool threads, int __inst>
	using alloc = __default_alloc_template<threads, __inst>;

#ifdef ALLOCATOR_DEBUG
	template<bool threads,int __inst>
	typename __default_alloc_template<threads,__inst>::allocator_size_count
		__default_alloc_template<threads, __inst>::S;
#endif

	template <typename Ty, bool threads = false>
	class basic_malloc {
	private:
		using allocator_type = alloc<threads, 0>;
	public:

		using value_type = Ty;
		using reference = Ty&;
		using const_reference = const Ty&;
		using pointer = Ty*;
		using const_pointer = const Ty*;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		template <typename _Other,bool _Threads = threads>
		struct rebind {
			static_assert(threads == _Threads);
			using other = basic_malloc<_Other,_Threads>;
		};

		constexpr basic_malloc() noexcept {}
		constexpr basic_malloc(const basic_malloc&) noexcept = default;
		template <class _Other>
		constexpr basic_malloc(const basic_malloc<_Other>&) noexcept {}
		~basic_malloc() = default;
		basic_malloc& operator=(const basic_malloc&) noexcept = default;

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

	};

	template<typename T, typename U>
	bool operator==(const basic_malloc<T>&, const basic_malloc<U>&) {
		return true;
	}

	template<typename T, typename U>
	bool operator!=(const basic_malloc<T>&, const basic_malloc<U>&) {
		return false;
	}

#ifndef __USE_THREADS
	template<typename T>
	using mallocator = basic_malloc<T,false>;
#else 
	template<typename T>
	using mallocator = basic_malloc<T,true>;
#endif

}

#endif