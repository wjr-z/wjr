#ifndef WJR_PREPROCESSOR_ARITHMATIC_SUB_HPP__
#define WJR_PREPROCESSOR_ARITHMATIC_SUB_HPP__

#include <wjr/preprocessor/arithmetic/add.hpp>
#include <wjr/preprocessor/arithmetic/inc.hpp>
#include <wjr/preprocessor/arithmetic/neg.hpp>

#define WJR_PP_SUB(x, y) WJR_PP_ADD(x, WJR_PP_NEG(y))

#endif // ! WJR_PREPROCESSOR_ARITHMATIC_SUB_HPP__