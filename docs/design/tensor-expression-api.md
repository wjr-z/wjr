# 第一版表达式接口

`<wjr/np/expression.hpp>` 是可选 `wjr::np` 的实际计算接口。
它提供 as_expr、内建 +、-、*、/、一元 +/-，以及写入预分配输出的 eval_to。
完整广播、创建拥有结果的 eval、自定义 map、规约与 SIMD policy 留给后续能力。

```cpp
namespace np = wjr::np;
auto x = np::as_expr(input.view());
auto y = np::as_expr(other.view());
auto difference = x - y;
np::eval_to(output.view(), 0.5 * difference * difference);
```

## 形状、类型与寿命

首版接受默认 accessor、layout_right/left 的算术元素 view，拒绝 volatile、
layout_stride 与代理存储。所有数组叶子和输出必须具有相同逻辑 shape；
rank 或已知静态 extent 冲突编译拒绝，动态 extent 在求值入口用 WJR_ASSERT_L2 检查。
rank 大于 1 时，所有数组叶子与输出还须具有相同布局；rank 0/1 的布局顺序等价。
它不将一个数组轴的长度 1 隐式广播到 N，也不把 rank-0 view 隐式扩成高 rank。
普通标量可以加入任意数组表达式；纯标量表达式也可以填充任意支持的输出域。

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
兼容的动态/静态 extent 组合保留已知静态值；runtime shape 查询与完整广播推导尚未提供。
节点和内部访问协议仍属实验接口，不承诺稳定 ABI。

## 直接求值的前置条件

eval_to 不调整输出、不分配、不保存共享所有权，也不扫描地址区间。
每个读叶子必须与输出逻辑存储不交，或在每个对应索引上引用同一对象。
因此 `eval_to(out, as_expr(out) + as_expr(out))` 可原位执行；移位重叠和原地转置违反前置条件。
每点的全部 RHS 读取发生在该点写出之前，不添加 restrict 或依赖某次遍历来修复重叠。
关闭对应断言时，动态 shape 与所有存储/别名条件仍由调用方保证。

rank 0 处理一个对象。任意 extent 为零时不准备数据绑定、不读取数组、不执行运算，
也不偏移空指针；例如空输出的除零表达式不会实际进行除法。
构造表达式时对标量进行的普通值捕获仍会发生。
内建算术入口为 constexpr/noexcept；浮点硬件异常不等同于 C++ 异常。

## 执行与性能边界

表达式保留树形值闭包。每次非空求值只将叶子指针与已捕获标量复制到一个自动存储期 tuple，
原表达式类型作为编译期运算计划；循环中没有 shape 解释或每点断言。
它不会分配临时数组、建立 DAG，或承诺昂贵重复子树只计算一次。
内建运算不主动重结合、改写 FMA 或启用 fast-math；调用方的编译选项仍生效。

该绑定方式在真实 mdspan 探针中恢复了 MSVC 的 axpby 自动向量化；Clang/GCC 也生成批量循环。
MSVC 的重复叶子探针仍保留 security cookie 与重复 alias guards，不能据此声称全面零开销。
现有证据是有限生成码与正确性检查，不是吞吐、编译成本或任意表达式树的完整验收。

运行示例见 [examples/np](../../examples/np/README.md)。
