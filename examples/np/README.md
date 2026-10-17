# 预分配 tensor 计算示例

`elementwise.cpp` 和 `expression.cpp` 均使用已发布的 `wjr::np` 接口，
通过独立 CMake 消费安装包。表达式示例在下方说明。

`elementwise.cpp` 演示已公开的 `wjr::np` 接口：

- `fill_to` 写入已有 buffer；双目 `transform_to` 在一遍循环中完成 `(a-b)*scale+bias`。
- `try_column_view` 借用非连续列，再用单目 `transform_to` 原位缩放。
- `transpose_view` 只交换 metadata；`copy_to` 将结果实际写入另一块行优先 buffer。

示例使用栈上的 `std::array` 管理数据，view 不延长其生命周期。
计算入口不分配临时数组；输出到 `iostream` 的成本不计入这一约定。
示例不做计时，不将打印或固定小数组的结果用作性能结论。
输入输出要么逻辑元素不相交，要么逐索引精确相同；这里的转置使用独立输出。

先构建、安装启用 NP 的 WJR（已有安装时可跳过）：

```sh
cmake -S . -B build -DWJR_ENABLE_NP=ON -DWJR_CPU_TARGET=baseline \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$PWD/wjr-install"
cmake --build build --parallel
cmake --install build
```

然后独立构建此示例。仅链接可选 `wjr::np` target，C++20 要求由包传递：

```sh
cmake -S examples/np -B example-build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$PWD/wjr-install"
cmake --build example-build --parallel
ctest --test-dir example-build --output-on-failure
```

直接运行 `example-build/wjr_np_elementwise`（多配置生成器通常在 `Release/` 下）。
Windows PowerShell 请将 `$PWD` 展开为相应绝对安装路径，并按 shell 规则换行。
预期输出：

```text
fused (a-b)*2+1:
3 5 7
9 11 13
column stride: 3
after scaling column 1:
3 2.5 7
9 5.5 13
materialized transpose:
3 9
2.5 5.5
7 13
```

动态 shape、dtype 转换、别名与异常的完整契约见
[逐元素计算](../../docs/design/tensor-elementwise.md)。elementwise 示例没有表达式树或隐式广播。

## 值闭包与表达式

`expression.cpp` 使用真实 mdspan 叶子，以 `as_expr` 构造 `0.5*x + 0.25*y`，
通过 `eval_to` 一遍写入预分配的 owner。随后返回由局部 difference 节点组成的平方表达式，
原位更新同一输出。局部节点按值保存，输入数据保持存活，不产生中间 tensor。

运行同一构建目录的 `wjr_np_expression`，输出：

```text
2.00 2.25 2.50
2.75 3.00 3.25
```

示例还计算 `[2,3] * [2,1] + [3]`，把每行系数和每列偏置广播到矩阵，直接写入已有输出。
数组支持右对齐广播、混合连续布局与正步长 view；普通 scalar 也可参与计算。
类型提升逐节点遵循 C++，不将中间整数除法提升成浮点运算。
范围和别名合同见 [表达式 API](../../docs/design/tensor-expression-api.md)。

## 行中心化与规约

同一 CMake 项目还会构建 `wjr_np_row_centering`，CTest 运行这些示例。
`row_centering.cpp` 用显式预分配的 `[rows,1]` workspace 保存 `sum_to` 结果，
再逐行通过 `transform_to` 减均值。最后复用输出存平方残差并全量 `sum`。
没有 lazy reducer，也没有为每个输出元素重复计算均值。
示例的列数固定为 3；处理动态空行时，均值的除零规则需由调用方另行决定。

```text
row sums: 6 18
centered rows:
-1 0 1
-2 0 2
sum of squared residuals: 10
```

累计类型、逻辑顺序、空域及不交叠要求见[规约契约](../../docs/design/tensor-reductions.md)。

`expression_simd.cpp` demonstrates explicit SIMD evaluation of a packed
expression, including a scalar tail. Install WJR with both `WJR_ENABLE_NP=ON`
and `WJR_ENABLE_XSIMD=ON`, then configure this example project with
`-DWJR_NP_EXAMPLES_XSIMD=ON`. It links the optional `wjr::simd_xsimd` component;
the ordinary examples keep their existing dependencies. The precise dtype and
alias contract is described in [SIMD expression evaluation](../../docs/design/tensor-expression-xsimd.md).

## 物化为独立拥有数据的结果

`evaluate.cpp` 使用 `eval((x-center)*(x-center))` 创建新的 tensor owner，
展示与预分配 `eval_to` 的区别。`wjr_np_evaluate` 输出平方偏差，随后修改原数组，
确认结果保持独立。allocator、错误和 layout 约定见
[表达式物化](../../docs/design/tensor-evaluation.md)。

## 前缀和、区间和与二维积分表

`wjr_np_prefix_sums` 展示 `cumsum_to` 的三个组合：一维 inclusive prefix，
通过前缀相减计算固定宽度窗口总和，以及先列后行生成二维离散积分表。
所有输出均预分配；第二次二维 scan 直接复用输出，不分配中间数组。
CTest 包含该示例，预期输出为：

```text
inclusive prefix: 1 3 6 10 15 21
width-3 window totals: 6 9 12 15
2D integral table:
1 3 6
5 12 21
```

有序累计、输出转换、原位别名及浮点前缀相减的限制见
[scan 契约](../../docs/design/tensor-scans.md)。
