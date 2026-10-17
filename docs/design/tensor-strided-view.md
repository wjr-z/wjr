# checked 正 stride 借用

`<wjr/np/checked_strided_view.hpp>` 提供 CPU 外部数据的正 stride 接入边界，链接可选 `wjr::np`：

```cpp
using shape = wjr::np::extents<std::size_t, wjr::np::dynamic_extent, 300>;
auto panel = wjr::np::try_make_strided_tensor_view<shape>(
    data, available_elements, std::array<std::int64_t, 2>{days, 300},
    std::array<std::int64_t, 2>{row_pitch, 1});
// expected<tensor_view<元素类型, shape, layout_stride>, shape_error>
```

stride 单位是 T 对象数，不能直接传 NumPy/DLPack 的 byte stride。
shape 与 stride 可以使用不同整数类型；两者都检查后再转成 index_type，不接受 bool 或浮点。
静态 extent 和 T 的 const 性保留。返回已有 mdspan alias，不增加标记、共享所有权或索引检查。
调用方仍需保证地址、类型、对齐、包含间隙的存储范围及整个使用期间的生命周期有效。

## 固定后端的合法输入子集

首版复用固定 Kokkos `layout_stride::mapping`，不实现新的布局。
其 [array 构造函数](https://github.com/kokkos/mdspan/blob/0e4eba59ad53451784b3839bdf98b25fa2d64931/include/experimental/__p0009_bits/layout_stride.hpp#L307)
只注释前置条件而不校验：stride 严格为正、required span 可表示为 index_type，
并存在轴排列使相邻轴满足 `stride[next] >= stride[previous] * extent[previous]`。
[P0009R18](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0009r18.html)
提供这一排列约束的背景；不能通过构造 mapping 后调用始终为 true 的 is_unique() 验证外部输入。

适配器在边界建立一次排列见证：零 extent 轴先放，剩余按 stride 升序；相同 stride 按 extent 升序，
使 singleton 轴排在前面。用除法比较相邻轴，不先计算可能溢出的乘积；末轴不需要 stride * extent。
非空 shape 下这验证了后端要求的排列条件，但不是任意映射的通用唯一性算法。
例如 `[2,2]` 配合 stride `[2,3]` 的四个偏移互不相同，却没有满足该构造前置条件的排列，
因此返回 `unsupported_stride_order`，不将它错误报告为已经证明重叠。

空 shape 使用保守的零轴优先顺序：`[2,0,2]` / stride `[1,100,1]` 有其他合法排列，
但本入口仍返回 `unsupported_stride_order`。同时复用现有 check_metadata 的完整 shape 检查，
包括空 shape 的无零连续段乘积约束；不宣称接受后端可能表示的全部空映射。
固定 rank 的局部数组和插入排序不分配内存，普通访问不再排序或重新校验。

## 跨度与失败

先验证全部 shape 和 stride，再检查排列、span 乘加、字节跨度和 available_elements，
最后才构造 extents/mapping/view。正常 metadata 错误返回 expected，不用断言报告输入错误。
非空所需对象数为 `1 + sum((extent - 1) * stride)`，rank-0 为 1，空映射为 0。
例如 shape `[2,3]` / stride `[8,1]` 的逻辑大小为 6，所需存储跨度为 11；
末行之后的额外 padding 可以存在，但不要求必须提供。

固定后端的 required_span_size() 按原轴顺序累加，到零 extent 才返回 0。
因此 `[2,0]` / stride `[INT_MAX,1]` 会在到零之前溢出；入口同样保护首个零前的每步乘加。
不会因为最终逻辑大小为 0 就跳过这些约束，也不会要求最后一轴的 stride * extent 可表示。
实际跨度还须满足 size_t、PTRDIFF_MAX 字节上界及所声明存储长度。

负数或零 stride 返回 `nonpositive_stride`，stride 不可表示返回 `stride_out_of_range`；
排列不在支持集合返回 `unsupported_stride_order`。其余 shape/算术/存储错误沿用 shape_error。
空 shape 的 nullptr/0 合法；非零 available_elements 不能携带 nullptr。

不支持负 stride、零 stride 广播、自定义 accessor、byte stride 自动转换或设备地址。
[P3959R0](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2026/p3959r0.html)
讨论了允许空映射的零 stride；本接口遵循固定快照的严格正数前置条件，不采用其中的放宽。
该入口可以描述行间填充和转置后的正步长数据，不引入 padded owner、表达式或 evaluator。
