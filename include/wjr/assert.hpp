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

#include <iostream> // for operator<<, cerr, basic_ostream, ostream
#include <utility>  // for forward

#include <wjr/preprocessor/config/attribute.hpp>

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

#if WJR_DEBUG_LEVEL == 0 && !defined(WJR_LIGHT_ASSERT)
    #define WJR_LIGHT_ASSERT
#endif

namespace wjr {

WJR_NORETURN WJR_COLD extern void __assert_failed(const char *expr, const char *file,
                                                  const char *func, int line) noexcept;

WJR_NORETURN WJR_COLD extern void __assert_light_failed(const char *expr) noexcept;

// LCOV_EXCL_START

/// @private
template <typename... Args>
WJR_NORETURN void __assert_failed_handler(const char *expr, const char *file, const char *func,
                                          int line, Args &&...args) noexcept {
    std::cerr << "Assert message:";
    (void)(std::cerr << ... << std::forward<Args>(args));
    std::cerr << '\n';
    __assert_failed(expr, file, func, line);
}

/// @private
WJR_NORETURN inline void __assert_failed_handler(const char *expr, const char *file,
                                                 const char *func, int line) noexcept {
    __assert_failed(expr, file, func, line);
}

/// @private
template <typename... Args>
WJR_NORETURN void __assert_light_failed_handler(const char *expr, Args &&...) noexcept {
    __assert_light_failed(expr);
}

/// @private
WJR_NORETURN inline void __assert_light_failed_handler(const char *expr) noexcept {
    __assert_light_failed(expr);
}

#if defined(WJR_LIGHT_ASSERT)
    #define WJR_ASSERT_FAILED_HANDLER(expr, _file, _function, _line, ...)                          \
        ::wjr::__assert_light_failed_handler(expr, ##__VA_ARGS__)
#else
    #define WJR_ASSERT_FAILED_HANDLER(expr, file, function, line, ...)                             \
        ::wjr::__assert_failed_handler(expr, file, function, line, ##__VA_ARGS__)
#endif

// LCOV_EXCL_STOP

#define WJR_ASSERT_CHECK_I(expr, ...)                                                              \
    do {                                                                                           \
        if (WJR_UNLIKELY(!(expr))) {                                                               \
            WJR_ASSERT_FAILED_HANDLER(#expr, WJR_FILE, WJR_CURRENT_FUNCTION, WJR_LINE,             \
                                      ##__VA_ARGS__);                                              \
            WJR_UNREACHABLE();                                                                     \
        }                                                                                          \
    } while (false)

// do nothing
#define WJR_ASSERT_UNCHECK_I(...) ((void)0)

#define WJR_ASSERT_ASSUME_CHECK_I(expr, ...)                                                       \
    do {                                                                                           \
        if (WJR_UNLIKELY(!(expr))) {                                                               \
            WJR_ASSERT_FAILED_HANDLER(#expr, WJR_FILE, WJR_CURRENT_FUNCTION, WJR_LINE,             \
                                      ##__VA_ARGS__);                                              \
            WJR_UNREACHABLE();                                                                     \
        }                                                                                          \
    } while (false)

// do nothing
#define WJR_ASSERT_ASSUME_UNCHECK_I(expr, ...) WJR_ASSUME(expr)

// level of assert is zero at default.
#define WJR_ASSERT_L0(...) WJR_ASSERT_CHECK_I(__VA_ARGS__)
#define WJR_ASSERT_ASSUME_L0(...) WJR_ASSERT_ASSUME_CHECK_I(0, __VA_ARGS__)

#if WJR_DEBUG_LEVEL >= 1
    #define WJR_ASSERT_L1(...) WJR_ASSERT_CHECK_I(__VA_ARGS__)
    #define WJR_ASSERT_ASSUME_L1(...) WJR_ASSERT_ASSUME_CHECK_I(__VA_ARGS__)
#else
    #define WJR_ASSERT_L1(...) WJR_ASSERT_UNCHECK_I(__VA_ARGS__)
    #define WJR_ASSERT_ASSUME_L1(...) WJR_ASSERT_ASSUME_UNCHECK_I(__VA_ARGS__)
#endif

#if WJR_DEBUG_LEVEL >= 2
    #define WJR_ASSERT_L2(...) WJR_ASSERT_CHECK_I(__VA_ARGS__)
    #define WJR_ASSERT_ASSUME_L2(...) WJR_ASSERT_ASSUME_CHECK_I(__VA_ARGS__)
#else
    #define WJR_ASSERT_L2(...) WJR_ASSERT_UNCHECK_I(__VA_ARGS__)
    #define WJR_ASSERT_ASSUME_L2(...) WJR_ASSERT_ASSUME_UNCHECK_I(__VA_ARGS__)
#endif

#if WJR_DEBUG_LEVEL >= 3
    #define WJR_ASSERT_L3(...) WJR_ASSERT_CHECK_I(__VA_ARGS__)
    #define WJR_ASSERT_ASSUME_L3(...) WJR_ASSERT_ASSUME_CHECK_I(__VA_ARGS__)
#else
    #define WJR_ASSERT_L3(...) WJR_ASSERT_UNCHECK_I(__VA_ARGS__)
    #define WJR_ASSERT_ASSUME_L3(...) WJR_ASSERT_ASSUME_UNCHECK_I(__VA_ARGS__)
#endif

#define WJR_ASSERT(...) WJR_ASSERT_L1(__VA_ARGS__)
#define WJR_ASSERT_ASSUME(...) WJR_ASSERT_ASSUME_L1(__VA_ARGS__)

#define WJR_CHECK(...) WJR_ASSERT_L0(__VA_ARGS__)

} // namespace wjr

#endif // WJR_ASSERT_HPP__