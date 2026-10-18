# NP 后续能力实施计划

本计划落实[核心标准](np-core-standard.md)，按可用能力组织实现、针对性测试、例子与证据。
每组自然形成完整审查，不以固定行数拆开同一个不变量，也不为占满审查容量填充工作。
审查基线 `39fb36ba` 已有值表达式、广播/stride 求值、eval、xsimd、cast、view sum/scan 与表达式 sum。
map/mask 与正 stride owner 已于本次审查期间合入，准确快照见核心标准。
当前同步落实构造边界收敛：可选 try_make_* 薄函数合并到 make_view.hpp，返回同一种 mdspan；
不制作 checked 数据结构，可信内部直接构造，owner/reshape 使用内部 metadata 而不绕公共工厂。

## 1. 完整 span owner 与组合验收

优先级 P0，依赖现有 owner/mapping；与中性构造头及可信内部路径迁移同步。
完成显式 shape/strides make/adopt、required_span_size 观察与分配所需内部 mapping 验证。
静态 extents 仍保留每实例 strides；普通 packed owner 的表示和构造保持原合同。
容器覆盖完整 span，has_storage 不比较逻辑 size；初批不提供 shape-only/default stride owner 或 array 配置。

文档和例子解释孔洞的生命周期与 default_construct 后先写后读责任。
逻辑 eval 只写逻辑对象，不能声称整个底层容器随后都可复制；packed eval(view) 是逻辑副本入口。
不通过清零孔洞或把普通 owner 复制改成新语义规避这一边界。

验收：padded row/column、静态/动态 strides、rank-0/空域、非法排序/负/零/溢出、adopt 不提前移动、
状态 allocator/失败清理/移动源、非平凡孔洞对象；组合 map/select、sum 与 scalar fallback。
生成码比较同合同 mapping + container，记录分配跨度，不能把有孔 allocation 与 packed 数据吞吐混比。
这组主体已合入，剩余重点是构造头迁移、组合验收与证据，不要求重新开发。

## 2. 保留静态信息的正步长切片

优先级 P1，依赖合法 view/mapping，不依赖新增 owner 或 SIMD。
设计一个受限通用 slice 入口：整数缩轴、full_extent、半开 range、正 step，复用固定后端 submdspan。
先支持默认 accessor 与已有布局子集；每一新增 accessor/layout 明确转换与 offset_policy。
返回类型保留可证明的静态 extents、packed 顺序和单位内步长，不无条件擦成全动态 layout_stride。

先 probe 固定后端对空 slice、窄 index、静态 slice 和 padded 结果的实际行为，
再决定薄适配或受限转换；不为便利接口升级全部 backend 或修改 vendored 代码。
合法 view 的存储不重复做全范围校验；普通切片优先沿用 submdspan 的可信参数前提，
不为切片再建立独立 checked 类型或通用错误恢复框架。

例子：从有行间 padding 的矩阵选窗口、取每隔两个特征、转置后读取。
验收：rank 改变、混合静态 extents、空/尾端范围、越界/步长溢出、const、布局类型断言及零分配。
分别比较直接地址描述符、WJR view 和后端 submdspan；布局分支与丢失的静态信息都计入成本。
signed reverse 与可写重叠不混入本组。

## 3. 有数值合同的函数与实用流水线

优先级 P1，可与切片并行，依赖现有静态操作；map/mask 用于组合示例，不依赖 SIMD。
选少量实际需要的 sqrt、exp、log、abs、isfinite 等具名静态节点，精确使用标量标准库 overload 的结果类型。
整数 sqrt/exp/log 按相应 cmath overload 转换；C++20 不额外承诺这些调用能常量求值。
整数 abs 的最小值、unsigned/bool 准入、定义域、NaN/Inf、带符号零、errno/fenv 和 noexcept 逐项说明。
不增加每元素诊断，不一次建立全部 NumPy ufunc 目录，也不自动把 map 认作 batch callable。

例子使用当前 sum/keepdims 的显式小 workspace 计算中心化、有效值选择和尺度转换。
危险计算在 map 内保护时传原始操作数，不能先把非法 child 算式传进去；select 也不提供保护。
验收：同 std 调用的独立标量 oracle、mixed dtype/cast、空域不调用、异常/FP 边界、真实 bool 与安装消费。
新数学函数首先 scalar 正确；成熟 math 后端或 SIMD 加速须另行匹配误差与 fenv 合同。

## 4. 带类型的 predicate SIMD

优先级 P1，性能驱动；依赖 mask/select 和当前显式 xsimd policy。
对支持的 homogeneous float/double 根表达式，建立私有 numeric/predicate batch 资格与结果类型。
比较/逻辑生成 lane 几何匹配的 predicate，select 两分支为数值 batch；不增加中间 bool 数组。
独立 bool materialization 暂走 scalar；native mask 表示不进入公共 AST 或跨 ISA ABI。

验收：eager 两分支、NaN/Inf/正负零、连续/unit/splat、tail 不越界、精确原位、
mixed integer division/cast/未知 map 整体 fallback，不实例化无效 batch 分支。
对照相同 ISA、相同选择语义的手写 SIMD；同时记录 setup、fallback、编译时间与代码体积。
某子集若只增加复杂度而无实测收益，保留 scalar，不强求所有 mask 向量化。

## 5. Ordered fused collective 与扫描组合

优先级 P1，依赖 expression sum 与最终 map 绑定；可独立于 predicate SIMD。
已有 view 四列 sum/scan 不重复实现。先为适合的表达式 axis-0 sum 增加 unit/splat 多列路径，
每列保留独立 accumulator 和原顺序；可能抛出的子树继续 line-at-a-time，保留完成 line 才提交的保证。
若实际流水线需要，再把同一绑定用于 expression scan，明确逐前缀写入、原位和异常行为。

四列是起点，八加四尾段是候选；不公开微调旋钮或直接把 portable 默认改成十六。
验收真实 API 的 float/double/wide Acc、短/尾/混合长度、复杂树/捕获状态、原位 scan 与部分失败。
默认改变前补 Linux/SysV GCC/Clang 的计时与生成码；区分 ABI 寄存器保存与热循环 spill，
报告缓存计数器缺失，匹配 ordered FP 语义。现有 Windows 证据只证明机会。

## 6. 一种有用的规约数值选择

优先级 P2，依赖 terminal sum；由使用者的精度或重排需求决定先做哪一种。
需要固定分组时先实现 pairwise：明确树形、叶块 cutoff、空域/带符号零、布局一致性和栈上界。
需要重结合吞吐时实现 relaxed SIMD：只放宽累计加法分组，保留 child dtype/cast/除法。
两者不强制同时交付，也不改变默认 ordered sum。

pairwise 初版可限制布局，不能无声退回另一种分组；泛 stride 扁平化不能每点增加除模却宣称免费。
relaxed fallback 须满足所声明的数值合同。整数重结合因不同中间溢出条件另行设计。
验收高精度 oracle 的消去/量级测试、NaN/Inf/零、精确 grouping oracle、同分组手写性能和无隐藏 payload。
stable norm、max/mean/variance 在明确空域/NaN/tie/溢出策略后独立进入，不借泛型 reduce 跳过这些决定。

## 7. 绑定表示与编译成本收敛

优先级 P2，依赖实际 map/mask/函数/规约案例；解决已定位问题，不只提交 checker。
测 2/4/8/16 叶子、重复树、局部与非内联 F、静态/动态 shape，分开构造、首次与重复求值。
记录 AST/plan 尺寸、数据与函数 slot 展开、tuple 索引、helper outlining、编译时间/峰值内存和二进制。
单次表达式不分配并不证明每叶每轴绑定或 repeated state load 没有代价。

明确两个 F 基准：同 const F& 调用，以及手工外提字段的最佳实用循环。
小系数可以是 scalar operand；不默认复制任意 F，不以 restrict 修饰普通原位入口。
每次只选能解决已测问题的变化：精简绑定、有限专门化、静态 prefix 表，或有明确寿命的引用表达式。
大树复制/实例化未形成实际瓶颈前，不增加公共 plan、DAG、let/cache 或强制 fusion 边界。

## 8. 小 owner 与布局互操作的独立准入

优先级 P3，依赖明确工作负载；两条支线互不依赖，不机械拼成一个 MR。
若大量小 owner 的对象密度/移动成本突出，先研究兼容 Container 合同的 opt-in exact storage。
它通常仍需 pointer+count；指针级静态 owner 需整合生命周期，不是换容器名称即可达到。
保留 allocator hooks、释放长度、copy selection、传播规则、失败状态、moved-from 查询和 noinit 语义。
验收 sizeof/alignof、分配数量/字节、owner 数组访问和构造+计算，不只测大 payload 热循环。

若外部数据要求 reverse 或 padding witness，选择相应互操作支线：
signed read-only descriptor 明确逻辑原点/min-max offset；padded/aligned 适配明确支持、pitch 和切片降级。
可写 zero/overlap、任意字节缓冲区和设备内存不默认开放。STL 支持不能由语言模式推断。

## 顺序与发布门槛

先完成候选组合验收及第 1 组，再推进第 2/3 组可见能力；第 4/5 组按性能需要独立推进。
第 6 组依据数值需求，第 7/8 组依据实测缺口，避免把计划执行成必须创造全部机制的清单。
安全 assign/scratch、oneTBB、BLAS、大型 graph export 保留需求门槛，低层接口不增加预防性检查。

每组运行最小相关 NP 测试/ASan 与必要安装消费，涉及 C++ 时执行仓库规定 clang-format hook。
额外标准/ISA 组合优先 Linux，保留现有 Windows 两项；性能测试独立于噪声敏感的普通 CI。
代码与文档必须写同一合同，未实现规则保持后续状态，审查前更新精确快照和遗漏验证。
