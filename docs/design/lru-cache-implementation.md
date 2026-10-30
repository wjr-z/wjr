<!-- markdownlint-disable MD043 -->
<!-- The implementation plan uses capability and acceptance headings. -->

# LRU cache 后续实施与验收

状态：实施计划，2026-10-30；用户当前只要求设计，暂不实施。
所有阶段都是未来任务，不因合入本文自动授权新增代码、依赖或构建目标。
行为与异常合同以 [LRU 设计](lru-cache.md) 为准。

## L01：可用的单线程固定容量 cache

这是一个完整能力 MR，不把结构体、几个 API 或测试拆成独立交付。
先确认标准库组合与成熟候选的选择，再交付选定后端的最小公共接口、
完整失败语义、针对性测试和使用文档。只适配需要的功能，
不提供通用后端框架、不顺带实现 hash table，也不预留未使用的策略层。

拟议头文件位于 `include/wjr/container/lru_cache.hpp`，测试放在
`test/container/`，遵循已有容器的构建注册方式；正式路径由实现 MR 确认。
通用 C++20/baseline 头文件不得获得 ISA target 依赖。
本设计不提前声明新的 CMake alias、安装组件或可用 API。

首期必须把不可抛 Hash 域、容量 0、借用指针失效、固定容量和暂时 C+1
写入公共注释。检查普通 Key/T、不可复制但可原地构造的 T、
非默认构造 T、状态 Hash/Eq、计数 allocator、PMR 和高对齐 payload。
构造失败时由已完成的 RAII 成员回收，不能依赖尚未构造完成对象的析构函数。
检查容量加一溢出、不可实现的 bucket 请求和 reserve 失败。

allocator 必须负责真实 map pair 的构造与销毁。传递到 T 的 uses-allocator
协议需独立核对，不能假定 Entry 包装自动传播内层 allocator；
先明确支持范围，再用 PMR payload 的资源归属测试验收。
需要适配时只在 Entry 构造处处理，不复制整个 allocator 框架。

copy/move 默认删除；测试准确的 traits，证明实现没有额外声明 relocatable。
只在未来真实使用需要时另开 allocator-aware 移动能力，不为通过 trait 测试伪标属性。
transparent 查找只有在 C++20 Hash/Eq 约束及诊断均完整时才加入，
不借此承诺透明插入、转换后键等价或新的标准库设施。

## 正确性与失败注入

用简单顺序模型作为 oracle，按随机操作序列比较成员、值及 MRU→LRU 顺序。
测试中的观测入口可以检查内部不变量，但不为调试暴露公共可改节点或每次操作加检查。
完整覆盖以下同一事务，而不是只测返回值：

- 容量 0、1、2；重复键、重复 MRU 命中、miss、peek、erase、clear 和销毁。
- A/B/get A/C 序列；满容量失败后旧 victim 仍在，顺序也完全不变。
- 重复 try_emplace 不构造 T；容量 0 不构造 Key/T；既有键赋值成功才 promotion。
- 第 N 次分配、Key/T 构造、Eq 抛出，逐故障点检查资源平衡和后续可用性。
  赋值部分修改后抛出的 T 用例只验证其自身保证及 recency 不变。
- 构造阶段 reserve(C+1) 后，C+1 的临时插入不 rehash；victim iterator 有效。
  多种 C、Hash 碰撞和 STL 都要测，不靠某个实现的 bucket 增长偶然通过。
- 以稳定指针观察其他键插入、promotion 后地址不变；失效后不解引用。
  检查以同 cache 的键/值为参数时的合法 alias 用例和重复命中路径。
- clear/析构解除全部 hook，正确销毁高对齐对象；allocator 钩子、资源及存活数平衡。

Hash 抛出不在首期接受域，通过编译期拒绝用例验证，不给它编造回滚测试承诺。
失败注入/构造计数与时间测量分开，统计插桩不能混入吞吐结论。
按仓库规则运行格式 hook、最小相关 `tests` 构建与 ctest；
Linux GCC/Clang 优先，Windows 保持既有 MSVC/Clang 两项，验证 C++20。
沿用已有 ASan 检查 UAF、越界与清理；无需为这一容器扩 Windows 矩阵。

## 同契约性能验证

性能实验作为后续 L02，依赖 L01 正确性验收；本轮不添加 benchmark 文件。
使用仓库现有 Google Benchmark 及默认关闭的专项方式，
避免正常测试强制下载 Boost/Folly 或构建它们的无关示例。
比较用第三方来源固定版本/commit，记录许可证、配置和传递依赖。
若后续采用外部依赖，需单独评审启用方式、支持矩阵和禁用时无查找/链接的行为。

每个对手固定相同的 promotion、淘汰、容量 0、异常域、地址稳定性与构造语义。
Folly 的容量 0 行为不同，应单列或排除，不能混入等价排名。
最强可用候选包括标准 list+map、MultiIndex，以及可实际构建的 Folly；
构建失败/不适用要留证，不把缺席候选写成已被击败。

| 维度 | 必需负载 |
| --- | --- |
| 数据 | 整数小值、长 string Key、64B payload；公布实际长度/对齐 |
| 容量 | 64、4096、大容量；报告实际 bucket、负载率和工作集字节 |
| 命中 | hot hit、重复 MRU、uniform 与 Zipf；种子/分布参数固定 |
| 缺失 | 非满 miss、满容量新键淘汰；Key/T 构造包含在相同计时边界 |
| 顺序 | 超容量顺序扫描导致 thrashing、混合 hit/miss/erase |
| 复用 | 相同数据流分别 get promotion 与 peek 不 promotion |

预生成访问序列、输出消费方式、预热、重复次数和计时边界保持一致。
分别记录 steady-state 与 cold fill，不能用构造/预留移出计时掩盖总成本。
报告 ns/op、吞吐与重复样本分布；批量 benchmark 平均值不冒充单请求 p99。
单独记录 sizeof/alignof、节点/bucket/深层分配的次数和字节、live/peak，
包含瞬时 C+1；RSS 只能作为另一个指标，不能代替精确分配计数。
敏感选择仅比较 MRU 短路、victim 二次查找、节点/回指布局及必要的分配策略。
检查生成码/代码大小；可用时采 branch/cache counters，缺失则明确说明。

Linux x86-64 与 Windows x86-64 分别做同机 candidate/reference 比较，
记录 CPU、OS、编译器、STL、allocator、flags、源码 SHA 和原始输出。
不可把不同 CPU 的绝对时间差解释为 OS 差异；ARM 有原生需求和设备后再验收。
不以单个热命中数字宣布整体胜出，也不预设 pool、自制索引或更少分配一定更快。

## 交付门槛与复议

L01 的验收是完整行为、失败保证、生命周期和最小平台检查，
L02 才允许提出有范围的性能结论。每份报告保留持平/落后项和未运行检查。
若保留标准库候选，需要解释双查找、bucket 冗余和节点成本相对成熟实现的取舍；
若改为薄适配，重新核对契约与依赖成本，不把上游全部 API 变成 WJR 承诺。
TTL、并发或字节预算只有真实调用需求出现后再做独立设计，不能阻塞此有限能力。
