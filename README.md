require : \
C++17

1. container \
TODO :  \
intrusive list, intrusive rb-tree, B+-tree, B-tree ...

2. math \
optimized add, sub for GCC/Clang    \
wjr::addc/wjr::subc are similar to __builtin_addc/__builtin_subc.    \
But for those who do not support __builtin_addc and __builtin_subc's compiler has been adapted. \
For GCC/Clang that supports __builtin_constant_p, constant input has been optimized.  \
wjr::addc_n/wjr::subc_n are similar to mpn_add_n/mpn_sub_n. \
But also optimized the constant input for GCC/Clang that supports __builtin_constant_p. \
TODO : \
AVX-512 optimaztion for addc_n/subc_n.  \
clz, ctz, popcount, mul_1, mul_n, ... \
constexpr math functions ...