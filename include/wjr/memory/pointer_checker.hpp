#ifndef WJR_MEMORY_POINTER_CHECKER_HPP__
#define WJR_MEMORY_POINTER_CHECKER_HPP__

#include <wjr/preprocessor.hpp>

#if WJR_DEBUG_LEVEL > 2
#define WJR_HAS_DEBUG_POINTER_CHECKER WJR_HAS_DEF
#endif

template <typename T>
class debug_pointer_checker {};

template <typename T>
class release_pointer_checker {};

#if WJR_HAS_DEBUG(POINTER_CHECKER)
template <typename T>
using pointer_checker = debug_pointer_checker<T>;
#else
template <typename T>
using pointer_checker = release_pointer_checker<T>;
#endif

#endif // WJR_MEMORY_POINTER_CHECKER_HPP__