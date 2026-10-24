WJR 的模块通过 CMake alias 提供。使用某个模块时，在自己的 CMake
target 上链接对应的 `wjr::<module>`；CMake 会自动传递头文件路径、C++
标准和模块依赖。

```cmake
target_link_libraries(my_app PRIVATE wjr::json)
```

如果不需要拆分依赖，直接链接总库 `wjr::wjr` 即可。下面列出当前面向用户的模块。

## `wjr::config`

该 target 集中提供公共头文件路径、最低 C++ 标准和影响头文件契约的配置宏。
编译告警默认保持私有。ISA 选项由独立的 `wjr::isa` 提供，只有数值和架构相关模块依赖它；
容器、迭代器、通用内存管理和断言模块不自动获得 native ISA 要求。

基础配置模块，提供平台、编译器、特性检测和库内部使用的配置定义。

通常不需要单独链接。只有在编写依赖 WJR 配置宏或基础配置类型的底层组件时，才适合直接声明这个依赖。

## `wjr::assert`

断言和调试辅助功能，供库内代码检查前置条件、内部状态和不可达路径。

编写与 WJR 风格一致的底层组件，且需要使用 WJR 断言接口时使用。
启用 `WJR_ENABLE_ASSERT_BACKTRACE` 后，该模块可以额外提供断言回溯信息。

普通模式报告表达式和位置；`WJR_LIGHT_ASSERT` 仅报告失败表达式，Release 默认采用此模式。
可选的 `WJR_MINIMAL_ASSERT=ON` 使启用的断言失败时直接调用零参数
`std::terminate()`，不准备或求值诊断参数，也不引入 `<iostream>`。
它优先于 light 模式，适合调用方接受缺少断言文本和回溯的构建；断言条件及分级规则保持原样。
直接包含头文件时，可在包含前定义同名宏。该选项通过 `wjr::config` 传递给消费者。
静态链接且其他代码也不使用诊断时，无须从 `wjr::assert` 提取包含 iostream 的实现；
共享库仍保留既有诊断导出函数及其固定成本。

只在某个失败路径使用最小处理时，可以直接调用零参数
`WJR_ASSERT_FAILED_HANDLER_MINIMAL()`；它不依赖全局模式，也直接展开为 `std::terminate()`。

## `wjr::iterator`

迭代器相关的基础工具和适配支持，为容器、算法及其他模块提供通用迭代器能力。
连续迭代器使用标准 C++20 tag，core 不再依赖 range-v3；兼容边界见
[标准 ranges 决策](../design/std-ranges.md)。

实现自定义容器或需要复用 WJR 迭代器工具时使用；只使用上层模块时通常会通过依赖传递得到它。

## `wjr::memory`

内存管理和分配器相关组件，包含库内数据结构使用的内存工具。

实现对分配策略有要求的容器或需要直接使用 WJR 内存组件时使用。普通业务代码通常不需要单独依赖它。

## `wjr::math`

数学和位运算相关的基础算法，部分操作会使用 x64 架构优化。

需要复用 WJR 数学算法，或实现依赖这些基础运算的组件时使用。项目会根据目标平台选择相应的架构实现。

## `wjr::biginteger`

大整数类型和运算支持，包含加减乘除等大整数操作，并在可用时使用架构优化。

需要处理超出内置整数范围的整数，或需要 WJR 大整数接口时使用。

## `wjr::json`

JSON 数据结构和解析、序列化相关功能。

程序需要读取、构造或输出 JSON 数据时使用。该模块依赖 `wjr::biginteger` 及 JSON 相关的架构实现。

## `wjr::ring_buffer`

环形缓冲区容器，适合先进先出、固定容量或连续生产消费场景。

需要低开销地暂存数据流、事件或生产者消费者之间的数据时使用。它依赖 `wjr::assert` 和 `wjr::iterator`。

## `wjr::tbb`（可选）

设置 `WJR_ENABLE_TBB=ON` 后提供 `<wjr/parallel/tbb.hpp>` 的同步
`wjr::tbb::parallel_for_chunks`，借用同一个 const callable 执行非重叠分块。
只通过 config package 发现系统或预编译 oneTBB 2021.11 以上版本，不下载或构建其源码。
普通 WJR/NP 不发现或链接 oneTBB；该模块只依赖通用配置、断言和 `TBB::tbb`，不引入 ISA 要求。
安装消费使用 `find_package(wjr CONFIG REQUIRED COMPONENTS tbb)`；
借用、继承取消及异常合同见[同步分块](../design/parallel-tbb.md)，
复用调用方 arena 的可运行例子见 [examples/tbb](../../examples/tbb)。

## `wjr::np`（可选、实验性）

设置 `WJR_ENABLE_NP=ON` 后提供 C++20 的多维借用 view，包含
`<wjr/np/view.hpp>` 并链接 `wjr::np`，直接按 mdspan 前置条件构造。
可选 try_make_* 薄函数位于 `<wjr/np/make_view.hpp>`；成功仍返回同一 alias，不增加 checked 数据结构或索引检查。
owner/eval 仍使用 shape_result 报告必要元数据失败，见[结果类型别名](../design/expected-results.md)。
它复用固定版本 Kokkos mdspan，并传递 expected 所需的 baseline 断言与迭代器依赖，
不依赖 `wjr::isa`，不默认链接到 `wjr::wjr`。
安装消费使用 `find_package(wjr CONFIG REQUIRED COMPONENTS np)`。
`<wjr/np/rank.hpp>` 提供复用调用方工作区的[单轴排名](../design/tensor-rank.md)，
支持并列规则、NaN policy 及连续/正 stride/padded view。
数据生命周期、未检查构造的前置条件和已测空间限制见
[借用数据模型](../design/tensor-data-model.md)与[核心标准](../design/np-core-standard.md)。`<wjr/np/tensor.hpp>` 提供
容器管理生命周期的连续 owner 及显式借用，见
[owner 契约](../design/tensor-owner.md)。正 stride owner 使用
`<wjr/np/strided_tensor.hpp>`，见[跨度与孔洞契约](../design/tensor-strided-owner.md)。`<wjr/np/copy.hpp>` 和
`<wjr/np/transform.hpp>` 提供预分配输出的复制、填充及单目/双目融合变换，
支持连续和正步长输入，见[逐元素计算](../design/tensor-elementwise.md)。
`<wjr/np/transpose.hpp>` 提供[二维转置借用](../design/tensor-transpose.md)；
与 `copy_to` 组合可显式重排数据。`<wjr/np/expression.hpp>` 已提供值表达式、广播与预分配 eval_to，
`<wjr/np/evaluate.hpp>` 提供独立拥有结果的 eval；见[表达式接口](../design/tensor-expression-api.md)。
显式 cast、表达式 terminal sum 与可选 xsimd policy 使用各自头文件，状态与依赖见核心标准。
预分配计算、列视图和转置复制的可运行示例见 [examples/np](../../examples/np)。
`<wjr/np/reduce.hpp>` 提供立即执行的全量 `sum` 和单轴 `sum_to`，
支持显式累计类型与 `keepdims`，见[规约契约](../design/tensor-reductions.md)。
`<wjr/np/scan.hpp>` 提供[预分配单轴前缀和](../design/tensor-scans.md)，支持精确原位计算。

## `wjr::np_cblas`（可选、实验性）

设置 `WJR_ENABLE_NP=ON` 和 `WJR_ENABLE_NP_CBLAS=ON`，通过 pkg-config 发现
OpenBLAS LP64 后提供 `<wjr/np/cblas.hpp>` 的 `dot`、预分配 `gemm_to` 和 `gemv_to`。
安装消费使用 `find_package(wjr CONFIG REQUIRED COMPONENTS np_cblas)`。
普通 `wjr` / `np` 不发现或链接该 provider；不引入 native ISA 编译要求。
适配只借用 float/double view，复用成熟 BLAS 内核，不构造中间 tensor。
布局、转置、整数 ABI、beta-zero 与 provider 线程责任见
[CBLAS 契约](../design/tensor-cblas.md)。

## `wjr::simd_xsimd`（可选、实验性）

设置 `WJR_ENABLE_XSIMD=ON` 后提供固定版本 xsimd 的 batch、谓词、有界扫描及连续变换内核。
包含 `<wjr/simd/xsimd.hpp>` 并链接 `wjr::simd_xsimd`；该模块显式依赖 `wjr::isa`，
不默认链接到 `wjr::wjr`。安装消费使用 `COMPONENTS simd_xsimd`。
架构相关类型、mask 顺序和后续扩展边界见 [SIMD 后端初版](../design/simd.md)。

## 内部模块

下面的 target 用于库内部实现，一般不建议应用程序直接依赖：

- `wjr::format-internal`：格式化实现的对象库，不是面向用户的独立模块。
- `wjr::arch-*`：架构相关实现，例如 `wjr::arch-math`、`wjr::arch-json`
  和 `wjr::arch-biginteger`。
- `wjr::asm-*`：NASM 汇编实现，只有启用对应架构汇编时才会生成。

应用程序优先使用上面列出的公共模块或总库 `wjr::wjr`，不要依赖这些内部 target 的名称和实现细节。

## 顶层头文件

常用功能提供顶层入口，应用程序可以直接包含：

```cpp
#include <wjr/json.hpp>
#include <wjr/ring_buffer.hpp>
```

其中 `wjr/json.hpp` 提供 JSON document 的公共入口，`wjr/ring_buffer.hpp`
转发到环形缓冲区容器。其他功能仍可直接包含对应目录下的头文件，例如
`wjr/math.hpp`、`wjr/biginteger.hpp` 和 `wjr/container/ring_buffer.hpp`。

## 使用方式

在 WJR 已作为子目录加入项目，或通过安装包 `find_package(wjr CONFIG REQUIRED)` 找到后，链接模块 alias：

```cmake
find_package(wjr CONFIG REQUIRED)

target_link_libraries(my_app PRIVATE wjr::biginteger wjr::json)
```

只需要构建某个模块时，可以构建对应的实际 target：

```sh
cmake --build build --target wjr-json
```

实际 target 名称用于构建命令；在 `target_link_libraries` 中应使用 `wjr::<module>` alias。
