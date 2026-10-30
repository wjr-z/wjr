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

Add a standalone `standalone.cpp` directly in this directory to automatically
create `wjr_np_standalone` and its same-named CTest entry. Every discovered
source must have its own `main`; nested example projects remain independent.
`expression_simd.cpp` retains the `WJR_NP_EXAMPLES_XSIMD` option and extra
`wjr::simd_xsimd` dependency.

`cblas.cpp` is also opt-in. Build and install WJR with
`-DWJR_ENABLE_NP=ON -DWJR_ENABLE_NP_CBLAS=ON` and an OpenBLAS LP64 development
package discoverable through pkg-config. Then configure these examples with
`-DWJR_NP_EXAMPLES_CBLAS=ON` and build/run `wjr_np_cblas`. It multiplies two
borrowed 2x2 matrices into a live, default-initialized output and computes a dot
product, without a WJR temporary. Provider workspace and threading remain
OpenBLAS/user responsibilities; see the [CBLAS contract](../../docs/design/tensor-cblas.md).

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

`norm.cpp` / `wjr_np_norm` 直接借用矩阵的非连续列，计算稳定 L2 范数。
它不会创建平方数组；示例同时包含直接平方会溢出和下溢的量级：

```text
column norms: 5e+200 5e-200
```

默认 dtype、NaN 优先及支持的 view 子集见[范数契约](../../docs/design/tensor-norm.md)。

`expression_mean.cpp` / `wjr_np_expression_mean` 直接计算 `mean(a*a)` 和
非线性纯 `map` 的均值，不创建中间 tensor。完整类型、空域与顺序说明见
[表达式 mean](../../docs/design/tensor-expression-mean.md)；融合不意味着总比物化更快。

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

## 表达式融合规约

`wjr_np_expression_reduce` 先将每行均值写入调用方的小 workspace，
再直接计算 `sum((x-mean)*(x-mean))` 和逐行平方残差和。
减法、乘法与累计共用一次遍历，不物化矩阵大小的中间结果。
示例也展示 `sum_to(..., axes<1>, keepdims)` 与广播的组合：

```text
row means: 2 6
row squared residuals: 2 8
sum of squared residuals: 10
```

累计类型、严格逻辑顺序、空域与输出别名要求见
[表达式规约契约](../../docs/design/tensor-expression-reductions.md)。

## 指定转换发生的位置

`wjr_np_expression_cast` 对比 `cast<double>(a)/b` 与 `cast<double>(a/b)`。
前者在除法前转换整数输入，后者保留中间整数除法，再转换其结果；
两者都直接写入预分配输出，不构造转换后的中间数组。

```text
cast before division: 1.5 3.5 2.66667
cast after division: 1 3 2
```

类型、生命周期与 scalar/SIMD 边界见
[显式转换契约](../../docs/design/tensor-expression-casts.md)。

## 融合自定义逐点函数

`wjr_np_expression_map` 将每列中心和缩放广播到样本矩阵，在一次预分配写入中完成归一化与截断。
随后 `sum(map(square, normalized))` 直接计算能量，未物化完整的平方中间结果。
参数使用显式 scalar/view operands；callable 本身无状态，便于编译器保留系数快照。

```text
clipped normalized panel energy: 7
```

值闭包、纯度、异常和捕获状态的性能限制见 [pure map](../../docs/design/tensor-expression-map.md)。

## bool mask 与 clip

`wjr_np_expression_mask` 使用比较和命名逻辑运算生成独立 bool owner，
再通过嵌套 `select` 把数值裁剪到 `[0,1]`，直接写入预分配输出。
select 两侧都求值；示例的所有分支在整个输入域内都有效。

```text
within [0,1]: 0 1 1 1 0
clipped: 0 -0 0.5 1 1
```

NaN、带符号零、类型提升、广播与 eager 约定见
[mask/select 契约](../../docs/design/tensor-expression-masks.md)。

## Padded owning tensor

`strided_tensor.cpp` allocates a positive-stride matrix, fuses an expression
into its logical elements, and reduces without visiting padding.
It distinguishes logical size from the allocated address span and uses
`default_construct` to avoid initializing arithmetic values before overwrite.

## 标准数学函数与显式运算域

`wjr_np_expression_math` 将 `abs` 结果写入显式 magnitude workspace，
再用其总和生成平方根相对幅值；同一输出还用于 `exp`、`log` 和 guarded raw-input map。
这里的平方根变换不是对极端值提供保护的稳定 L2 norm。

```text
rooted relative magnitudes: 0.353553 0 0.612372 0.707107
magnitude decay: 0.367879 1 0.0497871 0.0183156
log(1+magnitude): 0.693147 0 1.38629 1.60944
guarded log of raw inputs: 0 0 1.09861 1.38629
```

guard 接收原始输入，在 callback 内判断后再调用 log；不是用 eager select 保护非法分支。
标准重载类型、errno/fenv、constexpr 和剩余执行成本见
[scalar math 契约](../../docs/design/tensor-expression-math.md)。

## Borrowed slices

`wjr_np_slice` selects alternate columns of a padded owner, computes directly
into the selected elements and reduces the result. Neither slicing nor the
expression allocates an intermediate array; untouched columns and padding
remain untouched. Its output is:

```text
selected shape: 2x3, strides: 8,2, total: 222
```

`strided_slice{offset,extent,step}` uses a source interval length, rather than
Python's stop coordinate. See the [slice contract](../../docs/design/tensor-slices.md)
for examples, layout preservation and trusted-input preconditions.

## Concatenate borrowed arrays

`wjr_np_concatenate` appends a column to a borrowed matrix directly into an
existing output buffer. Source views can have different lengths along the axis.
See [concatenate.cpp](concatenate.cpp) and the
[concatenation contract](../../docs/design/tensor-concatenate.md).

## Fused prefix sums

`wjr_np_expression_scan` broadcasts a row baseline and scans squared residuals
directly into a preallocated output. Prefix differences then provide window
totals without building a squared-residual temporary:

```text
row 0 width-3 squared residual totals: 5 14 29
row 1 width-3 squared residual totals: 20 56 116
```

Accumulator promotion, aliasing and partial exception commits are described in
the [expression scan contract](../../docs/design/tensor-expression-scan.md).


wjr_np_mean: ordered row means with a keepdims workspace, followed by
broadcast centering; see [mean.cpp](mean.cpp) and [the contract](../../docs/design/tensor-mean.md).
## Explicit pairwise summation

`wjr_np_pairwise` compares ordered float, partial pairwise float and an explicit
double accumulator on an exactly known cancellation case. Pairwise grouping
improves this example but is not a correctly rounded sum or an accuracy
guarantee for every input. See the [grouping contract](../../docs/design/tensor-pairwise-sum.md).

`wjr_np_expression_pairwise` fuses squared values and a pure scaled expression
into the same grouping without transformed storage. `square(x)` captures and
evaluates one child once per point; see [the square contract](../../docs/design/tensor-square.md),
[expression_pairwise.cpp](expression_pairwise.cpp) and
[its narrower expression contract](../../docs/design/tensor-expression-pairwise.md).


`wjr_np_variance`: population/sample variance and standard deviation from a
borrowed view; see [variance.cpp](variance.cpp).

## Full-view extrema

`extrema.cpp` finds the first minimum and maximum in a borrowed padded window.
Argument reductions return logical row-major indices, independent of the
window's physical pitch; equal values retain their first logical occurrence.
See [the extrema contract](../../docs/design/tensor-extrema.md) for empty-input
preconditions, dtype, NaN and signed-zero behavior.

## Locate extrema along an axis

`wjr_np_extrema_axis` finds each row's minimum column and each column's maximum
row, preserving the first tie. The second output demonstrates `keepdims`.
See [extrema_axis.cpp](extrema_axis.cpp) and the
[axis-index contract](../../docs/design/tensor-axis-argextrema.md).

## Stable row normalization

`wjr_np_norm_axis` computes stable row L2 norms into a keepdims workspace and
uses the existing broadcast evaluator to normalize extreme-magnitude rows.
Every row in the example has a finite nonzero norm; zero/nonfinite handling
belongs to the application's normalization policy. See
[norm_axis.cpp](norm_axis.cpp) and the [axis norm contract](../../docs/design/tensor-axis-norm.md).

## Shared selection for several quantiles

`wjr_np_quantile` computes several linear quantiles in the original query order.
It reuses partitioning across the requested ranks and caller-provided rank
workspace. The example uses the in-place entry to avoid a data copy; a source
preserving entry takes explicit data workspace. See [quantile.cpp](quantile.cpp)
and the [selection contract](../../docs/design/tensor-partition-quantile.md).

## Reusable ranking workspace

`wjr_np_rank` ranks each row with average ties and omitted NaNs. One caller-owned
index workspace is reused across rows; no result or temporary container is
allocated by WJR. See [rank.cpp](rank.cpp) and the
[ranking contract](../../docs/design/tensor-rank.md) for tie policies and strided views.

## Search in an ordered array

`wjr_np_searchsorted` finds left/right insertion positions on a borrowed ordered
grid, reusing caller-provided output arrays. Duplicate values need no special
index container. See [searchsorted.cpp](searchsorted.cpp) and the
[search contract](../../docs/design/tensor-searchsorted.md).

## Compact selected values and positions

`wjr_np_compact` writes masked values and logical nonzero positions into reusable
output arrays, returning the actual counts without a counting pass or allocation.
See [compact.cpp](compact.cpp) and the [compaction contract](../../docs/design/tensor-compaction.md).

## Merge already sorted arrays

`wjr_np_merge` stably merges two sorted borrowed arrays into caller-owned output.
Equal values from the left precede those from the right; neither input is changed.
See [merge.cpp](merge.cpp) and the [merge contract](../../docs/design/tensor-merge.md).

## Paired scalar statistics

`wjr_np_statistics` computes sample covariance and a Pearson coefficient while
omitting pairs containing NaNs. The numerical kernels borrow both inputs and
allocate no intermediate arrays. See [statistics.cpp](statistics.cpp) and the
[statistics contract](../../docs/design/tensor-statistics.md).

## Count and accumulate integer bins

`wjr_np_bincount` counts repeated integer indices and accumulates weights into
caller-owned output bins. Both operations reset the output and use no workspace.
See [bincount.cpp](bincount.cpp) and the [bincount contract](../../docs/design/tensor-bincount.md).

## Count values between explicit boundaries

`wjr_np_histogram` bins borrowed values into caller-owned counts without an
intermediate index array. The final bin includes the rightmost edge; NaN and
out-of-range values are ignored. See [histogram.cpp](histogram.cpp) and the
[histogram contract](../../docs/design/tensor-histogram.md).

## Segmented reductions

`wjr_np_segmented_reduce` reuses a compact offset array for sums, means, and a
custom minimum fold. Empty segments keep each operation's identity or empty
result; no grouped container is built. See [segmented_reduce.cpp](segmented_reduce.cpp)
and the [segmented reduction contract](../../docs/design/tensor-segmented-reduce.md).

## Sort values and reduce adjacent runs

`wjr_np_sort` demonstrates source-preserving and in-place value sorting.
`wjr_np_run_offsets` extracts adjacent run boundaries and reduces a separate
value array with those boundaries. Nonadjacent equal keys remain separate.

`wjr_np_sorted_runs` composes sorting, run extraction, segmented sums and first
differences into per-value counts and sums. All intermediate storage is explicit
and caller-owned; no grouping container or hidden workspace is introduced.
See [sorted_runs.cpp](sorted_runs.cpp), the [sorting contract](../../docs/design/tensor-sort.md)
and the [run-boundary contract](../../docs/design/tensor-run-offsets.md).

## Reuse a sorting permutation

`wjr_np_argsort` sorts indices without moving the source, then uses `take_to`
to reorder a separate label array. Stable ordering needs no extra sorting buffer.
See [argsort.cpp](argsort.cpp) and the [argsort contract](../../docs/design/tensor-argsort.md).

`wjr_np_take` gathers repeated indices into a two-dimensional destination.
The output has the index array's shape; all storage belongs to the caller.
See [take.cpp](take.cpp) and the [indexed-read contract](../../docs/design/tensor-take.md).

## Apply indexed updates

`wjr_np_put` writes sparse updates into an existing caller-owned buffer.
Repeated indices keep the last update; unaddressed entries remain intact.
See [put.cpp](put.cpp) and the [indexed-write contract](../../docs/design/tensor-put.md).

## Select indices without a full sort

`wjr_np_argpartition` selects the three largest values without moving the source
or sorting the selected prefix. The same indices can gather matching records
from another array. See [argpartition.cpp](argpartition.cpp) and the
[indexed selection contract](../../docs/design/tensor-argpartition.md).
