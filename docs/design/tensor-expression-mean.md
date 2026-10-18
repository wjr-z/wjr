# 表达式 mean

`<wjr/np/expression_mean.hpp>` 提供两个立即执行入口，`E` 是现有值闭包表达式：

```cpp
double total = np::mean(a * a);
np::mean_to(row_means, a * a, np::axes<1>);
np::mean_to(row_means_keepdims, a * a, np::axes<1>, np::keepdims);
```

`mean` 返回全量标量结果；`mean_to` 将单个编译期轴的均值写入调用方提供的浮点 view，
可选保留被规约轴并将其 extent 设为 1。输出 shape 必须与表达式自然广播 shape 删除/保留该轴后相同。
这两个入口都不分配中间 tensor。现有 [view mean](tensor-mean.md) 保持原接口及行为；表达式 variance 不在本次范围。

```cpp
const auto a = np::as_expr(input_view);
const auto mean_square = np::mean(a * a);
const auto bounded = np::mean(np::map([](double x) noexcept {
    return x / (1 + x * x);
}, a));
```

形状、广播、布局和借用遵循 [terminal sum](tensor-expression-reductions.md)：
规约域是表达式的自然广播 shape，按逻辑行主序从正零累计。
正 stride、padded、mixed layout、scalar 与 rank 0 复用已有绑定和遍历。
动态 shape 兼容性和逻辑 size/offset 可表示性仍是已有前置条件；
没有额外的 checked 重载、错误对象或持久校验状态。

`mean_to` 的每个输出独立从正零开始，按所选轴递增执行有序加法，再除以该轴长度一次。
输出为空时不读取表达式；轴长度为零而输出非空时，每个输出写入 accumulator 类型的 quiet NaN，
不读取叶子或调用 map。可抛表达式按 line 完成后提交：已完成 line 保留，失败 line 未写入。

默认累计和返回类型与 view mean 相同：整数/bool 表达式使用 double，
浮点表达式保留其结果类型。显式 Acc 必须是非 cv 浮点类型。
`mean_to` 的目标元素也必须是非 cv 浮点类型；目标类型 D 可与 Acc 不同，
只在每条 line 完成累计和除法后转换存储。
每个节点先按自身的 C++ dtype 执行，结果才转成 Acc；
例如 `mean<double>(integer_a / integer_b)` 保留整数除法，不提前将子节点改为 double。
节点和累计的溢出/转换前置条件仍须满足。

每个结果从正零执行 ordered sum，最后除以转换到 Acc 的逻辑元素数一次。
不将除法改成逐点缩放，不改变累计分组，也不使用 pairwise、补偿或 relaxed SIMD。
累计本身可能溢出，即使数学均值可表示；显式更宽 Acc 可改善部分情况，但这不是稳定均值算法。
浮点数值、NaN/Inf、signed zero 与非默认编译/运行环境的限制与 view mean 一致。

`mean` 空域在绑定数据、读取元素、调用 operation 和除法前返回 quiet NaN，支持合法的 null 空 view。
rank 0 的一个元素同样从正零累加后除以 1。
入口 constexpr，noexcept 随表达式读取；可抛 map 正常向外传播，AST 与输入仍由调用方持有。
`mean` 的全量结果只在函数正常返回后可用，没有部分输出，也没有额外的异常回滚状态。

## 绑定与成本

原 terminal sum 的非空求值体被机械提取为私有、强内联的 `sum_nonempty_expression`，
接收已绑定域的 const 引用，不复制 domain、不再次验证空域或 shape。
公开 sum 和 mean 各只建立一次自然域，再各绑定一次数据/函数槽；
mean 不在解析自身形状后再次调用公开 sum。
packed 快路径和泛 stride walker 均保留，math/map 的原 dtype 计算位置不变。

`mean_to` 调用 expression axis sum 的同一绑定和 walker，在每条 line 的 accumulator 计算完成后除一次，
然后立即写出该 line；不先调用 `sum_to` 再遍历缩放输出。rank-2、`axes<0>` 仍可进入已有四列交错，
只对每列的最终累计值做一次除法。目标逻辑元素必须与所有表达式输入叶子不交，
沿用 `sum_to` 的别名前置条件。sum 的模板实例化通过 `Mean=false` 编译期保留原来的直接写出分支。

不分配中间 payload，不复制或移动求值中的 callable；callable 自身的执行成本由其实现决定。
每个逻辑坐标按 AST 求值，没有额外的前置计算或第二次元素扫描。
原树里重复出现的子树仍可能重复求值，不提供 CSE 或 lazy reducer 缓存。
稀疏/广播 metadata 的 setup 成本仍然存在，融合并不保证优于显式物化或最佳手写循环。

## 验证

13 项全量及单轴 mean GTest在 Clang 18 O3/ASan/debug 3、MSVC 19.42、GCC 12 正常和
`-fno-exceptions` 下通过；包括 axis/keepdims shape 约束、默认与显式 Acc、ordered cancellation、
stride/padding、空轴、空输出、rank-1 输出和可抛 map 完整 line 提交。另有原有 expression sum 回归。
覆盖 constexpr、dtype、原整数除法、显式 Acc、消去敏感顺序、
动态右对齐广播、padding/stride、rank 0、NaN/Inf/zero、空域跳过调用、重复子树、
move-only callable、求值无 callable copy/move 和可抛 map。
GCC 12/Clang 18 的独立程序另通过 118 组实际 API/同合同手写对照及一项异常检查，含分配计数。

Windows x64 `-O3 -march=x86-64 -ffp-contract=off` 的六组旧 sum 探针覆盖 packed、stride、
broadcast、map、static 和 mixed integer/float。提取前后 Clang 的规范化指令完全相同；
GCC 的指令数全部相同，broadcast 仅交换了两个独立的入口空域测试次序，其余五组指令也相同。
新 mean 的 paired probe 使用相同的逐节点 dtype、ordered 累计、空域和最终一次除法。
Clang/GCC 的静态六元素 mean 与手写入口指令数分别为 34/34、39/39；
GCC 动态 packed 探针仍有额外的入口组织和 unsigned count 转换代码（45/36），不在逐点循环内。
泛布局也可能保留 setup/helper 差异。

本次固定 3x4 的 axis probe 中，Clang/GCC 的旧 `sum_to(..., axes<1>)` 与本次修改前规范化指令序列
完全相同，分别 41/120 条。`axes<0>` mean 对同序手写四列 interleaved loop，Clang 都为 29 条，
GCC 都为 34 条且序列相同；Clang 调度不同。`axes<1>` WJR 路径为 Clang/GCC 45/129 条，
对照手写 noalias loop 为 41/52 条；GCC WJR 探针保留了输入输出范围判断及 overlap-safe 路径，
而对照显式编码了接口已有的不相交前置条件。该差异是进一步检查代码形态的线索，不能单靠静态计数
推导吞吐。以上均为 Windows x64 工具链证据；没有 Linux/SysV 计时，不宣称普遍零开销或 lazy 更快。

可运行的 [expression_mean.cpp](../../examples/np/expression_mean.cpp) 演示平方均值和非线性纯 map；
它独立消费安装后的 `wjr::np`，不打印或物化中间数组。
