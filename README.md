最低要求：  \
C++17，64位系统，CMake。  \
绝大多数优化仅针对x64。 目前异常处理支持不佳。  

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

# coverage
```
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release -DWJR_BUILD_TESTS=ON -DWJR_ENABLE_COVERAGE=ON \
-DWJR_DEBUG_LEVEL_RELEASE=3

cmake --build build --target coverage
```