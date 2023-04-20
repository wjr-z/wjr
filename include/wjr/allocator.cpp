#include <stdio.h>
#include <wjr/allocator.h>

_WJR_BEGIN

#if defined(WJR_TEST_ALLOCATOR)
void __test_allocator::test() const {
	if (_Count != 0) {
		printf("memory leak: %lld bytes", _Count);
	}
}
__test_allocator::~__test_allocator() {
	test();
}
__test_allocator __test_allocator_instance;
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

_WJR_END