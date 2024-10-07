#ifndef WJR_PREPROCESSOR_LOGICAL_BASIC_HPP__
#define WJR_PREPROCESSOR_LOGICAL_BASIC_HPP__

#define WJR_PP_BOOL_IF(cond, t, f) WJR_PP_BOOL_IF_I(cond, t, f)
#define WJR_PP_BOOL_IF_I(cond, t, f) WJR_PP_BOOL_IF_I##cond(t, f)
#define WJR_PP_BOOL_IF_I0(t, f) f
#define WJR_PP_BOOL_IF_I1(t, f) t

#define WJR_PP_BOOL_NOT(x) WJR_PP_BOOL_IF(x, 0, 1)

#define WJR_PP_BOOL_AND(x, y) WJR_PP_BOOL_AND_I(x, y)
#define WJR_PP_BOOL_OR(x, y) WJR_PP_BOOL_OR_I(x, y)
#define WJR_PP_BOOL_XOR(x, y) WJR_PP_BOOL_XOR_I(x, y)

#define WJR_PP_BOOL_AND_I(x, y) WJR_PP_BOOL_AND_I##x##y()
#define WJR_PP_BOOL_AND_I00() 0
#define WJR_PP_BOOL_AND_I01() 0
#define WJR_PP_BOOL_AND_I10() 0
#define WJR_PP_BOOL_AND_I11() 1

#define WJR_PP_BOOL_OR_I(x, y) WJR_PP_BOOL_OR_I##x##y()
#define WJR_PP_BOOL_OR_I00() 0
#define WJR_PP_BOOL_OR_I01() 1
#define WJR_PP_BOOL_OR_I10() 1
#define WJR_PP_BOOL_OR_I11() 1

#define WJR_PP_BOOL_XOR_I(x, y) WJR_PP_BOOL_XOR_I##x##y()
#define WJR_PP_BOOL_XOR_I00() 0
#define WJR_PP_BOOL_XOR_I01() 1
#define WJR_PP_BOOL_XOR_I10() 1
#define WJR_PP_BOOL_XOR_I11() 0

#endif // ! WJR_PREPROCESSOR_LOGICAL_BASIC_HPP__