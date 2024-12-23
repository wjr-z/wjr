
#include <atomic>
#include <chrono>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <variant>
#include <vector>

#include <condition_variable>

#include <charconv>
#include <queue>
#include <wjr/biginteger.hpp>
#include <wjr/concurrency/pause.hpp>
#include <wjr/expected.hpp>
#include <wjr/format/dragonbox.hpp>
#include <wjr/format/fastfloat.hpp>
#include <wjr/simd/simd.hpp>

#include <wjr/algorithm.hpp>
#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>

#include <wjr/json/document.hpp>

#include "simdjson.hpp"

#include <gmp.h>

namespace wjr {
template class btree_map<int, int, std::less<int>>;
}

// #include <gmp.h>

auto get() { return std::chrono::high_resolution_clock::now().time_since_epoch().count(); }

using namespace wjr;

template <typename Func>
auto test(Func fn) {
    auto s = get();
    fn();
    auto x = get() - s;
    std::cout << x << std::endl;
    return x;
}

struct comp {
    template <typename T>
    bool operator()(const T &a, const T &b) const {
        return a < b;
    }
};

static std::mt19937 mt_rand(std::random_device{}());

template <typename T, typename Settings = void>
struct trandom_fn {};

template <typename T, typename Settings = void>
inline constexpr trandom_fn<T, Settings> trandom{};

template <>
struct trandom_fn<int, void> {
    int operator()() const { return mt_rand(); }
};

template <>
struct trandom_fn<std::string, void> {
    std::string operator()() const {
        std::string s;
        size_t n = mt_rand() % 64;
        for (size_t i = 0; i < n; ++i) {
            s.push_back(static_cast<char>(mt_rand()));
        }

        return s;
    }
};

static uint64_t cnt = 0;
static uint64_t cnt2 = 0;
static uint64_t cnt96 = 0;
static uint64_t cnt160 = 0;
static uint64_t size = 0;
static uint64_t size2 = 0;

void *operator new(std::size_t n) noexcept(false) {
    ++cnt;
    size += n;
    if (n == 96) {
        ++cnt96;
    } else if (n == 160)
        ++cnt160;
    return malloc(n);
}
void operator delete(void *p) throw() {
    ++cnt2;
    free(p);
}

void *operator new[](std::size_t s) noexcept(false) {
    ++cnt;
    size += s;
    if (s == 96) {
        ++cnt96;
    } else if (s == 160)
        ++cnt160;
    return malloc(s);
}
void operator delete[](void *p) throw() {
    ++cnt2;
    free(p);
}

using Al = std::allocator<char>;
using Alt = std::allocator_traits<Al>;

auto allocate_node(size_t n) -> void * {
    Al al;
    return Alt::allocate(al, n);
}

int main() {
    while (0) {
        int n = 1024;
        int m = 6e8 / (n * log2(n) + 8);

        test([&]() {
            for (int j = 0; j < m; ++j) {
                vector<void *> vec;
                for (int i = 0; i < n; ++i) {
                    if (i % 33 == 0) {
                        vec.push_back(allocate_node(160));
                    }
                    if (i % 6 == 0) {
                        vec.push_back(allocate_node(96));
                    }
                    vec.push_back(allocate_node(8));
                }
                for (auto i : vec) {
                    free(i);
                }
            }
        });

        test([&]() {
            for (int j = 0; j < m; ++j) {
                vector<void *> vec;
                for (int i = 0; i < n; ++i) {
                    vec.push_back(malloc(40));
                }
                for (auto i : vec) {
                    free(i);
                }
            }
        });

        return 0;
    }

    // for (int n = 8; n <= 1e5; n <<= 1)
    {
        int n = 1024;
        vector<int> vec;
        std::mt19937 mt_rand(std::random_device{}());
        int m = 1e4;
        for (int i = 0; i < n; ++i)
            vec.emplace_back(trandom<int>());
        std::cout << "n = " << n << std::endl;
        test([&]() {
            btree_map<int, int> bt;
            for (auto i : vec) {
                bt.emplace(i, i);
            }
            for (int j = 0; j < m; ++j) {
                for (auto i : vec) {
                    auto iter = bt.lower_bound(i);
                    if (iter != bt.end()) {
                        asm volatile("" : "+r"(iter->second));
                    }
                }
            }
        });

        test([&]() {
            std::map<int, int> bt;
            for (auto i : vec) {
                bt.emplace(i, i);
            }
            for (int j = 0; j < m; ++j) {
                for (auto i : vec) {
                    auto iter = bt.lower_bound(i);
                    if (iter != bt.end()) {
                        asm volatile("" : "+r"(iter->second));
                    }
                }
            }
        });

        // test([&]() {
        //     for (int j = 0; j < m; ++j) {
        //         cnt = cnt2 = size = 0;
        //         cnt96 = cnt160 = 0;
        //         std::map<int, int, comp> bt;
        //         for (auto i : vec) {
        //             bt.emplace(i, i);
        //         }
        //         std::cout << cnt << ' ' << cnt2 << ' ' << size << std::endl;
        //         std::cout << cnt96 << ' ' << cnt160 << std::endl;
        //     }
        // });
        // std::cout << cnt << ' ' << cnt2 << ' ' << size << std::endl;
        return 0;
    }

    for (int n = 8; n <= 1e5; n <<= 1) {
        vector<std::string> vec;
        std::mt19937 mt_rand(std::random_device{}());
        int m = 2e7 / (n * log2(n) + 8);
        for (int i = 0; i < n; ++i)
            vec.emplace_back(trandom<std::string>());
        std::cout << "n = " << n << std::endl;
        test([&]() {
            for (int j = 0; j < m; ++j) {
                btree_map<std::string, std::string, comp> bt;
                for (auto &i : vec) {
                    bt.emplace(i, i);
                }
            }
        });
        test([&]() {
            for (int j = 0; j < m; ++j) {
                std::map<std::string, std::string, comp> bt;
                for (auto &i : vec) {
                    bt.emplace(i, i);
                }
            }
        });
    }

    return 0;
}

/*
(( (x * 2 ^ 64) / 10 ^ 19 + 1) * 10 ^ k) / 2 ^ 64

*/