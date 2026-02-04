#ifndef WJR_PREPROCESSOR_ARITHMATIC_NOT_HPP__
#define WJR_PREPROCESSOR_ARITHMATIC_NOT_HPP__

#define WJR_PP_NOT(x) WJR_PP_NOT_I(x)
#define WJR_PP_NOT_I(x) WJR_PP_NOT_##x

#include <wjr/preprocessor/arithmetic/detail/not.hpp>

#endif // ! WJR_PREPROCESSOR_ARITHMATIC_NOT_HPP__