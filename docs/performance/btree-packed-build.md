# 有序输入的 packed B+ tree 构建

本轮将显式 `make_btree_from_sorted_unique` 与现有 **有效 hint 导入** 比较。它适合已经严格有序且唯一的快照；普通构造和插入不自动切换。[接口、生命周期和异常合同](../design/btree-packed-build.md)单独说明。

结论：compact map 的构建与初始内存占用在本轮三个 profile 中均明显改善；legacy map 的内存也下降，但 Linux N4096 有明确 CPU 退化，因此不能作为通用的更快导入方式。CPU 敏感的 legacy 调用继续使用 `insert_with_hint`。没有修改默认容量、普通节点布局或地址语义。

## 固定口径

- 功能源冻结 commit `002e092415103b4865baa30883592dd6e057980f`；研究 commit `eea9c18ac6438f507773e17fa53e27ffd0d3b8dd` 只替换研究 workflow，功能头、测试和测量 TU 的 SHA-256 一致。后续交付仅补文档/数据。
- Windows：i5-13500HX，Clang 18.1.8 / MSVC STL 143.202408，固定 CPU0。Linux：同一 Ubuntu 24.04 worker、AMD EPYC 7763，依次 GCC 13.3.0 和 Clang 18.1.3，系统 libstdc++，未固定 CPU。三个 profile 都是 C++20、`-O3 -DNDEBUG -march=x86-64`。没有 MSVC 编译器的性能结论；MSVC 做正确性验证。
- Google Benchmark 1.9.0，N16，u64 key，u64 或 `array<byte,64>` mapped；n=17/64/4096，严格递增且唯一。每组五个原始重复、每重复至少 0.02s，使用 wall time。**预先生成输入不计时，构造及销毁计时**。同一 binary 中 candidate/reference 均使用 `std::allocator`，没有计数器。不同 CPU 的绝对时间不能归因为操作系统差异。
- WJR reference 使用现有 `insert_with_hint`；std 和 Abseil 对每个递增条目使用 `insert(end(), value)`。Abseil 固定未修改源码 `2a2d6aad626f75a32c8b7bb775fbff73d558b8b1`；没有把源码加入仓库。
- 内存由独立可执行文件计量：所有 rebind 共享计数，包括节点、legacy 单个 value 和 builder 的指针工作区。输入 vector 不计入；值没有其他动态分配。字节为 allocator **requested bytes**，不表示 RSS、malloc 元数据或资源保留量。每组销毁后 live=0，分配/释放次数匹配。

原始五次时间、范围、全部 n=0/1/2/3/16/17/64/4096 内存结果和源码哈希收在 [data/btree-packed-build.json](data/btree-packed-build.json)。Linux 原始 artifact、编译命令和完整 sanitizer 输出见[研究 run 41266410602](https://github.com/wjr-z/wjr/actions/runs/41266410602)。这是一次限定筛查，没有采集硬件计数器、混合后续修改或 lookup 吞吐。

## 时间：packed / 同类型 hint

小于 1 表示 packed 更快。括号为五次 raw 的保守交叉包络 `[min(packed)/max(hint), max(packed)/min(hint)]`，不是置信区间。

| Value / policy / n | Windows Clang | Linux GCC | Linux Clang |
|---|---:|---:|---:|
| u64 legacy 17 | .797 [.683,.969] | .787 [.772,.799] | .969 [.929,1.055] |
| u64 compact 17 | .933 [.886,1.014] | .289 [.288,.291] | .657 [.616,.659] |
| u64 legacy 64 | .807 [.757,.868] | .710 [.707,.725] | .931 [.923,.942] |
| u64 compact 64 | .386 [.355,.425] | .120 [.120,.121] | .253 [.252,.257] |
| u64 legacy 4096 | .515 [.496,.608] | **1.095 [1.075,1.098]** | **1.413 [1.397,1.425]** |
| u64 compact 4096 | .247 [.213,.256] | .128 [.127,.129] | .250 [.240,.254] |
| 64B legacy 17 | .842 [.802,.905] | .726 [.681,.729] | .882 [.877,.892] |
| 64B compact 17 | .812 [.709,.861] | .368 [.363,.372] | .534 [.375,.551] |
| 64B legacy 64 | .841 [.751,.953] | .621 [.618,.630] | .765 [.757,.771] |
| 64B compact 64 | .365 [.187,.392] | .224 [.199,.228] | .275 [.272,.289] |
| 64B legacy 4096 | .609 [.546,.667] | .991 [.968,1.030] | **1.262 [1.254,1.265]** |
| 64B compact 4096 | .337 [.299,.369] | .195 [.189,.197] | .275 [.274,.279] |

Windows 小型 u64 compact 和 Linux Clang 小型 u64 legacy 的包络跨 1；不宣称这些场景有确定收益。Linux N4096 legacy 的退化也不能用“减少分配”解释为整体改善。该接口让调用方选择构建后内存与导入 CPU 的取舍，不添加平台阈值或自动分派。

以下保留 N4096 的强对照，仍为 **WJR packed / 对照的 hinted construct+destroy**：

| Profile | u64 legacy / std | u64 legacy / Absl | u64 compact / std | u64 compact / Absl | 64B legacy / std | 64B legacy / Absl | 64B compact / std | 64B compact / Absl |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Windows Clang | .889 | 2.584 | .067 | .194 | .862 | 1.359 | .137 | .216 |
| Linux GCC | 1.254 | 4.624 | .093 | .343 | 1.136 | 1.701 | .179 | .268 |
| Linux Clang | 1.251 | 3.179 | .088 | .224 | 1.126 | 1.448 | .168 | .216 |

legacy 的每值 allocation 和其地址稳定优势继续存在；packed 并未消除它相对 Abseil 的构建代价。compact 的这些数值只描述本次有序、已知长度、无重复导入，不能推广到随机插入或查找。

## 内存：n=4096

以下四组 WJR 结果在 Windows、Linux GCC 和 Linux Clang 一致。`sizeof(Tree)` 分别保持 legacy 48B、u64 compact 64B、64B compact 176B。

| 类型 | hint built / peak B | packed built B | packed peak B | packed total requested B | hint → packed allocation 次数 |
|---|---:|---:|---:|---:|---:|
| u64 legacy | 154464 | 111392 | 113280 | 113576 | 4607 → 4372 |
| u64 compact | 147168 | 78624 | 80512 | 80808 | 511 → 276 |
| 64B legacy | 383840 | 340768 | 342656 | 342952 | 4607 → 4372 |
| 64B compact | 554848 | 308000 | 309888 | 310184 | 511 → 276 |

N4096 的 packed tree 有 256 个满叶、16 个中间节点和 1 个 root；构建过程中还分配三次指针工作区。legacy 另有 4096 次 value allocation。工作区结束时释放，因此 peak 比 built 大 1888B，total requested 比 built 大 2184B。compact built 下降约 46.6%/44.5%；legacy 下降约 27.9%/11.2%。这些是实际计数，未把工作区当零成本。

Abseil 的 built requested 为 u64 73472B、64B mapped 320488B；因此 u64 packed compact 仍比它多 5152B。std 的对应值 Windows 为 196656/426088B、Linux 为 196608/425984B；实现的 sentry/object 差异保留在原始数据中。

边界负例：n=17 时，packed 与 hint 的最终 WJR requested bytes 完全相同，却新增两次工作区分配、峰值增加 24B。n<=16 不分配指针工作区。满叶较少留下插入余量，未测后续 mutation 的成本，不能从初始内存结果推出其更优。

## 生命周期验证与重现

本机 Clang Release 的 127 个 Btree 测试、MSVC Release 的新增五个测试通过。Windows Clang ASan/UBSan 的四个正常路径测试通过；异常清理测试遇到已知 WinEH sanitizer SEH 访问异常，本机不声称该路径通过。相同完整五测试在上述 Linux run 的 Clang ASan/UBSan 下全部通过，包含 SSO/partial leaf/input increment 和逐层工作区失败。

```sh
cmake -S . -B build-bulk -GNinja -DCMAKE_BUILD_TYPE=Release \
  -DWJR_CXX_STANDARD=20 -DWJR_CPU_TARGET=baseline \
  -DWJR_BUILD_BENCHMARKS=ON -DWJR_BENCHMARK_PROFILE=btree
cmake --build build-bulk --target btree_bulk_build btree_bulk_memory --parallel 4
./build-bulk/bin/btree_bulk_build --benchmark_out=bulk-timing.json \
  --benchmark_report_aggregates_only=false
./build-bulk/bin/btree_bulk_memory > bulk-memory.csv
```

这两个 target 是 `EXCLUDE_FROM_ALL`，默认 benchmark 和测试构建不会编译它们。启用既有 `WJR_BENCHMARK_ABSEIL` 及正确的 reviewed source 路径可加入 Abseil；关闭时不查找/下载它。实际编译参数见 run artifact 中的 compile commands。
