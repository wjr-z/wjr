# function_ref 的借用与调用契约

`wjr::function_ref` 保留两指针大小的非持有描述符，用于一次调用链中的回调借用。
它不分配，也不延长目标对象的生命期；可保存回调的 API 需要另外选择持有类型。

## 目标的 cv 限定

对象构造使用 `std::remove_reference_t<F>` 保留原对象的 const/volatile，
随后叠加签名中的 const；约束、noexcept 判断和 thunk 使用同一调用类型。
不能先 remove_cvref，再从擦除指针恢复一个去掉 const 的目标。

| 目标对象 | `function_ref<R(Args...)>` 调用类型 | 带 const 签名的调用类型 |
| --- | --- | --- |
| `F&` | `F&` | `const F&` |
| `const F&` | `const F&` | `const F&` |
| `volatile F&` | `volatile F&` | `const volatile F&` |
| `const volatile F&` | `const volatile F&` | `const volatile F&` |

擦除存储采用 const volatile void 指针，恢复时只以该 thunk 的原始调用类型访问。
指针的 cv 擦除不授权修改 const 对象，也不把 volatile 访问降为普通访问。
这与 [标准 function_ref 的对象构造规则](https://eel.is/c++draft/func.wrap.ref.ctor) 中
remove_reference、cv 叠加和 is-invocable-using 的分工一致；不宣称已实现整个标准接口。

例如同时具有非 const noexcept 重载和可能抛异常的 const 重载的对象：
借用其 const 实例时，只能选择后者，因此不能构造 noexcept function_ref。
该条件在构造约束中拒绝，而不是等到函数体错误或运行时 terminate。

## 版本与验证

`nontype_t` / `nontype` 继续作为 WJR 的固定扩展类型，
不根据语言版本自动改用一个可能尚未提供或已改名的标准库设施。
这也避免同一公开签名因翻译单元的 C++20/23/实验性 C++26 模式不同而切换 tag 类型。
保留现有函数指针和 nontype 调用入口，本轮不增加持有语义或新的成员绑定入口。

验收覆盖四种目标 cv、带/不带 const 与 noexcept 的四类签名、
不可调用目标的负向约束、复制、普通函数与 nontype 目标。
布局和生成码只按实际编译器探针报告；不把源码变短视为吞吐量提升。

本地 Clang 18 的 C++20/23、MSVC 19.42 的 C++20 及 Clang C++20 ASan
通过新增 4 项和已有 5 项回调测试；GCC 12 的独立 cv 分派与负向构造探针通过。
两种 C++ 标准下的 5 组既有合法调用生成相同函数指令，x64 描述符仍为 16 字节。
Clang 18 的实验性 C++2c 模式可包含此头并构造 WJR nontype 回调，
原语言版本分支会因标准库没有对应名字而失败；这不将 C++26 升为仓库构建基线。
