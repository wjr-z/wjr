# WJR AI Agent 说明

## AI 工作上下文

- 本文件是仓库级 AI agent 指令；开始修改前先阅读与任务直接相关的源码、测试和构建配置。
- 保持修改聚焦，优先修复控制行为的最近代码路径；不要因为发现无关问题而扩大范围。
- 尊重工作区中已有的用户修改、暂存内容和未跟踪文件，不使用 `git reset --hard` 或 `git checkout --` 覆盖它们。
- 除非用户明确要求，不创建提交、不创建分支，也不修改生成目录或 vendored third-party 代码。
- 修改行为时优先补充或更新针对性测试；完成后运行与修改范围最小且最相关的构建、测试或配置校验，并报告未运行的检查。

## 项目

- 这是一个使用 CMake 构建的 C++17 库。大多数架构相关的优化针对 x64。
- 公共头文件位于 `include/wjr`，实现代码位于 `src`。
- 根目录的 `CMakeLists.txt` 是构建选项和 target 依赖关系的权威来源。
- 各模块的用途和 CMake alias 见 [模块说明](docs/modules/README.md)。
- `third-party` 下是 vendored 依赖；除非任务明确针对第三方代码，否则不要修改。
- `build`、`build_asan`、`CMakeFiles`、`compile_commands.json` 等是生成内容，不应纳入修改。

## 构建和测试

- 使用 `build` 等源码目录之外的目录进行构建。
- 配置时添加 `-DWJR_BUILD_TESTS=ON`，然后构建 `tests` target。
- 覆盖率需要同时配置 `-DWJR_BUILD_TESTS=ON` 和 `-DWJR_ENABLE_COVERAGE=ON`，然后构建 `coverage` target。
- 配置时添加 `-DWJR_BUILD_BENCHMARKS=ON`，然后构建 `benchmarks` target。
- 复现问题时保留原来的构建类型和相关选项。
- 推荐使用源码目录之外的构建目录，例如 `build`；不要把生成文件写入源码目录。

## CI

- CI workflow 位于 `.github/workflows`；根目录 `CMakeLists.txt` 仍是构建选项和 target 的权威来源。
- `linux.yml` 在 `ubuntu-latest` 上测试 Clang 17-20 和 GCC 9、11-14，启用测试和 ASan，并运行 GMP 测试环境。
- `windows.yml` 在 `windows-latest` 上测试 MSVC 和 Clang，安装 NASM，关闭 GMP 测试，并运行 Windows 测试。
- Linux、Windows 和 pre-commit workflow 都在 `main` 的 push、pull request 上运行，也支持手动 `workflow_dispatch`；Linux/Windows workflow 会跳过仅修改 `README.md`、`docs/**`、`AGENTS.md`、`.editorconfig`、`.gitattributes`、`.clang-format`、`.pre-commit-config.yaml` 或 pre-commit workflow 的变更。
- pre-commit workflow 不使用这些排除项，格式规则或 hook 配置变化应由它负责验证；源码、测试、依赖、LSan 配置以及 Linux/Windows workflow 的变化仍应触发对应编译 CI。
- 修改编译器选项、依赖、测试配置或 workflow 时，至少运行对应平台或 target 的最小验证；不要把 GitHub Actions 生成目录或缓存提交到仓库。

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

## 格式化与换行

- `.editorconfig` 和 `.gitattributes` 统一文本文件使用 LF；新增或修改文件应保留 LF 和末尾换行。
- 不要用会自动补充末尾换行的批处理工具覆盖没有末尾换行的旧文件，除非任务明确要求修复该问题。
- 需要批量归一化已跟踪文件时，使用 `git add --renormalize -u`；它不会添加未跟踪文件，但会暂存已跟踪文件的当前修改，执行后必须检查 `git diff --cached`。
- 不要使用 `git add .` 代替上述命令，因为工作区可能包含日志、构建产物或用户自己的未跟踪文件。

## Pre-commit

- 代码格式检查由根目录 `.pre-commit-config.yaml` 管理，使用 `pre-commit/mirrors-clang-format` 的 `v18.1.8`，不依赖开发者本机预装的 clang-format。
- hook 检查 C/C++ 文件，排除 `third-party` 和 `build`/`build_*` 目录；不要另行创建仓库内自定义 hook 或安装脚本。
- pre-commit 没有数值型并行度配置，`require_serial` 只能将单个 hook 限制为串行；默认配置不强制串行，以免本地检查变慢。
- 本地标准流程：
	- `python -m pip install pre-commit`
	- `python -m pre_commit install`
	- `python -m pre_commit run --all-files`
- `.github/workflows/pre-commit.yml` 在 `main` 的 push 和 pull request 上只检查变更文件，手动 `workflow_dispatch` 执行全量检查；修改格式规则时要同时考虑本地 hook 和 GitHub Actions。