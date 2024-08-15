#ifndef WJR_ASSERT_HPP__
#define WJR_ASSERT_HPP__

/**
 * @file assert.hpp
 * @author wjr
 * @brief Assertion utilities
 *
 * @details WJR_DEBUG_LEVEL : 0 ~ 3 \n
 * 0 : Release \n
 * 1 : Beta \n
 * 2 : Runtime detect \n
 * 3 : Maximize runtime detect, for debug \n
 * If WJR_DEBUG_LEVEL is not defined, \n
 * If NDEBUG is defined, WJR_DEBUG_LEVEL is set to 0 by default. \n
 * Otherwise, WJR_DEBUG_LEVEL is set to 1 by default. \n
 * WJR_ASSERT_L(level, expr) : Specify the level of assertion, \n
 * if the WJR_DEBUG_LEVEL is greater than or equal to the level, \n
 * the assertion is executed. \n
 * WJR_ASSERT(expr) : Equivalent to WJR_ASSERT_L(1, expr) \n
 * WJR_ASSERT_0(expr) : Always execute the assertion \n
 *
 * @version 0.1
 * @date 2024-06-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <iostream>

#include <wjr/preprocessor.hpp>

#ifndef WJR_DEBUG_LEVEL
#if defined(NDEBUG)
#define WJR_DEBUG_LEVEL 0
#else
#define WJR_DEBUG_LEVEL 1
#endif
#endif

#if WJR_DEBUG_LEVEL < 0 || WJR_DEBUG_LEVEL > 3
#error "WJR_DEBUG_LEVEL must be 0 ~ 3"
#endif

namespace wjr {

#define WJR_DEBUG_IF(level, expr0, expr1)                                                \
    WJR_PP_BOOL_IF(WJR_PP_GT(WJR_DEBUG_LEVEL, level), expr0, expr1)

WJR_NORETURN extern void __assert_failed(const char *expr, const char *file,
                                         const char *func, int line) noexcept;

// LCOV_EXCL_START

/// @private
template <typename... Args>
WJR_NOINLINE void __assert_handler(const char *expr, const char *file, const char *func,
                                   int line, Args &&...args) noexcept {
    std::cerr << "Additional information: ";
    (void)(std::cerr << ... << std::forward<Args>(args));
    std::cerr << '\n';
    __assert_failed(expr, file, func, line);
}

/// @private
inline void __assert_handler(const char *expr, const char *file, const char *func,
                             int line) noexcept {
    __assert_failed(expr, file, func, line);
}

// LCOV_EXCL_STOP

#define WJR_ASSERT_CHECK_I(expr, ...)                                                    \
    do {                                                                                 \
        if (WJR_UNLIKELY(!(expr))) {                                                     \
            ::wjr::__assert_handler(#expr, WJR_FILE, WJR_CURRENT_FUNCTION, WJR_LINE,     \
                                    ##__VA_ARGS__);                                      \
        }                                                                                \
    } while (false)

// do nothing
#define WJR_ASSERT_UNCHECK_I(expr, ...)                                                  \
    do {                                                                                 \
    } while (false)

// level = [0, 2]
// The higher the level, the less likely it is to be detected
// Runtime detect  : 1
// Maximize detect : 2
#define WJR_ASSERT_L(level, ...)                                                         \
    WJR_DEBUG_IF(level, WJR_ASSERT_CHECK_I, WJR_ASSERT_UNCHECK_I)                        \
    (__VA_ARGS__)

// level of assert is zero at default.
#define WJR_ASSERT_L0(...) WJR_ASSERT_CHECK_I(__VA_ARGS__)
#define WJR_ASSERT_L1(...) WJR_ASSERT_L(1, __VA_ARGS__)
#define WJR_ASSERT_L2(...) WJR_ASSERT_L(2, __VA_ARGS__)
#define WJR_ASSERT_L3(...) WJR_ASSERT_L(3, __VA_ARGS__)
#define WJR_ASSERT(...) WJR_ASSERT_L1(__VA_ARGS__)

#define WJR_ASSERT_ASSUME_L(level, ...)                                                  \
    WJR_ASSERT_L(level, __VA_ARGS__);                                                    \
    __WJR_ASSERT_ASSUME_L_ASSUME(__VA_ARGS__)
#define __WJR_ASSERT_ASSUME_L_ASSUME(expr, ...) WJR_ASSUME(expr)

#define WJR_ASSERT_ASSUME_L0(...) WJR_ASSERT_ASSUME_L(0, __VA_ARGS__)
#define WJR_ASSERT_ASSUME_L1(...) WJR_ASSERT_ASSUME_L(1, __VA_ARGS__)
#define WJR_ASSERT_ASSUME_L2(...) WJR_ASSERT_ASSUME_L(2, __VA_ARGS__)
#define WJR_ASSERT_ASSUME_L3(...) WJR_ASSERT_ASSUME_L(3, __VA_ARGS__)
#define WJR_ASSERT_ASSUME(...) WJR_ASSERT_ASSUME_L1(__VA_ARGS__)

} // namespace wjr

#endif // WJR_ASSERT_HPP__