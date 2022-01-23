#include "../include/mallocator.h"

namespace wjr {
	thread_local char* __basic_default_alloc_template<true>::start_free = 0;

	thread_local char* __basic_default_alloc_template<true>::end_free = 0;

	thread_local size_t __basic_default_alloc_template<true>::heap_size = 0;

	thread_local typename __basic_default_alloc_template<true>::obj* volatile
		__basic_default_alloc_template<true>::free_list[__NFREELISTS]
		= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

	char* __basic_default_alloc_template<false>::start_free = 0;

	char* __basic_default_alloc_template<false>::end_free = 0;

	size_t __basic_default_alloc_template<false>::heap_size = 0;

	typename __basic_default_alloc_template<false>::obj* volatile
		__basic_default_alloc_template<false>::free_list[__NFREELISTS]
		= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
}