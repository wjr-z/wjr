# 二维 transpose 借用

`<wjr/np/transpose.hpp>` 提供 `transpose_view(source)`，用于二维连续
`tensor_view<T, Extents, layout_right/left>`。返回的 view 交换两条轴，
`result(j, i)` 与 `source(i, j)` 引用同一个对象；不重排或复制元素。

```cpp
#include <wjr/np/tensor.hpp>
#include <wjr/np/transpose.hpp>

using shape = wjr::np::extents<std::size_t, wjr::np::dynamic_extent, 300>;
using matrix = wjr::np::tensor<double, shape>;
auto storage = wjr::np::make_tensor<matrix>(std::array<std::size_t, 2>{1000, 300});
if (storage) {
    auto rows = storage->view();
    auto columns = wjr::np::transpose_view(rows);
    columns(7, 12) = 2.0; // 修改 rows(12, 7)。
}
```

结果保留元素类型及 const 性、index_type 和交换后的静态 extent。
`layout_right` 变为 `layout_left`，反之亦然；连续维度及物理存储不变。
因此对结果做物理 flatten 得到相同的元素顺序，不能将其解释为按新行序复制。
需要实际行列重排时，显式调用 `copy_to(dst, transpose_view(src))` 写入预分配输出，
参见[逐元素计算](tensor-elementwise.md)；此接口不隐藏临时 buffer。
再 transpose 一次恢复原 view 类型、shape 和地址。

调用方须提供有效的源 view，包括可表示的 mapping 和存活、对齐且类型正确的存储。
接口为 constexpr/noexcept，不重复校验已成立的 shape/存储约束，不做指针算术，
不分配、不增加引用计数、不构造或移动元素。零 extent 的 nullptr view 可以转置；
结果仍为空，不能解引用。返回描述符的空间成本沿用现有 mdspan 后端，
不承诺所有编译器都将完全静态 view 压缩成一个指针。

owner 必须显式通过左值 `.view()` 借用；不从 owner 隐式生成 view，
也不绕过右值 owner 的借用限制。轻量临时 view 描述符可以作为参数，
但返回值不延长底层数据寿命，owner 被替换或销毁后仍会失效。
`const view<T>` 保持可写，只有 `view<const T>` 表达只读元素。

初版不接受自定义 accessor、动态 rank、任意轴排列或 layout_stride。
left/right 的互换直接复用既有 mdspan mapping，不建立新布局或 evaluator。
stride 转置需要额外处理固定后端在空 shape 上的 observer 溢出顺序问题，
不能仅交换数组后宣称任意已验证 mapping 仍可安全调用所有 observer。
后续扩展需先决定该后端限制的处理方式。
