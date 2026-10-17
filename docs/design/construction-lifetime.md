# 容器构造失败时的对象寿命

vector、small_vector 和 inplace_vector 完成构造后，size 对应已构造的对象。
批量构造期间可以预设目标 size，但必须由局部守卫负责失败回滚。
不在逐元素循环中更新容器 size，也不为此给容器增加字段或共享控制块。

委托构造完成基础对象后，后续元素构造失败会调用容器析构。
旧实现预先将 size 设为目标长度；标准 uninitialized 算法已经回滚成功前缀后，
容器析构又按完整 size 销毁元素，会重复析构已回滚对象并访问从未构造的对象。
局部守卫在失败时先把 size 清零，让容器析构只释放已有容量；成功后解除守卫。
存储建立必须在失败时保持目标原状：small_vector 先完成分配，再提交指针、容量和 size。
分配失败时目标仍为空，容器析构无需再清零；元素回滚守卫因此在存储建立后才生效。
它只负责元素构造阶段的失败，不把分配失败也纳入元素清理区域。
沿用批量复制或初始化前预设 size 的成功路径，不新增完成后的 size 提交。

这是自定义 Storage 的明确异常契约：uninitialized_construct 失败时须保持目标不变，
不能预先修改 size 再让分配抛异常。内置存储满足此约束；扩展存储也须遵守。

## uninitialized 算法的责任

批量构造函数负责在异常时销毁其成功前缀，随后原样传播异常。
标准算法已有这一责任；自写的 iterator/sentinel 或 allocator hook 循环也必须遵守。
手写循环使用一个局部前缀守卫，在全部成功后解除；不在每个元素上分配或添加状态标志。
守卫不持有存储，直接保存起点和当前输出游标；循环推进其内部游标，
避免守卫引用外部游标带来的别名关系。成功后只解除清理，不再复制最终迭代器。
allocator 分支仍通过相同 allocator_traits 销毁对象。
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
缩短守卫覆盖范围后，Clang 动态 fill 恢复内联，与修复前的正常路径逐指令一致；
GCC 动态构造不再需要分配失败时的多余 size 清零，MSVC 的构造探针保持不变。
没有对整个构造函数强制内联，也没有引入运行时异常计数或额外检查。
这些结论仅覆盖所测类型和调用场景，不承诺所有实例化或编译器版本严格零开销。
前缀守卫持有自己的游标后，MSVC 自定义 allocator 值初始化恢复 memset；
相较不含回滚的旧实现，仍有栈帧和 call/return 成本，没有宣称完全消除。
Clang/GCC 的独立批量算法探针保持指令数量，未按元素 noexcept 猜测并跳过清理。
新增回归覆盖输入解引用、递增、sentinel 比较抛出，以及元素无抛但 allocator 定制抛出。
测试统一使用 allocator 成员定制，不特化当前标准库禁止用户特化的 allocator_traits。
