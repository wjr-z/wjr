# 单轴排名

`<wjr/np/rank.hpp>` 提供单线程 `rankdata_to`，用于一般数值数组的排序排名。
不包含金融语义、线程调度、运行时表达式图或 Python 绑定。

```cpp
np::rankdata_to(output, input, workspace); // 一维，默认 average / propagate
np::rankdata_to<np::rank_method::ordinal>(output, input, workspace, np::axes<1>);
np::rankdata_to<np::rank_method::average, np::rank_nan_policy::omit>(
    output, input, workspace, np::axes<1>);
```

## 语义

排名从 1 开始，按输入元素原类型升序比较，不先转换为浮点数。
例如相邻的两个大于 2^53 的整数仍有不同次序。
输入支持现有算术元素类型；输出为可写浮点 view。
整数输入通常使用 double 输出；输出精度不足时，排名按正常浮点转换舍入。

| method | 相等元素的排名 |
| --- | --- |
| average | 占据名次的平均值，默认 |
| min | 占据名次的最小值 |
| max | 占据名次的最大值 |
| dense | 同值同名次，下一个不同值只增加 1 |
| ordinal | 所有名次不同，同值按轴内原索引顺序排列 |

NaN 默认使所在整条轴线的输出为 NaN；`omit` 忽略 NaN，原位置仍输出 NaN。
正负无穷正常参与排序；正负零视为相等。
这些规则参考 [SciPy rankdata](https://docs.scipy.org/doc/scipy/reference/generated/scipy.stats.rankdata.html)，
不提供其 raise policy、动态轴、隐式 flatten 或自动分配结果的入口。
不支持假定 NaN 不存在的 fast-math 编译模式。

## 视图与存储

沿用现有 Kokkos mdspan alias、固定 rank 和正 stride mapping。
输入和输出 shape 相同，支持 layout_left/right、合法 layout_stride 和现有 padded layout。
逐轴线排名，padding 不读写；不改变 view 的表示、所有权或生命周期。

前置条件是合法 view、输入输出逻辑存储互不重叠，且工作区与两者存储不重叠。
工作区是 `std::span<size_t>`，至少有 `input.extent(Axis)` 个已开始生命周期的对象；
仅使用前面这些对象，同一工作区依次复用于所有轴线和后续调用。
空输入不访问任何指针，也不要求非空工作区。
shape 与工作区大小沿用 L2 断言；不添加常驻校验状态或发布模式的检查路径。

WJR 不分配临时容器；以调用方的索引工作区调用 `std::sort`。
每条长度 n 的轴线使用 O(n) 工作区及 O(n log n) 最坏比较次数，
另有标准排序实现自身的调用栈成本。ordinal 通过同值时比较原索引保证顺序，
不使用可能分配缓冲区的 stable_sort。

## 性能边界

NaN 在索引准备时处理，比较器只读取原值并比较；无需复制整份输入。
连续内轴的步长以编译期常量传递，避免排序比较器保留通用 stride 元数据。
其余布局使用必要的 stride 寻址。method 与 NaN policy 在编译期分发。
没有默认初始化输出再覆盖的步骤，也没有每条线单独申请工作区。

`benchmarks` 的 np profile 增加 `rank_average<false/true>`，对比相同 NaN、并列值、
输出与工作区语义的手写实现。覆盖 0/1/16/300/4096 个元素，及随机、升序、降序、
全相等、多并列五种分布；每个 case 先核对结果。
它衡量重复执行内核，不包含输入转换、分配、Python 调用或完整研究报告成本。
两条路径使用相同的非内联调用边界，基线同样缓存并列组的首值。
排序仍使用平台标准库；本接口不承诺优于成熟排序实现或所有形状下都无退化。

见 [示例](../../examples/np/rank.cpp) 与 [针对性测试](../../test/np/rank.cpp)。
