bison calc.y -o calc.tab.h
flex --c++ -ocalc.cpp calc.l
g++ -O3 -std=c++17 -DNDEBUG -march=native -DWJR_DEBUG_LEVEL=0 \
-I ../../include/ calc.cpp ../../src/wjr/math.cpp ../../src/wjr/x86/math/mul.cpp -o calc
