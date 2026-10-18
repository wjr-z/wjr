# Pure pointwise map

`<wjr/np/expression.hpp>` 提供 `np::map(function, operands...)`，至少需要一个 operand。
它把 C++ callable 纳入现有值闭包表达式；不增加解释器、算子注册表或中间 tensor。

```cpp
auto clipped = np::map(
    [](double x, double center, double inverse_scale, double limit) noexcept {
        const auto value = (x - center) * inverse_scale;
        return value < -limit ? -limit : (value > limit ? limit : value);
    }, input_view, center_view, inverse_scale_view, 3.0);
np::eval_to(output_view, clipped);
auto energy = np::sum(np::map([](double x) noexcept { return x * x; }, clipped));
```

`eval`、`sum`、`sum_to` 分别由 `<wjr/np/evaluate.hpp>`、
`<wjr/np/expression_reduce.hpp>` 提供。`examples/np/expression_map.cpp` 是可编译的
二维特征归一化与截断示例；中心和缩放 `[1, features]` 广播到样本矩阵。

## 值、类型和生命周期

每个 operand 遵循 `as_expr` 的值捕获规则，可为受支持的 mdspan、标量或表达式。
静态 rank/extents、布局偏好、右对齐广播、正步长和空域行为沿用
[基础表达式接口](tensor-expression-api.md)。所有 operand 都参与域解析；callable 不提供 shape。
rank-0 view 仍是借用的一个对象，不会变成捕获标量。

AST 保存 `std::decay_t<F>` 的真实对象，包括构造、移动或析构有可观察副作用的空类型。
左值 callable 按值复制，右值移动；求值通过 `const F&` 调用。
move-only callable 可以从右值建立节点；后续组合需显式移动该节点，不能隐式复制左值。
表达式可以反复以 `const&` 求值，求值不构造、复制或移动 callable。
callable 自己管理的资源遵循其 C++ 类型语义；库不加入共享所有权。

调用的每个参数都是对应 child 的 `value_type` **纯值**，与该 child 是 view、标量还是
中间节点无关。只接受可由 `const F&` 调用且返回非引用 arithmetic 值的 callable；
先拒绝引用，再移除结果的顶层 cv。void、代理对象、仅接受可变左值引用的接口被拒绝。
`int / int` 的结果先按 C++ 规则生成，再传给 map；输出类型不反向改变子树计算。
需要改变某一步的 dtype 时使用 [显式 cast](tensor-expression-casts.md)。

## 纯度、别名和异常

callable 必须是纯逐点计算：结果只依赖参数及本次求值期间稳定的配置；
不依赖调用顺序、对象身份、调用次数或跨元素副作用。其捕获内存必须与输出的逻辑存储不交。
不修改 source、callable 或捕获配置，也不在求值期间由其他线程改变它们。
这是一项调用方前置条件，不会注入 `restrict` 或运行时检查。
显式数据 operand 保留现有逐索引精确原地别名规则；广播数据不能与输出产生跨索引依赖。
借用 view 不延长底层存储寿命，捕获裸指针也不延长其所指对象寿命。

标量 callable 内的普通 C++ `if` 可保护该元素的除法、开方等操作；
它不会延迟读取其他 operand 的表达式。每个 child 在进入 callable 前已完成该元素的求值，
不同 child 的求值次序不作额外保证。空输出不调用 callable。

构造、复制或移动可能抛异常时，map、算术组合和 cast 的构造异常正常传播；
普通数值节点仍是 nothrow。调用异常可从 `eval_to`、`sum`、`sum_to` 传播，
已完成的输出写入不回滚；`sum_to` 仅提交已完成的规约线。
拥有结果的 `eval` 按原 allocator/owner 生命周期清理已分配存储。
`noexcept` 随实际构造或逐点调用，不把用户 callable 强制视为 nothrow。

## 执行与性能边界

每次非空求值把数据指针/标量快照绑定到 data tuple；另一个 function tuple 只保存
callable 的 `const F*`。函数与数据分别计数，函数指针不会进入 stride/offset 递增。
静态子节点协议提供编译期 arity 与访问；没有运行时树遍历。
纯算术/cast 表达式仍只绑定原来的 data tuple，不增加空 function tuple 或函数状态。
现有预分配入口没有隐藏分配、整缓冲区临时对象、额外遍历或内层检查。

首版含 map 的树整体走 scalar evaluator，包括显式 `xsimd_policy` 请求；编译器仍可自动向量化。
不会把标量 callable 猜测为 batch callable，不改变默认浮点或 fast-math 选项。

指针绑定不会自动证明任意捕获状态可以移出循环。局部构造的小 lambda 和显式标量 operand
可被 GCC/Clang 保存在寄存器；通过任意 `const E&` 借用既有捕获对象时，仍可能出现
配置别名保护及重复状态读取，与同样 `const F&` 的手写循环相似。
可将小数值系数作为上例中的显式 operand，向编译器暴露值快照。
不为了消除这些读取而在每次求值时复制任意 callable。
较大的嵌套树仍会影响内联预算、构造代码和编译成本，不能把此接口笼统称为完全零开销。
本轮生成码对照覆盖普通算术、小 lambda、借用配置及 2/4/8/16 叶子树；不据此宣称吞吐提升。
