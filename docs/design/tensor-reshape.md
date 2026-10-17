# 连续 view 的 flatten 与 reshape

`<wjr/np/reshape.hpp>` 提供两个只改变描述符的入口，仍链接可选 `wjr::np`。
它们复用固定 mdspan 类型，不分配、不复制元素，也不持有 owner 或延长对象寿命。
源必须是具有有效 metadata 和可寻址存储的 CPU `tensor_view`，其对象数可表示为 size_t；
这不是重新校验任意源描述符的入口，不再次检查源对象数。

```cpp
using shape = wjr::np::extents<std::size_t, 2, 3>;
wjr::np::tensor_view<double, shape> panel(data);
auto flat = wjr::np::flatten_view(panel);  // 静态一维 extent 为 6。
auto reshaped = wjr::np::try_reshape_view<
    wjr::np::extents<std::size_t, 3, 2>>(panel, std::array{3, 2});
```

## flatten

`flatten_view(source)` 返回一维 view，沿物理存储顺序访问原来的对象。
全静态 shape 保留编译期总长度；包含动态 extent 的 shape 使用一维动态 extent。
全静态乘积在乘法前检查可表示性，不能溢出 index_type/size_t，也不能等于后端的
dynamic_extent 哨兵；不满足时给出编译错误。静态零 extent 得到静态长度 0。
rank-0 标量得到长度 1，而空 shape 得到长度 0；不会对空指针做指针算术。

这是具有有效源 view 前置条件的普通描述符适配，不新增运行时 shape 检查。
原始 view 构造的长度、算术和存储前置条件仍由调用方负责。

## checked reshape

`try_reshape_view<NewExtents>(source, full_shape)` 返回
`expected<tensor_view<T, NewExtents, 原布局>, shape_error>`。
完整 shape 包含 rank 个整数，静态维度也要给出并匹配；不接受 bool、浮点或负维度推断。

新 shape 复用 `try_make_tensor_view` 的一次检查：逐维检查符号和可表示性、静态匹配，
再检查索引乘积、空 shape 的中间 stride、字节跨度和存储长度，然后才构造目标 mapping。
目标逻辑元素数必须与源完全相等；较大和较小都返回 `element_count_mismatch`，
其他 metadata 错误保留已有错误值。算术错误先于元素数不匹配。
返回普通 view 后，索引与热循环没有额外 reshape 检查。

两个接口都只接受默认 accessor 及类型中明确的 layout_left/right。
不接受 layout_stride，即使某个运行时实例恰好连续；也不接受自定义 accessor、广播或填充。
布局类型保留，因此 reshape 不执行转置或 row/column 布局转换。
例如同一物理序列 `[0,1,2,3,4,5]` 改为 `[3,2]` 后，row-major 的 `(1,0)` 为 2，
column-major 的 `(1,0)` 为 1。

## 借用与边界

元素 const 性原样保留：`view<const T>` 的结果只读，`const view<T>` 的结果仍可写。
临时 view 描述符可以传入，前提是底层对象仍存活；接口不接受容器或 owner 的隐式接入。
已借出的 view 不会使 owner 的移动、赋值或销毁变安全，生命周期继续由调用方管理。
这些入口不新增共享引用计数、运行时状态或第二套表达式引擎。
