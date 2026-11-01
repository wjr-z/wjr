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
| map、set、multiset | const/mutable `find`、`lower_bound`、`upper_bound`、`equal_range`，以及 `contains`、`count` |
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
不表示所有工作负载更快。已知长度时优先传 `string_view`；下面的窄缓存避免标准
string/less 的 C string 在搜索比较中反复计算长度。同 Key 入口策略及节点布局不变。

## Unique set 的借用插入与 interning

```cpp
wjr::btree_set<std::string, std::less<>> interned;
std::string_view token = "/api/v1/warehouses/inventory/availability";
auto [position, inserted] = interned.insert(token);
auto same = interned.insert(interned.end(), token);
```

`btree_set` 的普通/hint `insert` 先借用 Query 查找，仅在缺键时建立 owned Key。
已有键返回原 iterator，不构造、移动或分配临时 Key。正确 hint 使用已有相邻 gap，
错误 hint 回退已有搜索；没有保存借用对象。负约束及 query→Key 等价前提与 map 更新
相同。已有 Key 重载、显式 member template/函数取址与 range/list insert 保留。
multiset 每次本来都插入，不增加没有省构造收益的对应入口。

长 PMR path 的 200 次重复普通/hint 插入，临时 Key 分配从 200 降到零，owner resource
均无新增分配。Allocator-aware-only、不可移动且 align64 的 Key 也可直接从 Query
经 owner 构造，无需先生成一个可移动临时 Key。原隐式转换的异常仍在 noexcept 插入
引擎外传播；显式新查询及普通值构造遵循已有引擎合同。

## 标准 string 的一次长度扫描

仅当比较器**恰为 `std::less<>`**、Key 为
`std::basic_string<char,std::char_traits<char>,Alloc>`、查询为普通非 volatile
char pointer/array 时，共享 lookup 边界将查询借用为 `string_view` 并缓存长度。
缓存覆盖 find/bounds/count/at/equal-range/erase 和 map/set 的透明更新/hint。
空树不比较，lookup 不预扫描。数组按第一个 NUL 确定 C string 长度，绝不假设 N−1；
显式 string_view 仍保留完整长度及内嵌零。

Key/value 构造始终转发**原 Query**：不改变 allocator hook 收到的参数类型或 #279
的异常边界。缺键的 owned Key 构造本身仍可扫描一次，此处承诺是 lookup 不重复扫描。
自定义比较器、traits、其他 Query 类型及 volatile pointer 不规范化，保留原调用和
可见副作用；`std::greater<>` 等未纳入这个窄策略。

Clang18 baseline 实际汇编中，旧 C string `_search_impl` 循环及 comparator lambda
含 strlen；新比较循环零 strlen，成功 borrowed lookup/重复插入只在入口扫描一次，
新 Key 构造的扫描位于独立 miss 分支。已有 Key 和 string_view 入口、普通 insert、
bounds 及 layout 的 baseline/native 汇编保持一致。未据此宣称通用吞吐提升。
