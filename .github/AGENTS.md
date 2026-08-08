# WJR AI Agent 说明

## 项目

- 这是一个使用 CMake 构建的 C++17 库。大多数架构相关的优化针对 x64。
- 公共头文件位于 `include/wjr`，实现代码位于 `src`。
- 根目录的 `CMakeLists.txt` 是构建选项和 target 依赖关系的权威来源。
- 各模块的用途和 CMake alias 见 [模块说明](../docs/modules/README.md)。

## 构建和测试

- 使用 `build` 等源码目录之外的目录进行构建。
- 配置时添加 `-DWJR_BUILD_TESTS=ON`，然后构建 `tests` target。
- 覆盖率需要同时配置 `-DWJR_BUILD_TESTS=ON` 和 `-DWJR_ENABLE_COVERAGE=ON`，然后构建 `coverage` target。
- 配置时添加 `-DWJR_BUILD_BENCHMARKS=ON`，然后构建 `benchmarks` target。
- 复现问题时保留原来的构建类型和相关选项。

## Target

- `wjr` 是最终库 target，同时提供 `wjr::wjr` alias。
- 模块 alias 使用 `wjr::<module>` 的形式，对应的实现 target 名称为 `wjr-<module>`。
- 其他 CMake 项目应优先链接 `wjr::` alias，不要依赖实现 target 的名称。

## 修改规则

- 保持修改聚焦，并遵循附近 CMake 和 C++ 代码的风格。
- 除非任务明确要求，不要修改 `build`、`build_asan`、`CMakeFiles`、`compile_commands.json` 等生成文件或构建目录。
- 除非任务明确针对第三方代码，不要修改 `third-party` 下的 vendored 代码。
- 除非任务明确要求破坏性变更，否则保持公共 API 和 C++17 兼容性。
- 对行为变更添加或更新针对性的测试。
- 完成前运行与修改最相关的、范围最小的构建或测试 target，并报告缺失的依赖或失败的检查。