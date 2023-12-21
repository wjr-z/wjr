#include <chrono>
#include <iostream>
#include <random>
#include <wjr/atomic.hpp>
#include <wjr/math/add.hpp>
#include <wjr/math/sub.hpp>
#include <wjr/preprocessor.hpp>

extern auto foo(uint64_t a, uint64_t b, uint64_t c, uint64_t &d) {
    return wjr::addc(a, b, 1, d);
}

int main() { return 0; }