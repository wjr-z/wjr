# 多分片一次组树的收益和代价

固定大小分片逐次 append 时，累计树升高会使后续操作回到逐元素 merge。
新的 [owner-range 重载](../design/btree-shard-range.md)一次处理所有等高 inner roots，
避免这种中途退化；混合高度和小树仍正确回退，不改变普通 merge 或默认存储策略。

## 固定方法

- 功能源 `31921a96ae790b3d5213470bc5b9663bc4e0bc20`；研究源仅改 workflow，
  [run41281630880](https://github.com/wjr-z/wjr/actions/runs/41281630880)包含编译命令和完整 sanitizer。
  [原始数据](data/btree-shard-range.json)保留300个raw重复、60个内存行、全部包络与源码SHA-256。
- Windows i5-13500HX，Clang18.1.8/MSVCSTL143.202408，CPU0；Linux同一Ubuntu24.04 worker、
  EPYC9V45，GCC13.3.0/Clang18.1.3、系统libstdc++，不固定CPU。
  C++20、`-O3 -DNDEBUG -march=x86-64`，无MSVC性能结论，不把不同CPU时间差当作OS影响。
- Google Benchmark1.9.0，固定N16，8个512元素的递增分片，键为`s*1024+i`，互不交叠。
  mapped为u64或`array<byte,64>`；每组32次操作、五个raw重复，**只计归并**，构造/销毁排除。
  同binary、std::allocator，无计数器。所有来源为空、全部键和值均在计时外验证。
- WJR分别测逐分片merge、逐分片append和一次range。std/Abseil先将首个分片swap进空目标，
  再测普通merge或`extract(begin()) + insert(end(), node)`正确hint；不让成熟对照吃首分片的无谓成本。
  Abseil固定未修改源码`2a2d6aad626f75a32c8b7bb775fbff73d558b8b1`。
- 独立计数pass包含source vector的owner对象数组、node/value和所有workspace请求，
  销毁后live=0、分配/释放平衡。requested bytes不等于RSS或pool retained bytes。

## 时间

表内为一次range / 各列对照的中位比值，小于1更快。

| Profile / 类型 | 逐分片append | 逐分片merge | std swap+nodehint | Abseil swap+nodehint |
|---|---:|---:|---:|---:|
| Windows u64 legacy | .0107 | .0087 | .0310 | .0219 |
| Windows u64 compact | .0109 | .0078 | .0295 | .0208 |
| Windows 64B legacy | .0133 | .0100 | .0356 | .0139 |
| Windows 64B compact | .0094 | .0067 | .0303 | .0119 |
| Linux GCC u64 legacy | .0162 | .0102 | .0321 | .0352 |
| Linux GCC u64 compact | .0163 | .0096 | .0323 | .0354 |
| Linux GCC 64B legacy | .0164 | .0099 | .0354 | .0196 |
| Linux GCC 64B compact | .0121 | .0079 | .0349 | .0193 |
| Linux Clang u64 legacy | .0158 | .0111 | .0332 | .0315 |
| Linux Clang u64 compact | .0151 | .0102 | .0322 | .0305 |
| Linux Clang 64B legacy | .0152 | .0104 | .0327 | .0164 |
| Linux Clang 64B compact | .0120 | .0080 | .0335 | .0168 |

所有交叉raw包络均低于1。一次range只处理数十个root child descriptors而不遍历4096个值，
结构性差异明确；短操作仍包含pause/resume/barrier底噪，不宣传精确百倍吞吐。
本轮仅固定等高分片；不同高度的合法回退没有被包装为新性能收益。

## 内存负例与操作计数

三个profile的WJR请求一致，单位B；after包含尚存在的空source owner数组。

| 类型 | 逐append after | range after | 逐append peak | range peak |
|---|---:|---:|---:|---:|
| u64 legacy | 154560 | 153856 | 155296 | 155904 |
| u64 compact | 147392 | 146816 | 148256 | 148864 |
| 64B legacy | 383936 | 383232 | 384672 | 385280 |
| 64B compact | 555968 | **556288** | 557728 | **558336** |

workspace共896B、3次分配；range的peak比逐append高608B。
64B compact最终还多320B，因为保留原leaf布局，没有为了合并边界而搬移值。
所以这不是通用memory压缩；它选择值地址/生命周期不变和更少工作，接受临时空间与容量取舍。

本轮逐append发生319次allocation/321次deallocation，逐merge为447/448。
range为3/7：3次workspace请求，释放workspace并淘汰4个旧root，没有新增parent，也没有value moves。
若原root不够会另分配必要parent；20×150功能测试覆盖两个额外parent及其失败清理。
单个非空source无workspace，混合高度probe合法回退并保留实际value移动的负例。

std swap+nodehint本轮归并期间零allocation，Abseil依值大小有240/897次；这些成熟实现优势保留。
大compact结果仍比std/Abseil更大，raw内存表完整记录，不能仅凭低churn宣称更省内存。

## 生命周期、代码与验证

iterator和可能抛出的workspace构造前置，新增parent在rewire前由两层RAII管理。
测试覆盖每个workspace分配失败、第一/第二额外parent的pointer构造失败、首轮及第二轮iterator失败，
原所有者均保持不变且无泄漏。原node/value noexcept域不变；非空公开状态只在完整发布后改变。

MSVC10测试通过；Windows Clang ASan/UBSan非异常8测试通过，异常清理依赖同源Linux完整10测试通过。
不声称本机WinEH异常sanitizer路径通过，也不为已知工具链限制扭曲代码。
覆盖N4/5/16/64、prefix/空/单source、引用和counted范围、string separator、greater/multiset、
immovable/move-only、PMR、align64、allocator ID、value地址和hook观察。

hook observer核对iterator数量及first/middle/last的find/lower/upper层次关系，
完成后再全量核对地址；替代每次hook临时建立堆set，Windows ASan同覆盖从43.7s降至4.3s。
普通standard merge整wrapper及callee汇编新旧相同（仅去注释空行），包括include新range头但不使用的情况。
heavy combine只按traits生成，不随iterator/sentinel类型复制；不增加tree字段或void*擦类型路径。

```sh
cmake -S . -B build-shards -GNinja -DCMAKE_BUILD_TYPE=Release \
  -DWJR_CXX_STANDARD=20 -DWJR_CPU_TARGET=baseline \
  -DWJR_BUILD_BENCHMARKS=ON -DWJR_BENCHMARK_PROFILE=btree
cmake --build build-shards --target btree_shard_range btree_shard_range_memory --parallel 4
./build-shards/bin/btree_shard_range --benchmark_out=shards.json \
  --benchmark_report_aggregates_only=false
./build-shards/bin/btree_shard_range_memory > shards.csv
```

两个target均EXCLUDE_FROM_ALL。计数allocator共享现有merge probe的独立helper，
RAII保持计数器寿命；提取后原disjoint probe的64内存行逐字等价，计时分支不引入计数器。
