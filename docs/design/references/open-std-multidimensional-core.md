# C++ 多维数据核心：mdspan、映射和相邻提案

本文聚焦多维 index space、layout mapping、accessor、切片和相关标准化边界，供 WJR NP 核心设计审查使用。
引用区分已经发布的标准、当前公开工作草案、单独提案和特定后端支持。
**WG21 草案不是已发布的 ISO 标准，也不证明任何特定 STL 已实现。**

## 1. 状态快照与主要来源

### 状态快照

- `mdspan` 与 `extents` 已进入 C++23，主提案为 [P0009R18](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0009r18.html)。
- 本文用 [eel.is 当前公开 C++ 草案的 `mdspan` 章节](https://eel.is/c++draft/mdspan.mdspan.overview)逐条核对现行 wording。
- 当前草案包含 `submdspan`、padded layouts 等后续纳入的多维接口；这些是工作草案状态。
- `submdspan` 的提案基线是 [P2630R4](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2630r4.html)，现行草案已含对应章节。
- `layout_left_padded` / `layout_right_padded` 的提案材料是 [P2642R6](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p2642r6.pdf)；现行草案索引含 padded layout 条目。
- `mdarray` 是 [P1684R5](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p1684r5.html) 所述 owner 提案，本研究不能将其称为现行 C++ 标准设施。
- [P1673R13 linear algebra](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p1673r13.html) 已投票进入 C++ 工作草案；它定义面向 BLAS 的自由函数算法接口，不是 NumPy 风格 lazy elementwise AST。
- [P2897R7 `aligned_accessor`](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p2897r7.html) 是提案历史版本；2026-10-18 核对的[当前草案](https://eel.is/c++draft/mdspan.accessor.aligned)已经包含 aligned_accessor 的 synopsis 与成员规范。这不证明所用后端或目标 STL 已提供它。
- 项目实际后端来自固定的 Kokkos/mdspan 快照：`0e4eba59ad53451784b3839bdf98b25fa2d64931`。
- WJR 的 [`src/np/CMakeLists.txt`](../../../src/np/CMakeLists.txt) 明确用 `GIT_TAG 0e4eba59ad53451784b3839bdf98b25fa2d64931` 固定 Kokkos 源码，同时记录其项目版本字符串 0.6.0 不足以识别实际快照。
- WJR 对应后端实现来自 [Kokkos/mdspan commit](https://github.com/kokkos/mdspan/tree/0e4eba59ad53451784b3839bdf98b25fa2d64931)。
- 目标编译器和标准库上的支持应单独实测；C++23 语言模式不意味着 `std::mdspan` 可用。

### 来源索引

| 主题 | 主要来源 | 证据类别 |
| --- | --- | --- |
| `mdspan`, extents, layout/accessor 分工 | [P0009R18](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0009r18.html) | C++23 已采用提案文本；与最终现行草案做 wording 对照 |
| 现行 mdspan class / trivial copy 条件 | [draft mdspan overview](https://eel.is/c++draft/mdspan.mdspan.overview) | 公开工作草案 |
| extents 静/动态信息 | [draft extents](https://eel.is/c++draft/mdspan.extents) | 公开工作草案 |
| mapping 性质与 span | [draft mapping requirements](https://eel.is/c++draft/mdspan.layout.reqmts) | 公开工作草案 |
| `layout_stride` 构造限制 | [draft layout_stride constructors](https://eel.is/c++draft/mdspan.layout.stride.cons) | 公开工作草案 |
| slices 和 `submdspan` | [draft submdspan](https://eel.is/c++draft/mdspan.sub.sub)、[P2630R4](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2630r4.html) | 当前草案 + 提案依据 |
| padded layout | [P2642R6](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p2642r6.pdf) | 提案版本；现行草案索引另行核对 |
| owner proposal | [P1684R5](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p1684r5.html) | 提案，不作为标准 API |
| linear algebra algorithms | [P1673R13](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p1673r13.html) | 已进入当前草案的算法提案 |
| aligned accessor | [当前草案](https://eel.is/c++draft/mdspan.accessor.aligned)、[P2897R7](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p2897r7.html) | 当前草案接口 + 历史提案；后端支持另验 |

## 2. 类型分层和映射语义

### `extents`: index space，不是存储

`std::extents<IndexType, Extents...>` 描述 rank 维的 Cartesian index space。
`rank()` 和 `rank_dynamic()` 是编译期可用常量。
每轴可为静态 extent 或 `dynamic_extent`。
静态 extent 直接编码在类型；动态值保存在对象中。
混合静态/动态 extents 允许编译器传播已知 rank 和部分轴长度。
`extent(r)` 给当前轴的实际长度。
每个索引轴定义区间 `[0, extent(r))`。

`extents` 是 trivially copyable；IndexType 可为 signed 或 unsigned integer type，见[overview](https://eel.is/c++draft/mdspan.extents.overview)。构造输入必须非负、可表示并匹配静态 extent；checked 入口在转换前拒绝负输入，不要求 index_type 自身能表示负数。
extents 不知道 buffer 地址、buffer 长度、allocator 或元素生命周期。
rank-0 的 index space 有一个逻辑坐标空 tuple。

rank-0 的 size 是 1；任意 extent 为 0 的非零 rank shape 的 size 为 0。
extent 乘积的 representability/overflow 必须在外部 allocation 边界处理。

### mapping: index 到 offset

layout policy 对某个 extents 类型定义 `mapping<Extents>`。
核心映射调用 `mapping(i...)` 把合法的多维 index 映射到非负的 `index_type` offset。
mapping 不持有元素，也不定义数据句柄。
映射要求 copyable、equality-comparable、nothrow move/assign/swap。
mapping 的 `required_span_size()` 是能覆盖映射 offset 的最小非负 span 数量。
若 index space 为空，required span 为 0。

若非空，required span 是 `1 + max(mapping(i...))`。
连续、唯一、穷尽是不同性质。
`unique`: 不同逻辑坐标不映射到同一个 offset。
`exhaustive`: span 范围 `[0, required_span_size())` 中没有未被任何合法坐标覆盖的空洞。
`strided`: 每维索引增加 1 时 mapping offset 增加固定 stride。
`is_always_*` 是类型性质，保证该 mapping 类型所有实例的能力。
对象级 `is_*` 是具体 mapping 实例属性。

mapping 可以保守返回 false，即便数学上实际满足；调用者不能把 false 当成证明其反面。
`layout_right` / `layout_left` 给出紧凑的连续次序。
`layout_stride` 保存显式 strides，但其约束有限，不能表达任意 NumPy mapping。

### accessor 与 `mdspan`

`mdspan<ElementType, Extents, Layout, Accessor>` 组合三类状态：

1. data handle，由 accessor 定义，可不必是裸 `T*`。
2. mapping，由 layout policy 对应的 extents 构造。
3. accessor policy，定义引用类型、offset policy 和访问规则。

`operator[]`/多索引访问大体是 `accessor.access(handle, mapping(index...))`。
因此“线性 offset”不必等于 `T* + offset`。
`mdspan` 是 non-owning view；复制 view 不复制底层元素，也不延长底层资源生命期。
`const mdspan<T,...>` 是浅 const：成员函数仍可经可写 `T` handle 修改元素。
元素只读应写作 `mdspan<const T,...>`。
转换允许增加元素 const，不能删除 const。
data handle、mapping 和 accessor 要分别检查转换合法性。

默认 accessor 的 data handle 是指针，但自定义 accessor 可持有其他 handle 类型。
标准 mapping/accessor 有 nothrow 移动等要求；view 对象 trivially copyable 只在 accessor、mapping 和 handle 都 trivially copyable 时成立。
所以“mdspan 一定是一个指针”及“任何 mdspan 一定可 trivial-copy”都是错误的无条件结论。
`[[no_unique_address]]` 可压缩空成员，但 ABI、类型相同成员地址规则和实现布局仍应实测。

它不是所有空成员在所有编译器上都保证零字节的承诺。

## 3. `size`、`required_span_size` 与 allocation

`mdspan::size()` 是逻辑 index space 元素数。
`mapping.required_span_size()` 是映射所覆盖的 offset span 长度。
唯一且 exhaustive 时二者相等；单凭数值相等不能反证 mapping 既唯一又 exhaustive，例如有重复 offset 和空洞可能恰好抵消。
例如 shape `[2,3]` 且 strides `[4,1]`：逻辑 size 为 6，最大 offset 是 4 + 2，span size 为 7。
该 span 中 offset 3 是 padding hole。

allocator 至少要确保 data handle/accessor 能访问需要覆盖的 span；不能只分配逻辑 size。
如果某个 mapping 有 offset base/偏移概念，也需纳入 accessor 和 accessible range 合同。
`required_span_size` 不等同于 `std::vector::size()`、capacity、字节数或 allocation 可用字节。
元素数乘 `sizeof(T)` 也需防止 size_t overflow 与 ptrdiff 表示问题。
映射有效并不自动证明用户给出的 data handle 指向如此长的有效对象数组。

`mdspan` constructor 的前置条件包括 mapping 覆盖范围对 handle/accessor 可访问；它不检查实际 allocation。
checked API 应先验证可验证的整数范围、required span、byte span、长度和 null 条件，再构造 mdspan。typed pointer 不足以探测对象生命期或实际 allocation；WJR 仍要求调用方提供有效且自然对齐的对象。额外对齐与地址空间检查属于明确接收相应信息的外部适配层。
空 shape 与 rank 0 不可合并成“空数据”。

空 `mdspan` 可有 null handle，但不可以通过 null 做索引或指针偏移。
rank-0 的逻辑标量需要一个可访问元素。

## 4. `layout_stride` 的限制和 uniqueness

当前公开草案 `layout_stride::mapping` 接收 extents 与 rank 长度的 stride 序列。
对每一维，转换后的 stride 必须严格大于 0。
因此标准 `layout_stride` 不支持 zero-stride broadcast。
它也不支持 negative stride/reverse view。
required span 必须可表示为 `index_type`。
多维 mapping 必须存在轴排列，使排序后的 stride 满足：每个下一个 stride 至少是前一轴 stride 乘此前一轴 extent。

这是该标准 layout_stride 构造器要求的轴排序条件，保证其允许子集的 uniqueness；不能外推为所有数学上唯一 stride 的判据。例如 shape (2,2)、strides (2,3) 的四个 offset 不重复，却不满足上述排列条件。WJR 正确地拒绝这个不属于后端构造子集的输入。
这允许 gap/padding；`exhaustive()` 仍可能为 false。
rank 0 没有 stride 参数。
extent 0 时逻辑域为空，offset 范围另按 required span 规则处理。

对空轴或 extent 1 轴，某些 stride 值对实际元素地址没有影响，但 constructor contract 仍要求正值。
不能把“索引永远不会访问空轴”用作传入 0/负 stride 的理由。
从其他 mapping 转 `layout_stride` 也要求映射 offset 为 0、正 stride、足够 representable 和 always-unique/strided 条件。
不要通过 cast 把 signed stride 转成 size/index type；负值转无符号会变成巨大正数。
`layout_stride` 也不代表带符号 pointer diff 安全，实际底层资源仍由调用者负责。

若产品要求反转或 broadcast view，应有不同的自定义 mapping 或高层表达式语义。
若 mapping 重叠，只读读取可在更宽松自定义 API 里定义；标准 `layout_stride` 本身仍拒绝该表示。
可写重叠需要明确写入顺序或禁止，否则执行次序改变输出。

## 5. Preconditions、索引、constexpr 与异常边界

标准 mdspan 是低级 descriptor，索引 operator 的前置条件由调用者承担。
普通 `operator[]` 不承诺调试/发布均检查每一维边界。
当前工作草案出现的 `at`/checked access 与具体后端支持需分开验证。
不能把新草案功能反推到 WJR 最低 C++20 目标。
mapping 构造器中的 positive stride、unique 条件、required-span representability 等属于有效输入前置条件。
若使用 violating precondition 的 mapping construction，不能期待库可恢复报告 shape error。

真实外部数据导入可在其边界验证；WJR 普通路径采用可信 mdspan 构造，可保留返回同一种 alias 的 try_make_* 薄函数，不制作 checked 数据结构。owner 的分配前检查仍独立保留，见[核心标准](../np-core-standard.md)。
一旦已建立 view 的可信 mapping，可在热循环避免重复完整校验。
`extents` 的 rank/static_extent 是纯编译期信息。
mapping 也可在 constexpr context 构造/访问，但是否实际常量求值取决于输入和表达式上下文。
“API constexpr”不意味着该映射和完整数据计算自动在编译期执行。
表达式模板 AST 的 static type 信息也不等于 compile-time evaluation。
异常规格应从组成状态和实际算法推导。

extents/mapping/view 的大多数 descriptor 操作要求或提供 noexcept move；分配、owner construction、checked error construction 和用户 callback 是单独边界。
为获得简单 ABI 而将可抛异常的用户操作标成无条件 noexcept 会导致 terminate，不是免费优化。

## 6. `submdspan`：slice、offset、mapping 和 accessor

当前工作草案的 `submdspan` 将 rank 数量的 slice specifier 应用于源 view。
整数索引会 collapse 一轴。
`full_extent` 保留完整轴。
`std::pair` range 切片表达半开区间。
strided slice 表示带 step 的片段。
输出 extents 可保留 full axis 和静态 extent-slice 的静态信息；一般动态切片的 extent 为 dynamic。
`submdspan_mapping` 根据源 mapping 和 slices 计算新 mapping 与 offset。

通常新 data handle 是通过源 accessor 的 `offset(handle, offset)` 得到。
返回 mdspan 使用源 accessor 的 `offset_policy`，以适应偏移后 data handle 类型变化。
因此切片返回值不总是沿用完全相同 accessor 类型。
标准接口不拥有源 buffer；切片 view 的生命期不能超过数据句柄所表示对象的有效期。
空切片不能对空/null handle 做不必要的指针加法。
若 slice 结果保留 `layout_left`/`layout_right` 性质，mapping 能表达紧凑布局。

含 step/gap 的 slice 可能回落到 `layout_stride`。
标准 `submdspan` slice stride 仍受 mapping 正 stride 规则约束。
反向切片所需负步长语义不由此标准接口直接提供。

### alignment accessor 的子视图影响

如果源 accessor 声称 pointer 具备更强 alignment，增加一个任意元素 offset 后该保证可能消失。
`offset_policy` 可以降级到普通 default accessor。
这不是无谓保守：源首指针对齐不证明子切片首指针仍对齐。
若要保留更强 alignment，调用者需证明 offset 的 byte multiple 满足要求。
一个 `assume_aligned` 访问器表达的是调用方 contract，不是运行时自动对齐检查。

## 7. padded layouts 与 aligned_accessor

### P2642 padded layouts

`layout_left_padded<S>` / `layout_right_padded<S>` 保留规则布局与指定 leading dimension padding。
它们适合显式表达行/列 padding、vector alignment 或库调用的 lda。
这比把所有元数据退化成任意 strides 保留更多静态性质。
P2642 的工作是 C++26 多维设施演进的一部分；本任务在 P2642R6 提案文本和现行草案目录作了状态核对。
具体 stdlib 支持必须按 feature-test macro、头文件和目标版本验证。

不要按 proposal PDF 就假定 Clang/GCC/MSVC STL 当前提供它。
padding 改变 required span 与逻辑 size 的关系。
垫充元素不是逻辑 index space 的元素。
只有 allocation 足以覆盖 padded span 时 mapping 才可用。

### P2897 aligned_accessor

P2897R7 讨论一种能表达 pointer over-alignment 的 mdspan accessor。
accessor 返回带 `assume_aligned` 的访问指针/引用路径。
其前提是调用方确实满足声明的对齐。
切片后的 offset 可能降低对齐，因此 offset policy 可以退回普通 accessor。
若 SIMD kernel 需要更强对齐，不能用 descriptor 原始 handle 的初始对齐替代每个 tile/row 起点的证明。

aligned_accessor 已存在于当前工作草案，offset_policy 明确是 default_accessor<ElementType>。WJR 不能由草案存在性宣称其固定后端或任意目标 STL 已可使用；升级和支持测试是独立事项。
有实际 kernel 收益时再引入受限 alignment policy 或后端适配；目前不为追随提案增加另一套 accessor framework。

## 8. `mdarray`、linalg 与表达式树边界

### P1684 `mdarray`

`mdarray` 提案把 mapping/extents 与既有 owning container 组合。
它将分配、allocator 和元素 value semantics 交给 container。
P1684R5 讨论移动后容器与 mapping 的一致性、required span、`container_size()` 等行为。
这些是值得参考的 owner 问题清单。
它并不是 P0009 `mdspan` 的自然自动拥有版本，也不能由标准 mdspan 本身管理 buffer。

在现行公开草案未核对到 mdarray standard synopsis；因此只当 proposal/reference implementation 研究。
Kokkos 当前仓可能提供 proposal 的 experimental `mdarray` 代码；这证明后端有实现，不证明标准状态或 WJR owner 合同适用。

### P1673 linear algebra

P1673 提供 mdspan 参数上的 BLAS 风格 free functions。
算法可以约定矩阵乘、向量运算、solve 等 operation 的 shape/layout/accessor 要求。
它把数据结构与运算接口分开。
草案/提案中的部分算法可重新排列浮点求和，调用者不能据名称默认 bitwise deterministic。
该接口不提供通用 `a + b * c` 的 lazy expression node tree。
也不自动定义通用 NumPy broadcasting、xarray owner、expression closure lifetime 或 map callable AST。

可借鉴的是算法函数接收 mdspan view 的互操作层，不应误判为 expression engine。

### SIMD 邻接工作

标准 SIMD proposal 的 batch/mask、ABI 和 execution 模型是不同层次。
本 brief 只把 SIMD 视为 kernel/backend 层问题：mapping 与 accessor 提供 stride/data access contract，算法决定是否批量读写。
mdspan 自身不定义 SIMD batch type、loop vectorization、tail 或 dtype promotion。
开启某种标准/提案 API也不证明编译器会向量化一个任意 strided expression。

## 9. 与 WJR 当前核心的映射

### 当前快照

本审查 worktree 是 base `39fb36bae1f937493657f49cb8075b707389e63f`。
`include/wjr/np/view.hpp` 将公开 WJR view 名称 alias 到 Kokkos mdspan 的 extents/layout/accessor。
WJR CMake 固定 Kokkos snapshot 为 `0e4eba59ad53451784b3839bdf98b25fa2d64931`，要求 C++20。
后端不是根据每个翻译单元的 `__cplusplus` 自动切换到 `std::mdspan`。

已存在 owner 在 `tensor.hpp` 组合 mapping 与 storage。
表达式 AST 在 `expression.hpp` 使用 view/scalar/unary/binary value nodes，底层 mapping metadata 与 evaluator 分层。
`core/expression.hpp` 在此 base 仍是占位类，不能据其文件名宣称存在第二套实现。
`core/assign.hpp` 也是当前阶段骨架；实际 expression API/assignment 实现在 NP 头文件的相关代码中。

当前 expression view 支持 left/right 以及标准后端 `layout_stride` descriptor 子集。
WJR 当前 view/owner 不拥有 shared_ptr，也不对借用数据计引用。
审查基线 owner 只采用 packed left/right layout，另有正 stride 构造失败报告函数；最新设计合并到 make_view.hpp，可信内部直接构造同一种 view。
两者是不同能力：借用 view 的 mapping 有效，不代表 owning storage 已按 required span 分配。

负 stride owner 尚未实现；不能用已有 positive-stride view path 预设其 offset 语义。
WJR 的[正 stride owner](../tensor-strided-owner.md) 已由 #82 合入：显式 shape/strides factory/adopt、packed 默认配置不变、逻辑大小与 required span 分开。
本参考研究未运行其编译或性能验证；实现状态与统一约束见[核心标准](../np-core-standard.md)。

核心审查接受“不提供 shape-only/default strided owner”及“静态 extents 仍保存 runtime strides”。新建/采用容器长度精确等于 required span，普通容器复制可能读取孔洞，default_construct 的先写后读责任覆盖这些位置。
本次已审查工作副本的数据模型；实现测试由对应能力交付，不能把审查替代编译与运行证据。统一状态和不变量见[核心标准](../np-core-standard.md)。

### 借鉴边界

- 继续复用 `extents`、mapping、accessor 和 submdspan 的成熟职责拆分。
- 把逻辑 shape count、mapping span 和 owner allocation count 作为不同字段/计算来审查。
- view 构造由可信调用方满足前提；owner 分配检查保留内部，外部 byte-buffer 适配另定可检查信息，不建立通用 checked view 层。
- `submdspan` 的 accessor offset policy 说明子视图并非只能作指针加法。
- 用 mapping 能力决定 linear kernel，不凭 layout type 名称做过强推断。
- 对 descriptor state 的 size/triviality 做工具链实测，尤其 MSVC ABI。
- C++23/26 standard APIs 作为适配互操作入口；C++20 后端仍固定并受 CMake dependency pin 控制。

### 不照搬的地方

- `mdspan` 不提供 owner；owner 仍需 allocator/lifetime/copy/move contract。
- 标准 positive `layout_stride` 不满足 NumPy negative/zero stride view 的全部需要。
- `layout_stride` uniqueness precondition 不适用于广播映射。
- `required_span_size` 不验证实际分配长度。
- proposal 的 standardization status 不能替代项目编译器矩阵证据。
- 不因为未来草案有 `at()`、padded layouts 或 accessor 就把新标准设为 WJR C++20 requirement。
- 不通过增加 runtime metadata/branches 让每一个热循环重复检查已经验证的 mapping。
- 不根据 `[[no_unique_address]]` 推断所有编译器/所有 policy 组合都零空间成本。

## 10. WJR 决策与剩余验证

统一裁决见[核心标准](../np-core-standard.md)：复用后端，保留 fixed-rank 借用与 Container + mapping，正 stride owner 分配完整 span，广播零步长只用于只读表达式投影。signed-stride 不塞入标准 mapping，泛切片以保存静态 shape/layout 证据为下一步，padded/aligned 后端能力按实际支持逐项准入。

| 已可判断 | 仍须实测或单独设计 |
| --- | --- |
| index_type 可 signed/unsigned；rank-0 与空域不同 | 新编译器/后端升级后的尺寸、约束与无异常包含测试 |
| required span 不等于逻辑 size 或容量 | 有孔 owner 的构造/复制/异常和初始化成本 |
| layout_stride 只覆盖标准要求子集 | signed descriptor 的 offset、范围、可写性与互操作 |
| accessor.offset/offset_policy 影响切片结果 | 正切片、空范围、窄 index 和布局保持的实际后端行为 |
| 草案状态不等于 STL 支持 | C++23 std::mdspan 与固定 Kokkos 的显式互操作 |

当前后端 view 尺寸与编译器证据已有[数据模型文档](../tensor-data-model.md#已测成本与限制)，无需把历史 probe 当作尚待启动的工作。本批未重跑尺寸或 throughput 测量；尤其 MSVC ABI 的静态 view 空间缺口仍存在于该历史证据中，不能称为已修复。

## 11. 精确条款和核对链接

### Current draft

- [`mdspan` synopsis/overview](https://eel.is/c++draft/mdspan.mdspan.overview)：四个模板参数、mapping/accessor state、rank/stride observations、trivially-copyable 条件。
- [`extents`](https://eel.is/c++draft/mdspan.extents)：静态/动态 extent，rank，构造约束。
- [`layout mapping requirements`](https://eel.is/c++draft/mdspan.layout.reqmts)：offset、required span、unique/exhaustive/strided 定义。
- [`layout_stride` constructor preconditions](https://eel.is/c++draft/mdspan.layout.stride.cons)：正 stride、span representability 和 uniqueness 的 stride-order 条件。
- [`submdspan` function](https://eel.is/c++draft/mdspan.sub.sub)：切片约束、mapping result 和 accessor offset policy。
- [`submdspan slice types`](https://eel.is/c++draft/mdspan.sub.overview)：full extent、integral、range/strided slice 类别。
- [`padded layouts` 草案索引](https://eel.is/c++draft/libraryindex)：确认当前公开草案目录确有 `layout_left_padded`/`layout_right_padded` 命名条目。
- [`linalg` 草案索引](https://eel.is/c++draft/libraryindex)：确认当前草案中有 linear algebra 名称；具体实现另行查实现库。

### Papers and pinned implementation

- [P0009R18 mdspan](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0009r18.html)
- [P2630R4 submdspan](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2630r4.html)
- [P2642R6 padded layouts](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p2642r6.pdf)
- [P1684R5 mdarray](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p1684r5.html)
- [P1673R13 linear algebra](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p1673r13.html)
- [P2897R7 aligned_accessor](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p2897r7.html)
- [Kokkos/mdspan pinned commit](https://github.com/kokkos/mdspan/tree/0e4eba59ad53451784b3839bdf98b25fa2d64931)
- [WJR NP CMake pin](../../../src/np/CMakeLists.txt)
- [WJR NP package config](../../../cmake/wjr-np-config.cmake)
- [WJR tensor data model](../tensor-data-model.md)

本次浏览核对了 P0009R18、current eel 草案中 mdspan/extents/layout_stride/submdspan 条目、P2630R4、P2642R6、P1684R5、P1673R13、P2897R7，以及 WJR/Kokkos 固定快照。
未验证任一候选 STL 对 C++26 接口的实现覆盖，也未运行实际 Kokkos/WJR 编译 probe。
