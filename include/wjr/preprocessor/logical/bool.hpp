#ifndef WJR_PREPROCESSOR_LOGICAL_BOOL_HPP__
#define WJR_PREPROCESSOR_LOGICAL_BOOL_HPP__

#define WJR_PP_BOOL(x) WJR_PP_BOOL_I(x)
#define WJR_PP_BOOL_I(x) WJR_PP_BOOL_##x

#include <wjr/preprocessor/logical/detail/bool.hpp>

#endif // WJR_PREPROCESSOR_LOGICAL_BOOL_HPP__