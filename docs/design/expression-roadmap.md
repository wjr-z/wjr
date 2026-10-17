# 表达式实施计划

本计划落实 [表达式设计](tensor-expressions.md)，按可用能力分组，不按文件或任意行数切分。
每组包含实现、针对性验证与能展示功能的示例。通常控制在约 1000–2000 行内，
自然较小的能力不填充代码；依赖合入后及时刷新分支。

## 第一阶段：直接可用的计算

### 1. 表达式值与连续求值

依赖：现有 np owner/view、逐元素接口。

- 实现 as_expr、scalar/view/node 值闭包、内建一/二元算术、真实标量 result type。
- 保留静态 rank/extents，提供后续共用的广播代数；本组先支持同 shape 连续输入与 scalar。
- eval_to 不分配，使用真实 mdspan 数据，保留精确 pointwise 原地合同。
- 拒绝临时 owner 借用和不可复制节点左值；不悄悄换成引用捕获。
- 示例：axpby、返回局部 difference-square 表达式。

验收：三主流编译器 C++20；标量/描述符快照、局部节点返回、rank0/空输入、
中间整数除法/promotion、分配计数与真实 mdspan 叶子生成码。
若首组需要缩小数值类型子集，必须明确范围，不把 double 原型称作通用实现。
不混入自定义 callable、共享图、别名求交或调度器。

### 2. 固定 rank 广播与 stride 求值

依赖：1。

- 每个叶子一次投影到输出域，加入赋值式广播。
- 支持 left/right 混合、正 stride、静态 singleton 和 leading-rank 扩展。
- 提供自然 shape 查询与可复用的 checked shape resolution，执行内循环不重验。
- 示例：行 bias、列 scaling、转置输入；输出可带 padding。

验收：独立标量 oracle 覆盖 rank0–4、固定/混合/动态 shape、0/1/N 规则、
[N,1]+[1,M]、空域不读取/不调用数值函数、padding 保持、错误在 checked 边界写入前返回。

### 3. 拥有结果与显式物化

依赖：2，可在范围自然较小时与 2 合并。

- eval 根据自然 shape/dtype 创建默认 row-major owner，保持 make_tensor 的 expected/allocator 约定。
- 通过小型具名入口选择 layout/allocator，不预建复杂存储 policy 框架。
- eval 始终物化新结果，不根据输入类别有时返回引用。
- 示例：物化昂贵子表达式后，多次借用其 view。

验收：零维度与标量结果、类型断言、分配前元数据溢出处理、抛出时清理、allocator 保留、
安装消费。单独测量 owner 初始化+计算，不能复用预分配结果冒充拥有结果成本。

## 第二阶段：性能与执行合同

### 4. 可选 SIMD 后端

依赖：2；不阻塞基础标量功能。

- 独立可选 header/target 与明确 policy，复用 xsimd 原语与 ISA 约定。
- 同类 float/double 内建树，多个叶子、packed stream、splat 和有界 scalar tail。
- 不支持的 mixed dtype/operation/mapping 保守走标量，不能让无效 batch 分支参与实例化。
- 示例：同一表达式选择标量和 SIMD，展示相同结果。

验收：声明的 FP 合同下标量/batch 一致、mixed integer division 反例、
offset/alignment/tail/空/原地、ASan 与三编译器手写 SIMD 对照；安装消费者验证可选依赖。
不增加 Windows job，不把噪声性能数据作为通过门槛。

### 5. 内层行绑定与常用广播性能

依赖：2、4。

- 根据共同 stride witness 选择连续内层，按行/列重新绑定，处理 zero-inner-stride splat。
- 只增加本组实际需要的 axis collapse，不生成所有 leaf load-mode 组合。
- SIMD 之外仍保留清楚的标量 fallback；调查 MSVC 时采用小型独立 cursor 对照，不猜测性重写 AST。
- 示例：row bias、column scale、padded rows。

验收：静态/动态内维、重复叶子、交替与打乱的短/长输入；
比较 setup+loop、scalar hoist 和 pointwise 原地安全，记录编译体积。

### 6. 安全直接求值与 caller scratch

依赖：2、3，可独立于 SIMD 性能工作。

- 一次把存储关系分为 disjoint、exact pointwise、hazardous 或 unknown。
- try_eval_to 在写入前返回 shape/overlap 错误；typed scratch 支持安全 assign_to。
- 仅在确实省去有用工作时加入明确 noalias promise，不自动映射成 restrict。
- 示例：重叠位移与原地转置通过预分配 scratch 完成。

验收：前/后移位、转置、重复同源、输出行/元素被广播、span 相交但逻辑不交的列、
scratch 别名/shape/空域、异常与 copy-back 保证。不建立通用集合求交器。

## 第三阶段：表达力与规约

### 7. Pure map 与显式转换

依赖：1；SIMD 适配依赖 4。

- 存储 const-invocable pure callable，保留配置、转发、非复制状态与条件 noexcept。
- 明确 cast 位置，SIMD conversion 按能力保守准入。
- 示例：callable 局部临时实现每元素复用，与整体物化对照。

验收：有可见构造/析构的空 functor 不被丢弃、状态 copy/move、显式 move-only、异常。
纯度是语义要求，不凭 arithmetic 返回类型自动承诺 SIMD 等价。

### 8. Terminal sum 与 axis reduction

依赖：2、3；这是首批研究/训练示例的重要能力。

- sum(expr) 与 sum_to，编译期或固定数量 axes、keepdims、显式 accumulator 与顺序。
- 融合 pointwise 输入 map，初版要求规约输出与输入不交。
- 明确定义空域/空轴列表/identity，执行前处理重复与非法轴。
- 示例：row centering，再计算平方残差；规约 workspace 显式预分配。

验收：全量/行/列 sum、非连续轴、转置/padding、keepdims 后广播、zero extent、
累计 promotion、整数溢出前置条件与消去敏感有限输入；计数证明每坐标按合同读取。
不把 lazy reducer 加入 pointwise concept。

### 9. Mask 与 eager selection

依赖：数值核心稳定后的 2、4、7。

- 比较节点、backend-local mask、eager select、具名逻辑组合。
- 明确真实 bool 输出或显式 byte-mask，处理默认 owner 的 vector<bool> 限制。
- 示例：两侧均合法的 clipping/selection；惰性 masked division 留给独立能力。

验收：NaN/正负零、mask/data tail、不同编码；不泄漏 ISA mask ABI，
不把 eager select 描述成除零保护。

## 后续按证据选择

### 10. 构造、复用与编译成本

依赖：1–7 有实际使用后。

比较 2/4/8/16 叶子、重复树、静态/动态 shape 与状态 functor，
分别记录节点/plan 大小、非内联边界、构造指令、编译时间和二进制体积。
只为已证明的问题选一项：缩小 bound 状态、expression_ref、有限 fusion 或局部 map。
不同时引入全部机制；引用包装拒绝临时值且不宣称缓存结果。

### 11. 数值 collective 扩展

依赖：8。以下候选按独立模块交付，不合成一个大 MR：

- max + stable softmax/normalization，显式 scratch 与 NaN/空域合同。
- 显式 relaxed SIMD sum，比较误差与速度，说明重排。
- 具有明确溢出/下溢行为的 stable norm。
- 经评估的 BLAS matmul 边界与可选 pointwise epilogue。

### 12. 可选 oneTBB executor

依赖：成熟的绑定/别名合同与足够大工作量。

独立模块、明确 policy、实测 grainsize/阈值；分割可独立写入域，
规定 callable 并发安全、异常与嵌套并行。小任务保留顺序路径。
验收 medium/large 与内存带宽场景，不用线程调度掩盖低效串行 kernel，
不把非法重叠当作可并行修复的问题。

### 13. 最小 graph export 实验

依赖：内建运算语义稳定；不阻塞 CPU 功能。

用 visitor 把 leaf、builtin、cast、shape 与显式规约降到独立拥有的图。
拒绝尚不支持的 lambda/accessor/policy；区分常量与外部 buffer 绑定。
先比较一个小型 CPU/导出表达式，包括中间整数运算，再决定 XLA 接入范围。
未使用 exporter 时，不增加普通表达式的字段、分配或第三方依赖。

## 第一批交付顺序

先交付 1–4 的实际计算能力；随后按需求选 5 的速度、6 的安全赋值与 8 的规约。
不要用 checker-only 或 design-only 小片段填满并行数量。
有展示价值的能力附简短 examples，性能驱动与可读示例分开。
更宽的 dynamic rank、负 stride、complex、mixed-lane SIMD、严格 fenv、自动 DAG CSE、
异步拥有闭包和完整 XLA 支持都保留为独立范围。
