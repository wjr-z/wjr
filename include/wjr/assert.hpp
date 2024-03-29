#ifndef WJR_ASSERT_HPP__
#define WJR_ASSERT_HPP__

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

#include <wjr/preprocessor.hpp>

namespace wjr {

// ASSERT_LEVEL : 0 ~ 3
// 0 : Release (defined(NDEBUG) && ! defined(WJR_DEBUG_LEVEL))
// 1 : Some simple runtime checks, such as boundary checks (default)
// 2 : Most runtime checks
// 3 : Maximize runtime checks
#ifndef WJR_DEBUG_LEVEL
#if defined(NDEBUG)
#define WJR_DEBUG_LEVEL 0
#else
#define WJR_DEBUG_LEVEL 1
#endif
#endif

// use WJR_THROW instead of std::abort
#ifdef WJR_ASSERT_THROW
#define WJR_ASSERT_NORETURN
#define WJR_ASSERT_ABORT() WJR_THROW(std::runtime_error("assertion failed"))
#else
#define WJR_ASSERT_NORETURN WJR_NORETURN
#define WJR_ASSERT_ABORT() std::abort()
#endif

#define WJR_DEBUG_IF(level, expr0, expr1)                                                \
    WJR_PP_BOOL_IF(WJR_PP_GT(WJR_DEBUG_LEVEL, level), expr0, expr1)

#define WJR_DEBUG_EXPR_L(level, expr) WJR_DEBUG_IF(level, expr, )
#define WJR_DEBUG_EXPR(expr) WJR_DEBUG_EXPR_L(0, expr)

template <typename... Args>
struct assert_format {
    assert_format(const char *const fmt, std::tuple<Args...> &&args)
        : m_fmt(fmt), m_args(std::move(args)) {}

    const char *const m_fmt;
    std::tuple<Args...> m_args;
};

template <typename... Args>
assert_format(const char *const, std::tuple<Args...> &&) -> assert_format<Args...>;

#define WJR_ASSERT_FORMAT(fmt, ...)                                                      \
    ::wjr::assert_format(fmt, std::make_tuple(__VA_ARGS__))

template <typename T>
struct __is_assert_format : std::false_type {};

template <typename... Args>
struct __is_assert_format<assert_format<Args...>> : std::true_type {};

class __assert_handler_t {
private:
    template <typename... Args>
    void __handler_format(const assert_format<Args...> &fmt) const {
        const char *const fmt_str = fmt.m_fmt;
        std::apply(
            [fmt_str](auto &&...args) {
                (void)fprintf(stderr, fmt_str, std::forward<decltype(args)>(args)...);
            },
            fmt.m_args);
    }

    template <typename T>
    void __handler(T &&t) const {
        if constexpr (__is_assert_format<std::decay_t<T>>::value) {
            __handler_format(std::forward<T>(t));
        } else {
            std::cerr << t;
        }
    }

    void handler() const {}

    template <typename... Args>
    void handler(Args &&...args) const {
        (void)fprintf(stderr, "Additional information:\n");
        (void)((__handler(std::forward<Args>(args)), ...));
        (void)fprintf(stderr, "\n");
    }

public:
    template <typename... Args>
    WJR_ASSERT_NORETURN WJR_NOINLINE void operator()(const char *expr, const char *file,
                                                     int line, Args &&...args) const {
        (void)fprintf(stderr, "Assertion failed: %s", expr);
        if ((file != nullptr) && (file[0] != '\0')) {
            (void)fprintf(stderr, ", file %s", file);
        }
        if (line != -1) {
            (void)fprintf(stderr, ", line %d", line);
        }
        (void)fprintf(stderr, "\n");
        handler(std::forward<Args>(args)...);
        WJR_ASSERT_ABORT();
    }
};

inline constexpr __assert_handler_t __assert_handler{};

#define WJR_ASSERT_NOMESSAGE_FAIL(handler, exprstr) handler(exprstr, WJR_FILE, WJR_LINE)
#define WJR_ASSERT_MESSAGE_FAIL(handler, exprstr, ...)                                   \
    handler(exprstr, WJR_FILE, WJR_LINE, __VA_ARGS__)

#define WJR_ASSERT_CHECK_I_NOMESSAGE(handler, expr)                                      \
    do {                                                                                 \
        if (WJR_UNLIKELY(!(expr))) {                                                     \
            WJR_ASSERT_NOMESSAGE_FAIL(handler, #expr);                                   \
        }                                                                                \
    } while (0)
#define WJR_ASSERT_CHECK_I_MESSAGE(handler, expr, ...)                                   \
    do {                                                                                 \
        if (WJR_UNLIKELY(!(expr))) {                                                     \
            WJR_ASSERT_MESSAGE_FAIL(handler, #expr, __VA_ARGS__);                        \
        }                                                                                \
    } while (0)

#define WJR_ASSERT_CHECK_I(...)                                                          \
    WJR_ASSERT_CHECK_I_N(WJR_PP_ARGS_LEN(__VA_ARGS__), __VA_ARGS__)
#define WJR_ASSERT_CHECK_I_N(N, ...)                                                     \
    WJR_PP_BOOL_IF(WJR_PP_EQ(N, 1), WJR_ASSERT_CHECK_I_NOMESSAGE,                        \
                   WJR_ASSERT_CHECK_I_MESSAGE)                                           \
    (::wjr::__assert_handler, __VA_ARGS__)

// do nothing
#define WJR_ASSERT_UNCHECK_I(expr, ...)

// level = [0, 2]
// The higher the level, the less likely it is to be detected
// Runtime detect  : 1
// Maximize detect : 2
#define WJR_ASSERT_L(level, ...)                                                         \
    WJR_DEBUG_IF(level, WJR_ASSERT_CHECK_I, WJR_ASSERT_UNCHECK_I)                        \
    (__VA_ARGS__)

// level of assert is zero at default.
#define WJR_ASSERT(...) WJR_ASSERT_L(0, __VA_ARGS__)
#define WJR_ASSERT_L1(...) WJR_ASSERT_L(1, __VA_ARGS__)
#define WJR_ASSERT_L2(...) WJR_ASSERT_L(2, __VA_ARGS__)
#define WJR_ASSERT_L3(...) WJR_ASSERT_L(3, __VA_ARGS__)

#define WJR_ASSERT_ASSUME_L(level, ...)                                                  \
    WJR_ASSERT_L(level, __VA_ARGS__);                                                    \
    __WJR_ASSERT_ASSUME_L_ASSUME(__VA_ARGS__)
#define __WJR_ASSERT_ASSUME_L_ASSUME(expr, ...) WJR_ASSUME(expr)

#define WJR_ASSERT_ASSUME(...) WJR_ASSERT_ASSUME_L(0, __VA_ARGS__)
#define WJR_ASSERT_ASSUME_L1(...) WJR_ASSERT_ASSUME_L(1, __VA_ARGS__)
#define WJR_ASSERT_ASSUME_L2(...) WJR_ASSERT_ASSUME_L(2, __VA_ARGS__)
#define WJR_ASSERT_ASSUME_L3(...) WJR_ASSERT_ASSUME_L(3, __VA_ARGS__)

} // namespace wjr

#endif // WJR_ASSERT_HPP__