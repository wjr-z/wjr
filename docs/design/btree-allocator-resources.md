# B-tree allocator instances and PMR

`btree_map`、`btree_set` 和 `btree_multiset` 支持 `(comparator, allocator)` 构造。
`get_allocator()` 返回当前 allocator 的副本，不能用返回值替换已有树的 allocator。
原有 default/comp 构造继续直接构造状态，不经临时 allocator 委托。

```cpp
#include <memory_resource>
#include <string>
#include <wjr/container/btree_map.hpp>

std::pmr::unsynchronized_pool_resource storage;
using allocator = std::pmr::polymorphic_allocator<char>;
wjr::btree_map<std::string, int, std::less<>, allocator> routes(
    std::less<>{}, allocator{&storage});
routes.emplace("/orders", 7);
const int handler = routes.at(std::string("/orders"));
```

资源先于树构造、后于树销毁。allocator 借用资源；资源及其底层缓冲必须覆盖树的
使用期，在树仍持有分配对象时不能调用资源的 `release()`。
相同写法可传入 `std::pmr::monotonic_buffer_resource`，由调用者决定缓冲和回收策略。

节点和间接 value 通过同一 allocator 状态的 rebind 分配，并按实际类型对齐及释放。
普通 `std::string` 自有缓冲仍使用自己的 allocator；需要字符串缓冲也使用该资源时，
可将 Key/mapped 类型改为 `std::pmr::string`，现有 uses-allocator 构造会传递资源。

此接口不添加 copy/move-with-allocator 重载，不改变现有分配/元素构造失败策略。
它也不在树中添加资源所有权或额外字段。当前 x64 Windows 实测默认空 allocator 树为
48B，polymorphic allocator 树为既有的 64B；后者包含资源指针及对齐成本。
