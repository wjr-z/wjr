#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <variant>
#include <vector>

#include <thread>

#include <wjr/network/timer.hpp>

#include <condition_variable>

// #include <wjr/biginteger.hpp>
// #include <wjr/bplus_tree.hpp>
// #include <wjr/memory/safe_pointer.hpp>

auto get() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

using namespace wjr;

template <typename Func>
void test(Func fn) {
    auto s = get();
    fn();
    std::cout << get() - s << std::endl;
}

// struct node {
//     node() = default;
//     node(const node &) = default;
//     node &operator=(const node &) = delete;
//     ~node() = default;
// };

// extern "C" void __gmpn_mul_basecase(mp_ptr, mp_srcptr, mp_size_t, mp_srcptr,
// mp_size_t);

// extern "C" void __gmpn_toom22_mul(mp_ptr, mp_srcptr, mp_size_t, mp_srcptr, mp_size_t,
//                                   mp_ptr);

// extern "C" void __gmpn_toom42_mul(mp_ptr, mp_srcptr, mp_size_t, mp_srcptr, mp_size_t,
//                                   mp_ptr);

// extern "C" void __gmpn_toom63_mul(mp_ptr, mp_srcptr, mp_size_t, mp_srcptr, mp_size_t,
//                                   mp_ptr);

#include <signal.h>
#include <stdio.h>
#include <time.h>

std::chrono::high_resolution_clock::time_point start;

auto getx() { return clock(); }

uint64_t S;

uint64_t cnt = 0;
int step = 0;

const int N = 1e7;
const int M = 1e5;

WJR_NOINLINE void handle(int) {
    auto x = std::chrono::high_resolution_clock::now();
    cnt += (x - start).count();
    start = x;
    ++step;
    if (step == N) {
        std::cout << cnt << ' ' << (cnt / step) << '\n';
        std::cout << "CPU : " << (getx() - S) << '\n';
        exit(0);
    }
}

void foo2() {
    std::mutex mt;
    std::condition_variable cv;
    auto now = std::chrono::high_resolution_clock::now();
    while (true) {
        now += std::chrono::nanoseconds(200'000);
        {
            std::unique_lock lk(mt);
            cv.wait_until(lk, now);
        }
        handle(0);
    }
}

void foo3() {
    auto now = std::chrono::high_resolution_clock::now();
    while (true) {
        now += std::chrono::nanoseconds(5'000'000);
        std::this_thread::sleep_until(now);
        for (int i = 0; i < M; ++i) {
            handle(0);
        }
    }
}

template <typename Allocator>
struct allocator_delete {
    template <typename T>
    void operator()(T *ptr) const {
        Allocator alloc;
        alloc.deallocate(ptr, 1);
    }
};

void foo4() {
    auto now = std::chrono::high_resolution_clock::now();

    timing_wheels timer(0);

    for (int i = 0; i < M; ++i) {
        timer.add([]() { handle(0); }, 1);
    }

    while (true) {
        now += std::chrono::nanoseconds(5'000'000);
        std::this_thread::sleep_until(now);

        timer.next();
        timer.run();

        for (int i = 0; i < M; ++i) {
            timer.add([]() { handle(0); }, 1);
        }
    }
}

int main() {

    // test([&]() {
    //     for (int i = 0; i < M; ++i) {
    //         timing_wheels t(0);
    //         asm volatile("" ::: "memory");
    //     }
    // });

    // return 0;
    
    S = getx();
    start = std::chrono::high_resolution_clock::now();
    foo4();
}

// int main() {

//     timer_create()

//     test([&]() {
//         const int N = 100;
//         for (int i = 0; i < N; ++i) {
//             std::this_thread::sleep_for(std::chrono::milliseconds(1));
//         }
//     });

//     // {
//     //     const int N = 1e7;
//     //     test([&]() {
//     //         for (int i = 0; i < N; ++i) {
//     //             auto ptr = malloc((ctz(to_unsigned(i) + 1) + 1) * 8);
//     //             asm volatile("" : "+r"(ptr));
//     //             free(ptr);
//     //         }
//     //     });
//     // }

//     // return 0;
//     // for (int n = 500; n <= 4000; n += 64) {
//     //     std::mt19937_64 mt_rand(time(0));

//     //     int m = n;
//     //     const int N = 1e8 / pow(n, 1.57);
//     //     vector<uint64_t> a(n), b(n), c(n * 2), d(n * 2), e(n * 20);

//     //     for (auto &i : a) {
//     //         i = mt_rand();
//     //         // i = 1ull << 63;
//     //     }

//     //     for (auto &i : b) {
//     //         i = mt_rand();
//     //         // i = 1ull << 63;
//     //     }

//     //     std::cout << "m = " << m << std::endl;

//     //     std::cout << "toom-cook-44 : ";
//     //     test([&]() {
//     //         for (int i = 0; i < N; ++i) {
//     //             // basecase_mul_s(c.data(), a.data(), n, b.data(), m);
//     //             toom44_mul_s(c.data(), a.data(), n, b.data(), m, span<uint64_t>(e));
//     //             // rsblsh_n(c.data(), a.data(), b.data(), n, 2);
//     //         }
//     //     });

//     //     std::cout << "toom-cook-55 : ";
//     //     test([&]() {
//     //         for (int i = 0; i < N; ++i) {
//     //             // __gmpn_toom22_mul(c.data(), a.data(), n, b.data(), m, e.data());
//     //             toom55_mul_s(d.data(), a.data(), n, b.data(), m, span<uint64_t>(e));
//     //             // rsblsh_n(c.data(), a.data(), b.data(), n, 2);
//     //         }
//     //     });

//     //     // std::cout << "toom-cook-44 : ";
//     //     // test([&]() {
//     //     //     for (int i = 0; i < N; ++i) {
//     //     //         toom44_mul_s(d.data(), a.data(), n, b.data(), m,
//     span<uint64_t>(e));
//     //     //         // toom63_mul_s(c.data(), a.data(), n, b.data(), m,
//     //     span<uint64_t>(e));
//     //     //         // rsblsh_n(c.data(), a.data(), b.data(), n, 2);
//     //     //         // submul_1(a.data(), b.data(), n, 2);
//     //     //     }
//     //     // });

//     //     // std::cout << "mpn_mul      : ";
//     //     // test([&]() {
//     //     //     for (int i = 0; i < N; ++i) {
//     //     //         mpn_mul(e.data(), a.data(), n, b.data(), m);
//     //     //         // toom44_mul_s(c.data(), a.data(), n, b.data(), m,
//     //     span<uint64_t>(e));
//     //     //         // toom63_mul_s(c.data(), a.data(), n, b.data(), m,
//     //     span<uint64_t>(e));
//     //     //         // rsblsh_n(c.data(), a.data(), b.data(), n, 2);
//     //     //         // submul_1(a.data(), b.data(), n, 2);
//     //     //     }
//     //     // });

//     //     // test([&]() {
//     //     //     for (int i = 0; i < N; ++i) {
//     //     //         // toom42_mul_s(d.data(), a.data(), n, b.data(), m,
//     //     span<uint64_t>(e));
//     //     //         // mul_s(d.data(), a.data(), n, b.data(), m);
//     //     //         // mpn_mul(d.data(), a.data(), n, b.data(), m);
//     //     //         __gmpn_toom63_mul(d.data(), a.data(), n, b.data(), m, e.data());
//     //     //     }
//     //     // });

//     //     // if (!std::equal(c.data(), c.data() + n + m, d.data())) {
//     //     //     std::cout << "error\n";
//     //     //     for (int i = 0; i < n + m; ++i) {
//     //     //         std::cout << c[i] << ',';
//     //     //     }
//     //     //     std::cout << '\n';
//     //     //     for (int i = 0; i < n + m; ++i) {
//     //     //         std::cout << d[i] << ',';
//     //     //     }
//     //     //     std::cout << '\n';
//     //     //     return 0;
//     //     // }
//     // }

//     // return 0;

//     // // wjr::node pp;
//     // // int xn = 0, xm = 0;
//     // // pp.reset();
//     // // for (int n = WJR_TOOM22_MUL_THRESHOLD; n < 800; ++n) {
//     // //     for (int m = WJR_TOOM22_MUL_THRESHOLD; m <= n; m += 2) {
//     // //         // if (!toom55_ok(n, m)) {
//     // //         //     continue;
//     // //         // }

//     // //         std::mt19937_64 mt_rand(time(0));

//     // //         const int N = 1e4;
//     // //         vector<uint64_t> a(n), b(n), c(n * 2), d(n * 2), e(n * 20);

//     // //         for (auto &i : a) {
//     // //             i = mt_rand();
//     // //         }

//     // //         for (auto &i : b) {
//     // //             i = mt_rand();
//     // //         }

//     // //         wjr::it.reset();
//     // //         mul_s(c.data(), a.data(), n, b.data(), m);
//     // //         auto x = it.get();
//     // //         // printf("%d : %zu (%zu, %zu, %d)\n", n, x, x / m, x % m, x - m *
//     2);
//     // //         const auto xx = x - m * 4 - m / 2;
//     // //         if (xx > pp.get()) {
//     // //             xn = n;
//     // //             xm = m;
//     // //         }
//     // //         pp.update(xx);
//     // //     }
//     // //     printf("%d %ld", n, pp.get());
//     // //     std::cout << std::endl;
//     // // }
//     // // printf("%d\n", pp.get());
//     // // printf("%d %d\n", xn, xm);
//     // // return 0;

//     // {
//     //     biginteger x;
//     //     x = 3;
//     //     pow(x, x, 1000000);
//     //     vector<char> vec;
//     //     std::cout << x.size() << '\n';
//     //     auto s = get();
//     //     biginteger_to_chars(std::back_inserter(vec), x.data(), x.size());
//     //     std::cout << get() - s << std::endl;
//     //     std::cout << vec.size() << '\n';
//     // }
//     // {
//     //     mpz_t x;
//     //     mpz_init_set_ui(x, 3);
//     //     mpz_pow_ui(x, x, 1000000);
//     //     vector<char> vec(477122, dctor);
//     //     std::cout << mpz_size(x) << '\n';
//     //     auto s = get();
//     //     mpz_get_str(vec.data(), 10, x);
//     //     std::cout << get() - s << std::endl;
//     // }
//     return 0;
// }