# 有序数组批量查找

`<wjr/np/searchsorted.hpp>` 提供 `searchsorted_to<search_side::left/right>(dst, sorted, queries)`。
sorted 是一维升序借用 view，查询和输出可以是相同形状的任意固定 rank，包括标量。
输出由调用方提供，不分配或保留索引、排序副本和临时数组。

```cpp
np::searchsorted_to(dst, sorted, queries); // 第一个相等位置
np::searchsorted_to<np::search_side::right>(dst, sorted, queries); // 最后一个相等值之后
```

没有相等值时，两者均返回维持升序的插入位置，范围为 `[0, sorted.extent(0)]`。
重复值、无穷及布尔数据均可；正负零相等。实数 NaN 统一排在非 NaN 之后；
NaN 查询的 left 返回第一个 NaN 的位置，right 返回数组长度。
这与 [NumPy searchsorted](https://numpy.org/doc/stable/reference/generated/numpy.searchsorted.html)
及其[实数排序约定](https://numpy.org/doc/stable/reference/generated/numpy.sort.html)一致，
但此接口没有 sorter 参数、自动排序或隐式 dtype 统一。

## 类型与存储

- sorted 和 queries 的去 cv 元素类型相同，均为非 volatile 算术类型，避免混合整数符号/位宽的隐式比较转换。
- 输出为可写、非 volatile 且非 bool 的整数类型。非空输出要求源长度可表示为该类型；连续源也须能形成标准库随机访问范围，长度可表示为 ptrdiff_t。
- 支持现有连续、正 stride 和 padded 布局；输出只写逻辑元素，保留孔洞。
- 输入和查询在调用中保持稳定，输出与两者的逻辑存储不交。不同查询不共享写入位置。
- 调用方保证源已排序；不扫描源核验。形状和必要输出范围仅使用既有 L2 断言。
- 空输出不访问任何元素。空源对每个查询返回 0；允许空源的 data handle 为 nullptr。

## 成本边界

每个查询做独立二分查找，时间 O(Q log(N+2))，除固定 rank 的遍历元数据外额外状态 O(1)。
连续源复用标准库 lower_bound、upper_bound 或 partition_point；非连续源保留相同判定的索引二分，避免构建自定义迭代器层。
批量入口复用现有 apply_kernel 遍历与布局分发，不添加公共 plan、结果容器或每元素形状校验。
浮点查询的 NaN 分类在二分循环外；普通浮点 left/right 比较本身即可正确处理源尾部 NaN，
不在每一轮重复分类。quiet NaN 为约定输入；不支持假定 NaN 不存在的 fast-math。

本实现不声称独立二分是所有批量查询的最优解。已排序查询可能受益于归并式扫描，
大批随机查询可能受益于交错查找；这些策略有不同的工作量、缓存和分支行为，待实际需求与测量后扩展。
不假定分支消除必然更快，也不依据单一重复查询的预测命中率选择全局路径。

NP benchmark profile 对照同调用边界的 `std::lower_bound/upper_bound`，
包含空源、小/大源、重复键和随机键。夹具构建、分配和结果核对均不计时。
该基准只覆盖连续 int64 数据；不据此推断浮点和任意 stride 的性能。

初步验证环境为 Windows x64、i5-13500HX、Clang 18.1.8、C++20、Release baseline。
检查生成码：连续整数路径消除了 view、回调和私有 helper 调用，核心查找循环与标准库基线相同。
但普通构建中，长度 16、256 个随机查询的 left 个案仍稳定慢约 17–20%；
仅在临时实验中统一代码块对齐后，差距降至约 1%。这与代码布局敏感的现象相符，
不等于已证明唯一硬件原因，也不证明所有环境零开销；没有把实验对齐选项加入库或 CI。
未采集硬件分支/缓存计数，Linux 性能仍待验证。这里保留复现用基准，不按该 Windows 个案增加特化策略。

参见[示例](../../examples/np/searchsorted.cpp)与[测试](../../test/np/searchsorted.cpp)。
