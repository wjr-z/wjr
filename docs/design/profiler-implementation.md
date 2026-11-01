# Profiler 实施状态与后续验收

本文是阶段状态和未完成工作的索引，不复制已交付 provider 的操作步骤或完整合同。
当前支持版本、命令、fixtures 和限制以[工具 README](../../devtools/profiler/README.md)
及各 provider guide 为准；设计合同见[profiler 设计](profiler.md)。

## 已交付 provider

| 阶段 | 状态 | 当前合同与详细证据 |
| --- | --- | --- |
| P01 Google Benchmark | importer 与合同比较已交付 | [当前版本与运行指南](../../devtools/profiler/README.md) |
| P02 LLVM-MCA | importer 与模型估计比较已交付 | [MCA guide](../../devtools/profiler/LLVM_MCA.md) |
| P03 cProfile | JSON importer、显式 exporter 和单次热点摘要已交付 | [cProfile guide](../../devtools/profiler/CPROFILE.md) |
| P04a perf stat | 离线 importer 与进程级摘要已交付；正向硬件采集未验证 | [perf stat guide](../../devtools/profiler/PERF_STAT.md) |
| P05 curl | 离线 importer 和 owned-localhost fixture 已交付 | [curl guide](../../devtools/profiler/CURL.md) |

P01–P03 构成本设计定义的离线 MVP，均已有实现。P04a、P05 的 importer 也已交付；这不代表
正向硬件采集、任意版本兼容或通用 collector 已交付。P00 的真实来源和原始字节证据随
provider fixture/provenance 保存；手写 synthetic fixture 只验证 schema 边界，不是测量。

## 后续 provider 准入

先有明确 workload 和使用问题，再固定 producer 版本、真实输入格式和最小离线导入范围。
未知或不匹配版本只作 exploratory；保留 raw、manifest、producer 退出状态及缺失原因，不能把
unsupported/not-counted 补成零。每项的新数据源须与 runtime measurement、model estimate、
sampled 或 derived 类型区分，不能自动参加跨 provider 排名或回归门禁。

| 候选 | 最小交付及边界 |
| --- | --- |
| P04b perf record | 有真实热点需求后读取匹配版本 `perf script` 导出；记录 build-id、符号和 lost samples。不解码 `perf.data`、不 attach 全系统、不自动提权 |
| P06a py-spy | 固定一个 speedscope sampled 格式；保留 weights、采样频率、线程、GIL/idle/native 状态和丢样；权限不足时只支持离线导入 |
| P06b allocation | 已交付 [MemoryManager 1.9.2 原生字段导入](../../devtools/profiler/MEMORY_MANAGER.md)，明确独立 pass、hook coverage、字节和分配单位；不全局替换 `new`。采样、RSS 和 Python 分配/拷贝归因仍按需验收 |
| P07a iperf3 | 固定实际 JSON 版本和两端授权 workload，记录方向、协议、并发、omit、zerocopy；不能由 HTTP 延迟推导链路吞吐 |
| P07b OTel | 为实际 SDK/proto/exporter 固定版本和输入格式；trace/span 因果关系、clock uncertainty 与跨机时间线分别验收 |
| P08 ETW/WPA | 固定 ADK、导出命令/profile、表头、单位和 locale；保留 ETL 与官方导出表。不做通用 ETL decoder、不增加 Windows 编译矩阵 |

Tracy 仅在需要 zone、锁或跨语言时间线时再评估，并固定 capture/export 子集及插桩成本。
ARM/iOS profiler 能力需等待实际设备、工具链与 workload，不能由离线 MVP 或交叉编译推断。

## 验收边界

每个新增 provider 都需要实际 fixture、来源记录、离线导入测试和支持/不支持说明。Python-only
能力不改变 WJR C++ target；新增 C++ producer 才按对应模块要求构建和格式化。文档、离线
parser 或 provider guide 不构成增加 Windows CI job、自动采集服务、性能阈值或授权范围的依据。
