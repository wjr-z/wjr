
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
    bool operator()(const int a, const int b) const {
        ++cnt;
        return a < b;
    }
};

int main() {
    {
        vector<int> vec;
        std::mt19937 mt_rand(std::random_device{}());
        int n = 1e6;
        for (int i = 0; i < n; ++i)
            vec.emplace_back(i);
        test([&]() {
            btree_map<int, int, comp> bt;
            for (auto i : vec) {
                bt.emplace(i, i);
            }

            // cnt = 0;

            // for (auto i : vec) {
            //     (void)bt.lower_bound(i);
            // }
        });
        std::cout << cnt << '\n';

        test([&]() {
            std::map<int, int, comp> bt;
            for (auto i : vec) {
                bt.emplace(i, i);
            }

            cnt = 0;

            for (auto i : vec) {
                (void)bt.lower_bound(i);
            }
        });
        std::cout << cnt << '\n';
        return 0;
    }
    {
        std::ifstream in("../data/twitter.json");
        std::stringstream buffer;
        buffer << in.rdbuf();
        std::string str = buffer.str();

        test([&]() {
            json::reader rd;
            for (int i = 0; i < 100; ++i) {
                rd.read(str);
                (void)json::document::parse(rd).value();
                // (void)json::check(rd);
            }
        });

        test([&]() {
            for (int i = 0; i < 500; ++i) {
                simdjson::dom::parser parser;
                (void)parser.parse(str);
            }
        });
    }
    {
        std::ifstream in("../data/new.json");
        std::stringstream buffer;
        buffer << in.rdbuf();
        std::string str = buffer.str();

        test([&]() {
            json::reader rd;
            for (int i = 0; i < 500; ++i) {
                rd.read(str);
                (void)json::document::parse(rd).value();
                // (void)json::check(rd);
            }
        });

        test([&]() {
            for (int i = 0; i < 500; ++i) {
                simdjson::dom::parser parser;
                auto it = parser.parse(str);
                // simdjson::ondemand::parser parser;
                // parser.iterate(str);
            }
        });
        return 0;
    }
    {
        std::mt19937_64 mt_rand(std::random_device{}());
        int m = 4000;
        int T = 1e8 / m;
        uint64_t c[1] = {333};
        vector<uint64_t> dst(m + 1);
        vector<uint64_t> tmp(m + 1);

        biginteger a, b;
        b = 333;

        size_t n;
        test([&]() {
            for (int i = 0; i < T; ++i) {
                pow(a, b, m);
            }
        });

        char buf0[12000], buf1[12000];
        size_t l0, l1;
        l0 = biginteger_to_chars(buf0, a.data(), a.size()) - buf0;

        mpz_t a2, b2;
        mpz_init_set_ui(a2, 0);
        mpz_init_set_ui(b2, 333);

        test([&]() {
            for (int i = 0; i < T; ++i) {
                mpz_pow_ui(a2, b2, m);
            }
        });

        mpz_get_str(buf1, 10, a2);
        l1 = strlen(buf1);
    }

    return 0;
}

/*
(( (x * 2 ^ 64) / 10 ^ 19 + 1) * 10 ^ k) / 2 ^ 64

*/