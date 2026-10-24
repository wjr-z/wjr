# 分段规约

`<wjr/np/segmented_reduce.hpp>` 在一维借用 view 上处理长度不同的连续段。
仅使用输入、输出和调用方偏移数组，不引入分组容器、索引所有权、线程调度或领域类型。

```cpp
const uint32_t offsets[] = {0, 2, 2, 6};
np::segmented_sum_to(dst, src, std::span(offsets));
np::segmented_mean_to(dst, src, std::span(offsets));
np::segmented_reduce_to(dst, src, std::span(offsets), 1., std::multiplies<>{});
```

第 g 个输出对应 `[offsets[g], offsets[g+1])`，允许相等边界表示空段。
边界可以覆盖输入的一个子区间，不要求从 0 开始或覆盖整个输入。
偏移可使用 32/64 位有符号或无符号整数，以避免强制转换/复制已有紧凑索引；不支持 bool 或 volatile。
参数是 span，可写/只读、固定/动态 extent 都可；函数始终只读边界。

## 通用入口

`segmented_reduce_to(dst, src, offsets, initial, operation)` 对每段从 initial 开始做左折叠。
段间和段内均按索引递增顺序执行，空段不调用 operation，直接写 initial。
累加器是算术类型，输入值以原类型传给 operation；返回值再赋给累加器。
这允许求和、乘积、最值、逻辑规约等数值操作，不创建可增长的聚合状态。

operation 通过引用借用，在所有段之间使用同一对象，不复制、不移动、不保留到返回后。
支持有状态或不可复制的 callable；不从 noexcept 推断纯度，不重排或合并调用。
noexcept 同时考虑调用与返回值赋给累加器时的转换。
若抛异常，已经完成的输出段保留，当前段尚未写回；不新增回滚状态或保护对象。
调用方应保证 operation 不修改输入、边界或干扰输出存储。

## 便捷入口

| 函数 | 空段 | NaN |
| --- | --- | --- |
| segmented_sum_to | 0 | 传播 |
| segmented_nansum_to | 0 | 跳过，全部缺失也返回 0 |
| segmented_mean_to | NaN | 传播 |
| segmented_nanmean_to | NaN | 跳过，按有效数归一化，全部缺失返回 NaN |

便捷入口沿用现有 sum/mean 的类型规则和 ordered 累加语义，允许显式指定累加类型。
sum 在每次相加前把输入转换到累加器；不引入树形分组或 reassociation。
mean 要求浮点输出/累加器，整数和 bool 输入默认使用 double。
NaN 入口对整数输入在编译期去掉缺失值判断；只有 nanmean 为有效元素维护计数。
无穷不作为缺失值；不支持假定 NaN 不存在的 fast-math 模式。

## 前置条件与成本

输入和输出均为合法一维连续、正 stride 或现有 padded view。
输出有 G 项且 G>0 时，边界恰有 G+1 项，非负、非递减且均不大于输入长度。
G=0 时不访问输入、输出或边界元素，边界可以为空。
输出逻辑存储必须与输入、边界不重叠；输入与边界在调用期间保持稳定。
算术溢出、不可表示的必要转换与无效存储由调用方避免。
仅在既有 L2 断言级别核对边界范围和个数，不增加发布模式逐元素检查。

每个段仅写一次输出，不预清零，不分配工作区或复制/排序分组信息。
时间复杂度 O(G+M)，M 为所选区间内输入元素数；额外状态 O(1)。
不为浮点 sum 自动使用多个累加器改变顺序，也不引入默认多线程执行。

np benchmark profile 的 `segmented_sum_kernel` 分别测手写、便捷 sum、通用 fold。
它们使用相同非内联调用边界与 ordered 累加；覆盖空输出、空段、短/长段及 stride=2，
先核对结果与输出孔洞，再测重复执行。它不包含分配、排序、上层键到分组的映射或 Python 调用。
参见[示例](../../examples/np/segmented_reduce.cpp)与[测试](../../test/np/segmented_reduce.cpp)。
