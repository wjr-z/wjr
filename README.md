最低要求：  \
C++17。  \
绝大多数优化仅针对GCC/Clang x64。   
1. 非侵入式容器 
- 可定制vector类，实现对应接口即可。  \
  目前默认实现了vector, static_vector, fixed_vector, sso_vector。   \
  static_vector 编译期确定静态长度，不会动态扩容。当 WJR_DEBUG_LEVEL >= 1 时 \
  且需要扩容时使用WJR_ASSERT报错。  \
  fixed_vector 在构造时给定一个长度，且后续操作不进行动态扩容。DEBUG 同static_vector。  \
  sso_vector 进行小对象优化，类似于std::string的SSO优化，提供一个模板参数用于确定小对象优化的size。
- dynamic_bitset
  动态长度bitset。暂未完全完成，优先级较低。
- B+树
  高度优化的内存B+树。目前暂时缺少对Value为trivial并且size较小时的inline优化。  \
  对于trivial且size小于等于8的key使用值而非指针，使用inline_key统一接口实现。   \
  对copy和search均进行了优化。目前随机情况下（缓存较差）较小点数的平均性能上略慢于红黑树，较大点数快于红黑树。    \
  缓存友好（例如数据有规律）时平均性能更好。
2. 侵入式容器
   处于设计阶段。   \
   侵入式容器存在较为明显的内存安全问题。生命周期较为独立，且内存分配器与容器解耦，容易遗漏释放。   \
   考虑结合使用unique_ptr的方案。
- list
  双向链表
- slist
  单链表
3. 预处理器
- 预处理阶段的ADD, SUB, CMP, INC, DEC, NEG, NOT等。
- 预处理阶段的MAP。\
  以WJR_HAS_DEBUG为例
  ```
  #define WJR_HAS_DEBUG_XXX WJR_HAS_DEF_VAR(3) 
  static_assert(WJR_HAS_DEBUG(XXX) == 3, "");
  #define WJR_HAS_DEBUG_NO_XXX WJR_HAS_DEF
  static_assert(WJR_HAS_DEBUG(XXX) == 0, "");
  ```
  目前定义了对应的 BUILTIN, ATTRIBUTE, FEATURE, SIMD, DEBUG。
- 队列 (a, b, c) 的算法等   \
  例如WJR_PP_QUEUE_TRANSFORM((a, b, c), f) -> (f(a), f(b), f(c))。  \
  较为复杂，后续考虑简化实现。
4. compressed_pair\<T, U\>    
  压缩对，使用EBO（空基类优化）。   \
  使用CRTP，一般与struct相比零开销。    \
  使用 dctor (default constructor标记) 可以使用默认构造代替值初始化。   \
  可使用成员函数get<0>, get<1>，或者first(), second()，或者operator[]\(std::integral_constant<B, index>\)   \
  示例：
  ```
  compressed_pair<int, int> x;
  x.first() == x.template get<0>() == x[0_zu]
  ```
5. tuple\<Args...\>   
  与std::tuple规范相同。优化类似compressed_pair。
6. span
  span的一个简单C++17实现。
7. inline_key
  值和指针的统一接口。例如函数传参根据T的大小，是否trivial等使用T或const T&。   \
  在B+树中使用inline_key进行优化，例如key为int, long等时显然可以使用值而非指针进行优化。    \
  规范待调整。
8. uninitialized
  一系列使用分配器的uninitialized函数。 \
  uninitialized类，可延迟构造，不进行析构，确保析构前为空。开启对应DEBUG level后会判断错误使用。
9. lazy\<T\>
  使用uninitialized实现，但会进行析构，需确保析构前不为空。
10. math