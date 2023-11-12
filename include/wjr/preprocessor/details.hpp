#ifndef WJR_PREPROCESSOR_DETAILS_HPP__
#define WJR_PREPROCESSOR_DETAILS_HPP__

#include <wjr/preprocessor/details/basic.hpp>
#include <wjr/preprocessor/details/iota.hpp>
#include <wjr/preprocessor/details/repeat.hpp>
#include <wjr/preprocessor/logical/basic.hpp>

#define WJR_PP_TABLE_DEF() WJR_PP_HOLDER, 1
#define WJR_PP_IS_TABLE(TABLE, KEY)                                                      \
    WJR_PP_BOOL_NOT(WJR_PP_IS_NULLPTR(WJR_PP_MAP_FIND(TABLE, KEY)))

#endif // ! WJR_PREPROCESSOR_DETAILS_HPP__