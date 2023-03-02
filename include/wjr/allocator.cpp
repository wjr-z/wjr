#include <stdio.h>
#include <wjr/allocator.h>

_WJR_BEGIN
#if defined(WJR_TEST_ALLOCATOR)
__test_allocator::~__test_allocator() {
	if (_Count != 0) {
		printf("memory leak: %llu bytes", _Count);
	}
}
__test_allocator __test_allocator_instance;
#endif
_WJR_END