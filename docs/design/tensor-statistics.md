# 双变量统计原语

`<wjr/np/statistics.hpp>` 提供相同长度的一维借用 view 之间的标量统计：

```cpp
auto sample = np::covariance(x, y);          // ddof=1
auto population = np::covariance(x, y, 0);
auto correlation = np::pearson_correlation(x, y);
auto missing = np::nanpearson_correlation(x, y);
auto wide = np::nancovariance<double>(x, y);
```

这些是一般数值计算原语，不包含日期、行业、交易、研究指标或调度语义。
它们返回单个协方差或 Pearson 系数；不创建协方差矩阵，不计算 p 值或置信区间。
后续矩阵/单轴接口应复用内核，但不预先创建公共统计 plan 或结果状态对象。

## 类型、形状与数值合同

支持现有算术输入元素及一维连续/正 stride/padded view；输入可以互相别名。
长度必须相等，静态不匹配在约束中排除，动态不匹配沿用 L2 断言。
不分配存储、不修改输入；多遍读取期间输入必须稳定。

默认累加类型按两输入的 common_type 决定：浮点保留该类型，否则使用 double。
可显式选择无 cv 限定的浮点累加器。数值先转换到累加类型，整数不承诺超过其精度后的精确性。
这与 WJR mean 的类型原则一致，区别于 NumPy cov 默认至少 float64。

`covariance` 默认按 `N-1` 归一化，`ddof=0` 按 N；ddof 是非负 size_t。
N 不大于 ddof 返回 NaN。它使用两遍中心化算法：
第一遍按索引顺序累加相对于首个有效观测的差值，以构造均值；第二遍累加中心化乘积。
这避免 `sum(x*y) - sum(x)*sum(y)/N` 的大数相减，也使重复的浮点常数得到精确的零离差。
普通路径仍受累加器舍入、转换和中间值可表示范围限制；协方差不做溢出缩放恢复。

`pearson_correlation` 在第二遍同时累加交叉乘积与两个平方和，不创建中心化数组。
归一化不先相乘两个平方和，并先除较小范数以避免不必要的中间上溢/下溢。
普通有限正常量级走两遍路径；平方和为零、次正规或溢出，或交叉和非有限时，进入独立冷路径，
按各输入最大绝对值缩放后重算。常量、少于两个观测、非有限观测返回 NaN。
有限结果裁剪到 [-1,1]；极接近常量的输入仍可能放大舍入误差，不承诺逐位匹配 SciPy。

默认 NaN 传播。`nancovariance`、`nanpearson_correlation` 执行成对删除：
任一输入为 NaN 就同时删除该对观测，均值、有效计数与第二遍使用同一组保留项。
第二遍将缺失对的离差置零后规约，避免在累加器依赖链末端选择旧值/新值。
它们不分别过滤两输入，不把无穷当作缺失值。没有有效对时返回 NaN。
没有告警、异常、errno 检查、隐式缺失值填补或 always-on 参数检查。
不支持假定 NaN/无穷不存在的 fast-math 模式。

## 成本与验证

输入 reader 在连续布局只保留指针，stride 输入额外保留必要步长。
普通协方差不计算平方和；普通非 NaN 接口不计数或逐点筛查缺失值。
两输入均为整数时，NaN 接口在编译期转到普通路径。
NaN 接口显式支付重复筛选成本，换取不分配掩码/压缩数组。
极端量级恢复只给 Pearson 使用，不把它的多遍分类、最大值扫描放到普通协方差中。

np benchmark profile 的 `covariance_kernel` 对比同累加顺序、相同非内联边界的手写协方差。
覆盖 0/1/16/300/4096 长度、连续/stride=2、普通/成对忽略 NaN；每个 case 先核对结果。
NaN 场景包含无缺失、每 17 项缺失一项、随机约半数缺失及全部缺失，
另有第二遍将缺失离差置零的手写基线，避免只比较某一种分支写法。
它不测 Pearson 冷路径，也不代表数据准备、Python 接入或完整应用耗时。

参考 [NumPy cov](https://numpy.org/doc/stable/reference/generated/numpy.cov.html) 的归一化定义，
以及 [SciPy pearsonr](https://docs.scipy.org/doc/scipy/reference/generated/scipy.stats.pearsonr.html)
的系数公式和常量输入语义；本实现不复制其完整 API。
见[示例](../../examples/np/statistics.cpp)和[测试](../../test/np/statistics.cpp)。
