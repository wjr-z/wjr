# 压缩存储的语义与布局边界

`compressed_pair` 组合已有 EBO 存储和元素的特殊成员约束，不增加共享控制块。
本次修复控制构造、赋值、访问及 swap 的最近路径，保留既有存储方案。

## 已修复的契约

- `(value, default_construct)` 的三个存储分支正确推导并转发第一个参数，
  包括左值引用和 move-only 类型；不读取被有意默认初始化的标量值。
- 异构赋值的 noexcept 检查实际左值元素及转发后的来源类别。
  基础类型赋值恢复 noexcept；可能抛出的 ref-qualified 或右值赋值可以正常传播异常。
- 右值 member get 和 const 右值 std::get 保持索引、const 性及引用折叠，
  不把引用元素变成新对象。
- 元素交换使用 ADL，约束和 noexcept 都按标准 swappable traits 判断。
  hidden friend 支持普通 unqualified swap、ranges::swap 和嵌套 pair，
  保留原有 std::swap 兼容入口；不要求仅能通过 ADL 交换的元素可以移动。

嵌套交换还需要将不可执行的 tuple-like get 纳入替换检测。
`_is_tuple_test_impl` 在 get 不可用时返回 false，不在 traits 或候选转换构造中产生硬错误。
此变更不修改 capture_leaf 的存储，也不改写 tuple 实现。

## 重复类型与 MSVC ABI 的局部布局修复

所有 ABI 中，相同 cv-unqualified 类型都采用成员存储，使两个元素具备标准布局的 container_of 恢复路径。
GNU ABI 下，普通及过对齐的同类型重复空元素保持原来的尺寸、对齐和地址偏移。
混合 cv 的 `pair<E, const E>` 原来可能重叠，修复后也会有意改变布局。

在本地 Clang 18.1.8/MSVC ABI 和 MSVC 19.42 中，继承的空元素可能与相同类型元素、
或另一个元素的相同类型基类重合。该 ABI 还额外针对可静态识别的基类关联，
让这两个元素采用成员存储。判断忽略顶层 cv，不修改全局 is_compressed。
相同类型子对象的地址约束参见 [C++ 对象模型](https://eel.is/c++draft/intro.object#10)。

这会有意改变受影响实例的布局，使用这些实例的翻译单元及二进制库需要一起重新编译。
例如本地 x64、C++20、Release 的结果如下；E 是普通空类，A 是 alignas(64) 空类。

| 实例 | GCC 12.1 前后尺寸 | Clang 18/MSVC ABI 前→后 | MSVC 19.42 前→后 |
| --- | ---: | ---: | ---: |
| pair<E, int> | 4 | 4 → 4 | 4 → 4 |
| pair<allocator<int>, int> | 4 | 4 → 4 | 4 → 4 |
| pair<stateful int wrapper, int> | 8 | 8 → 8 | 8 → 8 |
| pair<E, E> | 2 | 1 → 2 | 1 → 2 |
| pair<E, const E> | 1 → 2 | 1 → 2 | 1 → 2 |
| pair<A, A> | 128 | 64 → 128 | 64 → 128 |

普通、final、有状态、cv-qualified、过对齐及独立空策略的现有尺寸和特殊成员 traits 有针对性检查。
同/反方向基类关联、重复空类型和 cv-qualified 同类元素验证不同地址；
container_of 跟随实际存储分支恢复对象。
三个编译器上，普通空策略、真实 allocator 和有状态元素的四个访问探针，
前后生成相同指令；这些结果不等于完整工作负载吞吐量测量。

## 仍存在的限制

本次判断**不能识别任意成员嵌套关系**。例如 `struct C { E value; };` 中的 E，
可能仍与 pair<E, C> 的第一个元素在 MSVC ABI 下重合；std::array<E, 1> 也有同类问题。
需要这种对象地址语义时，应使用普通成员存储，不能把这个 EBO 路径当作通用保证。

外部原型比较了 no_unique_address 成员存储：本地 GCC 12 和 Clang 18/MSVC ABI 可以区分该嵌套情形，
MSVC 19.42 的 msvc 属性仍观察到重合。因此，本次不切换属性或整体重写布局，
也不宣称升级 C++20 就解决了全部压缩存储问题。
后续若更换表示，应单独核对编译器、对象地址、ABI、类型尺寸及生成码，
不能为规避某类实例而让所有 MSVC allocator/标量组合失去压缩。
