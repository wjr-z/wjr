#ifndef WJR_PREPROCCESSOR_QUEUE_BASIC_HPP__
#define WJR_PREPROCCESSOR_QUEUE_BASIC_HPP__

#include <wjr/preprocessor/detail/basic.hpp>

#define WJR_PP_QUEUE_EXPAND(queue) WJR_PP_QUEUE_EXPAND_I queue
#define WJR_PP_QUEUE_EXPAND_I(...) __VA_ARGS__

#define WJR_PP_QUEUE_FRONT(queue) WJR_PP_QUEUE_FRONT_I queue
#define WJR_PP_QUEUE_FRONT_I(x, ...) x

#define WJR_PP_QUEUE_POP_FRONT(queue) WJR_PP_QUEUE_POP_FRONT_I queue
#define WJR_PP_QUEUE_POP_FRONT_I(x, ...) (__VA_ARGS__)

#define WJR_PP_QUEUE_PUSH_FRONT(queue, ...) WJR_PP_QUEUE_PUSH_FRONT_I(queue, __VA_ARGS__)
#define WJR_PP_QUEUE_PUSH_FRONT_I(queue, ...) (__VA_ARGS__, WJR_PP_QUEUE_EXPAND(queue))

#define WJR_PP_QUEUE_PUSH_BACK(queue, ...) WJR_PP_QUEUE_PUSH_BACK_I(queue, __VA_ARGS__)
#define WJR_PP_QUEUE_PUSH_BACK_I(queue, ...) (WJR_PP_QUEUE_EXPAND(queue), __VA_ARGS__)

#define WJR_PP_QUEUE_SIZE(queue) WJR_PP_QUEUE_SIZE_I(queue)
#define WJR_PP_QUEUE_SIZE_I(queue) WJR_PP_ARGS_LEN queue

#endif // ! WJR_PREPROCCESSOR_QUEUE_BASIC_HPP__