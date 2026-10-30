# B-tree transparent lookup

`btree_map` 和 `btree_set` 可使用透明比较器，借用 `std::string_view`、C string
或其他查询类型检索已有键，无需先构造临时 `Key`。默认比较器仍是
`std::less<Key>`；使用 `std::less<>` 或声明 `is_transparent` 的自定义比较器启用异构查询。

## 从请求路径查询 string 键

```cpp
#include <string>
#include <string_view>
#include <utility>
#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>

wjr::btree_map<std::string, int, std::less<>> handlers;
handlers.emplace("/api/v1/warehouses/inventory/availability", 7);

std::string_view request = "/api/v1/warehouses/inventory/availability?limit=20";
const auto path = request.substr(0, request.find('?'));
const int handler = std::as_const(handlers).at(path);
auto position = handlers.lower_bound(path);

wjr::btree_set<std::string, std::less<>> enabled;
enabled.emplace("/api/v1/warehouses/inventory/availability");
const bool available = enabled.count(path) != 0;
```

树拥有插入的 string；查询仅在调用期间借用 `path`，不会保存其引用。
该路径同样适合字符串较长、无法依靠短字符串优化避免临时 Key 分配的场景。

## API 和比较合同

| 容器 | 支持透明查询的现有操作 |
| --- | --- |
| map、set、multiset | const/mutable `lower_bound`、`upper_bound` |
| map、set | `count` |
| map | const/mutable `at` |

所有原有 `const Key&` 重载保留。异构重载要求比较器的 `is_transparent` 表示类型，
并且 `const Compare&` 可将 `const Key&` 与 `const Query&` 双向比较为布尔谓词。
查询类型不必能转换成 Key；不透明或只支持单向比较的类型不会获得这些新重载。

与原有 B-tree 查询一样，比较器必须为存储键和查询值建立一致的排序/等价关系，
且这些比较调用不得抛出异常。`is_transparent` 和可调用性检查不证明 noexcept。
`lower_bound`、`upper_bound`、`count` 保留 noexcept；比较器抛出异常会终止进程。
`at` 对不存在的等价键继续抛出 `std::out_of_range`，关闭异常时沿用现有失败策略。

成功查询不会构造 Key 或分配树存储。自定义比较器自身的工作与分配由调用者控制。
查询不修改树；返回的 iterator/reference 仍服从已有 B-tree 失效规则，
不能因接口类似 `std::map` 就假定跨插入/删除保持稳定。

## 实现边界

查询值类型仅传播到已有搜索和等价性检查模板；节点形状、Key/value 的 inline/indirect
表示、线性/混合/二分搜索选择以及插入的 Key 路径保持不变。
新重载没有类型擦除、运行期类型分派或保存查询状态。
