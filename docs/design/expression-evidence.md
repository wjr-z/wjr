# 表达式原型与参考实现证据

这是 [表达式架构](tensor-expressions.md) 的带快照验收输入；统一契约见[核心标准](np-core-standard.md)。
下文前半记录 2026-10-17/18 的早期 span 原型，不代表当前生产实现仍缺少已经合入的功能。
可运行原型位于 [examples/np/expression_design](../../examples/np/expression_design/README.md)。
原型只处理 double 和已经绑定的遍历域，尚不包含完整 shape 推导、任意广播、
重叠规划、规约或图编译。

## 本地测量范围

2026-10-17/18，Windows x64，Intel Core i5-13500HX：

| 工具链 | 标准与主要选项 | 已运行的范围 |
| --- | --- | --- |
| Clang 18.1.8，MSVC STL | C++20，`-O3 -DNDEBUG`，默认 x64 ISA | 语义、分配计数、生成码、显式 xsimd、探索性计时 |
| GCC 12.1，MinGW | C++20，`-O3 -DNDEBUG`，默认 x64 ISA | 独立语义、分配计数、生成码、显式 xsimd |
| MSVC 19.42 | C++20，`/O2 /Ob2 /Zc:preprocessor /DNDEBUG`，默认 x64 ISA | 语义、分配计数、生成码、显式 xsimd、探索性计时 |

Clang 另运行 C++23、C++20 ASan 和禁异常示例。
禁异常配置关闭包含抛出式分配计数器的测试目标，只构建示例与代码生成对象。
ASan 使用自己的分配运算符，因此分配计数与 ASan 分开运行。

原型的输入/输出预分配，包含长度 0–512、固定长度 4、合法返回的局部表达式、
重复子树、0–7 行与 0–32 列的行广播，以及逻辑末端正好是分配末端的小缓冲区。
三个普通工具链下，所测表达式构造与求值区间的分配计数均为 0；
显式测试缓冲区分配与打印不在计数区间。这不证明未来任意用户 functor 不会分配。

## 有限成功与未通过的门槛

| 路径 | 当前事实 | 可以得出的结论 |
| --- | --- | --- |
| Clang/GCC 标量 AST，axpby、重复子树、固定小数组 | 与对应手写内核的静态指令数相同；无额外 helper call | 所测内联树可以消除结构性调用开销；指令调度与寄存器仍可能不同 |
| Clang/GCC 按行绑定广播 | 比手写多 1–2 条静态指令 | 元数据外提有效，但仍需检查新增指令所在路径 |
| MSVC 标量 AST | 手写循环自动向量化，AST 循环没有；向量器报告 reason 1200 | 尚未达到手写内核的验收要求；指令数更少不能被解释为更快 |
| 显式 xsimd AST | 三工具链均生成批量循环；原型和直接 xsimd 的设置、尾部及寄存器调度存在差异 | 显式 SIMD 是可控路径，但不能由此宣称整体零开销 |

MSVC 的 [reason 1200](https://learn.microsoft.com/en-us/cpp/error-messages/tool-errors/vectorizer-and-parallelizer-messages)
表示无法证明不存在循环携带的数据依赖。分别把内核参数从 const 引用改为值、
把 children 从 compressed_pair 改为 std::pair、以及把运算替换为强制内联的类型标签，
都没有单独恢复该原型的自动向量化。目前未定位唯一根因，不据此改变公开闭包语义，
也不对普通原地求值入口偷偷添加 restrict。

所测节点表示：静态 span 叶子 8 字节、动态 span 叶子 16 字节，
双输入 axpby 树 48 字节，三工具链相同。运算标签没有实例字段，children 使用已有
compressed_pair。这是这个实验的尺寸，不是 mdspan、任意树或跨调用 ABI 的保证。

Google Benchmark 1.9.0 的探索性运行覆盖 4、300、262144 元素，以及八种长度交替输入，
使用 real time、每项 7 次重复、每次最少 0.02 秒。未固定 CPU 亲和性/频率，
没有硬件分支或缓存计数器，部分大数组结果变异系数接近 10%。
显式 SIMD 的 axpby 有接近手写的样本，也存在重复子树路径明显慢于手写的样本。
这些数据用于暴露问题，不作为吞吐量验收或跨机器收益结论。
后续正式比较需增加测量时长、随机交错和稳定机器上的独立重复，且统一语义、ISA 与调用上下文。

## 当前实现与历史证据的关系

基线 39fb36ba 已有真实 mdspan 值表达式、广播/stride、eval、cast、表达式 sum 与可选 SIMD。
原型 MSVC reason 1200 不能继续作为生产 axpby 的现状：实际绑定路径已恢复所测自动向量化，
但重复叶子的 alias guards、复杂树的内联预算、通用布局 setup 与 helper 调用仍有剩余成本。
拥有结果的标准数值 eval 已使用 default_construct 消除所测初始化清零；view sum/scan 的四列路径已实现。
这些结论的详细范围分别见实际 API 文档，不能用历史负面结果覆盖后来修复。

本次核心审查重新读取 map 152260e5、mask d9194269 与正 stride owner 工作副本。
map 的本地/显式 scalar 配置可更容易外提；跨非内联 AST 的 const F* 仍可能产生状态加载和别名保护。
这类生成码观察不等于吞吐；比较必须同时包含同 const F& 和手工外提字段的基准。
既有宽列实验全部在 Windows x64，8/16 宽块有收益也有尾段/寄存器退化，不能作为 Linux 默认宽度证据。
本次未重新运行这些计时、硬件计数器或编译矩阵；新能力须保存自己的精确快照和实测范围。

构造/复用基准还必须匹配已捕获系数的快照和 FP contraction 边界。
把手写版本的活系数地址暴露给输出别名、或让一方形成 FMA 而另一方保留乘加，都不是公平对照。
当前基准应按实际 expression closure 检查，而非沿用看起来类似的源代码。

## xtensor 固定版本审查

参考 xtensor **0.27.1**，提交
[`f115228a7fc277b9be1bef34572831f8fd53ac73`](https://github.com/xtensor-stack/xtensor/tree/f115228a7fc277b9be1bef34572831f8fd53ac73)，
xtl 0.8.0，提交 `ef84e9f27020ad54961c99acd293d80d4b775dd3`。
下列结果只针对所列快照与组合，不推及所有 xtensor 版本。

### 节点表示与闭包

[`xsharable_expression`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/core/xexpression.hpp#L85-L103)
内置 shared_ptr 字段；未启用共享表达式时可以为空，但字段仍属于对象表示。
[`xfunction`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/core/xfunction.hpp#L398-L400)
另有操作数、functor 与 shape 缓存。它们服务更广的共享/动态 shape 接口，
不能简单断言没有价值；WJR 的轻量值树不直接继承这组状态。

xtensor/xtl 的闭包会借用部分左值，包括左值标量。
例如创建 `a + scale` 后改变 scale，惰性表达式可能读取新值。
WJR 已采用标量值快照，这是可见语义差异，需要明确说明。
值捕获描述符与轻量节点能避免引用函数内局部节点，但不会延长底层数组寿命。

### 静态广播的零维度

该快照的固定 shape promotion 在已测用例中把 `[0]` 与 `[1]` 推为 `[1]`，
把 `[0]` 与 `[2]` 推为 `[2]`；运行时广播对前者得到 `[0]`。
WJR 必须逐轴应用相等/单维扩展规则：`0` 与 `1` 得 `0`，`0` 与 `2` 不兼容，
不能直接复用取 max 的固定 shape 算法。
参见 [`xshape.hpp`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/core/xshape.hpp)。

### 规约共享不等于结果缓存

用合法的逐坐标访问统计 2×3 输入的读取：同一个三元素 sum 访问两次会读源 6 次；
逐元素计算 `input(i,j) - sums(i,0)` 总共读取 24 次；
先物化 sum，再减法共读取 12 次。三工具链与 ASan 均验证这一计数路径。
共享表达式对象本身并没有把规约结果自动缓存。
因此 collective 操作必须有执行边界，不能直接作为廉价 pointwise 节点逐元素递归调用。

另一个独立的版本兼容性用例中：

```cpp
xt::xtensor<double, 2> a = {{1., 2., 3.}, {4., 5., 6.}};
auto sums = xt::sum(a, {1}, xt::keep_dims);
xt::xtensor<double, 2> out = a - sums;
```

该固定快照在本地 Clang 18 ASan 下报告 reducer stepper 的越界读取；
先物化 sums 后的路径正常。这个失败与上面的合法计数实验分开处理，
不使用有未定义行为的自动广播路径作性能依据。
WJR 应针对 keepdims、轴投影、空轴与广播规约分别验证，而不是直接照抄 reducer/stepper 组合。

### 混合 dtype 的 batch 与 tail

同一表达式必须定义中间节点的运算类型，不能只定义最终输出类型。
该快照在 xsimd 13.2.0（提交 `1f8dd9c8e162968d9b4ff0251c56d431b8777f36`）
和 14.3.0（提交 `e88a72831858123924f7118f345dfe5d70d95991`）下均出现如下结果：

```cpp
// numerator/denominator 是九个 int32 的 3/2，offset 是九个 float 的 0。
auto e = numerator / denominator + offset;
xt::xtensor<float, 1> result = e;
```

Clang 18、GCC 12、MSVC 19.42：标量读取 `e(0)` 为 1；启用 SIMD 后，
批量前缀为 1.5，标量尾部为 1；先物化整数除法则全为 1。
参见 [`requested_value_type`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/core/xassign.hpp#L434-L436)
与 [`load_simd_impl`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/core/xfunction.hpp#L907-L909)。
这要求 WJR 保留逐节点 promotion/cast 语义；尚未证明等价的混合类型子树先走标量路径。

### 赋值与遍历分层

xtensor 并非性能上限；以下机制既有功能价值，也可能增加 WJR 当前子集不需要的成本。
普通 xtensor 表达式赋值已有
[`overlap checker`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/core/xsemantic.hpp#L622-L642)，
不能描述成所有赋值都会物化。noalias 绕过相关安全处理，也不等于结果缓存。
[`xassign`](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/core/xassign.hpp#L450-L476)
区分兼容线性遍历、strided SIMD 内循环与通用 stepper；每个输入连续，
并不能证明它们具有相同逻辑遍历顺序。单元素广播、切片后对齐和混合类型准入
都需要独立能力判断。这些分工值得借鉴，具体状态表示与快路径需要重新测量。
