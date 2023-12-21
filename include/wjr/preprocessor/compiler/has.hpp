#ifndef WJR_PREPROCESSOR_COMPILER_HAS_HPP__
#define WJR_PREPROCESSOR_COMPILER_HAS_HPP__

#include <wjr/preprocessor/details/basic.hpp>
#include <wjr/preprocessor/logical/basic.hpp>

#define WJR_HAS_DEF WJR_PP_MAP_DEF(1)
#define WJR_HAS_UNDEF WJR_PP_MAP_DEF(0)

#define WJR_HAS_FIND(MAP, KEY) WJR_PP_CONCAT(WJR_HAS_FIND_I_, WJR_PP_MAP_FIND(MAP, KEY))
#define WJR_HAS_FIND_I_0 0
#define WJR_HAS_FIND_I_1 1
#define WJR_HAS_FIND_I_WJR_PP_NULLPTR 0

// Currently only has_builtin, has_attribute, has_feature are supported.
#define WJR_HAS_BUILTIN_FIND(KEY) WJR_HAS_FIND(WJR_HAS_BUILTIN_, KEY)
#define WJR_HAS_ATTRIBUTE_FIND(KEY) WJR_HAS_FIND(WJR_HAS_ATTRIBUTE_, KEY)
#define WJR_HAS_FEATURE_FIND(KEY) WJR_HAS_FIND(WJR_HAS_FEATURE_, KEY)

// Distinguishing between uppercase and GCC/Clang compiler support.
// Additional Support List :

// WJR_HAS_BUILTIN : ADDC, ASM_ADDC, SUBC, ASM_SUBC
// WJR_HAS_ATTRIBUTE :
// WJR_HAS_FEATURE : IS_CONSTANT_EVALUATED, INT128, INT128_DIV, INLINE_ASM

// ADDC :
//  wjr::builtin_addc, optimized by using __builtin_addc if supported.
// ASM_ADDC : 
//  wjr::builtin_addc, include ADDC and maybe optimize by using inline assembly.

#endif // WJR_PREPROCESSOR_COMPILER_HAS_HPP__