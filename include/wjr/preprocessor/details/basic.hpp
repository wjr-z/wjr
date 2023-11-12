#ifndef WJR_PREPROCESSOR_DETAILS_BASIC_HPP__
#define WJR_PREPROCESSOR_DETAILS_BASIC_HPP__

#define WJR_PP_EMPTY(...)

#define WJR_PP_CONCAT(x, y) WJR_PP_CONCAT_I(x, y)
#define WJR_PP_CONCAT_I(x, y) x##y

#define WJR_PP_EXPAND(x) WJR_PP_EXPAND_I(x)
#define WJR_PP_EXPAND_I(x) x

#define WJR_PP_STR(x) WJR_PP_STR_I(x)
#define WJR_PP_STR_I(x) #x

#define WJR_PP_ESC(x) WJR_PP_ESC_(WJR_PP_ESC_I, x)
#define WJR_PP_ESC_(M, x) M x
#define WJR_PP_ESC_I(...) __VA_ARGS__

// don't support 0 agument
#define WJR_PP_ARGS_LEN(...) WJR_PP_ARGS_LEN_I(__VA_ARGS__)

#define WJR_PP_ARGS_LEN_I(...)                                                           \
    WJR_PP_EXPAND(WJR_PP_ARGS_LEN_(0, ##__VA_ARGS__, WJR_PP_ARGS_LEN_RSEQ_N()))

#define WJR_PP_ARGS_LEN_RSEQ_N()                                                         \
    64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44,  \
        43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24,  \
        23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2,  \
        1, 0
#define WJR_PP_ARGS_LEN_(...) WJR_PP_EXPAND(WJR_PP_ARGS_LEN_N(__VA_ARGS__))
#define WJR_PP_ARGS_LEN_N(                                                               \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18,     \
    _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, \
    _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, \
    _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, N, ...)             \
    N

#define WJR_PP_HOLDER
#define WJR_PP_NULLPTR

#define WJR_PP_IS_NULLPTR(VAL) WJR_PP_IS_NULLPTR_I(WJR_PP_IS_NULLPTR##VAL, 0)
#define WJR_PP_IS_NULLPTR_I(...) WJR_PP_IS_NULLPTR_II(__VA_ARGS__)
#define WJR_PP_IS_NULLPTR_II(HOLDER, VAL, ...) VAL
#define WJR_PP_IS_NULLPTR_WJR_PP_NULLPTR WJR_PP_HOLDER, 1

#define WJR_PP_MAP_DEF(VAL) WJR_PP_HOLDER, VAL

// if MAP ## KEY is defined as WJR_PP_MAP_DEF, then return VAL
// else return WJR_PP_NULLPTR
#define WJR_PP_MAP_FIND(MAP, KEY) WJR_PP_MAP_FIND_I(MAP, KEY)
#define WJR_PP_MAP_FIND_I(MAP, KEY) WJR_PP_MAP_FIND_II(MAP##KEY, WJR_PP_NULLPTR)
#define WJR_PP_MAP_FIND_II(...) WJR_PP_MAP_FIND_III(__VA_ARGS__)
#define WJR_PP_MAP_FIND_III(HOLDER, VAL, ...) VAL

#define WJR_PP_DEF_VAR(VAR) _WJR_PP_FROM_VAR(VAR)

#define WJR_PP_FROM_VAR(VAR) WJR_PP_FROM_VAR_I(WJR_PP_FROM_VAR##VAR, VAR)
#define WJR_PP_FROM_VAR_I(...) WJR_PP_FROM_VAR_II(__VA_ARGS__)
#define WJR_PP_FROM_VAR_II(HOLDER, VAL, ...) VAL
#define WJR_PP_FROM_VAR_WJR_PP_FROM_VAR(VAR) WJR_PP_HOLDER, VAR

#endif // ! WJR_PREPROCESSOR_DETAILS_BASIC_HPP__