# C++20 iterator 的关联类型与连续访问

`iterator_difference_t` 和 `iterator_reference_t` 分别使用标准的
`std::iter_difference_t`、`std::iter_reference_t`。因此，不提供完整 legacy
`iterator_traits` 成员的 C++20 输入迭代器也能查询差值和实际解引用类型；
解引用返回代理或值时，不强制把它当作左值引用。

`iterator_value_t` 优先使用 `std::iter_value_t`，保留标准
`indirectly_readable_traits`、`incrementable_traits` 和显式 `iterator_traits`
的定制规则。传统输出迭代器可能只有 `value_type = void`，没有标准关联值类型；
此时保留旧的 `iterator_traits::value_type` 回退。参见
[关联类型规则](https://eel.is/c++draft/iterator.assoc.types)。

`iterator_pointer_t`、`iterator_category_t` 及现有 input/output/category traits
继续使用 legacy 分类。关联类型升级不代表所有 WJR 容器接口已接受全部 C++20
输入迭代器；例如仅有 `void` postfix increment 的输入游标仍没有 legacy category。

## 连续访问的边界

`is_contiguous_iterator_v` 同时接受标准 `std::contiguous_iterator` 和现有 range-v3
连续迭代器，保留数组和 trivial 元素 move iterator 的 WJR 扩展。
扩展不代表该类型满足标准概念。`void*` 和函数指针没有连续对象元素，结果为 false；
`const` 和 `volatile` 对象指针的元素限定保持。非 trivial 元素的 move iterator
不会被解包成普通指针，以免丢失移动语义。

WJR 自身的 contiguous tag 仍保留 range-v3 定义。两套 tag 的直接替换或双继承会破坏
现有识别，具体证据见 [span 互操作](span-interop.md)。标准迭代器满足
[连续访问要求](https://eel.is/c++draft/iterator.concept.contiguous)，可以通过
`std::to_address` 提取对应元素地址，不需要新增虚接口或共享所有权。

## 解包之后恢复输出状态

release 路径可以将连续迭代器解包为指针，但 `copy` 和 `copy_n` 必须通过已有
`_iter_wrap` 恢复返回类型及状态。否则已有 `std::vector::iterator` 会因 raw pointer
不能隐式转换而编译失败。新接纳的 `std::counted_iterator` 还必须保留剩余 count，
不能只返回正确地址。`move`、`move_n` 复用同一路径。

`uninitialized_copy` 和 relocation 已有恢复路径。relocation 的两个 bulk-copy
分支现在显式取 `to_address`，避免 debug level 3 保留迭代器包装时无法调用 memcpy。
没有调整分支选择、添加运行时 dispatch 或改变空范围的既有提前返回。
测试同时验证返回 base/count、空范围、非 trivial 移动、cv 元素和检查模式。
`counted_iterator` 的状态契约见 [标准适配器定义](https://eel.is/c++draft/counted.iterator)。
