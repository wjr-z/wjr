# 预分配输出与逐元素融合

链接可选 target `wjr::np` 后，`<wjr/np/copy.hpp>` 提供 `copy_to(dst, src)`、
`fill_to(dst, value)`；`<wjr/np/transform.hpp>` 提供单目和双目 `transform_to`。
输出均由调用方提前分配，不调整 shape，不分配临时 tensor，不保留所有权。

```cpp
#include <wjr/np/copy.hpp>
#include <wjr/np/transform.hpp>
#include <wjr/np/transpose.hpp>

namespace np = wjr::np;
using shape = np::extents<std::size_t, 2, 3>;
const double prices[] = {10, 12, 14, 20, 24, 28};
const double baseline[] = {8, 8, 8, 16, 16, 16};
double result[6];
np::tensor_view<const double, shape> a(prices), b(baseline);
np::tensor_view<double, shape> out(result);

np::transform_to(out, a, b, [](double x, double y) noexcept {
    return (x - y) / 2.0; // 一个循环完成相减和缩放。
});
np::transform_to(out, out, [](double x) noexcept {
    return x < 0 ? 0 : (x > 3 ? 3 : x); // 同位置原位 clip。
});

double transposed_storage[6];
np::tensor_view<double, np::extents<std::size_t, 3, 2>> transposed(transposed_storage);
np::copy_to(transposed, np::transpose_view(out)); // 按新行序实际重排数据。
np::fill_to(out, 0.0);
```

## 首版契约

- 接受固定 rank、`layout_right`、`layout_left`、正步长 `layout_stride`，以及默认 accessor。
  元素为非 volatile 的算术类型；输出元素必须可写。真正的 `bool[]` 可用，位代理不可用。
- 所有 view 都须满足 mdspan 的有效存储、映射唯一性及可表示性前置条件。
  可以显式使用 owner 的左值 `.view()`；入口不隐式借用 owner，也不延长存储寿命。
- 输入输出 shape 必须相等，无广播。rank 或已知静态 extent 不匹配在编译期拒绝；
  动态 extent 用入口处的 `WJR_ASSERT_L2` 检查一次。关闭对应断言时，调用方仍须满足契约。
- `copy_to` 要求去除 const 后的元素类型一致。`fill_to` 在调用时将标量转换为输出类型并按值保存，
  因而 `fill_to(dst, dst(0, 0))` 不会因后续写入而改变填充值。
- `transform_to` 的输入和输出类型均由 view 明确指定，允许不同算术类型。
  lambda 的结果使用普通 C++ 赋值转换，可能截断或缩窄；不提供隐式 dtype 推导或饱和转换。
  调用方负责转换范围、除零、整数溢出以及所需的 NaN/舍入语义。
- callable 作为左值使用，支持有状态、不可复制的 callable；输入始终为 const 引用。
  每个逻辑元素调用一次，遍历顺序不是 API 保证。callable 应只写其结果；
  不应通过捕获状态修改其他输入元素或使下一次读取失效。
  callable 自身的分配和副作用由调用方负责，无分配约定针对库的遍历和输出管理。
- rank-0 是一个标量。任意 extent 为零则不读取 stride、不做指针算术、不调用 callable，
  nullptr 空 view 可用。所有 padding 均保留。

## 别名与异常

每个输入的逻辑元素集合须与输出不相交，或在每一个对应逻辑索引上引用同一对象。
因此两列虽然物理跨度相交，只要实际元素不相交，仍可以直接计算。
`transform_to(out, out, op)` 以及双目某个输入与输出完全同位置的情况可以原位计算。

`copy_to` 在同一 data handle、所有 extent 大于 1 的轴上 stride 相等时直接返回，
避免把精确自复制交给 `std::copy_n`。只有 extent 为 1 的轴，其 stride 不影响该判断。
其余交叠，包括错位切片和方阵原位转置，均违反前置条件。
接口不扫描地址范围、不添加 `restrict`、不自动分配防别名临时数组。

`copy_to`、`fill_to` 为 constexpr/noexcept；`transform_to` 的 noexcept 取决于 callable
调用及其结果的赋值转换。若操作抛出，已写入元素保留，后续元素未处理；不回滚已有输出。
无异常配置可使用不抛出的操作。所有入口都可在后端允许的 C++20 常量求值场景中使用。

## 执行边界

连续且逻辑顺序一致时，复制/填充直接复用 `std::copy_n` / `std::fill_n`，变换使用一维循环。
其他布局在入口收集 shape 和 stride，使用编译期 rank 的嵌套循环；叶节点只有访问、调用和赋值，
不重复解析 shape，不经逐点 mdspan 索引，也不创建表达式节点或动态分发。
输出为 `layout_left` 时沿其连续轴优先遍历；其他布局按末轴优先遍历。
首版不为任意 stride 自动选循环顺序或 tile，不承诺任意转置/稀疏步长都达到手写专用核的性能。

这些入口是后续 evaluator 可复用的执行原语，并非完整 NumPy 运算或表达式系统。
表达式树、广播绑定、向量化后端、矩阵乘、reduction 和未知别名的临时工作区由后续模块分别设计。
当前没有自动并行、显式 SIMD 或全局 fast-math。

## 初版验证

Clang 18.1.8、MSVC 19.42 的 C++20 baseline 配置通过相关 53 项 NP 测试及消费程序；
Clang 的 O3/AddressSanitizer 配置也通过。GCC 12.1 独立程序运行本文件功能的 7 组测试体，
涵盖 constexpr、跨布局、原位别名、padding、空/标量和异常；无异常独立程序通过，
并用全局 allocation counter 检查库操作期间没有分配。

外部最小 probe 以相同 alias 契约与手写循环/标准算法对照；下面是 WJR/手写的静态指令数，
不是运行时间。Clang/GCC 使用 O3，MSVC 使用 O2；默认 x64 baseline，无 fast-math。

| probe | Clang 18 | GCC 12 | MSVC 19.42 |
| --- | --- | --- | --- |
| 一维动态 copy | 7/7 | 9/9 | 9/9 |
| 一维动态 fill | 35/35 | 27/27 | 8/8 |
| 一维动态 affine | 63/63 | 42/42 | 55/60 |
| 一维动态 `(a-b)/2` | 59/59 | 45/46 | 74/76 |
| 一维动态 stride affine | 65/74 | 58/56 | 17/14 |
| 固定 2×3 转置复制 | 13/13 | 13/13 | 15/11 |

copy/fill 保留标准库选择的 memmove/memset/fill_n 调用；连续变换和一维 stride probe
没有新增包装调用或分配。MSVC 的转置复制未内联，15 条只统计调用方，
其内部复制循环还需执行；这里是明确的剩余成本，不是零开销结论。
未测通用多维 stride 的最佳循环排序、吞吐量或硬件分支/缓存计数器。
