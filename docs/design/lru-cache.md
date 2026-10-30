<!-- markdownlint-disable MD043 -->
<!-- Design sections follow the cache contract rather than a fixed template. -->

# LRU cache：容量、生命周期与成本设计

状态：设计，2026-10-30；本次只交付文档，尚无 LRU 实现或性能结果。
后续任务与验收见 [实施计划](lru-cache-implementation.md)。

## 首期用途与边界

面向单线程中重复查询昂贵结果的场景：调用者持有 cache，cache 独占 Key/T，
命中更新最近使用顺序，满容量的新键插入淘汰最久未使用的一个条目。
容量按条目数计，构造后固定；它不限制 Key/T 的深层分配或进程 RSS。
例如容量 2，插入 A、B，读取 A，再插入 C，应保留 A、C 并销毁 B。

首期不包含 TTL、锁、分片、异步加载、淘汰回调、字节预算、动态改容量、
持久化、共享所有权和策略插件。也不提供会隐式构造 T 的 `operator[]`。
外部同步由调用者负责；即使是命中读取也可能写 recency 链。

WJR 已有 [intrusive list](intrusive-lists.md) 和
[allocator/PMR 用法](btree-allocator-resources.md)，没有现成的 hash/LRU 容器。
复用 `include/wjr/container/list.hpp` 的 typed/tagged hook 与 noexcept 链操作，
不为 LRU 重建通用链表或 hash table。通用容器保持 C++20 和架构基线，
本设计不需要 ISA 分派、SIMD 或额外 native 编译参数。

## 候选与复用决定

先保留标准库组合候选，与满足相同契约的成熟实现比较，再决定发布形式。
以下上游版本是审查参考，不是依赖准入，也不是最新版本声明。

- `std::list<pair<const Key, T>>` 加 `std::unordered_map<Key, iterator>`：
  易实现、可逐步验证的参考；每项两份 Key、两个容器节点及 bucket 成本。
  大 Key 的复制与深层分配必须计入，不能把这个方便的参考当性能上限。
- [Boost.MultiIndex 1.86.0][boost] 的 hashed_unique + sequenced 索引：
  同一元素具备查找与顺序索引，避免双份 Key，作为强对照候选。
  接入前核对 Boost Software License、头文件传递范围、编译成本和支持矩阵。
- [Folly v2024.08.19.00 EvictingCacheMap][folly]：
  Apache-2.0；采用 F14 指针索引与 Boost intrusive list，提供现成 LRU 行为。
  该版本非线程安全，容量 0 表示关闭自动淘汰，与本文语义不同。
  先作为外部对照评估，不默认引入 Folly 的依赖与构建负担。
- `std::unordered_map<Key, Entry>` 加 WJR intrusive hook：
  不增第三方依赖、Key 只存一份，是首个待验证候选；尚不能断言更快或更省。

若成熟实现满足契约且总维护成本更低，优先薄适配；只公开所需子集。
只有测出实际收益或明确的依赖/接口优势才保留自制组合。
更换索引不能只看 lookup 吞吐：[Abseil 容器指南][abseil] 明确区分
flat 表 rehash 后地址失效与 node 表的地址稳定性，二者不宜直接互换。

## 节点与所有权

标准库候选的真实存储是 unordered_map 的 `pair<const Key, Entry>`。
Entry 派生自 `intrusive::list_node<Entry, lru_tag>`，包含 T 和一个 `const Key*`。
成功构造 map 元素后，把回指设置为 `addressof(pair.first)`，再接入 recency 链。
回指用于从 LRU 尾节点查找其 map 元素；不重复拥有 Key。
禁止从 `pair.second` 通过 `offsetof` 或 container_of 猜测 `pair.first` 地址。

[标准 unordered 要求][unordered] 保证 rehash 不使元素指针/引用失效，
但会使 iterator 失效。因此 hook、T 地址和 Key 回指可保持；
不能长期缓存 map iterator，也不能把 flat hash 表的槽地址当稳定节点地址。
MRU 在链首，LRU 在链尾；sentinel 只作链头，不能解引用为 Entry。

在 8 字节指针的常见 x64 ABI 下，LRU 自身元数据是 prev/next/keyptr 共 24B。
这不是总节点大小：还要计入 Key/T、hash 节点链接、可能的 hash 缓存、
对齐与 padding、bucket 数组、容器对象、allocator 状态以及分配器头部/碎片。
典型 node STL 可能每个新元素分配一次，但标准不保证具体分配次数；
bucket 与 Key/T 的深层分配单独计数，失败清理和短暂额外节点也计入峰值。

allocator 通过标准容器的 rebind/construct/destroy 路径管理真实节点，
不使用 placement new 绕过自定义 allocator 钩子。PMR resource 是借用，
需活到 cache 销毁后；T 内部资源传播不能仅凭外层使用 PMR 就假定完成。
不额外重复保存标准容器已保存的 allocator/Hash/Eq 状态。

Entry 地址参与环和 Key 回指，不可盲目标记 trivially relocatable 或默认移动。
cache 自身含 sentinel，默认 move 也会留下指向旧 sentinel 的链接。
首期禁用 copy/move；未来确有需求时再设计 allocator-aware 转移及头尾修复，
用测试证明对象生命周期、地址和异常保证后才声明相应 trait/noexcept。

## 拟议接口与可观察语义

名称暂定 `lru_cache<Key, T, Hash, Eq, Alloc>`，不是当前可用 API。
不公开底层 hash 表、load factor 修改或可改 Key 的迭代器。

| 操作 | 结果与顺序 |
| --- | --- |
| `capacity()` / `size()` / `empty()` | 无分配、无 promotion |
| `get(key)` | 命中返回 `T*` 并升至 MRU；未命中返回 null |
| `peek(key) const` | 返回 `const T*` 或 null；不改变顺序 |
| `try_emplace(key, args...)` | 返回 `{T*, inserted}`；重复键不构造 T，仍 promotion |
| `insert_or_assign(key, value)` | 返回 `{T*, inserted}`；成功后为 MRU |
| `erase(key)` | 返回是否删除；不改变其余条目的相对顺序 |
| `clear()` | 销毁所有条目；容量不变，不承诺归还 bucket 内存 |

不再添加与 `get` 重复的 `find`。容量 0 时插入返回 `{nullptr, false}`，
不存储条目、不构造 Key/T；调用方传参表达式仍按 C++ 规则先求值。
查找始终未命中。构造阶段不为它预留 C+1 个节点的 bucket。

借用的 T 指针在该条目 erase、淘汰、clear 或 cache 销毁后失效。
其他条目插入、promotion 不改变其地址；修改 T 不影响 Key 或索引。
不得让同一 cache 的借用键/值参数在其生命周期结束后继续被操作读取；
具体 alias 用例纳入测试，不靠未声明的 shared_ptr 延长生命。
Hash/Eq、allocator、构造/析构及赋值期间不得重入修改同一 cache。

首期要求 Hash 对 Key 的调用不抛出，Key/T 析构和释放按标准约定不抛出。
Hash/Eq 必须满足等价关系、相等键同 hash、存储期间结果稳定等容器要求；
`noexcept` 不意味着纯函数，也不保证用户副作用可回滚。
Eq、Key/T 构造和分配允许抛出。约束以 C++20 concept/标准 traits 表达，
可命名 `nothrow_hash_for`；T 的构造重载用 `std::constructible_from` 等约束。
若提供异构查找，另用 `transparent_cache_lookup` 检查透明 Hash/Eq 及双向等价性，
不把 C++26 的异构插入设施当 C++20 已有功能。

## 满容量插入的事务边界

对容量 C > 0，在构造时检查 C+1 溢出及底层 max_size 限制，
并 `reserve(C + 1)`；之后不改变 max_load_factor、bucket 或容量。
这允许插入暂时到 C+1 而不 rehash，确保本次已取得的 victim iterator 有效。
对外操作完成时始终 `size() <= C`；live 对象峰值允许 C+1。

缺失键插入按以下顺序执行，不能先淘汰再尝试构造新值：

1. find 新键。若存在，按重复键规则处理，不选 victim。
2. 满容量时，用尾节点 Key 回指 find 出 victim iterator。
   这里 Eq 抛出时尚未修改 cache。
3. `try_emplace` 构造新 map 元素。分配、Eq 或构造失败时，旧集合和顺序不变。
4. 成功后设置 Key 回指、接入 MRU；这些内部操作必须 noexcept。
5. 满容量时 unlink victim，再用保存的 iterator erase；提交后回到 C 项。

[unordered 异常规则][unordered-except] 对单元素插入的无效果保证
明确排除了 Hash 抛出。首期限制不抛 Hash 正是此事务证明的前提；
不能把标准未承诺的行为当强保证，也不通过 catch 后重新哈希猜测恢复状态。
该证明只覆盖 cache 的成员、顺序与已存值，不能撤销调用方实参或用户回调副作用。

命中 `get` 先查找再做 noexcept promotion；Eq 失败不改变顺序。
已有键 `insert_or_assign` 先赋值，成功后才 promotion；赋值抛出时顺序不变，
T 的内容只获得该赋值操作自身的保证，不能泛称强保证。
`erase(key)` 先完成可能抛出的查找，再 unlink/erase；`clear` 先解除链再销毁节点。
不把所有操作统一标为 noexcept，也不在正常路径重复做完整环校验。

## 性能结论与复议条件

查找/插入的平均复杂度随合格 hash 期望为 O(1)，最坏查找仍可能 O(C)。
promotion 只改链且不分配，已是 MRU 时可避免摘接；这是机制推论，不是实测收益。
full miss 额外查询 victim、链写入和 reserve 的 bucket 空间都是真实成本。
若实测被这些成本主导，再比较指针索引/池化方案，不先构建通用 hash 平台。

首期发布以 [验证矩阵](lru-cache-implementation.md#同契约性能验证) 为准：
与最强实用候选在同环境下比较吞吐、尾部样本、对象布局、分配次数/字节和峰值。
若没有可解释的优势，就记录持平/落后并采用更简单的实现或暂不发布。
本轮没有 benchmark、汇编分析或收益数据，文档不能作为性能宣传依据。

[unordered]: https://eel.is/c++draft/unord.req
[unordered-except]: https://eel.is/c++draft/unord.req.except
[boost]:
  https://www.boost.org/doc/libs/1_86_0/libs/multi_index/doc/tutorial/indices.html
[folly]:
  https://raw.githubusercontent.com/facebook/folly/v2024.08.19.00/folly/container/EvictingCacheMap.h
[abseil]: https://abseil.io/docs/cpp/guides/container
