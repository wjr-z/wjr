# 拥有存储的正 stride tensor

`<wjr/np/strided_tensor.hpp>` 为现有 `tensor` 增加 `layout_stride` 配置。
owner 仍组合一个 mapping 和一个 Container，计算借用普通 `view()`；
不新增数据指针、逻辑大小缓存、共享所有权或每元素检查。

```cpp
using matrix = np::tensor<float, np::extents<size_t, 2, 3>, np::layout_stride>;
auto result = np::make_tensor<matrix>(std::array{2, 3}, std::array{8, 1},
                                     wjr::default_construct);
// result->size() == 6; required_span_size() == storage_size() == 11.
np::eval_to(result->view(), expression);
```

## 存储与生命周期

stride 以元素数计数，支持范围与[正 stride view](tensor-strided-view.md) 相同：
严格正数、满足后端排列约束、非重叠且可表示。负 stride、零 stride 和任意重叠布局不在此接口中。
无论 extents 是否全静态，运行时 stride 都保留在每个 owner 的 mapping 中。
连续布局的全静态 mapping 仍压缩为空 holder；不改变其表示和执行路径。

逻辑 `size()` 是 extents 的乘积。`required_span_size()` 是 mapping 从起点到最后
可访问位置所需的对象数，非空时为 `1 + sum((extent - 1) * stride)`。
rank-0 两者均为 1；空域两者均为 0。`storage_size()` 是容器内已构造对象数，
`has_storage()` 比较后两者，而不拿逻辑大小判断是否可借用。

factory 精确分配 span，不额外要求最后一行之后的 padding。span 内包括孔洞在内的对象
都合法开始生命周期、由容器统一销毁；不把未构造 capacity 当作对象存储。
默认调用值初始化整个 span。显式 `default_construct` 沿用容器的默认初始化路径，
对普通算术类型省去初值写入；非平凡构造、allocator construct/destroy hook 和异常清理仍执行。
未写入的算术位置不得读取。表达式求值只写逻辑元素，规约跳过 padding。
owner 的复制和赋值委托容器，可能读取整个 span，不能假设只复制逻辑元素。
采用 `default_construct` 后，在任何可能读取整个容器的操作前需初始化包括孔洞在内的全部位置；
只需要逻辑值的独立副本时，可用 `np::eval(np::as_expr(owner.view()))` 生成 packed owner。

`make_tensor<Owner>(shape, strides[, default_construct][, allocator_arg, allocator])`
在分配前复用同一 mapping 边界校验，再检查容器和 allocator 上限。
布局元数据失败返回 `shape_result`；分配或对象构造失败沿用 allocator/容器异常策略。
普通 view factory 复用该校验，未增加内层验证。

`adopt_tensor<Owner>(shape, strides, std::move(storage))` 要求容器长度精确等于 span。
元数据或长度错误不移动容器；成功保留原数据地址。容量可大于长度。
复制、移动和赋值沿用现有 owner 契约：赋值先完成容器操作再提交 mapping；
移动源保留其原 mapping，实际存储可能为空，调用方用 `has_storage()/try_view()` 区分。

stride owner 必须显式给出 shape 和 strides，不提供默认构造或仅 shape 的 factory；
避免为空 mapping 推断无意义的 stride。首版不接受 `std::array` 作为 stride owner 存储，
其编译期元素数与运行时 span 的关系需要独立契约。现有连续 owner API 不变。

## 参考边界与成本

xtensor 0.27.1 的 `xarray` 提供 shape/strides 构造。
其[对应 resize](https://github.com/xtensor-stack/xtensor/blob/f115228a7fc277b9be1bef34572831f8fd53ac73/include/xtensor/containers/xcontainer.hpp#L1058)
要求动态 layout，按 shape 元素数调整容器；不据此推断任意带孔布局都已分配足够跨度。
本接口复用 mdspan mapping，并明确为完整 span 建立对象生命周期。

stride owner 相比连续布局保留不可推导的 stride；这是所选运行时布局的状态成本。
默认容器仍是 `tensor_vector`，尚未消除其 capacity 等 vector 元数据。
分配、对象尺寸和生成码须与同契约的 mapping + storage 组合比较；
此功能不代表任意 strided 计算都能与连续流式内核同速。

可运行的[填充矩阵示例](../../examples/np/strided_tensor.cpp) 展示分配、融合写入及跳过孔洞的规约。
