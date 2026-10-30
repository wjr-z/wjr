# B-tree 叶节点字节预算（experimental）

`<wjr/container/experimental/byte_btree.hpp>` 提供 `wjr::experimental` 下的
`leaf_bytes<B>`、`byte_btree_layout`、`byte_btree_map` 和 `byte_btree_set`。
预算包含叶头、槽位和末尾对齐填充，以 allocator 实际请求的叶存储大小为准。
它不包括 allocator 自身的元数据、间接 value 分配或进程 RSS。

```cpp
#include <cstdint>
#include <type_traits>
#include <wjr/container/experimental/byte_btree.hpp>

using namespace wjr::experimental;
using budget = leaf_bytes<256>;
using info = byte_btree_layout<std::uint64_t, std::uint64_t, budget>;
using map_type = byte_btree_map<std::uint64_t, std::uint64_t, budget>;
using set_type = byte_btree_set<std::uint64_t, budget>;

// Ordinary x64: 32-byte leaf header + 28 eight-byte slots = 256 bytes.
static_assert(info::leaf_slots == 28);
static_assert(info::leaf_allocation_bytes == 256);
static_assert(info::budget_satisfied);
static_assert(std::is_same_v<map_type,
    wjr::btree_map<std::uint64_t, std::uint64_t, std::less<std::uint64_t>,
                   std::allocator<char>, info::leaf_slots>>);

int main() {
    map_type counts;
    counts.try_emplace(42, 7);
    set_type enabled;
    enabled.emplace(42);
    counts.erase(counts.lower_bound(42));
    return enabled.count(42) == 1 ? 0 : 1;
}
```

参数顺序是 `byte_btree_map<Key, Value, Budget, Compare, Alloc>` 和
`byte_btree_set<Key, Budget, Compare, Alloc>`；默认预算是 `leaf_bytes<256>`。
比较器、allocator 和全部操作直接使用既有容器，别名没有额外继承层、字段或运行时选择器。
需要固定容量时继续使用 `wjr::btree_map/set<..., N>`；原模板及 `N == 0` 的规则不变。
可以独立使用 `byte_btree_layout<Key, Value, Budget, Alloc>`，set 将 Value 设为 `void`。
Key 和非 void Value 必须是完整、没有 cv 修饰的非数组对象类型，Budget 必须是 `leaf_bytes<B>`。

选择器枚举 `[4, 64]` 并选出满足预算的最大 N。对齐会让多个容量对应相同分配大小，
因此预算未必恰好除尽槽位大小。例如普通 x64 的 byte set 在 B=48 时选 N=16；
B=47 时连 N=4 都放不下，仍选 N=4，并通过 `budget_satisfied == false` 明确报告。
大预算最多选择 N=64；B=0 也合法，采用相同的最小容量回退。布局取决于目标 ABI 和 allocator
提供的 size_type，跨平台代码应读取 descriptor，不应把示例数值当成 ABI 保证。

`leaf_slots`、`leaf_allocation_bytes`、`inner_allocation_bytes` 和
`budget_satisfied` 都是 constexpr。叶大小直接取自引擎实际分配的共享 storage 类型，
内节点大小直接使用引擎的 inner node 类型。内节点沿用同一 N，包含 separator 和 N+1
个子指针，**可以超过 B**；这不是整树统一节点字节上限，也没有拆分 inner/leaf 容量。

前两个元素使用对象内的槽位，第三个元素开始分配叶。小且 trivial 的 value 可内联；
现有 map pair 通常走指针槽位，每个 value 仍分别分配。叶预算不会消除这些分配，
也不改变 SSO、槽位移动、搜索或 split/merge 算法。

这些别名继承既有迭代器和异常合同：插入、删除可能使 iterator/reference 失效；
声明 noexcept 的操作中比较器、allocator 或 value 抛异常将终止进程。
接口类似标准有序容器，不代表具有 std::map/set 的引用稳定性、完整 API 或异常保证。
本功能只提供可解释的编译期配置，未声称 N28 比默认 N16 更快，也未修改默认容量。
