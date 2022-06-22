#include "mallocator.h"

namespace wjr {
#ifdef ALLOCATOR_DEBUG
	allocator_debuger allocator_debuger_ref;
#endif

	thread_local char* __basic_default_alloc_template__<true>::start_free = 0;

	thread_local char* __basic_default_alloc_template__<true>::end_free = 0;

	thread_local size_t __basic_default_alloc_template__<true>::heap_size = 0;

	thread_local typename __basic_default_alloc_template__<true>::obj* volatile
		__basic_default_alloc_template__<true>::free_list[__NFREELISTS]
		= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

	char* __basic_default_alloc_template__<false>::start_free = 0;

	char* __basic_default_alloc_template__<false>::end_free = 0;

	size_t __basic_default_alloc_template__<false>::heap_size = 0;

	typename __basic_default_alloc_template__<false>::obj* volatile
		__basic_default_alloc_template__<false>::free_list[__NFREELISTS]
		= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
}