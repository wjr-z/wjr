# 多秩选择与批量 quantile

这里实现数值数组的通用选择原语，不包含分组、时间索引或领域规则。

## partition_at

`<wjr/np/partition.hpp>` 的 `partition_at(values, ranks)` 在一维连续可写 view 上原地定位多个秩。
ranks 是调用方提供的非递减整数 span，允许重复，不会被修改；每个秩均在 `[0,N)`。
例如 `{2, 5, 8}` 只要求这三个位置具有完整排序后的值，不要求其他位置有序。
每个已选位置的左侧均不大于该位置，右侧均不小于该位置。

默认 `partition_method::nth_element` 选择中间请求的秩，用标准库 nth_element 定位，
再分治仍含请求的两侧子区间；相邻端点秩改用 min/max_element，一样复用已经完成的数据划分。
递归按请求数平衡，栈深 O(log(K+1))。单秩平均线性的假设下，约需 O(N log(K+1)) 选择工作。

`partition_at<partition_method::shared_pivot>(values, ranks)` 显式选择共享值 pivot 策略。
以下的三路划分、预算和小区间阈值仅属于该策略，不向默认路径增加状态或检查。

算法从首/中/末三个值选 pivot，用两次标准库 partition 形成小于、等价、大于三个区间。
仅检查有序请求的首尾，即可 O(1) 判定整批都在一侧并整体收缩，不扫描或重新排序请求。
跨侧时才二分分流请求；等价值区间一次满足其中所有秩。
递归请求较少的一侧、循环处理另一侧，递归栈 O(log(K+1))。
第一次划分后全在左侧时，直接跳过等价值区间的第二次划分。
私有具名常量 partition_sort_cutoff（初值 16）控制小区间直接排序；
划分预算由 partition_depth_factor（初值 2）乘初始长度 bit_width 决定，耗尽则排序当前区间，
避免自定义划分连续退化。只有单一目标秩（含重复请求）时复用 nth_element；
该目标恰在子区间首/末位置则使用 min_element/max_element 后交换到位。
不依赖数据值均匀，不复制 GCC 私有实现。标准库单秩选择的复杂度仍沿用具体标准库保证。
在通常的划分平衡和单秩平均线性假设下，选择工作约 O(N log(K+1))；
另有请求分流工作。两次 partition 的额外扫描不一定优于 nth_element 分治，需对照实际分布。
大量请求时完整排序可能更合适，本版不增加按请求密度选择算法的运行时调度。

浮点比较把所有 NaN 放在非 NaN 之后，正负零相等；不保证稳定顺序或相等值的位模式选择。
布尔数组使用真实 bool 存储。秩支持不同整数位宽，不接受 bool/volatile。
源及秩存储有效、不交；源可形成长度可表示为 ptrdiff_t 的标准库随机访问范围。
空请求不访问数据，包括 nullptr 的空源。不分配数据或请求工作区。
秩的顺序及范围是调用方前置条件，仅由既有 L2 断言核对。

## quantile

`<wjr/np/quantile.hpp>` 首版实现一维线性分位数，概率在 `[0,1]`，可以乱序或重复。
输出与概率具有相同形状，输出顺序保持原查询顺序。

```cpp
np::quantile_inplace_to(dst, values, probabilities, rank_workspace);
np::quantile_to(dst, source, probabilities, data_workspace, rank_workspace);
np::quantile_to<np::quantile_nan_policy::omit>(
    dst, source, probabilities, data_workspace, rank_workspace);
np::quantile_inplace_to<np::quantile_nan_policy::propagate, np::partition_method::shared_pivot>(
    dst, values, probabilities, rank_workspace);
```

- 原地入口允许重排连续源，避免数据副本；输出与源、概率不交，源与概率也不交。
- 保留源的入口把数据写到调用方提供的 N 个同 dtype 活对象，支持连续/正 stride/padded 源。
  浮点缺失值处理与复制合并，不额外建立过滤掩码或预清零工作区。
- 两者使用调用方提供的至少 2Q 个活 size_t 作为秩工作区，均不隐藏分配。
  空输出直接返回；空源、全部缺失或传播 NaN 时，不使用秩工作区。
- 数据工作区与所有视图、秩工作区不交；秩工作区与所有视图不交。输入及概率在调用中保持稳定。
- 概率和输出支持正 stride/padded 布局，只更新输出逻辑元素，保留孔洞。

quantile 同样默认使用 nth_element 分治，第二个模板参数可显式指定 shared_pivot。
每个概率生成 `h=q*(N-1)`，选择 floor(h) 及有需要的下一秩，再对两个值插值。
所有所需秩一起排序并执行多秩选择，不对每个概率从完整源重新开始选择。
端点和整数位置不额外要求下一秩；浮点舍入到末端时钳到最后合法位置。
源 dtype 在选择中保持不变，避免先转 double 导致大整数比较次序丢失。
插值计算类型为源、概率、输出及 double 的 common_type，只有选中端点参与转换。
必要数值转换须可表示；有限端点复用 C++20 std::lerp 避免直接相减造成不必要溢出。

NaN policy 默认为 propagate：源含 NaN 则所有输出为 NaN；omit 只使用非 NaN 的有效长度。
空源及全缺失均输出 NaN。原地 omit 会压紧非 NaN 前缀，尾部内容不保证保留原始多重集合。
单个无穷端点支配内部插值；两个同号无穷返回该无穷，相反无穷的内部插值返回 NaN。
端点概率直接返回所选端点。这是明确的扩展实数合同，不承诺逐位复现 NumPy 对无穷插值的结果。
quiet NaN 为约定输入，不支持假设 NaN 不存在的 fast-math 模式。

除数据选择外，秩构建/排序约 O(Q log(Q+1))，保留源还需要一次 O(N) 复制。
这些成本和调用方工作区均计入完整算法的比较，不把无隐藏分配等同于无内存成本。
本版没有多轴、加权概率、其余 quantile method、自动多线程或选择计划对象。

## 验证与性能基线

测试对照完整排序后的选中值、左右划分及数据多重集合；覆盖重复秩、升/降序、相等值、NaN/Inf、
空输入、真实 bool、大整数、乱序概率、插值边界、原输入保留和 stride 输出孔洞。
示例见 [quantile.cpp](../../examples/np/quantile.cpp)。

NP benchmark 的 partition_copy_kernel 比较默认/显式共享 pivot、平衡请求 nth_element 手写分治、
升序目标在缩短后缀逐次 nth_element、完整排序，所有方案均包含从相同源恢复数据的成本。
包含单个/多个秩、均匀/中部聚集/低端聚集/高端聚集请求和小/大源。
quantile_copy_kernel 对比 int64/double 的整个保留源 quantile 与复制+完整排序+插值，包含秩整理的成本。
性能结论需注明平台；没有因 Windows 局部结果改变已有标量入口或编译选项。

shared_pivot 策略初步数据：Windows x64、i5-13500HX、Clang 18.1.8、C++20、Release baseline，
每项 3 次重复、随机交错，中位耗时如下（均包含复制，单位 ms）：

| 有限值数据集，N=65536 | shared_pivot | 对照 |
| --- | ---: | ---: |
| 257 个分散秩 | 2.08 | 平衡 nth_element 分治 2.46；升序缩短后缀 6.93；全排序 3.38 |
| double，3 个 quantile | 0.71 | 全排序后插值 3.84 |
| double，31 个 quantile | 1.38 | 全排序后插值 3.75 |
| int64，3 个 quantile | 0.62 | 全排序后插值 3.23 |

Linux/SysV 的补充验证使用 ubuntu-24.04 GitHub runner、GNU 13.3、C++20/native，
[同二进制对照](https://github.com/wjr-z/wjr/actions/runs/40086837646)中，共享 pivot 相比手写平衡 nth 分治，
在 65536 项源、3/31/257 个分散或聚集请求上慢约 5–31%。该结果决定默认保留 nth 分治，
不因 Windows 收益改变 Linux 优先的默认选择。两种策略均可显式复现，不增加分布识别调度。

小输入中直接排序仍可更快；NaN 数据集性能未测量。
未采集硬件分支/缓存计数，不据此宣称普遍加速或整个接口无成本。
