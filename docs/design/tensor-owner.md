# Tensor owner 存储与 API 契约

本轮在 [初始数据模型](tensor-data-model.md#owner-的初步结构决策) 的 Container + mapping
决策上收敛首版 owner。这里只确定契约，公共 owner、checked factory 和 evaluator 尚未实现。
计算继续接收现有 `tensor_view`；这一决策不引入第二套表达式引擎。

## 复用结论

首版组合已有容器与固定 Kokkos mapping，不直接 alias 或公开继承后端的实验性 mdarray。
不另建 allocator、buffer 或共享控制块；默认采用 `std::vector<T>`。
`wjr::vector<T>` 保留为显式存储选择，其扩容、异常和 allocator 行为须独立验收后再考虑替换默认值。
本轮没有测得两者吞吐量差异，不以本库容器身份作为默认选型依据。

[P1684R5](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p1684r5.html)
提供容器适配器的设计依据，也提出通过实际容器长度判断移动后能否访问数据。
截至本轮核对，`wg21.link/p1684` 仍指向 R5。
后续 [P3308R0](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p3308r0.html)
讨论了 `in_place_t` 构造和更窄的构造集合；不将讨论当作已完成的标准接口。
WJR 首版采用少量命名 factory，避免同时转发所有容器构造重载。

固定 mdspan 快照 `0e4eba59ad53451784b3839bdf98b25fa2d64931` 中的 mdarray
可作行为对照，但与本库所需契约有以下差异：

| 固定源码接口 | 核对结果 | WJR 边界 |
| --- | --- | --- |
| [container()](https://github.com/kokkos/mdspan/blob/0e4eba59ad53451784b3839bdf98b25fa2d64931/include/experimental/__p1684_bits/mdarray.hpp#L368) | 返回可写容器引用；clear/resize 后 mapping 不随之改变 | 不暴露可改变存储长度或 allocator 的容器引用 |
| [to_mdspan()](https://github.com/kokkos/mdspan/blob/0e4eba59ad53451784b3839bdf98b25fa2d64931/include/experimental/__p1684_bits/mdarray.hpp#L434) | 没有 ref qualifier；临时 owner 可产生借用描述符 | 所有借用入口删除 `&&` 和 `const &&` |
| [默认赋值](https://github.com/kokkos/mdspan/blob/0e4eba59ad53451784b3839bdf98b25fa2d64931/include/experimental/__p1684_bits/mdarray.hpp#L246) | 先赋 mapping，再赋容器；后者抛异常时元数据可能已变 | 容器操作成功后再无抛提交 mapping |
| size() | 返回容器长度，移动后可能与逻辑 shape 不同 | 分开提供逻辑 size() 与 storage_size() |

薄包装 mdarray 仍需拦截借用、隐藏修改入口、替换赋值并另行校验 shape，
省下的主要只是已有的两成员组合。因此直接复用成熟容器和 mapping，
只实现这层额外契约，不修改上游源码。

xtensor 0.27.1 固定为
[`f115228a7fc277b9be1bef34572831f8fd53ac73`](https://github.com/xtensor-stack/xtensor/tree/f115228a7fc277b9be1bef34572831f8fd53ac73)。
其 `xtensor_container<std::vector<T>, rank, row_major>` 是相同底层存储的比较对象。
它的 [xstrided_container](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/containers/xcontainer.hpp#L326)
同时保存 shape、strides、backstrides 和 layout，支持比首版连续 owner 更广的操作。
本轮不引入其 owner/表达式依赖；后续 evaluator 仍须比较可复用的 xtensor 子集，不能据此先实现替代引擎。

## 类型与存储

首版模板形态为：

```cpp
// 设计签名，尚无公共实现。
template <class T, class Extents, class Layout = layout_right,
          class Container = std::vector<T>>
class tensor;
```

- `T` 是完整、非 cv 的对象类型，与 `Container::value_type` 完全相同；不接受引用或代理元素。
- `Extents` 复用当前后端，rank 固定在类型中，保留全部静态 extent。
- 首版 `Layout` 只接受 `layout_right` 或 `layout_left`，不接受 padding、负 stride 或任意 stride。
- 容器独立拥有全部已构造对象，仅动态 shape 保存实际 mapping。
  外层使用 `compressed_pair<mapping_holder, Container>`，不重复保存 data、size、capacity 或 allocator。
- 不增加 moved-from 标志、运行时 dtype、accessor 状态或引用计数。
  view 使用现有默认 accessor；自定义访问策略属于借用适配层。

`mapping_holder` 只为这里允许的两种布局分成两种情况：有动态 extent 时持有实际 mapping；
全静态时为空类型，通过 `inline static constexpr mapping_type` 提供共享的常量 mapping。
两者统一返回 `const mapping_type&`，全静态的 mapping 不必在每个 owner 中重复保存。
实例化共享 mapping 前先在编译期验证全静态 shape 和 stride 运算。
`compressed_pair` 通过 EBO 压缩这个空 holder，无需修改上游内部属性宏，
也不扩展成通用 storage policy。mapping 的地址不代表某个 owner 的身份。
此选择不改变现有 tensor_view 的表示；借出 view 时仍按后端接口复制必要描述符。

容器有两种首版存储配置，不建立通用 storage policy 框架：

| 配置 | 必需契约 |
| --- | --- |
| vector 类动态存储 | 空构造、按检查后的长度创建值初始化对象；data/size/max_size；所用复制、移动、赋值操作 |
| 显式 `std::array<T, N>` | Extents 全静态，N 在编译期等于逻辑元素数；通过值初始化构造对象 |

共同要求 `data()`/`data() const` 分别精确返回 `T*`/`const T*`，
元素引用分别为 `T&`/`const T&`，`data()[i]` 与容器第 i 个对象相同。
不能只用 `std::ranges::contiguous_range` 约束而意外排除 WJR 当前迭代器。
`data()`、`size()` 及其 const 观察操作不得抛异常。
销毁必须安全；失败的容器操作须留下仍可查询 data/size、析构和重新赋值的有效容器表示。
这些语义要求不能单凭 requires 表达式证明。

`std::vector<bool>` 不符合真实指针和引用要求，明确拒绝实例化，不降级为位代理。
首版默认存储因此只覆盖非 bool；真实 bool owner 显式指定合规容器，例如 `std::array<bool, N>`，
或经对应操作验收的 `wjr::vector<bool>`。本轮原型验证了后者的真实 bool 分配、读写与 const view，
没有据此声称已完成全部 allocator/赋值测试。
过对齐类型由 Container/allocator 保证对齐；owner 不额外承诺超出 T 所需的 SIMD 对齐。

## 逻辑 shape 与有效存储

定义 `logical_size` 为已经验证可表示的 extents 乘积。
当前两种连续布局下它等于 `mapping.required_span_size()`。
定义：

```cpp
bool has_storage() const noexcept {
    return storage_size() == logical_size;
}
```

其中 logical_size 从经过校验的 mapping 计算，不新增缓存字段。
容器 size 记录实际已构造对象数；capacity 可以更大，超出 size 的槽位仍未构造。
成功的新建和恢复操作必须满足严格相等，不能用更大的容器掩盖 shape 不匹配。

| 情况 | 默认 shape | 逻辑大小与初始化 |
| --- | --- | --- |
| 有动态 extent | 动态 extent 为 0，静态 extent 不变 | 逻辑大小 0，容器为空，has_storage 为 true |
| 全静态非零 shape | 类型声明的 shape | 构造对应数量的值初始化对象；默认 vector 可能分配 |
| rank-0 | 空维度列表 | 一个值初始化标量，逻辑大小 1；不是空 owner |
| 存在零 extent | 保留该 shape | 逻辑大小 0，合法空 view 可以携带空指针 |

默认构造同样先满足静态运算约束，再创建存储。
数值元素的值初始化得到零；首版不提供未初始化存储捷径。
静态 shape 也不自动改用内嵌数组，避免大数组无意进入对象或栈。

移动或容器操作失败后，允许得到 **mapping 有效但存储不完整** 的表示。
`size()` 仍返回逻辑大小，`storage_size()` 返回实际对象数；两者的差异可由 has_storage 查询。
不改写静态 extent 来伪造空 shape，也不额外分配只为保持移动源的存储。
例如，静态 shape [2,3] 的 vector 被移走后通常为逻辑大小 6、存储大小 0；
array 的元素移动后仍有 6 个对象，可能继续满足 has_storage，但元素处于各自移动后状态。
这些结果遵循容器语义，不要求所有移动源一律为空。

不完整表示允许查询、析构、复制/移动表示、赋值和恢复，不能作为 tensor 数据或表达式输入。
复制这种状态只复制 mapping 以及 Container 中实际存在的对象；
只有源 has_storage 时才承诺完整 tensor 值的深拷贝。
再次移动不完整 owner 合法，目的端也可能不完整，不新增异常或隐含分配。

## 借用和最小接口

owner 提供 extents、mapping、逻辑 size/empty、storage_size 和 has_storage 观察接口。
mapping 只返回 const 引用；不公开可写 container/storage 引用。
首版通过以下两个入口借用数据：

```cpp
view_type view() & noexcept;
const_view_type view() const & noexcept;
std::optional<view_type> try_view() & noexcept;
std::optional<const_view_type> try_view() const & noexcept;
// 上述四个入口的 && / const && 重载均删除。
```

`view()` 的明确前置条件是 has_storage()；调试断言辅助发现违反，Release 不负责恢复该状态。
不确定状态的调用方使用 `try_view()`：不完整时返回 nullopt，否则返回普通轻量描述符。
optional 不分配，也不持有 owner 或延长底层对象寿命；检查只发生在借用边界。
合法空 shape 返回有值的空 view，不能与 nullopt 混淆。

普通热循环仍使用 `tensor_view`，不逐元素检查 owner 的存储状态。
若后续添加 owner 的 `operator()`，它也必须具有相同的 has_storage 前置条件；
首版不需要靠重复 owner 索引接口来执行计算。
const owner 借出 `view<const T>`，而 const 描述符本身仍遵循现有 mdspan 的浅 const 规则。
不提供 owner 到 view 的隐式转换，也不把 owner 注册为 borrowed_range。

owner 销毁或可能替换存储的操作使先前借用失效；即使某次容器移动保留了地址，首版不额外承诺 view 继续有效。
从一个仍存活的左值获得 view 后返回它，仍需调用方保证存储寿命；删除右值重载不能证明所有借用安全。

## checked 创建边界

首版使用命名 factory 接收固定 rank 的完整整数 shape，避免不可信数据先进入后端 extents/mapping。
建议的调用形式如下；实现 MR 必须以约束和错误测试确认这些签名：

```cpp
using panel = tensor<double, extents<std::size_t, dynamic_extent, 300>>;
auto result = make_tensor<panel>(std::array<std::int64_t, 2>{days, 300});
// metadata 错误：wjr::expected<panel, shape_error>；容器分配异常仍可传播。

auto adopted = adopt_tensor<panel>(shape, std::move(existing_container));
```

shape 总是含 rank 个值，静态维度也必须与类型匹配。
成功采用已有容器前检查其 size 精确相等，然后才移动它；不默默 resize 或复制补齐。
factory 的元数据错误使用已有 `wjr::expected` 返回，区分非法 extent、静态不匹配、算术越界和存储长度不匹配。
这不是一个将所有 OOM 转成状态码的接口；Container 的分配、元素构造或移动异常保留原语义。
无异常配置不能因这个设计被宣称具有统一、可恢复的 OOM 机制。

检查顺序固定为：

1. 在原整数类型上检查每个 extent 非负、可表示为 index_type，核对静态值；不先窄化。
2. 用除法上界等检查乘法，再计算乘积，覆盖后端所需的 prefix/suffix stride。
   尚未构造 mapping，不借助可能已溢出的 required_span_size() 做校验。
3. 检查逻辑元素数可表示为容器 size_type，字节数可表示为 size_t，
   并满足首版的 PTRDIFF_MAX 字节跨度上界和实际 Container/allocator max_size。
4. 创建 extents/mapping；创建相应数量的值初始化对象，或采用已验证容器。
   仅在这些步骤全部成功后发布完整 owner。

空 shape 的总元素数为 0，并不能豁免 mapping 的中间算术。
例如 int 索引的 [0,46341,46341] 仍可能在 layout_right 的 stride(0) 查询时溢出；
[46341,46341,0] 在当前后端逐项累乘 required_span_size 时也会先溢出。
首版保守检查每段不含零的连续维度乘积可表示，拒绝这些外部 shape；
[46341,0,46341] 则无此中间乘积问题且不需分配。
所有 extent 本身的合法性仍须逐项检查，不能遇到第一个零就提前接受。
rank-0 的空乘积为 1，仍必须通过标量所需的字节数和容量检查。

allocator 定制通过 Container 类型及有明确 allocator tag 的 factory 重载传入。
检查 max_size 和实际创建容器须使用同一 allocator 状态；
不在外层保存第二份 allocator，也不转发无约束的任意构造参数包。
首版不提供 resize/reserve、可写 storage、任意 mapping 或表达式赋值；
同类型赋值仍可改变动态 shape，这是替换 owner 的值，须遵守下面的提交顺序。

## 复制、移动与异常

mapping 属于已校验的 layout_left/right，要求其复制/赋值不抛异常。
复制构造和移动构造分别使用容器自身对应构造；复制 allocator 的选择交给容器，
不另行绕过 select_on_container_copy_construction。

复制及移动赋值都先处理 self-assignment，再执行 Container 赋值，成功后才复制 mapping。
自赋值保持不变，包括当前为不完整状态时；不盲目调用 WJR vector 有额外契约的自移动路径。
noexcept 按实际 Container 与 mapping 操作推导，不能无条件标记移动为 noexcept。
不完整源的表示同样按此规则传播，不因逻辑大小较大就读取容器之外的对象。

容器操作抛异常时，目标保留旧 mapping，容器处于其承诺的有效失败状态；
移动源也保留原 mapping，实际元素和存储按容器语义处理。
双方都可重新查询 has_storage，不完整时 try_view 返回 nullopt，重新赋值成功后恢复。
不承诺所有 allocator/T 都有强异常保证：
POCCA 为 true 且 allocator 不等时，容器可能先释放旧存储，随后新分配失败；
POCMA 为 false 且 allocator 不等时，移动可能重新分配并移动元素，且可能抛出。
即使 allocator 相等，元素复制失败也不能一概推断容器保持旧长度。

本地原型证明“仅把 mapping 提交放最后”不足以保持完整存储，因此需要上述可查询状态契约。
若 Container 对某次操作保证目标完全不变，mapping 的无抛后提交使 owner 继承该保证；
除此以外只承诺这里列出的有效表示，不凭空提升容器的异常保证。
不为此引入 heap control block 或通用事务框架。
首版不新增 owner 专用 swap；后续若需要强保证提交，必须先证明容器交换无抛，
并满足 allocator 可传播或相等的条件，不能只看 std::vector::swap 的 noexcept 声明。

## 外部原型证据与实现门槛

原型使用上述固定 mdspan/xtensor 源码，xtl 0.8.0 固定为
[`ef84e9f27020ad54961c99acd293d80d4b775dd3`](https://github.com/xtensor-stack/xtl/tree/ef84e9f27020ad54961c99acd293d80d4b775dd3)，
不修改它们；WJR 公共目录没有加入 owner/evaluator 原型。
Clang 18.1.8/MSVC STL 的 C++20、C++23、C++20 ASan，以及 MSVC 19.42 的 C++20 Release 均通过。
这些配置都在 x64 Windows，未声称已完成 Linux 或其它 STL 验证。

有针对性的结果如下：

- mdarray 默认 rank-0 为 1 个零初始化 int，全静态 [2,3] 为 6 个，动态默认为空。
- 清空 mdarray.container 后，required span 为 6 而 storage size 为 0；临时 to_mdspan 调用被接受。
- mdarray 的元素复制异常后，目标 required span 为 6、实际存储为 0。
  xtensor 使用相同 std::vector 存储的复制异常后，shape 乘积为 6、实际存储也为 0。
- 容器先赋值的组合原型在传播 allocator 后分配失败时保留旧 shape，has_storage 为 false；
  后续赋值恢复。不同 allocator 的移动分配失败、元素移动异常也有独立用例。
- 不完整表示的复制和再次移动保持可查询；const view、右值借用拒绝、空 shape 与 rank-0 的 try_view 区分均通过。
- 算术原型在构造 mapping 前拒绝负 extent、窄索引乘积越界、容量不足，以及上述含零 stride 越界。

相同 std::vector<int> 存储、动态二维 shape、Release 下的尺寸如下，单位为字节：

| 表示 | Clang 18 / MSVC ABI | MSVC 19.42 |
| --- | ---: | ---: |
| std::vector<int> 本身 | 24 | 24 |
| mapping + std::vector 的 compressed_pair 原型 | 40 | 40 |
| 窄 holder + std::vector 的动态二维原型 | 40 | 40 |
| 固定 Kokkos 实验性 mdarray | 40 | 40 |
| xt::xtensor_container<std::vector<int>, 2, row_major> | 104 | 104 |

xtensor 的额外状态支持其更通用的 strides、迭代和表达式能力，尺寸比较不等价于功能替换或吞吐量结论。
相同 ABI 下，直接组合全静态后端 mapping 与 std::vector 为 32 字节，
与 std::array<int,6> 为 28 字节。采用上述空 mapping holder 后，两者分别为 24 和 24 字节，
与相应 Container 本身相同；动态二维仍为 40 字节。
两种编译器均验证了这些尺寸及 row/column 索引，不能推广成跨编译器 ABI 承诺。
Clang 18 C++20 Release 下，静态 vector/array owner 的索引分别与直接地址计算生成相同指令；
借用描述符的生成也分别与直接使用相同常量 mapping 的写法一致，共四组探针。
这些结果支持消除所测 owner 的冗余静态状态，没有证明吞吐量提升，
也没有改变当前后端静态 view 在 MSVC ABI 下仍为 16 字节的限制。

实际 owner 实现 MR 还需把这些边界变成正式测试，补齐 Linux GCC/Clang、无异常编译、
状态 allocator 与部分构造清理，并比较相同输入的布局及借用索引生成码。
只在这些契约通过后加入连续预分配输出求值；广播、切片、表达式融合与 xtensor evaluator 复用仍属后续独立决策。
