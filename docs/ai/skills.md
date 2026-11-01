# 本仓库使用的 Skills

技能只安装在本机；仓库只维护这份清单。[AGENTS.md](../../AGENTS.md)
要求每次对话先读取本页，再按任务自动选择技能。每个技能正文在同一会话只读取一次；
不适用的技能不加载，不在启动时联网检索或安装。

| 技能 | 何时使用 | 来源 |
| --- | --- | --- |
| `high-performance-cpp-foundations` | C++ 基础库设计、concept、容器、架构与性能工作 | 本机维护的项目专用技能 |
| `cpp-testing` | C++ 测试设计、失败诊断、GoogleTest/CTest、sanitizer 配置 | [affaan-m/ecc](https://github.com/affaan-m/ecc/tree/b38be6a7f6350964d59328b52f0624e1d1d552d1/skills/cpp-testing) |
| `dispatching-parallel-agents` | 用户授权下，拆分和调度两个以上互不写入同一文件的独立任务 | [obra/superpowers](https://github.com/obra/superpowers/tree/2ce8aa15d05bd57213e6f746f953aafb7ded7a8b/skills/dispatching-parallel-agents) |
| `find-skills` | 用户要求搜索技能，或明确存在尚未覆盖的专业能力缺口 | [vercel-labs/skills](https://github.com/vercel-labs/skills/tree/92a9a34ac3bf88e695e468b90c7bf32f163d32c8/skills/find-skills) |

仅此 4 个属于仓库任务清单；本机其他项目的技能不自动加载。性能优化已有专门技能，
不叠加多套通用 C++/性能指南。新增技能前用 `find-skills` 检索并审查来源、内容与实际缺口，
优先替换重复技能，而不是持续增加数量。

通用技能中的测试优先、完整测试或安全重写示例不能覆盖本库的验证成本与输入前提。
简单薄包装只做最小验证；复杂生命周期、并发和真实性能路径按风险验证。
子代理获得明确任务、文件归属、依赖、验收与输出格式；父代理复核并协调共享 CPU/构建资源。
