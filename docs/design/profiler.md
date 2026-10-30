# Profiler：离线证据与可解释对比设计

状态：设计，2026-10-30；本文没有实现 CLI、provider 或采集服务。
实施顺序与验收见 [profiler 实施计划](profiler-implementation.md)。
目标是让已有汇编、静态模型、C++/Python 实测和后续网络证据可追溯地比较，
先交付一个无需安装采集工具的离线 CLI，再按真实需求增加 provider。

## 交付决定

先做一个 Python 标准库实现的离线 CLI，读入 LLVM-MCA JSON、仓库现用的
Google Benchmark v1.9.0 JSON 和一种 cProfile 导出 JSON，输出 Markdown 与 JSON 对比报告。
分析机器不必安装编译器、LLVM-MCA、perf 或目标程序。运行采集是独立、显式操作；
首版不做 UI、常驻 agent、采集 daemon、云存储、插件发现或通用 profiler SDK。

优先 x86-64 的 Linux GCC/Clang、Windows MSVC/Clang 和 CPython。对敏感的共用热路径，
在两平台分别进行同环境、同合同的 candidate/reference 对比，再观察方向是否一致；
不同 CPU、编译器或 STL 的绝对时间不能归因于 OS。离线 CLI 在 Windows/Linux 读取同一数据合同。
已有 Google Benchmark 两平台实测不等于本工具的 importer 已实现；ETW/WPA 则尚未验收。
Python 采样和网络是后续能力；不增加 Windows CI 矩阵。ARM/iOS 暂后置，
有真实 workload、原生设备和工具链后再分开验收，不以交叉编译或 Linux ARM 代替设备测量。

用户实际工作流固定为：

```mermaid
flowchart LR
    A[授权本地或远端采集] --> B[不可变原始 artifact 与 manifest]
    B --> C[薄适配器规范化]
    C --> D[合同及环境兼容性判定]
    D --> E[同合同 baseline 对比]
    E --> F[指标变化 原始位置 限制与下一步]
```

首个可用任务是 **P01：Google Benchmark 离线导入与合同对比**，无需等待运行采集或其他模块。
P02 加 MCA，P03 加 cProfile 后才称为此设计的 MVP；后续 provider 不阻塞这三项。

## 分开回答的问题

| 证据 | 能回答的问题 | 必须保留的解释边界 |
| --- | --- | --- |
| 编译器汇编、目标文件、优化说明 | 实际选了哪些指令、是否内联、有无调用/分支/栈/宽读、代码大小 | 汇编长度和优化提示不等于运行时间；链接及调用方可能改变结果 |
| LLVM-MCA | 指定 CPU 调度模型下的依赖、吞吐估计、资源压力 | `estimate`；不模拟真实分支预测、cache miss、OS、端到端时间；不把周期除主频当延迟 |
| Google Benchmark | 已定义 workload 的 CPU/墙钟每迭代耗时与计数器 | `measurement`；计时区间、优化屏障、预热、线程和分配策略必须相同 |
| perf stat / record | 计数器、采样热点与调用栈 | 采样数不等于精确调用数；PMU 不可用/复用不足为缺失；CPU 样本不能直接解释等待 |
| 内存/分配计数 | 分配次数/字节、活跃量、峰值、拷贝或移动次数 | allocation 与 RSS/驻留页不同；插桩会扰动时间；精确计数与抽样估计区分 |
| cProfile | Python 调用次数、函数自身/累计时间 | 确定性插桩；累计时间不能逐行相加；C 扩展细节缺失，不用于 Python 与 C++ 速度排名 |
| py-spy / Scalene | 采样栈热点；后者可解释 Python/native CPU、内存及拷贝 | 样本权重、频率、丢样、GIL/idle/native 过滤决定含义；不承诺逐事件精确 |
| curl / iperf3 / OTel | 请求阶段、传输能力、服务端阶段与跨服务因果关系 | 三者不是同一种基准；首字节等待不等于纯服务器计算，跨度不等于 CPU 时间 |

普通 benchmark 难暴露的 trait 分派问题由针对性证据补足：只在敏感 relocate/move
结构上检查 triviality、WJR 可扩展 trivially-relocatable 合同、准确的 `noexcept`，
结合 bulk-relocate/copy/move 计数、生成码、对象布局和异常路径。先证明生命周期、
所有权与地址稳定性，不能根据 profiler 建议盲目声明 relocatable/noexcept。
不把 trait 审计扩展成全部普通代码的固定流程。

## 最小工具集合与复用决策

以下是 2026-10-30 已核对的固定参考，不是最新版本声明，也不自动新增依赖。
源码活动只能说明仓库仍有更新，不能保证版本兼容性。未执行的工具不列为已支持 provider；
正式接入前必须补该版本真实 fixture 和平台 smoke test。MVP 的固定格式与后续候选分开验收。

| 工具与固定参考 | 格式/权限/成本 | 决定 |
| --- | --- | --- |
| [LLVM 18.1.8](https://github.com/llvm/llvm-project/tree/llvmorg-18.1.8/llvm/tools/llvm-mca)；另核对 [20.1.0 文档](https://releases.llvm.org/20.1.0/docs/CommandGuide/llvm-mca.html) | 原生 `-json`，无需 root；Apache-2.0 WITH LLVM-exception；安装 LLVM 较大，但仅 producer 需要 | MVP 用 18.1.8 已验证格式，20.x 仅候选，fixture 通过再扩大支持 |
| [Google Benchmark v1.9.0](https://github.com/google/benchmark/tree/v1.9.0) | 原生 JSON，普通进程权限；Apache-2.0；仓库已有构建依赖 | MVP 沿用现有版本，不为 profiler 升级库或另建 benchmark 框架 |
| [CPython v3.12.7](https://github.com/python/cpython/tree/v3.12.7/Lib) | cProfile/pstats，原始 `.prof` 含版本相关 marshal；解释器许可证及 pstats 的 Apache notice | MVP 自有 JSON exporter，producer 上用匹配解释器转换；分析器只读 JSON |
| [Linux perf v6.12](https://github.com/torvalds/linux/tree/v6.12/tools/perf) | stat JSON / perf.data；依赖内核权限与 PMU，GPL 条款按组件保留；不链接进 WJR | 第一批扩展，先 stat，后 record/script；无权限时仍可分析既有导出 |
| [curl 8.10.1](https://github.com/curl/curl/tree/curl-8_10_1) | write-out JSON，curl permissive license；普通授权 HTTP 请求 | 网络首个 provider，只支持简单请求阶段，首版不装 iperf/OTel |
| [iperf3 3.17.1](https://github.com/esnet/iperf/tree/3.17.1) | `-J`，BSD 及组件 notice；需要自己控制两端，不扫描发现服务器 | 有传输吞吐需求再导入；不从 HTTP 请求延迟推导链路吞吐 |
| [OTel specification v1.37.0](https://github.com/open-telemetry/opentelemetry-specification/tree/v1.37.0) | trace/span ID、时间戳；Apache-2.0；具体 SDK、OTLP proto/文件格式需另外固定 | 未来导入既有 spans；不先部署 collector，不把本规范版本当 SDK/协议版本 |
| [py-spy v0.4.0](https://github.com/benfred/py-spy/tree/v0.4.0) | speedscope/raw，MIT；外部读目标内存，受 ptrace/容器/OS 权限影响 | Python 采样首选候选，优先导入 speedscope；不自动提权 |
| [Scalene v1.5.51](https://github.com/plasma-umass/scalene/tree/v1.5.51) | JSON，Apache-2.0；内存采集有平台限制，该版本 Windows 不支持 memory/copy | 需要 Python 分配/拷贝归因才接入；不用自动优化或联网 AI 功能 |
| [Tracy v0.11.1](https://github.com/wolfpld/tracy/tree/v0.11.1) | zone/alloc/锁与 trace，BSD-3-Clause；应用插桩和采集端有成本 | 需要跨语言 zone 或锁时间线时复用；不为微内核默认启用 |
| [ETW/WPR/WPA](https://learn.microsoft.com/en-us/windows-hardware/test/wpt/windows-performance-recorder) | Windows ADK 工具，ETL 与按 profile 导出的表；依 provider/session 可能要求管理员 | Windows 后续只接受一个固定导出 profile；SDK/ADK 版本、分发许可与权限未验收，不捆绑 |

许可证来源保留固定版本的上游文件：
[LLVM LICENSE](https://github.com/llvm/llvm-project/blob/llvmorg-18.1.8/llvm/LICENSE.TXT)、
[Benchmark LICENSE](https://github.com/google/benchmark/blob/v1.9.0/LICENSE)、
[CPython LICENSE](https://github.com/python/cpython/blob/v3.12.7/LICENSE) 与
[pstats.py notice](https://github.com/python/cpython/blob/v3.12.7/Lib/pstats.py)。
perf 以 [Linux 根 COPYING](https://github.com/torvalds/linux/blob/v6.12/COPYING)
与逐文件 SPDX 为准，组件许可/分发审计仍是正式接入条件；不存在可据以概括所有组件的
`tools/perf/COPYING`。ETW 未固定 ADK 和分发条款，不捆绑工具。

## 已验证与未验证

| 证据 | 实际结果 | 尚未证明 |
| --- | --- | --- |
| Windows LLVM-MCA 18.1.8 格式探针 | 明确 Linux triple、Skylake model 和 100 次模拟迭代，原生 JSON 退出 0；读取到 Summary、InstructionInfo、ResourcePressure | 不是 Linux runtime 测量；未验收 LLVM 20.x、正式 importer 或各平台采集器 |
| CPython 3.12.7 的同名函数探针 | 两个文件的 `(file,line,name)` 有两项，`get_stats_profile()` 的按名结果只剩一项 | 未实现 exporter；未验收其他 Python 版本、递归/线程覆盖和生产 fixture 全集 |
| 仓库 Google Benchmark v1.9.0 实测 | [B+ tree 基线](../performance/btree-baseline.md) 已在 Windows Clang/MSVC 和 Linux GCC/Clang 生成真实 JSON；[Linux 运行](https://github.com/wjr-z/wjr/actions/runs/41041208599)提供原始重复与环境信息 | 尚未实现本设计的通用导入器、semantic manifest、跨 provider 对比或回归门禁 |
| 其他 provider | 上表固定版本源码/格式/许可证参考已核对；本机 curl 版本探针是 8.11.1 | 没有 perf/PMU、curl 8.10.1 网络请求、py-spy、Scalene、Tracy、OTel 或 ETW/WPA 采集验收 |

MCA 探针的两条指令为 `addq %rbx, %rax` 与 `imulq $3, %rax, %rax`，
region 名为 `dependency_probe`。`SummaryView` 给出 `Iterations=100`、
`Instructions=200`、`TotalCycles=403`、`BlockRThroughput=1`。
这同时验证两个容易混淆的字段：只有 **2 条静态指令**；总模拟周期除迭代数的 4.03
与块吞吐估计 1 是不同量。资源记录的 `InstructionIndex=2` 是块汇总，不能计作第三条指令。
汇编 UTF-8 字节的 SHA-256 为
`f92246ff6854398ad9ce8acddb65e6533816db3d74e74ed8c501a67782a327c1`。
原探针保存的是 subprocess 换行归一化后的 stdout 摘要，不冒充生产 raw-byte artifact；
P00/P02 仍需建立可分发、逐字节保留的正式 fixtures。

## Artifact 与最小数据合同 v1

一个 bundle 是普通目录，不要求数据库或归档格式：`manifest.json`、`raw/`、`normalized.json`，
报告位于独立输出目录。raw 按采集字节保留，SHA-256 绑定；stdout、stderr、退出状态分别记录。
规范化和报告可重建，不改 raw。不自动从 artifact 内 URL 获取内容或执行其命令。
允许不带源码分发 bundle；源码/二进制/汇编都可仅保留哈希，缺失字段显式为 unknown。

| 对象 | v1 必要字段与规则 |
| --- | --- |
| bundle | schema_version、bundle_id、producer、creation time、revision、dirty patch hash、adapter version |
| tools/build | compiler/analyzer/interpreter/library 完整版本，编译/链接/采集 argv 数组、标准、优化级别、debug/sanitizer/LTO/PGO、依赖及构建 fingerprint |
| host/target | OS/kernel、架构/ABI、CPU 型号/stepping、启用 ISA、显式 CPU target、MCA scheduling model；采集 host 与分析 target 分开；CPU affinity/core type/频率策略已知则填 |
| workload | id/version、参数、数据尺寸/对齐/分布/种子/哈希、预热、线程/并发、迭代/工作单位、计时范围、allocation/setup 是否计入 |
| semantic_contract | id/version、输入域、返回/错误/异常语义、整数/浮点规则、别名/生命周期/所有权、checked/trusted 前提；变体到共同合同的显式映射及正确性证据 |
| region | id、源路径/符号/行或 MCA marker、源/汇编/二进制哈希、variant、与 workload/contract 的关联；源哈希不同本来就是候选比较的正常情况 |
| raw_source | 相对路径、format/provider、完整工具版本、bytes SHA-256、record locator（JSON pointer/行/region）；若脱敏则是新 artifact，保留前后哈希与规则 |
| observation | metric、kind、unit、scope、value/raw repetitions、aggregation、n、variance/dispersion、clock domain、time basis、inclusion、quality/missing reason、raw_source locator |
| correlation（可选） | experiment_id/run_id、host/process instance、PID/TID、trace/span/parent ID、clock calibration/uncertainty；PID 不作为跨运行身份 |

`kind` 仅为 `measurement`、`estimate` 或 `derived`；derived 必须指向输入且继承 estimate/
sampling 属性。计数也注明 `exact_instrumented`、`sampled` 或 `hardware_scaled`，不能因命名为 count
就承诺精确。单位用明确的 ns/iteration、cycles/block、count/run、bytes、samples 或比例；
ops/items/bytes 的归一化必须来自显式工作量，不把名字里出现的 n 当已处理元素数。
每条指标声明越小/越大/无单调优劣，资源压力、IPC、内存峰值不自动合成总分。

薄适配器是内部函数 `import_artifact(raw, manifest) -> observations + diagnostics`。
只实现实际使用字段；原始扩展字段仍可回查，不全量重新导出上游 API。
集中注册三个 importer 即可，不建动态插件 ABI。schema major 不支持时报错；未知 optional 字段保留来源。
未知工具版本只能进入 exploratory 结果，不能静默假设格式和指标语义未变。

## 三个 MVP importer 的具体边界

**Google Benchmark：**读取 `context` 和 `benchmarks`，支持 `json_schema_version=1` 的已测
v1.9.0 子集；区分 `run_type=iteration/aggregate`、repetition_index、time_unit、threads、
real_time/cpu_time、iterations 与 aggregate_name/aggregate_unit。只从原始 iteration 行形成
重复样本，不把 mean/median/stddev/cv 再算成样本；仅有 aggregate 时标记 insufficient_samples。
CPU time 与 wall time 分列，显式 manual/time mode 必须由 manifest 解释。
error_occurred 行保留为失败，缺失/非有限计数器不补零；NaN/Infinity 可能出现在上游输出，
严格捕获并以缺失原因处理，不能在标准 JSON 报告中继续写非法数字。
初版任意 user counter 仅展示原名和值，未配置单位与 Counter flags 解释前不参加回归判定。

**MCA：**读取 `CodeRegions[]`、`SummaryView`、`InstructionInfoView`、
`ResourcePressureView`、`TargetInfo` 与 SimulationParameters。保留 region 名和汇编顺序。
Summary 的 Instructions 是模拟总指令数，不能当静态代码大小；Iterations 不是统计重复测量。
TotalCycles/Iterations 也不等于 BlockRThroughput；后者是块的模型吞吐估计，分别呈现。
ResourcePressureInfo 中 InstructionIndex 等于该块指令数的记录是块汇总，不再算一条指令。
资源名字/索引只在相同 LLVM/CPU model 下比较。没有 JSON dependency graph 就不自行猜图；
timeline/bottleneck 文本作为原始附件，后续仅在真实需求时增加版本化解析。
带 call/unsupported instruction、忽略错误或不完整 region 的数据必须标记限制，不能给出整体零开销结论。

**Python：**v1 使用 `wjr.cprofile-json/1`，producer exporter 针对 CPython 3.12.x 核对，
当前仅 3.12.7 执行通过。在 producer 上从 `pstats.Stats.stats` 的版本相关 tuple 导出
`(file,line,name)`、primitive_calls/total_calls、self_s/cumulative_s 与 caller edges；
该内部映射是有版本边界的薄适配，不能假装稳定的跨版本 `.prof` 协议。
默认只接收 JSON；用户自有 `.prof` 的显式转换留在匹配 producer，不分析任意不可信 marshal。
`get_stats_profile()` 会按函数名覆盖同名项并舍入耗时，本机两文件同名函数探针已复现，不能用它导出。
保留递归语义，不累加 cumulative_s；不同 revision 函数定位需显式 region map，不能只按行号猜。
首版每个独立运行对应一个 profile；无多运行时仅诊断热点，不以调用事件当重复样本。

## 可解释对比

先判 eligibility，再计算变化。必要条件是共同合同、相同 workload/输入分布、计时 scope、
单位、并发和插桩模式；source revision/hash 与实现 variant 可不同，但必须可追溯并有语义证据。
默认 measurement 要相同机器/CPU/core class/OS、编译器及 flags/build fingerprint；
未知关键字段为 inconclusive。声明比较轴（例如换编译器）可展示 contextual delta，
仍不得冒充同环境回归门禁。跨机器不自动按主频或 IPC 校准；native 必须记录实际 resolved features。
MCA 另外要求同 analyzer/model/triple/iterations/region scope，且始终与 runtime 分开比较。

报告顺序：合同/环境资格 → 每项 baseline/candidate/绝对差/相对差 → n 与离散 → 来源位置
→ 可能解释 → 缺少什么证据。报告分类为 comparable、contextual、inconclusive、failed。
baseline=0 时不算比例；缺失数据不补零，不以另一 provider 的指标替代。
例如“MCA BlockRThroughput 降低；实测 ns/call 在波动内；尚无 branch/cache counters”，
不能写成“端到端提速”或把资源压力降低当真实瓶颈已经消失。

MVP 默认展示 median、min/max、MAD，n≥2 才有 sample stddev；没有样本则 variance=null。
推荐至少 7 次独立运行，基础门槛 n≥5；不把 benchmark 内部 iterations 当独立 n。
原始 repetition 的实验层级记录为 in_process_repetition 或 independent_process_run。
首版无自动统计显著性检验、无复合 score、无自动零开销认证；阈值门禁延后到有
独立运行与稳定机器证据时单独实施，避免把噪声阈值包装成置信区间。

## 时钟、跨语言及网络归因

duration 优先由同一进程/主机单调时钟相减；UTC 只作关联，必须记录 clock/time basis。
CPU time、wall duration、on-CPU sample weight、网络跨度是不同指标，不能相加得总耗时。
同进程 Python 调 C++ 时，用共同 run_id、线程/region 及可选 span 显式关联；
cProfile 通常把 native call 作为一个叶项，perf 可解释其内部，但二者区间会重叠，不能重复计费。
trace 的 inclusive/exclusive、异步等待与并行子任务分别注明；OTel span 的 self wall time
不能简单减去所有 child duration（child 可重叠）。首版不计算统一“时间归属百分比”。

跨机器按 trace_id/span_id/请求关联 ID 建因果关系。无校准不按绝对时间拼接成精确时间线，
不把客户端 span 减服务端 span称为网络延迟。有校准也记录偏移、漂移、测量方法与误差界；
NTP 开启不能视为零误差。聚合 profile 没有事件时间戳时只作 run/region 关联。

curl v1 只支持单次非重试、非 redirect、无 proxy、无连接复用的 HTTP/1.1 请求；
保留 DNS/连接/TLS/pretransfer/first-byte/total 原始累计时间及 exitcode/status、字节、协议。
在满足单调关系时派生阶段差：解析、连接、可选 TLS、首字节前等待、接收尾段；
首字节前等待包含发送/网络/排队/服务计算，接收尾段也可能包含服务端流式生成。
不推断纯服务器 CPU。HTTP/2 多路复用、重定向、失败 partial timing 保留 raw，标记超出 v1 范围。
curl 固定版本文档的 time_posttransfer 单位需额外核对实现，v1 不使用该新增字段。
服务端阶段只有授权的 OTel spans/应用计时才分解；iperf3 仅描述自身 sender/receiver workload，
方向、协议、并发、时长/omit 与 zerocopy flags 都影响合同，不和请求 latency 混排行。

## 开销、采集与远端边界

“零开销”是被分析的基础抽象相对优秀同语义手写实现的目标。profiler 本身有 CPU、
采样、内存、I/O、符号解析与插桩成本，不能许诺零开销采集。offline 导入不改变目标二进制。
性能基准、正确性/sanitizer、精确计数/alloc 插桩、采样各用独立 run/build，显式关联；
不开启插桩的基准不能偷偷混用插桩结果的时间。必要时记录 profiler on/off 的墙钟成本，
只作扰动证据，不减掉固定百分比“修正”结果。

采集由用户命令、本地 provider、已授权 GitHub worker 或 Compiler Explorer 输出 bundle。
不强制本机 LLVM；远端工作请求明确源文件/命令/工具版本/输出/可见性，授权范围外不上传源码。
CE 的 public snippet/link 需显式允许；无授权时保留本地源码哈希并离线分析已有汇编。
GitHub workflow 先做手动、指定 revision 的 artifact 采集；共享 runner 的 runtime timing
默认 contextual，稳定硬件后才做回归。未固定 compiler/analyzer 的 CE 输出也只作 exploratory。

只分析获授权的本人进程、测试程序及双方允许的测试流量；不网络扫描或采集邻居进程。
perf 记录 paranoid/capability/PMU 情况，拒绝时给缺失原因，不自动 sudo、改 sysctl 或扩 scope。
源码路径、命令行、URL、headers、trace attributes 可能含秘密：producer 默认只输出必要元数据，
网络 fixture 不留 body/凭据，原始文件本地保管；脱敏是可追溯变换，不覆盖原证据。
这里不建立泛化安全框架。

## 示范与完成边界

复用 `benchmarks/src/simd_scan.cpp` 的 `scan/wjr/u64` 与 `scan/loop/u64`，合同为返回首个
匹配索引或 n；n、offset、hit 与 mixed seed=42 逐项相同。保留固定长度与 shuffled mixed
结果，不能把只固定长度获益推广到一般分支行为。items_processed 的 mixed 含义是 calls，
不能转为完整扫描元素吞吐。MCA 选择一个标记明确、无 call 的热点 region，与 caller 汇编相连。
Python 示例用同一解释器下的授权小型批处理程序，分别展示自身耗时/累计耗时，
增加同名函数和递归 fixture；不将其与 C++ scan 耗时作速度比较。

MVP 完成意味着：没有采集工具的 Windows/Linux 机器可用固定 fixtures 重建三个格式的
JSON/Markdown 报告，能拒绝合同/单位/环境不匹配，并逐项追到 raw。没有 runtime fixture 的
Linux/PMU/ETW/ARM/iOS 能力均不能在支持表打勾。
