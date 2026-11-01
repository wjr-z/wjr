<!-- markdownlint-disable MD043 -->
<!-- Ownership and publication order are the review boundaries. -->

# LRU：可选淘汰回调与压缩二级缓存协议

状态：2026-11-01，待审核设计；与 [生命周期设计](lru-lifecycle.md) 一起评审。
本文不提供已实现 API，不新增线程、队列、依赖或 benchmark，不授权后续代码。
目标是让“淘汰后交给压缩/异步二级缓存”具有完整所有权，而不是在 demo 中捕获一个将被释放的 T&。

## 无回调时必须完全裁掉

建议使用编译期 policy，默认 `no_eviction_callback` 的实现特化中**没有** callback
成员、函数指针、std::function、事件队列、L2 version registry、per-Node event flag，
也没有 runtime `if(callback)`。有回调路径通过 if constexpr/特化选择；空 policy 的
EBO/no_unique_address 需按编译器验证，不能把空类型的 sizeof=1 冒充没有对象成本。
不得为未来可能开启的回调提前给所有节点增加版本/任务字段。

验收应比较同一无回调算法的 sizeof/alignof、Node 字段、分配次数及调用者内 codegen，
证明无分支/间接调用/隐藏状态。启用回调后的必需 owner、allocator、版本和队列预算单独计费。
不先做十几个策略开关；首期只有明确的 disabled 与已定义的 handoff 能力。

## 唯一 owner，而非异步裸引用

victim 必须 refs==0。primary 在其串行化域内将它从候选结构和 key index 移除，
同时把真实 Node 转给 typed `evicted_entry` capsule；提交后 cache 不再拥有它。
capsule 可移动、不可复制，携带正确销毁 Node 所需的 allocator/resource/domain 信息。
`T` 本身可以 immovable；移动 capsule 不要求移动 T 或 Key，也不允许猜 pair 的布局。

不同 backend 的 owning extract 是额外能力，不可由 erase(cursor) 推导：

- std node map 可评估 node_handle extraction，并保留实际 node allocator；不是只拿 T*。
- flat pointer index 先移除 Node* 再转移稳定 Node owner；分配器状态须合法转移/保留。
- inline flat 元素无法直接给 immovable T 生成稳定 detached owner。需另有可移动 payload
  事务、稳定 slot 或不支持该 handoff 域，不能暗中增加 Key/T copy。

capsule 建立必须在提交段无抛；如需分配包装/control block，应在可回滚准备段完成，
或使用不分配的 inline owner 表示。分离出来的 PMR Node 仍依赖 resource 生命周期；
借用 resource 必须活到全部 capsule/job 终态。它是否可从 worker 线程释放也要独立证明。

建议 hook 按值接收 capsule，返回 nonthrow 的 handoff 状态：参数在返回前要么转移到
已接受的任务，要么由 RAII 销毁。`accepted` 仅表示任务接收，不表示已压缩/存储成功。
policy 应捕获内部异常并报告/drop，不允许异常穿过析构路径。
若最终允许抛出 hook，已提交 primary eviction 也不回滚，API 必须能报告“primary 成功、
secondary 失败”，不能返回一个看似完全未执行的插入异常。

首期建议 primary 操作的失败保证止于 commit；hook 失败不重插 victim，不撤回已返回的
新 handle，不伪装原子持久化。新条目在调用 hook 前已经取得返回 handle 的引用，
其生命周期不依赖之后的回调是否触发其他缓存活动。出锁不自动许可同步无限重入；
首期 hook 不同步重入同一 cache，异步回入使用公开协议和预算。

## 事件覆盖表

| 原因 | 是否触发普通 eviction hook | L2 语义 |
| --- | --- | --- |
| capacity 淘汰，refs0 | 是，启用时传唯一 owner | 可 best-effort demote |
| 最后一次 handle release | 否，仍是 resident | 只恢复候选资格；不是淘汰或访问 |
| pinned erase 被拒绝 | 否 | L1/L2/版本都不改变 |
| explicit erase/invalidate 成功 | 默认不 demote | integrated L2 模式需失效当前逻辑 key，阻止旧任务复活 |
| replacement（以后若支持） | 单独 reason，不当 capacity | 新 generation；旧内容不得覆盖新内容 |
| try_clear 成功 | 默认不 demote | integrated L2 模式原子失效 namespace/相关记录，再清理 blobs |
| close / destructor | 默认不执行依赖外部对象的用户 hook | 拒绝新任务，显式 drain 已接受任务 |
| 显式 demote_all/drain 模式（以后） | 独立操作及 reason | 调用者仍提供有效 policy/resource/executor，不能隐含在析构 |

只有 primary LRU 时，erase 就删除 primary；它不暗示自动寻找一个外部 L2。
选择 integrated two-tier facade 后，应提供清楚的 `invalidate` 语义，跨两个层执行
版本失效，而不是让用户以为独立两次 erase 可以线性化为一次操作。
首期若该 key 在 primary 被 pinned，invalidate 也应返回 busy 且不改版本；若业务要求
立即隐藏旧值并让旧 handle 继续使用，则进入另行审核的 detached/version-retention 模式。

## 每 key 协调记录与状态机

异步 demotion 和 promotion 使用同一个按 key 路由的 coordinator shard；其锁可以与
对应 primary shard 的锁合一，避免先持 L2 锁再取 L1 锁等相反顺序。
重压缩、分配、IO、等待 executor 均在该锁外。若实现拆锁，必须先给完整 lock order
和版本 commit 协议，不能让 callback 在 primary 锁内进入另一个 cache。

启用 L2 的 key 记录包含不可复用 identity、generation、当前 primary/L2 发布状态和
inflight ticket 关联。generation 表示逻辑内容/失效版本；迁移 job 另带 operation epoch，
使相同内容的旧 demotion 也不能覆盖一次较新的 promotion。不要把 recency 序号当内容版本。

可审核的 identity 建议为 `(runtime_incarnation, shard_id, record_creation_id)`：
incarnation 由进程内 checked monotonic domain-creation counter 分配，仅在创建域的
冷路径使用；record_creation_id 在 shard 锁内独立递增，不是访问计数。记录序号耗尽时
拒绝新记录并报告 sequence_exhausted；可先关闭并 drain 原域，再申请新 incarnation，
不能在有 ticket 时重置。incarnation counter 自身耗尽则拒绝创建新域，不回绕或假设
drain 后还能取得新编号。地址不参与身份比较。此方案只覆盖活进程内唯一性；首个
L2 域要求启动时使用新建/已清空的 volatile namespace，不能把进程重启后重置的 counter
当成持久唯一 ID。

```text
primary resident (pinned / eligible)
  -- capacity commit, unique capsule --> demotion pending
  -- encode/stage outside lock --> staged immutable blob
  -- identity + generation + operation-epoch check --> L2 published

L2 published
  -- retain blob + read ticket --> decode pending
  -- candidate built outside lock --> primary version/capacity commit
  -- successful commit --> conditional L2 invalidation/removal

任一 pending -- failure / stale / cancel acknowledged --> owner cleanup, ticket terminal
```

所有 publish 都比较 **当前 record identity + generation + operation epoch**。
删除 key 后重建时，不能仅把一个版本数重置成 0。地址复用也不能成为“相同 record”的证据。
old job 持 ticket，ticket 保活旧 record；新 incarnation 具有不同身份。

record 仅在无 L1、无有效 L2、无 inflight ticket 时回收；不靠定时删除 tombstone 猜测
旧任务一定结束。记录数量应受 resident 数、有效 L2 条目数和 inflight 上限共同约束。
对完全不存在且无任务的 key 失效，不应永远创建一个 tombstone。
过时的物理 blob 即使尚未回收，只要没有相匹配的有效 record 就必须 lookup miss。

首个域建议仅支持进程内二级缓存。runtime incarnation 改变后拒绝旧 blobs；
volatile registry 加 blob header 不自动提供跨重启一致性。跨重启复用需要外部权威内容
revision 或持久化 version/tombstone 协议，属于单独未实现范围。
generation/epoch 耗尽须停止新任务并安全 drain/rebase，不能在仍有 ticket 时 wrap 复用。

## Demotion 的提交顺序

1. 在 primary 锁内确认 victim refs0、record 仍当前，按策略预留 retire/job/byte budget。
2. primary admission 与 victim 脱离一起提交，建立 unique capsule 和 demotion ticket。
3. 解锁后编码/压缩到独立 immutable blob。编码器必须消费拥有的 Node，或先生成拥有的
   serialized bytes；不得把 capsule 局部变量里的裸指针交给后台后立即析构。
4. 编码、IO staging 完成后再取 coordinator 锁。只有记录身份/版本/epoch 仍匹配且
   未有较新 primary/promotion/invalidation 时，才原子 publish 当前 blob 索引。
5. 不匹配则丢弃 staged blob；匹配则记录发布状态。随后释放原 Node、预算及 ticket。

实际存储 write 若在锁外执行，应先 stage 到独立 blob id，再 CAS/条件发布 key→blob。
“锁外检查一次 generation，随后无条件写 L2”仍然会覆盖新值。provider 若没有条件发布
能力，必须加协调索引/短 commit gate，或拒绝该异步一致性合同，不能把普通 put 包装成 CAS。

## Promotion 的提交顺序

1. 在 coordinator 锁内查 primary；命中直接 acquire。miss 时保活当前 L2 blob 和
   read ticket，取得 identity/version/epoch 快照及 bounded decode/build budget。
2. 解锁读取、校验格式/codec version、解压和构造 candidate；blob view 必须由 owner 保活。
3. 重取锁**先检查 closed**；关闭则只清理/ack，不能返回新 handle 或发布 primary。
   open 时查 primary 和当前记录。若其他线程已入场，按当前 record acquire 它的新 handle，
   并丢弃重复 candidate；这不要求旧 read ticket 与新值同版本。旧 ticket 不得覆盖较新值。
4. 若版本变化/closed/容量全 pinned，则拒绝或报告相应状态，清理 candidate；在尚未
   成功发布 primary 前，不删除 L2 的唯一有效副本。
5. primary 成功提交时在协调锁内撤销读取快照对应的逻辑 L2 publication；锁外物理清理
   必须 conditional remove 精确的 `(record_identity, generation,
   publication_operation_epoch, blob_id)`，其 expected tuple 由 read ticket 保留。
   不能仅比较 identity/version：同版本可能已重新 demote 出一个更新 publication。
   CAS 不匹配则不得删除当前 key 映射；只释放旧 blob 的独占/引用 owner。
   exclusive 模式提交后逻辑唯一，迁移和物理回收期间仍有临时双份字节。

无 single-flight 也可通过最终版本复核保证正确，但并发 miss 会重复解压；相同 key 的
single-flight 是后续可测能力，不是先创建一个无界 future/map。若支持，共享的是受预算
限制的 load record；每个 waiter 的取消不必取消其他人的结果，最后 waiter 取消也要等
worker 确认停止使用 buffer 才可释放 owner。

inclusive 模式允许两个层保存同一 generation，但写入与 invalidate 仍需协调，不能因
“允许重复”就允许旧 generation 再变成当前值。首先只选一种模式交付，避免无依据增加开关。

## 必须证明的交错

| 交错 | 错误做法 | 必须的结果 |
| --- | --- | --- |
| demote A(v1) 慢；插入 A(v2)；旧压缩完成 | 无条件 L2 put(v1) | ticket 版本/epoch 不匹配，丢弃 v1 |
| demote A；invalidate A；任务完成 | 只删当前 L2 key | invalidate 使旧 ticket 无权 publish，不能复活 |
| promotion decode v1；另一线程写 v2 | decode 后直接 insert_or_assign | 再检查 coordinator，保留 v2，不覆盖 |
| promotion 成功；旧 demotion 完成 | 只比较内容版本 | membership operation epoch/primary 状态拒绝旧迁移 |
| promotion(e2) 成功后同版本再次 demote(e3)；e2 的物理 cleanup 延迟完成 | cleanup 只按 identity/generation 删 key | 比较 e2 快照的 blob_id + publication epoch，不能删除 e3 新 publication |
| record 删除后同 key 重建 | version 从 0 重置、比数字 | identity 不同；旧 ticket 保活旧记录直至终态 |
| 取消/close 时 worker 仍读 Node | 立即 free capsule/codec buffer | 取消仅请求停止，ack 后才释放 |
| 解压失败或 primary all pinned | 先从 L2 extract 唯一副本 | L2 保留有效副本，candidate 清理且不发布 |

这些是拟议协议的验收要求，不是已有 provider 自动具备的性质。

## Backpressure、失败与总内存

容量 C 只限制 primary resident，不能代表实际内存上限。启用两级后必须分别预算：
resident/pinned、retired Node、并行 candidate、未压缩序列化、compressed staging、
已发布 L2 blob、decode buffers、key/version registry、队列和 waiters。
一份内容迁移时可能同时存在 Node + serialized bytes + compressed bytes；compression
ratio 不能掩盖这段峰值。深层 T 分配若未由 resource 统计，只能承诺用户 charge 的预算，
不能声称对任意 T 严格限制真实字节/RSS。

预算同时限制 job 数与保留字节。ticket 在资源消耗前取得；未知 encoded 大小时，需有
codec 上界或可失败的增量 byte reservation。编码超限/坏数据/版本不支持/OOM 都走
已选失败策略，不能不断把超大任务转移进另一个未计费 buffer。

| 队列满策略 | primary 语义与代价 |
| --- | --- |
| best-effort drop（建议首期） | primary 仍成功；销毁独占 victim，报告 dropped；不保证 L2 保留 |
| reject-before-primary-commit | 先取得任务预算，否则拒绝新入场；T 可能在 MT 二次复核前已构造；不在提交后尝试回滚 |
| caller-runs synchronous encode | 仅在锁外执行，计入调用延迟及 retire/temporary budget；不能让每个线程无限保留未计费 victim |

不默认阻塞 enqueue；尤其不得在 shard 锁内等待 worker、压缩或取另一个 shard 的锁。
压缩失败默认清理 owner、标记 secondary failure，primary 已提交成功。若业务需要“无损
移交后才允许淘汰”，那是更强事务/存储合同，不能由普通缓存回调伪装达成。
所有 counter/metrics 也是可选成本，不为了报告失败给无回调 cache 增加常驻统计字段。

## 关闭和资源寿命

close 先在 coordinator 内关闭新 primary/L2 请求；不在锁内等任务。之后按协议取消或
等待已接受任务，丢弃不再有发布权的结果，收齐 terminal acknowledgement，最后销毁
codec、executor、resource、registry。调用者持有的 primary handle 也必须先释放。
若 executor/resource 是借用对象，其寿命必须覆盖 drain；移动 unique capsule 不延长外部
PMR resource 寿命。回调析构不访问一个已经销毁的 app 对象。

本稿选择：close 线性化后，**所有此前接受的 encode/promotion ticket 均不得再 publish
L1/L2 或向新调用返回 handle**，只可停止、清理并 ack。最终复核必须先检查 closed，
再判断 existing-return/new-commit；不能因“primary 恰好已存在”绕过关闭。
close 之前已经返回的 primary handle 继续按原合同保活。

析构默认只做无用户回调的资源回收；显式 close/drain 承担可等待/可失败流程。
若未来让异步任务活过 cache facade，应独立 owning domain 保留实际 resource/executor
依赖并计入状态/分配，不以捕获 this 或“后台线程很快结束”作为寿命保证。

## 成熟参考与实施门槛

[RocksDB 9.7.3 `secondary_cache.h`][rocks-secondary] 明确区分 pending/ready 结果、
Lookup 等待和调用方对象所有权；Insert 中 caller 保留对象，不能直接照搬成异步裸指针。
该接口也禁止异常穿出扩展。它提供本稿的对照边界，不证明 WJR 的版本/CAS/取消协议。
[Folly 固定版 prune][folly] 的独占 owner 与脱离后 hook 可参考，但不具备本稿的双层协调。

批准后先用同步内存 L2/provider stub 和确定性调度测试 ownership/version，再接压缩与
bounded async executor；不能先做 happy-path demo 后补 erase/close/失败语义。
验收覆盖上表全部交错、每 N 次分配失败、pending/read/decode 分段取消、队列满与字节满、
generation 小位宽耗尽、资源跨线程释放、终态 exactly-once cleanup。
性能则在相同内容/版本/失效合同下计入全链构造、压缩、解压、排队、命中率和总峰值，
无回调分支单独证明布局/代码成本；未经测量不承诺压缩后端一定更快或更省峰值。

[rocks-secondary]: https://github.com/facebook/rocksdb/blob/v9.7.3/include/rocksdb/secondary_cache.h
[folly]: https://github.com/facebook/folly/blob/v2024.08.19.00/folly/container/EvictingCacheMap.h
