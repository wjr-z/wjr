#include <iostream>
#include <wjr/preprocessor.hpp>
int main() {
#define WJR_PP_FUNC_INLINE WJR_PP_TABLE_DEF()
    std::cout << WJR_PP_STR((WJR_PP_IS_TABLE(WJR_PP_FUNC_, INLINE))) << '\n';
    return 0;
}
