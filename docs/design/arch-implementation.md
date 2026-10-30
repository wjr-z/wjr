# arch 实施计划与验收

依据：[架构设计](arch-foundations.md)。执行模型：`gpt-6.1-sol`，reasoning `high`。
用户授权提交 MR，同时待审上限为 4；现有 #135 计入额度。各阶段不要求同时实现。

## A1：部分 SIMD 加载的无对齐前提修复（本轮实施）

范围：SSE 尾部的 16/32 位读取，以及复用它们的 AVX 部分加载测试。

1. 读取当前头文件、`read_memory`、宏、单测注册和 CI；不要修改 third-party。
2. 写出覆盖奇数地址、所有合法长度与低位有效字节的测试；独立证明旧路径在
   alignment sanitizer 下失败。测试不得读取 AVX 未指定的高位。
3. 只替换尾部 typed dereference 为现有 `read_memory` 加字节偏移；保持 ISA 宏、
   API、内联方式、载入宽度、未指定高位和零长度行为。不增加 release 检查。
4. 用 exact-size allocation + ASan 或受保护页验证没有尾部越界；空长度允许空指针。
   测试独立的固定长度入口及运行时长度入口，避免只验证一个 switch 分支。
5. 与合法 fixed-size memcpy 基线及旧实现比较优化生成码。至少检查 SSE2、SSE4.1、
   AVX/AVX2 对应的配置，报告本机不能运行的 profile，不能执行宿主不支持的指令。
6. 用仓库 `tests` target 验证；本地 MSVC 使用新 build 或 clean-first，避免历史
   Ninja header dependency 问题。跑相关单测、现有 UTF-8/JSON 调用方以及 CTest。
7. 新文件显式格式化，执行 `python -m pre_commit run clang-format --all-files`；
   不为格式化暂存无关文件。按本任务清单提交。

验收：旧问题有复现；相关测试通过；无新增越界或高位契约；没有为修复增加热路径
检查/分配；GCC/Clang Linux 证据优先，Windows 证据单列；未完成部分明确列出。
若无本地 Linux，用已授权的 MR CI 或最小研究 workflow 获取证据，不能把交叉编译
结果称为原生吞吐。原生性能测试不得与重负载构建并行争抢 CPU。

## A2：扫描契约与测量基础

独立于 A1，仅在 A1 已形成可审查版本后进行。

- 固定 uint64 等值与不等值、正反向返回值语义，覆盖 0、短路径边界、向量/展开边界、
  first/middle/last/no-hit、多命中和自然对齐但非向量对齐地址。
- 用 scalar oracle 验证公共入口，不用实现本身计算 expected。
- byte-mask 和 lane-mask 转换必须有针对性测试；不要提前建立公有通用 SIMD 类型。
- 复用 `benchmarks/src/simd_scan.cpp`，固定和 shuffled 场景都比较，记录实际 CPU、
  compiler、flags、尾部处理、代码大小和方差。硬件 counters 未采集要明确说明。
- 只有生成码与测量支持时才修改正式扫描路径；否则交付测试与有结论的实验即可。

## A3：CPU provider 与构建边界

先证明 `BUILD_TESTING` 父工程污染或依赖初始化的具体问题，再在最小 CMake 路径修复。
验证 build-tree / installed consumer、无效配置、C++20/23、baseline/明确 ISA；
现有 `X86Info` 符号兼容，默认配置不增加 provider 或下载。不得扩大 Windows CI 矩阵。

## A4：ARM64 准入

先建立 AArch64/Linux 编译和 portable correctness 的证据，再验证 iOS toolchain/SDK
与设备。选择一个代表性扫描或填充内核比较 xsimd 与 NEON；缺原生设备时保留候选，
不宣布支持所有模块或同等性能。32 位和第三种架构不纳入本轮。

## MR、清理与交付记录

每个 MR 是完整能力，含必要测试和文档；不为凑额度拆分。最多 4 个待审，不自行合并。
使用独立 worktree，保留主工作区用户文件；不把不同 worktree 指向同一 build 目录。
共享依赖源码与编译缓存可以复用。清理前检查 tracked/untracked 文件、正在运行的
agent、build 的源目录引用、merged MR final head 或 patch equivalence。
远程删除使用精确 SHA lease；历史 mismatch、未合入研究输入和用户分支保留。

本轮验证结果与后续未完成项由实施者追加至下方；计划中的检查不代表已执行。

## 本轮结果

A1 已修复 `loadu_si48/80/96/112` 的五处尾部 typed dereference，改为
`read_memory<uint16_t/uint32_t>` 加字节偏移。没有改变 ISA 分支、API、加载范围或
高位语义，也没有新增运行时检查、分配或清零。新单测由现有 `tests` target 自动发现，
覆盖 SSE 的 1–8、AVX 的 1–16 个 16 位单元，逐一调用固定入口及运行时入口，
每种长度检查 0–31 字节偏移；仅比较有效前缀，零长度单独验证空指针。

验证基线为 `0f0288de`，修复源代码为 `1d9b79a8`。Linux 专项证据来自
[GitHub worker 运行 41000051238](https://github.com/wjr-z/wjr/actions/runs/41000051238)，
研究分支提交 `163d4c71`；原始 flags、汇编、sanitizer 日志和静态分析保存在该运行的
`arch-partial-loads-20261030` artifact。研究 workflow/probe 位于研究分支，没有加入
正式 CI 或安装目标。

| 平台与工具链 | 已完成验证 | 结果 |
| --- | --- | --- |
| Ubuntu 24.04 x86-64，GCC 13.3、Clang 18.1.3 | C++20，SSE2/SSE4.1/AVX/AVX2，独立全长度 probe，ASan + alignment sanitizer | 八组新实现均通过；旧实现均报告奇数地址的 `uint16_t` 非对齐读取 |
| 同一 Linux worker | `-O3 -DNDEBUG -march=x86-64` 加对应 ISA 参数，48/80/96/112 位固定入口与旧实现、合法 scalar memcpy + intrinsic 基线比较 | 32/32 组逐指令一致；没有新增调用、分支、栈写或宽读取 |
| Windows x64，Clang 18.1.8，i5-13500HX | C++20 Release/native，仓库 `tests` target、完整 CTest（含 JSON/UTF-8 调用方） | 12/12 CTest 通过；新 SSE/AVX 单测通过 |
| 同一 Windows 工具链 | SSE2/SSE4.1/AVX/AVX2 独立 probe，ASan + alignment sanitizer，以及优化汇编比较 | 四组新实现通过、旧实现失败；16/16 固定入口与旧实现和合法 intrinsic 基线逐指令一致 |
| Windows x64，MSVC 19.42 | fresh CMake 构建；新单测另行编译链接运行，baseline 与 `/arch:AVX2` | 新测试分别 1/1、2/2 通过；完整 `tests` 构建被既有 preprocessor 测试阻断，见下文 |
| 格式与补丁 | 新测试显式格式化并暂存后，`python -m pre_commit run clang-format --all-files`、`git diff --check` | 通过 |

ASan probe 为每个长度和偏移分配精确大小的字节数组，使有效前缀终点与分配终点
重合；任何尾部宽读取都会进入 redzone。运行时入口通过 noinline 包装保留动态长度。
Windows 结果与 Linux worker 结果分别记录，没有把 Windows 或交叉编译称为 Linux 实测。

上述逐指令一致证明的是本次正确性修复没有增加既有路径成本，尚不证明所有路径已达到
相同语义的优秀手写上限。额外比较了零初始化 vector + 固定长度 memcpy，以及 96 位
`unpacklo_epi64` 候选。SSE2 的现有路径有三次读取及两次串联 insert，而 unpack 候选
只需两次读取和一次 shuffle，指令数相同也不能视为吞吐相同。该算法候选留给独立优化
验证；未运行吞吐 benchmark、未采集硬件 counters，不以静态调度模型代替真实测量。

本机 MSVC 19.42 的完整构建失败在 `test/preprocessor/preprocessor.cpp:129` 的字符串
static_assert。从 `0f0288de` 提取同一源文件，以同一 C++20 和 `/Zc:preprocessor`
参数独立编译也失败，确认不是此次 SIMD 改动引入；本轮未修改无关预处理器测试。
完整 Linux 仓库测试与现有 Windows CI 由 A1 MR 的常规工作流继续验证。
