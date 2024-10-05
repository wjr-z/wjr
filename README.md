最低要求：  \
C++17，64位系统。  \
绝大多数优化仅针对x64。 目前异常处理支持不佳。  
1. 非侵入式容器 bitset, B+-tree, basic_vector...
2. 侵入式容器 list, forward_list, lock-free forward_list...
3. 预处理器 
4. compressed_pair\<T, U\>    
5. tuple\<Args...\>   
6. span
7. inline_key
8. uninitialized : aligned_storage_t替代品
9. lazy_initialized\<T\> : 延迟初始化
10. biginteger
11. math : 
12. JSON
13. format : to_chars, from_chars 。 fast_float改进。
14. template preprocessor : 
15. stack alloccator : 动态扩展内存分配器。堆预分配内存模拟栈。
16. string 