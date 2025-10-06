/**
 * @file macros/requires.hpp
 * @brief SFINAE helper macros
 * @author wjr
 *
 * Provides convenient macros for template constraints using enable_if.
 */

#ifndef WJR_MACROS_REQUIRES_HPP__
#define WJR_MACROS_REQUIRES_HPP__

/**
 * @brief SFINAE constraint macro
 * @param ... Boolean condition
 *
 * Use in template parameter list to enable function only when condition is true.
 * Example: template<typename T, WJR_REQUIRES(std::is_integral_v<T>)>
 */
#define WJR_REQUIRES(...) std::enable_if_t<(__VA_ARGS__), int> = 0
#define WJR_REQUIRES_I(...) std::enable_if_t<(__VA_ARGS__), int>

#endif // WJR_MACROS_REQUIRES_HPP__