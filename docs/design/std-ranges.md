# Core 使用标准 C++20 ranges

Core 的 iterator tag、range concepts、访问操作和 subrange 使用标准 C++20 设施，
不再下载、导出或强制查找 range-v3。公开的 WJR traits、`from_range` 扩展、
allocator 接口及 `basic_common_reference` 定制点保留；本次不重写它们的独立契约。

## 选择依据

当前 core 只在四个头文件中直接使用 range-v3：iterator 分类、容器兼容 range concept、
vector 的范围构造和 ring buffer 的两个连续分段。它们都存在 C++20 标准接口，
没有使用 range-v3 独有的 view pipeline。直接使用标准库可以消除默认包的这项传递依赖。

range-v3 的 [0.12.0 release](https://github.com/ericniebler/range-v3/releases/tag/0.12.0)
和本次审查的 master 固定提交
[5ba39112](https://github.com/ericniebler/range-v3/tree/5ba39112d2eafbb3c851a16a537f29b445022998)
仍有独立的 contiguous tag。后者不是这里采用的新依赖。
[tag 定义](https://github.com/ericniebler/range-v3/blob/5ba39112d2eafbb3c851a16a537f29b445022998/include/range/v3/iterator/traits.hpp)
及 [概念来源](https://github.com/ericniebler/range-v3/blob/5ba39112d2eafbb3c851a16a537f29b445022998/include/range/v3/iterator/concepts.hpp)
说明升级该快照不能自动统一 tag。双继承又会产生有歧义的 random-access 基类，
因此采用原生标准接口，并明确接受以下兼容性变化。

## API 和约束边界

| 接口 | 当前约定 | 兼容性变化 |
| --- | --- | --- |
| `contiguous_iterator_tag` | `std::contiguous_iterator_tag` | 外部 range-v3 不再把 WJR iterator 识别为 contiguous |
| `is_contiguous_iterator_v` | 标准 contiguous 加原有数组、trivial move-iterator 扩展 | 仅有 range-v3 tag 的外部类型不再自动进入 WJR 连续路径 |
| `vector(from_range, range)` | 标准 input range，引用可转换为元素类型 | 定制遵循标准 concepts 和 traits；range-v3 专属注册不再生效 |
| `ring_buffer::array_range` | `std::ranges::subrange<pointer>` | 具体返回类型及依赖该类型的签名改变 |
| `ring_buffer::const_array_range` | `std::ranges::subrange<const_pointer>` | 保留只读元素访问和两段分解 |

标准范围约束不会额外要求 common range 或 legacy category。sized 和 forward range
继续先求长度并预分配；单遍 input range 只遍历一次，按原策略增长。
不同 iterator/sentinel 类型也必须能构造，不能只让约束成立而在函数体中失败。
局部输入迭代器通过移动传入构造辅助函数，支持 move-only 输入；没有 legacy category
的 common 输入使用已有的回滚循环，不交给依赖 legacy traits 的标准算法。
数组和 trivial move-iterator 仍是 WJR 扩展，不意味着它们符合标准 contiguous 概念。

已有用户如果依赖 range-v3 contiguous 算法，应在该边界显式建立其 pointer subrange，
并自行链接 range-v3。普通 WJR view 仍不负责数据生命周期；该桥也不延长底层存储的寿命。
需要更新受影响的重载、具体类型声明或 traits 定制，并一起重新编译相关翻译单元。

```cpp
// Application supplies its range-v3 dependency.
#include <range/v3/view/subrange.hpp>
#include <wjr/span.hpp>
int values[] = {1, 2, 3};
wjr::span view(values);
auto compatible = ranges::subrange(view.data(), view.data() + view.size());
```

WJR iterator 和容器的数据字段不变。标准算法可能通过 `to_address` 采用 bulk 操作，
不承诺逐元素保留 WJR 检查；直接的迭代器操作继续遵循检查模式。
checked iterator 仍引用容器或视图的上下文，因此不为 WJR span 注册 borrowed range。
关于显式标准 span 桥和临时描述符，见 [span 互操作](span-interop.md)。

## 已测布局和生成码

本地 x64、C++20、`WJR_DEBUG_LEVEL=0` 的前后对比如下。Clang 18 和 GCC 12 使用
`-O3 -march=x86-64`，MSVC 19.42 使用 `/O2`，没有 native 配置。

| 类型 | GCC 12 前后尺寸 | Clang 18 / MSVC ABI 前→后 | MSVC 19.42 前→后 |
| --- | ---: | ---: | ---: |
| 动态 span | 16 | 16→16 | 16→16 |
| 静态 span | 8 | 8→8 | 8→8 |
| span iterator | 8 | 8→8 | 8→8 |
| `vector<int>` | 24 | 24→24 | 24→24 |
| `ring_buffer<int>` | 40 | 40→40 | 40→40 |
| array_range | 16 | 24→16 | 24→16 |

受测类型的对齐均保持。分段描述符在 MSVC ABI 下的大小变化也是 ABI 变化，
不能用容器字段未变来代替这项检查。三个编译器上，span 随机访问及遍历、vector 访问、
ring 分段总长度及从标准 span 构造 vector 共 15 组探针的规范化指令一致；
这不等于完整负载的吞吐量结论。
