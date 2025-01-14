最低要求：  \
C++17，64位系统。  \
绝大多数优化仅针对x64。 目前异常处理支持不佳。  
1. 非侵入式容器 bitset, B+-tree, basic_vector...
- bitset : 支持不初始化值的bitset。在解析器中很常见，仅需根据depth更新而不需要初始化。
- B+-tree : 优化的B+-tree, 对每次拷贝长度的上下限进行了计算，并根据此定制copy/copy_backward。
bset, bmap ... 
- basic_vector : 可定制通用vector，暂时支持类似std::vector, std::inplace_vector(C++26), fixed_vector等。\
inplace_vector为栈上分配内存。  \
fixed_vector仅构造时动态分配内存，之后不进行扩容，这有利于部分优化实现。\
biginteger中size为int32_t，需要表示正负数，也可定制其vector，只需修改其storage即可。    \
提供安全的方式修改vector的storage，即安全的方式从T*转换为vector\<T\>，或反过来。
2. 侵入式容器 list, forward_list, lock-free forward_list...
类似Linux侵入式容器
3. 预处理器
4. compressed_pair\<T, U\>  \
EBO优化pair。尽可能的trivial
5. tuple\<Args...\>   \
尽可能的trivial
6. span \
C++20 span的C++17实现。
7. inline_arg   \
模板函数中很难选择使用值传递还是引用传递，这是const T&和T的wrapper，可以根据trivial, size等手动/自动选择相应的wrapper。
8. uninitialized : aligned_storage_t替代品  \
尽可能的trivial。手动构造和析构。
9. lazy_initialized\<T\> : 延迟初始化   \
类似于uninitialized，但是必须在生命周期前构造，生命周期结束时自动析构。
10. math : \
clz(countl_zero), ctz(coutr_zero), popcount, uint128_t, mul64x64 ...    \
大整数底层实现，性能与GMP中mpn基本相同(x64下)，部分略快于mpn，部分算法优化远快于mpn。   \
GCC/Clang支持内联汇编，部分简单函数可内联。后续可能对常量长度进行专门优化。
11. biginteger :    \
大整数库封装类
12. JSON :  \
综合了simdjson的性能和nlohmann的易用性，使用SIMD加速解析，使用nlohmann类似数据结构构建document，    \
使用了定制的relocate优化。可自定义visitor，例如使用虚函数直接根据类构建，或类似simdjson生成迭代器等。
13. format : to_chars, from_chars ...
fast_float整数path优化。    \
to_chars支持直接输出到迭代器。对于back_inserter，并非简单的定义buffer，写入到buffer然后拷贝到迭代器中，  \
而是判断容器是否支持直接拷贝（例如默认std::char_traits可以简单拷贝，其他traits需要自定义是否支持），是否支持resize/append，\
若支持，则可以使用resize/append后直接写入
```
std::basic_string<char, nodex<char>> str;
        auto ptr = to_chars_unchecked(std::back_inserter(str), 123);
```
14. template preprocessor : \
例如
```
tp_sort_t<tp_list<integral_constant<int, 3>, integral_constant<int, 2>,
                                     integral_constant<int, 1>>>
=> tp_list<integral_constant<int, 1>, integral_constant<int, 2>,
                                     integral_constant<int, 3>>
```
可自定义比较参数。  \
15. stack alloccator : 动态扩展内存分配器。堆预分配内存模拟栈   \
16. string :    \
实现部分C++17中不支持的函数，例如resize_and_overwrite、starts_with、ends_with等 \
17. switch of tp_list.  \
Example :
```
using type = tp_integers_list_t<char, 'a', 'b', 'c', '3', '4'>;
vswitch<type>('a', [](auto) {
    ///
});
```
tp_integers_list_t可以通过宏字符串参数构建，也可以使用tp_xxx_t构建。    \
直接调用switch，支持最大256个case。 \
18. expected    \
C++20 expected的C++17实现。    \
新增compressed_unexpected，但目前仅为复用代码，后续可能移至其他类（使用错误码而非bool记录是否存在值）。
19. crtp    \
control_special_members_base可以方便的生成尽可能trivial的特殊成员函数。 \
enable_special_members_of_args_base可以方便的禁用特殊成员函数。
20. concurrency \
pause，lock-free单向链表简单实现，spin_mutex    \
todo : 
1. 无锁数据结构，RCU, scheduler等
2. 使用forked fastfloat/dragonbox.
3. 支持使用fmt

## todo
1. 目前尚未适配需要使用GMP的测试。需要自行指定GMP Path。
2. 测试CPU x86-64 alderlake（主要的优化target cpu）。
