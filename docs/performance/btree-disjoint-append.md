# 有序 disjoint 分片的 graph append

显式 `append_btree_disjoint` 对满足[区间/所有权合同](../design/btree-disjoint-append.md)的分片拼接节点，
保持普通 merge 不变。本轮四种 map 在适合的 inner-root 场景中，Windows/Linux 均比现有 merge 及成熟
std/Abseil 的 node-extract+end-hint 路径快。不同高度仍回退，不能用于宣称通用 merge 更快。

## 方法

- 功能源 `2aeacc01da26a46f0e7db12f5262425ece92a9a4`，研究源
  `30dd37dad17023577f1f9347124ff7f0786d1a98` 仅改研究 workflow，功能/测试/benchmark blob 一致。
  [原始数据](data/btree-disjoint-append.json)含全部 960 raw 重复、192 内存行、交叉包络与源码 SHA-256。
  [Linux run 41272238364](https://github.com/wjr-z/wjr/actions/runs/41272238364)含编译命令、原始输出和 sanitizer。
- Windows i5-13500HX，Clang18.1.8/MSVC STL143.202408，固定 CPU0；Linux 同一 Ubuntu24.04 worker、
  AMD EPYC7763，依次 GCC13.3.0 和 Clang18.1.3、系统 libstdc++，不固定 CPU。
  均 C++20、`-O3 -DNDEBUG -march=x86-64`。没有 MSVC 性能结论，不将不同 CPU 的绝对差异归因 OS。
- Google Benchmark1.9.0，每重复固定32次、五个 raw 重复，wall time；**只计 append/merge**，
  逐项递增构造两树和销毁均排除。每个 profile 的 candidate/reference 同 binary、相同 `std::allocator`，
  无计数器。u64 key，u64 或 `array<byte,64>` mapped，固定 N16。
- WJR 对照是当前标准 comparator 的普通 merge，和 append 使用完全相同的 traits。
  std/Abseil 同时测普通 merge 和以下合法的正确 end hint 路径：

  ```cpp
  while (!source.empty())
      destination.insert(destination.end(), source.extract(source.begin()));
  ```

  Abseil 固定未修改源码 `2a2d6aad626f75a32c8b7bb775fbff73d558b8b1`。
  没有新增 WJR node handle API，也不只使用较弱的通用 merge 基线。
- 内存使用独立可执行文件及计数 allocator/rebind，包括 node/value 全部请求，销毁后 live=0、次数平衡。
  这是 requested bytes，不是 RSS 或 pool retention；typed 栈工作区另行记账。

四种输入均 destination 从键0递增构造，source 从8192递增构造，严格不交叠：

| 模式 | destination / source 个数 | 本轮实际路径 |
|---|---:|---|
| leaf9 | 9 / 9 | 保留两叶，分配 parent |
| inner150 | 150 / 150 | 均分 child，分配 parent |
| large4096 | 4096 / 4096 | 合并 root children，释放一个 root |
| unequal | 4096 / 64 | 高度不同，普通 merge 回退 |

## 时间：append / 同 traits 普通 merge

| 模式/值/存储 | Windows Clang | Linux GCC | Linux Clang |
|---|---:|---:|---:|
| leaf9 u64 legacy | .816 | .860 | .918 |
| leaf9 u64 compact | .788 | .860 | .893 |
| leaf9 64B legacy | .796 | .872 | .902 |
| leaf9 64B compact | .757 | .804 | .850 |
| inner150 u64 legacy | .159 | .216 | .277 |
| inner150 u64 compact | .183 | .234 | .278 |
| inner150 64B legacy | .166 | .213 | .285 |
| inner150 64B compact | .123 | .167 | .196 |
| large4096 u64 legacy | .004 | .010 | .012 |
| large4096 u64 compact | .004 | .008 | .009 |
| large4096 64B legacy | .005 | .010 | .013 |
| large4096 64B compact | .004 | .007 | .008 |
| unequal u64 legacy | 1.049 | .999 | .986 |
| unequal u64 compact | .993 | 1.005 | .994 |
| unequal 64B legacy | .823 | .985 | 1.005 |
| unequal 64B compact | 1.061 | 1.005 | 1.000 |

inner150 与 large4096 的全部 raw 交叉包络都低于1。large4096 的固定 metadata 工作很短，
包含 harness pause/resume 和 barrier 底噪，不能把这些比值当作精确百倍吞吐收益。
结构性证据是没有逐元素 value 操作，node churn 由数百次降至0或1次。

小型与回退结果有明确限制。Windows u64 legacy leaf9 的包络为 [.757,1.869]，不宣称确定收益。
所有 unequal 的包络跨1，包括 Windows compact64B 1.061 [.965,1.524]；不宣称回退更快或完全零成本。
本轮 unequal 的 WJR append 仍比 std node-hint 慢约1.46–2.42倍、比 Abseil node-hint 慢约1.20–2.21倍。
这些成熟实现的优势保留在数据中，不能从等高图结果推广到不等高累加。

强对照的 large4096，append / **node-hint** 的中位比值：

| Profile | u64 legacy/std | u64 compact/std | 64B legacy/std | 64B compact/std | u64 legacy/Absl | u64 compact/Absl | 64B legacy/Absl | 64B compact/Absl |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Windows Clang | .014 | .017 | .018 | .018 | .009 | .011 | .006 | .007 |
| Linux GCC | .025 | .021 | .027 | .024 | .036 | .030 | .019 | .017 |
| Linux Clang | .027 | .020 | .030 | .025 | .029 | .021 | .017 | .014 |

## 内存和必要成本

三个 profile 的 WJR 大型结果一致：

| 类型 | 普通 merge alloc/free | append alloc/free | merge after B | append after B | merge peak B | append peak B |
|---|---:|---:|---:|---:|---:|---:|
| u64 legacy | 512/511 | 0/1 | 309216 | 308640 | 309504 | 308928 |
| u64 compact | 512/511 | 0/1 | 294624 | 294048 | 294912 | 294336 |
| 64B legacy | 512/511 | 0/1 | 767968 | 767392 | 768256 | 767680 |
| 64B compact | 512/511 | 0/1 | 1109984 | 1109408 | 1111168 | 1109696 |

append 仅省去两个 inner node 的最终占用（576B），主要收益是免除逐元素工作和 churn，
不是重写叶存储或解决 compact 大 mapped 的总体内存问题。leaf9/inner150 都分配一个288B parent；
unequal 保留原 merge 的8次分配与8次释放。完整小场景/强基线内存数据也已保存。

固定 typed 工作区 N16/u64 separator 为536B，N64为2072B；实际栈帧由编译器决定。
它没有动态工作区，但不是零空间。普通路径只新增 friend 声明；标准 u64 legacy merge 的完整
wrapper/callee 汇编在新旧 core 间逐字相同（仅去注释、空行），包括“include实验头但不调用append”的情形。
不增加 tree/node 字段，不把 typed node/key/allocator 擦成 void 指针，也不统一强制 noinline。

## 正确性与重现

Clang Release 的1156组边界 shape/mutation smoke、MSVC四个针对测试、Windows和Linux Clang
ASan/UBSan各四测试通过。包括 Source reuse、后续随机erase/copy/move/swap、奇偶N4/5/16/64、
long string separator、multiset、PMR、align64非平凡move-only、allocator ID保持、hook观察完整公开状态。

```sh
cmake -S . -B build-join -GNinja -DCMAKE_BUILD_TYPE=Release \
  -DWJR_CXX_STANDARD=20 -DWJR_CPU_TARGET=baseline \
  -DWJR_BUILD_BENCHMARKS=ON -DWJR_BENCHMARK_PROFILE=btree
cmake --build build-join --target btree_join btree_join_memory --parallel 4
./build-join/bin/btree_join --benchmark_out=join-timing.json \
  --benchmark_report_aggregates_only=false
./build-join/bin/btree_join_memory > join-memory.csv
```

这两个 target 为 `EXCLUDE_FROM_ALL`，复用已有 merge probe；未启用 join 宏时不解析实验头或注册新场景。
可通过已有 Abseil option 和 reviewed-source 路径加入强对照；没有默认下载新依赖或新增生产 workflow。
