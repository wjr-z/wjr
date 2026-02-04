#ifndef WJR_PREPROCESSOR_ARITHMATIC_NEG_HPP__
#define WJR_PREPROCESSOR_ARITHMATIC_NEG_HPP__

#define WJR_PP_NEG(x) WJR_PP_NEG_I(x)
#define WJR_PP_NEG_I(x) WJR_PP_NEG_##x

#include <wjr/preprocessor/arithmetic/detail/neg.hpp>

#endif // ! WJR_PREPROCESSOR_ARITHMATIC_NEG_HPP__