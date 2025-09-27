最低要求：  \
C++17，64位系统，CMake。  \
绝大多数优化仅针对x64。 目前异常处理支持不佳。  

## todo
1. AMD上性能暂未测试调优（优先级低）
2. 可抢占调度器(EEVDF)
3. 协程库
4. 修复动态库下的unique_stack_allocator（优先级低）
5. JSON封装（目前封装性能一般，并非零开销）
6. biginteger完善，lazy计算（优先级低）
7. C++17 mdspan支持，以及numpy风格数据结构+算法
8. ring_buffer完善
9. log（优先级低）
10. malloc库替换
11. ranges支持
12. 模块（优先级低）

# coverage
```
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release -DWJR_BUILD_TESTS=ON -DWJR_ENABLE_COVERAGE=ON \
-DWJR_DEBUG_LEVEL_RELEASE=3

cmake --build build --target coverage
```