# 正 stride 借用

`<wjr/np/view.hpp>` 的 tensor_view alias 直接组合固定 Kokkos mdspan mapping。
普通借用由可信调用方满足后端前置条件。可选 try_make_strided_tensor_view 仍保留为报告构造错误的薄函数，
与连续构造入口合并到 make_view.hpp；它返回同一种 mdspan，不是 checked view 数据结构。
构造函数使用中性 `make_view.hpp`；普通计算不依赖该头，见[核心标准](np-core-standard.md)。

```cpp
namespace np = wjr::np;
using shape = np::extents<std::size_t, np::dynamic_extent, 300>;
using view = np::tensor_view<const double, shape, np::layout_stride>;
const shape dimensions(days, 300);
const view::mapping_type mapping(dimensions, std::array<std::size_t, 2>{row_pitch, 1});
const view panel(data, mapping);
```

例中 days、row_pitch 和 data 必须已经满足下述条件，可信内部直接使用此路径。
若调用方需要失败结果，可显式调用 try_make_strided_tensor_view<shape>(data, available_elements, shape_values, strides)。
它先验证整数、排列与 span，再返回 shape_result<tensor_view<...>>；这是一次函数调用的选择，不改变后续 view 类型或索引。
stride 以元素数计数，不能直接把字节 stride 传入。保留静态 extent 和元素 const 性，
复制只复制 descriptor，不增加共享所有权、分配或逐索引检查。

## 合法输入子集

固定后端的 [layout_stride 构造](https://github.com/kokkos/mdspan/blob/0e4eba59ad53451784b3839bdf98b25fa2d64931/include/experimental/__p0009_bits/layout_stride.hpp)
要求每轴 stride 严格为正，required span 可表示，并存在轴排列使
`stride[next] >= stride[previous] * extent[previous]`。
调用 is_unique() 不能检查一个已违反构造前提的 mapping。
shape (2,2)、strides (2,3) 虽数学上单射，仍不满足此构造条件，不属于本接口子集。

非空 span 为 `1 + sum((extent - 1) * stride)`；shape (2,3)、strides (8,1) 的 size 为 6、span 为 11。
data 必须覆盖所需的有效对象范围，不只覆盖逻辑 size；不要求最后一行之后额外的 padding。
rank-0 需要一个对象，含零 extent 的逻辑域为空。
空域仍须满足后端执行的中间算术；不能先发生乘加溢出再依赖后续零 extent 返回 0。

自然对齐、真实类型、可访问范围和生命周期均由调用方承担。
标准 layout_stride 不接收 negative/zero stride；表达式广播在内部只读投影中使用零步长，
不构造这种非法 mapping。signed-stride 和外部 byte-buffer/device 适配是独立后续范围。

## 与 owner 和计算的关系

这是借用 view，不分配孔洞存储，也不维护 allocation 长度。
正 stride owner 必须在分配前验证完整 span，容器长度足以覆盖它；该责任留在 owner 内部。
view 可以直接传给现有正 stride 逐元素计算、表达式与 sum；每个入口另有 dtype/alias 前置条件。
任意相交/重复写 mapping 不因只读描述符构造成功就被授权作为输出。
