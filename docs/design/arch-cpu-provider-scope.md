# CPU provider 的构建选项隔离

## 问题与设计

2026-10-30 在 `0f0288de` 上用嵌入消费者复现：父工程设置
`BUILD_TESTING=ON CACHE BOOL`，随后 `add_subdirectory(wjr)`，即便
`WJR_BUILD_TESTS=OFF`，父工程的 `BUILD_TESTING` 也变成 OFF。
最近控制点为 `src/arch/x86/CMakeLists.txt` 中
`set(BUILD_TESTING OFF CACHE BOOL "" FORCE)`。

此设置本来只应禁用 Google cpu_features 自带的测试。固定依赖 v0.10.1
的 CMake 最低版本为 3.13，其 `option(BUILD_TESTING ...)` 遵循 CMP0077 NEW，
允许从调用目录传入普通变量。WJR 已在 `src/arch/x86` 独立目录加载 provider；
优先直接用目录作用域普通变量，避免写全局 cache，也不需要新的包装函数或保存/恢复框架。
实现者必须通过真实 provider 验证此推断，不能只测试模拟 `option()`。

保留 cpu_features 版本、链接关系、baseline target、安装与现有公开 CPU 信息符号。
不调整动态初始化、填充阈值和 ISA 分派；这些是独立决策。本修复不改变运行时代码。

## 实施与验收

执行模型 `gpt-6.1-sol high`。先读 AGENTS.md、根 CMake、target helper、provider
固定源码及现有 consumer/test 注册。

1. 最小替换 cache FORCE；验证只在 cpu_features 加载的目录内屏蔽测试。
2. 增加真实嵌入消费者回归：父工程 BUILD_TESTING ON/OFF 与未定义状态，正常变量
   和 cache 的有效值/是否定义保持；provider 测试关闭，父/兄弟测试仍按父设置注册。
   避免递归启动完整 WJR 测试或重新下载已存在依赖。
3. 使用仓库最小配置测试注册方式，验证旧代码失败/修复通过；不添加无关Windows job。
4. 验证 build-tree 与 installed consumer，CPU baseline 依赖不变；覆盖
   C++20/23、baseline/当前支持的显式ISA和一个无效配置。使用独立 build 目录。
5. 运行相关 CMake/CTest、格式检查和差异检查；无C++修改时无需人为引入格式变更。
6. 独立提交 MR，最多四个待审，不自动合并。报告具体检查与缺项。

这是一项 build 作用域修复，不声称带来吞吐优化。后续 arch 路线优先 x86-64 GCC/Clang，
保留MSVC；ARM64 Linux/iOS先验证portable基础，不复制所有平台内核。

## 验证结果

本地验证环境为 Windows x64、Clang 18.1.8 GNU frontend、Ninja、Release，
使用 MSVC SDK 和独立 build 目录，复用固定版本的 cpu_features 源码。

- 生产修改仅将 `BUILD_TESTING` 改为 `src/arch/x86` 目录普通变量；
  真实 v0.10.1 provider 的 `option()` 和 `include(CTest)` 均保持测试关闭。
- 新增七个 `wjr_provider_scope_*` 配置测试，覆盖 cache ON/OFF、normal ON/OFF、
  未定义，以及 normal 遮蔽相反 cache 的两种状态。检查各变量的值、存在性和 cache
  元数据，并检查 provider 测试目录/target 未建立、父工程前后及兄弟目录的测试注册。
  配置测试复用已获取的 provider 源码，关闭嵌入 WJR 自身的测试与安装。
- 恢复旧的 cache FORCE 后，`cache-on` 用例按预期报
  `WJR changed the parent BUILD_TESTING cache value.`；修复后七例全部通过。
- C++20/baseline 的 `tests` target 构建和 CTest 19/19 通过；
  嵌入 fixture 的父工程前后与兄弟目录三个测试实际运行通过。
- C++20/baseline、C++20/native、C++23/baseline、C++23/native、
  C++23/x86-64-v3 均完成库构建、安装、build-tree 消费者构建运行、installed
  消费者构建与 CTest（每组 5/5）。五组实际编译命令中的 `cpuinfo.cpp` 始终为
  `-march=x86-64`，未继承 native 或显式 ISA。
- 无效 `WJR_CPU_TARGET=wjr-invalid-target` 按预期配置失败；
  `python -m pre_commit run --all-files` 和 `git diff --check` 通过。

本地未运行 Linux GCC/Clang、MSVC frontend 或 ASan；这些配置仍由既有 CI 验证。
这次未修改运行时代码，因此未新增性能测量。
