# View 求和与单轴规约

`<wjr/np/reduce.hpp>` 提供立即执行的 `sum` 和 `sum_to`，链接可选 target `wjr::np`。
输入为现有 `tensor_view`，不增加表达式节点、共享状态或隐式临时数组。
这为[后续表达式](tensor-expressions.md)的 terminal reduction 留出同名扩展点，当前不接受 lazy expression。

```cpp
namespace np = wjr::np;
double values[] = {1, 2, 3, 4, 6, 8};
double row_buffer[2];
np::tensor_view<const double, np::extents<std::size_t, 2, 3>> input(values);
np::tensor_view<double, np::extents<std::size_t, 2, 1>> rows(row_buffer);

double total = np::sum(input);                  // 24
np::sum_to<double>(rows, input, np::axes<1>, np::keepdims); // [6, 18]
```

完整[行中心化示例](../../examples/np/row_centering.cpp)预分配 row sums，逐行通过
`transform_to` 减去均值，再原位计算平方残差并 `sum`。每个 row sum 只计算一次；
当前没有广播 evaluator，因此显式借用行后计算，不在每个输出坐标重复规约。

## 类型与数值规则

- `sum<Accumulator = void>(input)` 返回累计类型。默认类型是 C++ 的 `decltype(T + T)`：
  bool 和小整数通常提升为 int，float 仍为 float，double 仍为 double。
  `sum<double>(float_view)` 在每次加法前将输入转为 double。
- `sum_to<Accumulator = void>(out, input, axes<Axis>)` 去掉所选轴，输出 rank 减 1；
  增加 `keepdims` 参数则保留该轴，其输出 extent 必须为 1。其余轴按原顺序保留。
  输出类型由 out 指定，最终赋值使用普通 C++ 转换。
- 累计类型独立于输出类型：float 输入规约到 double 输出，默认仍在 float 中累计。
  需要更宽累计时显式写 `sum_to<double>`。显式累计类型须为无 cv 的算术类型，
  且其自身加法不再发生整数提升；拒绝 bool/char/short 等窄累计状态。
- 从累计类型的零开始，按顺序执行普通标量加法；不使用 pairwise、Kahan、SIMD 横向合并、
  多线程重排或隐式 fast-math。显式更宽累计可改善部分消去问题，不保证数值稳定性或精确性。
- 有符号整数累计不得溢出，浮点转整数及最终输出转换需满足 C++ 的可表示性要求。
  无符号运算、浮点舍入、NaN、无穷大及正负零沿用普通标量加法和编译器浮点配置。
  默认不跳过 NaN，不保证跨编译器/ISA 逐位一致；调用方打开 fast-math 后不能再依赖本页的顺序敏感结果。

全量 `sum` 按逻辑行优先顺序折叠，末轴变化最快，与输入的物理布局无关。
`sum_to` 按输出逻辑行优先顺序写结果，每个结果沿所选轴的索引 0、1、… 顺序累计。
例如 column-major 输入的全量浮点结果不会因为简单扫描物理 buffer 而改变加法顺序。

## Shape、存储与空域

输入输出支持默认 accessor、非 volatile 算术元素、left/right 和有效的正 stride mapping。
输出元素必须可写；padding 保留。owner 须显式通过左值 `.view()` 借用。
输入输出必须是有效的 CPU view，映射、存储长度、对象寿命及元数据可表示性由调用方保证。

初版 `sum_to` 只接受单个编译期轴。`axes<>`、多个轴、非法轴和已知静态 shape 不匹配在编译期拒绝，
空轴列表不会被误解释为全量规约。动态输出 extent 用入口处的 `WJR_ASSERT_L2` 检查一次；
关闭对应断言仍须满足 shape 契约。runtime axis 数组、多个轴和通用 reducer 留待后续扩展。

全量空输入返回累计类型的零；rank-0 是一个元素，从零加上它后返回。
rank-0 没有可选轴，因此仅支持全量 `sum`。
`sum_to` 的空输出不访问输入；如果只有被规约轴长度为零，非空输出的每个逻辑元素写零。
这些情况在任何输入 stride 观察、数据读取和指针偏移之前处理，允许有效的 nullptr 空 view。
全部入口 constexpr/noexcept，不分配，不改变 owner 容量，不捕获生命周期。

`sum_to` 要求输入输出的逻辑元素集合不相交，不能套用逐点 transform 的同索引原位规则。
没有隐式别名扫描或暂存；交叠规约违反前置条件。
bounding span 相交但实际元素不交的切片仍可使用，例如输入为矩阵前两列、输出为独立的第三列。

## 执行与初版验收

常见全量连续行优先输入直接使用一维标量循环，一维 stride 使用直接步长循环。
其他全量布局保持逻辑行序；单轴输出使用编译期 rank 的标量偏移遍历，跳过 keepdims 的被规约轴。
输入指针、累计轴的长度和步长在内层加法循环前确定，内层不重复检查 shape。
泛多维路径不保证选择缓存最优顺序；严格顺序也限制了浮点规约的自动向量化空间。

测试覆盖行/列、keepdims、三维中间轴、转置/padding、动态和窄 index、真实 bool、
全量/输出/被规约轴为空、rank-0、constexpr、精度提升、消去敏感输入和显式行中心化。
针对性无异常程序及 allocation counter 验证计算过程无分配。

六组外部 paired probe 使用等价标量加法和空域语义，在 Clang 18/GCC 12 的 O3、
MSVC 19.42 的 O2 下比较生成码，未使用 fast-math。Clang 六组均与手写静态指令数相同；
MSVC 的全量/显式 double/一维 stride/固定 keepdims 也相同。
GCC 的动态行/列以及 MSVC 动态行/列仍有额外边界组织成本，后者还保留后端动态 mdspan
构造的栈临时量/security-cookie 与零轴清零分支；不是全平台零开销结论。
没有收集吞吐量、硬件分支/缓存计数器或对任意 stride 作普遍性能承诺。
