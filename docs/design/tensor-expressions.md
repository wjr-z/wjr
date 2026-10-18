# Tensor 表达式与求值设计

本设计在现有 owner/view、copy/transform 和 SIMD 原语上增加可组合的计算接口。
目标是让常见表达式达到与同等语义、同等 ISA 的较优手写内核接近的实际开销。
预分配求值不隐藏分配，普通节点不包含 shared_ptr、虚函数或运行时图。
本页解释结构选择；跨模块不变量和准确状态以[核心标准](np-core-standard.md)为准。
算术/广播/物化/cast/sum 已实现，API 见[表达式接口](tensor-expression-api.md)及其配套页面；
候选和后续能力在各节标出，不把现有实现称为全面零开销。

配套资料：

- [参考实现与原型证据](expression-evidence.md)：固定版本源码审查、已复现边界和当前性能限制。
- [分阶段实现计划](expression-roadmap.md)：8 组有依赖、示例与验收条件的后续能力。
- [可运行设计示例](../../examples/np/expression_design/README.md)：独立实验，未接入生产 target。
- [现有逐元素接口](tensor-elementwise.md)：当前可直接使用的 copy/fill/transform。

## 接口与职责

以下算术、eval_to 与 eval 已实现，保持 make_tensor、allocator 与异常配置约定：

```cpp
namespace np = wjr::np;

auto x = np::as_expr(x_owner.view());
auto y = np::as_expr(y_owner.view());
auto expr = alpha * x + beta * y;
np::eval_to(output.view(), expr); // 预分配，遵循可信输入前置条件。

auto result = np::eval(expr);    // expected<拥有结果的 tensor, shape_error>。

auto square_difference = [](auto input, double center) {
    auto d = np::as_expr(input) - center;
    return d * d;               // 局部描述符和节点不以引用逃逸。
};
np::eval_to(output.view(), square_difference(input.view(), 0.5));
```

| 层 | 职责 | 普通执行不需要的状态 |
| --- | --- | --- |
| owner/view | 存储寿命、extents、mapping、accessor | 表达式图、线程池、计算缓存 |
| expression | 标量运算语义、值闭包、静态 rank/shape 类型 | 每节点动态 shape 缓存、共享控制块 |
| binding | 一次确定输出域、广播投影、遍历与后端资格 | 逐元素 shape 校验、持久计划管理器 |
| executor | 读取叶子、应用运算、写出结果 | 每元素虚分发、隐式临时数组 |

保留 tensor_view 的 mdspan alias。alias 不会让 WJR 的 operator+ 自动参与 Kokkos 类型的 ADL；
as_expr 必须返回真正定义在 WJR 中的表达式类型。其关联命名空间提供受约束的运算符，
不向 Kokkos 注入重载，也不加入会捕获任意用户类型的全局运算符。
内部节点位于 detail 时，也必须确认公开 expression 包装的 ADL 实际有效。

第一版不提供 view 的表达式赋值：普通 view 赋值仍是描述符重绑定，
eval_to 才明确表示写入元素。owner 不隐式进入泛型 scalar 重载。

## 节点与闭包

初始内部协议包含表达式识别、value_type、静态 rank/extents，以及内部访问器。
map 集成增加私有 expression_children 的 arity/child 访问，统一元数据遍历；
数据叶与真实 callable 分别绑定，静态操作不占函数 slot。内建 unary/binary 热读仍可专门化，
不为形式统一牺牲已验证的 MSVC 自动向量化。
节点类型不承诺稳定 ABI，不要求模拟容器、保存 allocator 或暴露可写迭代器。

```cpp
template<class View> struct view_expression;
template<class T> struct scalar_expression; // rank 0，保留实际 T。
template<class Op, class Child> struct unary_expression;
template<class Op, class Left, class Right> struct binary_expression;
// map 候选使用 mapped_expression<Function, Children...>。
```

内建算术的 Op 是类型标签，不保存空操作实例。children 复用现有压缩存储，
但以各编译器实际 sizeof/alignof 为准；不能仅凭 EBO 或 no_unique_address 宣称任意树都无 padding。
用户 functor 则必须保存对象，保留其状态、构造/析构和 noexcept；is_empty 不是丢弃该对象的理由。

| 输入 | 默认闭包 | 可见语义 |
| --- | --- | --- |
| 算术 scalar 左/右值 | 按实际类型复制 | 后续修改原标量不改变已构造表达式 |
| 轻量 view 左/右值 | 描述符值快照 | 原 view 重绑定不影响表达式；数据仍借用 |
| 可复制 expr 左值 | 复制节点 | 可安全返回由局部节点构造的表达式 |
| expr 右值 | 移动节点 | 支持显式移动的非复制状态 |
| 不可复制 expr 左值 | 编译拒绝 | 不偷偷回退成借用节点 |
| owner | 初版显式左值 .view() | 不深拷贝，不隐式接管 owner |
| 临时 owner | 拒绝借用 | 不绕过当前已删除的右值 .view() |

快照保存的是描述符和系数，底层数组值仍在求值时读取。
owner 扩容、销毁或被替换会使表达式失效；值闭包不修复返回局部 owner 数据的悬垂。
复制包含引用捕获的 lambda 也不会深复制其被引用对象。

公开 eval_to 接收 const Expr&，执行边界不再次无条件复制整棵树。
绑定阶段可以复制小型 cursor 或在本次调用内借用不可变 functor 状态，
但不依据尚未解释的编译器问题随意改变公开闭包规则。

### 描述复用、局部计算与结果缓存

`auto q = a + b; q * q` 包含两个子树出现位置，不承诺构建 DAG 或自动缓存值。
递归重复组合可能使节点大小、编译时间和内联压力迅速增长。
编译器可能消除重复加载，不能据此承诺昂贵函数或规约只执行一次。

采用两种明确方式：在 pure map 的每元素 callable 中使用局部变量，
或者通过 eval/eval_to 显式物化可复用结果。
expression_ref、own、let、cache 分别涉及借用、所有权、词法绑定或结果失效规则，
不一次加入；只有实际构造/跨调用成本证明需要时再选一种扩展。
共享表达式描述不等于缓存计算结果。

## Shape 与一次绑定

rank 固定，extent 可以静态、动态或混合。广播从右对齐，缺失轴视为 1。
逐轴规则按顺序为：a==1 得 b；b==1 得 a；a==b 得 a；否则不兼容。
因此 0 与 1 得 0，0 与 0 得 0，0 与 2 拒绝，不能用 max 代替。

| 静态输入轴 | 推导的结果轴 | 必要的动态条件 |
| --- | --- | --- |
| 两个已知兼容轴 | 已知结果 | 无 |
| 两个已知不兼容轴 | 编译拒绝 | 无 |
| dynamic 与已知 N!=1 | 静态 N | dynamic 必须是 1 或 N，包括 N=0 |
| dynamic 与静态 1 | dynamic | 无额外扩展限制 |
| dynamic 与 dynamic | dynamic | 运行时应用同一规则 |

eval(e) 使用表达式的自然结果 shape。eval_to(out,e) 则采用赋值式广播到固定输出：
标量可以填充 [N,M]，[M] 可以扩为 [N,M]，[N,M] 不能通过丢轴写入 [M]。
不能仅验证输入彼此兼容，还必须保证最终域恰好适配输出且不改变其 shape。

对 eval_to，可直接将每个叶子的尾轴投影到输出域；不逐节点反复求动态 shape。
对 eval，先折叠自然 shape、验证存储大小并创建 owner，再绑定输出域。
一次 shape pass 加一次 binding pass 可以接受，不为省这点工作给每个节点增加 mutable 缓存。
显式 shape 查询可以返回固定长度数组，其代价为叶子出现次数乘 rank，不能在内循环反复调用。

绑定状态限于本次求值：一份输出域/遍历顺序、每叶子的指针或 accessor 状态、
有效步长及必要操作状态。不要同时保留整棵 AST 的所有 metadata 副本和重复 cursor 树。
只读广播的零步长属于执行元数据，不构造违反正步长唯一映射要求的 layout_stride。
第一版无需可重绑定的公共 plan 类型；持久计划的指针、shape、owner 失效规则另行设计。

rank 0 表示一个元素；任何零 extent 使执行域为空。
checked 边界仍需先处理元数据不兼容，执行阶段随后直接返回：不读取数据、不调用数值函数、
不预先读取广播标量，不对 nullptr 做偏移，也不提前计算无用的危险标量子树。
正常 C++ 捕获的构造行为不在这项空域抑制约定中。

## 访问与执行器

优先只支持当前普通算术元素、默认 accessor、left/right 和可用正 stride 的子集。
自定义 accessor、代理、volatile、负 stride、动态 rank 与设备句柄不隐式进入 raw-pointer 快路径。

执行分为兼容的连续线性流、可证明连续的内层行/列，以及通用固定 rank 的 stride 遍历。
输入各自连续不代表相同逻辑顺序；row-major 与 column-major 混合需要按坐标处理。
泛路径使用预绑定的步进/重置，避免每点除法、取模和重新解释 shape。
只在证明纯逐元素语义允许时选择有利输出布局的遍历顺序。

静态 stride 0/1 应留在类型或编译期投影中。动态广播不要为 L 个叶子展开 2^L 种内核；
初版保留清晰的标量 fallback，再为常见按行绑定、scalar splat 增加有限专门化。
动态 stride/load mode 若仍在内层产生工作，应计入成本，不能描述成已经完全外提。
大静态 shape 不通过模板展开所有元素，以免代码和编译时间失控。

### SIMD 是独立能力

标量表达式有效不要求 SIMD 有效。SIMD 初版限同类 float/double 的内建运算、
兼容 packed 输入流、标量 splat、完整 batch 与有界标量尾部。
同类要求覆盖算术节点的计算类型；float 数组配 double 常量并不自动属于同类 float 树，
未证明转换等价的组合先走标量，而不是仅检查叶子是否都是浮点数。
mixed/narrow-integer 子树、未知操作与不兼容布局先保持正确的标量路径。

不能把所有叶子转换为最终 batch dtype 再运算：这会改变中间整数除法等语义。
后续 mixed SIMD 必须解决逐节点类型与不同硬件 lane 数的转换域，
不是简单为每节点返回 batch<value_type> 就能完成。

`[M]` 行广播可以逐行重新绑定；`[N,1]` 沿内轴扩展需要 splat，不能普通连续 batch load。
确认非空后才准备被广播元素。初版使用 unaligned load/store，
以后对齐优化逐叶子证明，切片后不能继承未经验证的对齐承诺。
尾部不读取逻辑区间外的对象。

一/二输入流可复用现有 xsimd transform 原语；更多叶子直接使用小型 cursor executor，
不伪造索引输入数组，也不串联多个整数组 transform 造成中间结果。
采用独立可选 header/target 与明确 executor policy，不用每 TU 的后端宏改变同一个
baseline eval_to 特化定义，避免 ODR 和 ISA 要求不一致。
普通节点与 view 不持有 SIMD 寄存器、线程池或 ISA 状态。

## 输出、别名与内存

as_expr、eval_to 与 eval 已交付；下表中 checked/scratch 边界仍未实现，只有实际便利层需求出现后才推进，
不优先于可用函数和数值能力，也不改变可信 eval_to 的成本。

| 接口 | 分配与责任 |
| --- | --- |
| eval_to(out,e) | 无隐藏分配；有效 shape 与可直接求值的别名关系是可信输入前置条件 |
| eval(e) | 显式创建拥有结果；使用 expected 报告可恢复元数据错误，OOM 遵循 allocator 约定 |
| try_eval_to(out,e)，后续 | 写入前一次验证 shape/可证明的别名关系；无法证明安全时返回错误 |
| assign_to(out,e)，后续 | 安全便利入口，必要时显式允许临时存储，不调整 view shape |
| assign_to(out,e,scratch)，后续 | 使用调用方已构造、容量/shape 合适的 typed scratch，不在调用内分配 |

低级 eval_to 沿用现有 transform_to 的直接写入约定：每个叶子与输出逻辑存储不交，
或每个输出坐标读取的确实是同一个输出元素。坐标的所有 RHS 读取须先于该元素写入。
只比较 data_handle 或 shape 不足以证明同映射；extent 为 1 的轴才可忽略无效 stride。

- `out = 2*out + disjoint` 与 `out = out + out` 可以直接执行。
- 移位重叠、原地转置通常不能；普通前向标量循环也不会自动修复。
- 把输出的一行广播到所有行，可能在覆盖该行后污染后续输入。
- 借用 out[0] 的广播 scalar 不能依靠某个内核碰巧提前读取来获得安全承诺；
  在求值前复制出来的普通 scalar 值则独立且安全。
- 规约跨坐标读取，初版要求规约输入输出不交，不套用 pointwise 原地规则。

安全边界可以保守证明地址区间不交。stride bounding span 相交不等于逻辑元素相交，
无法证明时报告 unknown 或使用 scratch；不引入昂贵的通用集合求交算法。
无关指针不能直接相减，地址排序需采用明确、受支持的实现。
noalias 如以后加入，必须明确承诺输出与所有读叶子不交；它不允许原地别名，
也不自动等同于 restrict，不给普通 view 或每个输入强加该属性。

owner 的表达式赋值/扩容是独立能力：先保存仍需读取的 RHS，不能先 resize 清空自身再求值。
强异常保证需要临时求值成功后真正不抛出的提交；可能抛出的 scratch copy-back 只有部分更新保证。
eval_to 遇到操作异常允许部分输出已更新，noexcept 反映捕获、调用与赋值的真实行为。

当前 owner 默认复用 WJR vector，普通工厂仍值初始化；eval 显式使用 default_construct 工厂，
合法开始结果对象生命周期后写入。标准 allocator 和普通数值元素不要求先清零。
自定义 allocator 钩子仍保留，不向 vector capacity 中尚无对象的位置直接写入。
拥有结果的分配、元数据和异常路径成本须单独测量，不能由预分配原型推断。

## 数值、函数与规约

标量运算定义语义。逐节点从真实标量操作推导 value_type，遵循 C++ promotion，
不以整棵树的 common_type 替代。int/int+float 的除法保持整数语义；
窄整数可能先提升为 int。cast<T>(child) 在该节点之后转换，目标赋值转换只发生在根 store。
普通算术保留 C++ 的溢出/除零前置条件，不隐式承诺饱和、环绕或 NumPy promotion。

保留表达式括号与节点顺序，不主动重结合、改写 FMA、把 x*0 折叠为 0 或把 x/x 折叠为 1。
不向整个库添加 fast-math。调用方 flags 仍可能允许 contraction，
没有 fast-math 也不等于所有平台拥有严格相同的 fenv、NaN payload 或 errno 行为。
规约若采用不同顺序，也不能默认逐位一致。

map 集成保存 const-invocable、语义纯且本次执行配置稳定的真实 callable。
const 语法不能证明纯度；执行可能重排、重复或向量化，调用计数、日志、RNG 推进与输入修改不属于该合同。
现有 transform_to 的逐元素调用约定继续保留，不能因为新表达式层而悄悄收紧。
可移动不可复制 callable 需要显式移动其表达式；异常与 noexcept 条件如实传播。
所有 child 按自身 value_type 纯值传入，引用/void/proxy 返回被拒绝；捕获内存必须与输出逻辑存储不交。
绑定 const F* 消除求值复制，但不能保证捕获状态不重载；同 const F& 与手动外提字段是两个应记录的基准。

mask/选择已有独立候选，尚未包含在本页审查基线；状态见核心标准。
eager select 会计算两侧，不能充当除零或无效读取的惰性保护。
不使用重载 &&/|| 暗示标量短路；真正逐 lane 的惰性条件需要另一种执行能力。
默认 tensor_vector<bool> 已提供真实 bool 对象，可直接物化；std::vector<bool> 仍不满足 owner 要求。
map、比较、逻辑和 select 候选在显式 xsimd 请求下整体 scalar fallback，predicate batch 尚待独立实现。

### 规约是执行边界

已实现终结式 sum(expr) 与 sum_to(out,expr,axes<I>[,keepdims])，允许融合输入 pointwise 树，
不把 lazy reducer 当作普通逐点叶子。规约结果显式物化后，再作为 view 广播。
这避免 `(x-mean(x))` 在每个输出坐标重新聚合整个输入。

当前 axis 接口只接受一个编译期轴；固定长度 runtime 数组和多轴是后续能力。
任意 runtime 轴个数配合 keepdims=false 会改变 rank，超出当前固定 rank 模型。
若以后接收轴列表，空列表表示不规约，不能与全部轴混淆；重复/越界轴在执行前处理。
当前 keepdims 保留所选单轴为 singleton。

sum 当前累计类型按标量加法结果推导，可显式指定更宽累计类型；
更宽输出不代表中间自动更宽。全量按逻辑行主序、单轴按轴索引递增，从零累计。
空域 identity 为 0；pairwise 与 relaxed SIMD 是独立未来选择，不改变默认顺序。
min/max、mean、variance、norm 分别定义初始值、空域、NaN 和稳定性，
不在第一个泛型 reduce 中隐藏这些决策。SIMD/parallel 重排规约是明确的数值 policy。

```cpp
// 已实现接口：row_sums 为 [rows,1]，columns==0 先按数值合同处理。
np::sum_to(row_sums.view(), np::as_expr(input.view()), np::axes<1>, np::keepdims);
np::eval_to(centered.view(),
            np::as_expr(input.view()) - np::as_expr(row_sums.view()) / columns);
```

matmul、scan、排序、softmax 等同样需要独立执行/数值合同。
矩阵乘优先接入成熟 kernel，不让一个普通 element reader 每次访问偷偷做一次 dot product。

## 验收与后续扩展

使用真实 mdspan 叶子，而不仅是 span 原型，比较构造+求值和稳定重复求值。
代表用例包括 axpby、重复 sum-square、小静态 shape、行 bias、列 splat、转置 stride 输入、
mixed 整数除法，以及规约后广播。覆盖空/短/tail/中等/流式长度和交替、打乱的长度分布。

手写对照必须有相同分配、别名、dtype、浮点、ISA、线程和调用上下文，
explicit SIMD 对比 explicit SIMD。检查热循环调用、metadata 分支、加载与依赖链，
同时记录对象/plan 大小、跨非内联边界构造、编译时间与代码体积。
不要求机器码逐字相同，不把噪声内的小差异判成胜利或退化。
明显、稳定的吞吐退化必须解释或修复，不能只展示指令总数。

历史 span 原型曾不能恢复 MSVC 标量向量化；实际 mdspan 实现的局部绑定已恢复所测 axpby 路径。
重复叶子、复杂 map、通用 stride 的 setup/outlining 与捕获状态加载仍有缺口。
历史原型不能代替当前代码证据，现有有限探针也不足以承诺任意树的完整吞吐或编译成本。
噪声敏感的性能测试定期/手动运行；普通 CI 保留正确性、标量 fallback、编译与格式验证。

oneTBB 接收已经绑定、可独立写入的域，负责分块、grainsize、异常与嵌套并行。
小任务保持顺序路径，普通节点不保存 task/shared lifetime。
异步生命周期由独立执行上下文承担，不能把裸 view 当作寿命租约。

未来 XLA lowering 通过访问 leaf、内建操作、cast 和显式规约结构构造独立拥有的图，
不能把原始指针当作稳定图 identity。任意 C++ lambda、accessor 或数值 policy 不保证可导出。
IR 分配、DAG 规范化、编译缓存与设备 lease 位于适配边界，
未使用 exporter 时不增加普通 CPU AST 的字段或依赖。

## xtensor 取舍依据

审查以固定的 xtensor 0.27.1 / xtl 0.8.0 源码为准，具体定位与复现见 [证据](expression-evidence.md)。

| 领域 | 借鉴 | 调整或不直接复用 |
| --- | --- | --- |
| xexpression / closure | 可识别表达式、ADL、转发/寿命分工 | 默认值快照；不引入普通节点的共享状态 |
| xfunction | 操作数与运算分开，逐节点标量类型，独立 SIMD 能力 | 不复制每节点动态 shape 缓存，不以最终 batch dtype 改写内部语义 |
| xstrides / xshape | 右对齐、singleton 投影 | 自己统一静态/动态零维度规则，避免固定快照的 max 边界问题 |
| xsemantic / xnoalias | 安全赋值与直接赋值分开；当前普通赋值已有 overlap checker | eval_to 明确保留无隐分配；安全便利层另行实现 |
| xassign / stepper | 线性、内层 stride、通用遍历逐级选择 | 固定 rank、一次绑定；不照搬全部动态 rank 状态与迭代器外观 |
| xreducer | axis、keepdims、lazy/immediate 的不同成本 | 先 terminal 规约和显式物化，不依赖共享描述自动缓存结果 |

xtensor 并非零开销或最佳性能默认方案。上述机制的存在不构成 WJR 照搬理由；
具体选择仍与同语义最佳实用手写基准比较，不据有限尺寸/源码证据反推 WJR 整体更快。
mdspan 继续提供数据模型，表达式/求值器负责它没有规定的算术、广播、别名和调度语义。
