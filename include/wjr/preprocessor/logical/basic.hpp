#ifndef WJR_PREPROCESSOR_LOGICAL_BASIC_HPP
#define WJR_PREPROCESSOR_LOGICAL_BASIC_HPP

#define WJR_PP_BOOL_IF(cond, t, f) WJR_PP_BOOL_IF_I(cond, t, f)
#define WJR_PP_BOOL_IF_I(cond, t, f) WJR_PP_BOOL_IF_I##cond(t, f)
#define WJR_PP_BOOL_IF_I0(t, f) f
#define WJR_PP_BOOL_IF_I1(t, f) t

#define WJR_PP_BOOL_NOT(x) WJR_PP_BOOL_IF(x, 0, 1)

#define WJR_PP_BOOL_AND(x, y) WJR_PP_BOOL_IF(x, WJR_PP_BOOL_IF(y, 1, 0), 0)
#define WJR_PP_BOOL_OR(x, y) WJR_PP_BOOL_IF(x, 1, WJR_PP_BOOL_IF(y, 1, 0))
#define WJR_PP_BOOL_XOR(x, y) WJR_PP_BOOL_IF(x, WJR_PP_BOOL_NOT(y), y)

#endif // ! WJR_PREPROCESSOR_LOGICAL_BASIC_HPP