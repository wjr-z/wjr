<!-- markdownlint-disable MD043 -->
<!-- Every implementation phase requires approval after the lifecycle review. -->

# LRU：审核门槛与后续能力交付

状态：2026-11-01，设计阶段；用户已撤回此前 LRU 实现提案，要求审核后再实施。
**本文及相关设计 MR 合入，不构成 runtime 实现授权。** 尚无已交付 LRU API 或性能结论。

当前合同入口是 [访问、外部持有与并发所有权](lru-lifecycle.md)，以及
[淘汰回调与二级缓存](lru-secondary-cache.md)。旧 [LRU 设计](lru-cache.md) 仅保留
借用型基线、后端源码比较和历史依据，不以旧任务顺序绕过新的生命周期要求。

## D0：先完成集中审核

先明确以下选择，再由用户批准第一阶段实现：

- acquire / touch 作为访问事件；handle copy、持有和最后 release 不更新 last_use。
- 双 ring、indexed min-heap 或成熟 intrusive tree 的复杂度、Node/数组成本、回绕
  慢路径和无抛最后释放可证明；不默认 release-MRU。
- cache 活过 handle；pinned erase 返回 busy；try_clear all-or-no-change；close/drain
  和跨 shard 线性化合同清楚。若改用 detached domain，另算所有权和预算。
- ST/MT 入场构造保证、两阶段冲突浪费、B 个候选预算和 allocator 并发域明确。
- 选定真实 dense/flat 候选与标准 baseline；adapter 的地址、cursor、erase 搬移、
  异常、allocator 和 owning extract 能力不由同名方法推断。
- 无回调模式无回调/L2 状态；有回调的 primary commit、unique capsule、失败/队列满
  和 generation 发布协议完整，不能捕获裸引用表达异步所有权。

未完成的适配、尚未运行的数据或编译成功都不是性能收益。实施前记录已选合同和未支持域，
不新增泛用空框架。下列阶段是连贯能力边界，不要求为了 MR 数量逐条机械拆分。

## L01：完整 ST pinned cache 与真实后端比较

获批后的首份 runtime MR 应包含 acquire/handle/touch/入场/erase/try_clear/close、
选定排名结构、异常事务、示例和安装消费者，不先发布 ref ptr 壳或半个容器。

unknown backend 必须显式适配。比较 standard node 与至少一个同合同 flat/dense 方案。
inline movable Entry 也做小型完整 LRU 原型成本比较，但 live Node* handle 下不能搬移
pinned Node；slot-id 需计入间接层和版本。不能以取消 pinning 保证制造 inline 优势。

独立 oracle 只保存 last access、refs、内容版本，直接求最旧 eligible，不复制实现结构。
覆盖容量 0/1/全 pinned、长持有旧 A 后释放、reverse release、copy/move 不 touch、
explicit touch、重复入场不构造 T、失败不改旧成员/排名、每 N 次分配/Eq/构造失败、
不可移动 T、PMR/scoped allocator、高对齐、小位宽序号回绕。

最后释放不分配排序包装节点。验证准确 traits/noexcept、合法 hook move、Key/Node 地址
和 owning extract，不假标 relocatable 跳过重接。遵循现有 CMake/module 注册，保持
C++20/baseline，不加 ISA flags 或 Windows 矩阵。运行 focused correctness、Linux
ASan/UBSan、全量格式 hook；不同平台实际覆盖如实记录。

性能包括完整 lookup/acquire/release/touch/churn/admission 和构造析构，分配计数与计时
分离；共同强 hasher 与各 backend native hasher 两组解释。保留 pinned 比例、长持有、
key/value 形状的常见组及退化组，不只测裸 map。Windows/Linux 各自在同机同 trace 比较，
公布 raw samples、flags/source/依赖版本，不跨机器解释 OS。live/peak 与 RSS 分开。

## L02：可选回调与同步二级缓存

在 L01 上形成 typed evicted_entry ownership handoff，不只加一个空 callback 参数。
std node extraction 与 flat stable Node owner 分别证明 allocator/resource 寿命；
callback-free 版本用 sizeof/alloc/codegen 证明没有额外字段、分支或调用。

同步内存 L2 提供真实调用：capacity demotion、查询 promotion、invalidate、版本拒绝旧
结果、重复 key、编码失败/超预算/primary full 的清理和返回状态。压缩使用另经评审的
成熟 codec，固定版本/license/启用成本，不自造压缩器，不用压缩比掩盖构造峰值。

尚不需要异步就不预留 executor/queue/waiter 状态；需要异步则先完成可转移 owner，
不能从同步 hook 借用 T& 投递后台后让 primary 析构 T。

## L03：MT 与分片能力

首个 MT 正确模型用单锁 exact access-LRU；分片方案声明 per-shard LRU，不把它与全局
exact 当同一策略。global timestamp 不代替 candidate arbitration。每 shard 固定容量
不均的拒绝语义、全局 try_clear/close 和一致 snapshot 的锁序必须明确。

key lookup、refs 0↔1、候选、erase 与 owner 脱离在同域串行化。不能裸 T& 逃出锁就声称
payload 线程安全。独立 handle 的发布与同一 handle 并发 reset 是不同问题；Hash/Eq/
allocator/resource 的线程安全也分别证明。

先做确定性交错：last release 对 acquire/erase、两阶段 duplicate/all-pinned、close/
try_clear、候选预算及跨线程 free；再用 TSan、ASan/UBSan 和随机模型补充。
atomic positive clone / count>1 CAS-release 只在内存序和最后释放协议获证后优化，
不发布“atomic refs + 无锁链表”的未证明组合。

测量单热 key 计数争用、不同 shard 扩展、锁期/尾样本、构造冲突浪费、预算不均、慢析构
及 allocator 锁成本，不仅平均吞吐。独立 ST 借用路径保持没有锁/atomic/shared_ptr 成本。

## L04：有预算和终态确认的异步二级缓存

使用获批 executor/runtime 或成熟 provider，形成真实压缩/查询/取消 consumer，不重造
通用 scheduler。先固定 job 数、retired/candidate/codec bytes、registry/waiters 上限
和 queue-full 策略，再接后台任务。

任务持有 capsule/blob 和 key record ticket；stage 后 conditional publish，promotion
先构造再复核，成功前保留 L2 副本。identity/generation/operation epoch 防止旧结果
覆盖新值、erase 复活和地址 ABA。取消请求不是完成；owner 在 worker 停止使用后回收。
close/drain 不持 shard 锁等任务，跨重启持久一致性不由进程内版本协议冒充。

覆盖二级设计的全部交错、每 N 次分配失败、pending/read/decode 各阶段取消、队列/字节满、
序号耗尽及资源跨线程释放，确认终态 cleanup exactly-once。计时包括编码、解码、排队、
命中率、尾延迟及全部 live/peak bytes；provider 裸 put/get 不能替代完整多级 cache 成本。

## 审核后仍可拒绝实现

同合同下没有值得维护的能力或成本优势时，归档原型并停止，不为数量发布。
失败也不自动授权更大 arena、slot runtime、锁策略或后端框架。
设计审核是实现依据，不是实现、测试或性能收益的替代品。
