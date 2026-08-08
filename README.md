最低要求：  \
C++17，64位系统，CMake。  \
绝大多数优化仅针对x64。 目前异常处理支持不佳。  

# 构建 target
项目使用 CMake 构建。配置完成后，可以通过下面的命令构建指定 target：

```sh
cmake --build build --target <target>
```

常用 target 的用途和使用方式如下：

- `wjr`：WJR 的最终库，包含项目的主要模块。普通项目需要使用 WJR 时构建它；在自己的 CMake 项目中通常链接 `wjr::wjr`。
	```cmake
	target_link_libraries(my_app PRIVATE wjr::wjr)
	```
- `wjr::<module>`：单个模块的 CMake alias。各模块的用途、适用场景和依赖方式见 [模块说明](docs/modules/README.md)；对应的实际构建 target 名称为 `wjr-<module>`。
	```cmake
	target_link_libraries(my_app PRIVATE wjr::json)
	```
- `tests`：测试可执行文件。修改代码后需要编译并运行单元测试时使用。配置时添加 `-DWJR_BUILD_TESTS=ON`，然后执行：
	```sh
	cmake --build build --target tests
	ctest --test-dir build --output-on-failure
	```
- `coverage`：运行测试并生成覆盖率报告。需要检查测试覆盖范围时使用；配置时同时添加 `-DWJR_BUILD_TESTS=ON` 和 `-DWJR_ENABLE_COVERAGE=ON`，并确保环境中有 `llvm-cov` 和 `llvm-profdata`。
	```sh
	cmake --build build --target coverage
	```
	报告生成在 `build/coverage` 目录。
- `benchmarks`：基准测试可执行文件。需要比较性能或评估优化效果时使用；配置时添加 `-DWJR_BUILD_BENCHMARKS=ON`，然后执行：
	```sh
	cmake --build build --target benchmarks
	```
	可执行文件位于 `build/bin` 目录。

## todo
- [ ] AMD上性能暂未测试调优（优先级低）
- [ ] 可抢占调度器(EEVDF)
- [ ] 协程库
- [ ] 修复动态库下的unique_stack_allocator（优先级低）
- [ ] JSON封装（目前封装性能一般，并非零开销）
- [x] biginteger完善，lazy计算（优先级低）
- [ ] C++17 mdspan支持，以及numpy风格数据结构+算法
- [ ] ring_buffer完善
- [ ] log（优先级低）
- [ ] malloc库替换
- [ ] ranges支持
- [ ] 模块（优先级低）
- [ ] MSVC 编译flag
- [ ] 日志、告警

# test
```
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release -DWJR_BUILD_TESTS=ON \
-DWJR_ENABLE_COVERAGE=ON \
-DWJR_DEBUG_LEVEL_RELEASE=3

cmake --build build --target coverage
```
