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
当前基线是一条全局 MRU→LRU 链；下文另评估冷热分段和 dense 指针索引。

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
更换索引不能只看 lookup 吞吐：[Abseil 容器指南][abseil] 区分 flat 表
rehash 后地址失效与 node 表的地址稳定性；允许失效的接口可以采用前者。

## 冷热链与内存池是不同决策

一个 hash 索引即可服务所有条目。若“冷”只是全局 recency 链的较旧后缀，
一条 ring 加 boundary 就能表示；也可以两个 head 分别串 hot/cold，
每个条目在任一时刻只属一条链，仍只需一个 hook。链尾定位和摘链为 O(1)，
但完整淘汰还包含 hash 索引删除及 Key/T 析构，不承诺总耗时最坏 O(1)。

若冷热有独立访问规则，就已经改变精确 LRU 策略。一个待评估的 segmented LRU
方案是新项进入 cold 首部，cold hit 升入 hot 首部，hot hit 移到 hot 首部，
hot 超预算时其尾项降至 cold 首部，缺失插入从 cold 尾淘汰。
采用前必须固定 hot 预算、容量 0/1、cold 空时的规则，以及重复插入和 peek
是否 promotion；这些规则尚未选定，不因使用两个 head 自动变成 2Q。
顺序扫描抗污染、命中率和链写入成本都需要同一访问 trace 验证。

“冷热池”在这里表示淘汰顺序，不代表分配池。稳定 arena/free-list 是另一个
内存决策；对象搬移需执行 hook 重接及已声明的借用失效合同，不能静默搬移或回收。
本轮保留精确 LRU 为可复现基线，冷热方案先完成策略合同与 trace 比较再决定。

## Dense/flat 索引的首批研究候选

三条路线同等进入选型，不为了保留基线的稳定指针保证排除 inline 优化：

- dense set 内联 `Entry{Key, T, hook}`，自定义 move 重接已有链；
  Key 由 Entry 拥有，存储期间逻辑只读，搬移时随 Entry 转移。
- 独立稳定的 `Node{const Key, T, hook}`，
  加 `set<Node*, HashByKey, EqByKey>`；透明 Hash/Eq 按 Node 内的 Key 查找。
  Key 仅存一次，避免 `map<Key, Node*>` 的重复 Key 和深层字符串分配。
  cache 独占 Node，索引借用指针；数组搬移指针不搬移 Node。
  待插入 Node 先由 RAII guard 持有；索引 erase 完成前 Node 必须存活，
  clear 时先移除索引再销毁 Node。
- node_map 内联 hook，利用后端的稳定节点；不单独分配第二个拥有者。

| 固定参考 | 安全布局与待验证边界 |
| --- | --- |
| [ankerl v4.5.0][dense]，MIT | inline 可移动 Entry 与稳定 Node 指针 set |
| [Boost 1.86.0][boost-node]，BSL-1.0 | node_map 嵌 hook；flat_set 索引 Node 指针 |
| [Abseil 20240722.0][absl-node]，Apache-2.0 | node 地址稳定；flat 使用指针索引；异常合同需另行证明 |

ankerl 的 [固定版本实现][dense-source] 在 erase 时以末尾元素补被删位置，
并修正其 bucket 索引；预先 reserve 只能避免增长搬移，不能避免这种 erase 搬移。
segmented_map/set 仅保证插入稳定，erase 仍须处理重接。Boost flat 同样需处理
rehash 搬移；含 hook 的 Entry 可以移动，但不能依赖默认 memberwise move。

WJR 已有 `replace(from, to)`、`remove()`、`init_self()`，复用它们，
不重建 intrusive 框架。inline move 构造先完成 Key/T 转移，再重接 source→destination，
erase 补洞的 move assignment 先处理 destination 原有节点（淘汰路径先 unlink），
再转移值并接管 source 的链位置。moved-from 后置条件按后端真实序列选择：
若只会销毁 source 且 Entry 析构不自动 unlink，可以保留不可复用的 hook metadata，
省去复位写入；若 source 会赋值/复用或析构自动 unlink，则用 init_self 等建立空链状态。
未链接 source、自移动、相邻节点、唯一节点、链端点必须单独定义；
`replace` 不负责清理 source，也不能对自环/自移动无条件调用。
原语使用 trusted 前提，不把 `swap_with` 当成所有边界都成立的通用移动算法。

优先用 dense set 的 Entry 自有 Key，便于 move 内合法重绑全部内部状态；
若选择 `map<Key, Entry>` 加 Key 回指，Entry move 本身不知道新 pair.first 地址，
必须另有后端支持的合法重绑机制，不能从 pair.second 做 container_of。
移动成功不代表引用稳定：inline 方案允许增长/erase 补洞使外部 T 指针失效，
这项合同应写入接口，而不是隐含在具体 STL 行为中。

异常保证必须按后端重新证明，不继承 `std::unordered_map` 的标准保证。
ankerl 此版本透明 set 的单参数 emplace 先 Hash/Eq，再追加元素；
泛用多参数 emplace 的顺序不同。研究时固定选中重载、预留 C+1 的 value/bucket
空间、不可抛 Hash，验证每个失败点与 erase 的 hash 调用。
inline 优化域可要求 Key/T move 构造、move 赋值、析构和相关 allocator 钩子不抛出；
还须处理 Eq、初始构造及分配失败，不能只检查 `is_nothrow_move_constructible`。
若部分重接后再发生异常，容器回滚/清理可能走不同移动路径，必须逐条证明链和索引一致；
未证明前限制可接受类型/操作域，不沿用标准表的强保证宣传。
指针方案也不能因 payload 在外部就假定索引必不抛出；两种方案均不长期保存 slot iterator。

Boost node_map 文档为 try_emplace 承诺排除 Hash 异常后的 no-effect 保证，
erase(iterator) 不抛出，适合优先验证是否满足当前事务合同。
其 load 上限由实现管理，不能照搬标准表 max_load_factor 的控制方式。
Abseil 该版本 [flat 头文件][absl-flat] 与 node 头文件均明示不具备异常安全保证；
只证明地址稳定不够，未补足所需保证前不能当成等价后端。

优先研究 ankerl 自有 Key 的 inline set，同时比较稳定 Node 指针 set 与
Boost node_map，并保留标准 node 表作参考；
不按上游 headline benchmark 直接选赢家，也不为研究自动引入任何依赖。
dense 指针方案仍有 Node 分配、查 Key 的一次解引用、指针数组、bucket/metadata
与容量余量；inline 方案减少这些间接成本，但增加实际搬移与重接及更窄的类型域。
两条冷热链也有 head、计数和潜在的分类状态成本。
这些成本与单节点方案的 Key 回指、hash 节点开销一起实测，不能只比较 hook 字节。

## 节点与所有权

以下固定容量事务先以标准库候选为基线，其存储为 `pair<const Key, Entry>`。
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

Entry 地址参与环和 Key 回指，不可盲目标记 trivially relocatable 或默认移动；
这不禁止自定义 move：inline 候选必须执行上述 hook 重接，不能 bulk memcpy 跳过它。
cache 自身含 sentinel，默认 move 也会留下指向旧 sentinel 的链接。
基线 cache 禁用 copy/move，与内部 Entry 的移动是不同决策；后续 cache 对象移动
再设计 allocator-aware 转移及头尾修复，
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
promotion 本身不搬移对象；修改 T 不影响 Key 或索引。
标准/node_map 与稳定 Node 指针方案在其他条目插入/删除后保持地址；
inline 方案可在插入增长或其他条目 erase 补洞后失效，按后端明确声明，
不承诺跨任意 mutation 保留 T 指针。最终公共接口选择前必须固定这项区别。
不得让同一 cache 的借用键/值参数在其生命周期结束后继续被操作读取；
具体 alias 用例纳入测试，不靠未声明的 shared_ptr 延长生命。
Hash/Eq、allocator、构造/析构及赋值期间不得重入修改同一 cache。

标准表基线要求 Hash 对 Key 的调用不抛出，Key/T 析构和释放按标准约定不抛出。
Hash/Eq 必须满足等价关系、相等键同 hash、存储期间结果稳定等容器要求；
`noexcept` 不意味着纯函数，也不保证用户副作用可回滚。
基线允许 Eq、Key/T 构造和分配抛出；inline 后端的更窄域独立声明。
约束以 C++20 concept/标准 traits 表达，
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
[dense]:
  https://raw.githubusercontent.com/martinus/unordered_dense/v4.5.0/README.md
[dense-source]:
  https://raw.githubusercontent.com/martinus/unordered_dense/v4.5.0/include/ankerl/unordered_dense.h
[boost-node]:
  https://github.com/boostorg/unordered/blob/boost-1.86.0/doc/unordered/unordered_node_map.adoc
[absl-node]:
  https://raw.githubusercontent.com/abseil/abseil-cpp/20240722.0/absl/container/node_hash_map.h
[absl-flat]:
  https://raw.githubusercontent.com/abseil/abseil-cpp/20240722.0/absl/container/flat_hash_map.h
