# Compact repack：用一次值迁移回收叶空闲空间

纯图归并保留原叶布局，因而很快，但不能消除顺序插入和分片留下的空闲 slot。
显式 [repack](../design/btree-compact-repack.md) 在归并后移动全部 compact 值，回收约44–46%的保留请求字节。
它比只改图慢7.6–24.9倍；不进入默认修改路径，也不替换 legacy 的值地址合同。

## 固定方法

- 功能源 `f9510ae9209c31c088782c80af9af73f23ceb9bc`；研究源 `e533e1a50c01b37b2874184af32b45e8a787c397`
  仅更改 workflow。[Linux run41284632291](https://github.com/wjr-z/wjr/actions/runs/41284632291)
  保留编译命令、CPU/工具链和完整 ASan/UBSan。[数据](data/btree-compact-repack.json)包含330个 raw 重复、
  66个内存行、全部包络及6个功能文件的精确 SHA-256。
- Windows i5-13500HX、Clang18.1.8、MSVCSTL143.202408、固定CPU0；Linux Ubuntu24.04同一 EPYC9V74 worker，
  GCC13.3.0/Clang18.1.3、系统 libstdc++，不固定CPU。C++20、`-O3 -DNDEBUG -march=x86-64`。
  没有 MSVC 速度结论，也不把不同CPU的绝对时间差归因为OS。
- 沿用 shard fixture：N16、8个512-key shard、键为 `s*1024+i`，mapped 为u64或 `array<byte,64>`。
  Google Benchmark1.9.0，每组32次操作、5个 raw 重复；同binary、std allocator，无计数器。
  构造/销毁 fixture 排除，**计时包含完整归并、全部值迁移、workspace和旧节点释放**。
  在计时外验证全部键/值和所有 source 为空，没有只测更密的结果而漏掉搬移成本。
- std/Abseil先swap首片进空目标，再比较merge和 `extract(begin()) + insert(end(), node)` 的正确hint。
  Abseil固定未修改源码 `2a2d6aad626f75a32c8b7bb775fbff73d558b8b1`，不使用首片逐元素归并的弱对照。
- Windows binary在提交前由冻结功能代码构建；原context显示 `7253995b...-dirty`。
  构建后的唯一源码调整是恢复 btree.hpp 原有的无末尾LF格式，没有C++ token变化；该来源说明在数据中保留。

## 时间收益与代价

表内为 `batch + repack` / 对照的中位时间比，小于1更快。

| Profile / mapped | 仅图batch | 逐片append | 逐片merge | std swap+nodehint | Abseil swap+nodehint |
|---|---:|---:|---:|---:|---:|
| Windows u64 | 16.198 | .157 | .125 | .486 | .297 |
| Windows 64B | 24.874 | .244 | .172 | .775 | .288 |
| Linux GCC u64 | 7.626 | .153 | .101 | .236 | .260 |
| Linux GCC 64B | 15.695 | .220 | .146 | .535 | .282 |
| Linux Clang u64 | 7.747 | .196 | .116 | .248 | .253 |
| Linux Clang 64B | 14.372 | .276 | .171 | .505 | .272 |

相对仅图batch的所有包络均大于1，相对其余对照的包络均小于1。
这里包络是 `[min(candidate)/max(reference), max(candidate)/min(reference)]`，不是置信区间。
例如Windows 64B对std为 `[.640,.858]`，Linux Clang为 `[.464,.545]`。
候选绝对中位时间：Windows u64/64B为21.97/38.17µs，Linux GCC为13.41/28.39µs，
Linux Clang为13.77/28.42µs。纯图batch约1–2µs，更接近框架/计时底噪，巨大倍数不应用作精确的微操作成本模型。

这不是免费压缩：每个现有值都重新构造一次并销毁旧对象，多出274次分配和509次释放。
若最终空间并不重要，应该保留纯图结果；已达到最少叶数的树直接返回false。
没有测重建后的 lookup/scan，不能据此宣称读吞吐提高，也没有为选择自动阈值增加新实验。

## 保留字节和峰值

独立 typed counting allocator pass，包含source vector的owner对象数组、所有node/value/workspace。
所有profile的compact和Abseil行一致；下表是请求字节而非RSS/allocator retained，析构后live全部为0。

| mapped / 操作 | 保留请求字节 | 峰值请求字节 | 归并阶段分配 / 释放 |
|---|---:|---:|---:|
| u64 纯图batch | 146816 | 148864 | 3 / 7 |
| u64 batch+repack | 79136 | 149152 | 277 / 516 |
| u64 Abseil swap+nodehint | 73664 | 80064 | 240 / 259 |
| 64B 纯图batch | 556288 | 558336 | 3 / 7 |
| 64B batch+repack | 309408 | 559520 | 277 / 516 |
| 64B Abseil swap+nodehint | 320680 | 326672 | 897 / 910 |

相对纯图batch，u64保留字节下降46.1%，64B下降44.4%；峰值分别只多288B和1184B，
因为旧叶消费后即释放，而不是同时保留完整旧图和完整新图。
这不表示峰值已接近最终结果：输入本身已占147968B/557440B，不能把其历史成本删掉。
相对Abseil，u64最终仍多7.4%，64B最终少3.5%，但峰值分别仍多86.3%/71.3%。
std nodehint保留Windows197168B/427048B、Linux196992B/426368B，源自其不同STL布局；不混合计算跨平台比例。

单独4096个 align64、自引用 move-only payload 的资源probe也验证4096次构造/旧销毁、精确对齐与完整释放：
977088B→545568B、峰值981248B。该pair含128B对齐布局，与上述64B array fixture不同，不混为同一数据。

## 正确性及使用边界

MSVC Release13项通过；Windows Clang ASan/UBSan10项非异常测试通过，
完整Linux ASan/UBSan13项通过，包括workspace allocation/metadata construct失败。
Windows异常sanitizer受此前独立复现的 WinEH funclet问题阻断，未宣称该部分本机通过。
格式检查通过；未使用新header能力时，默认 legacy merge wrapper及callee完整汇编与基线相同
（仅去注释/空行比较），未向默认路径增加运行时标志或调用。

只在现有compact scalar map、修改阶段结束、允许全部引用失效且最终空间确有价值时显式调用。
工作区准备失败保持原树不变；开始迁移后沿用原node/value不抛出/terminate合同，
不是std::map级可恢复OOM或引用稳定性的替代方案。

复用现有显式target，无新增自动矩阵：

```sh
cmake -S . -B build-repack -GNinja -DCMAKE_BUILD_TYPE=Release \
  -DWJR_CXX_STANDARD=20 -DWJR_CPU_TARGET=baseline \
  -DWJR_BUILD_BENCHMARKS=ON -DWJR_BENCHMARK_PROFILE=btree
cmake --build build-repack --target btree_shard_range btree_shard_range_memory --parallel 4
./build-repack/bin/btree_shard_range --benchmark_out=repack.json \
  --benchmark_report_aggregates_only=false
./build-repack/bin/btree_shard_range_memory > repack.csv
```

Abseil比较另显式启用 `WJR_BENCHMARK_ABSEIL` 并指定上述固定源码。
两个target保持 `EXCLUDE_FROM_ALL`，一般benchmark或库构建不会新增计时工作。
