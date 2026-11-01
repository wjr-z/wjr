# 追加有序且不重叠的 B+ tree 分片

索引分片已按键区间排序时，普通 `merge` 仍按元素执行查找、来源重平衡和目标插入。
`experimental/btree_join.hpp` 提供显式的 `append_btree_disjoint(destination, source)`，
在节点形态适合时直接拼接图；普通 merge 的代码和选择策略不变。

```cpp
#include <wjr/container/btree_map.hpp>
#include <wjr/container/experimental/btree_join.hpp>

wjr::btree_map<int, int> index, later_partition;
for (int i = 0; i < 4096; ++i) {
    index.try_emplace(i, i);
    later_partition.try_emplace(i + 8192, i);
}
wjr::experimental::append_btree_disjoint(index, later_partition);
// index 拥有全部条目，later_partition 为空且可以重新使用。
```

## 合同

- 两个参数必须具有相同的实际 traits，包括 Key/Value、comparator、allocator 和容量配置。
  接受 legacy map/set/multiset 与 compact map，但不混合存储策略或不同容量。
- 使用标准 `std::less<Key>`/`std::less<>`/`std::greater<Key>`/`std::greater<>`。
  自定义 comparator 不参与重载；不推定相同类型的自定义实例具有相同排序状态。
- allocator 必须比较相等，和普通 merge 一样不传播或替换实例；资源必须继续覆盖结果的生命周期。
- 当两树非空时，destination 的最后一个键必须按 comparator **严格先于** source 的第一个键。
  这保证整个区间不重叠。`greater` 下也按迭代顺序解释“后续分片”。debug assertion 检查边界，
  release 不执行检查；这不是通用范围验证接口。相接的 multiset 分片也不能共享边界等价键。
- source 成为空树并保持可用；空 source 和 self-append 是 no-op。后续普通 insert/erase/copy/move/swap
  使用原节点格式。legacy 值地址稳定性保留；compact 仍采用原有修改失效合同，不能因一次拼接未移动值
  就假定所有调用均保留 compact 引用。
- 比较、node/value 操作沿用普通 tree 的不抛出域。公开入口没有额外的 `noexcept`；节点分配/销毁
  继续调用原有 `noexcept` helper，没有可恢复 OOM 或新的强异常保证。hook 不可重入修改任一 tree。

该接口适合已经按区间分开的分片归并。任意交叠或未知区间继续使用普通 `merge`。

## 拼接协议

SSO、不同高度及不适合的叶根回退普通 merge。以下两种形态直接拼接：

1. 两个 heap leaf 的元素数都至少为 `floor(N/2)`，且总数超过 N：保留两个叶，分配一个父节点。
   总量不超过 N 或存在过小叶时不强行保留额外节点。
2. 等高 inner root：冻结至多 `2*(N+1)` 个 child 指针及对应 separator。
   若所有 child 能放入一个 root，就扩展左 root 并释放右 root；否则将 child 尽量均分回两 root，
   再分配一个共同 parent。新非 root 的 child 数至少为 `floor(N/2)+1`，满足原最小占用不变量。

所有 child 原本处于同一层，且本来就是合法 non-root；它们的子树不变。separator 是实际 child 的首键，
沿原来的 scalar-key 或借用 key-pointer 表示复制，不构造值对象或伪装 mutable pair。
分配新 parent 在修改任何图之前完成；重写只涉及不抛出的 typed metadata。
完整 root、size、叶链发布并清空 source 后才销毁多余 root。
因此 allocator 的分配/销毁 hook 可观察有效的旧两树或完整的新树与空 source，
不会观察到半拼接的公开 tree。针对测试验证了这个顺序。

graph 路径没有 value construction/destruction、move 或单值 allocation；最多分配/释放各一个 inner node。
它使用固定 typed 栈工作区，N16/u64 separator 时为 536B，N64 时为 2072B；实际函数栈帧由编译器决定。
它不是零空间操作，但不分配动态工作区。不改变 tree 大小、节点容量或正常路径。

等高 inner 图的工作量为 O(height + N)，不会逐元素访问。
不同高度仍是普通 merge 的复杂度和代价，因此大量大小不等分片的累加不能据此假定获得相同收益。
[跨平台计时、成熟 node-hint 对照和负例](../performance/btree-disjoint-append.md)给出适用边界。

多个等高分片可使用独立 `btree_join_range.hpp` 的[owner-range 重载](btree-shard-range.md)，
一次规范化所有 root，避免逐次累加升高后的回退；两树接口无需引入其 workspace 或模板定义。
