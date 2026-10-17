# 容器构造失败时的对象寿命

vector、small_vector 和 inplace_vector 完成构造后，size 对应已构造的对象。
批量构造期间可以预设目标 size，但必须由局部守卫负责失败回滚。
不在逐元素循环中更新容器 size，也不为此给容器增加字段或共享控制块。

委托构造完成基础对象后，后续元素构造失败会调用容器析构。
旧实现预先将 size 设为目标长度；标准 uninitialized 算法已经回滚成功前缀后，
容器析构又按完整 size 销毁元素，会重复析构已回滚对象并访问从未构造的对象。
局部守卫在失败时先把 size 清零，让容器析构只释放已有容量；成功后解除守卫。
守卫必须在申请存储前建立：small_vector 存储会先设置 size，再尝试分配，
因此分配失败也需要清零。沿用成功路径预设 size 的写入顺序，
避免在批量复制或初始化后新增一次提交，妨碍原有的尾调用和内联。

## uninitialized 算法的责任

批量构造函数负责在异常时销毁其成功前缀，随后原样传播异常。
标准算法已有这一责任；自写的 iterator/sentinel 或 allocator hook 循环也必须遵守。
手写循环使用一个局部前缀守卫，在全部成功后解除；不在每个元素上分配或添加状态标志。
守卫不持有存储，只记录输出游标；解除时清空游标引用，不再复制最终迭代器。
allocator 分支通过相同 allocator 销毁对象。
输出迭代器的复制、比较和递增须满足 uninitialized 算法的非抛异常前置条件。

只有 construct 和 destroy 都没有定制行为时，才能把异常清理交给原始标准算法。
只定制 destroy 的 allocator 仍应收到前缀回滚的 destroy 调用。
default_construct 的 allocator 路径直接执行无参数 construct，
不先创建一个临时 T 再复制或移动到目标，也不因 T 平凡就跳过用户的 construct hook。

不同类型的 iterator/sentinel 保留为一对；不能只把 counted iterator 解包成指针，
却仍用 default_sentinel 与该指针比较。输出结果继续使用现有的迭代器恢复规则。

这些契约覆盖构造及其使用的 uninitialized copy/default/value/fill 算法。
本轮没有宣称所有 resize、insert、append 或重分配操作都已有强异常保证，
也没有改写 relocation 的消费和失败语义。

## 验证范围

回归用合法对象地址集合核对销毁，不通过读取未构造对象的成员来判断失败。
覆盖普通 vector、small_vector 的内嵌/堆存储、inplace_vector，
默认/值/填充/复制/区间构造，以及元素或 allocator hook 在首个、中间、末尾抛出。
分配失败不应产生虚假的对象析构，成功和失败的分配/释放及 construct/destroy 必须配对。
另有 destroy-only allocator 和 counted/default_sentinel 的批量构造回归。

Clang 18/Windows ASan 在优化后的 GTest 异常测试中出现静默 trap，
完全不包含 WJR 的标准算法对照也能复现。
将测试 catch 保留在 noinline helper 中即可通过 -O3 ASan；
生产实现仍按原优化等级构建，没有关闭 sanitizer 或修改算法来规避测试。

代码生成对照使用 Windows x64、DEBUG0、baseline，Clang 18/GCC 12 的 -O3
及 MSVC 19.42 的 /O2，检查 int 的动态、内嵌、定长构造及独立批量算法。
标准 allocator 的独立 copy/fill/value 路径和已知容量 small/inplace 路径保持基线；
GCC 动态构造只在异常清理路径增加 size 清零。
仍有两类优化器差异：Clang 的动态 fill 构造跨过内联预算，改为调用已有构造 helper；
MSVC 的自定义 allocator 值初始化由 memset 变为标量清零循环。
不能只根据元素类型的 noexcept 跳过清理，因为 allocator hook 仍可能抛出。
本轮优先保证对象寿命正确，不据此宣称吞吐或分支预测改善。
