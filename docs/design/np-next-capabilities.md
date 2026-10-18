# NP 下一阶段能力与执行边界

2026-10-18 的一次 Astra 集中审视核对了 `0565e732`（#110）、现有核心标准、
xtensor/WG21 参考和下一阶段的依赖接口。本文保存选定合同和实施分组；
代码审查、正确性测试、生成码和计时是不同证据，设计审视没有替代后面三项。
基线已有能力见[核心标准](np-core-standard.md)，不再重复建设。

## 实施分组

这是一组可独立交付的能力，不是必须建立全部机制的清单。
A/B/F/G 为本轮选择；[表达式 pairwise](tensor-expression-pairwise.md)已随后合入 #111。
C 是下一阶段互操作方向，
D/E/H 保留用途或实测准入。每组包含对应测试、用法文档和有展示价值的例子。

| 组 | 依赖与状态 | 交付能力 | 验收重点 |
| --- | --- | --- | --- |
| A | 独立，本轮 | 默认关闭的 `wjr::tbb` 同步分块执行 | 借用 callable、异常与继承取消、直接 oneTBB 对照、安装消费 |
| B | 现有表达式绑定，本轮 | 表达式全量 min/max，view/表达式单轴 min_to/max_to | 广播/stride、首个 NaN/相等值、空轴、逐线提交、不分配 |
| C | 外部数据需求，下一阶段 | 受限 CPU DLPack 借用与独占 release lease | 固定 rank/精确 dtype/正步长，普通只读 view，释放一次 |
| D | 现有 xsimd，按用途准入 | 显式 relaxed SIMD expression sum | 仅放宽累计加法分组，保留 child dtype、转换与 fallback |
| E | A 加实际批处理证据 | 显式并行独立输出线的 axis sum/mean | 每线仍按原顺序；一次绑定；部分写入和取消合同 |
| F | 现有 np_cblas，本轮 | GEMV 薄适配 | LP64、order/transpose/pitch/increment、beta 零不读不预清零 |
| G | 现有 stable norm，本轮 | view 单轴 norm_l2_to | 复用每线稳定算法，极端量级/NaN+Inf/空轴/孔洞 |
| H | 已定位成本缺口，按实测准入 | 一次私有绑定表示简化 | 对象和绑定尺寸、编译成本、生成码与实际吞吐；无收益则不改 |

不为达到 MR 数量或行数拆分同一不变量，也不把已合入 MR 计入待审上限。
在少于六个待审 MR 时继续独立工作；用户负责合并。

## A：可选 oneTBB 同步边界

接口为 `<wjr/parallel/tbb.hpp>` 的
`wjr::tbb::parallel_for_chunks(first, last, grain, const F&)`，索引类型为 `size_t`。
实现使用一个 `blocked_range` 和 `parallel_for`，小型任务对象只借用真实 F，
支持可移动但不可复制的 const-callable。回调接收 `[begin,end)`，返回值被丢弃。
`first <= last`、`grain > 0` 是入口断言级前提，空范围不调用回调。

不提供 WJR 线程池、scheduler 对象、每次调用创建的 arena、全局线程配置或异步结果。
调用方可创建并复用 `oneapi::tbb::task_arena`；F 的捕获对象寿命、同步和数据竞争由调用方负责。
同步借用保证本次回调在函数返回或异常传播前结束，不保证继承上层取消后全部索引已执行。
异常和取消沿用 provider；已写结果不回滚，不承诺失败索引顺序。

依赖构建成本是明确约束：`WJR_ENABLE_TBB` 默认 OFF，普通配置不搜索 TBB，
不 FetchContent、不自动下载、不源码编译。显式开启时发现系统或预编译
`TBBConfig`，使用 `TBB::tbb`；安装消费时仅所请求组件重新发现依赖。
不把 TBB 链接或 native 选项传给普通 `wjr`/`wjr::np`。
Linux 在一个现有合适 job 安装 `libtbb-dev`，不扩充 Windows 的两项矩阵。

已审接口基线是 [oneTBB 2021.11.0](https://github.com/uxlfoundation/oneTBB/tree/8b829acc65569019edb896c5150d427f288e8aba)，
最低包版本采用 2021.11，非 EXACT；使用者选择的实际版本需记录。
适配层的成本对照是同合同直接 oneTBB 调用，不能相对串行循环声称调度零成本。
核对 callable 复制、任务布局、setup/额外调用、冷启动和重复使用；指令数不代替吞吐。

## B/G：轴向数值能力

表达式全量 min/max 按自然广播域的逻辑顺序折叠，返回表达式值类型。
单轴 `min_to/max_to(dst, source, axes<I>[,keepdims])` 保留首个相等值，
包括 signed zero，并沿用已有 view extrema 的首个 NaN 规则。
空输出不读 source；非空输出的规约轴不能为空，这是可信输入前提。
可抛 source 完成一条线后才写该输出；不保证首 NaN 后仍调用余下映射。
首版只返回值，不同时加入 arg-extrema、SIMD 或并行极值。

`norm_l2_to(dst, view, axes<I>[,keepdims])` 使用已有稳定 kernel 逐线计算，
保留浮点 accumulator 及有限输入转换可表示的前提。空线返回正零；
输出和输入不交，padding 不属于逻辑输入输出。例子用预分配结果完成行/列归一化。

现有 Blue norm 在桶累积溢出时会以 scaled 算法重新读取 source。
这适用于计算期间稳定的 view，不能直接替换成表达式 reader 后声称每个元素只求值一次。
表达式 norm 需要另行选择重读许可、真正单遍状态或显式物化，不混入本组。

## F：显式成熟内核与工作区

`cblas::gemv_to` 延伸现有 LP64 边界，而不自行实现 GEMV 或自动规划。
输入矩阵保留物理 order/pitch，向量保留正 increment，转置改变数学维度。
输出与两个输入不交；不搜索别名，不隐式复制。
beta 零不读取也不预清零输出，零内维或 alpha 零显式只计算 beta*y。
空输出在 stride 访问与 LP64 窄化前返回。实际 provider 数值和安装消费验证不可由 recording stub 替代。

流水线以借用数据、静态表达式或成熟 eager 内核、调用方 workspace 为接缝。
重复使用的 exp、投影等中间结果可以显式物化；lazy fusion 不天然优于成熟专门内核。
不增加自动 epilogue、分配型 planner 或修改 BLAS 全局线程数。

## C：后续 CPU 互操作边界

候选为 [DLPack v1.3 的固定源码](https://github.com/dmlc/dlpack/blob/84d107bf416c6bab9ae68ad285876600d230490d/include/dlpack/dlpack.h)，
不是完整 Python 协议或新 exchange API。先支持 CPU、固定 rank、精确 dtype、
正且不重叠 stride 的只读借用，校验外部元数据后返回普通 view。
独占 lease 仅在互操作边界保留生产者及其 release，不给普通 view 增加引用计数或状态。

需区分借用与接管、只读标志、版本、字节 offset、空域和失败时释放。
1.2 及以后的非零 rank tensor 不能用空 strides；早期版本的空 strides 才表示连续布局。
DLPack 不给出 allocation 长度，适配器不能证明任意指针有效。
不默认接纳负/重叠/广播 stride、设备、低位 dtype 或自动所有权导出。

## D/E/H 的准入及延期

relaxed SIMD sum 使用独立显式许可，不改变默认 ordered sum 或已有 pairwise 树。
许可只影响 accumulator 加法分组，不允许重排 child、改变整数运算/转换、
启用全局 fast-math 或暗中改变 FMA。对照同语义手写 SIMD 及适用的成熟内核，
覆盖短/tail/混合长度、抵消和量级差异。

NP 并行便利接口只能在已证明并行安全的子集上调度独立输出线。
noexcept 不等于纯性或并行安全；未知 map、math 副作用不能据此获准。
先写入调用方已有输出，不提供取消后可能留下未写对象的 owning parallel eval。
单条 ordered fold、pairwise 树与串行默认保持不变。

绑定优化先定位尺寸、状态加载、编译时间或生成码的实际问题，再选一个私有改法。
例如省略 scalar-only stride 状态或静态索引前缀；不创建公共 plan、缓存或 DAG。
本轮 sum/mean restrict 实验没有改善实际公开入口，改为非内联又新增真实调用，
因此保留原实现，不把无收益重构当成优化交付。

通用 scheduler、自动 lazy/eager planner、exact-storage 替换、signed stride、
Arrow、运行时图、设备执行和 OpenXLA lowering 仍有独立需求门槛。
编译期 C++ 表达式不自动成为可导出的运行时图。
