# SIMD 填充基线与迁移实验

本实验比较现有 `wjr::set_n`、允许编译器自动向量化的普通循环，以及可选的 xsimd 填充。
先验证真实内核和测量方法，再决定 SIMD 后端的迁移范围；不把一次填充实验扩展为通用 SIMD 结论。

## 构建和复现

```sh
cmake -S . -B build-simd -GNinja \
  -DCMAKE_BUILD_TYPE=Release \
  -DWJR_BUILD_BENCHMARKS=ON -DWJR_USE_GMP_TESTS=OFF \
  -DWJR_CXX_STANDARD=20 -DWJR_CPU_TARGET=native \
  -DWJR_BENCHMARK_PROFILE=simd -DWJR_BENCHMARK_XSIMD=ON
cmake --build build-simd --target benchmarks
./build-simd/bin/benchmarks \
  --benchmark_repetitions=5 --benchmark_min_time=0.05s \
  --benchmark_report_aggregates_only=true \
  --benchmark_out=simd-results.json --benchmark_out_format=json
```

Windows 使用对应的 `.exe` 路径。比较固定 ISA 时使用同一工具链支持的 `baseline`、`x86-64-v3`
或 `/arch` 配置；native 结果只代表实际构建机。JSON 记录编译器、WJR 标准和 CPU 配置，
xsimd 行另记录实际选择的架构与 lane 数。完整编译命令、CPU 型号、构建提交、异常、
sanitizer、汇编及优化选项应随结果一起保存，不能只凭 `native` 字符串判断两次实验相同。

`WJR_BENCHMARK_XSIMD` 默认关闭，并且只允许用于 simd profile。
开启时获取 [xsimd 14.3.0](https://github.com/xtensor-stack/xsimd/tree/14.3.0) 对应的固定 commit
`e88a72831858123924f7118f345dfe5d70d95991`；它只链接到基准程序，不进入 WJR 的公共头文件、
模块依赖或安装包。离线环境可通过标准 FetchContent source override 使用相同源码。
不开启该选项时，仍可比较 WJR 与普通循环。
core/all 继续使用既有基准集合，simd 是独立实验 profile。嵌入的 Google Benchmark 也关闭安装，
避免构建基准目标后安装 WJR 仍依赖未构建的 benchmark_main 或发布测试依赖。

## 语义和测量边界

实验覆盖 uint16/uint32/uint64：

- value 场景使用经过优化屏障的不重复字节值；zero 场景允许编译器识别常量零和 memset。
- 长度覆盖空数组、向量边界、300 个元素以及更大的数组。
- offset=0 从 64 字节对齐的地址开始；offset=1 偏移一个完整元素，仍符合 T 的自然对齐。
- 三个实现仅写 `[dst, dst + n)`。xsimd 路径使用完整批量存储和标量尾部，没有越界尾部存储。

`fill` 在一个用例中反复使用同一长度。`mixed_fill` 每次从预生成序列中读取长度：
workload=0 交替使用一个和三个 WJR 向量宽度；workload=1 使用固定种子打乱的 4096 项序列，
包含零、小数组、边界及 300 元素。生成和打乱不计时，序列读取、索引和调用的开销计时。
固定 seed 不保证跨标准库得到相同 shuffle 序列，跨构建比较还需固定标准库实现。
交替或打乱也不等于已知的分支命中率，不能仅凭场景名称推断硬件行为。

每个用例先在计时之外验证填充值和前后保护区，错误立即令进程失败。
计时循环使用内存优化屏障，防止重复写入被删除；分配、初始化、校验和字符串标签不计入内核时间。
普通循环允许自动向量化和 memset 识别，不宣称为禁止向量化的 scalar 基线。

结果代表重复写入同一缓冲区的 warm-cache 工作负载，不能直接外推为 DRAM 带宽或交易系统端到端收益。
该 profile 使用 wall-clock real_time，比较相同用例的重复运行中位数并检查方差。
固定长度可能让编译器外提条件或让预测器充分学习；混合长度用于补充这种场景，不能替代真实调用负载。
尽量固定核心、保持相同电源策略并避免同时构建或运行其他重负载任务。
ASan 用于正确性验证，其计时不能与未启用 sanitizer 的结果混用。

## 候选优化与迁移门槛

考察的候选是在达到一个向量宽度后，对不超过两个向量的动态长度内联首尾两个 store。
这可能减少辅助函数调用，也会改变分支历史、代码体积和寄存器分配。
本 MR 保留现有正式内核，候选用于独立 A/B 实验，不因固定长度结果好就加入生产路径。

候选仅在现有 `n >= type_width` 分支中、常量长度特化之后增加以下代码；其他路径使用原实现：

```cpp
if (n <= type_width * 2) {
    const auto y = simd::set1(val, T());
    simd::storeu(dst, y);
    simd::storeu(dst + n - type_width, y);
    return;
}
```

2026-10-17 的探索性 A/B 使用 i5-13500HX、Windows x64、Clang 18.1.8 和 MSVC 14.42 标准库，
C++20、Release `-O3 -DNDEBUG -march=native`、NASM 开启、ASan 关闭、WJR release debug level=3。
两个二进制使用相同 benchmark 源码，control 的正式内核取自 addeabb4，candidate 只加上述分支。
进程固定逻辑 CPU 0，依次运行 control、candidate、candidate、control；每次每用例 7 次重复、最短 0.01s。
96 个匹配用例中，以下是 uint64 非零 value、offset=0 的 real_time 中位数，单位 ns：

| 调用分布 | control A | candidate B | candidate C | control D |
| --- | ---: | ---: | ---: | ---: |
| 固定 n=4 | 1.052 | 0.607 | 0.533 | 1.045 |
| 固定 n=8 | 1.045 | 0.542 | 0.538 | 1.063 |
| 交替长度 | 1.327 | 1.235 | 1.102 | 1.377 |
| 打乱长度 | 3.113 | 4.118 | 2.894 | 3.114 |

这些短时测量的重复运行变异系数整体偏高，不能据此确定通用提速或回退，更不能归因于分支失误。
固定短长度有改善迹象，混合结果不足以支持采纳，因此该候选没有加入正式代码。
独立的动态 uint64 填充封装也核对了 Clang 汇编：原实现直接尾调 large helper 的范围内，
候选增加了一次长度比较和条件跳转。这个调用上下文的生成码不能代替实际内联调用点或 PMU 证据。

行为测试覆盖零值、全一、高位值、不重复字节值、向量边界及合法偏移；精确长度分配供 ASan 检查尾部越界。
优化的验收同时检查生成码、固定/混合长度、较大数组、内联与外部调用，以及目标 ISA 和编译器。
不能仅根据调用次数或指令数减少断言整体更快。

有 Linux PMU 权限时，可对单个长时间运行的用例补采硬件计数，例如：

```sh
perf stat -r 5 -e cycles,instructions,branches,branch-misses,cache-misses -- \
  ./build-simd/bin/benchmarks \
  --benchmark_filter='^mixed_fill/wjr/u64/value/workload:1/offset:1/real_time$' \
  --benchmark_min_time=100000000x --benchmark_repetitions=1
```

计数包含进程启动、校验与 benchmark 调度，不能把全部 branch-misses 直接归因于某条内核分支。
事件可用性与 multiplex 情况需单独记录；本地 Windows 实验未采集 PMU，不报告预测命中率。
参见 [perf stat 手册](https://man7.org/linux/man-pages/man1/perf-stat.1.html)。

填充实验只能回答这一类 store 内核是否适合迁移。后续通用 SIMD 选型仍需单独比较算术、比较掩码、
shuffle、reduction、NaN/舍入语义与编译成本。现有有价值的特化保留，薄接口优先覆盖实际需要的操作。
参考 [xsimd batch 接口](https://xsimd.readthedocs.io/en/stable/api/xsimd_batch.html)。

CI 在已有的一个 Linux job 中构建并冒烟验证三个后端；不对共享 runner 上的纳秒延迟设通过阈值。
Windows 不增加 job 或步骤。手动 benchmark workflow 可选择 simd profile 获取完整 JSON artifact，
每周默认的 core profile 保持不变。

## 扫描与谓词对照

同一个 simd profile 还提供 `scan/{wjr,loop,xsimd}/u64` 与 `mixed_scan/{wjr,loop,xsimd}/u64`。
比较首个相等元素的索引；无匹配返回元素数。xsimd 路径使用
[实验后端](../design/simd.md)的完整向量加载及标量尾部，不迁移原有扫描器。

```sh
./build-simd/bin/benchmarks \
  --benchmark_filter='^(scan|mixed_scan)/' \
  --benchmark_min_time=0.05s --benchmark_repetitions=7 \
  --benchmark_enable_random_interleaving=true \
  --benchmark_out=scan.json --benchmark_out_format=json
```

固定用例覆盖 0 到 65536 个 uint64 元素、64 字节对齐地址及一个元素偏移，
`hit:0/1/2/3` 分别为无匹配、首元素、中间、末元素匹配。
校验和数据准备在计时外完成，验证失败以非零退出。
固定用例的 items/s 按逻辑比较至结果的元素数统计，不当作实际硬件加载字节数。

混合用例使用 4096 项 pointer/count 序列，将 16 种长度、四种命中模式和两种偏移组合打乱。
每种组合读取已准备的独立缓冲区，不在计时中修改数据或分配。
它计量每秒调用数，包含序列读取及多个缓冲区的缓存成本；不能把差异全部归因于分支预测。
固定种子为 42，复现时同样需要固定 STL 的 shuffle 实现。

本轮增加正确性、安装消费及生成码检查，不给扫描吞吐设置 CI 阈值。
后端扩展仍需在目标硬件上重复测量，结合调用点和可用 PMU 计数；
一次短数组或固定命中位置的胜出不构成全库替换依据。
