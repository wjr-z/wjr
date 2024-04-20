#ifndef WJR_ASSERT_HPP__
#define WJR_ASSERT_HPP__

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

#include <wjr/preprocessor.hpp>

#ifdef WJR_ASSERT_THROW
#include <sstream>
#endif

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
#else
#define WJR_ASSERT_NORETURN WJR_NORETURN
#endif

#define WJR_DEBUG_IF(level, expr0, expr1)                                                \
    WJR_PP_BOOL_IF(WJR_PP_GT(WJR_DEBUG_LEVEL, level), expr0, expr1)

#define WJR_DEBUG_EXPR_L(level, expr) WJR_DEBUG_IF(level, expr, )
#define WJR_DEBUG_EXPR(expr) WJR_DEBUG_EXPR_L(0, expr)

class __assert_handler_t {
private:
    template <typename Output>
    static Output &handler(Output &out) {
        return out;
    }

    template <typename Output, typename... Args>
    static Output &handler(Output &out, Args &&...args) {
        out << "Additional information:\n";
        (void)(out << ... << std::forward<Args>(args));
        out << '\n';
        return out;
    }

    template <typename... Args>
    WJR_ASSERT_NORETURN WJR_NOINLINE static void
    fn(const char *expr, const char *file, const char *func, int line, Args &&...args) {
#ifndef WJR_ASSERT_THROW
        if (file[0] != '\0') {
            std::cerr << file << ':';
        }
        if (line != -1) {
            std::cerr << line << ':';
        }
        std::cerr << func << ": Assertion `" << expr << "' failed.\n";
        handler(std::cerr, std::forward<Args>(args)...);
        std::abort();
#else
        std::ostringstream os;
        if (file[0] != '\0') {
            os << file << ':';
        }
        if (line != -1) {
            os << line << ':';
        }
        os << func << ": Assertion `" << expr << "' failed.\n";
        handler(os, std::forward<Args>(args)...);
        WJR_THROW(std::runtime_error(os.str()));
#endif
    }

public:
    template <typename... Args>
    void operator()(const char *expr, const char *file, const char *func, int line,
                    Args &&...args) const {
        fn(expr, file, func, line, std::forward<Args>(args)...);
    }
};

inline constexpr __assert_handler_t __assert_handler{};

#define WJR_ASSERT_NOMESSAGE_FAIL(handler, exprstr)                                      \
    handler(exprstr, WJR_FILE, WJR_CURRENT_FUNCTION, WJR_LINE)
#define WJR_ASSERT_MESSAGE_FAIL(handler, exprstr, ...)                                   \
    handler(exprstr, WJR_FILE, WJR_CURRENT_FUNCTION, WJR_LINE, __VA_ARGS__)

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