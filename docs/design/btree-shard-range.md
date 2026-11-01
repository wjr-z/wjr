# 一次归并多个有序 owner 分片

逐个调用 disjoint append 时，累计树会升高，后续小分片可能落到不同高度的逐元素 merge。
`experimental/btree_join_range.hpp` 提供同名 range 重载，一次规范化多个等高 inner root。

```cpp
#include <wjr/container/btree_map.hpp>
#include <wjr/container/experimental/btree_join_range.hpp>

using index_type = wjr::btree_map<unsigned long long, int>;
std::vector<index_type> partitions(8);
for (unsigned s = 0; s < partitions.size(); ++s)
    for (unsigned i = 0; i < 512; ++i)
        partitions[s].try_emplace(s * 1024 + i, i);
index_type result;
wjr::experimental::append_btree_disjoint(result, partitions.begin(), partitions.end());
// result 拥有4096条目；原8个对象保留且全部为空。
```

## 范围与所有权

沿用[单分片接口](btree-disjoint-append.md)的同 traits、标准 comparator、相等 allocator 和严格区间顺序。
非空 source 必须按输入顺序严格跟在 destination 和前一非空分片之后；各 source 对象互不相同，
不能包含 destination。结果元素数必须可由 tree 的 size_type 表示。

输入要求 forward iterator 和 sentinel，解引用必须是可转为同一 mutable basic_btree 引用的 lvalue。
这允许普通 owner vector、引用包装 vector 和 counted forward 范围，不接受 const 或 move iterator。
源对象及引用在调用期间保持有效。不是可重用单个临时 owner 的流式输入接口。

iterator 前端只计数并收集引用；较大的图算法只按 traits 实例化一次，避免为每种 iterator/sentinel
重复生成整个归并。空范围/全空分片不分配 workspace；仅一个非空分片走已有两树接口，
也不建立 workspace。输入 forward 迭代可能遍历两次。

如果 destination 非空，其 root 也参与规范化。所有参与者均为等高 inner root 时，
提取原 root 的 child descriptors，按原顺序均分成合法父节点，再逐层组树。
复用原 root 对象，只有不足时才通过 destination allocator 分配额外 parent。
最后 splice 叶链，发布结果，再释放多余 root。现有叶、值及下层节点不迁移。
Root 高度不一致、SSO 或叶根则按原顺序调用已有两树 append，保留其合法回退行为。

source 的 comparator/allocator 实例不传播；source 为空且可以再用。
legacy 值地址语义保留，compact 的通用接口仍遵循原修改失效合同，因为回退可能移动值。
value 内部资源不会被改写，所有仍被引用的资源必须继续覆盖结果生命周期。

## 准备阶段的异常闭环

所有用户 iterator 操作在修改图之前完成。先建立 source 引用、typed child/minimum descriptors、
root pointer pool，预留全部容量并构造 workspace 元素；原 tree 在这些步骤中保持完整。
前置 iterator、workspace 分配或 metadata construct 失败会传播异常，原 owner 全部不变。

新增 parent 也在 rewire 前分配。刚创建但尚未进入 pointer pool 的 parent 由单节点 guard 管理；
已经入 pool 的新增 parent 由另一个 guard 管理。自定义 allocator 在 pool pointer 构造时抛出，
包括第一个和第二个额外 parent 的失败点，都不会泄漏或误删借用的原 root。
这些情形有 allocator ledger、原对象地址和生命周期测试。

Node 分配/构造/销毁仍调用原有 noexcept helper，不将其变成可恢复 OOM 路径。
图重写阶段只赋值已存在的 typed metadata，不增长 vector、不构造 workspace 元素，也不调用输入 iterator。
非 root child 数满足原 `floor(N/2)+1` 下限；各层 child 高度一致。
工作数组的前缀在对应 child 范围读取完成后才覆盖为 parent，不会破坏尚未消费的 descriptors。

完整结果和空 source 发布后才释放废 root 或 workspace；allocator hooks 可观察完整的旧 owners
或完整结果与空 sources。回退仍是原逐分片操作，沿用普通 tree 的比较、node/value 不抛出域；
不增加标准容器的 recoverable value/allocation 保证。

## 成本

准备和组树按 source 数量、root child 数量和树高工作，不逐元素访问等高图的值。
动态 workspace 使用 destination 的同一 allocator rebind，包含引用数组、child descriptors 和 root pool，
自定义 construct/destroy hooks 正常执行。它不是零分配操作，也不保证最终 heap 比逐分片方式更小。

仅合并上层节点保留叶布局，避免值移动；已知代价是临时 workspace，以及部分大 mapped 场景少许额外
leaf 空闲容量。不同高度仍回退，不能从固定分片快路径推广到任意持续增量更新。
[性能与内存报告](../performance/btree-shard-range.md)列出成熟 first-swap/node-hint 对照、峰值及负例。
