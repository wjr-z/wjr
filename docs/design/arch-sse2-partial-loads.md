<!-- markdownlint-disable MD043 -->
<!-- This record uses experiment-specific headings. -->

# SSE2 partial-load：已采用的实现与测量记录

96 位 SSE2 fallback 的 `unpacklo_epi64` 优化已由 #139 合入；当前实现见
`include/wjr/arch/x86/simd/sse.hpp` 的 `sse::loadu_si96`。本页保存实验基线和历史证据，
不构成所有平台、编译器或长度分布均最优的声明。基础的无对齐读取合同及 #137 修复见
[实施记录](arch-implementation.md)。

## 实现合同与对照

研究以已包含 #137 修复的 `6a72906d` 为源码基线，只改 `!SSE4_1` 的 `loadu_si96`；
`loadu_si112` 复用它，不改 SSE4.1 默认路径或公共 API。旧 SSE2 96 位实现读取 8+2+2
字节并串联两次 16 位 insert；采纳的路径读取 8+4 字节并 `unpacklo_epi64`。112 位路径
在第六个 16 位 lane 插入尾部。两者保留 SSE 高位零值、任意字节地址及精确读取范围，
不增加检查、分配或清零。总读取字节仍为 12/14，读取次数由 3/4 降至 2/3。

合法性能基线使用固定大小 scalar `memcpy` 建立整数值，再调用相同 intrinsic；旧的
非对齐 typed dereference 不作为性能上限。比较独立 load wrapper 和被真实累加操作消费的
批循环，避免只测函数调用。实验用 GCC/Clang Linux C++20 SSE2，并以未改的 SSE4.1/AVX2
作对照；同时核对 0/1/15 字节偏移、固定/交替/固定 seed shuffled 长度、8 KiB/1 MiB
工作集。计时不生成数据或分配，先编译后绑定可用 CPU，以四个独立累加器串行轮换三种
实现。汇编与 LLVM-MCA 分别作为静态证据；二者不能替代真实运行时间。

## 历史实测

两轮均为 Ubuntu 24.04 x86-64、GCC 13.3 / Clang 18.1.3、C++20、
`-O3 -DNDEBUG -march=x86-64 -fcf-protection=none`；GCC 另加 `-fno-ipa-icf`，
对照 profile 加 `-msse4.1` 或 `-mavx2`。首轮为 [Xeon 8573C](https://github.com/wjr-z/wjr/actions/runs/41001041384)，
提交 `342f085e`，每样本 64 MiB；复验为 [EPYC 9V45](https://github.com/wjr-z/wjr/actions/runs/41001317102)，
提交 `90e12255`，每样本 256 MiB，保留全部九个原始样本。完整命令、CPU、汇编、CSV 和
MCA 输出在两轮的 `arch-sse2-throughput-20261030` artifact。

下表为 SSE2 候选/原实现的耗时比（小于 1 表示更快）。每格汇总六个工作集/偏移组合
各自九轮中位数的中位数；所有组合及高方差样本均保留。

| CPU / 编译器 | 固定 96 | 固定 112 | 交替 | shuffled |
| --- | --- | --- | --- | --- |
| Xeon / Clang | 0.682 | 0.818 | 0.794 | 0.969 |
| Xeon / GCC | 0.673 | 0.819 | 0.712 | 0.825 |
| EPYC / Clang | 0.681 | 0.760 | 0.632 | 0.832 |
| EPYC / GCC | 0.680 | 0.754 | 0.562 | 0.762 |

固定长度与交替场景在两台 worker、两种编译器上均受益。首轮 Clang shuffled 的最差单项
比值为 1.063，最高 CV 约 0.091；复验的 SSE2 shuffled 所有组合小于 1。未改的
SSE4.1/AVX2 对照也有明显 shuffled 波动，因此不能泛化单个组合差距。1 MiB 是较大缓存
工作集而非 DRAM；未采集硬件 counters，不宣称所有平台最优。

## 生成码、模型与正确性

GCC/Clang 的 SSE2 96/112 位候选 wrapper 均与合法手写 unpack 基线逐指令一致；inline
批循环无新增调用或栈写，Clang 的候选与手写批循环也相同。LLVM-MCA 18 Skylake 模型
估算 96 位 block throughput 从 2 降至 1、112 位从 3 降至 2；Znver3 分别从 3.5 降至
1、从 5 降至 2.5。已排除入口标记和 `ret`。这些是静态调度估计，不是实测延迟或吞吐。

Linux 六种编译器/ISA 组合均通过 exact-size ASan + alignment sanitizer。Windows MSVC
19.42 的 SSE2/AVX2 独立检查通过；SSE2 wrapper 从 5/7 条降至 3/5 条指令并与手写基线
一致，AVX2 默认路径相同。生产改动另通过 Windows Clang 18.1.8 全长度 sanitizer probe、
MSVC 全长度 probe 与 SSE 针对性单测；仓库 Clang Release/baseline 构建及 CTest 19/19
通过，含 JSON/UTF-8 调用方。required all-files 格式检查通过。

复杂或可能退化的内核改动才需性能测量；本次已有两轮跨 CPU 证据，无须为相同结论重复
扩大基准。简单薄包装按风险做正确性、生成码和格式验证即可。
