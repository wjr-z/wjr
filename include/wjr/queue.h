#pragma once
#ifndef __WJR_QUEUE_H
#define __WJR_QUEUE_H

#include <queue>

#include <wjr/deque.h>

_WJR_BEGIN

template<typename T, typename Alloc = std::allocator<T>>
using queue = std::queue<T, wjr::deque<T, Alloc>>;

_WJR_END

#endif // __WJR_QUEUE_H