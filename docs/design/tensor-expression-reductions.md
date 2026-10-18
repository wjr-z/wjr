# 表达式的 terminal sum

`<wjr/np/expression_reduce.hpp>` 为现有值闭包表达式提供立即执行的规约：

```cpp
const auto x = np::as_expr(input_view);
const auto residual = x - np::as_expr(mean_view);
const double energy = np::sum(residual * residual);
np::sum_to(row_energy, residual * residual, np::axes<1>);
np::sum_to(row_energy_keepdims, residual * residual, np::axes<1>, np::keepdims);
```

`sum(expr)` 返回标量；`sum_to(dst, expr, axes<N>)` 写入调用方的输出。
后者首版只接受一个编译期 axis，删除该轴；加 `keepdims` 则把它的 extent 设为 1。
rank 0 表达式只有全量 `sum`，会计算一次。
这些入口不会构造 lazy reducer，也不会先 `eval` 成中间 tensor。
现有 [view 规约](tensor-reductions.md) 保持原接口和行为。

## Shape、类型与顺序

规约域是表达式各叶子按右对齐广播得到的自然 shape，不由输出 shape 扩张。
`sum_to` 的输出必须恰好匹配删除或保留该轴后的 shape。
静态不兼容的输出被约束拒绝；动态叶子广播兼容性和输出 shape 是调用前置条件，
在 `WJR_ASSERT_L2` 中验证。Release 关闭断言后，必要的动态 shape 解析仍会执行；
入口没有额外的错误对象或 checked 重载。

默认 accumulator 是 `decltype(T{} + T{})`，`T` 为整个表达式的结果类型；
例如 `uint8_t` 和 `bool` 的累计类型通常是 `int`。
可以写 `sum<double>(expr)` 或 `sum_to<double>(dst, expr, axes<N>)` 显式选择。
显式类型要求自身相加仍返回同一类型，因此 `bool`、`short` 等不会被接受。
每个表达式节点先遵守自己的 C++ 类型语义，再转换成 accumulator；
例如 `(integer_a / integer_b) + float_c` 保留中间整数除法。
输出类型只影响最后一次赋值，不决定累计类型，也不会通过读回输出进行累计。

全量规约从零开始，按逻辑行主序依次执行 `acc += static_cast<Acc>(value)`；
这与输入的物理 `layout_left`、`layout_right` 或正步长布局无关。
单轴规约中，每个输出独立从零开始，按所选轴索引递增累计。
泛步长全量路径跨行传递同一个 accumulator，不将各行分别求和后相加。
这里不提供重排、树状求和、补偿求和或 SIMD 水平规约。
浮点 NaN、舍入和溢出遵守相应 C++ 运算；有符号整数中间值和累计值不得溢出。
编译器的 fast-math 等选项仍可能改变语言通常保证的浮点行为。

空域的全量结果为零。若所选轴为空而输出非空，`sum_to` 写入零；
若输出本身为空，入口直接返回。这些路径不读取叶子元素，也不计算输入 stride
或对 null 数据指针做偏移。正常路径同现有 kernel 一样要求有效的 mdspan 映射、
元素生命周期和可表示的 extent/offset；表达式的逻辑域也必须可由 `size_t` 表示。

## 借用、异常与开销

输入支持现有表达式的 arithmetic 叶子、scalar、广播和正步长映射；
输出支持可写 arithmetic 元素、默认 accessor 及 left/right/stride 布局。
不接受 volatile、自定义 accessor、负步长或零步长源映射。
广播后的有效访问步长可以为零。
`sum_to` 的输出逻辑元素必须与所有输入叶子的逻辑元素不相交；不扫描地址检测别名。
AST 按值保存节点和 view 元数据，数据仍由调用方持有，必须在整个执行期间存活。

公开运算的 `noexcept` 跟随表达式读取。内置算术路径不抛出；
公开 expression 节点使用可抛静态 operation 时，异常正常向外传播。
单轴输出在一条 line 完成后才写入：此前完成的 line 保留，失败 line 未提交；
不提供全输出回滚。

入口一次解析 shape，并绑定叶子指针和广播 stride。
packed-right 且每个非 scalar 叶子恰好覆盖整个逻辑域时，使用一次线性循环；
这个 witness 建立后，动态线性元素数来自已有有效 packed 叶子的 shape。
其他路径使用现有 walker 遍历外轴，在内轴循环中递增整数偏移。
全静态 shape 不做运行时 checked shape 解析。
每个坐标按 AST 语义计算一次；重复子树仍会重复求值，没有公共子表达式缓存。
辅助空间是编译期 rank 与叶子数量决定的少量栈元数据，没有中间 payload 或隐式分配。

二维 `axes<0>` 的非抛出内建算术/pure map 子树，在列数至少为 4、每个叶子的有效列步长
为 0 或 1 时，可交错处理四个独立输出列。每列仍按原行顺序从零累计，没有横向合并 accumulator，
显式 Acc 和最终输出转换保持原语义。行间 padding、行/列广播和输出 stride 沿用已有绑定；
叶子全部为单位列步长时使用常量 lane 偏移，否则使用 unit/splat 路径，最多两种模式。
不足四列、其他步长/axis/rank 继续原路径，块末尾的 1–3 列也保持原有逐线累加。
资格与模式仅在入口判断，不在每个点重查 shape，不复制 callable，也不公开块宽参数。

`noexcept` 并不意味着没有错误状态副作用。math、cast、mask 和未知静态 operation
首版保守保持原逐列路径；例如 `std::log` 的 errno 仍按原 line 顺序调用，
不新增不同标准库之间 errno 取值一致的承诺。可能抛出的 map 同样保留逐线完成后提交，
不把它改成可能在异常时失去已完成输出的跨列交错。

这并不保证所有布局、编译器都与较优手写循环具有相同指令数。
最初逐线实现的 Clang 18/GCC 12 六组 baseline `-O3 -ffp-contract=off` 历史探针中，
公开 kernel 内无残留函数调用或分配调用；动态一维残差与静态 2×3 整数探针的
静态指令数分别与对应手写循环相同。泛步长、单轴和列主序探针仍有布局相关差异。
MSVC 19.42 仍保留部分绑定/遍历 helper 调用和较多 setup 指令。
静态生成码审查只说明这些探针的代码形态，不代表普遍吞吐提升。
当前四列路径的 Linux 实际 API 基准、生成码与剩余差距见
[列规约成本记录](../performance/np-expression-columns.md)。

完整例子见 [expression_reduce.cpp](../../examples/np/expression_reduce.cpp)：
只物化每行均值的小 workspace，直接融合计算平方残差的全量和及逐行和。
