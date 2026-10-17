# 二维行、列借用

`<wjr/np/subview.hpp>` 提供 try_row_view(source, index) 和 try_column_view(source, index)，
返回包含一维 tensor_view 的 std::optional，链接可选 `wjr::np`。

```cpp
auto day = wjr::np::try_row_view(panel, day_index);
auto security = wjr::np::try_column_view(panel, security_index);
if (day) {
    // (*day)(i) 直接访问原对象，不复制数据。
}
```

首版仅接受 rank-2、layout_left/right、默认 accessor 的有效 CPU view。
不接受 layout_stride、自定义 accessor、owner 隐式转换或通用 slice 参数。
固定后端的 submdspan 在窄 index_type 上存在 stride 整数提升后列表初始化的编译问题；
首版要求 index_type 至少具有 int 的值位数，可使用 int、size_t 或 ptrdiff_t。
uint8_t/uint16_t 等窄 index_type 编译拒绝，不隐式提升输出类型或改写第三方头。
源的 metadata、存储和生命周期是前置条件；入口不重新校验 shape 或分配存储。
索引必须为非 bool 整数；负数和不小于选中维度的值返回 nullopt。
先在原类型上比较边界，再转换为 index_type；有效边界已证明可表示性，不重复检查宽度。

实际切片复用[固定版本 submdspan](https://github.com/kokkos/mdspan/blob/0e4eba59ad53451784b3839bdf98b25fa2d64931/include/experimental/__p2630_bits/submdspan.hpp)，
不实现第二套偏移和 mapping 推导。输出保留未选中维度的静态 extent，以及 T 的 const 性。

| 源布局 | 行 view | 列 view |
| --- | --- | --- |
| layout_right | layout_right，一维连续 | layout_stride |
| layout_left | layout_stride | layout_left，一维连续 |

const 描述符不改变元素可写性；元素为 const T 的源返回只读 view。
合法索引选到长度为 0 的行或列时，optional 有值且包含空 view，空指针句柄保持为空。
被选中的维度本身为 0 时，没有合法索引，返回 nullopt。

临时 view 描述符可以传入，但底层对象仍须存活。owner 应显式通过仍存活的左值 view() 借用；
这两个入口不绕过 owner 的已删除右值 view()，也不把 owner 注册为可借用范围。
结果不延长存储寿命；owner 的替换、移动或销毁仍可使已有借用失效。
索引边界检查只发生在创建行/列描述符时，之后的一维访问没有新增检查或引用计数操作。
