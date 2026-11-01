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

## 借用 key 完成 map 更新

普通 map 和 compact map 的透明比较器还支持借用查询的 `try_emplace`、
`insert_or_assign`、`operator[]`，以及前两者的 hint 重载。例如从请求路径累计计数：

```cpp
wjr::btree_map<std::string, unsigned, std::less<>> requests;
std::string_view path = "/api/v1/warehouses/inventory/availability";
++requests[path];
requests.try_emplace(path, 0); // 已存在时不构造 string 或 mapped。
requests.insert_or_assign(path, 7);
```

查询先沿已有搜索或 hint 路径定位。仅当键不存在时，才将查询完美转发给 owned Key
的构造；树不会保存借用视图。已有键的 `insert_or_assign` 直接赋值，`operator[]`
不构造默认 mapped。`string_view` 保留显式长度（包括内嵌零字符），不会自动转成
C string；自定义透明比较器收到原查询类型，不进行语义归一化。

这些重载在 C++20 即可使用，接口参考
[P2363R5 的异构关联容器更新](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2363r5.html)。
它们要求透明标记、Key/Query 双向比较、可用的普通或受支持 allocator-extended Key
构造，并排除 iterator 查询以保留 hint 重载解析。所有原有 Key 重载保留；compact
仍只接受原先允许的 scalar Key 和非抛迁移 mapped 类型，不因此支持 string 存储。

**调用前提：查询转换成 Key 后必须保持相同的排序/等价关系。** 例如转换会截断数值，
或自定义 Key 构造会改变比较结果的查询，不满足该合同。比较器仍不得抛出；构造、
分配及插入继续服从已有引擎的终止域，不新增标准 map 的异常保证或稳定引用承诺。
正确 hint 避免 root search，错误 hint 仍回退，算法本身未改。

原本通过隐式转换调用 Key 重载的查询，其转换异常继续在进入 noexcept 插入引擎前
传播；缺键时先建立 owned Key，再在已定位的位置插入，不重复搜索。已有键则完全
不执行该转换。这也意味着隐式 C-string 转 PMR string 的缺键路径仍可先使用
默认 resource 建立临时 Key，最终存储通过目标 allocator 构造，不能把重复键的
零分配结论扩大到所有缺键路径。此前不能隐式调用的显式查询（如 string_view）直接
沿新重载的 allocator 构造路径，构造失败仍服从引擎的既有终止域。

针对一个超过 SSO 长度的 PMR path，已存在键的 300 次 C-string 更新在旧实现产生
300 次临时 Key 分配，新实现为零；目标 resource 两者均无新增分配。借用
`string_view` 更新此前不可调用，现在也无需临时 Key。这是特定路径的分配计数，
不表示所有工作负载更快：C string 比较可能反复计算长度，已知长度时优先传
`string_view`。未改变同 Key 入口的运行时策略或节点布局。
