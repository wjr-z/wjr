# Tensor 借用数据模型

本轮确定 CPU 借用描述符的最小数据结构，并提供实验性的 `wjr::np` 模块。
它不分配、不延长数据寿命。owner、有限切片、广播与 evaluator 已有独立 API；
共同不变量和准确状态见[核心标准](np-core-standard.md)。此接口尚未承诺稳定 ABI。

## 选择与版本边界

采用 Kokkos mdspan 的固定历史快照
[`0e4eba59ad53451784b3839bdf98b25fa2d64931`](https://github.com/kokkos/mdspan/tree/0e4eba59ad53451784b3839bdf98b25fa2d64931)，
提交时间 2025-05-12。它的 CMake 项目版本仍为 `0.6.0`，但**不是**旧的
`mdspan-0.6.0` release；版本号不足以识别所用源码，升级必须核对完整提交。
许可为 Apache-2.0 WITH LLVM-exception。构建可通过 FetchContent 的源码覆盖变量离线使用同一快照。

该快照已包含后续构造约束修正，提供所需的 C++23 mdspan 子集。
没有采用审查时的 stable HEAD `d1636dadb9f7a57fcca534d4c199b5e1b7666eaf`：
其新增 C++26 `at()` 中的 `throw` 在本地 Clang 18 的 `-fno-exceptions` 下使包含头文件就失败，
即使没有调用 `at()`。本轮不修改第三方代码，也不为未使用的新接口增加异常要求。

新模块要求 C++20，与 WJR 的最低标准一致；上游在 C++17 下仍有默认构造及 conditional explicit 的已知差异。
无论使用 C++20 还是 C++23，公开基础类型固定为这一 Kokkos 实现，
不按标准版本切换为 `std::mdspan`。标准 mdspan 将来通过显式适配器互操作。

候选取舍如下，结论仅针对当前数据结构层：

| 候选 | 当前用途与限制 |
| --- | --- |
| 固定 Kokkos mdspan | 直接复用 extents、mapping、accessor 与构造约束；本轮已测试所需子集，Windows 静态 view 的空间限制见下文 |
| 标准库 mdspan | 标准接口是互操作目标；当前 WJR 的编译器与 STL 组合不能统一要求其可用 |
| xtensor 0.27.1 | [固定版本](https://github.com/xtensor-stack/xtensor/tree/f115228a7fc277b9be1bef34572831f8fd53ac73)要求 C++20，适合继续比较 owner/表达式子集；本轮没有测得它更快或更慢，也未引入其表达式依赖 |
| 自建 extents/mapping | 当前没有额外语义需求足以抵消重复维护标准约束的成本 |

不公开继承后端类，也不增加一个只转发相同构造函数的包装类。
初版使用 alias，因此不会新增数据成员、虚函数或共享所有权；后端本身的表示成本仍需实测。
升级时重跑约束、尺寸、索引生成码、无异常编译及安装消费测试，再有意更新 pin。

## 首个接口

```cpp
#include <wjr/np/view.hpp>

using shape = wjr::np::extents<std::size_t, wjr::np::dynamic_extent, 300>;
using daily_panel = wjr::np::tensor_view<const double, shape>;

// data 必须覆盖 days * 300 个有效 double 对象，且在 view 使用期间存活。
daily_panel panel(data, days);
auto close = panel(day, security);
```

`tensor_view<T, Extents, Layout = layout_right, Accessor = default_accessor<T>>`
只保存后端的 data handle、mapping 和 accessor。
同时提供 `extents`、`dextents`、`dynamic_extent`、`layout_right`、`layout_left`、
`layout_stride` 和 `default_accessor`。不重导出整个后端命名空间。
mapping 内含 extents，不重复保存 shape。

- rank 固定在类型中，extent 可以静态、动态或混合；静态信息可用于常量传播。
- `view<const T>` 只读；`const view<T>` 仍可写。派生类数组不能隐式当作基类数组。
- 动态转静态 extent 需要显式构造；调用方仍须保证实际 shape 匹配且索引值可表示。
- 默认构造仅用于具有动态 extent 的 view。rank-0 表示一个标量，需要有效对象才能访问；
  含零 extent 的 view 为空，可以携带空指针，但不得解引用或对它执行指针算术。
- `layout_stride` 用于正步长唯一映射；逻辑大小与 required span 可以不同。
  负步长、零步长广播及未知重叠映射不在此接口支持范围内。
- `bool` 视图访问真实 bool 对象；过对齐对象仍要求调用方提供正确对齐的存储。
- 复制 view 只复制描述符。原始指针构造无法检测临时 owner，数据失效责任仍属于调用方。

这里的构造和 `operator()` 都遵循 mdspan 的**未检查前置条件**。
它们不是 NumPy/DLPack 的安全接入边界，不检查外部存储长度、溢出、stride 合法性或生命周期。
调用方在构造前满足 shape、跨度与对象有效性的前置条件；
不能先调用可能溢出的 required_span_size() 再事后判断是否合法。
连续 view 的零拷贝 flatten 与 checked reshape 见 [元数据适配](tensor-reshape.md)。
二维连续源的行/列借用见 [受限子 view](tensor-subview.md)。
调试边界检查、`at()`、通用切片或标准 mdspan 互操作尚未实现。

## 直接构造与迁移

普通借用直接使用 mdspan alias；可选 try_make_tensor_view/try_make_strided_tensor_view 函数仍可报告构造失败，
成功返回同一种 tensor_view。本次将这两个薄函数合并到 make_view.hpp，替换 checked_view.hpp/checked_strided_view.hpp 头名；
旧 include 迁移到中性的构造头；普通计算只需 view.hpp。不另造 checked/trusted/unchecked 数据结构，也不增加访问检查。

```cpp
#include <wjr/np/view.hpp>

using shape = wjr::np::extents<std::size_t, wjr::np::dynamic_extent, 300>;
wjr::np::tensor_view<const double, shape> panel(data, days);
// 可信调用方保证维度、对象跨度、类型/对齐和寿命；构造不扫描内存或再次验证。
```

mdspan alias 已是所需的薄接口，可信内部数据直接使用它。
需要恢复外部元数据错误时显式调用 make_view.hpp 中的 try_make_*；静态不匹配、负数、溢出和长度不足在构造前报告。
它们不能探测真实 allocation 或寿命，typed pointer 仍须有效、自然对齐且存活。
owner/eval 分配前必要的整数、跨度和容量检查留在内部，不 include 公共构造头或绕行其 buffer/null 验证。
未来 Python/设备适配若面对不可信元数据，在真实外部边界决定验证责任，不先为它建立通用校验框架。
正步长映射的直接构造与前提见[正 stride 借用](tensor-strided-view.md)。

索引固定使用 `view(i, j)`。`wjr::np` 通过公开 CMake 定义启用后端 paren 运算符、禁用 bracket 运算符，
使 C++20/23 使用相同拼写。整个程序直接使用此后端的翻译单元也须遵循相同宏配置与源码版本，
不可在别处先以不同命名空间或宏配置包含另一份 Kokkos mdspan。

## 可选依赖与安装

```sh
cmake -S . -B build -DWJR_ENABLE_NP=ON -DWJR_BUILD_TESTS=ON
cmake --build build --target np_tests
ctest --test-dir build -R wjr_np_tests --output-on-failure
```

源码消费时链接 `wjr::np`；它传递 `wjr::config`、固定后端及 baseline 的断言和迭代器依赖，
使 shape_result 等返回值在调试配置下也有完整的头文件和链接要求，不链接 `wjr::isa`。
默认 `WJR_ENABLE_NP=OFF` 不下载 mdspan，普通 `wjr::wjr` 不依赖 np。
启用安装时，上游头文件及 mdspan CMake 包随安装前缀安装，np 单独导出。

```cmake
find_package(wjr CONFIG REQUIRED COMPONENTS np)
target_link_libraries(research PRIVATE wjr::np)
```

普通 `find_package(wjr)` 不导入 np，也不查找 mdspan。
`OPTIONAL_COMPONENTS` 不可用时对应 `wjr_<component>_FOUND` 为 false；必需组件缺失会使包发现失败。
组件配置文件名为 `wjr-<component>-config.cmake`，仅装载请求的组件，重复发现可用。
如果集成项目已经定义上游 `std::mdspan` CMake target，应确保它对应同一快照及配置；
这个历史 target 名称并不表示 C++ 类型切换到 `std` 命名空间。
源码集成允许该 target 不参加 WJR 的导出；对应 mdspan 安装包由集成方提供。
安装发现也应使用同一安装前缀中的依赖，不能只凭 `0.6.0` 版本字符串接受另一份实现。

## 已测成本与限制

以下为本地 x64、Release、C++20 的尺寸，单位为字节；不是跨编译器 ABI 承诺。
静态 shape 为 `[2,3]`，混合为 `[dynamic,3]`，动态为二维，index_type 为 size_t。

| 编译器与 ABI | 静态 view | 混合 view | 动态 view | 静态/混合/动态 mapping |
| --- | ---: | ---: | ---: | --- |
| GCC 12.1 / MinGW x64 | 8 | 16 | 24 | 1 / 8 / 16 |
| Clang 18.1.8 / MSVC x64 | 16 | 16 | 24 | 1 / 8 / 16 |
| MSVC 19.42 / x64 | 16 | 16 | 24 | 1 / 8 / 16 |

静态 view 在后两种配置下**尚未达到一个指针的尺寸目标**。
静态 mapping 虽然只占 1 字节，但默认 MSVC ABI 下其成员和策略对象未能全部压缩，
与 8 字节句柄组合后产生 padding。不能用“零开销”概括这个尚存的空间限制。
试验性覆盖上游内部属性宏后，MSVC 19.42 可达到 8 字节，Clang 18/MSVC ABI 仍为 16；
本模块没有启用这组内部宏，以免将未声明的后端 ABI 开关变成隐含要求。
[Microsoft 的说明](https://devblogs.microsoft.com/cppblog/msvc-cpp20-and-the-std-cpp20-switch/)
解释了标准属性与 msvc 属性的 ABI 差异；后续应在更新编译器及候选后端时重新验证。

默认指针 view 的平凡复制、constexpr 索引、正确的 const/转换拒绝、row/column/正 stride 映射、
rank-0/空 shape、有状态 accessor、bool 与过对齐元素均有针对性测试。
索引探针比较同一输入下直接地址计算与现场构造 view 后索引：
Clang 18 的静态/动态索引生成相同指令；GCC 12 静态索引相同，动态索引指令数量及操作一致，寄存器分配不同。
这证明所测内联索引没有附加调用或分配，不能推广成吞吐量提升，也不能消除跨调用传递 16 字节描述符的成本。

## owner 的初步结构决策

此节保留链接锚点，初步方案现已实现为[owner 存储与 API 契约](tensor-owner.md)。
默认 tensor_vector + mapping_holder 复用容器生命周期；静态 packed mapping 压缩，动态 extents 按实例保存。
默认 owner 不再处于选型阶段，也没有自动采用 xtensor owner。正 stride owner 是本次审查接受的增量能力，
其完整 span 与状态在[核心标准](np-core-standard.md)中单独列明。

默认静态 owner 仍包含 vector 的 data/end/capacity；它不同于指针级 exact-storage 的表示下界。
既有容器带来的 allocator、异常和 moved-from 规则有维护价值，但不构成空间开销已消失的证据。
后续优化需用具体小 owner 工作负载证明收益，再决定是否新增 opt-in 存储。
