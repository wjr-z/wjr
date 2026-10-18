# 第一版表达式接口

`<wjr/np/expression.hpp>` 是可选 `wjr::np` 的实际计算接口。
它提供 as_expr、内建 +、-、*、/、一元 +/-，以及写入预分配输出的 eval_to。
支持右对齐数组广播与正步长 view；创建拥有结果的
[eval](tensor-evaluation.md) 与可选 SIMD policy 使用各自头文件。
本页描述基础算术接口；后续的 [pure map](tensor-expression-map.md)、
[表达式规约](tensor-expression-reductions.md) 与 [cast](tensor-expression-casts.md) 分别说明扩展契约。
调度不在本接口范围内。

```cpp
namespace np = wjr::np;
auto x = np::as_expr(input.view());
auto y = np::as_expr(other.view());
auto difference = x - y;
np::eval_to(output.view(), 0.5 * difference * difference);
```

## 形状、类型与寿命

接受默认 accessor、layout_right/left/stride 的算术元素 view，拒绝 volatile 与代理存储。
layout_stride 必须是有效、正步长且唯一的 mapping；外部输入可先使用
[positive-stride construction](tensor-strided-view.md) 构造。
不同布局按逻辑坐标组合，不会把转置描述符误当成同顺序连续数组。

数组 shape 右对齐，缺少的前导轴视为长度 1；每轴长度相等或一方为 1 才兼容。
例如 [2,1,3] + [4,1] 得 [2,4,3]，0 与 1 合并得 0，0 与 N>1 不兼容。
rank-0 view 广播其单个对象，普通标量也可加入任意表达式。
静态冲突编译拒绝；动态 shape 是直接求值的前置条件，在入口用 WJR_ASSERT_L2 检查。
eval_to 还允许把表达式广播到预分配输出，但不允许删除表达式的轴。
因此 [3] 可写到 [2,3]，[1,3] 不能写到 [3]。

```cpp
auto rows = np::as_expr(np::tensor_view<const double, np::extents<size_t, 2, 1>>(scale));
auto columns = np::as_expr(np::tensor_view<const double, np::extents<size_t, 3>>(bias));
np::eval_to(output.view(), x * rows + columns); // output: [2,3]
```

节点的 value_type 从每个标量操作实际推导，保留 C++ 逐节点 promotion：
int/int 得整数除法，之后再加 float 不会改写该除法；uint8_t 加法可以先提升为 int。
只有最后写出时才进行到输出类型的普通赋值转换。
调用方负责浮点转整数的范围、整数溢出、除零及其他普通 C++ 算术前置条件。
当前不提供 NumPy dtype promotion、饱和或隐式环绕语义。

标量按实际类型复制，view 按只读描述符保存，左值节点复制、右值节点移动。
之后修改原系数或重绑定原 view 不改变表达式；修改仍存活的底层元素会在求值时可见。
owner 必须显式通过仍存活的左值 .view() 借用，不能隐式捕获 owner 或绕过已删除的右值 view()。
表达式不延长底层存储寿命，返回局部 owner 的借用仍然无效。
返回只包含局部轻量节点的组合是安全的，前提是其借用的数据继续存活。

表达式类型提供 value_type、extents_type、layout_type、rank() 和 static_extent()。
右对齐合并保留可推导的静态 extent，例如动态长度与固定 N>1 合并得到静态 N。
layout_type 表示拥有结果时首选的连续布局：相同布局保留，混合布局或 stride 叶子选 layout_right。
它不表示每个叶子都能线性读取；叶子的实际 mapping 仍保存在原 view 中。
节点 read(i) 是内部绑定协议，不是任意广播表达式的公开逻辑线性索引接口。
节点和内部访问协议仍属实验接口，不承诺稳定 ABI。

## 直接求值的前置条件

eval_to 不调整输出、不分配、不保存共享所有权，也不扫描地址区间。
每个读叶子必须与输出逻辑存储不交，或在每个对应索引上引用同一对象。
因此 `eval_to(out, as_expr(out) + as_expr(out))` 可原位执行；移位重叠和原地转置违反前置条件。
广播源若位于输出内部，通常不能满足此条件；例如把输出第一行广播回整个矩阵会有跨点写后读危险。
每点的全部 RHS 读取发生在该点写出之前，不添加 restrict 或依赖某次遍历来修复重叠。
关闭对应断言时，动态 shape 与所有存储/别名条件仍由调用方保证。

rank 0 处理一个对象。任意 extent 为零时不准备数据绑定、不读取数组、不执行运算，
也不偏移空指针；例如空输出的除零表达式不会实际进行除法。
构造表达式时对标量进行的普通值捕获仍会发生。
内建算术入口支持 constexpr；内建算术类型的求值为 noexcept。
直接构造自定义静态 Operation 节点时，read/eval_to 随实际操作决定 noexcept；
操作抛异常会传播，已写出的点保留，不提供事务回滚。
浮点硬件异常不等同于 C++ 异常。

## 执行与性能边界

表达式保留树形值闭包。非空求值在入口把叶子指针与已捕获标量绑定为自动存储期 tuple，
原表达式类型作为编译期运算计划；循环中没有 shape 解释或每点断言。
连续同 shape、同布局路径在入口判定一次；广播/混合布局/stride 路径把每叶的有效步长绑定一次，
广播轴步长在内部执行计划中置零，以固定 rank 的嵌套循环增量推进偏移。
最内层 cursor 按值接收本行的指针、标量和步长，输出写入不会迫使每点重读栈上的 metadata。
这些内部零步长不构造违反后端前置条件的公开 layout_stride mapping。
空域先返回；不在每点计算广播 shape、除模坐标或调用运行时算子分发。
它不会分配临时数组、建立 DAG，或承诺昂贵重复子树只计算一次。
内建运算不主动重结合、改写 FMA 或启用 fast-math；调用方的编译选项仍生效。

线性和通用路径各自局部绑定，运行时只执行其中一次，避免共享 tuple 导致 MSVC 热循环重读栈上字段。
该绑定方式在真实 mdspan 探针中保留了 MSVC 的 axpby 自动向量化；Clang/GCC 也生成批量循环。
MSVC 的重复叶子探针仍保留 security cookie 与重复 alias guards，不能据此声称全面零开销。
通用路径当前仍是实验执行策略：它具有额外的 shape/stride 准备、外层 cursor 复制和可能的
helper outlining；MSVC 通用行内核的已检探针不再逐元素加载 metadata，但尚未实现广播 SIMD splat
或 stride 特化，不能据此保证与针对布局编写的手工循环同样快。连续路径的 boundary 判定、
security cookie 和代码体积也可能带来额外成本。
现有证据是有限生成码与正确性检查，不是吞吐、编译成本或任意表达式树的完整验收。

运行示例见 [examples/np](../../examples/np/README.md)。
