# 初始 tensor 数据模型

本轮确定 CPU 借用描述符的最小数据结构，并提供实验性的 `wjr::np` 模块。
它不分配、不延长数据寿命；owner、切片、广播、表达式及 evaluator 继续按
[总体设计](tensor.md) 单独推进。此接口尚未承诺稳定 ABI。

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
随后单独添加 checked 适配入口，在形成 mapping、指针或分配之前验证这些条件；
不能先调用可能溢出的 required_span_size() 再事后检查。
不将尚未实现的调试边界检查、`at()`、reshape、切片或标准 mdspan 互操作宣称为已有功能。

索引固定使用 `view(i, j)`。`wjr::np` 通过公开 CMake 定义启用后端 paren 运算符、禁用 bracket 运算符，
使 C++20/23 使用相同拼写。整个程序直接使用此后端的翻译单元也须遵循相同宏配置与源码版本，
不可在别处先以不同命名空间或宏配置包含另一份 Kokkos mdspan。

## 可选依赖与安装

```sh
cmake -S . -B build -DWJR_ENABLE_NP=ON -DWJR_BUILD_TESTS=ON
cmake --build build --target np_tests
ctest --test-dir build -R wjr_np_tests --output-on-failure
```

源码消费时链接 `wjr::np`；它只传递 `wjr::config` 和固定后端，不链接 `wjr::isa`。
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

下一步优先采用 **Container + mapping**，参照
[P1684R5](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p1684r5.html)。
容器拥有对象和 allocator，mapping 保存 shape；外层不再复制容器已有的指针、size、capacity、allocator。
首版只允许无填充的连续 layout_left/right；Container::data() 必须提供真实 T*，排除 vector<bool>。
自定义 allocator、alignment 及分配策略通过容器处理，而非附加一个通用共享控制块。

```cpp
// 仅结构草图，未实现 owner。
template <class Extents, class Layout, class Container>
struct owner_state {
    using mapping_type = typename Layout::template mapping<Extents>;
    wjr::compressed_pair<mapping_type, Container> state;
};
```

默认动态存储候选为已有 vector 类容器；小型完全静态 shape 可显式选 array 类容器，
不让静态大数组自动进入栈中。复制遵循容器值语义，allocator propagation/noexcept 由容器实际行为决定。
`view() &` 返回可写描述符，`view() const &` 返回只读元素描述符；`view() &&` 和 `view() const &&` 删除。
初始化先检查 shape/容量再创建容器；不暴露会破坏 mapping 与容器长度关系的可写 storage()。
移动后的静态 owner 可保留 shape 但失去动态存储，形成 view 前必须重新满足存储要求。

本地原型中，二维动态 mapping 加 Release std::vector<int> 为 40 字节（容器自身 24 字节，mapping 16 字节），
与这些必需状态之和一致。静态 mapping 加 std::array<int,6> 在 GCC 12 为 24 字节，
Clang/MSVC ABI 直接成员组合为 28 字节。原型没有实现 allocator/借用适配，也未测 xtensor owner 或自定义压缩 owner；
这些数据只支持继续评估组合存储，尚不足以定稿默认容器或承诺 owner API。
在实际 owner MR 中再验证强异常保证、状态 allocator、临时借用拒绝以及 checked shape 算术。
