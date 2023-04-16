#include <wjr/algo/mem-all.h>

_WJR_ALGO_BEGIN

#if defined(_WJR_FAST_MEMCHR)
template const uint8_t* __large__memchr(
	const uint8_t*, uint8_t, size_t, std::equal_to<>) noexcept;
template const uint8_t* __large__memrchr(
	const uint8_t*, uint8_t, size_t, std::equal_to<>) noexcept;

template const uint8_t* __large__memchr(
	const uint8_t*, uint8_t, size_t, std::not_equal_to<>) noexcept;
template const uint8_t* __large__memrchr(
	const uint8_t*, uint8_t, size_t, std::not_equal_to<>) noexcept;
#endif // _WJR_FAST_MEMCHR

#if defined(_WJR_FAST_MEMCMP)
template bool __large_memcmp(const uint8_t*, const uint8_t*, size_t, std::equal_to<>) noexcept;
template bool __large_memcmp(const uint8_t*, const uint8_t*, size_t, std::not_equal_to<>) noexcept;
#endif // _WJR_FAST_MEMCMP

#if defined(_WJR_FAST_MEMMIS)
template const uint8_t* __large__memmis(
	const uint8_t*, const uint8_t*, size_t, std::equal_to<>) noexcept;
template const uint8_t* __large__memrmis(
	const uint8_t*, const uint8_t*, size_t, std::equal_to<>) noexcept;
#endif // _WJR_FAST_MEMMIS

_WJR_ALGO_END