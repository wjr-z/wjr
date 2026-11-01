# 空目标 merge 的整图接管

批次/分片归并的第一个 `destination.merge(source)` 经常面对空目标。原实现即使没有任何键冲突，仍逐项删除来源并插入目标，重建所有节点。现在在相同 traits、已知标准比较器、来源拥有 heap root 时，复用现有 `_move_tree` 接管 root、size 和叶链。

值对象及节点的生命周期继续存在，不执行 value hooks 或节点重建；两边 allocator/comparator 实例不传播，source 成为可复用的空树。allocator 相等前提不变。**非空目标、SSO、自定义比较器、不同 traits 均保留原循环。**详细合同见 [merge 设计](../design/btree-merge.md)。

## 固定方法与限制

功能冻结 `692d355e`，研究 commit `b7e3b4cc24c7e30330ed64be9147898ced358a26` 仅改研究 workflow；相关头、benchmark 和测试的 SHA-256 已核对。[原始数据](data/btree-owner-adopt.json)保留 720 个最终 raw 重复、144 个内存行、全部控制组与失败 hint 原型的结果。[Linux run 41268371450](https://github.com/wjr-z/wjr/actions/runs/41268371450)含原始输出、编译命令和 sanitizer。

交付前已刷新到 packed-build 合入后的 main `91a8651b`。merge 的实际改动行、benchmark/test blob 与冻结版本一致；仅合并两个独立 `EXCLUDE_FROM_ALL` target 列表及上游 bulk friend 声明，CMake 配置通过。等价记录随数据保存，不重复整构建或计时。

- Windows i5-13500HX，Clang18.1.8/MSVC STL143.202408，固定 CPU0；Linux 同一 Ubuntu24.04 worker、AMD EPYC9V45，依次 GCC13.3.0/Clang18.1.3、系统 libstdc++，不固定 CPU。均 C++20、`-O3 -DNDEBUG -march=x86-64`。没有 MSVC 性能结论，不把跨 CPU 时间差归因 OS。
- Google Benchmark1.9.0，每个重复固定 32 次 merge，五个 raw 重复。**只计 merge，构造两棵树和销毁不计时**；同一 binary 使用 `std::allocator`、无计数器。
- WJR reference 是委托 `std::less<>` 的无状态等价 comparator，选择原通用路径。该 custom-comparator wrapper 及全部 callee 的 Clang 汇编与 base `e5fc5f81` 相同，仅删除注释/空行；新 standard-order 分支仍可能有不同编译布局，不能把控制组差异忽略。
- std 使用自身 `merge`，Abseil 使用未修改的固定源码 `2a2d6aad626f75a32c8b7bb775fbff73d558b8b1`。这些实现的节点和失效语义不同，结果仅表示此工作负载。
- 独立可执行文件以相同计数 allocator/rebind 测 requested bytes 和分配/释放次数，全部销毁后 live=0。没有测 RSS 或 pool retention。

所有类型是 N16、u64 key，mapped 为 u64 或 `array<byte,64>`。输入以递增次序逐项构造：

| 模式 | 来源 | 目标 | 冲突数 |
|---|---|---|---:|
| empty | 4096 个连续键 | 空 | 0 |
| interleaved | 4096 个偶数键 | 4096 个奇数键 | 0 |
| conflicting | 4096 个连续键 | 相同的 4096 键 | 4096 |
| sparse | 64 个键 `32+64*i` | 连续键 0..4095 | 64 |

## 空目标结果

以下为整图接管 / 原通用路径的 wall-time 中位比值，括号是 raw 交叉包络 `[min(new)/max(ref), max(new)/min(ref)]`，不是置信区间。

| 类型 | Windows Clang | Linux GCC | Linux Clang |
|---|---:|---:|---:|
| u64 legacy | .0031 [.0026,.0041] | .0066 [.0064,.0067] | .0081 [.0077,.0083] |
| u64 compact | .0029 [.0022,.0033] | .0064 [.0062,.0077] | .0072 [.0071,.0074] |
| 64B legacy | .0034 [.0023,.0042] | .0068 [.0066,.0080] | .0080 [.0079,.0082] |
| 64B compact | .0026 [.0024,.0028] | .0057 [.0055,.0059] | .0059 [.0059,.0060] |

接管路径的固定工作很短，读数包含 benchmark 的 pause/resume、barrier 等计时底噪，因此不将比值宣传为精确的百倍吞吐提升。结构性证据更直接：四个形态在全部三个 profile 都从 **511 次 node allocation + 511 次 deallocation 变为零**，也没有 value move。该操作不随来源元素数遍历。

同一次 empty 测量中，接管时间 / std 的范围为 .0032–.0099，/Abseil 为 .0035–.0183；同样受短路径计时底噪约束。std 本轮已是零 merge allocation，但仍按自身 merge 算法工作；Abseil u64/64B merge 为 281/1029 次分配及释放，原始数据保留这些差别。

内存最终占用未改变，只消除重建时的临时节点峰值。下表三个 profile 一致：

| 类型 | before/after requested B | 原路径 peak B | 接管 peak B |
|---|---:|---:|---:|
| u64 legacy | 154464 | 154752 | 154464 |
| u64 compact | 147168 | 147456 | 147168 |
| 64B legacy | 383840 | 384128 | 383840 |
| 64B compact | 554848 | 556032 | 554848 |

这不是通用内存压缩或减少 retained bytes；现有树的布局、value 间接存储和空闲容量均保留。

## 控制组、负例和代码大小

非空循环保持原文，但新增一次入口判断与不同 comparator 模板实例可能影响布局，**不声称非空吞吐零回退**。本轮非空比值范围为 Windows .921–1.059、GCC .957–1.078、Clang .926–1.026。GCC 的 u64 legacy interleaved 为 1.078 [1.025,1.123]，u64 compact conflicting 为 1.072 [1.016,1.135]；这两个包络完全超过 1，不能归为已证明无差别。Windows 的 u64 compact conflicting 为 .921 [.871,.971]；同样不将其当作非空算法优化。全部模式的五次结果可从原始数据单独评估。

曾尝试在标准同序 merge 中自动复用 range hint；Windows sparse u64 compact 达 1.264 [1.049,1.471]，64B legacy 达 1.371 [1.181,2.232]。因此整个 hint 集成撤回，没有阈值或新 API，也没有追加 Linux 原型矩阵。最终仅保留自然的整图所有权边界。

没有增加运行时 helper 或 type erasure。标准 u64 legacy merge wrapper 加实例化 callee 的代表性 Clang18 `-O3` object，text 11844→11940B（+96B），data 1274B 不变；custom comparator 的完整汇编不变。这不是全库代码大小上限。按 allocator/node 类型变化的合法模板边界保留。

## 验证和重现

最终本机 Clang Release 124 个 Btree 测试、MSVC Release 7 个 merge 测试、Windows Clang ASan/UBSan 7 测试通过；同源 Linux Clang ASan/UBSan 7 测试通过。覆盖空/SSO/多层根、source 再使用、self、冲突保留、stateful comparator、两种标准 comparator spelling、不同 NodeSize、multiset、PMR、align64 move-only、值地址/计数与 allocator hooks 平衡。

```sh
cmake -S . -B build-merge -GNinja -DCMAKE_BUILD_TYPE=Release \
  -DWJR_CXX_STANDARD=20 -DWJR_CPU_TARGET=baseline \
  -DWJR_BUILD_BENCHMARKS=ON -DWJR_BENCHMARK_PROFILE=btree
cmake --build build-merge --target btree_merge btree_merge_memory --parallel 4
./build-merge/bin/btree_merge --benchmark_out=merge-timing.json \
  --benchmark_report_aggregates_only=false
./build-merge/bin/btree_merge_memory > merge-memory.csv
```

两个 target 为 `EXCLUDE_FROM_ALL`；默认不新增编译。已有 `WJR_BENCHMARK_ABSEIL` 和 reviewed-source 路径可加入 Abseil，关闭时不查找/下载它。
