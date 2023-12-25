require : \
C++17, 64-bit   \
Currently, most optimizations do not target 32-bit encoding, so 32-bit is not currently supported

1. container \
    TODO :  \
    intrusive list, intrusive rb-tree, B+-tree, B-tree ...

2. math \
    addc/subc are similar to __builtin_addc/__builtin_subc.    \
    But for those who do not support __builtin_addc and __builtin_subc's compiler has been adapted. \
    For GCC/Clang that supports __builtin_constant_p, constant input has been optimized.  \
    addc_1/subc_1 are similar to mpn_add_1/mpn_sub_1. \
    support carry_in flag.  \
    addc_n/subc_n are similar to mpn_add_n/mpn_sub_n. \
    But also optimized the constant input for GCC/Clang that supports __builtin_constant_p. \
    mulx/mul_1/addmul_1/... \
    The design goal is to better inline these large integer functions.    \
    And generate better code through __builtin_constant_p for constant length or    \
    some other constant information. And support constexpr as much as possible  \
    \
    clz/ctz/popcount/is_power_of_two/lowbit/... \
    TODO : \
    AVX-512 optimaztion for addc_n/subc_n.  \
    mul ... \
    more constexpr math functions ...

3. preprocessor \
    WJR_PP_ADD, WJR_PP_SUB, WJR_PP_(LT|LE|GT|GE|EQ|NE), WJR_PP_INC, WJR_PP_DEC, WJR_PP_NEG. \
    Currently, only 6-bit operations are supported  \
    WJR_PP_BOOL_IF, WJR_PP_BOOL_NOT, WJR_PP_BOOL_AND, WJR_PP_BOOL_OR, WJR_PP_BOOL_XOR.  \
    WJR_PP_QUEUE :  \
    A queue is similar to (a, b, c, d, e).    \
    WJR_PP_QUEUE_CALL_N(queue, ops, n) : Each time the first two elements of the queue are evaluated   \
    according to the first element of ops and placed back at the head of the queue.     \
    For example : 
    ```
    #define F(a, b) a + b
    WJR_PP_QUEUE_CALL_N((1, 2, 3), (F, F), 2) 

    init :
        queue : (1, 2, 3), ops : (F, F)
    first time :
        queue : (1 + 2, 3), ops : (F)
    second time :
        queue : (1 + 2 + 3), ops : ()
    ```
    In this way, it can be easily extended to various functions.    \
    But it is difficult to implement a recursive version.   \
    I will try to support recursion in the future.  \
    support now :   \
    WJR_PP_QUEUE_TRANSFORM(queue, op) : (a, b, c), f -> (f(a), f(b), f(c))  \
    WJR_PP_QUEUE_TRANSFORMS(queue, ops) : (a, b, c), (f, g, h) -> (f(a), g(b), h(c))    \
    WJR_PP_QUEUE_ACCUMULATE(queue, op) : init, (a, b, c), f -> f(f(f(0, 1), 2), 3)  \
    WJR_PP_QUEUE_ACCUMULATES(queue, op) : init, (a, b, c), (f, g, h) -> h(g(f(0, 1), 2), 3) \
    ...

4. compressed_pair  \
    Currently undergoing refactoring. I hope it can be designed as compressed_tuple.

5. vector   \
    vector is broken now.   \
    Vector is designed to have customizable internal data structures,   \
    requiring only partial implementation of necessary interfaces.  \
    The biggest drawback of a vector is that in some cases it knows the actual size,    \
    and in some performance critical positions, the vector is not very suitable. \
    The vector designed based on STL is expected to easily expand to this \
    fixed length capacity situation, throwing an exception when the expansion \
    exceeds the fixed length capacity instead of reallocating. \
    And it should be able to be extended to SSO vectors. \
    And I hope to expand to more complex situations in the future, \
    such as copy on write.

6. more template operations

7. more constexpr support