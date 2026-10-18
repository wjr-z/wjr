# Tensor owner 存储与 API 契约

本轮在 [初始数据模型](tensor-data-model.md#owner-的初步结构决策) 的 Container + mapping
决策上收敛首版 owner。`<wjr/np/tensor.hpp>` 已实现下述连续存储子集和 checked factory，
通过可选 `wjr::np` target 使用；[eval](tensor-evaluation.md) 已提供拥有结果的物化。
本文主要说明 packed 基线，跨布局契约及正 stride owner 的候选状态见[核心标准](np-core-standard.md)。
计算继续接收现有 tensor_view。

## 复用结论

首版组合已有容器与固定 Kokkos mapping，不直接 alias 或公开继承后端的实验性 mdarray。
不另建 allocator、buffer 或共享控制块；默认采用 `np::tensor_vector<T>`。
这是复用 `wjr::basic_vector` 的薄 alias，仅将默认 storage 的移动后契约增强为可查询、复制的空表示，
不增加字段，保留原 allocator 和 trivial-relocation 策略。已有显式 `default_construct` 构造
可合法开始数值对象生命周期而不要求先写初值，
供拥有结果的 evaluator 使用。普通构造和 `make_tensor(shape)` 继续值初始化；
显式 `std::vector<T>` 和全静态 `std::array<T, N>` 仍受支持。
选择依据是可用的初始化语义，并非容器名称或尚未测量的吞吐量优势。
默认容器改变了公开类型；采用标准 vector 的代码应显式写出该 Container。
owner 的条件 `noexcept` 仍跟随容器；当前 WJR vector 的移动赋值因底层 deallocate
声明而保守地不是 nothrow，不保留原标准 vector 的该项 trait。

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
该版本的[默认容器](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/core/xtensor_config.hpp#L51)
实际为 `uvector<T, A>`；其 [safe_init_allocate](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/containers/xstorage.hpp#L160)
对满足其 trivial-default-construction trait 的元素省去构造循环。
WJR 复用已有 vector 的 default-initialization 路径，不照搬 uvector 的 allocator 行为。
它的 [xstrided_container](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/containers/xcontainer.hpp#L326)
同时保存 shape、strides、backstrides 和 layout，支持比首版连续 owner 更广的操作。
本轮不引入其 owner/表达式依赖；现有 WJR evaluator 已采用更窄的静态值树。
xtensor 仍是设计和同语义对照输入，不是性能上限，也不因成熟而要求替换当前实现。

## 类型与存储

首版模板形态为：

```cpp
template <class T, class Extents, class Layout = layout_right,
          class Container = tensor_vector<T>>
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
| vector 类动态存储 | `allocator_type`、`C(allocator)`、`C(count, allocator)`、`get_allocator()`；data/size/max_size；所用复制、移动、赋值操作 |
| 显式 `std::array<T, N>` | Extents 全静态，N 在编译期等于逻辑元素数；通过值初始化构造对象 |

共同要求 `data()`/`data() const` 分别精确返回 `T*`/`const T*`，
元素引用分别为 `T&`/`const T&`，`data()[i]` 与容器第 i 个对象相同。
不能只用 `std::ranges::contiguous_range` 约束而意外排除 WJR 当前迭代器。
`data()`、`size()`、`max_size()` 及其 const 观察操作不得抛异常。
销毁必须安全；失败的容器操作须留下仍可查询 data/size、析构和重新赋值的有效容器表示。
这些语义要求不能单凭 requires 表达式证明。

`std::vector<bool>` 不符合真实指针和引用要求，明确拒绝实例化，不降级为位代理。
默认 `tensor_vector<bool>` 使用真实 `bool` 对象，可直接提供 bool owner 与 const view。
普通 `wjr::vector`、`fixed_vector` 和零内嵌容量的 `small_vector` 的移动源
只保证既有的有限 storage 契约，不能用于 owner 强化的移动后观察与复制。
这些已知配置因此在 owner 约束中拒绝；显式 allocator 存储应使用
`tensor_vector<T, Allocator>`。这是修正此前仅语法可接受、移动后却不满足 owner 契约的适配，
不泛化拒绝其它 basic_vector 或用户容器。
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
数值元素的值初始化得到零；显式 default_construct factory 提供合法默认初始化，见下文，不能先读未写的算术值。
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
返回引用的 mapping/extents 观察接口同样拒绝右值，避免借出已销毁对象的元数据。
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
调用形式如下：

```cpp
using panel = tensor<double, extents<std::size_t, dynamic_extent, 300>>;
auto result = make_tensor<panel>(std::array<std::int64_t, 2>{days, 300});
// metadata 错误：wjr::expected<panel, shape_error>；容器分配异常仍可传播。

auto adopted = adopt_tensor<panel>(shape, std::move(existing_container));
auto allocated = make_tensor<panel>(shape, std::allocator_arg, allocator);

// 开始全部元素的生命周期；普通数值元素必须先写后读。
auto overwrite = make_tensor<panel>(shape, wjr::default_construct);
auto with_allocator = make_tensor<panel>(shape, wjr::default_construct,
                                         std::allocator_arg, allocator);
```

shape 总是含 rank 个值，静态维度也必须与类型匹配。
成功采用已有容器前检查其 size 精确相等，然后才移动它；不默默 resize 或复制补齐。
factory 的元数据错误使用已有 `wjr::expected` 返回，区分非法 extent、静态不匹配、算术越界、
`container_size_limit` 和 `storage_size_mismatch`。失败的 shape/长度检查不移动传入的容器。
这不是一个将所有 OOM 转成状态码的接口；Container 的分配、元素构造或移动异常保留原语义。
无异常配置不能因这个设计被宣称具有统一、可恢复的 OOM 机制。

`default_construct` 工厂仅接受具有 `C(count, default_construct, allocator)` 语义的容器，
不对 `std::vector` 或 `std::array` 提供悄悄值初始化的 fallback。
外部容器必须保证 tag 表示默认初始化，能匹配构造表达式本身不足以证明该语义。
标准 allocator 下，WJR vector 对数值类型复用 `std::uninitialized_default_construct_n`；
非平凡类型仍执行必要构造，自定义 allocator 的 `construct`/`destroy` 钩子保留。
因此该 tag 不承诺任意元素或 allocator 都没有初始化写入。
普通默认构造的非空静态 shape 同样先核对默认 allocator 的 max_size，
超限使用既有异常策略报告 `std::length_error`，不把超限请求交给 allocator。

检查顺序固定为：

1. 在原整数类型上检查每个 extent 非负、可表示为 index_type，核对静态值；不先窄化。
2. 用除法上界等检查乘法，再计算乘积，覆盖后端所需的 prefix/suffix stride。
   尚未构造 mapping，不借助可能已溢出的 required_span_size() 做校验。
3. 检查逻辑元素数可表示为容器 size_type，字节数可表示为 size_t，
   并满足首版的 PTRDIFF_MAX 字节跨度上界和实际 Container/allocator max_size。
4. 创建 extents/mapping；按选定初始化方式创建相应数量的活对象，或采用已验证容器。
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
实现以同一 allocator 值构造空容器查询 `max_size()`，再按长度构造值初始化存储；
空容器不分配元素，不作为永久成员保存。无 allocator 参数的 factory 要求 allocator 可默认构造。
显式 `std::array` 使用值初始化，不接受 allocator 重载。非默认构造元素可通过 `adopt_tensor` 引入。
factory 通过私有构造 key 直接在 expected 内创建 owner：新 array 不额外移动元素，
adopt 只执行一次 Container 移动；不先创建 owner 临时对象再整体移入 expected。
不在外层保存第二份 allocator，也不转发无约束的任意构造参数包。
首版不提供 resize/reserve、可写 storage、任意 mapping 或表达式赋值；
同类型赋值仍可改变动态 shape，这是替换 owner 的值，须遵守下面的提交顺序。

## 复制、移动与异常

本节 packed mapping 属于已校验的 layout_left/right，要求其复制/赋值不抛异常。
正 stride 扩展同样要求无抛提交，并把 has_storage 的比较对象改为 required span；不能拿逻辑 size 检验有孔存储。
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

以下是设计阶段的外部原型证据；首版正式实现的验证范围列在下一节。
原型使用上述固定 mdspan/xtensor 源码，xtl 0.8.0 固定为
[`ef84e9f27020ad54961c99acd293d80d4b775dd3`](https://github.com/xtensor-stack/xtl/tree/ef84e9f27020ad54961c99acd293d80d4b775dd3)，
不修改它们；这些原型当时没有加入 WJR 公共目录。
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

## 首版实现与验证

正式测试覆盖静态/动态/空/rank-0、row/column 布局、const 借用、右值借用拒绝、
静态 mapping 压缩、checked 创建/采用，以及不完整表示的传播和恢复。
状态 allocator 用例验证 max_size、复制构造的 allocator 选择、分配失败和不等 allocator 的移动；
抛异常元素覆盖部分构造清理及移动失败后的元数据保留。
默认存储现为 tensor_vector；普通值初始化、显式默认初始化、真实 bool、非平凡元素构造、
allocator 钩子及部分构造回滚均有针对回归，显式标准容器仍保留原语义。
NP 专用 storage 移动后清空全部三指针，源对象的 size/capacity 与 owner 存储观察保持有效。
普通 WJR vector 仍只清空 data，保留原本只保证销毁/有限存储操作的移动后契约，
不为 tensor 增加额外两次存储。它留下的 end/capacity 不可供 owner 的移动后查询或复制使用。
复制赋值的 `noexcept` 同时计入元素复制，允许分配异常正常传播；
范围赋值增长路径在元素构造失败后只释放新分配，已构造前缀由原有 uninitialized 算法销毁。

复用 vector 仍保留三指针（data/end/capacity），所测 x64 下为 24 字节；
动态二维 owner 另有 16 字节 extents，全静态 holder 不占额外字节。
全静态 exact-size owner 理想上只需 data 和 allocator，当前仍保存 end/capacity；
动态 owner 的 mapping 已含逻辑长度，vector 的 end 也重复表达了它。
本轮以现有容器的维护、allocator 与异常语义换取这部分常量状态，尚未达到 owner 的空间下界；
exact-size buffer 或整合 owner/storage 是后续独立设计，不在本轮另建 allocator-aware buffer。
`allocate_at_least` 也可能返回超过逻辑 size 的容量，容器必须保存它以正确释放。
这些是当前复用方案相对于 exact-size 存储的空间成本，不能把无初始化写入推广成所有维度的零开销。

本地 Clang 18 的 C++20/debug-3、C++23/debug-0、C++20 ASan，
MSVC 19.42 的 C++20/debug-3，以及仅链接已安装 wjr::np 的 consumer 通过。
GCC 12/MinGW 的独立程序覆盖创建、移动、空/rank-0 和宽整数 shape 拒绝；
本地该 GCC 的线程模型不支持 GTest 所需 std::mutex，未据此声称全套 GCC/Linux 已运行。
现有 range-v3 头仍阻止直接 -fno-exceptions；使用另一独立改动的标准 ranges 头后，
owner 的无异常创建/借用/元数据错误路径可编译运行，不将其声明为当前主线完整无异常支持。

Clang 18 Release 下，静态 vector/array owner 的借用与索引分别与直接使用相同常量 mapping
的写法生成相同指令；固定 shape 创建只有一次元素分配，空容器 max_size 探针不引入额外分配。
这些检查支持所测包装路径的空间与生成码目标，不是吞吐量结论或通用零成本保证。
连续预分配求值、广播、有限切片与表达式融合已实现；本节历史尺寸/生成码证据不因此自动覆盖后来所有路径。
泛切片、exact-storage 与更广后端复用按[实施计划](expression-roadmap.md)的证据门槛推进。
