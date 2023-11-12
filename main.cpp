#include <iostream>
#include <wjr/preprocessor.hpp>
int main() {
#define WJR_PP_MAP_inline WJR_PP_MAP_DEF
    std::cout << WJR_PP_STR(WJR_PP_EXPAND(WJR_PP_MAP(WJR_PP_MAP_, inline))) << '\n';
    return 0;
}
