# 表达式物化为拥有数据的 tensor

`<wjr/np/evaluate.hpp>` 提供 `np::eval(expression)`。它分配一个结果容器，
将表达式融合写入该容器，返回 `expected<tensor<...>, shape_error>`。
即使输入只是单个 view，结果也始终独立拥有元素，不会返回借用对象。

```cpp
auto x = np::as_expr(input.view());
auto result = np::eval((x - 0.5) * (x - 0.5));
if (result) {
    auto values = result->view();
    // values 借用 result；result 必须保持存活且未改变其存储。
}
```

结果保留表达式逐节点推导的 `value_type`、编译期 extents 和 packed layout。
首版保留 `layout_left`，不会暗中转换为行优先布局；输入仍遵循当前表达式的
相同 shape、相容遍历顺序约定。不同 shape 在分配前返回
`shape_error::expression_shape_mismatch`。通用广播和指定输出布局留给后续扩展。

纯标量表达式产生 rank 0、一个元素的 owner。空数组保留所有维度，且不读取输入。
默认容器为 `std::vector<T>`；显式 allocator 可通过以下重载传入，其 `value_type`
必须与表达式结果完全相同：

```cpp
auto result = np::eval(expression, std::allocator_arg, allocator);
```

shape 的索引范围、乘积、字节跨度和容器容量检查直接复用 `make_tensor`。
分配失败沿用 allocator 的异常策略，不被转换为新的错误包装。`bool` 结果暂不支持，
因为默认的 `std::vector<bool>` 不能提供普通 `bool*` 存储；bool 输入的整型提升结果
仍可物化。owner 必须先显式调用 `.view()`，不会被计算接口隐式借用或复制。

普通 `eval_to` 的无隐藏分配约定没有改变。`eval` 是明确的物化边界，默认 vector
会先初始化元素，再由融合循环写入；首版没有承诺省去初始化写入，也不会通过写入
未构造的 vector capacity 来规避 C++ 对象生命周期规则。性能敏感的重复计算可以
复用已有输出，并调用 `eval_to`。

[`examples/np/evaluate.cpp`](../../examples/np/evaluate.cpp) 展示平方偏差的一次融合物化，
运行后输出：

```text
materialized squared deviations:
6.25 2.25 0.25
0.25 2.25 6.25
```
