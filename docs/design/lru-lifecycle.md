<!-- markdownlint-disable MD043 -->
<!-- This is a contract and algorithm review, not an implementation announcement. -->

# LRU：访问顺序、外部持有与并发所有权

状态：2026-11-01，**待审核设计，不授权实现**。当前 `main` 没有可用的 LRU
公共接口。[此前实现提案 #225](https://github.com/wjr-z/wjr/pull/225) 已关闭；
本地研究代码和测试不构成已交付能力或性能结论。
本文是新的评审入口，优先于旧 [借用型基线](lru-cache.md) 中不支持持有追踪的设想。
[二级缓存协议](lru-secondary-cache.md) 与本文一起审核；实施另需明确批准。

## 要解决的调用问题

调用者查找昂贵结果后，可能跨越后续 cache 操作继续使用它。仅返回 `T*` 无法判断
它是否仍被使用；淘汰后指针会失效。因此核心能力需要轻量 RAII handle，持有期间
节点不可淘汰。持有保证生命周期；它既不自动说明数据仍热，也不提供 T 的互斥访问。

本次明确要求：外部引用数 `refs > 0` 的 resident 不在淘汰候选结构中，`0→1` 移出，
`1→0` 自动恢复资格；不得销毁仍被引用的节点。容量含 pinned resident。全 pinned 时
不能靠越过容量或释放活引用完成入场。关闭、erase、回调和异步任务必须遵守同一所有权协议。

以下类型名均为拟议名称。首期以 `pinned_lru_cache` 为审核对象；若保留借用型
`lru_cache`，它是独立合同和独立零计数成本路径，不能让同一裸指针随配置偷偷变成强引用。
不在本稿实现 CRTP 框架、自造哈希表、无锁 freelist、TTL 或冷热命中率策略。

## 固定成熟参考及使用边界

| 固定来源 | 源码事实 | 本设计取舍 |
| --- | --- | --- |
| [LevelDB 1.23 `cache.cc`][leveldb]，[接口][leveldb-api]，BSD-3-Clause | `Ref/Unref` 在 mutex 下转移候选资格；最后释放回最新端；erase 可留下 detached handle；析构要求没有未释放 handle | 参考所有权与锁域，**不把释放顺序当访问时间 oracle**；首期不采用 detached erase |
| [RocksDB 9.7.3 `lru_cache.h`][rocks-h]、[实现][rocks-cc]，Apache-2.0 或 GPLv2，含 LevelDB BSD 部分 | 区分 resident pinned、resident unpinned、detached pinned；Release 锁内改变状态、锁外释放；该版本 Release 的一处 eviction callback 分支被 `false` 禁用 | 参考三态及解锁后析构；不能据回调声明声称其任意 handoff 已可用 |
| [Folly v2024.08.19.00 `EvictingCacheMap.h`][folly]，Apache-2.0 | 非线程安全；prune 先从索引和链脱离，再由独占 owner 调用 hook；默认 hook 不覆盖 erase/析构；容量 0 含义不同 | 参考脱离后所有权，不复用其容量 0 或完整生命周期语义 |
| [Boost.Intrusive 1.86 有序容器][boost-tree]，BSL-1.0 | 提供现成红黑树 hook 和对数复杂度；节点不额外拥有 payload | 若选择树，复用成熟实现，先审依赖与 ABI，不自造 RBtree |

这里是按源码提炼的设计参考，不复制实现，不新增这些依赖，不套用上游吞吐声明。
上述许可证可在各固定源码树的 LICENSE 文件核对；真实复用代码时还需保留其声明。

## 使用事件与持有事件必须分开

建议将成功 `acquire(key)` 和成功新入场定义为访问事件；`touch(handle)` 显式报告
长时间持有期间的再次使用。`pin(key)` 可提供“不更新访问顺序的持有”，待实际调用确认后
再决定是否首期公开。不要为对称性增加重复接口。

| 事件 | 引用数 | 访问排名 |
| --- | --- | --- |
| `acquire` 命中 | `+1`，必要时摘候选 | 赋新的访问序号 |
| handle copy / move | copy `+1`，move 仅转移一个引用 | 不变 |
| `touch(handle)` | 不变 | 赋新的访问序号 |
| `operator*` / `operator->` / `get` | 不变 | 不隐式计时 |
| 最后 `reset` / 析构 | `1→0`，恢复候选资格 | **保留最后访问序号** |
| miss、失败入场、失败赋值 | 不变 | 不变 |

表中序号描述逻辑访问排名；双 ring 方案可直接用链顺序表达，不强加显式序号字段。

指针解引用不是可靠的使用计量入口：`auto& value = *handle` 之后的使用无法自动观察；
在 `operator->` 内 touch 也覆盖不了逃逸引用。不能声称“只要持有就一直 MRU”，也不以
handle copy 或 release 冒充真实访问。调用者若要保持热度，应在有意义的使用点 touch。

最小反例：先访问并持有 A，再访问 B；随后释放 A，最后插入 C。此时 A 比 B 更旧，
若 A、B 都未被持有，应淘汰 A。把 A 最后释放时插入 MRU 会错误淘汰 B。
长期 pinned A 的旧访问序号也必须参与之后的排序恢复和序号重标定。

## 最小 API 与可见结果

| 拟议操作 | 合同 |
| --- | --- |
| `acquire(key) -> handle` | miss 或 closed 返回空；hit 计一次访问并 pin。返回前完成索引、计数和候选结构的同一次状态转换 |
| `touch(handle) -> touch_status` | 非空 handle 来自同一 cache；open 时返回 touched 并更新排名，不新增引用；close 后返回 closed，排名不变 |
| `try_emplace(key,args...) -> admission_result` | 区分 inserted、existing、disabled、capacity_pinned、closed；成功返回 handle。ST duplicate 或 MT 初检 duplicate 不构造 T；MT 二次查重可已构造 candidate。existing 按 acquire 记一次访问 |
| `try_erase(key) -> erase_result` | 区分 erased、missing、pinned、closed；pinned 不删除、不改变顺序 |
| `try_clear() -> bool` | open/closed 均可调用；有任意 handle 则失败且完全不变，成功清空全部条目，不重新 open、不 demote。不得部分删除却仍名为 clear |
| `close()` / `drain()` | 见关闭协议。关闭后不接受新访问/入场；已有 handle 仍可安全释放 |
| `size()` / `capacity()` | resident 数量与固定条目预算；size 在 MT 取全部 shard 锁提供一致快照，closed 时仍可查询；不是总内存或 RSS |

默认 handle 暴露 `const T&/const T*`，避免把生命周期保证包装成并发可变访问保证。
若需可变 T，可另用明确的调用方同步合同或受锁访问接口；本稿不允许边持有边悄悄替换 T。
`insert_or_assign` 暂不作为 pinned 首期必需接口：若之后支持，pinned hit 必须返回 busy，
或另行定义旧版本保留及 detached budget，不能向现有 handle 换掉对象。

不为 MT 提供会逃出锁的裸 `peek`。无触摸查询若确有用途，应返回 pin handle，或提供
有明确锁期的受限访问。ST 借用变体可以有裸 `peek`，但须声明任何 erase/eviction 后失效。

容量 0 不构造 stored Key/T；但函数实参求值与必要类型转换仍由调用者在调用前执行。
正容量按 resident 数计，外层 allocator/PMR 不自动覆盖 Key/T 深层分配，uses-allocator
和 scoped allocator 必须逐层验证。首期 cache copy/move 禁用，避免 owner、sentinel、
资源和 handle 指向旧对象；这与内部可移动 Entry 是不同决策。

## 所有权、不变量及轻量 handle

首期 cache 独占所有 resident Node；handle 仅阻止 cache 销毁对应 Node。
`refs` 只统计外部 handle，不把 cache 自身的所有权混进该数。

1. 每个 resident 恰好出现在 key 索引一次，Key 存储期间逻辑只读。
2. `refs == 0` 当且仅当它在候选结构中；`refs > 0` 时完全不在该结构。
3. 候选结构按最后访问排名有序；最小者是下一 victim。
4. pin、恢复候选、unlink、索引脱离及 owner 转移必须在同一串行化域内完成。
5. 只有不在索引、无外部引用且由唯一 retired owner 持有的 Node 才能析构或交给二级缓存。

ST handle 可以只有一个 `Node*`，Node 包含非 atomic 的外部计数及 owner/backpointer。
copy 加计数，move 不改计数，析构减计数；不使用每 handle control block，不因 clone 分配。
计数加法需定义上界检查/失败合同，不允许 wrap；释放和析构不抛。
是否做可复用 intrusive pointer 应由 pinning 实际调用证明；不先发布一个无人调用的智能指针壳。

首期采用 **cache/domain 必须活过全部 handle**。`try_clear` 需要检查有无外部持有，
可遍历现有索引，不为这个冷查询强加全局 pinned counter；是否缓存计数由真实查询负载决定。
违反析构生命周期前置条件是调用方错误，不能只靠 debug assert 把 release UAF 合法化。
应推荐显式 close/drain，并在公共文档直接写出前置条件。

若以后要求 handle 活过 cache，需要独立 domain：例如 domain 一次分配、每 Node 一次
domain 持有，再由 Node 计数服务 handle clone；这仍有额外分配/状态/释放成本。
`make_shared` 可以合并对象与 control block 为一次分配，不能声称 shared_ptr 必然多一次分配；
应比较实际布局、计数和 atomics，而不是仅凭名字判定成本。

## 排名恢复：三种精确候选，不用 release-MRU 近似替代

令 `C` 为容量，`N<=C` 为当前 resident 数，`U` 为 unpinned 数，`P=N-U` 为 pinned 数。
以下均另有期望 O(1)、
最坏 O(C) 的 key 哈希查找。只有相同访问定义和生命周期保证才能直接比较。

| 方案 | acquire 摘候选 / 最后 release 恢复 | victim | 额外结构与难点 |
| --- | --- | --- | --- |
| A：全 resident access ring + eligible ring | 摘候选 O(1)；release 从 access 邻居扫描，最坏 O(C)，常见受邻近 pinned 数影响 | eligible 尾 O(1) | 每 Node 两组 prev/next；access ring 含 pinned，但 eviction ring 不含；作直观 oracle 候选 |
| B1：indexed min-heap，只有 eligible | 按 heap_pos 删除 O(log U)；按原 last_use 插回 O(log U) | root O(1)，删除 O(log U) | 预留 C 个 Node*；每 Node last_use + heap_pos；局部性值得验证，不能预先断言更快 |
| B2：成熟 intrusive 有序树，只有 eligible | 摘除/插回 O(log U)；不按 release 时间更新 key | 缓存最小节点 O(1)，删除依实现保证 | 约 3 pointers 与颜色/对齐，外加 last_use；不自制树，依赖与代码体积另评 |

A 在全 resident ring 上给每次访问移至最新位置；pin 只摘 eligible hook，保留 access
hook。最后释放时沿 access ring 找最近 eligible 邻居，再插入 eligible ring 的对应位置。
这样 release 不改变真实顺序，但长 pinned 区段会增加释放延迟；两个 hook 的复用和清理须证明。
不能只在 acquire 时保存旧 prev/next：这两个邻居可能在长持有期间被淘汰，release 时
再解引用保存的 anchor 会 UAF；它不是无需额外生命周期协议的 O(1) 中间插回方案。
单链表若不保留额外搜索信息，按旧排名插回中间一般需要扫描；这不是对所有 word-RAM
数据结构声称普遍的 O(1) 不可能性。

B1 用数组 `heap[0..U)` 存 eligible Node*，Node 的 `heap_pos` 指向自身槽；pinned 用
invalid position。删除槽 i：以尾指针补洞，更新该节点 pos，然后按比较结果向上或向下
sift，每交换一次都更新两个 Node 的 pos。插回在尾部 append 并 sift-up。
操作比较 `(last_use, unique_tie)`，其中 tie 仅在设计允许相同序号时需要；串行独立递增
序号可消除此字段。每次恢复资格使用 Node 保留的 last_use；不创建新的访问事件。
数组在构造阶段预留 C，last release 不分配、不抛。现有 vector 可复用；标准 heap 算法
不自动维护外部 heap_pos，所需的少量有位置更新的 sift 逻辑须单独验证。
**不使用 lazy stale heap records**：旧 Node* 会 UAF，版本包装也不能解决无界滞留及内存上限。

B2 只将 eligible Node 的 intrusive tree hook 链入，按 last_use 排序；更新排名必须先
移除再修改排序键，不能留在容器内修改 key 并假定合法。Node 先完成类型/allocator 构造，
再参与 hook。候选树不得额外为每次 release 分配一个 std::set 包装节点。

另一对照是单 resident ring，淘汰时跳过 pinned：实现简单但扫描会转移到入场路径，且
pinned 仍在扫描用 ring 中，不满足本次“不在淘汰候选结构”的合同，不作为默认承诺。

## 访问序号、回绕与复杂度边界

序号型 B1/B2 在 ST 使用 cache 内递增序号，在 MT 使用 shard 锁内递增序号；
A 使用全 resident ring 的相对顺序，不需要序号回绕路径。不使用墙钟时间，不声称跨
shard 存在全局 total LRU。触摸顺序以锁内提交的事件顺序为准。引用 copy/release 不取号。

不能依赖无符号 wrap 的普通 `<` 排序：长 pinned 节点可能保留极旧序号，不满足半区间
比较法的窗口前提。回绕慢路径必须把 **所有 resident，包括 pinned** 一起重标定。
简单减去最小值也不够：长期 pinned 节点一直持有最老序号时，min-offset 可能没有进展。

B1 可在暂停该 shard 时把 pinned 指针追加到已预留 C 的同一数组，按旧序号原地排序，
赋紧凑新排名，压回 refs==0 的元素并重建 heap/pos。需要选定确实不分配的排序实现，
O(C log C) 时间、无新 Node 分配；不能破坏 key 索引或暂时向外发布中间状态。
B2 可以使用明确的冷路径 scratch 和失败前不改变状态的事务，或维护额外可重标定顺序；
这些空间/异常成本不能省略。加宽到 128 位也仍需给耗尽合同，不能称永不溢出。
该选择与排名方案一并审核，首次实现不得留下未定义的 wrap 行为。

## ST 入场、失败与淘汰事务

ST 操作期间禁止 Hash/Eq、allocator、构造/析构重入同一 cache。Hash 对存储 Key 的
调用必须满足选定后端的 nonthrow 域；Eq/构造/分配是否可抛由 adapter 实际证明。

1. 查新 key；duplicate 返回现有 handle 并记访问，不构造新 T。
2. 容量 0 / closed / full 且候选空时立即返回状态，不构造 stored Key/T。
3. 固定当前最旧 eligible victim；准备它的合法删除 token 和所需 callback 预算。
4. 在独占 candidate owner 下完成 Key/T 构造及索引 admission。失败不改变旧成员/排名。
   容量正数时允许暂时 C+1 个构造对象，但对外提交后 resident<=C。
5. 无抛提交：新条目绑定 owner、访问序号、refs=1；victim 摘候选/索引并转为唯一 owner。
6. 提交成功后再调用可选回调/销毁 victim；回调失败不回滚已完成的 primary 操作。

后端若不能保证步骤 4 的失败不影响旧对象，就不能仅因有 try_emplace 同名函数加入该域。
intrusive heap/tree 的内部步骤也必须 nonthrow，不能在正常释放路径分配恢复排序节点。
赋值失败只获得 T 自身的保证；不将此泛称整个 cache 的强保证。

## MT：同一 shard 锁保护资格、索引和引用转换

首个正确模型用成熟 mutex + intrusive/array 数据结构。锁保护 key index、所有 refs、
候选结构、访问序号、resident/closing 状态和 owner 脱离提交。每个操作的线性化点在
该锁内；Hash/Eq 的线程安全以及 Key 逻辑不变仍是前提。锁期内不运行压缩、IO 或用户回调。

| 路径 | 锁内动作 | 锁外动作 |
| --- | --- | --- |
| acquire | find；若 refs0 则摘候选；增 refs；记录一次访问 | 通过 handle 只读使用 T |
| handle copy / reset | 验证域；加/减 refs；1→0 按原排名插回 | move 不改计数；不因 release touch |
| erase | find；refs>0 返回 pinned；否则摘候选与索引并转唯一 owner | 析构或按约定通知 |
| admission | 二次查重、容量/closing/版本复核；排名与索引原子提交 | 构造 candidate、回调/析构需见下文边界 |

第一版即使“已有正引用的 clone”也按同一锁协议。以后可以证明 atomic-positive-clone
快路径，但需同时证明 source handle 本身安全发布、所有计数访问原子化、溢出和最后释放协议。
仅把 refs 改 atomic，不等于可以在锁外 `--refs` 到 0 后再取锁。

一个待证明的优化是：已有活 source handle 的 clone 用 relaxed fetch_add；release
仅在观察 count>1 时 CAS 减一，观察到 1 时先取 shard 锁，再完成减一和候选转换。
竞争 clone 可能使锁内读数重新变大，必须按实际结果处理，不能依据过时的 count==1
强行入链。memory order、erase/retired 状态、发布和复用须统一证明后才可选用。
单热 key 仍会争用同一计数 cache line；嵌入计数不等于无 atomic 成本或必优于 shared_ptr。

反例：线程 R 在锁外将 1 减到 0，尚未取锁；线程 E 看到 0，erase 并 free；R 再访问 Node
准备恢复候选，形成 UAF。或者 acquisition 在减计数与入链之间复活它，导致 pinned 节点
错误留在候选中。同锁转换消除这两个窗口；不能用 lock-free 链表掩盖 owner 竞态。

T 的跨线程只读安全不等于它内部 mutable 字段安全；持有两个 handle 不许可无同步写 T。
对同一个 handle 对象并发 copy/reset 仍需调用者同步。互相独立、已正确发布的 handle
才参与 cache 内部引用协议。首期 ST handle 不能借名用于 MT。

## MT 构造临界区的明确选择

锁外构造可缩短临界区，却会在复核时遇到 duplicate、closed 或 all-pinned。
两阶段建议是：锁内初检/取得 bounded build ticket；解锁构造 candidate；重新加锁查重与
复核 victim，然后提交或销毁 candidate。它允许最终拒绝前已经构造 T，必须公开此事实。
初检即 all-pinned 的拒绝可以不构造，**不承诺每一种竞争导致的拒绝都不构造**。

若要求 MT 也完全不构造任何最终拒绝的 T，则需构造持锁，或 reservation/等待规则防止
victim 被并发 pin；这会扩大临界区或改变 acquire 的阻塞合同。三者不能不加说明地兼得。
本稿建议先审核两阶段，测量冲突浪费和锁时长；用户若选择更强保证，再调整算法。

构造候选数 B 必须有独立上限；外部并发线程可能同时创建 B 个候选，不能继续宣称整机
只会有 C+1 个活对象。两阶段 Hash/Eq 状态和 PMR resource 也不能在锁外无保护并发调用。
若用 unsynchronized_pool_resource，锁外 allocate/free 须改为同域串行、单独 allocator 锁，
或选线程安全资源；“节点已从 shard 分离”不使其分配器自动线程安全。

分片按稳定 hash 路由，`C_i` 总和固定，余数分配可复现；shard 满而其他 shard 空仍可能拒绝。
每 shard 的 exact LRU 不等于全局 LRU。动态搬移、跨 shard 借容量和全局排序另作设计，
不以共享 atomic 计数或跨 shard 热锁悄悄实现；本稿的 size 明确采用全 shard 一致快照。
全局 exact LRU 可用单锁作为正确性基线；若要多锁而全局 exact，还需 candidate arbitration
和跨 shard 提交协议，一个 global fetch_add timestamp 并不能单独保证候选一致性。
用于 shard 路由的 Hash 若在锁外调用，需要额外线程安全合同，noexcept/const 不代表纯函数或无数据竞争。

## 关闭、erase 与长期持有

首期 `try_erase(pinned)` 返回 busy；不从索引隐藏它，不另造 detached pinned 状态。
`try_clear` 有持有时 all-or-no-change；它与 `close/drain` 是不同操作。
MT 跨 shard 的 try_clear 须按固定顺序取得所有 shard 锁，先全部检查再全部提交，失败时
任何 shard 都不清空。全局 close/一致 size snapshot 同样要有明确跨 shard 线性化协议，
不能逐 shard 修改到一半才发现 pinned，然后宣称 all-or-no-change。
本稿选择 size 也按固定锁序取得全部 shard 锁；它是 O(shard_count) 冷查询，不新增全局
热 atomic resident counter。close 在所有锁持有时设置全部 closing 状态，作为其全局
线性化点；不将“逐个 shard 慢慢关闭”宣传为已经完成的全局 close。

MT try_erase/try_clear 提交只脱离 owner，T 析构和 allocator 释放放在锁外，但仍受
resource 的线程域约束。try_clear 需要可无抛脱离的 owning batch，或在提交前准备 owner
容器；预备分配失败时不清任何条目。收集/脱离 O(N) 和取全部锁的停顿属于冷路径真实
成本，owner batch 的暂存字节也计入峰值，不宣称 clear 是无额外内存的瞬时操作。

建议 close 在锁内拒绝新 acquire/admission，已有 handle 仍可释放。最后 release
继续按原排名恢复资格；drain 等待 refs 全部为 0 后，锁内整体脱离，锁外销毁，最后等待
已接受二级任务达到终态。ST 不提供在同一线程持 handle 再阻塞 drain 的伪方案；可提供
try_drain 或由调用者先释放。MT 在持自身 handle 时同步等待也可能自死锁，应直接写入合同。

close 后 touch 返回 closed；已有 handle 仍可 copy/move/reset，copy 仅延长原持有而
不计访问，调用者必须最终释放它们。drain 的完成依赖这些外部持有结束，不承诺用户永久
保留 handle 时仍能完成。closed 上的 try_clear 仍按 all-or-no-change 清理，不恢复服务。

将来若支持 erase/replacement 后旧 handle 继续存活，则应增加明确 detached budget 和
计费，覆盖旧版本数量、Key/T 深层内存和 shutdown 的 owner；否则用户持旧 handle
可以制造无限 retention，表面 capacity 再严格也无意义。不可直接套用成熟库的 detached
语义，却继续宣称总存储受 resident C 限制。

## 后端、节点搬移与实际成本

用户可选择不同 map，但未知后端需要显式 adapter，不能由方法签名自动推断正确性。
至少审这些真实影响算法的能力：Key/Node 地址、bounded admission token、erase 搬移、
异构 Hash/Eq、reserve/growth、异常保证、allocator 实例传播，以及是否能把真实 Node
及其分配器一起无损脱离给 owning capsule。普通 erase(iterator) 不等于 ownership extract。

- node map 可内嵌 Key/Entry/hook，不为索引再分配一个 Node；std node handle extract
  的 allocator 与释放线程限制必须跟随 capsule，不能仅保存 T*。
- flat index + stable Node 保留一份 Key，index 存 Node*；需要计入 Node 分配、间接加载、
  指针数组和桶容量，而非只比裸 map lookup。
- inline movable Entry 可以用 WJR 既有 replace/remove 接好 hook，不必永远排除。
  但 live handle 指向 Node 时不能搬移它；可研究 slot-id/generation handle 或 pinned
  稳定存储域。额外一次间接加载、slot table、版本和被 pinned 桶对增长的影响均计入。
  不以 pair.second 的 offsetof/container_of 猜 pair.first，不盲目复制 Key 修补回指。

固定比较候选为 ankerl 4.5.0（MIT）、Abseil 20240722.0（Apache-2.0）、LLVM
llvmorg-21.1.0（Apache-2.0 WITH LLVM-exception）及标准 node baseline。
[ankerl erase][dense] 会搬移末尾元素；reserve 不能消除 erase 搬移。
[Abseil 该版][absl] 明示非异常安全，不能把 nothrow T move 当成 allocator/Eq 都安全。
[LLVM DenseMap][llvm] 的 KeyInfo 空/墓碑值和非 STL allocator 路径须另行适配。
std::flat_map 是 C++23 有序平坦容器，不是上述哈希表。

空回调成本、后端成本和可选计数不能凭模板名字推断。x64 成本预算需包括：Node 的
owner/count/rank/heap_pos 或 hook、Key 回指、真实 Key/T、padding、hash metadata、
heap 数组、cache/shard mutex、allocator/resource、candidate/retired/job owner。
引用追踪模式与零引用计数借用模式分组，不能靠删掉生命周期保证宣布更快。

## 审核决策与后续验收

审核需明确：访问事件定义；A/B1/B2 排名方案；ST 与 MT 构造拒绝保证；handle constness；
erase/clear/close；引用是否可超出 cache；回调和二级协议；后端与 allocator 可接受域。
这些未批准前不发布 runtime MR。批准后按 [实施门槛](lru-cache-implementation.md) 分阶段。

第一阶段 oracle 应直接保存每个 key 的最后访问序号、外部 refs 和内容版本，独立求
`argmin(last_use | refs==0)`，不复制实现的数据结构。重点测试长持有旧 A、更新 B 后释放 A，
reverse release、copy/move 不 touch、显式 touch、全 pinned、容量 0/1、失败不改排名、
序号小位宽强制回绕、异常清理、PMR owner 和不可移动 T。

MT 需确定性交错测试 acquire/last-release/erase、并行 admission 二次查重、close/drain、
候选预算满及跨线程释放资源；配合 TSan 与 ASan/UBSan，不把无数据竞争等同于线性化正确。
单个随机压力测试不能代替上述状态机与进度证明。

获批后的性能比较才运行：同平台/同机/同 trace 比 lookup+acquire/release、touch、pin
比例、长持有分布、churn、冷构造/析构及锁竞争；单独统计 Node/index/heap/candidate/retired
的 allocations、live/peak bytes 和 RSS，保留落后组。共同强 hasher 与各表惯用 hasher
分开解释，不能用平台不同的弱 identity hash 归因 flat layout。Windows/Linux 同域各自
比较，不跨机器解释 OS 效应。不在本设计稿声称已实现或已测出的速度。

[leveldb]: https://github.com/google/leveldb/blob/1.23/util/cache.cc
[leveldb-api]: https://github.com/google/leveldb/blob/1.23/include/leveldb/cache.h
[rocks-h]: https://github.com/facebook/rocksdb/blob/v9.7.3/cache/lru_cache.h
[rocks-cc]: https://github.com/facebook/rocksdb/blob/v9.7.3/cache/lru_cache.cc
[folly]: https://github.com/facebook/folly/blob/v2024.08.19.00/folly/container/EvictingCacheMap.h
[boost-tree]: https://www.boost.org/doc/libs/1_86_0/doc/html/intrusive/set_multiset.html
[dense]: https://github.com/martinus/unordered_dense/blob/v4.5.0/include/ankerl/unordered_dense.h
[absl]: https://github.com/abseil/abseil-cpp/blob/20240722.0/absl/container/flat_hash_map.h
[llvm]: https://github.com/llvm/llvm-project/blob/llvmorg-21.1.0/llvm/include/llvm/ADT/DenseMap.h
