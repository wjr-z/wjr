<!-- markdownlint-disable MD043 -->
<!-- Semantic headings apply here; there is no required-heading template. -->

# SSE2 partial load 候选与验收

本实验基于已合入 MR #137 的非对齐读取修复，源码基线为 `6a72906d`。
只考虑 `!SSE4_1` 分支的
`loadu_si96`，以及复用该分支的 `loadu_si112`，不改 SSE4.1 默认路径或公共 API。
以相同契约的优秀手写实现为上限参考；静态模型仅用于提出假设。

## 候选与合同

当前 96 位 SSE2 路径读取 8+2+2 字节，再做两次串联 16 位 insert。候选读取
8+4 字节并 `unpacklo_epi64`：低 64 位保留首个读取，接上零扩展的 32 位尾部。
112 位路径继续在第 6 个 16 位 lane 插入尾部。两者都保留现有 SSE 高位零值、
任意字节地址及不越界的要求，不增加检查、分配或清零步骤。

合法手写强基线通过固定大小 scalar memcpy 建立整数值，再直接使用 intrinsic；
不能用原有非对齐 typed dereference 作为性能上限。同时比较独立 load wrapper
和 inline load 被真实累加操作消费的批循环，避免只测函数调用成本。

## 假设与实验

A1 的 Linux Clang 汇编经 LLVM-MCA 建模，SSE2 unpack 候选可能减少访存微操作和
insert 依赖。该静态估计不代表目标机器真实延迟或吞吐，仍需下列实测：

- GCC/Clang Linux，C++20 SSE2；SSE4.1/AVX2 作未改路径的对照。
- 固定 96/112 位、交替和固定 seed 的 shuffled 长度；8 KiB 与 1 MiB 工作集，
  0/1/15 字节偏移。四个独立累加器，轮换三种实现的测序，报告多轮中位数与分散度。
- 计时阶段不分配或生成随机数据，不与编译等重负载并行；记录 CPU、compiler、flags。
- 运行 exact-size allocation + ASan/alignment sanitizer，核对高位及有效前缀。
- 对照相同语义的手写基线，检查纯 wrapper 与批循环没有意外调用、栈写或宽读取；
  对 LLVM-MCA 排除入口标记和返回指令，分别列出静态模型与真实结果。
- Windows Clang/MSVC 进行 SSE2/AVX2 正确性及代码生成检查；本机数据单独记录。

只有真实结果支持收益且没有无法解释的回归，才把候选提升为生产改动和性能 MR。
未达到手写基线或存在噪声时保留实验结论，不以更短源码、指令数或模型预测代替证据。

## 结果

采用该候选，仅替换 `!SSE4_1` 的 96 位 fallback；112 位自然复用它。
未改 SSE4.1/AVX 默认路径，没有增加检查、分配或清零。读取总字节数仍为 12/14，
读取次数分别由 3/4 次降为 2/3 次。

两轮 Linux 实测均先完成编译，再绑一个可用 CPU 串行运行，未与编译并行：

- [首轮 Xeon 8573C](https://github.com/wjr-z/wjr/actions/runs/41001041384)，
  研究提交 `342f085e`，每个样本处理 64 MiB。
- [复验 EPYC 9V45](https://github.com/wjr-z/wjr/actions/runs/41001317102)，
  研究提交 `90e12255`，每个样本处理 256 MiB，并保留全部九个原始样本。

均为 Ubuntu 24.04 x86-64、GCC 13.3 / Clang 18.1.3，使用 C++20、
`-O3 -DNDEBUG -march=x86-64 -fcf-protection=none`；GCC 另加 `-fno-ipa-icf`，
对照 profile 加 `-msse4.1` 或 `-mavx2`。完整命令、CPU、汇编、CSV 和 MCA 输出
在两轮的 `arch-sse2-throughput-20261030` artifact。

下表为 SSE2 候选/原实现的耗时比，小于 1 表示更快。每格汇总六个工作集/偏移组合
各自九轮中位数的中位数；保留所有组合，未删除高方差结果。

| CPU / 编译器 | 固定 96 | 固定 112 | 交替 | shuffled |
| --- | --- | --- | --- | --- |
| Xeon / Clang | 0.682 | 0.818 | 0.794 | 0.969 |
| Xeon / GCC | 0.673 | 0.819 | 0.712 | 0.825 |
| EPYC / Clang | 0.681 | 0.760 | 0.632 | 0.832 |
| EPYC / GCC | 0.680 | 0.754 | 0.562 | 0.762 |

固定长度与交替场景在两个 worker、两种编译器上均受益。首轮 Clang shuffled
单个组合最差比值 1.063，CV 最高约 0.091；复验的 SSE2 shuffled 所有组合小于 1。
未改代码的 SSE4.1/AVX2 对照也有明显 shuffled 波动，不能把单个组合的差距泛化。
1 MiB 是较大缓存工作集，不代表 DRAM；未采集硬件 counters，不宣称所有平台最优。

GCC/Clang 的 SSE2 96/112 位候选 wrapper 均与合法手写 unpack 基线逐指令一致。
inline 批循环没有新增调用或栈写；Clang 的候选与手写批循环也相同。
LLVM-MCA 18 的 Skylake 模型估算 96 位 block throughput 为 2→1、112 位为 3→2，
Znver3 为 3.5→1、5→2.5；已排除入口标记和 ret，这些仍是静态模型估计。

Linux 六种编译器/ISA 组合的候选均通过 exact-size ASan + alignment sanitizer。
Windows MSVC 19.42 的 SSE2/AVX2 独立检查通过；SSE2 wrapper 从 5/7 条降至 3/5 条
指令，与手写基线一致，AVX2 默认路径相同。生产改动另通过 Windows Clang 18.1.8
全长度 ASan/alignment probe、MSVC 全长度 probe 和 SSE 针对性单测。
仓库 Clang Release/baseline 构建 `tests` 后，完整 CTest 19/19 通过，包含
partial-load 针对性单测及 JSON/UTF-8 调用方。required all-files 格式检查通过。

## 分级验证

简单薄包装及明显等价路径使用针对性正确性、必要生成码检查和格式检查即可。
复杂或可能退化的内核改动才补充性能测量；本次已有两轮跨 CPU 证据，不再为相同
结论重复扩大基准。后续按改动风险选择验证范围，避免让测量成本拖慢独立功能交付。
