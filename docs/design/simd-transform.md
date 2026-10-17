# 连续 SIMD 变换

`<wjr/simd/transform.hpp>` 在可选 `wjr::simd_xsimd` 模块中提供：

```cpp
transform_to(dst, src, count, operation);
transform_to(dst, lhs, rhs, count, operation);
```

这是明确选择 SIMD 的低层接口，适合预分配缓冲区中的缩放、线性组合或裁剪。
它不绑定 tensor、调度器或量化业务，也不默认引入通用核心库。
T 必须是固定后端支持的非 bool 算术元素；各输入和输出使用相同 T。
可通过 `transform_to<T, Arch>` 选择与调用方编译选项一致的后端架构。

## 算子与存储契约

同一个 operation 对象作为左值使用，不复制或移动；它须同时接受 batch 临时值和
标量 `const T&`，二元版本接受对应的两个参数。batch 结果必须是相同 batch 类型，
标量结果须能隐式转换为 T，以保留 uint8_t/uint16_t 等算术的普通整数提升。
调用方负责两种重载的逐元素语义一致，包括溢出、舍入和 NaN 策略。
操作次数为 count / batch::size 次 batch 调用及 count % batch::size 次标量调用；
不能把有跨元素依赖的状态机当作普通逐元素算子。

count 为零时所有缓冲区可以为 null，不形成空指针偏移，也不调用 operation。
非空缓冲区必须包含 count 个有效 T 对象，输出对象须可写。
每个输入与输出必须完全分离或首指针精确相同；其它部分重叠是调用方违反前置条件。
二元输入之间可以任意重叠。完整 batch 先读取两个输入再存储，因此支持 dst 等于任一输入或两者。
所有加载和存储使用后端未对齐操作；尾部只逐个访问剩余对象，不向后过读或过写。
没有分配、隐藏临时缓冲区或共享所有权，也不显式检查 alias 或增加逐元素 WJR_CHECK。

函数的条件 noexcept 覆盖 batch 调用和存储、标量调用和到 T 的赋值转换。
不要求用户包装或上游算子本身已标记 noexcept；可能抛出的操作正常传播异常。
异常发生时已完成的输出前缀保留，当前抛出的 batch 尚未存储，不提供整体回滚。
算子若自行修改输入或其它外部状态，仍遵循该算子自身的异常语义。

## 例子

```cpp
namespace simd = wjr::simd::xsimd_backend;

// dst/src 均为 float*，存储由调用方管理。
const float scale = 0.5f;
simd::transform_to(dst, src, count, [scale](auto value) {
    return value * decltype(value)(scale);
});

const float a = 0.25f, b = 0.75f;
simd::transform_to(dst, lhs, rhs, count, [a, b](auto x, auto y) {
    using value_type = decltype(x);
    return value_type(a) * x + value_type(b) * y;
});

// 比较为 false 时保留原值，因而不把 NaN 隐式当成上下界。
auto clip = [](auto value) {
    using V = decltype(value);
    if constexpr (std::is_arithmetic_v<V>) {
        return value < V(-1) ? V(-1) : value > V(1) ? V(1) : value;
    } else {
        return xsimd::select(value < V(-1), V(-1),
                             xsimd::select(value > V(1), V(1), value));
    }
};
simd::transform_to(dst, dst, count, clip);
```

这些例子不启用 fast-math，也不强制融合乘加；位级结果和浮点异常标志仍受算子、编译选项和架构影响。
复用固定版本的 [batch 加载和存储](https://github.com/xtensor-stack/xsimd/blob/e88a72831858123924f7118f345dfe5d70d95991/include/xsimd/types/xsimd_batch.hpp)
及 [select](https://xsimd.readthedocs.io/en/latest/api/batch_manip.html)，不重建算术操作或完整 xsimd API。
