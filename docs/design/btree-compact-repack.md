# Compact 索引完成批量修改后的空间回收

批量接管 shard 的图结构能避免逐值移动，但会保留原叶子的空闲 slot。
`experimental/btree_repack.hpp` 提供显式的 `repack_compact_btree(tree)`：
在修改阶段结束后，将现有 compact map 的值移入最少数量的普通叶，再建立普通 inner nodes。
这是读阶段前主动用 CPU 换保留空间的操作；不会改变默认插入、merge、容量或节点布局。

```cpp
#include <wjr/container/experimental/btree_join_range.hpp>
#include <wjr/container/experimental/btree_repack.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>
#include <vector>

using tree = wjr::experimental::compact_btree_map<unsigned long long, int>;
std::vector<tree> shards(8);
for (unsigned s = 0; s < shards.size(); ++s)
    for (unsigned i = 0; i < 512; ++i)
        shards[s].try_emplace(s * 1024 + i, i);
tree index;
wjr::experimental::append_btree_disjoint(index, shards.begin(), shards.end());
bool rebuilt = wjr::experimental::repack_compact_btree(index);
```

## 合同与失效

仅现有 compact storage policy 可调用：真实 `pair<const Key, Value>`、inline scalar key、
不抛出的 pair move 和 mapped destruction。legacy 的间接值地址稳定优势不被改变，
也不提供绕过其地址合同的 repack 重载。节点容量、默认树对象大小和已有配置均不变。

成功重建保留 allocator 和 comparator **实例**，不复制临时树，不调用 comparator，
每个值通过原 allocator-aware typed construction 建立一个新对象，再销毁旧对象。
不使用 byte relocation，不跳过自定义 construct/destroy hooks，不把 const-key pair 伪装成 mutable pair。
Mapped 的 allocator-aware 构造仍使用同一资源；所有被引用的 PMR 资源必须覆盖结果的生命周期。

返回 true 表示重建，所有元素引用、指针和迭代器失效。SSO、单叶根和已达到最少叶数时返回 false，
不分配、不搬移值，原引用和迭代器保持有效。判断最少叶数会遍历叶链；false 不承诺常量时间。
它不是普通修改路径中的自动压缩，也不承诺收回 allocator 自己 retained 的内存或降低 RSS。

## 所有权与异常边界

先用原 allocator 的 rebind 完整构造 `ceil(size/N)` 个 node pointers 的 workspace。
分配或 metadata construct 抛出时，vector 清理已完成元素，原树和值完全不变。
不额外分配一个完整的新树再清空旧树。

准备完成后，public root、size 和叶链先成为合法空树；旧图只由局部指针和递归遍历持有。
按原顺序构造新值，构造完成后才销毁对应旧值，一整片旧叶消费完才释放它。
旧 inner 在其全部 children 消费后释放。已经释放的旧内存可以被同一 allocator 重用；
不再读取旧叶链或已消费的 child。每个新叶均匀填充，非根 occupancy 保持现有下限。

完成全部值后，以预构造指针数组的前缀逐层组 parent；覆盖前已消费对应 child 范围。
根 parent 为 null，子节点 parent/pos 与 separator 来自新图。最后一次发布完整树和叶链。
只读 allocator/value hooks 观察到完整原树、合法空树或完整结果，不会观察半棵 public tree；
不允许 hook 重入修改同一容器。

Detach 后调用的是现有 `_create_*`、`_construct_leaf_value`、`_destroy_slot` 和 `_drop_*`
的 noexcept 边界；compact 值迁移域也保持不变，没有把原可恢复调用加进新的 noexcept 域。
Node/value allocator hook 或 OOM 在这些既有边界中仍会 terminate，不提供失败后恢复旧树的承诺。
这与 workspace 准备阶段可传播异常的合同不同；需要 recoverable node OOM 的应用不能据此获得该能力。

工作量为 O(values + nodes)，额外 workspace 为 O(最少叶数) 个指针，加递归树高的 stack。
每个值恰好新构造/旧销毁一次，node 分配和移动会增加 CPU 成本；峰值包含 workspace 和暂存的新叶，
不能把降低最终保留空间宣传成免费操作。完整对照见[测量报告](../performance/btree-compact-repack.md)。

## 验证

`BtreeRepack` 覆盖 SSO、叶根、N4/5/16/64、叶数边界、4096 个 align64 move-only self-referencing mapped、
精确 typed allocation/deallocation、构造/销毁次数、后续 erase/clear、第二次调用无分配、
stateful reverse comparator、PMR string 同 resource，以及 workspace allocation/construct 失败。
旧 join/range tests 一起运行，保证新增 helper 不改变既有图接管路径。
