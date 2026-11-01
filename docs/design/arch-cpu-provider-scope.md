# CPU provider 的构建选项隔离

状态：修复已实现并验证。此记录保留问题复现、目录作用域方案和验证边界；它不表示
运行时 CPU 检测、ISA 分派或吞吐发生改变。总的架构边界见
[arch 演进设计](arch-foundations.md)，实现状态见[实施记录](arch-implementation.md)。

## 问题与修复

2026-10-30 在源码快照 `0f0288de` 的嵌入消费者中复现：父工程设置
`BUILD_TESTING=ON CACHE BOOL`，再 `add_subdirectory(wjr)`；即便 `WJR_BUILD_TESTS=OFF`，
WJR 仍把父工程的 cache 值改为 OFF。原因是 `src/arch/x86/CMakeLists.txt` 用
`set(BUILD_TESTING OFF CACHE BOOL "" FORCE)` 禁用 Google cpu_features 自带测试。

固定依赖 cpu_features v0.10.1 的 CMake 最低版本为 3.13；其 `option(BUILD_TESTING ...)`
遵循 CMP0077 NEW，可读取调用目录的普通变量。WJR 在独立的 `src/arch/x86` 目录加载
provider，因此将 cache 强制赋值改为目录作用域普通变量 `set(BUILD_TESTING OFF)`，只屏蔽
provider 子目录测试，不污染父工程 cache、父/兄弟测试或普通变量。保留 cpu_features 版本、
链接关系、baseline target、安装行为和公开 `X86Info` 符号；不调整动态初始化、填充阈值或
ISA 分派。当前实现见 [src/arch/x86/CMakeLists.txt](../../src/arch/x86/CMakeLists.txt)。

## 验证结果

验证环境：Windows x64、Clang 18.1.8 GNU frontend、Ninja、Release、MSVC SDK；复用固定的
cpu_features 源码和独立 build 目录。

| 检查 | 历史结果 |
| --- | --- |
| 嵌入消费者配置测试 | 七种状态（cache ON/OFF、normal ON/OFF、未定义及 normal 遮蔽相反 cache）全部通过；检查父变量的值/定义、cache 类型与 help string、provider 测试关闭及父/兄弟测试注册。恢复旧 cache FORCE 后，cache-on 按预期失败 |
| C++20/baseline | `tests` 构建和 CTest 19/19 通过；父工程前后与兄弟目录三个测试通过 |
| 构建与安装消费者 | C++20 baseline/native、C++23 baseline/native/x86-64-v3 五组均构建、安装并运行 build-tree 与 installed consumer CTest（各 5/5）；`cpuinfo.cpp` 始终为 `-march=x86-64` |
| 无效配置和格式 | `WJR_CPU_TARGET=wjr-invalid-target` 按预期配置失败；all-files pre-commit 和 `git diff --check` 通过 |

本地未运行 Linux GCC/Clang、MSVC frontend 或 ASan；由既有 CI 覆盖的平台检查仍适用。
本次未修改运行时代码，未做性能测量。
