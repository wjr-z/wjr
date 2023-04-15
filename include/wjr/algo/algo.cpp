#include <wjr/algo/algo.h>

_WJR_ALGO_BEGIN

template const uint8_t* __large__memchr(
	const uint8_t*, uint8_t, size_t, std::equal_to<>)noexcept;

template const uint8_t* __large__memrchr(
	const uint8_t*, uint8_t, size_t, std::equal_to<>)noexcept;

_WJR_ALGO_END