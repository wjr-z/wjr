<!-- markdownlint-disable MD043 -->
<!-- Semantic headings apply here; there is no required-heading template. -->

# arch 演进设计

审查日期：2026-10-30。审查源码快照：`0f0288dececb29f59e4248fee136db36fe1e6e44`；
它是历史设计基线，不代表当前源码状态。当前实现与验证结果分别见
[实施记录](arch-implementation.md)、[SSE2 partial-load 研究](arch-sse2-partial-loads.md)
及[CPU provider 构建隔离](arch-cpu-provider-scope.md)。

## 结论与平台边界

保留有价值的专用内核；先固定语义和安全边界，再用等价实现的生成码与实测决定优化。
不重写跨架构 SIMD 框架，不把所有操作改为运行时分派。

| 优先级 | 平台 | 决策依据与边界 |
| --- | --- | --- |
| P0 | x86-64 Linux，GCC/Clang | 主要性能依据；检查 baseline 与明确 ISA 下的正确性和代码生成 |
| P0 | x86-64 Windows，MSVC/Clang | 保持现有 CI；修复共用契约，不为单个编译器给公共路径增加开销 |
| P1 | AArch64 Linux、iOS | 先验 portable fallback，再以原生工具链、设备和 workload 决定是否试验 NEON |
| 暂缓 | 32 位 x86/ARM 与其他架构 | 当前根 CMake 不支持非 64 位；宏或目录存在不等于架构支持 |

不构造编译器、OS、ISA 的全组合矩阵，也不因 Linux AArch64 结果推断 iOS。AArch64
工具链范围是 Linux/iOS Clang 与 Linux GCC；只有有真实用户、工具链和测量设备时才引入
另一种架构实现，只为必要热点添加平台代码。

## 当前实现与接口约束

| 区域 | 当前边界 | 状态入口 |
| --- | --- | --- |
| 配置与 CPU 信息 | `wjr::config` 和 `wjr::isa` 分离；`ARCH_DEPENDENT` 控制 ISA 传递；CPU 探测不等于运行时分派 | [provider 构建隔离及验证](arch-cpu-provider-scope.md) |
| SIMD 与部分加载 | 静态包装、ISA 宏守卫和低 ISA fallback；修复任意字节对齐读取，并采纳经验证的 SSE2 96 位优化 | [实施记录](arch-implementation.md)、[SSE2 数据与复现](arch-sse2-partial-loads.md) |
| 查找与比较 | uint64 专用内核、短路径和展开；保留返回值、长度边界及现有 SSE4.1/AVX2 分支 | [实施记录](arch-implementation.md) |
| 填充与移位 | 保留专用 SIMD、`rep stos`、阈值与展开策略；混合长度测量后再动正式路径 | [SIMD 测量](../performance/simd.md) |
| 大整数及 JSON/UTF-8 | 保留专用进位链和字节位集；字节掩码与数值 lane mask 是不同合同 | 各自模块实现和测试 |
| 其他架构 | generic 配置、Windows 入口或 preview 分配适配不表示已有 ARM 数值优化 | 需逐模块验证 |

部分加载接受任意字节对齐，只读其有效的 `N/8` 字节；调用方负责提供该可读范围，
范围内无须先存在 `uint16_t`/`uint32_t` 对象。低位有效字节原样保留；AVX 从 SSE cast
继承的未指定高位不承诺清零。`loadu_si16x` 的长度单位是 16 位单元：SSE 支持 0–8，
AVX 支持 0–16，零长度不访问指针。修复不得增加有效路径检查、分配或所有权状态。

安全读取使用 `read_memory<T>(byte_pointer + offset)` 建立类型安全的值读取；已有
`loadu_si16/32/64` 路径已使用该机制。性能目标是
与同语义、固定尺寸 `memcpy` 手写基线生成等价代码；须检查 GCC/Clang 实际指令、内联、
访问宽度和分支，不能由 intrinsic 数量或 `memcpy` 名称推断零开销。旧实现若含未定义
行为，不能作为唯一性能基线。“零开销”以优秀同合同实现为上限参考，未测吞吐不写成收益。
指令语义、操作数和 ISA 要求参考[Intel Intrinsics Guide](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html)。

掩码操作留在 backend 私有层。uint64 比较的 byte movemask 每 lane 占 8 位，xsimd
compact mask 每 lane 占 1 位，不能直接互换；JSON/UTF-8 继续显式使用 byte bitset。
第二个真实消费者出现前，不引入有状态通用 mask、虚函数、函数表或统一整数编码。

## 后端复用与分派

下列固定版本是 2026-10-30 审查对象，不是“最新”声明，也不会因本文而启用依赖。

| 候选 | 边界与决定 |
| --- | --- |
| [cpu_features v0.10.1](https://github.com/google/cpu_features/tree/v0.10.1) | 保留现有依赖与公开 `X86Info`；用于探测，不会弥补以 native 编译的调用方 |
| [xsimd 14.3.0](https://github.com/xtensor-stack/xsimd/tree/e88a72831858123924f7118f345dfe5d70d95991) | CMake 已固定 commit；可作数值包装对比，逐项验证编译成本、mask、尾部和数学语义 |
| [Highway 1.2.0](https://github.com/google/highway/tree/457c891775a7397bdb0376bb1031e6e027af1c48) | 仅作静态/多目标分派边界参考；当前不新增依赖、多份内核或初始化成本 |
| WJR intrinsic/汇编 | 少量有价值的专用实现可保留；新手写代码须胜过成熟后端的等价基线 |

正式新增后端前须审查固定版本、许可、API 历史、维护、编译器覆盖、构建耗时和二进制大小。
详细测量入口见[已有 SIMD 结果](../performance/simd.md)，避免重做已否定的短数组填充分支。

## 后续工作与测量入口

1. uint64 find/compare 的完整边界、mask 和性能对照仍应独立验证；复用
   `benchmarks/src/simd_scan.cpp`，比较 WJR、xsimd 与允许自动向量化的手写循环；无收益时不改正式路径。
2. AArch64 先验证 portable 构建与正确性。具备原生 Linux/iOS 设备后，选一个代表性内核
   对比 xsimd 与 NEON；不把 SSE/AVX 成员函数整套复制到 ARM。
3. 只有确有单二进制跨 ISA 部署需求时，才评估 pointer/length/scalar ABI 的独立内核及
   baseline dispatcher；inline 类型和编译单元 ISA 必须保持一致。
4. CPU 信息与填充阈值的动态初始化仍是独立审计项；需先验证初始化顺序和兼容性，当前
   没有据此证明用户故障。

Profiler 的已交付能力和当前支持限制见[工具 README](../../devtools/profiler/README.md)、
[MCA 指南](../../devtools/profiler/LLVM_MCA.md)与[分阶段记录](profiler-implementation.md)。
离线 profiler 不增加 WJR 运行时依赖、采样线程或公共接口；其模型估算仍与真实吞吐分开。
