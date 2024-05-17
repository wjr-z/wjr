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

#define WJR_DEBUG_IF(level, expr0, expr1)                                                \
    WJR_PP_BOOL_IF(WJR_PP_GT(WJR_DEBUG_LEVEL, level), expr0, expr1)

#define WJR_DEBUG_EXPR_L(level, expr) WJR_DEBUG_IF(level, expr, )
#define WJR_DEBUG_EXPR(expr) WJR_DEBUG_EXPR_L(0, expr)

WJR_NORETURN extern void __assert_failed(const char *expr, const char *file,
                                         const char *func, int line) noexcept;

// LCOV_EXCL_START

/// @private
class __assert_handler_t {
private:
    WJR_NORETURN static void fn(const char *expr, const char *file, const char *func,
                                int line) noexcept {
        __assert_failed(expr, file, func, line);
    }

    template <typename... Args>
    WJR_NORETURN WJR_NOINLINE static void fn(const char *expr, const char *file,
                                             const char *func, int line,
                                             Args &&...args) noexcept {
        std::cerr << "Additional information: ";
        (void)(std::cerr << ... << std::forward<Args>(args));
        std::cerr << '\n';
        __assert_failed(expr, file, func, line);
    }

public:
    template <typename... Args>
    WJR_NORETURN void operator()(const char *expr, const char *file, const char *func,
                                 int line, Args &&...args) const noexcept {
        fn(expr, file, func, line, std::forward<Args>(args)...);
    }
};

inline constexpr __assert_handler_t __assert_handler{};

// LCOV_EXCL_STOP

#define WJR_ASSERT_CHECK_I_HANDLER(handler, expr, ...)                                   \
    do {                                                                                 \
        if (WJR_UNLIKELY(!(expr))) {                                                     \
            handler(#expr, WJR_FILE, WJR_CURRENT_FUNCTION, WJR_LINE, ##__VA_ARGS__);     \
        }                                                                                \
    } while (0)

#define WJR_ASSERT_CHECK_I(...)                                                          \
    WJR_ASSERT_CHECK_I_HANDLER(::wjr::__assert_handler, __VA_ARGS__)

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

// always detect
#define WJR_ASSERT_LX(...) WJR_ASSERT_CHECK_I(__VA_ARGS__)

#define WJR_ASSERT_ASSUME_L(level, ...)                                                  \
    WJR_ASSERT_L(level, __VA_ARGS__);                                                    \
    __WJR_ASSERT_ASSUME_L_ASSUME(__VA_ARGS__)
#define __WJR_ASSERT_ASSUME_L_ASSUME(expr, ...) WJR_ASSUME(expr)

#define WJR_ASSERT_ASSUME(...) WJR_ASSERT_ASSUME_L(0, __VA_ARGS__)
#define WJR_ASSERT_ASSUME_L1(...) WJR_ASSERT_ASSUME_L(1, __VA_ARGS__)
#define WJR_ASSERT_ASSUME_L2(...) WJR_ASSERT_ASSUME_L(2, __VA_ARGS__)
#define WJR_ASSERT_ASSUME_L3(...) WJR_ASSERT_ASSUME_L(3, __VA_ARGS__)

#define WJR_ASSERT_ASSUME_LX(...)                                                        \
    WJR_ASSERT_LX(__VA_ARGS__);                                                          \
    WJR_ASSUME(__VA_ARGS__)

} // namespace wjr

#endif // WJR_ASSERT_HPP__