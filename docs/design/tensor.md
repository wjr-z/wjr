# Tensor 数据结构与计算设计草案

目标是一个通用的多维数据与计算层：owner 管理生命周期，计算主要借用轻量 view，
表达式保留编译期信息，evaluator 在边界处理广播、别名和后端选择。
这份草案约定初始数据模型及验收条件，不提供可用的 tensor API，也不添加实现。

## 复用基础与分层

标准化部分优先评估固定版本的 Kokkos mdspan，复用 extents、layout mapping 和 accessor，
不先重写同样的设施。C++23 的 `std::mdspan` 作为兼容视图的互操作入口。
一次构建固定内部基础类型及版本，避免因各翻译单元语言模式不同而自动切换公共后端类型。
[Kokkos mdspan](https://github.com/kokkos/mdspan) 提供 C++17/20 等 backport；
具体引入版本需通过编译器矩阵、布局和生成码验证后确定。

xtensor 用作 owner、view、表达式闭包、广播和求值设计的参照，也作为原型及正确性/性能对照候选。
是否直接使用其表达式层应先验证所需子集、编译时间、依赖范围和输出内存控制，
不预先实现一套完整的 xtensor 替代品。
参见 [xtensor 容器](https://xtensor.readthedocs.io/en/latest/container.html)。

### WG21 提案核对

以 open-std 原始提案理解设计动机，再按采用的标准版本、后续缺陷修正与固定版本的实现核对接口。
提案不是编译器已实现的保证，也不直接决定本库的对象 ABI。

| 参考 | 对当前设计的约束或启发 | 规划 |
| --- | --- | --- |
| [P0009R18 mdspan](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0009r18.html) | extents、mapping、accessor 分工；平凡复制取决于组成类型，不能无条件承诺 | 复用标准数据模型；默认 view 的句柄、mapping、accessor 都需验证平凡复制 |
| [P1684R5 mdarray](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p1684r5.html) | owner 可以组合 mapping 与已有存储容器，将分配责任交给容器 | 优先比较这种组合与 xtensor 子集，不预先自建 buffer；不宣称 mdarray 已成为标准设施 |
| [P2630R4 submdspan](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2630r4.html) | 切片同时生成 mapping、偏移和合适的 accessor | 复用可用的 submdspan；扩展负步长时保持独立语义 |
| [P2642R6 padded layouts](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p2642r6.pdf) | 行列填充布局可保留连续维度与 leading dimension 信息，避免全部退化为任意 strides | 作为后续 BLAS/SIMD 互操作候选，首版 owner 仍只采用无填充的连续布局 |
| [P2897R7 aligned_accessor](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p2897r7.html) | 原指针对齐不代表偏移后的指针仍对齐，offset_policy 可降级为默认 accessor | 对齐是显式前置条件；切片后重新证明或降级，不无条件保留标签 |

| 层 | 初始职责 | 持有的数据 |
| --- | --- | --- |
| extents 和 mapping | 索引到元素偏移的映射 | 必要的动态维度或步长；静态值进入类型 |
| 非持有 view | 对已有对象的多维访问 | data handle、mapping、accessor |
| tensor owner | 分配、构造、销毁、复制和移动 | 唯一持有的 buffer、allocator 和 mapping |
| expression | 组合运算及传播类型、shape、layout | 运算对象和操作数闭包 |
| evaluator | 选择循环、处理输出和别名 | 一次求值所需的规划与显式工作区 |
| 外部适配器 | Python、DLPack、Arrow、设备运行时 | 外部 owner、release callback 或异步事件 |

这些是职责边界，不是每一层都要自建的承诺。选型先比较 mdspan 的标准映射子集与
xtensor 的容器/表达式子集，并核对许可、维护、编译成本、输出内存控制及相关性能；
仅对实际不匹配的契约增加薄适配或小范围实现。

普通 owner 和 view 不默认持有 shared_ptr。需要跨语言或异步延寿时，由边界对象持有外部资源，
内层计算仍接收普通 view。第一阶段的 CPU 数据模型不携带每元素的动态 dtype 或设备分发。

下面是用于选型的结构草图，类型名及具体依赖尚未定稿；不是新增 API：

```cpp
// Layout::mapping<Extents> 内含 extents，不在 view 中重复存 shape。
template <class T, class Extents, class Layout, class Accessor>
using tensor_view = mdspan_backend::mdspan<T, Extents, Layout, Accessor>;

// 需要定制分配时的备选表示；先与 Container + mapping 方案比较。
struct allocation_state {
    allocator_pointer storage;
    size_type allocated_count;
};

struct owner_state {
    using mapped_storage = compressed_pair<mapping_type, allocation_state>;
    compressed_pair<allocator_type, mapped_storage> state;
};
```

常规 owner 首先只允许连续布局；所有逻辑元素均已构造，逻辑大小之外的预留槽位不构造对象。
逻辑大小由 extents 的乘积计算，mapping 提供 required_span_size()，allocated_count 记录容量；
失败构造时的已构造数量由局部清理守卫记录。
若复用已有容器，则由容器管理分配和构造状态，不在外层重复保存 allocator、指针或容量。
容器 size 至少覆盖 required_span_size()；首版无填充的连续布局可令 size 等于逻辑元素数，capacity 可以更大。
是否值得省去可推导的大小字段，需先测量对象密度和访问成本。
默认 accessor 只负责访问，不保留 lease；外部生命周期由调用或任务上下文管理。
默认原始指针 accessor 下，完全静态的连续 view 以一个指针为尺寸目标，
动态连续 view 只增加动态 extents；通用 stride view 再保存无法从类型或 extents 推导的步长。
这些是需用 sizeof 和生成码验证的目标，不依赖某个后端恰好采用的字段排列。

### 压缩存储原则

复用仓库已有的 `wjr::compressed_pair` / EBO，压缩空 allocator、静态 mapping 和无状态策略对象；
使用 C++20 的 `[[no_unique_address]]` 前核对目标编译器的实际布局支持，对有 ABI 限制的编译器保留 `compressed_pair`/EBO 路径。
同类优化也适用于无状态 deleter、运算 functor 和 accessor，但不能压掉有状态 allocator 或 accessor 的信息。

空类型不意味着任何组合都能占零字节：final 类型不能用继承实现 EBO，同类型子对象的地址规则、
对齐和 padding 也可能限制压缩。压缩布局不作为可序列化格式，也不跨编译器假定相同 ABI。
不通过 reinterpret_cast、裸 memcpy 或假设成员偏移来恢复被压缩对象；访问使用存储封装的正规接口。
验收覆盖空/有状态/final allocator、静态/混合 extents、空策略重复出现、过对齐元素，
同时检查 sizeof、alignof、构造/析构及复制移动语义，避免只优化一个默认实例。

## 维度与布局

主类型具有编译期 rank，extent 可以全部静态、全部动态或混合。
动态 extent 存在固定长度元数据中，避免为 shape 单独分配；静态 extent 不重复存储。
完全动态 rank 放在独立的互操作描述符中，进入内核前按需要适配或分派。

extent 值必须非负；extents 的 index_type 可按所选标准模型采用有符号或无符号整型。
signed-stride 扩展的偏移和步长必须能表达负值，跨索引类型转换前检查可表示性。
extent 乘积、字节数、stride 累乘、最小/最大偏移和 allocator 最大容量均在形成指针前检查。
静态可判定的错误在编译期拒绝；动态错误在 checked 构造或适配入口处理。

rank-0 的空维度列表表示一个标量，元素数为 1；任何一个 extent 为 0 时元素数为 0。
空视图可以没有数据指针，求值空结果时不读取元素或执行空指针算术。
`bool` 默认存储真实 bool 对象，不引入 vector<bool> 风格的位压缩代理；
位压缩 mask 和 nullable 数据使用独立适配类型。

标准布局首先覆盖 row-major、column-major 和正步长唯一映射。
编译期保留连续性、唯一性、穷尽性和对齐能力，转置或切片只在确实保持这些性质时保留相应标签。
切片后的地址不能继续假设具有原 buffer 的对齐。
对一般 accessor，切片经 accessor.offset(data_handle, offset) 生成新句柄，并采用其 offset_policy；
只有默认原始指针路径可以简化为指针偏移。空切片单独处理，避免对空指针执行算术。
对齐 accessor 在偏移后按可证明的对齐降级；重新提升需要检查或明确的调用方契约。

逻辑元素数、mapping.required_span_size() 与分配容量是不同概念。
唯一且穷尽的标准映射中前两者一致；填充或带间隔的映射可能需要更大的可访问存储范围。
外部适配按实际 address span 验证边界，不能只按元素数证明安全。

标准 `layout_stride` 要求正步长和唯一映射，不能承接任意 NumPy 视图。
需要负步长时增加独立 signed-stride mapping，data handle 指向逻辑原点；
checked 适配器同时接收存储范围，以最小和最大偏移验证可访问区间。
任意外部 strides 默认只读；可写性需由已知布局、合法切片或显式非重叠契约证明。
参见 [标准 layout_stride 条件](https://eel.is/c++draft/mdspan.layout.stride.cons)。

## 所有权和访问语义

owner 对 buffer 独立拥有并采用容器值语义：复制为深拷贝，移动按 allocator 契约转移。
clone 可显式表达复制成本；计算接口接收 view，且不隐式从 view 物化 owner。
这允许直接评估 xtensor 等已有 owner 的合适子集，而不预先为 move-only 限制重建容器。
allocator 是 owner 状态；复制和移动遵循 propagation 和相等性规则，不能总是假定可以窃取指针。
allocator 不允许传播且不相等时，移动可能需要重新分配和移动元素；noexcept 必须反映实际路径。
swap 只在允许传播或 allocator 相等时交换存储。

owner 负责部分构造失败时的清理。元数据校验在分配之前完成；异常与内存耗尽遵循
WJR 所选择的异常模式及 allocator 契约，不把任意 allocator 的 OOM 都承诺为可恢复状态。
原地写入已有对象时允许赋值抛出后保持有效但部分更新；需要强保证时显式求值到新 owner 再提交。

view 的复制和切片不分配、不增减引用计数，也不延长存储寿命。
元素 const 性由 `view<const T>` 表达；描述符自身为 const 不隐式使元素只读。
转换沿用所选 mdspan 的约束：可增加元素 const，不能移除；不能只凭 T* 可转换就接受派生类数组到基类数组。
动态 extent 转静态 extent 或收窄索引类型时显式转换并在 checked 入口校验，不能静默截断或改变 shape。
禁止从临时 owner 获取可逃逸的借用 view，例如在 owner 的右值上删除 view() 重载。
owner 扩容或被替换后，已有 view 失效。

静态 shape 的 moved-from owner 可以保留 shape 但失去 buffer；它必须可析构和重新赋值，
形成非空 view 或访问元素前需要重新获得存储。不能为制造“空 shape”而修改静态 extent。

索引参数个数由静态 rank 约束。普通索引在调试配置检查边界，checked 访问始终检查；
构造后的热点访问不重复执行完整 shape 校验。
reshape 只有在布局兼容且元素总数一致时产生 view；需要重排的数据通过显式物化操作生成 owner。

## 表达式与广播

表达式节点保存操作类型及闭包。左值 owner 和表达式借用引用，右值节点或 owner 按值保存并移动。
轻量 view 描述符可以按值保存，但底层 buffer 的借用义务仍然存在。
返回引用了局部左值 owner 的表达式仍然无效，右值闭包规则不会修复这种借用。
参照 [xtensor closure semantics](https://xtensor.readthedocs.io/en/latest/closure-semantics.html)。

编译期 shape 推导与运行时广播使用相同规则：从尾维对齐，两维相等或其中一维为 1 时兼容。
`0` 与 `1` 广播为 `0`，`0` 与大于 1 的维度不兼容，不能用简单 max 替代这套规则。
静态不兼容在编译期拒绝；动态不兼容在求值前拒绝。

广播是逻辑读取映射，可以用零步长表达重复读取，默认只读。
普通可写 view 不允许多个逻辑元素共享一个写入位置。
可写入口同时需要唯一映射、有效 address span 和存储边界；只证明其中一项不够。
广播 shape 和迭代计划在求值开始时确定，内层循环不逐元素重新解释 shape。

编译期优化包括常量 extent/stride 传播、去掉静态为 1 的维度、连续迭代专门化、
固定小形状的有限展开，以及逐元素表达式融合。大型静态 shape 不自动展开全部元素，
避免代码膨胀与编译时间失控。
这些优化不等于 constexpr 常量求值，也不自动产生可导出到 XLA 的图。

## 求值与别名

初始核心入口是对预分配输出求值的 eval_to(out, expression)，便利入口 eval(expression) 创建 owner。
view 赋值不改变其 shape。只有拥有输出的容器可以按显式约定调整容量。

求值前验证目标 shape、操作类型和广播，随后检查源与目标的存储关系。
仅在地址范围不相交或已证明同一逐元素映射时直接原地求值。
跨位移切片、转置、未知外部映射或 owner 调整容量可能覆盖输入时，先物化再提交。
为一份含有目标 owner 的表达式分配或重用输出之前，必须先保存仍被读取的输入。

显式 noalias 允许调用方承诺不重叠；普通 view 类型不默认携带 restrict 假设。
工作区由调用方或 evaluator 的一次执行上下文提供，不能把临时 buffer 的所有权塞入每个 view。

连续路径使用一维批量循环；通用路径使用预先建立的 strides 计划。
SIMD 是求值器的后端能力，不能成为 owner/view 的必要依赖。
矩阵乘等成熟运算优先考虑成熟后端，不把所有运算都强行实现成普通逐元素表达式。

默认浮点模式要求明确 NaN、缺失值、舍入和 reduction 语义，采用稳定的聚合算法；
不承诺不同 ISA 下自动逐位一致。允许重排或近似计算的 fast 模式显式选择，
不对整个集成库全局启用 fast-math。

## 互操作与设备边界

标准 mdspan 适配只覆盖映射能够表示的视图；负步长和零步长广播不隐式转换为标准 layout_stride。
NumPy、DLPack 的输入需要检查 dtype、shape、strides、地址空间、对齐和只读状态。
不兼容视图需要显式整理或报错，不能把复制隐藏在宣称零拷贝的接口中。

Arrow 的 offset、validity mask 与 release callback 由独立适配器处理，
缺失值不自动等同于浮点 NaN。导出为普通 tensor 时按需要同时提供数据和 mask。
Arrow 布尔位图等外部格式不能直接当作 bool 对象数组，必须显式转换或拒绝零拷贝。

Python 薄封装一次传递整块 view，并在调用或任务期间持有 Python owner。
热循环不创建 Python 对象；释放 GIL 时仍需保证输入不会被并发修改。

设备 buffer 由 PJRT 等运行时拥有并提供就绪/完成状态。
异步执行上下文保留需要的输入输出直到任务完成；host view 不可用于直接解引用设备指针。
同步适配器在调用期间持有外部 lease；异步提交必须转移或持有 lease，不能只排入裸 view。
失败或取消后，也要等运行时确认不再访问 buffer 才释放；发出取消请求本身不代表访问结束。
设备、流、事件和内存空间属于设备适配层，首版 CPU view 不为这些状态增加固定成本。
参见 [DLPack](https://github.com/dmlc/dlpack)、
[Arrow C Data Interface](https://arrow.apache.org/docs/format/CDataInterface.html) 和
[PJRT buffer 语义](https://openxla.org/xla/pjrt/cpp_api_overview)。

## 实施门槛

实现前先以最小原型比较所选 mdspan 和 xtensor 子集，验证编译器覆盖、类型尺寸、
借用规则、静态 shape 诊断与编译成本，再确定具体模板名称和依赖版本。
核心 data model 的验收包括：

- 默认 view 的句柄、mapping 和 accessor 均可平凡复制，构造、切片和 transpose 不分配，也无引用计数操作。
- 已知静态元数据不重复存储；类型尺寸与等价手写描述符比较。
- 等价索引/逐元素循环的生成码、向量化和分配行为与手写实现及候选库对照。
- rank-0、空维度、stride 溢出、非连续输入、别名和生命周期边界有独立行为测试。
- 同一组语义测试覆盖所有求值后端，不以更宽松的数值规则换取未声明的速度。

数据模型确认后，再依次实现 owner/view、预分配求值、有限广播和逐元素融合。
首先固定后端与编译器覆盖，比较 Container + mapping 和定制压缩存储的尺寸、语义及生成码；
再确定 owner 的具体模板参数和存储选择。其后补齐切片的 accessor 转换与空/负步长边界，
最后在有工作负载收益时引入 padded/aligned 特化，不要求第一版同时覆盖全部提案。
动态 rank、设备后端、XLA 图导出、自动微分、复杂索引及完整 NumPy API 属于后续单独设计范围。
