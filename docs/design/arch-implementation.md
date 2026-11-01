<!-- markdownlint-disable MD043 -->
<!-- This record uses status headings rather than a fixed document template. -->

# arch 实施状态与验收

本文记录历史实施结果和仍未完成的验证，不把计划中的检查说成已执行。设计边界见
[arch 演进设计](arch-foundations.md)；SSE2 性能优化的完整测量见
[SSE2 partial-load 记录](arch-sse2-partial-loads.md)。

## 状态

| 工作项 | 状态 | 记录 |
| --- | --- | --- |
| A1：部分加载的对齐与对象类型修复 | 已合入 #137；下列结果是当时的历史验证 | 本页“A1 修复记录” |
| SSE2 96 位 fallback 优化 | 已合入 #139；保留为历史实测，不推断所有平台最优 | [原始数据、负例与复现](arch-sse2-partial-loads.md) |
| CPU provider CMake 作用域 | 已验证完成；不改变运行时行为 | [实现与验证细节](arch-cpu-provider-scope.md) |
| uint64 扫描契约及跨后端测量 | 后续工作；改正式路径前需完整边界与等价基线证据 | [设计与边界](arch-foundations.md) |
| AArch64 准入 | 未完成；需 portable 验证及原生工具链、设备和 workload | [平台边界](arch-foundations.md) |

## A1 修复记录

### 修复合同

历史基线 `0f0288de` 的 `include/wjr/arch/x86/simd/sse.hpp` 中，`loadu_si48/80/96/112`
有五处把尾部 `void*` 转成 `uint16_t*` 或 `uint32_t*` 后解引用。即使硬件允许非对齐
访问，这仍对 C++ 对齐和对象类型施加了不必要的前提；AVX 部分加载间接复用这些实现。
修复复用 `read_memory<T>` 与字节偏移，没有改变 ISA 分支、API、加载范围、高位语义或
零长度行为，也未增加 release 检查、分配或清零。#137 的单测覆盖奇数字节地址、所有
合法长度、固定及运行时入口，只比较有效字节；零长度单独验证空指针。

### 历史证据

修复验证源码为 `1d9b79a8`，基线为 `0f0288de`。Linux 专项材料保存在
[GitHub worker 41000051238](https://github.com/wjr-z/wjr/actions/runs/41000051238)
的 `arch-partial-loads-20261030` artifact；研究提交 `163d4c71`。结果仅描述当时快照和环境：

| 平台与工具链 | 当时结果 |
| --- | --- |
| Linux GCC 13.3 / Clang 18.1.3 | SSE2、SSE4.1、AVX、AVX2 共八组 alignment sanitizer 新路径通过、旧路径失败；32/32 个入口与合法手写基线逐指令一致 |
| Windows Clang 18.1.8 | Release/native 仓库 CTest 12/12 通过；独立 sanitizer 探针复现旧失败，16/16 组代码一致 |
| Windows MSVC 19.42 | 新单测通过；baseline 与 `/arch:AVX2` 分别 1/1、2/2。完整构建在无关的 `test/preprocessor/preprocessor.cpp:129` 字符串 `static_assert` 失败；从同一基线独立编译也失败 |

Linux worker 为 Ubuntu 24.04 x86-64，使用 C++20。Windows Clang 的完整 CTest 包含 JSON/UTF-8
调用方；独立 exact-size allocation 探针启用 ASan 与 alignment sanitizer，并用 noinline
包装保留运行时长度。汇编使用 `-O3 -DNDEBUG -march=x86-64` 及对应 ISA 参数。Windows
Clang/MSVC 证据不替代 Linux 原生吞吐；本次没有运行吞吐基准或采集硬件 counters，不能据此
声称性能收益。完整 Linux 仓库测试及既有 Windows CI 当时仍由常规工作流验证。

当时观察到的 SSE2 unpack 96 位算法候选在本页初稿中留待独立测量；它后来由 #139 采用，
其完整性能数字、负例、静态 MCA 估计与真实测量边界只在
[SSE2 结果页](arch-sse2-partial-loads.md)维护，避免复制两份结果。
