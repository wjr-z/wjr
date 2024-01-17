require : \
C++17, 64-bit   \
Currently, most optimizations do not target 32-bit encoding, so 32-bit is not currently supported

1. container \
    TODO :  \
    intrusive list, intrusive rb-tree, B+-tree, B-tree ...

2. math \
    addc/subc are similar to __builtin_addc/__builtin_subc.    \
    For GCC/Clang, it has been optimized using inline assembly and __builtin_constant_p.    \
    addc_1/subc_1 are similar to mpn_add_1/mpn_sub_1 with carry flag. \
    addc_n/subc_n are similar to mpn_add_nc/mpn_sub_nc. \
    addc_s/subc_s . \
    abs_subc_n/abs_subc_s, abs of subc_n/subc_s. Ensure that the number of calculations is n, \
    even if two numbers are the same, and can obtain the highest non-zero position.  \
    mulx/mul_1/addmul_1/submul_1 (not optimized yet)/mul_s    \
    divmod_1/divexact_1/... \
    clz/ctz/popcount/is_power_of_two/lowbit/... \
    set_n, unlike memset, which can only assign uint8_t.     \
    set_n can be used to assign uint16_t/uint32_t/uint64_t.     \
    When the pointer can be aligned with sizeof (T) and n is large, it will be aligned.     \
    not_n. Align optimized versions.    \
    find_n/find_not_n/reverse_find_n/reverse_find_not_n.    \
    Each function has two overloads: double array comparison and array to value comparison. \
    compare_n/reverser_compare_n.   \
    TODO : \
    AVX-512 optimaztion for addc_n/subc_n.  \

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

5. template processor (tp)  \
    tp_list