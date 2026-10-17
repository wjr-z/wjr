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

## 为什么暂不直接改 iterator tag

WJR 目前使用 range-v3 0.12 的独立 contiguous tag。标准库优先读取
`ITER_TRAITS(I)::iterator_concept`；range-v3 对 primary 与自定义
`std::iterator_traits` 采用同样的来源选择，再优先读取 `iterator_concept`。
仅在 traits 中换成标准 tag，会让 range-v3 丢失 contiguous 识别。
两个独立 tag 的双继承则形成有歧义的 `std::random_access_iterator_tag` 基类，
不能满足两侧要求的 `derived_from`。参见
[标准概念来源规则](https://eel.is/c++draft/iterator.concepts.general)、
[range-v3 的概念来源](https://github.com/ericniebler/range-v3/blob/a81477931a8aa2ad025c6bda0609f38e09e4d7ec/include/range/v3/iterator/concepts.hpp) 和
[独立 tag 定义](https://github.com/ericniebler/range-v3/blob/a81477931a8aa2ad025c6bda0609f38e09e4d7ec/include/range/v3/iterator/traits.hpp)。

显式桥保留现有识别和布局，使标准算法通过标准视图工作；它不代表 WJR 的迭代器已经满足
`std::contiguous_iterator`。后续更换 range-v3 依赖或采用其公开兼容机制时，再评估直接互操作。

## 为什么不注册 borrowed_range

checked iterator 保存了指向 WJR 容器或视图对象的指针。即使底层数组仍有效，
临时 WJR span 销毁后，这个检查上下文也已经失效。因此不为 WJR span 注册
`std::ranges::enable_borrowed_range`。标准 span 本身是
[borrowed range](https://eel.is/c++draft/span.syn)，标准算法可以安全地从桥接后的临时标准描述符返回迭代器，
前提仍是底层数据继续有效。

Clang 18、C++20/23、Windows x64、`-O3 -march=x86-64`、`WJR_DEBUG_LEVEL=0`
下，6 组转换、往返及元素访问探针的函数指令与直接 pointer/count 构造对照一致；
不据此承诺任意调用场景的吞吐量。
