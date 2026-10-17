# 表达式物化为拥有数据的 tensor

`<wjr/np/evaluate.hpp>` 提供 `np::eval(expression)`。它分配一个结果容器，
将表达式融合写入该容器，返回 `expected<tensor<...>, shape_error>`。
即使输入只是单个 view，结果也始终独立拥有元素，不会返回借用对象。

```cpp
auto x = np::as_expr(input.view());
auto result = np::eval((x - 0.5) * (x - 0.5));
if (result) {
    auto values = result->view();
    // values 借用 result；result 必须保持存活且未改变其存储。
}
```

结果保留表达式逐节点推导的 `value_type`、编译期 extents 和 packed layout。
同布局的 packed 表达式保留 `layout_left`；混合布局或 strided 输入选择行优先
结果，并按逻辑坐标计算。自然 shape 由表达式的公共广播规则解析，而不是取首个
数组的 shape：维度右对齐，缺少的维度视为 1；相同维度或任一侧为 1 时可合并。
`0` 与 `1` 合并为 `0`，`0` 与大于 1 的维度不相容。不相容的 shape 在分配前返回
`shape_error::expression_shape_mismatch`。指定结果布局留给后续扩展。

纯标量表达式产生 rank 0、一个元素的 owner。空数组保留所有维度，且不读取输入。
默认容器为 `wjr::vector<T>`；显式 allocator 可通过以下重载传入，其 `value_type`
必须与表达式结果完全相同：

```cpp
auto result = np::eval(expression, std::allocator_arg, allocator);
```

shape 的索引范围、乘积、字节跨度和容器容量检查直接复用 `make_tensor`。
分配失败沿用 allocator 的异常策略，不被转换为新的错误包装。`bool` 标量和 view
可物化为真实 `bool*` 存储。owner 必须先显式调用 `.view()`，不会被计算接口隐式借用或复制。

普通 `eval_to` 的无隐藏分配约定没有改变。`eval` 是明确的物化边界，使用
`make_tensor(shape, default_construct, allocator_arg, allocator)` 开始全部结果对象的生命周期，
再由融合循环写入。标准 allocator 与普通数值类型下，不需要先清零结果数组；
不写入未构造的 vector capacity。普通 owner 工厂仍按契约值初始化，自定义 allocator
的构造钩子仍会执行，其必要写入不被绕过。性能敏感的重复计算可以复用已有输出并调用 `eval_to`。

与该功能相同语义的最低工作是分配、合法开始生命周期、一次结果写出。
当前方案消除了原先必有的初始化遍历；仍保存 vector capacity 与必要 shape，
分配器可返回额外容量，分配、检查及异常清理也没有因该优化消失。
它不代表任意 allocator、表达式或编译器的整体执行已经等同于最优手写实现。

本次验收使用 x64 baseline、C++20、debug level 0，Clang 18/GCC 12 为 `-O3`，
MSVC 19.42 为 `/O2`。数值 `2 * input + 1` 的 owning eval 探针原先存在一处结果
`memset`，切换后所测三个编译器的可达调用路径均不再清零 payload；普通 sized
std/WJR vector 构造仍保留清零，显式 default-init 和 `new double[n]` 均不写 payload。
这证明所测初始化遍历被移除，不是完整吞吐量或硬件计数器结论。

默认 vector 的移动修复额外清空源对象另外两个指针；相同 move/tuple-move 探针中，
Clang 指令数 9→10、GCC 11→12、MSVC 10→12，没有增加分配或引用计数。
这是保持可查询移动后表示所需的固定元数据工作。
范围赋值的失败清理只在可能抛出的增长路径出现：标准 allocator、double 指针输入的
无守卫/有守卫对照在 Clang/GCC 分别为相同的 68/99 条指令，
MSVC wrapper 及其五个可达函数的指令也一致；不据此推广到任意迭代器和元素类型。

[`examples/np/evaluate.cpp`](../../examples/np/evaluate.cpp) 展示平方偏差的一次融合物化，
运行后输出：

```text
materialized squared deviations:
6.25 2.25 0.25
0.25 2.25 6.25
```
