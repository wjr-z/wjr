/**
 * @file preprocessor/logical.hpp
 * @brief Preprocessor logical operations
 * @author wjr
 *
 * Provides preprocessor macros for boolean and conditional logic.
 */

#ifndef WJR_PREPROCESSOR_LOGICAL_HPP__
#define WJR_PREPROCESSOR_LOGICAL_HPP__

#include <wjr/preprocessor/logical/basic.hpp>
#include <wjr/preprocessor/logical/bool.hpp>

#define WJR_PP_BOOL_IF_NE(expr, t, f) WJR_PP_BOOL_IF(WJR_PP_BOOL(expr), t, f)

#endif // ! WJR_PREPROCESSOR_LOGICAL_HPP__