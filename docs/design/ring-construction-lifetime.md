# ring buffer 构造失败与分段对象寿命

ring buffer 的构造函数直接建立 allocator 和 storage 成员。
元素构造失败时，整个 ring buffer 尚未完成构造，其析构函数不会执行。
因此 uninitialized 算法回滚对象前缀后，还必须释放该次构造已经取得的存储。
局部分配守卫承担此责任，不为 ring buffer 增加字段。

存储建立沿用 [构造异常契约](construction-lifetime.md)：
`Storage::uninitialized_construct` 失败时保持目标原状；成功后才由分配守卫接管。
内置 default/fixed storage 都先完成分配再提交元数据。
自定义存储不能发布指针、容量或 size 后再抛出并把释放责任交给调用者。

wrapped source 的复制需要两个连续的 uninitialized copy。
第二段失败时，其算法只回滚第二段的成功前缀；外层前缀守卫负责已经完成的第一段。
随后分配守卫释放存储，避免泄漏第一段对象或把完整目标 size 当成已构造对象数。

单遍 input 构造通过 emplace 逐步建立对象；其局部守卫在失败时销毁已提交对象并释放容量。
这一入口直接使用的增长慢路径也保护新分配、末尾元素和移动前缀。
allocator 没有 construct/destroy 定制且重定位不会抛出时，保持现有重定位路径。
可能抛出的元素移动或 allocator hook 则先完成两段移动，再销毁旧对象。
失败后旧对象的生命周期、size 和容量仍有效，但元素值可能已移出，不承诺强异常保证。
没有给普通 emplace 成功分支增加检查，也没有改写其他 resize、insert 或 assign 路径。

回归覆盖值、默认、填充、复制、带 allocator 复制、forward 区间、input 区间和
initializer-list 八种构造，包括空区间、成功路径、元素/hook 抛出及分配失败。
wrapped 增长还在第一段、分段边界及第二段注入失败。
测试登记成功构造的合法地址，核对存活对象和分配/释放、construct/destroy 的配对。

Windows x64、baseline、DEBUG0 的外部代码生成对照使用 Clang 18/GCC 12 的
`-O3 -march=x86-64` 和 MSVC 19.42 的 `/O2`。
将分配守卫缩到分配成功之后，Clang int fill/range 恢复原来的内联及指令，
GCC int value/fill/range 指令保持一致，MSVC 对应构造指令仅改变临时变量编号。
普通 append、索引和 size 探针未增加指令；复制构造及其他实例化仍可能改变代码生成。
没有做吞吐量或硬件计数器基准，也不承诺所有类型和编译器严格零开销。
