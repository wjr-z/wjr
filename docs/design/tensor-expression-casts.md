# 显式表达式类型转换

`<wjr/np/expression_cast.hpp>` 提供 `np::cast<To>(value)`。
`value` 可以是现有 `as_expr` 接受的表达式、view 或 arithmetic scalar；
owner 需要显式传入仍然存活的 `owner.view()`，不借用临时 owner。
`To` 必须是无 cv 的算术类型，包含 `bool`；结果节点的 `value_type` 精确为 `To`。
输入表达式的结果也必须是 arithmetic 类型，不接受用户自定义转换对象。

```cpp
np::eval_to(out, np::cast<double>(a) / b); // 转成 double 后除法
np::eval_to(out, np::cast<double>(a / b)); // 先执行原节点的除法，再转换
```

对于整数 `a=3, b=2`，上面分别得到 `1.5` 与 `1.0`。
转换使用 C++ `static_cast<To>` 语义，不进行逐点范围检查；
例如浮点转整数时，截断后的值必须可由目标整数类型表示。
转成 `bool` 的结果是实际 bool 元素，`eval` 可将其物化进拥有 `bool*` 的结果 owner。

该接口复用现有无状态 `unary_expression`，保留子节点的 extents/layout 和值闭包。
建树不读取元素；借用数据需在执行期间存活。转换发生在自己的节点，
之后的节点仍使用各自的 C++ 类型提升规则。读取的 `noexcept` 跟随子树；
可抛子操作不会因外层 cast 而变成不抛操作。
广播、正步长、空域、逐索引原位别名和形状前置条件同现有 `eval_to`。
没有转换后的中间数组、额外所有权或操作对象 state。

当前 xsimd operation 白名单不包含 cast；即使恒等转换也保守使用 scalar fallback，
不会把整数转换或混合 dtype 子树错误替换成 batch 算术。
普通 scalar evaluator 中，恒等转换可由编译器消除。
Clang 18/GCC 12 的 baseline `-O3` 恒等转换探针与同一 evaluator 的无 cast 版本
指令数相同，节点尺寸也与子节点相同。四组配对手写转换循环中，Clang 指令数相同；
GCC 保留 1–3 条额外循环/计数指令，均没有残留调用。这不代表所有布局或编译器的
生成码、吞吐都完全相同。
使用和输出示例见 [expression_cast.cpp](../../examples/np/expression_cast.cpp)。
