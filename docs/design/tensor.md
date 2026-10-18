# Tensor 数据与计算设计导览

WJR 的多维计算层已经提供借用 view、独占 owner、预分配计算、广播值表达式和终结式规约。
共同语义与已实现/候选/后续能力的区分以[NP 核心标准](np-core-standard.md)为准，
本页负责导航和职责说明，不再保存与实际 API 重复的初始选型草图。

## 职责与入口

| 层 | 职责 | 文档 |
| --- | --- | --- |
| extents / mapping / accessor | 固定 rank 的索引域、布局与句柄访问 | [借用数据模型](tensor-data-model.md) |
| 直接借用 | 可信调用方满足 mdspan 构造前提；不另建 checked view 层 | [构造与迁移](tensor-data-model.md#直接构造与迁移)、[正 stride](tensor-strided-view.md) |
| owner | 容器生命周期、allocator、mapping 提交与可借用状态 | [owner](tensor-owner.md) |
| view 操作 | 借用行列、转置与布局兼容的 reshape | [subview](tensor-subview.md)、[transpose](tensor-transpose.md)、[reshape](tensor-reshape.md) |
| 直接计算 | 预分配 copy/fill/transform | [逐元素接口](tensor-elementwise.md) |
| 表达式 | 值闭包、逐节点类型、广播、融合与转换 | [API](tensor-expression-api.md)、[cast](tensor-expression-casts.md) |
| 物化 | 创建独立结果并显式承担分配 | [eval](tensor-evaluation.md) |
| collective | 有顺序合同的 view/表达式 sum 与 view scan | [sum](tensor-reductions.md)、[表达式 sum](tensor-expression-reductions.md)、[scan](tensor-scans.md) |
| 可选 SIMD | 显式后端与保守 scalar fallback | [表达式 xsimd](tensor-expression-xsimd.md) |

普通 view 不持有 shared_ptr，不为设备、线程池、Python 或计算缓存预留字段。
计算主要接收 view；owner 不自动转换成表达式，也不借用临时 owner。
map、mask 与正 stride owner 的审查快照及集成状态单列在核心标准，不由本页推断发布情况。

## 复用决定

底层固定复用 Kokkos mdspan `0e4eba59ad53451784b3839bdf98b25fa2d64931`。
WJR 保持 C++20，不按每个翻译单元的语言模式更换公共后端。
标准 mdspan 的未来互操作与后端升级各自验证，不能把草案 API 存在等价成 STL 可用。

owner 采用 Container + mapping_holder，默认 tensor_vector 的显式默认初始化路径支持完整覆写。
普通工厂仍值初始化；保留 vector capacity/长度的空间成本，不把容器复用说成最小表示。
计算层采用静态值闭包与每次调用的一次绑定，不直接继承 xtensor 的动态 shape/cache/共享基类。
这些选择基于所需合同，仍接受同语义测量的反证。

[xtensor 0.27.1 导读](references/xtensor-core.md)是设计案例和比较输入，既非零开销承诺，也非性能上限。
[WG21 多维核心导读](references/open-std-multidimensional-core.md)区分标准、草案、提案和固定实现，
解释逻辑大小、required span、分配长度、负步长与零步长的不同责任。
具体闭包、alias、SIMD 资格与执行结构见[表达式设计](tensor-expressions.md)。

## 接下来的边界

在已有完整 span owner 上，优先提供保留布局证据的正切片、有限数值函数、
按明确顺序执行的 fused collective，以及有测量支持的 SIMD 和表示改进。
[实施计划](expression-roadmap.md)包含依赖与验收，不把已经合入的 eval/cast/sum 重列成未来工作。

signed-stride、动态 rank 内核、公共 alias/scratch 便利层、精简 exact storage 均按实际需求单独准入。
它们不要求现在增加运行时 DAG、自动缓存、调度器或通用 ownership framework。
Python、DLPack、Arrow、oneTBB 与 OpenXLA 在独立边界处理 dtype、外部布局、lease、异步就绪和导出能力，
未使用时不增加普通 owner/view/AST 成本。具体准入规则由核心标准统一约束。
