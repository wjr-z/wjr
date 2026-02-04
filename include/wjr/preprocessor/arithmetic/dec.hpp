#ifndef WJR_PREPROCESSOR_ARITHMATIC_DEC_HPP__
#define WJR_PREPROCESSOR_ARITHMATIC_DEC_HPP__

#define WJR_PP_DEC(x) WJR_PP_DEC_I(x)
#define WJR_PP_DEC_I(x) WJR_PP_DEC_##x

#include <wjr/preprocessor/arithmetic/detail/dec.hpp>

#endif // WJR_PREPROCESSOR_ARITHMATIC_DEC_HPP__