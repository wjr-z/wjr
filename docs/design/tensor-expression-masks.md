# 比较、bool mask 与 eager select

`<wjr/np/expression_mask.hpp>` 提供六种逐元素比较 `< <= > >= == !=`。
沿用算术表达式的参与规则：至少一侧是现有表达式，另一侧可为可捕获的 scalar/view；
原生 scalar 的普通 C++ 比较不受影响。输入结果限 arithmetic，比较结果是 bool 表达式。
表达式没有到 scalar bool 的隐式转换。

```cpp
const auto x = np::as_expr(input);
auto valid = np::logical_and(x >= 0., x <= 1.);
auto flags = np::eval(valid); // 独立拥有真实 bool 元素，非 packed bitset
np::eval_to(output, np::select(x < 0., 0., np::select(x > 1., 1., x)));
```

`logical_not`、`logical_and`、`logical_or` 是命名接口，仅接受 bool-valued
expression/view/scalar，不把数字自动解释为真值。数字真值转换需显式 `cast<bool>`。
不重载 `&&`、`||`；命名逻辑运算也没有短路语义。

`select(mask, yes, no)` 要求 bool mask 和 arithmetic 分支，三者进行右对齐广播。
结果 shape 不依赖 mask 的数据。结果类型是分支值的 C++ 条件表达式通常算术转换，
例如 float/double 得 double，signed/unsigned 可能得 unsigned，bool/bool 仍为 bool。
mask 类型不参与分支数值提升，结果不会意外成为引用。

两侧分支先求值，再执行 scalar 选择；不承诺子树求值先后顺序。
因此全 false mask 也不能保护 `select(mask, numerator/denominator, 0)` 中的零分母。
两侧运算和访问都必须在整个执行域有效。需要真正按值分支的计算可使用纯 `map` lambda
中的 C++ `if`，不能将 eager tensor select 当作 guard。
空执行域不求值子树；rank 0 执行一次。

在通常非 fast-math 的 IEEE 浮点设置下，NaN 的有序比较为 false、相等为 false、
不等为 true；正负零相等。select 使用条件选择，保留被选分支的带符号零，
不用 `mask*a+(1-mask)*b` 代替。上述 clip 在 NaN 输入时选择原输入并传播 NaN；
不保证 NaN payload 或 signaling exception 的时序在不同编译器间完全一致。

节点按值保存闭包，数据仍是借用，沿用现有 shape、生命周期与逐索引别名前置条件。
所有子树均在当前坐标的输出 store 前读取，允许现有 `eval_to` 的精确逐点原位操作；
其他输入输出重叠仍违约。构造的 `noexcept` 取决于实际子树 copy/move，
读取的 `noexcept` 跟随子树；抛出分支或闭包拷贝异常正常传播。

select 复用内部静态 arity/child 协议，不保存 callable，也不增加函数绑定 slot。
它的 map 子节点仍按既有规则绑定自己的 callable。
没有临时 mask 数组或中间 payload；`eval(valid)` 的结果分配是显式物化行为。
显式 xsimd 入口现可将同一 float/double 数值树中的比较、命名逻辑与 eager select
映射为数值 batch 对应的 predicate batch，允许 bool scalar splat 和嵌套 predicate select。
bool view、bool 结果物化、mixed dtype、cast、map、math 和未知操作仍整树走 scalar fallback；
不实例化 `batch<bool>`，不改变逐节点 dtype。详见 [SIMD 契约](tensor-expression-xsimd.md)。
编译器仍可自行优化普通 scalar 循环；不保证所有布局都与手写循环指令数相同。
当前 Clang 18/GCC 12 baseline `-O3 -ffp-contract=off` 的 clip、bool mask、
混合整数除法选择和逐行广播四组配对探针均无残留调用或中间数组。
Clang 的 clip 与混合选择指令数和手写循环相同；GCC 相应路径仍有少量
循环/计数差异。广播的循环组织也可能不同，不将静态指令数当作吞吐结论。

可运行的真实 bool owner 与 clip 示例见
[expression_mask.cpp](../../examples/np/expression_mask.cpp)。
