#ifndef WJR_DISPATCH_HPP__
#define WJR_DISPATCH_HPP__

#include <wjr/preprocessor.hpp>
#include <wjr/tuple.hpp>

#define WJR_DISPATCH_FUNC_TABLE_MEMBER(queue)                                                      \
    WJR_PP_QUEUE_PUT(WJR_PP_QUEUE_TRANSFORM(queue, WJR_DISPATCH_FUNC_TABLE_MEMBER_I))
#define WJR_DISPATCH_FUNC_TABLE_MEMBER_I(x) WJR_DISPATCH_FUNC_TABLE_MEMBER_II x
#define WJR_DISPATCH_FUNC_TABLE_MEMBER_II(name, ret, ...) ret (*name)(void *, __VA_ARGS__);

#define WJR_DISPATCH_DISPATCHER(name, dispatcher, queue)                                           \
    WJR_DISPATCH_DISPATCHER_I(name, dispatcher, queue)

#define WJR_DISPATCH_DISPATCHER_I(name, dispatcher, queue)                                         \
    struct dispatcher {                                                                            \
        template <typename T>                                                                      \
        WJR_CONSTEXPR20 dispatcher(T *ref) noexcept : v_ptr(static_cast<void *>(ref)) {            \
            static constexpr name##_func_table table = {WJR_DISPATCH_STATIC_FUNC_TABLE(queue)};    \
            v_table = &table;                                                                      \
        }                                                                                          \
                                                                                                   \
        dispatcher(const dispatcher &) = default;                                                  \
        dispatcher(dispatcher &&) = default;                                                       \
        dispatcher &operator=(const dispatcher &) = default;                                       \
        dispatcher &operator=(dispatcher &&) = default;                                            \
                                                                                                   \
        WJR_DISPATCH_REGISTER_FUNCTION(queue)                                                      \
                                                                                                   \
        void *v_ptr = nullptr;                                                                     \
        const name##_func_table *v_table = nullptr;                                                \
    }

#define WJR_DISPATCH_STATIC_FUNC_TABLE(queue)                                                      \
    WJR_PP_QUEUE_EXPAND(                                                                           \
        WJR_PP_QUEUE_UNWRAP(WJR_PP_QUEUE_TRANSFORM(queue, WJR_DISPATCH_STATIC_FUNC_TABLE_I)))
#define WJR_DISPATCH_STATIC_FUNC_TABLE_I(x) WJR_DISPATCH_STATIC_FUNC_TABLE_II x
#define WJR_DISPATCH_STATIC_FUNC_TABLE_II(name, ret, ...)                                          \
    ([](void *ptr, auto &&...args) -> ret {                                                        \
        return static_cast<T *>(ptr)->name(std::forward<decltype(args)>(args)...);                 \
    })

#define WJR_DISPATCH_REGISTER_FUNCTION(queue)                                                      \
    WJR_PP_QUEUE_PUT(WJR_PP_QUEUE_TRANSFORM(queue, WJR_DISPATCH_REGISTER_FUNCTION_I))
#define WJR_DISPATCH_REGISTER_FUNCTION_I(queue) WJR_DISPATCH_REGISTER_FUNCTION_II queue
#define WJR_DISPATCH_REGISTER_FUNCTION_II(name, ret, ...)                                          \
    template <typename... Args>                                                                    \
    ret name(Args &&...args) {                                                                     \
        return v_table->name(v_ptr, std::forward<Args>(args)...);                                  \
    }

#define WJR_REGISTER_DISPATCH(name, queue)                                                         \
    struct name##_func_table {                                                                     \
        WJR_DISPATCH_FUNC_TABLE_MEMBER(queue)                                                      \
    };                                                                                             \
    WJR_DISPATCH_DISPATCHER(name, name##_dispacher, queue)

#endif // WJR_DISPATCH_HPP__