# arch 演进设计

审查日期：2026-10-30。源码基线：`0f0288dececb29f59e4248fee136db36fe1e6e44`。
本设计覆盖 `include/wjr/arch`、`src/arch` 及其配置、公开调用方和测试。
实施顺序与验收见 [实施文档](arch-implementation.md)。

## 结论与平台预算

保留现有专用内核，先修复可复现的契约问题、建立针对性测试，再用等价语义的
生成码和实测决定优化。现阶段不重写一套跨架构 SIMD 框架，也不把全部操作做成运行时分派。

| 优先级 | 范围 | 本轮与后续要求 |
| --- | --- | --- |
| P0 | x86-64 / Linux / GCC、Clang | 主要性能决策依据；baseline 与显式 ISA 的代码生成和正确性 |
| P0 | x86-64 / Windows / MSVC、Clang | 保持现有两项 CI；修复共用契约，避免为单一编译器增加通用开销 |
| P1 | AArch64 / Linux、iOS / Clang，Linux GCC | 先验证配置和 portable fallback，再对真实热点试验 NEON |
| 暂缓 | 32 位 x86/ARM、其他架构 | 根 CMake 当前拒绝非 64 位；不因宏存在而声称完整支持 |

不建立编译器 × OS × ISA 的全组合矩阵。本轮不选择第三种架构；以后最多选一种，
必须有实际用户、工具链和测量设备。iOS 的构建、链接与设备执行独立验收，
Linux AArch64 结果不能替代 iOS。只有必要的热点才增加平台实现。

## 当前实现与证据

| 区域 | 当前实现 | 后续处理 |
| --- | --- | --- |
| 配置/target | `wjr::config` 与 `wjr::isa` 分开；`ARCH_DEPENDENT` 控制 ISA 传递 | 保持；检查实际 compile commands 和安装消费者 |
| CPU 信息 | `cpu_features` v0.10.1；baseline `arch-cpuinfo`；公开头暴露 `X86Info` | 保留兼容符号，未来收窄内部依赖；不能把探测误当运行时分派 |
| SIMD | `sse`/`avx` 静态包装、宏守卫、部分低 ISA fallback | 首先锁定 partial load 与 mask 语义，避免全量包装迁移 |
| 查找/比较 | uint64 专用内核、前端短长度检查、SSE4.1/AVX2 分支、展开循环 | 用现有 scan benchmark 验证单个候选；保持返回值与尾部边界 |
| 填充/移位 | 自有 SIMD、rep stos、阈值与展开策略 | 先测混合长度；不凭固定长度或 TODO 改阈值 |
| 大整数 | x86 内联汇编、NASM 与通用实现 | 保留已专门优化的进位链；复用不等于替换所有内核 |
| JSON/UTF-8 | 字节位集与分类内核 | 与数值 lane predicate 分开；不让逻辑掩码接口破坏字节位图 |
| 其他 arch | generic 配置、Windows 入口、preview 分配适配 | 存在这些目录不表示已经有 ARM 数值优化 |

具体首个缺陷位于 `include/wjr/arch/x86/simd/sse.hpp`：
`loadu_si48/80/96/112` 在拼接尾部时把 `void*` 转为 `uint16_t*` 或
`uint32_t*` 后解引用。即使 x86 硬件允许非对齐访问，这也给 C++ 引入了
不应存在的对齐和对象类型前提；AVX 的部分加载间接复用这些实现。
基本的 `loadu_si16/32/64` 已经使用现有内存读取机制，修复应复用同一机制。
测试必须覆盖奇数字节地址，并用 alignment sanitizer 验证修复前后的区别。

另有两个后续审计点，暂不混入修复：`src/arch/x86/CMakeLists.txt` 强制修改
`BUILD_TESTING` cache；CPU 信息和填充阈值依赖动态初始化。需要单独的嵌入消费者、
初始化顺序和兼容性证据后才调整，不宣称本轮已证明它们引起用户故障。

## 内存和接口契约

部分加载允许任意字节对齐，只读取所需的 `N/8` 个有效字节；调用方提供可读范围。
不要求该范围内原本存在 `uint16_t`/`uint32_t` 对象。低位有效字节保持原值。
既有高位行为保持兼容，尤其 AVX 从 SSE cast 得到的未指定高位不能承诺清零，
也不能为了便于测试额外清零。`loadu_si16x` 的长度是 16 位单元数：SSE 0–8、
AVX 0–16；零长度不访问指针。现有有效调用不增加运行时检查、分配或所有权状态。

修复直接用 `read_memory<T>(byte_pointer + offset)` 读尾部。其实现通过 `memcpy`
建立类型安全的值读取；优化目标是与同样语义的固定尺寸 memcpy 手写基线生成同样的
load/insert 指令。必须检查 GCC/Clang 生成码，不能只凭 inline 或 memcpy 的名字声称零开销。
允许无害的寄存器命名差异；新增调用、分支、栈存储或宽读需解释并处理。

“零开销”以完成相同契约的优秀手写实现为上限参考，目标是达到相同上限或仅有轻微、
可量化的差异；与旧版一样快不自动达标。旧实现含未定义行为时更不能作为唯一基线。
x86 指令语义、操作数与 ISA 要求参考
[Intel Intrinsics Guide](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html)。
指令级 latency/throughput 不能直接代替调用方吞吐；一个 intrinsic 也可能展开为多条指令，
因此同时核对实际生成码、内联、内存访问与分支，而不按 intrinsic 个数估算性能。

后续掩码接口只在 backend 私有层表达 `any`、`all`、首个有效 lane 等操作：
uint64 比较的 byte movemask 每 lane 占 8 位，xsimd 的 compact mask 每 lane 占 1 位。
二者不能直接替换。JSON/UTF-8 继续显式使用 byte bitset。没有真实第二个消费者前，
不引入拥有状态的通用 mask 类型、虚函数、函数表或统一整数编码。

## 复用决策

以下版本是审查对象，不是“最新版本”声明。没有新依赖被此设计自动启用。

| 候选 | 已核对边界 | 决策与成本 |
| --- | --- | --- |
| [cpu_features v0.10.1](https://github.com/google/cpu_features/tree/v0.10.1) | 仓库已有依赖和 C++ `GetX86Info` 调用 | 保留；它负责探测，不能弥补已按 native 编译的调用方 |
| [xsimd 14.3.0](https://github.com/xtensor-stack/xsimd/tree/e88a72831858123924f7118f345dfe5d70d95991) | 仓库 CMake 已固定 commit；上游提供 SIMD 数值批量包装 | 首选可选对比后端，复用现有薄适配；编译成本、mask/尾部契约和数学语义逐项验证 |
| [Highway 1.2.0](https://github.com/google/highway/tree/457c891775a7397bdb0376bb1031e6e027af1c48) | 上游提供静态与多目标动态分派 | 作为未来分派边界参考；当前不新增依赖，不承担多份内核和初始化成本 |
| WJR 现有 intrinsic/汇编 | 已有静态 target、短路径与专用调用方 | 对少量有价值操作保留；新增手写实现须胜过成熟后端的等价基线 |

正式引入新后端前，补充固定版本的许可证、API 历史、维护状态、编译器覆盖、
构建耗时与二进制大小审计。当前仅用 Highway 作设计参考，不声称完成生产依赖准入。
已有实验见 [SIMD 测量](../performance/simd.md)，避免重复已否定的短数组填充分支。

## 后续优化边界

1. 先完成 partial load 修复与安全边界测试；独立 MR 可直接审查。
2. 为 uint64 find/compare 建立完整边界与 mask 测试，使用现有 scan benchmark
   比较 WJR、xsimd、允许自动向量化的手写循环。先选一个内核，未获益则保留正式实现。
3. 单独处理 CPU provider 的 CMake 作用域和安装消费者；保留兼容入口，避免顺手升级依赖。
4. AArch64 首先做 portable 构建审计；有原生 Linux/iOS 设备后再决定一个 NEON 试点。
   不复制整套 SSE/AVX 成员函数到 ARM 目录。
5. 只有部署确实需要单二进制跨 ISA，才增加 pointer/length/scalar ABI 的独立内核和
   baseline dispatcher。现有 inline 类型与编译单元 ISA 需保持一致。

性能报告同时列出读取/写入次数、对象或工作区大小、分配次数、代码大小、调用和分支。
首个修复没有增加分配/工作区；是否无额外指令由实测报告判断。未经测量的吞吐收益不写入结论。
