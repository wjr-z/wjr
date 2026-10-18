# 按轴计算稳定 L2 范数

`<wjr/np/norm_axis.hpp>` 提供：

```cpp
np::norm_l2_to<Accumulator = void>(dst, src, np::axes<I>);
np::norm_l2_to<Accumulator = void>(dst, src, np::axes<I>, np::keepdims);
```

对 `src` 的每条第 `I` 轴线计算稳定 L2 范数，直接写入调用者预分配的 `dst`。
默认删除该轴，`keepdims` 则将该轴长度保留为 1。仅接受一个编译期轴；
rank 1 可归约到 rank 0，rank 0 输入没有可选轴。

源与目标都是默认 accessor 的普通 view，支持 packed left/right、正 `layout_stride`
及已有 padded mapping。源为非 volatile 算术元素，目标为可写的非 cv 浮点元素；
不接受表达式或 owner 的隐式借用。owner 通过左值 `.view()` 进入。

累积类型与 [`norm_l2`](tensor-norm.md) 一致：浮点输入保留其类型，整数和 bool 默认 double；
显式 `Accumulator` 必须为非 cv 二进制浮点类型，与目标元素类型独立。
每个输入先转到累积类型，在线结束后将范数转换一次并写入目标。
有限输入必须能转换到累积类型，有限结果必须能转换到目标类型；入口不增加逐元素范围检查。

## 前置条件与数值语义

输入、输出必须是合法且存活的 CPU view，目标 shape 必须匹配删除轴或 keepdims 后的 shape。
静态 shape 不兼容在约束处拒绝，动态 shape 沿用一次 L2 断言前置条件。
输入与输出的逻辑存储必须不相交；不支持原位归约，也不建立隐藏缓冲区解决重叠。
调用期间输入保持稳定，包括罕见恢复重扫期间。

每条线按递增轴下标交给现有 Blue 三桶内核；不读取 padding，不改变普通 `norm_l2` 的内核。
大、小量级先按类型决定的精确二次幂缩放，避免 `sqrt(sum(x*x))` 的提前上溢或下溢。
桶溢出时保留现有 scale/sumsq 恢复，允许重读该线；这不是强制单遍算法。
没有中间 payload、分配、函数对象、共享所有权或逐点 shape 检查。

空输出不访问输入或输出；空的归约线写入正零，且不对 null 输入做指针偏移。
全为正负零的线也产生正零。每条线中 NaN 优先于正负 infinity；无 NaN 且含 infinity 时为正 infinity。
真实有限范数超出累积类型范围时允许结果为 infinity。
舍入误差、NaN payload、errno/fenv 的边界与已有稳定范数相同；不承诺跨布局或编译器逐位相等。
不启用 fast-math、SIMD 数值重结合或自动并行。

## 使用

[`norm_axis.cpp`](../../examples/np/norm_axis.cpp) 用 keepdims 工作区计算每行范数，
再用已有广播表达式原位归一化两行极大/极小输入。
例子明确要求每行范数有限且非零；通用应用应自行规定零范数与非有限输入的归一化策略。
不使用 eager `select` 掩盖已执行的除零。
