
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

struct table {
    void (*clear_if_reserved)(biginteger_data *, uint32_t);
};

template <typename T>
struct tt {
    static constexpr table tc = {
        [](biginteger_data *x, uint32_t n) { ((T *)x)->clear_if_reserved(n); }};
};

struct biginteger_dispatcher {

    template <typename T>
    biginteger_dispatcher(T *p) noexcept : ptr(p->__get_data()), table(&tt<T>::tc) {}

    void clear_if_reserved(uint32_t n) { table->clear_if_reserved(ptr, n); }

    uint64_t *data() const { return ptr->data(); }

    void set_ssize(int32_t n) const { ptr->set_ssize(n); }

    biginteger_data *ptr;
    const table *table;
};

int main() {
    {
        capture_leaf<int> x;
        auto &z = container_of<capture_leaf<int>>(x.get());

        compressed_pair<int, double> y;
        auto &z2 = container_of<compressed_pair<int, double>, double>(y.second());

        vector<int> vec;
        auto& s = vec.get_storage();
        
        container_of<vector<int>>(s);
        return 0;
    }
    // {
    //     int T = 1e6;
    //     test([&]() {
    //         for (int i = 0; i < T; ++i) {
    //             biginteger x;
    //             biginteger_dispatcher dst(&x);
    //             asm volatile("" : "+r"(dst.ptr)::"memory");
    //             dst.clear_if_reserved(1);
    //             dst.data()[0] = i;
    //             dst.set_ssize(i == 0 ? 0 : 1);
    //         }
    //     });

    //     test([&]() {
    //         for (int i = 0; i < T; ++i) {
    //             biginteger dst;
    //             dst.clear_if_reserved(1);
    //             dst.data()[0] = i;
    //             dst.set_ssize(i == 0 ? 0 : 1);
    //         }
    //     });
    // }
    return 0;
}

/*
(( (x * 2 ^ 64) / 10 ^ 19 + 1) * 10 ^ k) / 2 ^ 64

*/