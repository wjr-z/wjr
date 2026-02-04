#ifndef WJR_PREPROCESSOR_ARITHMATIC_INC_HPP__
#define WJR_PREPROCESSOR_ARITHMATIC_INC_HPP__

#define WJR_PP_INC(x) WJR_PP_INC_I(x)
#define WJR_PP_INC_I(x) WJR_PP_INC_##x

#include <wjr/preprocessor/arithmetic/detail/inc.hpp>

#endif // ! WJR_PREPROCESSOR_ARITHMATIC_INC_HPP__