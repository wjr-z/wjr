#ifndef WJR_PREPROCESSOR_ARITHMATIC_ADD_HPP__
#define WJR_PREPROCESSOR_ARITHMATIC_ADD_HPP__

#include <wjr/preprocessor/arithmatic/basic.hpp>
#include <wjr/preprocessor/detail/basic.hpp>

#define WJR_PP_ADD(x, y) WJR_PP_ADD_I(x, y)
#define WJR_PP_ADD_I(x, y)                                                               \
    WJR_PP_ARITHMATIC_TO_NUMBER(WJR_PP_CONCAT(WJR_PP_ARITHMATIC_FROM_NUMBER(x),          \
                                              WJR_PP_ARITHMATIC_FROM_NUMBER(y)))

#endif // ! WJR_PREPROCESSOR_ARITHMATIC_ADD_HPP__