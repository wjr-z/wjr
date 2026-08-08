# WJR 模块说明

WJR 的模块通过 CMake alias 提供。使用某个模块时，在自己的 CMake target 上链接对应的 `wjr::<module>`；CMake 会自动传递头文件路径、C++ 标准和模块依赖。

```cmake
target_link_libraries(my_app PRIVATE wjr::json)
```

如果不需要拆分依赖，直接链接总库 `wjr::wjr` 即可。下面列出当前面向用户的模块。

## `wjr::config`

基础配置模块，提供平台、编译器、特性检测和库内部使用的配置定义。

通常不需要单独链接。只有在编写依赖 WJR 配置宏或基础配置类型的底层组件时，才适合直接声明这个依赖。

## `wjr::assert`

断言和调试辅助功能，供库内代码检查前置条件、内部状态和不可达路径。

编写与 WJR 风格一致的底层组件，且需要使用 WJR 断言接口时使用。启用 `WJR_ENABLE_ASSERT_BACKTRACE` 后，该模块可以额外提供断言回溯信息。

## `wjr::iterator`

迭代器相关的基础工具和适配支持，为容器、算法及其他模块提供通用迭代器能力。

实现自定义容器或需要复用 WJR 迭代器工具时使用；只使用上层模块时通常会通过依赖传递得到它。

## `wjr::memory`

内存管理和分配器相关组件，包含库内数据结构使用的内存工具。

实现对分配策略有要求的容器或需要直接使用 WJR 内存组件时使用。普通业务代码通常不需要单独依赖它。

## `wjr::math`

数学和位运算相关的基础算法，部分操作会使用 x64 架构优化。

需要复用 WJR 数学算法，或实现依赖这些基础运算的组件时使用。项目会根据目标平台选择相应的架构实现。

## `wjr::biginteger`

大整数类型和运算支持，包含加减乘除等大整数操作，并在可用时使用架构优化。

需要处理超出内置整数范围的整数，或需要 WJR 大整数接口时使用。

## `wjr::json`

JSON 数据结构和解析、序列化相关功能。

程序需要读取、构造或输出 JSON 数据时使用。该模块依赖 `wjr::biginteger` 及 JSON 相关的架构实现。

## `wjr::ring_buffer`

环形缓冲区容器，适合先进先出、固定容量或连续生产消费场景。

需要低开销地暂存数据流、事件或生产者消费者之间的数据时使用。它依赖 `wjr::assert` 和 `wjr::iterator`。

## 内部模块

下面的 target 用于库内部实现，一般不建议应用程序直接依赖：

- `wjr::format-internal`：格式化实现的对象库，不是面向用户的独立模块。
- `wjr::arch-*`：架构相关实现，例如 `wjr::arch-math`、`wjr::arch-json` 和 `wjr::arch-biginteger`。
- `wjr::asm-*`：NASM 汇编实现，只有启用对应架构汇编时才会生成。

应用程序优先使用上面列出的公共模块或总库 `wjr::wjr`，不要依赖这些内部 target 的名称和实现细节。

## 使用方式

在 WJR 已作为子目录加入项目，或通过安装包 `find_package(wjr CONFIG REQUIRED)` 找到后，链接模块 alias：

```cmake
find_package(wjr CONFIG REQUIRED)

target_link_libraries(my_app PRIVATE wjr::biginteger wjr::json)
```

只需要构建某个模块时，可以构建对应的实际 target：

```sh
cmake --build build --target wjr-json
```

实际 target 名称用于构建命令；在 `target_link_libraries` 中应使用 `wjr::<module>` alias。
