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

## 显式交错批量入口

`searchsorted_batch_to<search_side::left/right>(dst, sorted, queries)` 接受一维查询和输出，
源、查询及输出分别支持连续、正 stride 和现有 padded 布局。元素、NaN、输出范围、别名和排序合同与
`searchsorted_to` 一致；多维查询可以先取行或合法 reshape，再显式调用。原入口及其默认策略保持不变。

```cpp
np::searchsorted_batch_to(dst, sorted, queries);
np::searchsorted_batch_to<np::search_side::right>(dst, sorted, queries);
```

实现把查询分成固定的小块，在块内按二分层次交错执行。每块共享区间长度和第一次读取的 pivot，
随后各查询的独立比较可供处理器并行调度。区间采用重叠的左右半区，统一缩短为原长度的一半向上取整，
长度为 1 时再做最后一次比较。它仍是 O(Q log(N+2))，没有声称减少所有后续层的比较次数。
查询不重排、不构建索引；每个查询只读取一次，最终位置只写回一次。

私有内核保留 `Block` 模板接缝，当前完整块宽度为 8，尾部独立执行标量查询。
每块的必要状态为 `Block * (sizeof(size_t) + sizeof(T))` 字节的 base 和查询值，
不做值初始化、不使用堆分配或引用计数。这是固定大小的局部对象；是否全部进入寄存器由编译器决定，
并非承诺无栈访问。较大的块可能增加寄存器压力及代码体积，宽度暂不作为公共 API。

设计参考了 [NumPy 数值 binsearch 的层次交错机制](https://github.com/numpy/numpy/blob/main/numpy/_core/src/npysort/binsearch.cpp)
（审阅日期 2026-10-24）。NumPy 当前实现将全批 base 暂存在输出，每层读写输出并重读查询，
因此只需 O(1) 局部状态；本实现以固定块状态换取一次查询读取和一次结果写入。
两者均保留在基准中，不能仅凭“批量”或“少写输出”断言更快。

基准同时保留标准二分、原独立入口、全批层次方案和固定宽度 4/8/32 的手写基线，
覆盖源长度 0/16/1024/65536、查询数 1/32/1024、随机/重复/升序查询及连续/stride=2 数据。
所有方案使用同一调用边界，核对完整结果及输出孔洞，分配和夹具构造不计时。
目前固定块优化的性能证据来自 Windows x64 Clang 18.1.8 baseline；Linux GCC/Clang 尚待测量，
宽度 8 不代表各平台的最优值，也不据此把原入口全局切换到批量路径。

同机同二进制、三次随机交错重复的 real-time 中位数示例，N=65536、Q=1024、int64、left：

| 数据 | 原独立二分 | 固定块入口 | min/max 共享前缀实验 |
| --- | ---: | ---: | ---: |
| 连续、随机查询 | 28.83 μs | 8.03 μs | 7.82 μs |
| stride=2、随机查询 | 66.30 μs | 8.32 μs | 8.71 μs |
| 连续、重复查询 | 16.06 μs | 5.41 μs | 1.74 μs |
| stride=2、重复查询 | 10.40 μs | 7.04 μs | 2.18 μs |

另一组相同调用边界的手写宽度 8 对照中，随机连续/stride 查询分别为 9.50/9.05 μs，
库入口为 7.99/8.59 μs。两者有不同的编译器循环展开和寄存器分配，不能将此结果解释为抽象有“负开销”。
生成码中私有 helper/view/callback 调用已内联，但仍存在寄存器溢出到栈；未采集硬件分支及缓存计数器。

基准还包含块 min/max 共享前缀的实验：先用 O(Block) 比较求查询边界，随后若两个端点均选择同一侧，
只更新共同 base 和长度；出现分歧后才进入逐查询状态。这能减少重复或局部集中的查询工作，
但随机查询仍支付边界归约和分歧判断。尚未将其加入生产路径，也没有忽略求边界的成本。
