#ifndef WJR_MACROS_REGISTER_HPP__
#define WJR_MACROS_REGISTER_HPP__

#include <type_traits>

#include <wjr/preprocessor/preview.hpp>

// a, b, c -> typename a, typename b, typename c
#define WJR_REGISTER_TYPENAMES(...)                                                                \
    WJR_PP_QUEUE_TRANSFORM((__VA_ARGS__), WJR_REGISTER_TYPENAMES_CALLER)
#define WJR_REGISTER_TYPENAMES_CALLER(x) typename x

#define WJR_REGISTER_HAS_TYPE_0(NAME, HAS_EXPR)                                                    \
    template <typename Enable, typename... Args>                                                   \
    struct __has_##NAME : std::false_type {};                                                      \
    template <typename... Args>                                                                    \
    struct __has_##NAME<std::void_t<decltype(HAS_EXPR)>, Args...> : std::true_type {};             \
    template <typename... Args>                                                                    \
    struct has_##NAME : __has_##NAME<void, Args...> {};                                            \
    template <typename... Args>                                                                    \
    constexpr bool has_##NAME##_v = has_##NAME<Args...>::value

#define WJR_REGISTER_HAS_TYPE_MORE(NAME, HAS_EXPR, ...)                                            \
    WJR_REGISTER_HAS_TYPE_MORE_I(NAME, HAS_EXPR, WJR_REGISTER_TYPENAMES(__VA_ARGS__), __VA_ARGS__)
#define WJR_REGISTER_HAS_TYPE_MORE_I(NAME, HAS_EXPR, TYPES, ...)                                   \
    template <typename Enable, WJR_PP_QUEUE_EXPAND(TYPES), typename... Args>                       \
    struct __has_##NAME : std::false_type {};                                                      \
    template <WJR_PP_QUEUE_EXPAND(TYPES), typename... Args>                                        \
    struct __has_##NAME<std::void_t<decltype(HAS_EXPR)>, __VA_ARGS__, Args...> : std::true_type {  \
    };                                                                                             \
    template <WJR_PP_QUEUE_EXPAND(TYPES), typename... Args>                                        \
    struct has_##NAME : __has_##NAME<void, __VA_ARGS__, Args...> {};                               \
    template <WJR_PP_QUEUE_EXPAND(TYPES), typename... Args>                                        \
    constexpr bool has_##NAME##_v = has_##NAME<__VA_ARGS__, Args...>::value

#define WJR_REGISTER_HAS_TYPE(NAME, ...)                                                           \
    WJR_REGISTER_HAS_TYPE_N(WJR_PP_ARGS_LEN(__VA_ARGS__), NAME, __VA_ARGS__)
#define WJR_REGISTER_HAS_TYPE_N(N, ...)                                                            \
    WJR_PP_BOOL_IF(WJR_PP_EQ(N, 1), WJR_REGISTER_HAS_TYPE_0, WJR_REGISTER_HAS_TYPE_MORE)           \
    (__VA_ARGS__)

namespace wjr::detail {
// used for SFINAE
constexpr static void allow_true_type(std::true_type) noexcept {}
constexpr static void allow_false_type(std::false_type) noexcept {}
} // namespace wjr::detail

#endif // WJR_MACROS_REGISTER_HPP__