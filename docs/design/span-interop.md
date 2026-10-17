# span 的显式标准库互操作

包含 `<wjr/span_std.hpp>` 后，`as_std_span(wjr::span<T, N>)` 返回
`std::span<T, N>`，`as_wjr_span(std::span<T, N>)` 返回 `wjr::span<T, N>`。
两者只传递 `data()` 和 `size()`，保留静态长度、元素 cv 限定和空视图语义，不分配内存。
接口从 span 描述符推导类型，不提供容器或通用 range 重载；先显式建立视图，再进行桥接。

```cpp
#include <algorithm>
#include <wjr/span_std.hpp>

int values[] = {4, 2, 1, 3};
auto standard = wjr::as_std_span(wjr::span(values));
std::ranges::sort(standard);
auto local = wjr::as_wjr_span(standard);
```

桥接不保留已有迭代器或其检查上下文。标准视图的访问、迭代和诊断遵循标准库的前置条件，
不会沿用 WJR 的 checked iterator 检查。转换回 WJR 后，新创建的迭代器仍按
`WJR_DEBUG_LEVEL` 绑定到新的 WJR 视图对象。桥接不延长底层数据的生命周期；
临时描述符可以转换，但示例中的 `values` 必须继续存活。

## 原生标准迭代器与显式桥

WJR contiguous iterator 已使用标准 tag，可以直接传给标准 ranges 算法；
WJR vector 的左值也可以直接构造标准 span。显式桥仍用于保留已有 span 的静态 extent，
并明确选择只携带 pointer/count 的标准描述符。range-v3 的兼容变化见
[标准 ranges 决策](std-ranges.md)。

标准算法可以利用 contiguous iterator 的 `to_address` 进行 bulk 操作，
不保证逐元素触发 WJR 迭代器检查。直接调用 WJR 迭代器操作时，仍保留原有检查；
使用标准算法和视图时仍需满足其范围及对象生命周期的前置条件。

## 为什么不注册 borrowed_range

checked iterator 保存了指向 WJR 容器或视图对象的指针。即使底层数组仍有效，
临时 WJR span 销毁后，这个检查上下文也已经失效。因此不为 WJR span 注册
`std::ranges::enable_borrowed_range`。标准 span 本身是
[borrowed range](https://eel.is/c++draft/span.syn)，标准算法可以安全地从桥接后的临时标准描述符返回迭代器，
前提仍是底层数据继续有效。

Clang 18、C++20/23、Windows x64、`-O3 -march=x86-64`、`WJR_DEBUG_LEVEL=0`
下，6 组转换、往返及元素访问探针的函数指令与直接 pointer/count 构造对照一致；
不据此承诺任意调用场景的吞吐量。
