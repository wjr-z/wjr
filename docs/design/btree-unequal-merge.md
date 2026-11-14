# 不等高分片的 graph merge

本实现基于已合入 #309 的 main `9227a90300247bf4eca560918ca23b5f38dd2cec`。
原型阶段以 #309 的 `03c093b34ace36ec2f2fecc8d542b68019fdc791` 验证，相关研究记录保留。
入口沿用普通 `merge` 及显式 `append_btree_disjoint`。普通自动路径继续使用相同 traits、标准
less/greater、allocator 相等和严格不交叠区间的条件，experimental compact 的普通选择策略保持。

## 同层边缘规范化

比较两树高度后，沿高树的首 child 或末 child 下降到低 root 所在层。两种方向和两种高树归属
共用一个运行时方向内核。高树中匹配到的节点已有合法 non-root 占用。

- 低 root 为 inner 时，将它和边缘节点的 child/separator 按键顺序冻结到固定 typed 工作区。
  合计可放入一个节点时复用边缘节点，发布后退休低 root。否则将 child 均分到两个原节点，
  向父节点插入一个额外 child。新 non-root 至少有 `floor(N/2)+1` 个 child。
- 低 root 为 heap leaf 时，要求其达到 non-root 最小占用，且两叶合计超过一个 leaf 的容量。
  这时直接向父节点挂接原 leaf。其他叶形态执行既有元素转移路径。

整个过程保留叶内容和值对象。inner separator 使用现有 scalar-key 或借用 key-pointer 表示。
append 时原边缘子树的最小键保持。prepend 时下降路径上的 child 位置均为0，新的最小键沿该
spine 传播，父节点只需维护插入后右 child 的 separator。

## 分配与发布

在任何原图 metadata 改写前，先扫描父 spine 的连续满节点，预分配其 split sibling，必要时
再预分配新 root。未发布的新 inner 借用自身 `m_parent` 字段形成临时链，工作区使用固定数组。
因此分配和构造 hook 观察完整旧两树。

随后只进行 typed metadata 写入。满父节点在原节点和预分配 sibling 间均分，向上继续插入。
遇到空位或创建新 root 后，统一发布 root、size、叶链，并将 source 置为空且可复用。
多余低 root 的销毁与释放发生在发布之后，hook 此时观察完整目标和空 source。
allocator 实例分别保留，比较、分配及值操作沿用既有不抛出域，hook 可读但不能重入修改两树。

## 选择成本

普通 checked 入口先用 inner root 的首末 separator 区间排除确定重叠的输入，然后检查真实
叶端点。separator 区间位于整树键区间内，因此该早拒绝同样适用于不等高 overlap 和 sparse
输入。完整高度扫描只发生在严格不交叠区间通过之后。

工作量受高度差和节点容量约束。原节点及值对象直接进入结果，额外节点请求最多为连续满
spine 的长度加新 root。该路径覆盖固定大小有序分片持续进入增长索引的负载。
