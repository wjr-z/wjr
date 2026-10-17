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

首版表达式要求数组同 shape，二维及以上还须同连续布局；普通 scalar 可以参与计算。
类型提升逐节点遵循 C++，不将中间整数除法提升成浮点运算。
范围和别名合同见 [表达式 API](../../docs/design/tensor-expression-api.md)。
