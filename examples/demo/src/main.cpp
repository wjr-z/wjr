
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

uint64_t cnt = 0;

struct comp {
    template <typename T>
    bool operator()(const T &a, const T &b) const {
        ++cnt;
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

int main() {
    {
        vector<int> vec;
        std::mt19937 mt_rand(std::random_device{}());
        int n = 1e3;
        int m = 1e4;
        for (int i = 0; i < n; ++i)
            vec.emplace_back(trandom<int>());
        test([&]() {
            for (int j = 0; j < m; ++j) {
                btree_map<int, int, comp> bt;
                for (auto i : vec) {
                    bt.emplace(i, i);
                }
            }

            // cnt = 0;

            // for (auto i : vec) {
            //     (void)bt.lower_bound(i);
            // }
        });
        std::cout << cnt << '\n';

        test([&]() {
            for (int j = 0; j < m; ++j) {
                std::map<int, int, comp> bt;
                for (auto i : vec) {
                    bt.emplace(i, i);
                }
            }

            // cnt = 0;

            // for (auto i : vec) {
            //     (void)bt.lower_bound(i);
            // }
        });
        std::cout << cnt << '\n';
        return 0;
    }

    {
        vector<std::string> vec;
        std::mt19937 mt_rand(std::random_device{}());
        int n = 1e6;
        for (int i = 0; i < n; ++i)
            vec.emplace_back(trandom<std::string>());
        test([&]() {
            btree_map<std::string, std::string, comp> bt;
            for (auto &i : vec) {
                bt.emplace(i, i);
            }

            // cnt = 0;

            // for (auto &i : vec) {
            //     (void)bt.lower_bound(i);
            // }
        });
        std::cout << cnt << '\n';

        test([&]() {
            std::map<std::string, std::string, comp> bt;
            for (auto &i : vec) {
                bt.emplace(i, i);
            }

            // cnt = 0;

            // for (auto &i : vec) {
            //     (void)bt.lower_bound(i);
            // }
        });
        std::cout << cnt << '\n';
    }

    return 0;
}

/*
(( (x * 2 ^ 64) / 10 ^ 19 + 1) * 10 ^ k) / 2 ^ 64

*/