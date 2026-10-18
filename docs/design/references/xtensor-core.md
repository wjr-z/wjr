# xtensor 0.27.1 核心数据模型与表达式执行

本文是面向 WJR NP 核心设计审查的源码导读，结论只针对 xtensor 0.27.1 固定快照。
上游 tag/commit：[`0.27.1` / `f115228a7fc277b9be1bef34572831f8fd53ac73`](https://github.com/xtensor-stack/xtensor/tree/f115228a7fc277b9be1bef34572831f8fd53ac73)。
该版本将 C++20 设为默认语言要求；xtensor 使用 xtl 的闭包、序列和类型工具。
它适合作为 owner、表达式闭包、广播、分配和 SIMD 的设计案例，不等价于仅有 view 的 `mdspan`。xtensor 并非零开销，也不作为最佳性能或默认方案；验收采用同语义的最佳实用手写基准。本文源码结论不是 WJR 更快的证据。

## 1. 状态与读法

### 固定对象

- 稳定发布：xtensor 0.27.1，源码 commit `f115228a7fc277b9be1bef34572831f8fd53ac73`。
- 上游发布记录和变更：[`0.27.1 changelog`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/docs/source/changelog.rst)。
- 主要依据是该 commit 的头文件，不用 `master` 源码推断本版本行为。
- 官方文档便于理解 API；涉及异常、缓存、alias、stride 的判断以下面的 pinned 实现为准。
- 官方文档入口：[containers](https://xtensor.readthedocs.io/en/latest/container.html)、[lazy expressions](https://xtensor.readthedocs.io/en/latest/expression.html)、[closure semantics](https://xtensor.readthedocs.io/en/latest/closure-semantics.html)。
- 文档网页会更新；它们不是该发布的二进制兼容或性能承诺。
- xtensor 支持 CPU 多维数组运算，提供可选 xsimd 集成；本报告不把 xsimd 的能力等同为 xtensor 所有路径均向量化。
- 该固定版本的多数接口是 header-only 模板。
- 这意味着用户翻译单元承担模板实例化、优化和诊断成本。
- 源码树、公开接口和模板图需要按真实消费子集单独评估。

### 固定源码入口

下列链接均固定在上面的 commit：

- [`xarray.hpp`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/containers/xarray.hpp)
- [`xtensor.hpp`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/containers/xtensor.hpp)
- [`xfixed.hpp`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/containers/xfixed.hpp)
- [`xcontainer.hpp`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/containers/xcontainer.hpp)
- [`xstorage.hpp`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/containers/xstorage.hpp)
- [`xexpression.hpp`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/core/xexpression.hpp)
- [`xfunction.hpp`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/core/xfunction.hpp)
- [`xscalar.hpp`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/containers/xscalar.hpp)
- [`xassign.hpp`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/core/xassign.hpp)
- [`xsemantic.hpp`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/core/xsemantic.hpp)
- [`xutils.hpp`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/utils/xutils.hpp)
- [`xstrided_view.hpp`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/views/xstrided_view.hpp)
- [`xmanipulation.hpp`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/misc/xmanipulation.hpp)
- [`xreducer.hpp`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/reducers/xreducer.hpp)
- [`xtensor_simd.hpp`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/utils/xtensor_simd.hpp)

## 2. 类型关系：owner、表达式和视图

### 三种 owner 形状

`xarray<T, L, A>` 的 rank 是运行时数据。
它可按不同 rank 重设 shape。
其 shape、stride、backstride 使用可变长序列类型。默认 shape 为内嵌容量 4 的 svector，动态 rank 不代表每次都发生堆分配；超出内嵌容量与具体序列配置时才分析实际分配。
默认存储模板参数来自 `uvector<T, A>`，而不是 `std::vector<T>`。

配置头 [`xtensor_config.hpp`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/core/xtensor_config.hpp) 的 `XTENSOR_DEFAULT_DATA_CONTAINER(T,A)` 默认为 `uvector<T,A>`；`XTENSOR_DEFAULT_ALLOCATOR(T)` 选择 allocator，普通配置为 std::allocator，启用 xsimd 时为 aligned_allocator，另可启用 allocation tracking。

因此比较默认容器时要看 allocator/storage 的具体类型和实现。
不能只从 `xarray` 的公开拼写推出存储对象大小或初始化成本。
`xtensor<T, N, L, A>` 的 rank `N` 编译期固定。
它通常仍有运行时 shape；静态 rank 使 shape/stride 序列可使用固定容量表示。
对于默认 row-major/column-major 路径，相关序列可放在对象内，而无需 rank 动态增长。
`xtensor_fixed<T, xshape<I...>, L>` 的 rank 和每个 extent 都是类型信息。
底层 storage 用固定 extent 计算出的元素总数表示。

编译期 shape 使 strides/backstrides 可用静态序列计算。
它也可能把全部元素存为对象内的固定数组。
大静态对象会变成大栈对象或大嵌入成员；“静态”不等于廉价。
模板实例数、复制尺寸、栈/对象 footprint 和编译时间都随 shape 值和使用方式变化。
简表：

| 类型 | rank | shape | 默认数据 owner | 适合场景 | 主要代价 |
| --- | --- | --- | --- | --- | --- |
| `xarray` | 动态 | 动态 | `uvector` | 数据驱动 rank | 动态元数据；超出内嵌容量时可能分配 |
| `xtensor<T,N>` | 编译期固定 | 动态 | `uvector` | rank 稳定、extent 变化 | 每个 N 仍实例化一套类型 |
| `xtensor_fixed<T,xshape<...>>` | 编译期固定 | 编译期固定 | fixed storage | 小型固定张量 | 对象/栈尺寸、类型树和编译负担 |

三者均作为 `xexpression` 参与表达式。
容器对象提供非 const 与 const 的访问接口。
大多数共用的访问、shape 和 assignment 能力由 CRTP 基类实现。
`xcontainer<D>` 提供 expression/container 公共访问接口。
`xstrided_container<D>` 再提供 shape/stride/layout/resize/reshape 相关能力。
具体容器实现 constructors、value semantics 和 storage 连接。
这是代码复用的 CRTP 分层，不是虚函数运行时多态。

### 继承图与表达式协议

```text
xexpression<D>
  ├── xcontainer<D>
  │     ├── xstrided_container<D>
  │     │      ├── xarray_container<...>
  │     │      └── xtensor_container<...>
  │     └── xfixed_container<...>  // xtensor_fixed 的实现
  └── 表达式节点，例如 xfunction、xreducer、视图和 generator
```

上图省略 semantic/iterable/sharable 等辅助基类；xfixed_container 直接继承 xcontainer，不继承 xstrided_container。
节点通过静态多态暴露 `shape()`、`dimension()`、`size()`、`layout()`、`element()`、stepper 等接口。
抽象 expression 类型本身不拥有数据。
一个 node 可能存 operand closure，也可能包含自身状态，例如缓存 shape。xfunction 继承 xsharable_expression，其 p_shared 成员即使未启用共享也占表示空间；普通空 shared_ptr 不等于已经分配控制块或每次调用都进行原子计数。
“lazy expression”因此不必是零状态 descriptor。

## 3. Storage 和初始化

`uvector<T,A>` 是 xtensor 自己的连续分配 owner。
其目标包括连续元素访问、allocator 接口和不经 `std::vector` capacity 控制的存储管理。
在该版本的 `xstorage.hpp`，`safe_init_allocate` 根据 `T` 的平凡性选择初始化路径。
需要默认构造的非平凡类型逐个建立对象生命周期。
平凡类型的分配路径可以不先写入默认值。
value-initialized 的容器构造仍需履行其可见值初始化语义。
供表达式完整覆写的 `empty`/default initialization 路径是另一种语义入口。

不能把未初始化读取说成合法，也不能把“allocator 不先清零”推广到任意构造器。
完整覆写路径仍需要正确建立 `T` 对象的生命周期。
WJR 的异常构造必须销毁已成功构造的前缀并释放分配。固定源码的 safe_init_allocate 本身未见回滚守卫，不能由其名字或非平凡构造循环推断任意 throwing allocator/元素都有该保证；本次未另做异常复现。
这些要求与 C++ 容器的 allocator `construct`/`destroy` 行为相关。
若 WJR 使用专用 overwrite owner，应比较分配、对象建立和计算本身。

`std::vector<T>(n)` 再覆写每个元素通常额外做一次初始化写入。
合法 baseline 必须采用支持的 lifetime 语义，不能直接写 vector 的未构造 capacity。

## 4. Shape、stride、backstride 与 resize

`xstrided_container` 保存 shape、strides、backstrides 和 layout。
访问的线性偏移按 `sum(index[r] * stride[r])` 计算。
backstride 通常为 `stride[r] * (shape[r] - 1)`，用于迭代器递增/回绕。
backstrides 是迭代辅助元数据，并不是额外的可访问范围证明。
非 singleton 的常规 row-major 内维 stride 为 1，column-major 首维类似；adapt_strides 会把 singleton 轴的 stride 置零，不能照搬为标准 layout_stride 的参数。
静态布局能从 shape 推出规则 strides。
动态布局可保存运行时 stride，并从相应初始化或 reshape 路径建立状态。

`xcontainer::shape()` 暴露当前 shape 序列；表达式和 container API 可通过不同基类层次提供访问。
`resize(shape, layout, force)` 先比较 shape/layout，再按 `compute_strides` 重算 strides/backstrides。
当目标是动态 layout 时可以按所选 layout 派生紧凑 stride。
普通 resize 的数据容器大小由 `compute_size(shape)`（shape 元素乘积）确定。

这点十分关键：`compute_size` 是逻辑元素个数，不是任意带 padding stride 的 `required_span_size`。
在 0.27.1 的 `xcontainer.hpp` 约 1058 行，stride 显式 resize overload 要求 `D::static_layout == layout_type::dynamic`。
该 overload 会复制 shape，`adapt_strides` 更新 strides/backstrides，然后调用 `resize_data_container(storage, compute_size(shape))`。

因此该路径不能证明 arbitrary padded storage 被正确分配。
它也不能推出负 stride 可安全用于 owning container。
对负 stride 来说，首个逻辑元素通常不在 allocation 起始处，且寻址所需 min/max offset 需分开计算。
不能只计算正向的逻辑元素乘积。
如果 stride 有间隙，span 一般大于逻辑 size。
若 storage 只分配逻辑 size，使用带间隙 mapping 会访问越界。
若 stride 重叠，多个逻辑索引可能写同一物理元素。

所以 `xarray` 支持运行时 shape 和 strided metadata，不等于任意外部 stride 都是安全 owner。
外部 buffer adaptor/view 与 owning container 的责任不同。
创建 strided view 的调用者须满足它访问的实际内存范围有效。
empty shape、rank 0、extent 0 需要分开考虑。
rank-0 的逻辑元素数通常为 1。
任何维度 extent 为 0 时逻辑元素数为 0。
空对象不能因 backstride 公式中的 `extent - 1` 造成无符号误读。
检查每个实现 helper 的零 extent 分支和乘法溢出行为。

### Resize 与 reshape 的语义差别

resize 可能改变逻辑元素数量并扩展/缩减 storage。
重新分配后既有 view/iterator 的借用关系不可再当作有效。
保持相同 shape 时可以避免分配，但是否保留原值由 API overload/force 语义决定。
reshape 要求元素总数兼容，并依目标 layout 重建索引解释。
reshape 不等于转置，也不保证复制数据。
若 reshape 能保留表达式的线性布局，它可维持底层 storage 不变。
要求重排时应明确生成新容器或调用 transpose/copy 路径。

`resize(shape, strides)` 的存在不可脱离其 static-layout 和 storage-size 实现约束解读。
不要通过某个 strided constructor 推断任意 padding/negative stride owner 一定正确。

## 5. Expression CRTP、闭包和值类别

xtensor 的运算符通常生成 `xfunction<F, CT...>`。
`F` 是 elementwise function object 类型。
`CT...` 是每个 operand 的 closure type。
这个 tuple 存储孩子节点/表达式对象。
节点把运算树的类型编码在模板参数中，并在元素读取时递归调用孩子。
`xexpression<D>` 提供 CRTP 入口以约束参与表达式操作的类型。
表达式组合在编译期决定节点树、value type 和可用优化路径。
这没有构造可单独保存、可调度或可跨调用执行的动态图。

### closure 规则

固定版本使用 `xtl::closure_type`、`const_closure_type` 和 xexpression-specific `xclosure`。
传入左值表达式时一般存引用。
传入右值表达式时存按值 closure，并可移动临时对象。
这样可以让 `(a + b) * c` 中的中间临时节点活到最外层表达式。
它也意味着保存 `auto e = a + b;` 不会复制 `a`、`b` 的数据。
只要 `a`、`b` 仍然存活且其 buffer 没有失效，`e` 才能安全读取。
函数若返回依赖局部 lvalue 的 lazy expression，局部对象离开作用域后会悬垂。

官方 closure 文档明确提示：需要返回 lazy 结果时应正确 move/forward 局部表达式，或调用 `eval` 物化。
借用 lvalue 是延迟计算的便利机制，不是生命周期所有权系统。
更细的陷阱是 scalar wrapping。
scalar operand 会由 `xscalar<closure>` 包装成 rank-0 expression。
一般规则可能借用 scalar lvalue；若 scalar 在后续被改写，延迟表达式读取的是当下 closure 所指对象。

`const_xclosure` 对 scalar 包装使用 xtl::const_closure_type；固定 xtl 0.8.0 对左值仍保存 const 引用，对右值保存值。const 前缀并不代表自动 scalar 快照。
不要仅凭“标量一般很小”假设所有 scalar operand 都立即复制。
应用需要稳定值语义时，可将标量显式复制到表达式，或把 expression 生命周期限制在原始标量有效期间。

### 可重用 expression 与临时陷阱

同一个具名表达式被两个父节点引用时，closure 可存对该表达式节点的引用。
临时表达式被组合时，应由右值 closure 拥有它。
重复绑定、从 helper 返回、转发同一节点多次时，需核对传递的是 lvalue 还是 rvalue。
诸如 `auto e = x + y; return e * e;` 中，返回节点若引用局部 `e` 将悬垂。
可返回 `std::move(e) * e` 的行为需谨慎，因为同一个对象一边移动一边被引用会引发求值顺序和状态问题。
更稳妥的是显式让返回树拥有各闭包，或在边界 eval。
表达式会在访问或 assignment 时再次读取源；源若在两次求值之间改变，结果也会改变。

惰性表达式缓存的是计算规则和 closures，不缓存元素结果。

## 6. xfunction、类型提升、标量和广播

`xfunction` 保存函数对象及 operand tuple。
默认 elementwise 函数是静态函数对象，编译器可内联。
`value_type` 由函数对孩子 `value_type` 的调用结果推导。
不同运算有各自的 C++ arithmetic conversion 语义。
这并不自动等同于 NumPy dtype promotion 的所有细节。
用户自定义 operator、窄整数运算和目标容器 value type 会影响实际转换位置。
在 WJR 比较中必须写清“叶子类型 → 运算类型 → 写入类型”的每一步。
将整个表达式最后 cast 与每个输入先 cast 不是同一运算。
广播沿尾部维度对齐。

缺失的前导维度按 extent 1 处理。
每一轴两边 extent 相等或其中之一为 1 时可广播。
extent 1 的操作数按该维复用同一值。
例：`(2,3)` 和 `(3,)` 得 `(2,3)`；`(2,1,4)` 和 `(3,4)` 得 `(2,3,4)`。
错误兼容的动态形状在实际 broadcast/assignment 边界检测。
静态 extent 可以拒绝一部分永远不兼容的组合。
运行时 shape 仍需要运行时验证。
scalar 的 rank-0 closure 会广播到数组的所有位置。
标准化后的 shape 可由表达式 `shape()` 暴露。

`xfunction` 为广播操作准备缓存，其中包含当前 shape 和 triviality 信息。
在该固定实现中，`xfunction` 的 cache 是 mutable 成员。
运行时 shape 缓存首次计算会写入；fixed_shape 的 cache specialization 使用静态信息，不统一承担相同动态缓存行为。
因此同一个 expression 对象被多个线程首次或并发求 shape/evaluate 时，不应假定 cache 是无数据竞争的纯 const 读取。
线程安全不可由 C++ 的 `const` 限定自动推出。
可在发布前单线程建立缓存，但这是否足够取决于所有后续可变入口。

更稳妥的并发方式是每个线程持有独立 expression object，或在 evaluator 外部预建不可变 metadata。
需要核对具体版本的 cache 初始化、失效和 `reuse_cache` 调用约定。

## 7. Stepper、linear path 与 assignment

xtensor assignment 并非一个固定的递归索引循环。
`xassign` 根据布局、线性可分配性、broadcast、数据类型和 SIMD 可用性选择路径。
trivial assignment 通常表示左右两侧可按同一一维线性顺序读取/写入。
满足时可用 data/buffer index loop。
xsimd 可用且类型/布局支持时走 SIMD assigner。
无法 vectorize 但仍 linear 时退回 iterator/data loop。
无法视为 linear 时使用 expression stepper。

stepper 将多维索引状态推进到每个孩子，并处理较低 rank operand 的广播重复。
stepper 机制保留通用 layout 能力，但其状态和每维分支可能比连续路径成本更高。
不能只看输出都是一个 loop 来断定两路径生成相同机器码。
线性 path 的关键条件包含 RHS 可线性赋值、shape 可广播成目标、布局有兼容序列。
axis order 和 contiguous 数据会显著影响向量化机会。
不规则 view/strides 可能落到 stepper 或不适合 SIMD 的通路。
实际是否优化需看 xsimd 配置、节点 `load_simd` 能力、alignment 和 tail。

### Resize 和 assign 的相互作用

对 owner 赋 expression 会处理输出 shape。
目标容器可能 resize，然后运行 RHS assignment。
对不可 resize 的 view assignment 则要求目标 shape 能接受 RHS broadcast。
为了支持表达式赋值到数组且不破坏 RHS alias，semantic assignment 可先 materialize temporary。
这可能引入一次 allocation 和完整临时写入；提交可能移动 storage 或逐元素复制，取决于 owner/view 的 assign_temporary 路径，不能统一计作再复制一次。

该成本属于 assignment contract 的 alias/shape 安全保护，而不是 lazy AST 本身。
同尺寸、无 alias、linear path 下是否能绕过临时要按当前具体 operator path 检查。
`xtensor` 官方 assignment 页面指出：linear contiguous assignment 与不规则 stepper 是两类路径，SIMD 是可选优化。

### alias 与 noalias

固定版本普通表达式赋值在 xsemantic_base::operator= 中使用 overlap checker；判为可能重叠时物化，否则直接 assign。启用 XTENSOR_FORCE_TEMPORARY_MEMORY_IN_ASSIGNMENTS 会强制临时，不能把该配置推广到默认所有赋值。
例如将参与 RHS 的数组 resize 为新 shape，可能在 RHS 仍待读取时覆盖或搬动数据。
临时结果能保护这种情形，但会有额外内存/复制。
`noalias(lhs) = rhs` 显式承诺目标不会与 RHS alias。
实现可以据此跳过某些临时创建。
错误承诺会让结果依赖遍历顺序或读取被覆盖数据。

noalias 是程序员的语义承诺，不是自动 alias analysis。
WJR 当前 eval_to/transform_to 有自己的逐索引原地合同；通用安全 assign_to 仍未实现。
不能未经证明把它扩成 transpose、shifted overlap、非唯一 mapping 或 arbitrary expression 的全方位 noalias。

## 8. Views 与步长边界

xtensor views 通常也是 lazy expressions，并保存 closure 到源表达式。
view 不一定拥有独立 buffer。
按值捕获右值 owning container 时可持有其资源；借用左值 owner 或非持有 adaptor 时不延长外部 buffer 寿命。必须检查闭包最终拥有的对象，不能统一说 view 永不拥有或总能延寿。
`view`/`xview` 的关闭类型仍受 lvalue/rvalue closure 规则影响。

### reshape 和 transpose

transpose 是维度置换和 stride 置换的视图表达。
对于常规 dense storage，它不一定移动数据。
返回类型可保留原 source closure，而索引元数据按置换映射。
reshape 可用线性顺序重解释 shape，但并不意味着每个 strided 输入都能无复制 reshape。
布局不兼容时可能要求表达式或拷贝路径。

### strided view 和 signed stride

view 机制能表示一般子视图、切片、反转等情形。
反向切片的 stride 可能是负值，起始 pointer/offset 会随切片起点调整。
要安全使用必须有 allocation 下界和上界证据，而不只是逻辑元素数量。
构造视图时的 descriptor 语义不代表任意输入都被验证。
broadcast view 可以让某一维的地址保持不变，形成零 stride。
这种映射对只读读取可有意义。
可写零 stride view 会使多个逻辑位置写同一物理元素，需禁止或明确定义顺序。
重叠 stride 也可能破坏 uniqueness 与 SIMD store 假设。

判断每个 view helper 的 index/stride 类型是否带符号。
某些 container stride 类型会采用无符号或布局内部约定；不能把它与 view stride 类型混为一谈。
再强调：0.27.1 `xcontainer` 的 stride resize overload 受 `static_layout == dynamic` 约束，并分配 `compute_size(shape)`。
这不涵盖“allocation 从逻辑起点延展到所有正负偏移”的完整 signed-stride owner。

对 WJR 通用 API，C++ mdspan `layout_stride` 是不同约束集，标准要求正 stride 和 uniqueness。
xtensor view 能表达更广的操作并不意味着 WJR 的 Kokkos mdspan alias 能无损转换。

## 9. Reducers、eval 与重复子表达式

`xt::sum` 等 reducer 构造 `xreducer`。
reducer 的 operand closure 可借用原输入/表达式。
轴选择、keep-dims 和 initial value 会影响 reducer shape 和 accumulator 类型。
reducer 支持被当作 expression 继续参与计算。
因此 `sum(x + y)` 仍可能在最终使用时遍历数据并读取两个输入。
`eval(expr)` 触发表达式求值。
对 xexpression，它通常返回新建的 evaluated container。
对已经是合适容器的输入，eval 可返回引用/避免重复物化。

编译期表达式没有隐式记忆上次结果。
将一个复杂 `auto e = ...` 用两次，通常会重新计算两次。
若后续都要复用中间值，可显式 `eval(e)` 到 temporary。
这让使用者在内存与计算之间做 tradeoff。
构造 `x + x` 是否重复 load 取决于 evaluator、compiler CSE 和 aliasing 可见性。
不得假设树自动做 common-subexpression elimination。
默认 sum 的数值累加顺序取决于具体 reducer/遍历路径。
SIMD 或并行 reduction 可能改变浮点结合顺序。

如果 contract 要确定性或逐项一致，必须固定 reduction algorithm 和 execution policy。

## 10. SIMD 与尾部处理

xtensor 可与 xsimd 集成。
`xfunction` 可在支持时对孩子 batch load，再调用 SIMD-aware function。
是否支持取决于函数实现、operand 类型、布局、stride 和 evaluator path。
通用自定义 functor 不应被假设有 SIMD overload。
没有可用 batch implementation 时，应存在 scalar `operator()` 路径。
批量主循环之外还需要处理长度不能整除 batch width 的尾部。

tail 可以用 scalar remainder，也可以用 masked load/store，取决于具体 backend 和安全合同。
未对齐指针和不满一个 batch 的剩余元素会约束 load/store 方法。
masked load 并不能自动使越界 pointer arithmetic 合法。
所有 batch 分支还要覆盖 rank-0 scalar broadcast、非 unit stride 和混合 dtype。
固定版本的某些 vectorizer 要求能从 expression data buffer 建立 index loop。
复杂 reducer/stride expression 不一定进入该路径。

检查 `load_simd` 和 `stepper` 实际选择，不要用“开了 xsimd”替代路径证据。

## 11. 成本模型、限制与性能评价

### 表达式成本

常规 fused expression 的理想成本是一遍读所需输入、一遍写输出，并在每元素执行必要算术。
这避免 intermediate array allocation 和 memory traffic。
但还会有表达式 object 构造/复制、closure、shape 查询、layout 分支和 loop setup。
小表达式重复使用可能让闭包引用、缓存状态和重复计算高于显式 materialize 的方案。
节点越深，类型名、重载解析、内联和优化器分析的树也越深。
树形表达式往往会被完全内联，但超长表达式可能拉长编译时间、增加代码体积和 instruction-cache 压力。

把表达式缓存到 `auto` 只缓存树，不缓存数值。

### Container 成本

动态 rank 需要可变长 shape/stride 序列；默认内嵌容量降低常见 rank 的堆分配，但仍有对象尺寸和分支成本。
动态布局和 arbitrary stride 需要保存更多 per-axis metadata。
静态 rank/shape 可把元数据或数据变成对象内数组，但对象 size 和模板数上升。
`uvector` 避免普通 vector capacity 语义中的某些空间冗余；它仍有分配和 allocator 行为。
不能只用“一个 allocation”断定 owner 和 WJR 相同快。
测量 allocation 次数/字节、初始化写入、复制、缓存局部性及重复构造/求值。

### xsimd 成本

xsimd 需要正确 CPU profile 和目标指令集。
通用 header 不得因实例化 SIMD 路径而要求整个消费者统一使用过强 ISA。
比较 baseline scalar 与明确 ISA target，保留 scalar fallback。
测试对齐、尾部、NaN、Inf、signed zero、精确 cast 边界和 exception/fenv 假设。

### 编译和 API 成本

表达式重载生态是生产力优势，也扩大了候选 overload 和诊断路径。
常用头文件会拉入 views、reducers、builders 和 traits 的模板定义。
选用子集仍需要实际编译文件时间、peak memory、binary size 和 error diagnostics 样本。
依赖范围、C++ 标准、xtl 版本和 ABI 要纳入稳定性评估。
基于公开 API 的适配器可以隔离升级变化。
不要把实验表达式内部 node 类型直接作为 WJR 公共 ABI。

### 异常语义

容器分配、allocator 构造和 expression closure copy/move 都可能抛出。
固定版 `xfunction` constructor 在头文件中为无条件 `noexcept` 声明/定义。
这与“任意用户 callable 的复制/移动可抛异常”并非普遍兼容。
若 WJR 自己存任意 functor，必须让构造函数 `noexcept` 由各成员真实操作条件决定。
SIMD intrinsic 本身通常不是 C++ exception 来源，但其调用的用户函数、memory accessor 或 checked boundary 可以有不同语义。

不应把整个 evaluator 一律 `noexcept` 来获得小幅优化。

## 12. 与 WJR 当前 NP 的对应

审查快照：此文档写作时专用 worktree base `39fb36bae1f937493657f49cb8075b707389e63f`。
此 commit 当前 `include/wjr/np` 已有 tensor owner/view、广播表达式、cast、assignment、reduction/scan 以及可选 xsimd 后端。
另审 map 152260e5 与 mask d9194269 候选，但不计入此基线。当前状态统一见[核心标准](../np-core-standard.md)。

### 相似点

WJR 使用真实 mdspan descriptor 做借用 view，节点把 descriptor/value closure 存在模板树中。
输入数据不因为表达式而被拷贝。
静态 rank/extents 在类型中可用于静态 shape 合并。
求值时以 bound input metadata 和紧凑循环完成融合计算。
默认 owner 的 fully-overwritten construction 避免不必要的零初始化写入。
WJR 把 ISA 相关 `xsimd` 能力留在显式可选后端/target 分层。

### 有意不同

WJR view descriptor 是 Kokkos `mdspan` alias，遵循其 mapping/accessor 合同。
WJR 不试图复制 xtensor 的 `xexpression` 通用 CRTP interface、stepper API 或全部 view 种类。
当前 WJR expression 支持 arithmetic numeric 子集，并将 node traversal、shape binding 和 evaluator 分开。
WJR 当前 owner 具备独占存储，不靠 expression lvalue closure 拥有 owner。

WJR 的 view 描述符与可复制 lvalue 节点按值快照，rvalue 节点移动；底层 buffer 仍借用。它与 xtensor 的 lvalue 节点引用是有意不同的合同。
WJR 目标是 GCC/Clang 性能优先，并维持 MSVC 可用且合理。
xtensor 的广泛 API/实现复杂度不应因相似 C++ 类型而整体移植。

### 可借鉴

- 把 owner、借用 view、lazy expression 和 evaluator 分开建模。
- 显式区分闭包拥有与借用；WJR 继续值快照，不照搬 lvalue 节点引用。
- 把 scalar 包成 rank-0 operand，并统一广播语义。
- 以连续 linear assignment 与通用 stepper assignment 区分不同成本路径。
- 用 conservative SIMD capability gate，再保留 scalar loop 和 tail。
- 让 materialization 作为明确决策，解决 alias 和复用问题。
- 用真实源代码核对 stride layout 和 allocation length。
- 在自定义 callable 节点中保存真实 F 对象，并审查构造异常规格。

### 不宜照搬

- 不要复制 `xfunction` mutable shape cache 而未解决并发读写。
- 不要照搬无条件 `noexcept` 后再支持 potentially-throwing F。
- 不要将 lvalue closure 的借用误说成生命周期保护。
- 不要把 xtensor 的所有动态 stride view 都换成 `std::layout_stride`。
- 不要将 strided owner 的 `compute_size(shape)` 当作 `required_span_size`。
- 不要自动引入可能多一次分配和复制的 alias temporary 到 WJR pointwise assignment。
- 不要承诺表达式自动做公共子表达式缓存或稳定 SIMD reduction 顺序。
- 不要为统一 API 引入 WJR 不需要的动态 rank、所有 view 操作或通用代理元素。
- 不要以小对象/no allocation 单项指标替代完整相同语义比较。

## 13. WJR 取舍与验收问题

本次核心审查已把闭包、dtype、alias、map、规约与 SIMD 的决定收敛到[核心标准](../np-core-standard.md)。不复制 xtensor 的共享基类、mutable shape cache 或通用 stepper 外观，也不把已有机制本身当作最佳方案。

| 成本来源 | 源码可确定的机制 | WJR 的判断与仍需证据 |
| --- | --- | --- |
| shape/stride/backstride/layout | 一般容器保留多份轴元数据；动态 rank 可超过内嵌容量 | packed mapping 保留必要 extents；通用步进状态在本次 binding，量测 setup 与反复求值 |
| 闭包/共享/cache | 左值引用、右值拥有、shared_ptr 字段与 mutable cache | 保留值闭包及无共享普通节点；其复制/大树成本仍需对照 |
| 临时/alias | 默认 checker 按路径选择直接或临时；noalias 是承诺 | eval_to 保持 trusted，eval 显式物化；比较时匹配检查与临时合同 |
| allocation/init | uvector 平凡路径可不写初值；不等于任意容器都零初始化 | 与合法 default-init baseline 比，不与不等价的 vector 值初始化混比 |
| layout/iteration | 线性、strided SIMD 与 stepper 条件不同 | 逐布局检查实际路径；单次 loop 不代表元数据已消除 |
| compile/tree | 模板结构与闭包决定实例化、重复子树与内联 | 分别测 2/4/8/16 叶子、非内联 F、构造和代码尺寸 |

当前没有本批重新测量的 xtensor/WJR 吞吐数据。先前复现的静态零 extent、mixed SIMD dtype 和 reducer 组合问题见[证据](../expression-evidence.md)，仅对固定快照成立。未来比较须匹配中间 dtype、系数快照、FP contraction、别名、初始化和目标 ISA；不能让一个实现承担另一实现被省去的语义。

## 14. 参考索引

### 官方文档

- [Container overview](https://xtensor.readthedocs.io/en/latest/container.html)
- [Container API index](https://xtensor.readthedocs.io/en/latest/api/container_index.html)
- [Expressions and lazy evaluation](https://xtensor.readthedocs.io/en/latest/expression.html)
- [Closure semantics](https://xtensor.readthedocs.io/en/latest/closure-semantics.html)
- [Views](https://xtensor.readthedocs.io/en/latest/view.html)
- [Assignment](https://xtensor.readthedocs.io/en/stable/developer/assignment.html)
- [Aliasing and noalias](https://xtensor.readthedocs.io/en/latest/container.html#aliasing-and-temporaries)
- [Version 0.27.1 changelog](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/docs/source/changelog.rst)

### 实现章节建议顺序

1. `xtensor_config.hpp`: 默认 allocator/storage 选择。
2. `xstorage.hpp`: `uvector` allocation、初始化和元素生命期。
3. `xcontainer.hpp`: shape/stride/layout、resize overload 和 storage size。
4. `xarray.hpp`、`xtensor.hpp`、`xfixed.hpp`: owner 类型组装。
5. `xutils.hpp` 与 `xexpression.hpp`: CRTP 和 closure traits。
6. `xscalar.hpp`、`xfunction.hpp`: scalar wrapping、缓存 shape 和 elementwise node。
7. `xassign.hpp`、`xsemantic.hpp`: trivial/stepper、temporary 和 assignment alias。
8. views headers: slice closure、stride 和 mapping。
9. `xreducer.hpp`: reducer shape/stepper/aggregation。
10. `xtensor_simd.hpp` 和 xfunction SIMD overload: supported batch path。

本 brief 已针对 pinned 0.27.1 的 `xcontainer.hpp` 检查了 stride resize 的 static-layout 条件、stride/backstride 重算和 `compute_size(shape)` 分配。
本次进一步直接下载固定 commit 头文件，核对 config、storage、container、closure、function、semantic 和 assignment 路径；修正了截断 commit 与若干旧目录链接。
其他算法和 compiler assembly 没有在本研究中执行独立基准。
