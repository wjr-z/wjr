# 标准 scalar math 表达式

`<wjr/np/expression_math.hpp>` 提供 `np::sqrt(x)`、`np::exp(x)`、`np::log(x)`, `np::expm1(x)`, `np::log1p(x)`、
`np::abs(x)`、`np::isfinite(x)`、`np::isnan(x)`、`np::isinf(x)`，
其中 `x` 是现有 `as_expr` 可捕获的 arithmetic scalar、view 或表达式。
owner 仍需显式传入存活的 view；不添加新的 checked 入口。

每个无状态 unary 节点直接对其子节点的值调用同名 `std` 函数。
结果类型和读取的 `noexcept` 来自实际标准库重载，而非统一指定为输入或输出 dtype。
例如整数 `sqrt` 得 double，float 的 `exp` 保留 float；
`sqrt(a/b)` 的整数除法仍先发生，想改变除法需写 `sqrt(cast<double>(a)/b)`。
`abs` 只接受有符号整数或浮点输入，拒绝 bool 和所有 unsigned 类型，
即使某个窄 unsigned 能经整型提升调用 `std::abs`。
窄 signed 输入可提升成 int；若绝对值不能由实际标准重载的返回类型表示，
例如 `std::abs(INT_MIN)`，仍是调用前置条件，不增加逐点检查。

节点构造不读取元素，按实际子树 copy/move 条件声明 `noexcept`；
它保留子节点的 extents、layout、值闭包及借用生命周期，没有 callable state 或函数 slot。
有状态或 move-only map 子树仍由现有协议处理，读取异常正常传播。
广播、正步长、空域、rank 0、逐点原位和别名规则沿用 `eval_to`。
空执行域不调用数学函数，rank 0 执行一次。
`constexpr` 允许常量构造节点；数学求值能否成为常量表达式取决于所用标准库和编译器，
不承诺 C++20 下 `cmath` 的一般常量求值。

## Small arguments

`np::expm1(x)` and `np::log1p(x)` call the actual `std::expm1` and
`std::log1p` overloads, rather than forming `exp(x)-1` or `log(1+x)` nodes.
The nodes avoid explicitly forming those cancellation-prone intermediate
values; actual accuracy remains a property of the standard library. The example
shows `double` input `1e-12` and `float` input `1e-8f`; it does not establish
correctly rounded results or an accuracy bound for other implementations or
inputs.

Integer inputs produce the standard overload's `double` result; `float` stays
`float`, even when the destination is `double`. Child integer division still
happens before the mathematical call. Signed zeros, nonfinite inputs, overflow,
and `log1p`'s pole/domain cases (`x == -1`, `x < -1`) follow the same standard
function's result, errno and floating-point behavior. No checks or error-state
reset are added. The existing C++20 constant-evaluation limitation and scalar
fallback/interleaving restrictions also apply to these nodes.

Windows x64 Clang 18 and GCC 12 paired probes keep the same standard calls and
inner-loop work as direct handwritten loops. The tested fixed-shape reused
trees have equal instruction counts. GCC's dynamic local-tree wrappers retain
six additional static instructions from a duplicated empty-return epilogue;
this is a code-size difference, not a measured throughput result.

## 数值与运算域

本模块使用实际标准库的准确度、NaN、无穷、带符号零、domain/range、errno 和浮点环境行为，
不替换近似 exp/log、不清除错误状态、不启用 fast-math，也不把 domain error 改成 shape error。
调用方仍需选择适合任务的运算域和编译选项。
不额外承诺 NaN payload、信号时序或不同后端间完全一致的浮点环境表现。

`select` 是 eager，不能用它保护非法分支；`map` 的子树也会在 callback 前求值。
需要真正的 scalar guard 时，应把原始输入交给 callback，再在内部判断：

```cpp
np::eval_to(out, np::map([](double value) {
    if (value > 0.) return std::log(value);
    return 0.;
}, input));
```

因此不应先构造 `map(guard, log(input))` 来试图避免非正输入的 log。

## 分类与缺失值处理

分类节点的结果是 `bool`，接受标准库支持的整数（含 bool、unsigned）和浮点输入。
整数分类不会先把表达式改成浮点树；每次调用仍使用 child 的实际类型。
`isfinite` 对有限值为真，`isnan` 对 NaN 为真，`isinf` 对正负无穷为真。
不把 NaN 或无穷归零，也不承诺在 fast-math/finite-math-only 等排除这些输入的选项下仍可识别它们。

分类可直接参与 mask、求和和物化；没有额外 callable state，不隐式分配布尔中间数组：

```cpp
const auto a = np::as_expr(input);
const auto finite = np::isfinite(a);
const auto count = np::sum(finite); // bool + bool 的默认累计类型是 int。
np::eval_to(output, np::select(finite, a, 0.));
```

很大的计数可显式使用 `np::sum<size_t>(finite)`，避免默认 int 的表示范围限制。
`select` 仍是 eager：上述替换只选择已经读到的值，不能改成
`select(isfinite(a), log(a), 0.)` 来保护非法 log 输入。
分类调用的 noexcept、错误状态和浮点异常行为来自当前标准库；有抛出 child 时照常传播。
当前显式 xsimd policy 对这些新节点整体回退到 scalar，不擅自改用 backend 特有分类语义。
Windows x64 上，Clang 18 的 baseline/native 三组配对探针（bool 输出、size_t 计数、有限值替换）
与对应手写循环指令数相同；GCC 12 的计数路径相同，其余最多多两条静态指令。
这些探针没有额外函数调用；这只是生成码比较，没有测量吞吐率，也不代表所有布局和表达式均无差异。

## 成本边界

这些节点没有中间 payload 或隐式分配，复用既有 executor。
当前 xsimd 白名单不包含它们，包含 math 子树的整个表达式保守走 scalar fallback，
避免换成具有不同 dtype、准确度或错误状态行为的 batch 运算。
编译器仍可优化普通 scalar 循环；标准函数的必要库调用不是额外的表达式分发。

Windows x64 Clang 18/MinGW GCC 12 的六组 `-O3 -ffp-contract=off` 配对探针中，
local 和 reused AST 都没有比同标准函数手写循环更多的静态 math call 位置。
Clang 的 local abs/exp/log 指令数相同，其他路径有少量循环/绑定差异。
reused 对照也实现 singleton 广播，避免与仅支持等 shape 的循环作不等语义比较。
现有 generic singleton-view 路径仍可能每点重载输入并计算 sqrt；手写快支路可以
先读取不变 scalar 并帮助编译器外提计算。这是当前 executor 的剩余成本，
本模块没有因此宣称所有表达式零开销，也没有假设标准库数学实现是所有需求下的最优选择。
静态指令数不等于吞吐，其他后端和布局需要独立测量。

完整示例见 [expression_math.cpp](../../examples/np/expression_math.cpp)：
显式保存 magnitude workspace、计算归一化边界、复用输出，并演示 raw-input guard。
