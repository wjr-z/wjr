# 预分配单轴前缀和

`<wjr/np/scan.hpp>` 提供 `cumsum_to<Accumulator = void>(out, input, axes<Axis>)`，
链接可选 `wjr::np`。这是立即执行的 inclusive prefix sum：每条轴线从零开始，
依次读入索引 0、1、… 的输入、更新独立累计值，并将本次前缀写到对应输出位置。
没有表达式树、隐藏临时数组、动态分发或自动并行。

```cpp
namespace np = wjr::np;
int source[] = {1, 2, 3, 4};
int prefix[4];
np::cumsum_to(np::tensor_view<int, np::extents<std::size_t, 4>>(prefix),
              np::tensor_view<const int, np::extents<std::size_t, 4>>(source), np::axes<0>);
// prefix = {1, 3, 6, 10}
```

当前只接受一个编译期轴，输入输出 shape 相同，rank 至少为 1。
例如二维行前缀使用 `axes<1>`，列前缀使用 `axes<0>`；不同线的累计值各自从零开始。
每条线始终沿指定轴递增，不重排该线的加法；独立线可以交错执行。
不支持空/多轴列表、rank-0、exclusive scan、初始值参数或通用 scan 运算符。

## 累计类型与数值语义

沿用[sum 规约](tensor-reductions.md)的类型规则：默认累计类型为 C++ 的 `decltype(T + T)`，
与输出 dtype 独立；可显式指定已经完成整数提升的算术类型，例如 `cumsum_to<double>`。
每次先把输入转换为累计类型再相加，输出转换发生在相加之后。

累计状态保存在独立的局部标量中，不从已转换的输出重新读取。
例如输入 double `{.75,.75,.75,.75}` 写入 int 输出得到 `{0,1,2,3}`，
而不是把每步已截断的输出用于下一次加法。
float 输入写 double 输出，默认仍是 float 累计；需要更宽中间值时必须显式选 double。
小整数默认提升为 int，真实 bool 输入可累计为计数；bool 输出仅按普通最终转换得到非零标记。

普通 C++ 转换、浮点舍入/NaN/无穷大及无符号取模规则保持有效。
有符号累计不得溢出，浮点转整数及其他输出转换须满足 C++ 可表示性要求。
没有逐点溢出检查、NaN 跳过、饱和算术、重排规约或隐式 fast-math。
连续浮点前缀依赖上一累计值；本接口不承诺 SIMD 横向 scan 或跨 ISA 逐位一致。

## View 与别名契约

接受默认 accessor、非 volatile 算术元素及有效的 left/right/正 stride view。
输出必须可写，所有 view 的对象寿命、存储和映射由调用方保证，owner 显式经 `.view()` 借用。
已知静态 shape 不匹配或轴越界在编译期拒绝；动态 shape 在入口通过 `WJR_ASSERT_L2` 检查一次。
任意零 extent 均在 stride 观察、指针偏移和数据读取前返回，允许有效的 nullptr 空 view。
只写逻辑元素，padding 保留；全部入口 constexpr/noexcept，计算本身不分配。

输入与输出必须逻辑不相交，或每个对应索引引用完全相同的对象。
精确原位 `cumsum_to(view, view, axes<Axis>)` 可用：先读取本元素并更新累计值，再覆盖该元素，
之后不会依赖已覆盖的旧值。singleton 轴上的不同无效 stride 不影响这一规则。
错位切片等其他交叠违反前置条件，不做地址范围扫描、`restrict` 假设或隐藏暂存。
物理跨度交叠但实际元素不交的列仍可直接计算。

## 使用与验收

[prefix_sums.cpp](../../examples/np/prefix_sums.cpp) 展示预分配的一维前缀，
通过两端前缀之差求固定宽度区间和，再组合列前缀与行原位前缀生成二维离散积分表。
这是已公开接口的直接组合，不引入量化业务规则或新表达式系统。
浮点前缀相减可能发生消去；它不自动等价于为每个窗口重新累计，调用方应选择合适精度。

针对性测试覆盖 constexpr、1D/2D/3D、行列/中间轴、左右布局、stride/padding、
原位及逻辑不交的同数组切片、空域、窄 index、真实 bool、输出转换反例和显式宽累计。
额外验证无异常配置及库操作期间的 allocation counter。
代码生成对照使用相同的有序算术及别名语义；不把指令数或小数组示例当作吞吐量结论。

Clang 18/GCC 12 的 O3 和 MSVC 19.42 的 O2 下，一维连续、显式宽累计、原位、
动态 stride 四组 probe 均与手写静态指令数相同，且无包装调用。
二维动态行/列路径仍存在编译器相关成本：Clang 的列路径展开为更多指令，
GCC 行列的边界组织略有不同，MSVC 保留动态 mdspan 元数据栈临时量/security-cookie。
这些结果不构成所有 rank/layout 的零开销证明；未收集吞吐量或硬件计数器。

二维连续行布局的列前缀使用四条独立累计链，并保留单列尾部；与单轴求和共享遍历。
这让编译器可以跨独立列向量化，保持每列的累计顺序与输出转换时机。
详见[轴计算开销对照](../performance/tensor-axis-kernels.md)。
