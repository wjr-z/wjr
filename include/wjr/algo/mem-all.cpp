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

#if defined(_WJR_FAST_MEMEQ)
template bool __large__memeq(const uint8_t*, const uint8_t*, size_t, std::equal_to<>) noexcept;
template bool __large__memeq(const uint8_t*, const uint8_t*, size_t, std::not_equal_to<>) noexcept;
#endif // _WJR_FAST_MEMEQ

#if defined(_WJR_FAST_MEMMIS)
template const uint8_t* __large__memmis(
	const uint8_t*, const uint8_t*, size_t, std::equal_to<>) noexcept;
template const uint8_t* __large__memrmis(
	const uint8_t*, const uint8_t*, size_t, std::equal_to<>) noexcept;
#endif // _WJR_FAST_MEMMIS

#if defined(_WJR_FAST_MEMSKIPW)
template const char* __large__memskipw(
	const char*, const char*) noexcept;
#endif // _WJR_FAST_MEMSKIPW

_WJR_ALGO_END