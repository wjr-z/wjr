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

## 验证与执行成本

7 项新 GTest 与原有 10 项完整范数测试在 Clang 18 O3/ASan/debug 3、MSVC 19.42 通过；
包括真实 64 MiB 输入的桶溢出恢复。GCC 12 的 C++23/no-exceptions 程序通过 245 组
shape/数值分布对照。独立安装包消费者只链接 `wjr::np`，运行上述归一化示例。
该消费者复用已安装的 baseline 库/config，并通过 CMake 安装本次全部公共头文件。

[固定 Linux 研究](https://github.com/wjr-z/wjr/actions/runs/39100307737) 使用
GCC 13.3、Clang 18.1.3、Ubuntu 24.04、AMD EPYC 9V74，
`-O3 -ffp-contract=off -fno-fast-math`，分别生成 baseline x86-64/native 代码。
源码为 `56d17df2`，研究分支 `research/np-axis-stable-norm` 的 `f8e37628` 保留探针及 driver。
两编译器均通过 245 组实际轴入口、重复公开 `norm_l2`、同 Blue 内核手写循环的数值对照。

原完整/strided 范数的 8 组入口及引用 helper 的指令、寄存器和控制流不变。
Clang 的行、列、float、宽累积、padded、静态 shape 共 12 组新接口/手写图相同。
GCC 的静态 shape 两组也相同；动态 packed 行保留常量步长内核，列使用双方同一动态步长内核，
外层仍有寄存器分配和空线分支布局差异。动态 `layout_stride` 的已知单位步长在手写端可以
专门化，但当前入口可能保留通用步长内核；这是一项剩余优化差距，没有用额外运行时分派掩盖它。
各线内数据循环没有逐点 descriptor 读取、分配或除模。
没有测量吞吐、硬件计数器或成熟 BLAS 二进制，不据此承诺所有布局零开销或加速。
