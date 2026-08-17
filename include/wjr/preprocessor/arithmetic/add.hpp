#ifndef WJR_PREPROCESSOR_ARITHMATIC_ADD_HPP__
#define WJR_PREPROCESSOR_ARITHMATIC_ADD_HPP__

#include <wjr/preprocessor/arithmetic/detail/number.hpp>
#include <wjr/preprocessor/detail/basic.hpp>

#define WJR_PP_ADD(x, y) WJR_PP_ADD_I(x, y)
#define WJR_PP_ADD_I(x, y) WJR_PP_ARITHMATIC_TO_NUMBER(WJR_PP_ARITHMATIC_PAIR(x, y))

#endif // ! WJR_PREPROCESSOR_ARITHMATIC_ADD_HPP__