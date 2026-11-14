# 不等高有序分片的 merge

普通 legacy `btree_map/set/multimap/multiset::merge` 可将严格不交叠、相同 traits、标准
`less/greater` 且 allocator 相等的两棵 heap 图连接到高树的边缘 spine。已有叶和值对象直接
进入结果，source 为空且可复用。低 root 的占用需满足连接后 non-root 的约束。
普通 experimental compact 仍使用其既有元素转移路径。
[设计说明](../design/btree-unequal-merge.md)记录规范化、分裂和 hook 发布顺序。

## 工作负载与来源

main 为 `9227a90300247bf4eca560918ca23b5f38dd2cec`，包含等高 graph merge。
计时 baseline `e5f95e503e5db9d1f779ef81fc3d192f46361bed` 只增加同源 benchmark 语料，
候选源码冻结于 `471d4d157ef3b390dc0463ca9c57657695c1d4cc`。
两侧 `benchmarks/src/btree_merge.cpp` 的 SHA256 均为
`03ad944e69652cd5a985cbd9518fb7c2a737aa30f5a69a0cd61483e559f8b7ee`。

使用容量 N16，key 为 uint64_t，mapped 为 uint64_t 或64B array。各树按递增键构造。
destination 初始4096项，单 source 含9、64或4096项。连续导入先构造32个64项或150项
source，每次 `merge` 后继续使用同一 destination。prepend 测试依次导入更小的分片。
构造、销毁和 source owner 数组分配排除在计时区间外。
单次操作固定32次迭代，连续32分片固定8次迭代，每进程5个 raw 重复，交替顺序3轮。
下表每项为15个 raw 样本的中位数，连续分片为整个32次 merge 的耗时。

Windows 使用 i5-13500HX、Clang18/MSVC19.42、固定 CPU0。Linux 使用 EPYC 7763、GCC13/Clang18，
编译命令记录在同次 artifact。所有 profile 均为 C++20 Release baseline。
std 和 Abseil 对照同时包含普通 merge 与逐节点 extract 加合法边缘 hint，prepend 使用 begin hint。
Abseil 固定于 `2a2d6aad626f75a32c8b7bb775fbff73d558b8b1`。

## 连续分片计时

| Profile | Shards | main us | core us | std hint us | Abseil hint us |
|---|---|---:|---:|---:|---:|
| Windows Clang18 | accumulate64 | 119.913 | 3.525 | 27.025 | 42.638 |
| Windows Clang18 | accumulate150 | 271.862 | 6.725 | 78.762 | 105.825 |
| Windows Clang18 | prepend64 | 74.838 | 4.100 | 30.863 | 43.738 |
| Windows MSVC19.42 | accumulate64 | 108.312 | 3.612 | 50.025 | 61.463 |
| Windows MSVC19.42 | accumulate150 | 252.737 | 5.900 | 134.987 | 141.838 |
| Windows MSVC19.42 | prepend64 | 93.238 | 3.963 | 50.312 | 85.937 |
| Linux GCC13 | accumulate64 | 125.349 | 4.383 | 49.236 | 37.605 |
| Linux GCC13 | accumulate150 | 294.642 | 5.316 | 124.737 | 87.908 |
| Linux GCC13 | prepend64 | 103.104 | 4.923 | 47.628 | 45.236 |
| Linux Clang18 | accumulate64 | 105.349 | 4.214 | 43.985 | 39.163 |
| Linux Clang18 | accumulate150 | 238.498 | 5.353 | 115.371 | 93.270 |
| Linux Clang18 | prepend64 | 65.341 | 4.597 | 49.237 | 32.793 |

两种 mapped、四个 profile 的连续32分片场景，相对原实现快12–55倍，相对同机最强
std/Abseil 路径快7.1–22.9倍。单个64项分片相对原实现快2.3–5.7倍，相对最强对照快
1.5–3.0倍。单个9项 leaf 的 Linux 结果接近最强 Abseil hint，u64 样本慢约4–5%。

## Fallback controls

下表为完整 profile 中 candidate 相对 main 的耗时变化，正值表示耗时增加。
`uq-conflict/overlap` 为64→4096，`leaf-sparse` 为9→4096，`tall-conflict` 为4096→64。
完整两种 mapped 的所有实现及逐轮中位数见数据文件。

### u64

| Profile | empty | interleaved | conflict | sparse | uq-conflict | uq-overlap | leaf-sparse | tall-conflict |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Windows Clang18 | -0.4% | -6.5% | -4.6% | +4.2% | +0.7% | +1.0% | -4.1% | +3.1% |
| Windows MSVC19.42 | +1.4% | +1.8% | -2.4% | -1.5% | -0.9% | +0.7% | +3.6% | +2.0% |
| Linux GCC13 | -5.2% | -1.4% | -11.4% | -2.6% | -5.5% | -1.7% | -4.5% | -1.9% |
| Linux Clang18 | -3.6% | +0.5% | -0.8% | -0.5% | -3.7% | -2.6% | -3.1% | +0.3% |

### large

| Profile | empty | interleaved | conflict | sparse | uq-conflict | uq-overlap | leaf-sparse | tall-conflict |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Windows Clang18 | -0.8% | +0.3% | +2.3% | +7.7% | +6.3% | +3.5% | -0.3% | +3.8% |
| Windows MSVC19.42 | +3.8% | +2.2% | +2.8% | +10.6% | +4.9% | +2.6% | -2.6% | +0.2% |
| Linux GCC13 | -4.2% | +0.3% | +2.4% | -5.3% | -3.8% | -4.1% | -6.4% | -0.0% |
| Linux Clang18 | -5.3% | +0.1% | -0.7% | -2.5% | -3.9% | -1.1% | -6.2% | -0.2% |


Windows large sparse 在全域运行中增加7.7%/10.6%，同源定向复测为3.1%/1.7%。
同次 reference 增加5.9%/5.3%，std/Abseil 约持平。large unequal conflict 在定向复测为
-1.3%/+4.6%，MSVC std 同项增加4.1%。Linux 的上述两类输入本轮均有小幅降低。
这些结果保留了实际选路成本及布局/计时不确定性，使用场景以有序不交叠分片持续累加为主。
[汇总数据](data/btree-unequal-merge.json)同时保留完整控制与定向复测。

## 所有权和选择成本

4096+64 的节点分配/释放从8/8降到0/0。连续32×64从255/256降到2/11，32×150从599/544
降到4/0，prepend32×64从250/256降到2/11。两种 mapped、四个 profile 的全部内存样本
销毁后 outstanding 均为0。这里统计 tree allocator 的节点请求。

自动入口为不等高树增加 root separator 区间判断，通过后再检查真实叶端点。
完全重叠的 inner root 区间可提前拒绝。混合、冲突和 sparse 输入仍需承担选路判断。
全域结果与定向控制复测分别保留，几百分点差异包含布局和计时波动。

验证包括 Clang 正常 CMake 的953项主单测、MSVC31项 merge/join 定向测试，以及 Linux
ASan/UBSan 的同31项测试。覆盖两种方向、满 spine 连续分裂、容量4/5/16/64、less/greater、
inline/indirect key、immovable mapped、内部重复 multi、PMR source 销毁后使用、allocator identity、
hook 内 find/遍历、source 复用及后续随机 erase/copy/move/swap。

本轮性能样本使用标准 allocator、N16和整数键。其他容量、字符串比较和自定义 allocator 的吞吐
未测量，所有权与取址契约由针对性测试覆盖。
[Linux 原始配对和 sanitizer artifact](https://github.com/wjr-z/wjr/actions/runs/43618367250)
保留完整 raw 样本、编译命令、source hashes、内存 CSV 和 worker 信息。
