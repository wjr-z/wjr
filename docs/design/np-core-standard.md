# WJR NP 核心标准

本文规定 WJR 多维数据与计算层的共同契约，适用于 C++20 的实验性 `wjr::np`。
目标是通用基础库：owner 管理资源，view 描述借用，静态表达式描述逐点计算，执行器选择循环。
金融研究、模型训练和 Python 都是使用者；它们不决定核心的数据类型、布局或生命周期。

## 状态与文档权威

审查日期 2026-10-18，详细源码审查从 `39fb36ba` 开始，并核对随后合入的 map、mask 和 stride owner。
首次规范集成基于 `21521c98` 与[构造入口迁移 #84](https://github.com/wjr-z/wjr/pull/84)。
以下能力清单已按 `97486826`（#111）重新核对；原审查基线不代表当前完成状态。
“已实现”只表示文档声明的子集；审查、生成码和实际测试是不同证据，不能相互替代。

| 状态 | 已确认范围 |
| --- | --- |
| 已实现 | 固定 mdspan view、连续及正 stride owner、真实 bool、default_construct、有限子视图/转置/reshape、预分配逐元素计算 |
| 已实现 | 值闭包算术、静态/动态广播、cast、eval_to、拥有结果的 eval、可选 xsimd、view sum/scan、表达式全量/单轴 ordered sum |
| 已实现 | map 的真实 callable、独立函数绑定、条件 noexcept、私有 child 协议；比较、具名逻辑和 eager select |
| 已实现 | 可选 try_make_* 函数使用 make_view.hpp 并返回普通 mdspan；可信内部直接构造，分配元数据处理留在内部 |
| 已实现 | 正步长 slice_view、静态布局信息保留、padded 适配；有限具名数值函数及 min/max/clip 表达式 |
| 已实现 | numeric/predicate xsimd、广播行与 scalar fallback；私有按操作特化的资格和计算适配 |
| 已实现 | view pairwise sum、stable norm、mean/variance、extrema、NaN-skipping full-view 规约、diff_to；表达式 scan 与全量/单轴 mean |
| 已实现 | 保留既定分组与 child dtype 的受限连续表达式 pairwise sum；setup、重复叶等成本边界见其 API 文档 |
| 已实现 | 独立可选 OpenBLAS LP64 dot/gemm_to，普通 np 不搜索或链接 BLAS |
| 接受的后续方向 | 按实测缺口优化绑定和别名信息；扩展明确数值 policy 的融合规约与显式成熟后端边界 |
| 未承诺实现 | signed-stride 借用、exact-storage 替换、公共 plan、通用 alias 求交、动态 rank 内核、调度器、运行时图与设备执行 |

本文对跨模块的不变量和取舍具有优先级；[模块索引](../modules/README.md)说明依赖，
各 API 文档说明实际签名、已支持子集与失败行为。[表达式设计](tensor-expressions.md)说明内部结构，
[实施计划](expression-roadmap.md)安排后续能力，[证据](expression-evidence.md)保留带快照的实验结果。
[下一阶段能力](np-next-capabilities.md)记录本轮集中审视后的实现分组和准入条件。
发生冲突时须修正文档或代码，不能通过把未来规则改称“已实现”消除差异。
实验性节点表示和后端对象布局不承诺稳定 ABI；新增契约若改变可见行为，必须随代码、针对性测试一起交付。

## 1. 参考设计与性能标尺

复用固定 Kokkos mdspan 的 extents、mapping、accessor；不重写同等标准设施。
后端 pin 为 `0e4eba59ad53451784b3839bdf98b25fa2d64931`，由
[`src/np/CMakeLists.txt`](../../src/np/CMakeLists.txt)管理。同一程序只使用一套后端类型与宏配置，
C++23 翻译单元不能自动换成另一套 `std::mdspan`。标准库互操作以后通过显式适配器完成。

[xtensor 参考](references/xtensor-core.md)提供闭包、广播、遍历和临时管理案例，
[WG21 参考](references/open-std-multidimensional-core.md)提供多维映射与访问器的标准边界。
xtensor 并非零开销，也不是性能上限或默认最佳选择；其动态 rank、backstrides、shape cache、
共享表达式和安全赋值服务更宽的功能，不能成为 WJR 引入同等状态的理由。
反过来，功能更少、对象更小或少一次分配也不证明 WJR 更快。

验收基准是同等语义的最佳实用手写实现，必要时再加入成熟库的对应路径。
先固定 dtype/中间转换、FP 顺序、alias、初始化、异常、ISA 和调用边界，再比较：

- 构造及首次求值、重复求值、跨非内联调用、分配加物化分别计时。
- 统计必要读写、遍历次数、状态加载、分支、初始化、容量和 allocator 元数据。
- 检查对象/绑定尺寸、helper 调用、代码体积、编译时间和模板实例化负担。
- 使用空/短/tail/大输入、动态和静态 shape、连续/广播/有孔布局、混合长度分布。

“源码确定”“生成码观察”“计时实测”“设计推测”分开记录。指令数不等于吞吐，
Windows x64 证据不冒充 Linux/SysV 结果。GCC/Clang 是主要优化目标；MSVC 保持正确和合理性能，
显著稳定退化必须调查。任何仍存成本都应解释或保留为缺口，不能由“零开销”命名消除。

## 2. 数据模型与合法地址域

rank 是类型信息，extents 可全静态、混合或全动态；动态值使用固定 rank 的元数据。
不为常规 view/AST 引入动态 rank 容器、运行时 dtype、设备号或共享控制块。
`index_type` 可为标准允许的 signed/unsigned integer；外部整数在收窄前验证非负和可表示性。

| 概念 | 定义与责任 |
| --- | --- |
| 逻辑 size | extents 的数学乘积；rank 0 为 1，任一零 extent 为 0 |
| required span | mapping 从 handle 原点到所需最大 offset 的覆盖长度；有孔映射可大于 size |
| storage_size | owner 容器内已开始生命周期的对象数 |
| capacity / allocation count | 容器为释放或增长保留的分配信息；不能当作已存在对象数 |

例如 shape `(2,3)`、strides `(4,1)` 的逻辑 size 为 6，required span 为 7，offset 3 是孔洞。
默认 accessor 需要完整覆盖七个对象的存储，计算只访问六个逻辑对象；不要求最后一行之后额外的 padding。
mapping 的合法性、存储跨度、对象生命周期、别名安全是四个不同证明。

### 标准正 stride 子集

`layout_left/right` 表示规则 packed 布局。`layout_stride` 只接收严格正且满足后端构造条件的 strides，
包括存在轴排列 `p`，使相邻轴满足 `s[p[i]] >= s[p[i-1]] * extent[p[i-1]]`。
调用方在构造前满足这个前提；需要分配存储的 owner 工厂用无溢出的比较验证，不能先构造非法 mapping 再检查。
这是 WJR 选择的有序非重叠子集，不是所有数学上单射的 stride 映射：
shape `(2,2)`、strides `(2,3)` 的 offsets 为 `{0,2,3,5}`，仍不属于此构造子集。

zero-stride 广播只存在于表达式的只读投影，不伪造 `layout_stride`；普通输出必须唯一。
negative stride 不可转成 unsigned stride 后传给后端。若后续需要 reverse/NumPy 适配，
使用独立 signed descriptor：handle 指向逻辑原点，checked 边界同时知道 allocation 范围，
先计算最小/最大偏移再形成指针；默认只读，可写性需要另行证明。
此方向尚不授权向现有 raw-pointer executor 添加每元素 signed/unsigned 分支。

### 可信构造与分配边界

直接 view 构造、索引和 `eval_to` 使用可信输入前置条件；沿用现有断言层级。
可信 view 直接使用 tensor_view(data, extents/mapping)。不维护 checked_xxx_view 数据结构、第二套 view 类型
或 trusted/unchecked 类型对。可选 try_make_tensor_view/try_make_strided_tensor_view 是报告构造错误的薄函数，
集中在[中性构造头](../../include/wjr/np/make_view.hpp)，成功返回同一个 mdspan alias；不增加状态或访问检查。
已有内部可信调用直接构造，不绕公共工厂；不因可恢复构造函数存在就要求所有调用支付其检查成本。

owner/eval 创建存储前仍需保证 shape、静态匹配、stride/span、字节跨度和 allocator 上限不会造成错误分配。
这些内部 helper 可供可选构造函数复用，但不成为每次借用必经的通用服务；shape_result 服务实际需要报告元数据错误的入口。
typed pointer 的有效对象、自然对齐、范围与寿命由调用方保证，库不探测地址。
未来 foreign-buffer 适配若确需检查 dtype/地址空间，应在该真实边界做一次，不反向增加普通 view 的开销。

含零 extent 不能豁免后端观察器会执行的中间算术；调用方须满足这些前提，owner 的分配检查匹配固定实现的 prefix/suffix 与 span 运算。
rank 0 需要一个有效对象，空 view 可以使用 null。空执行域在绑定数据前返回，不读广播源、不调用 map、
不计算危险标量子树，也不对 null 做指针加法。普通 C++ 参数求值和闭包构造仍会发生。
不在每个索引、节点读取或 SIMD lane 重复完整校验；必要诊断不能增加重复的成功路径工作。

## 3. Owner、初始化与借用

默认 owner 保留 `Container + mapping_holder`，使用 `tensor_vector<T>`。
allocator、data/end/capacity 由容器管理，外层不重复保存。全静态 packed mapping 可压缩；
全静态 extents 的 stride owner 仍必须保存运行时 strides，不能根据 rank_dynamic==0 把 mapping 丢掉。
空 allocator/策略可以压缩，但实际对象仍存在，状态、析构、同类型子对象规则和 ABI 限制必须保留。

完整 owner 的通用不变量是 `storage_size() == mapping.required_span_size()`；packed 时恰好等于逻辑 size。
正 stride owner 的 make/adopt 必须显式接收 shape 和 strides，首批不提供默认/shape-only 构造或 array 存储。
adopt 精确检查 span，容量可以更大；元数据失败不得提前移动输入容器。
普通静态 packed owner 的默认构造和显式 array 支持继续保留。

普通创建值初始化；完整覆写使用显式 `default_construct` 开始合法生命周期。
对普通数值与标准 allocator 可省去初值写入，非平凡构造、allocator hooks 和异常清理仍必须发生。
不能写入未构造的 vector capacity。拥有结果的 `eval` 创建 packed 结果，计算完成后全部逻辑元素可读。

有孔 owner 的 default_construct 覆盖整个 span，但孔洞可能保持不确定值。
只写逻辑元素不足以授权随后读取整个容器；复制/赋值/不等 allocator 的元素移动可能读取孔洞。
调用此类操作前须让其读取范围内的对象具有合法值。逻辑值独立副本使用
`eval(as_expr(owner.view()))`，只读逻辑域并生成 packed owner。不要为回避这个显式合同而默认清零孔洞，
也不能把容器级复制擅自改成只复制逻辑点并声称未改变容器语义。

owner 深拷贝与 allocator propagation 服从容器。赋值先操作容器，成功后无抛提交 mapping；
失败或移动后允许 mapping 有效、存储不完整的表示。size 仍是逻辑大小，has_storage/try_view 判断可借用性。
不能为制造“空 shape”重置静态 extents，也不向通用 vector 强加 NP 专用移动后元数据清空。
noexcept 根据实际容器/成员行为推导，不能把可能抛出的分配或元素构造变成 terminate。

构造优先让 RAII 成员承担资源；构造失败会销毁已完成的成员/基类，不调用完整对象的析构函数。
只有资源尚未交给成员时才需要局部清理 guard，不另建通用事务或 try_make<T> 分发框架。
static create 返回 expected 可以服务实际可恢复构造需求，但本次不增加该 API 或替换现有命名工厂。

view 的复制不分配、不增减引用计数、不延长 buffer 生命周期。
`const view<T>` 是浅 const，元素只读使用 `view<const T>`。
owner 只通过左值 `view()/try_view()` 借用；右值借用和返回元数据引用均拒绝。
owner 销毁、替换或可能搬迁存储使借用失效；删除右值重载不能发现返回局部 owner 数据等所有悬垂。
异步/Python 边界另持有 lease，常规 view 不因此增字段。

默认 vector 三指针及 capacity slack 是真实成本。小 owner 大量存储时可研究 opt-in exact storage，
但 count、失败状态、分配器释放长度和 allocator 传播不可凭空省略。
默认替换需尺寸、分配字节、构造/移动和实际小对象工作负载证据，并通过现有 allocator/异常测试。

## 4. 切片与布局证据

当前 API 只有 packed 二维 row/column、二维 transpose、packed flatten/reshape；
不能因固定后端提供 submdspan 就宣称 WJR 已支持任意切片。
接受的下一步是受限通用正步长切片的薄入口，复用后端 mapping/offset 计算及可信参数前置条件。
整数缩轴、full extent、半开范围和正 step 分别定义边界与输出静态信息，先覆盖当前默认 accessor 子集。

保留能静态证明的 packed/padded/unit-inner-stride 信息；不要一律退化为运行时 layout_stride，
也不要只凭源布局保持标签。一般 accessor 的新 handle 必须经 accessor.offset，结果采用 offset_policy。
aligned accessor 经任意偏移通常需降级；每行首地址和 pitch 都满足条件才可使用对齐 kernel。
当前草案有 padded layouts 与 aligned_accessor，采用它们仍须单独核验固定后端和目标 STL。

reshape 只重解释兼容的遍历顺序且逻辑大小相等；转置不等于 reshape。
非连续输入若需要重排，必须显式物化，不隐藏 payload 复制。
零长度切片、尾端空范围和 singleton 轴要有独立测试，不能只测试非空矩阵内部切块。

## 5. 表达式值与数值语义

保留静态值树，不改成运行时 DAG 或借用具名表达式的默认闭包。

| 输入 | 存储与后续可见性 |
| --- | --- |
| scalar | 实际算术类型的值快照；原系数改变不影响树 |
| view | 只读描述符值快照；重新绑定原 descriptor 不影响树，底层元素仍在求值时读取 |
| 可复制左值节点 | 复制子树；允许返回含局部轻量节点的表达式 |
| 右值节点 | 移动子树；move-only 状态要求显式转移 |
| 不可复制左值节点 | 编译拒绝，不隐式借用 |
| owner | 显式通过有效左值 view 接入，不自动接管临时 owner |

每个操作独立推导 value_type，遵循真实 C++ promotion。
`cast<double>(a/b)` 保留整数除法；`cast<double>(a)/cast<double>(b)` 可改变它。
输出类型只在最终 store 转换。默认不承诺 NumPy promotion、饱和、符号整数环绕或自动范围检查。
保留节点括号，不主动重结合、改写 FMA、把 x*0/x/x 化简；调用方编译选项仍可能改变 FP 行为。

广播从尾轴对齐，缺轴为 1；每轴按相等或一方为 1 合并，0 和 1 得 0，0 和 N>1 不兼容。
静态已知冲突编译拒绝；dynamic 与已知 N!=1 可得到静态 N，但仍须检查动态值是 1 或 N。
`eval` 使用自然 shape；`eval_to` 可扩展到固定输出，不能删除前导轴。
布局偏好只决定新结果的 packed 布局，不证明每个叶子能沿同一线性顺序读取。

`q*q` 有两个子树出现位置，不保证 CSE、调用次数或结果缓存。
每元素复用优先写在 map 内的局部变量；跨数组操作复用显式 eval 到工作区。
只有构造/编译/调用成本证明需要，才考虑一种小型引用或局部绑定扩展；不同时引入 let/cache/own/graph。

### Pure map 与 mask 契约

map 存真实 `decay_t<F>`，通过 `const F&` 对每个 child 的自身 value_type 纯值调用。
结果须是非引用 arithmetic 值；void/proxy/reference 在移除 cv 前拒绝。
构造可以有可见特殊成员行为，空 F 不能替换为每元素的 `F{}`；求值不复制或移动 F。
所有包含 map 的算术、cast、select 构造及读路径传播实际 noexcept。

纯度是调用方语义要求，const-invocable 只是语法条件。
调用不依赖次数、次序、参数地址或外部副作用；捕获配置和被引用内存需稳定、有效且与输出逻辑存储不交。
显式数据叶子仍可精确逐点原地，隐藏捕获不能用这个例外读取输出。
map 内的 C++ if 可以保护其内部计算，但 child 在调用前已经求值，不能保护 child 中的非法除法。

比较输出真实 bool；具名 logical_not/and/or 只接受 bool 值，不重载 &&/||。
select 的三棵子树参与广播，两分支 eager 求值；结果按 C++ 条件表达式的分支类型转换。
用条件选择保留选中值，不能替换为 mask*a+(1-mask)*b；这会改变 NaN/Inf/带符号零行为。
select 不承诺短路保护，两分支在整个执行域都必须有效。

## 6. 绑定、alias 与执行

保留每次调用的自动存储期 binding。shape/layout 判定在边界，内层只做必要数据读取和操作。
私有 child 协议统一 arity/child_type/child access；不把它冻结为对任意第三方节点开放的公共 ABI。
数据绑定存指针和 scalar 快照；函数绑定独立存 `const F*`，没有 stride，按编译期偏移访问。
内建静态操作不占函数 slot。对已证实影响向量化的热路径允许直接 unary/binary reader，
元数据遍历仍使用统一结构，不能为了形式统一接受明显退化。

优先兼容线性流，其次有证据的连续内层与 splat，再固定 rank 的通用步进。
不同 packed 布局不等于共同线性顺序。只读有效步长可为 0，输出步长不得因广播而变成重复写。
通用路径避免每点除模和完整 shape 查询；动态 stride、重复树、行绑定和 helper outlining 的剩余工作要实测。
不为 L 个叶子枚举全部 2^L load-mode 组合，不按每个静态元素展开大型数组。

| 边界 | 合同 |
| --- | --- |
| eval_to | 不隐藏分配或扫描 alias；shape/存储有效，读叶子不交或精确逐点同对象 |
| eval | 显式新建独立 packed owner；元数据错误返回 shape_error，分配/调用异常按真实路径传播 |
| sum_to | 输出与所有读叶子及捕获读取内存不交；不套用 pointwise 原地规则 |
| 未来安全赋值 | 独立 checked/scratch 入口；未知 overlap 可保守拒绝，不能改变 eval_to 的成本合同 |

精确原地要求每个输出坐标先完成全部 RHS 读取再 store；移位、转置、输出行广播回全矩阵不满足。
相同 data_handle 或 span 相交都不足以证明具体读写关系。
不在普通 view 添加 restrict，不把语义纯度等价成编译器已知的 noalias。
仅为便利提前增加通用 overlap 分类器或自动临时不属于本阶段优先工作。

`eval_to` 调用异常允许此前输出已更新；拥有结果的 eval 负责清理结果存储。
当前 axis sum 在一条规约线完成后提交；可能抛出的路径若改为跨线交错，必须保持或显式修订该保证。
并发读取 AST 不需要内部 mutable shape cache，但调用方仍须保证源数据、callable 配置和独立输出无数据竞争。

## 7. 规约、扫描与 SIMD

sum 是 terminal 操作，可以融合 pointwise 输入，但不是每次 read 都重新聚合的廉价节点。
全量默认按逻辑行主序从零逐项累计；单轴按该轴索引递增，各输出独立。
accumulator 默认由结果类型相加推导，显式 Acc 改变累计类型，不反向改变 child 类型。
空 sum 为零，rank-0 是从零加一个值；不能承诺负零输入必然原样返回。
多轴 runtime 个数与空轴列表尚未提供。已实现的
[mean](tensor-mean.md)、[variance](tensor-variance.md)、[extrema](tensor-extrema.md)
使用各自的空域、NaN、tie、累计类型与失败前置条件，不由 sum 的规则自动推导。

独立列交错可保持每列原顺序；同一结果分成多 accumulator 再合并则改变分组。
已有 view 和表达式四列路径不能重复列为未来功能；表达式路径用于 unit/splat、允许交错且不抛的子集。
默认不因某台机器的宽块计时改成 8/16，也不依据 SIMD lane 数选择通用最优宽度。

[view pairwise](tensor-pairwise-sum.md) 已固定分组、cutoff、空域和布局子集；
不支持的布局不能偷偷回退成不同分组并仍称确定。
[融合表达式入口](tensor-expression-pairwise.md)保留同一树与 child dtype，首版有明确的布局/操作准入。
relaxed SIMD 仍为后续扩展，须明确准入和 fallback 合同。
relaxed 只授权声明的累计重结合，不授权移动 cast、改写整数除法或全部 fast-math。
精度对照包含消去、极端量级、NaN/Inf/带符号零；pairwise 不保证每个输入都更准，更不等于正确舍入。
scan 还需保持每个 prefix 的累计类型，不能通过读回较窄输出继续累计。

SIMD 是可选 executor 能力。baseline `wjr::np` 不依赖 ISA；xsimd 使用独立 header/target/policy。
同一种公开 inline 特化不能按各 TU 的 ISA 宏产生不同语义或定义。
未知 callable、cast 或 mixed dtype 子树保守 scalar fallback；不能先升成最终 batch dtype 再运算。
map、cast、mixed dtype 和不支持的布局仍整体 fallback。
现有 predicate SIMD 区分 numeric batch 与 predicate batch，匹配 lane 几何且保持 mask 后端私有；
同类型 float/double 的比较、逻辑、eager select 和 min/max/clip 可进入既有批量路径。
不实例化 batch<bool> 冒充真实 bool 存储，不泄漏 native mask bit 编码。
完整 batch 加有界 tail 不越界读取；初始指针对齐不能替代切片/每行对齐证明。
显式 backend 不代表每个表达式都可 vectorize，合法 fallback 仍需正确性和成本证据。

## 8. 扩展准入与交付

有限具名数值函数、ordered collective、stable norm 和独立可选
[CBLAS 边界](tensor-cblas.md) 已落地；后续只按新用途或性能缺口扩展，不重复建设或复制整个 ufunc 目录。
min/max 的空域、NaN、相等值与带符号零沿用各自已声明合同；stable norm 不能用平方和在极端量级溢出的实现冒名。
softmax 显式分出 reduction 和复用 workspace，matmul 优先适配成熟 kernel。

oneTBB、OpenXLA、Python 和设备互操作在独立适配层；不用它们时普通节点无额外状态或依赖。
任意 lambda 不承诺可导出图，裸地址不是稳定 graph identity；异步任务持有 lease 并等待实际访问完成再释放。
这些边界保留扩展空间，不要求现在实现 scheduler、编译缓存或 ownership framework。

每组交付必须包含可用 API、针对性行为测试、可运行例子与诚实的成本说明。
本次不删除 try_make/try_row/try_column/try_reshape 函数 API；调整中性头名与内部依赖时同步迁移 includes 和例子。
数据模型改变验证 rank-0/空域/溢出/移动失败，闭包改变验证真实对象与异常，数值优化匹配 FP 合同。
性能结论记录精确源码、编译器、flags、CPU/ISA、调用边界和未测项目。
既有 Windows 两项 CI 保持；新增标准/ISA 组合优先 Linux。文档-only 更新核验两平台 paths-ignore，
不为文档修改重跑无关全套构建。具体下一批能力与依赖见[实施计划](expression-roadmap.md)。
