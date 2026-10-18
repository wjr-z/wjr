# 稳定 L2 范数

`<wjr/np/norm.hpp>` 提供 `np::norm_l2<Accumulator = void>(view)`，返回一个浮点标量。
不创建平方数组、不分配存储、不延长输入寿命，也不依赖 BLAS 二进制库。
本入口接受普通 view，不接受表达式或 owner 的隐式转换；owner 通过左值 `.view()` 借用。

```cpp
const double values[] = {3e200, 4e200};
np::tensor_view<const double, np::extents<size_t, 2>> vector(values);
const auto length = np::norm_l2(vector); // 5e200，直接平方会溢出
const auto wide = np::norm_l2<long double>(vector);
```

## 类型与布局

默认累计及返回类型：浮点元素保留其 float/double/long double 类型，整数和真实 bool 使用 double。
显式 `Accumulator` 必须是非 cv 浮点类型；当前使用二进制浮点模型。
每个元素先转换到该类型，再取绝对值和累计，避免对整数最小值做整数绝对值运算。
显式窄化时，有限输入必须在目标浮点类型的可转换范围内；函数不增加逐点范围检查。
累计有通常的浮点舍入，显式选择更宽类型可以减少误差，函数不承诺正确舍入。

支持默认 accessor、非 volatile 算术元素及：

- 任意固定 rank 的 packed `layout_right` / `layout_left`；按物理存储顺序遍历。
- rank 1 的有效正 `layout_stride`；按递增逻辑下标遍历，孔洞不读取。
- rank 0；其一个对象按标量处理，包括 left/right/stride descriptor。

不支持一般多维有孔/padded mapping、自定义 accessor 或 signed stride。
不同物理布局可以改变浮点累计分组，不承诺逐位相同的结果。
输入必须是合法、生命周期有效的 CPU view；沿用现有 mapping 与可寻址存储的前置条件，
入口不重复验证这些元数据，也不在循环内加入 shape 检查。

## 特殊值

空域在数据 handle、stride 观察与指针偏移前返回正零，可使用合法的 null 空 view。
全为正负零同样返回正零。rank 0 的有限值返回转换后值的绝对值。
任意 NaN 优先于 infinity，结果为 NaN；没有 NaN 而含正负 infinity 时返回正 infinity。
不会在遇到 infinity 时提前返回、遗漏后面的 NaN。
有限输入的真实范数超出返回类型范围时，可以返回正 infinity。

NaN payload、signaling NaN 的具体行为、errno 与浮点异常标志序列不作跨平台承诺。
不同编译器、标准库与缩放分组允许产生舍入差异；不启用 fast-math 或隐式并行重结合。
用户的 finite-math/fast-math、非默认舍入及 flush-to-zero 配置可能削弱上述数值保证。
C++20 不保证标准 `sqrt` 能常量求值，因此本入口不声明 constexpr。

## 缩放与必要成本

实现参考固定 [Reference BLAS xNRM2, LAPACK 3.12.1](https://github.com/Reference-LAPACK/lapack/blob/v3.12.1/BLAS/SRC/dnrm2.f90)
的 Blue 三桶方案：普通、小和大幅值分别累计，小/大幅值用类型决定的精确二次幂缩放。
头文件保留上游完整 BSD attribution/license，不引入上游运行时依赖。
相较 `sqrt(sum(x*x))`，每点的幅值分类和必要缩放用于避免提前上溢/下溢；
常规循环没有除法、sqrt、函数调用或分配。

只在桶累计溢出时，用 scale/sumsq 表示重扫输入，避免桶先溢出而真实范数仍可表示。
该恢复路径参考较早的 [LAPACK xLASSQ](https://github.com/Reference-LAPACK/lapack/blob/v3.8.0/SRC/dlassq.f)，
使用现有 cold/noinline 宏隔离；也处理 infinity，同时继续观察整个输入以保留 NaN 优先规则。
通常一遍，罕见恢复时最多两遍；没有中间 payload，也没有第二套公共策略或调参接口。
small/medium 合并直接计算 `high * sqrt(1 + (low/high)^2)`，避免再平方 high。

这是范围稳定的浮点算法，不是任意长度下的高精度求和；舍入误差仍可随长度增长。
不与不稳定的平方和实现作同合同性能比较。

## 当前验证与边界

10 项针对性 GTest 覆盖 dtype、packed/stride、rank 0、null 空域、正负零、整数最小值、
极端幅值、阈值邻居、NaN/Inf 的不同顺序，以及 float 桶累计溢出后的恢复。
最后一项用 64 MiB 输入触发真实公开路径，没有缩小算法阈值或替换私有实现。
Clang 18 O3/ASan/debug 3 和 MSVC 19.42 的测试及列范数示例通过。
GCC 12、Clang 18 的独立程序通过 62 组同合同手写/调整后的 hypot oracle 对照，
并观察到计算入口不调用分配。`std::hypot(Inf,NaN)` 的优先级与本入口不同，oracle 先按本文处理 NaN。

Windows x64 外部 paired probe 比较实际 view API 与直接 C++ Blue 循环，
双方使用相同 dtype、NaN 优先、overflow 恢复和 `-O3 -march=x86-64 -ffp-contract=off`。
包含空/短/tail、300/8192 元素、步长 1/3、普通/极小/极大/混合量级。
GCC/Clang 常规热循环只有必要的数据读取、分类与累计，没有逐点 descriptor metadata 读取或恢复调用。
helper 是否内联、冷分支布局、Windows ABI 寄存器保存与短输入 setup 仍随编译器改变。
本机计时存在波动，不能据此宣称所有 case 零开销、优于优化 BLAS，或代表 Linux/SysV 吞吐。
没有采集硬件计数器或测量第三方 BLAS 二进制。
