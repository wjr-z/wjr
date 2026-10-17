# SIMD 后端初版

初版选择 xsimd 14.3.0 作为可选的通用算术/谓词后端，以薄适配复用其寄存器类型和操作。
这不是现有 SSE/AVX 扫描器的全量替换，也不建立运行时分发器。
扩展使用自由函数和组合，不从 `xsimd::batch` 继承，避免改变其运算符返回类型、重载和寄存器表示。

## 依赖和使用

`WJR_ENABLE_XSIMD=ON` 创建独立的 `wjr::simd_xsimd` target，要求 C++20。
默认关闭，不下载 xsimd，也不把依赖加入 `wjr::wjr` 或通用容器。
FetchContent 固定到经过审查的
[14.3.0 commit](https://github.com/xtensor-stack/xsimd/tree/e88a72831858123924f7118f345dfe5d70d95991)。
也可以提供该版本的安装包或已有 target；头文件检查版本，升级需更新适配和验收。
对照基准与可选模块复用同一依赖，单独启用基准时不安装该依赖。
父项目提供源码 target 时，其安装责任仍归父项目；WJR 的安装消费必须能找到相应 xsimd 包。

```cmake
# 构建 WJR 时指定 -DWJR_ENABLE_XSIMD=ON。
find_package(wjr CONFIG REQUIRED COMPONENTS simd_xsimd)
target_link_libraries(app PRIVATE wjr::simd_xsimd)
```

安装消费通过显式 component 加载后端。未请求该 component 的消费不要求 xsimd。
后端使用 `wjr::isa` 的显式 CPU 配置，不将 native 要求添加到通用模块。
同一构建的内核和调用方必须使用一致的 ISA 配置；该 target 不是 fat binary。

## 数据与操作契约

`<wjr/simd/xsimd.hpp>` 中的 `wjr::simd::xsimd_backend` 目前只包含：

- `batch<T, Arch>`、`mask<T, Arch>`：后端本地类型的别名，不额外保存对象或拥有资源。
- `any(mask)`、`all(mask)`：使用后端原生谓词规约，不强制先转整数 mask。
- `first_true(mask)`：返回第一个 true lane 的索引；空结果返回 lane 数。
- `find_equal(src, value, count)`：返回首个相等元素或 count，作为有界读取的参考内核。

连续数据的一元/二元变换见 [`<wjr/simd/transform.hpp>`](simd-transform.md)：
输出缓冲区由调用方预分配，复用 batch load/store 和用户算子，不引入表达式引擎。

这些类型和名称仍属实验接口。架构进入 `batch` 的类型，因此它们不出现在跨 ISA 的编译内核 ABI 中。
未来的分发入口采用指针、元素数和标量参数；CPU 探测及 dispatcher 必须用基线 ISA 编译。
是否引入多目标分发，需要单独衡量间接调用、代码体积和首次探测成本。

谓词契约按 lane 定义。WJR 原有 `movemask_epi8` 按字节编码，不能直接用 lane mask 替换，
JSON/UTF-8 的字节位图路径继续单独管理。`first_true` 必须提取顺序信息，
具体指令成本取决于后端；不承诺每种 ISA 都等于一次 movemask。

参考扫描只接受后端有寄存器支持的非 bool 算术元素，使用未对齐完整向量加载和标量尾部，
不读取 `[src, src + count)` 之外的元素。count 为零时允许 src 为 null，且不形成空指针偏移。
所有非空输入必须指向足够多的有效 T 对象。浮点比较采用普通 `==`：NaN 不匹配，正负零匹配。
不启用 fast-math，不宣称不同平台的 NaN 异常标志或浮点环境副作用完全相同。

扫描不分配、不维护共享所有权、不做虚调用，也不隐藏复制。
对齐、masked load 的 fault 行为、动态 dtype 和任意 stride 不在该初版接口中。

## 扩展顺序与验收

先用现有 WJR、普通循环和 xsimd 对比扫描，覆盖命中位置、无命中、尾部、对齐和混合输入。
测试检查每个 lane 的谓词顺序，以及逻辑尾部就是分配末端的缓冲区；ASan 用于发现过读。
生成码检查包装是否内联，并与直接 xsimd 写法比较，不以代码行数推断成本。

下一步再根据实际计算需求加入 load/store、select 和算术内核，复用 xsimd 已有设施。
只有在现有扫描或特殊 shuffle 的语义/性能确实不匹配时增加局部扩展。
不预先复制完整 xsimd API，也不把张量元数据绑定到 SIMD 寄存器类型。

参考 [xsimd batch 源码](https://github.com/xtensor-stack/xsimd/blob/e88a72831858123924f7118f345dfe5d70d95991/include/xsimd/types/xsimd_batch.hpp)、
[Highway 分发边界](https://github.com/google/highway/blob/457c891775a7397bdb0376bb1031e6e027af1c48/README.md)
和[性能验证说明](../performance/simd.md)。
