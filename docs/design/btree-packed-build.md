# 从有序输入直接构建 B+ tree

已有排好序、去重的索引条目时，逐项 hint 插入仍会分裂节点、搬移已有条目，并留下约半满的叶节点。`experimental/btree_bulk.hpp` 提供显式的 packed 构建入口，在已知元素数量后直接建立均衡的叶节点和父节点。

```cpp
#include <wjr/container/experimental/btree_bulk.hpp>

std::vector<std::pair<int, std::string>> sorted_rows = {
    {1, "one"}, {3, "three"}, {8, "eight"}};
using index_type = wjr::btree_map<int, std::string>;
auto index = wjr::experimental::make_btree_from_sorted_unique<index_type>(
    sorted_rows.begin(), sorted_rows.end());
index.try_emplace(5, "five"); // 构建完成后仍是普通的可修改 tree。
```

这是供已有排序结果、索引快照和外部归并输出使用的入口。它不会排序、去重，也不会改变普通构造、`insert` 或 `insert_with_hint` 的路径。持续流式接收、未知长度或允许重复的输入，继续使用原来的导入接口。

## 输入与失败合同

- 输入转换为 tree 的实际 `value_type` 后，键必须按传入 comparator **严格有序且唯一**。转换前不同的键不能转换成等价键。debug assertion 检查相邻已存储的键；release 不执行验证。
- 支持现有 range 插入所接受的元素转换。迭代器必须是 forward iterator、具有 sized sentinel 的单遍迭代器，或包装 forward iterator 的 `std::move_iterator`。无长度的单遍输入不参与重载；可用 `std::counted_iterator` 显式提供准确长度。
- 长度必须非负、可由 tree 的 `size_type` 表示，范围和元素在构建期间保持有效。forward 输入先遍历一次求长度，再读取一次；sized 输入只读取一次。
- 可以使用 move iterator 消费 move-only mapped 值。失败时已经被移动的输入不会恢复。普通 copy 输入不会被修改。
- 返回的类型是传入的普通 tree 类型；`Tree(comp, alloc)` 必须构造空树。支持 legacy map/set/multiset 和现有 compact map；multiset 也仍要求本次输入唯一。
- 入口本身允许输入迭代器和辅助工作区的异常传播。部分叶的已完成对象、已完成的子树及工作区均由局部 guard 拥有，发布完整 root 之前，目标 tree 保持逻辑空状态。
- 原 tree 的节点分配、值构造/销毁及 comparator 不抛出合同继续适用；这些既有 `noexcept` helper 不会被改成可恢复分配路径。自定义 allocator 的 value hooks 与 typed allocation/deallocation 继续使用原协议。工作区的标准 vector 分配、初始化采用同一 allocator 的 rebind，允许正常传播异常。

例子中 `index` 自己拥有复制的条目；`sorted_rows` 的后续销毁不影响它。allocator-aware mapped 类型通过原构造协议获得指定资源：

```cpp
std::pmr::unsynchronized_pool_resource resource;
using allocator = std::pmr::polymorphic_allocator<char>;
using pmr_index = wjr::btree_map<int, std::pmr::string, std::less<>, allocator>;
auto index = wjr::experimental::make_btree_from_sorted_unique<pmr_index>(
    sorted_rows.begin(), sorted_rows.end(), std::less<>{}, allocator(&resource));
// resource 的生命周期必须覆盖 index 及其 mapped 值。
```

## 存储与修改边界

0–2 个元素使用原有 SSO，3–N 个元素直接填充一个普通 root leaf。这些情况不分配辅助工作区。更大的输入建立 `ceil(size/N)` 个叶节点，元素数量尽量均分；每层父节点也按最多 N+1 个子节点均分。非 root 节点保持现有最小占用不变量，NodeSize、节点布局和 tree 的 `sizeof` 都不改变。

工作区是以相同 allocator rebind 分配的节点指针数组。当前层和下一层数组在父节点连接期间同时存在；它们的分配次数、总字节和峰值已计入[性能报告](../performance/btree-packed-build.md)。它不是零额外分配的 streaming builder。

legacy map 的每个 value 仍单独分配；构建后保留其原有间接存储和地址语义。compact 仍构造真实 `pair<const Key, Value>`，保持原来的迁移与失效合同。后续 insert、erase、copy、move、swap 复用现有实现，没有另一套节点格式。

packed 叶节点的空闲容量较少，紧接着插入新键可能更早触发 split。构建性能和初始占用不能推出后续混合修改、lookup 或通用吞吐更好。需要接收大量新数据时，可以继续选择 hint 导入的原布局。

## 验证

针对测试覆盖 SSO、首个叶分裂边界、多层 root、奇偶 NodeSize、后续随机删除/插入、范围删除、copy/move/swap、间接字符串 separator、stateful comparator、move-only mapped、sized 单遍输入、PMR、64B 对齐、allocator hooks，以及输入/逐层工作区失败的释放。跨平台计时、内存口径及本机 sanitizer 限制见性能报告。
