#include <algorithm>
#include <list>
#include <string>
#include <string_view>
#include <vector>

#include "random.hpp"

#include <wjr/math.hpp>
#include <wjr/vector.hpp>

inline const int __int = 5082;
inline const int __int2 = 123;
inline const std::string __string = std::string("wjr");
inline const std::string __string2 = std::string("abc");

using namespace wjr;

static_assert(is_contiguous_iterator_v<typename vector<int>::iterator>, "");
static_assert(is_contiguous_iterator_v<typename vector<int>::const_iterator>, "");

static_assert(is_contiguous_iterator_v<typename vector<std::string>::iterator>, "");
static_assert(is_contiguous_iterator_v<typename vector<std::string>::const_iterator>, "");

static_assert(is_trivially_allocator_v<std::allocator<int>>, "");

template <typename Iter, typename Func>
void for_each_n(Iter first, size_t n, Func fn) {
    for (size_t i = 0; i < n; ++i) {
        fn(first[i]);
    }
}

template <typename Func>
void run_range(Func fn) {
    for (int i = 0; i < 32; ++i) {
        fn(i);
    }
    for (int i = 32; i < 128; i += 8) {
        fn(i);
        fn(i + 1);
    }
    for (int i = 128; i < 256; i += 32) {
        fn(i);
        fn(i + 1);
        fn(i + 2);
        fn(i + 3);
        fn(i + 7);
    }
    for (int i = 256; i < 1024; i <<= 1) {
        fn(i);
    }
}

template <typename Func>
void run_range2(Func fn) {
    // size(), capacity()
    auto gn = [&fn](int i) {
        // size() < capacity()
        fn(0, i);
        fn(1, i);
        fn(i, i + 1);
        fn(i, i + 2);
        fn(i, std::min(i * 2, i + 128));
        // size() == capacity()
        fn(i, i);
        // size() > capacity()
        fn(i + 1, i);
        fn(i + 2, i);
        fn(std::min(i * 2, i + 128), i);
    };
    run_range(gn);
}

template <typename Func>
void run_range3(Func fn) {
    // n size(), capacity()
    auto gn = [&fn](int i) {
        // n < size() < capacity()
        fn(0, i, i + 1);
        fn(1, i, i + 1);
        fn(i, i + 1, i + 2);
        // n == size() < capacity()
        fn(0, 0, i);
        fn(1, 1, i);
        fn(i, i, i + 1);
        // size() < n < capacity()
        fn(1, 0, i);
        fn(i, 0, i + 1);
        fn(i, 1, i + 1);
        fn(i + 1, i, i + 2);
        // size() < n == capacity()
        fn(i, 0, i);
        fn(i + 1, i, i + 1);
        // size() < capacity() < n
        fn(i + 1, 0, i);
        fn(i + 1, 1, i);
        fn(i + 2, i, i + 1);
        // n < size() == capacity()
        fn(0, i, i);
        fn(1, i, i);
        fn(i, i + 1, i + 1);
        // size() == capacity() < n
        fn(i + 1, 0, 0);
        fn(i + 1, 1, 1);
        fn(i + 1, i, i);
    };
    run_range(gn);
}

using int_ilist = std::initializer_list<int>;
using string_ilist = std::initializer_list<std::string>;

template <typename T>
using wvector = wjr::vector<T>;

namespace wjr {
template class basic_vector<default_vector_storage<int, std::allocator<int>>>;
template class basic_vector<default_vector_storage<std::string, std::allocator<std::string>>>;
} // namespace wjr

TEST(vector, construct) {

    // default
    {
        auto test = [](auto _Val) {
            using T = decltype(_Val);
            wvector<T> v;
            EXPECT_EQ(v.size(), 0);
            EXPECT_GE(v.capacity(), 0);
        };
        test(__int);
        test(__string);
    }
    {
        auto test = [](auto _Val) {
            using T = decltype(_Val);
            std::allocator<T> al;
            wvector<T> v(al);
            EXPECT_EQ(v.size(), 0);
            EXPECT_GE(v.capacity(), 0);
        };
        test(__int);
        test(__string);
    }
    {
        auto test = [](auto _Val, size_t n) {
            using T = decltype(_Val);
            std::allocator<T> al;
            wvector<T> v(n, al);
            EXPECT_EQ(v.size(), n);
            EXPECT_GE(v.capacity(), n);
            for_each_n(v.begin(), n, [](auto &x) { EXPECT_EQ(x, T()); });
        };
        run_range([&](int i) {
            test(__int, i);
            test(__string, i);
        });
    }
    {
        auto test = [](auto _Val, size_t n) {
            using T = decltype(_Val);
            std::allocator<T> al;
            wvector<T> V(n, _Val, al);
            wvector<T> v(V);
            EXPECT_EQ(v.size(), n);
            EXPECT_GE(v.capacity(), n);
            EXPECT_EQ(V.size(), n);
            EXPECT_GE(V.capacity(), n);
            for_each_n(v.begin(), n, [&_Val](auto &x) { EXPECT_EQ(x, _Val); });
            for_each_n(V.begin(), n, [&_Val](auto &x) { EXPECT_EQ(x, _Val); });
        };
        run_range([&](int i) {
            test(0, i);
            test(1, i);
            test(trandom<int>(), i);
            test(std::string(""), i);
            test(std::string("a"), i);
            test(trandom<std::string>(), i);
        });
    }
    {
        auto test = [](auto _Val, size_t n) {
            using T = decltype(_Val);
            std::allocator<T> al;
            wvector<T> V(n, _Val, al);
            wvector<T> v(V, al);
            EXPECT_EQ(v.size(), n);
            EXPECT_GE(v.capacity(), n);
            EXPECT_EQ(V.size(), n);
            EXPECT_GE(V.capacity(), n);
            for_each_n(v.begin(), n, [&_Val](auto &x) { EXPECT_EQ(x, _Val); });
            for_each_n(V.begin(), n, [&_Val](auto &x) { EXPECT_EQ(x, _Val); });
        };
        run_range([&](int i) {
            test(0, i);
            test(1, i);
            test(trandom<int>(), i);
            test(std::string(""), i);
            test(std::string("a"), i);
            test(trandom<std::string>(), i);
        });
    }
    {
        auto test = [](auto _Val, size_t n) {
            using T = decltype(_Val);
            std::allocator<T> al;
            wvector<T> V(n, _Val, al);
            wvector<T> v(std::move(V));
            EXPECT_EQ(v.size(), n);
            EXPECT_GE(v.capacity(), n);
            for_each_n(v.begin(), n, [&_Val](auto &x) { EXPECT_EQ(x, _Val); });
        };
        run_range([&](int i) {
            test(__int, i);
            test(__string, i);
        });
    }
    {
        auto test = [](auto _Val, size_t n) {
            using T = decltype(_Val);
            std::allocator<T> al;
            wvector<T> V(n, _Val, al);
            wvector<T> v(std::move(V), al);
            EXPECT_EQ(v.size(), n);
            EXPECT_GE(v.capacity(), n);
            for_each_n(v.begin(), n, [&_Val](auto &x) { EXPECT_EQ(x, _Val); });
        };
        run_range([&](int i) {
            test(__int, i);
            test(__string, i);
        });
    }
    {
        auto test = [](auto _Val, int n, auto first, auto last) {
            using T = decltype(_Val);
            std::allocator<T> al;
            wvector<T> v(first, last, al);
            EXPECT_EQ(v.size(), n);
            EXPECT_GE(v.capacity(), n);
            EXPECT_TRUE(std::equal(first, last, v.begin()));
        };
        run_range([&](int i) {
            {
                std::vector<int> vec(i);
                std::generate_n(vec.begin(), i, trandom<int>);
                test(__int, i, vec.begin(), vec.end());
            }
            {
                std::vector<std::string> vec(i);
                std::generate_n(vec.begin(), i, trandom<std::string>);
                test(__string, i, vec.begin(), vec.end());
            }

            {
                std::list<int> vec;
                std::generate_n(std::back_inserter(vec), i, trandom<int>);
                test(__int, i, vec.begin(), vec.end());
            }
            {
                std::list<std::string> vec;
                std::generate_n(std::back_inserter(vec), i, trandom<std::string>);
                test(__string, i, vec.begin(), vec.end());
            }
        });
    }
    {
        auto test = [](auto _Val, auto il) {
            using T = decltype(_Val);
            auto n = il.size();
            std::allocator<T> al;
            wvector<T> v(il, al);
            EXPECT_EQ(v.size(), n);
            EXPECT_GE(v.capacity(), n);
            EXPECT_TRUE(std::equal(il.begin(), il.end(), v.begin()));
        };

        test(__int, int_ilist{});
        test(__int, int_ilist{1});
        test(__int, int_ilist{1, 2});
        test(__int, int_ilist{trandom<int>(), trandom<int>(), trandom<int>(), trandom<int>()});
        test(__int, int_ilist{trandom<int>(), trandom<int>(), trandom<int>(), trandom<int>(),
                              trandom<int>(), trandom<int>(), trandom<int>(), trandom<int>()});
        test(__string, string_ilist{});
        test(__string, string_ilist{trandom<std::string>()});
        test(__string, string_ilist{trandom<std::string>(), trandom<std::string>()});
        test(__string, string_ilist{trandom<std::string>(), trandom<std::string>(),
                                    trandom<std::string>(), trandom<std::string>()});
        test(__string,
             string_ilist{trandom<std::string>(), trandom<std::string>(), trandom<std::string>(),
                          trandom<std::string>(), trandom<std::string>(), trandom<std::string>(),
                          trandom<std::string>(), trandom<std::string>()});
    }
}

TEST(vector, assignment) {
    // vector& operator=(const vector&)
    {
        auto test = [](auto _Val, size_t n, size_t s, size_t c) {
            using T = decltype(_Val);
            std::allocator<T> al;
            wvector<T> V(n, _Val, al);
            wvector<T> v(c);
            v.resize(s);
            EXPECT_EQ(v.size(), s);
            EXPECT_GE(v.capacity(), c);
            v = V;
            EXPECT_EQ(v.size(), n);
            EXPECT_GE(v.capacity(), c);
            EXPECT_EQ(V.size(), n);
            EXPECT_GE(V.capacity(), n);
            for_each_n(v.begin(), n, [&_Val](auto &x) { EXPECT_EQ(x, _Val); });
            for_each_n(V.begin(), n, [&_Val](auto &x) { EXPECT_EQ(x, _Val); });
        };
        run_range3([&](int n, int s, int c) {
            test(__int, n, s, c);
            test(__string, n, s, c);
        });
    }
    {
        auto test = [](auto _Val, size_t n, size_t s, size_t c) {
            using T = decltype(_Val);
            std::allocator<T> al;
            wvector<T> V(n, _Val, al);
            wvector<T> v(c);
            v.resize(s);
            v = std::move(V);
            EXPECT_EQ(v.size(), n);
            EXPECT_GE(v.capacity(), n);
            for_each_n(v.begin(), n, [&_Val](auto &x) { EXPECT_EQ(x, _Val); });
        };
        run_range3([&](int n, int s, int c) {
            test(__int, n, s, c);
            test(__string, n, s, c);
        });
    }
    {
        auto test = [](auto _Val, auto il) {
            using T = decltype(_Val);
            auto n = il.size();
            wvector<T> v;
            v = il;
            EXPECT_EQ(v.size(), n);
            EXPECT_GE(v.capacity(), n);
            EXPECT_TRUE(std::equal(il.begin(), il.end(), v.begin()));
        };

        test(__int, int_ilist{});
        test(__int, int_ilist{1});
        test(__int, int_ilist{1, 2});
        test(__int, int_ilist{trandom<int>(), trandom<int>(), trandom<int>(), trandom<int>()});
        test(__int, int_ilist{trandom<int>(), trandom<int>(), trandom<int>(), trandom<int>(),
                              trandom<int>(), trandom<int>(), trandom<int>(), trandom<int>()});
        test(__string, string_ilist{});
        test(__string, string_ilist{trandom<std::string>()});
        test(__string, string_ilist{trandom<std::string>(), trandom<std::string>()});
        test(__string, string_ilist{trandom<std::string>(), trandom<std::string>(),
                                    trandom<std::string>(), trandom<std::string>()});
        test(__string,
             string_ilist{trandom<std::string>(), trandom<std::string>(), trandom<std::string>(),
                          trandom<std::string>(), trandom<std::string>(), trandom<std::string>(),
                          trandom<std::string>(), trandom<std::string>()});
    }
}

TEST(vector, assign) {
    // assign(size_t _Count, const vlaue_type& _Val)
    {
        auto test = [](auto _Val, size_t n, size_t s, size_t c) {
            using T = decltype(_Val);
            wvector<T> v(c);
            v.resize(s);
            v.assign(n, _Val);
            EXPECT_EQ(v.size(), n);
            EXPECT_GE(v.capacity(), n);
            for_each_n(v.begin(), n,
                       [&](auto &x) { EXPECT_EQ(x, _Val) << n << ' ' << s << ' ' << c; });
        };
        run_range3([&](int n, int s, int c) {
            test(__int, n, s, c);
            test(__string, n, s, c);
        });
    }
    {
        auto test = [](auto _Val, int n, auto first, auto last) {
            using T = decltype(_Val);
            wvector<T> v;
            v.assign(first, last);
            EXPECT_EQ(v.size(), n);
            EXPECT_GE(v.capacity(), n);
            EXPECT_TRUE(std::equal(first, last, v.begin()));
        };

        run_range([&](int i) {
            {
                std::vector<int> vec(i);
                std::generate_n(vec.begin(), i, trandom<int>);
                test(__int, i, vec.begin(), vec.end());
            }
            {
                std::vector<std::string> vec(i);
                std::generate_n(vec.begin(), i, trandom<std::string>);
                test(__string, i, vec.begin(), vec.end());
            }

            {
                std::list<int> vec;
                std::generate_n(std::back_inserter(vec), i, trandom<int>);
                test(__int, i, vec.begin(), vec.end());
            }
            {
                std::list<std::string> vec;
                std::generate_n(std::back_inserter(vec), i, trandom<std::string>);
                test(__string, i, vec.begin(), vec.end());
            }
        });
    }
    {
        auto test = [](auto _Val, auto il) {
            using T = decltype(_Val);
            auto n = il.size();
            wvector<T> v;
            v.assign(il);
            EXPECT_EQ(v.size(), n);
            EXPECT_GE(v.capacity(), n);
            EXPECT_TRUE(std::equal(il.begin(), il.end(), v.begin()));
        };

        test(__int, int_ilist{});
        test(__int, int_ilist{1});
        test(__int, int_ilist{1, 2});
        test(__int, int_ilist{trandom<int>(), trandom<int>(), trandom<int>(), trandom<int>()});
        test(__int, int_ilist{trandom<int>(), trandom<int>(), trandom<int>(), trandom<int>(),
                              trandom<int>(), trandom<int>(), trandom<int>(), trandom<int>()});
        test(__string, string_ilist{});
        test(__string, string_ilist{trandom<std::string>()});
        test(__string, string_ilist{trandom<std::string>(), trandom<std::string>()});
        test(__string, string_ilist{trandom<std::string>(), trandom<std::string>(),
                                    trandom<std::string>(), trandom<std::string>()});
        test(__string,
             string_ilist{trandom<std::string>(), trandom<std::string>(), trandom<std::string>(),
                          trandom<std::string>(), trandom<std::string>(), trandom<std::string>(),
                          trandom<std::string>(), trandom<std::string>()});
    }
}

TEST(vector, resize) {
    {
        auto test = [](auto _Val, size_t n, size_t s, size_t c) {
            using T = decltype(_Val);
            wvector<T> v(c);
            v.resize(s);
            v.resize(n, _Val);
            EXPECT_EQ(v.size(), n);
            EXPECT_GE(v.capacity(), n);
            T val{};
            if (n > s) {
                for_each_n(v.begin(), s, [&](auto &x) { EXPECT_EQ(x, val); });
                for_each_n(v.begin() + s, n - s, [&](auto &x) { EXPECT_EQ(x, _Val); });
            } else {
                for_each_n(v.begin(), n, [&](auto &x) { EXPECT_EQ(x, val); });
            }
        };
        run_range3([&](int n, int s, int c) {
            test(__int, n, s, c);
            test(__string, n, s, c);
        });
    }

    {
        auto test = [](auto _Val, size_t n, size_t s, size_t c) {
            using T = decltype(_Val);
            wvector<T> v(c);
            v.resize(s);
            v.resize(n);
            EXPECT_EQ(v.size(), n);
            EXPECT_GE(v.capacity(), n);
            T val{};
            if (n > s) {
                for_each_n(v.begin(), s, [&](auto &x) { EXPECT_EQ(x, val); });
                for_each_n(v.begin() + s, n - s, [&](auto &x) { EXPECT_EQ(x, val); });
            } else {
                for_each_n(v.begin(), n, [&](auto &x) { EXPECT_EQ(x, val); });
            }
        };
        run_range3([&](int n, int s, int c) {
            test(__int, n, s, c);
            test(__string, n, s, c);
        });
    }
}

TEST(vector, reserve) {
    {
        auto test = [](auto _Val, size_t n, size_t s, size_t c) {
            using T = decltype(_Val);
            wvector<T> v(c, _Val);
            v.resize(s, _Val);
            v.reserve(n);
            EXPECT_EQ(v.size(), s);
            EXPECT_GE(v.capacity(), n);
            for_each_n(v.begin(), s, [&_Val](auto &x) { EXPECT_EQ(x, _Val); });
        };
        run_range3([&](int n, int s, int c) {
            test(__int, n, s, c);
            test(__string, n, s, c);
        });
    }
}

TEST(vector, shrink_to_fit) {
    {
        auto test = [](auto _Val, size_t s, size_t c) {
            using T = decltype(_Val);
            wvector<T> v(c, _Val);
            v.resize(s, _Val);
            v.shrink_to_fit();
            EXPECT_EQ(v.size(), s);
            EXPECT_GE(v.capacity(), s);
            for_each_n(v.begin(), s, [&_Val](auto &x) { EXPECT_EQ(x, _Val); });
        };
        run_range2([&](int s, int c) {
            test(__int, s, c);
            test(__string, s, c);
        });
    }
}

TEST(vector, emplace_back) {
    {
        auto test = [](auto _Val, size_t n) {
            using T = decltype(_Val);
            wvector<T> v;
            for (size_t i = 0; i < n; ++i) {
                v.emplace_back(_Val);
            }
            EXPECT_EQ(v.size(), n);
            EXPECT_GE(v.capacity(), n);
            for_each_n(v.begin(), n, [&_Val](auto &x) { EXPECT_EQ(x, _Val); });
        };
        run_range([&](int i) {
            test(__int, i);
            test(__string, i);
        });
    }

    {
        wvector<int> a;
        a.emplace_back(default_construct);

        WJR_CHECK(a.size() == 1);
    }

    {
        wvector<std::string> a;
        a.emplace_back(default_construct);

        WJR_CHECK(a.size() == 1);
        WJR_CHECK(a[0].empty());
    }
}

TEST(vector, pop_back) {
    {
        auto test = [](auto _Val, size_t n) {
            if (!n)
                return;
            using T = decltype(_Val);
            wvector<T> v(n, _Val);
            v.pop_back();
            EXPECT_EQ(v.size(), n - 1);
            EXPECT_GE(v.capacity(), n);
            for_each_n(v.begin(), n - 1, [&_Val](auto &x) { EXPECT_EQ(x, _Val); });
        };
        run_range([&](int i) {
            test(__int, i);
            test(__string, i);
        });
    }
}

TEST(vector, empty) {
    {
        auto test = [](auto _Val) {
            using T = decltype(_Val);
            wvector<T> v;
            EXPECT_EQ(v.size(), 0);
            EXPECT_GE(v.capacity(), 0);
            EXPECT_TRUE(v.empty());
        };
        [&]() {
            test(__int);
            test(__string);
        }();
    }
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
TEST(vector, at) {
    {
        auto test = [](auto _Val, size_t n, size_t i) {
            using T = decltype(_Val);
            wvector<T> v(n, _Val);
            EXPECT_EQ(v.size(), n);
            EXPECT_GE(v.capacity(), n);
            try {
                EXPECT_EQ(v.at(i), _Val);
                EXPECT_LT(i, n);
            } catch (...) {
                EXPECT_GE(i, n);
            }
        };
        run_range2([&](int s, int c) {
            test(__int, s, c);
            test(__string, s, c);
        });
    }
}
#endif

TEST(vector, emplace) {
    {
        auto test = [](auto _Val, auto _Val2, size_t n, size_t s, size_t c) {
            if (n > s)
                return;
            using T = decltype(_Val);
            wvector<T> v(c, _Val);
            v.resize(s, _Val);
            v.insert(v.begin() + n, _Val2);
            EXPECT_EQ(v.size(), s + 1);
            for (size_t i = 0; i < n; ++i) {
                EXPECT_EQ(v[i], _Val);
            }
            EXPECT_EQ(v[n], _Val2);
            for (size_t i = n + 1; i < s + 1; ++i) {
                EXPECT_EQ(v[i], _Val);
            }
        };
        run_range3([&](size_t n, size_t s, size_t c) {
            test(__int, __int2, n, s, c);
            test(__string, __string2, n, s, c);
        });
    }
}

TEST(vector, insert) {
    {
        auto test = [](auto _Val, auto _Val2, size_t n, size_t s, size_t c) {
            using T = decltype(_Val);
            auto __test = [&](size_t pos) {
                if (__has_high_bit(pos) || pos > s) {
                    return;
                }
                wvector<T> v(c, _Val);
                v.resize(s, _Val);
                v.insert(v.begin() + pos, n, _Val2);
                EXPECT_EQ(v.size(), s + n);
                for (size_t i = 0; i < pos; ++i) {
                    EXPECT_EQ(v[i], _Val);
                }
                for (size_t i = 0; i < n; ++i) {
                    EXPECT_EQ(v[i + pos], _Val2);
                }
                for (size_t i = pos + n; i < s + n; ++i) {
                    EXPECT_EQ(v[i], _Val);
                }
            };
            __test(0);
            __test(s);
            __test(1);
            __test(s / 2);
            __test(s - 1);
        };
        run_range3([&](int n, int s, int c) {
            test(__int, __int2, n, s, c);
            test(__string, __string2, n, s, c);
        });
    }
}

TEST(vector, erase) {
    {
        auto test = [](auto _Val, size_t n, size_t s, size_t c) {
            if (n >= s || !s)
                return;
            using T = decltype(_Val);
            wvector<T> v(c, _Val);
            v.resize(s, _Val);
            v.erase(v.begin() + n);
            EXPECT_EQ(v.size(), s - 1);
            for (size_t i = 0; i < s - 1; ++i) {
                EXPECT_EQ(v[i], _Val);
            }
        };
        run_range3([&](int n, int s, int c) {
            test(__int, n, s, c);
            test(__string, n, s, c);
        });
    }
}

TEST(vector, swap) {
    {
        vector<int> v1(32, 1);
        vector<int> v2(64, 2);
        v1.swap(v2);
        EXPECT_EQ(v1.size(), 64);
        EXPECT_EQ(v2.size(), 32);
        for_each_n(v1.begin(), 64, [](auto &x) { EXPECT_EQ(x, 2); });
        for_each_n(v2.begin(), 32, [](auto &x) { EXPECT_EQ(x, 1); });
    }

    for (int n = 0; n < 16; ++n)
        for (int m = 0; m < 16; ++m) {
            inplace_vector<int, 16> v1(n, 1);
            inplace_vector<int, 16> v2(m, 2);
            v1.swap(v2);
            EXPECT_EQ(v1.size(), m);
            EXPECT_EQ(v2.size(), n);
            for_each_n(v1.begin(), m, [](auto &x) { EXPECT_EQ(x, 2); });
            for_each_n(v2.begin(), n, [](auto &x) { EXPECT_EQ(x, 1); });
        }

    for (int n = 0; n < 32; ++n)
        for (int m = 0; m < 32; ++m) {
            inplace_vector<int, 32> v1(n, 1);
            inplace_vector<int, 32> v2(m, 2);
            v1.swap(v2);
            EXPECT_EQ(v1.size(), m);
            EXPECT_EQ(v2.size(), n);
            for_each_n(v1.begin(), m, [](auto &x) { EXPECT_EQ(x, 2); });
            for_each_n(v2.begin(), n, [](auto &x) { EXPECT_EQ(x, 1); });
        }

    for (int n = 0; n < 32; ++n)
        for (int m = 0; m < 32; ++m) {
            small_vector<int, 4> v1(n, 1);
            small_vector<int, 4> v2(m, 2);
            v1.swap(v2);
            EXPECT_EQ(v1.size(), m);
            EXPECT_EQ(v2.size(), n);
            for_each_n(v1.begin(), m, [](auto &x) { EXPECT_EQ(x, 2); });
            for_each_n(v2.begin(), n, [](auto &x) { EXPECT_EQ(x, 1); });
        }
}

TEST(vector, ptr_unsafe) {
    {
        wvector<int> a(16);
        WJR_CHECK(a.begin_unsafe() == wjr::to_address(a.begin()));
        WJR_CHECK(a.end_unsafe() == wjr::to_address(a.end()));
        WJR_CHECK(a.buf_end_unsafe() == a.begin_unsafe() + a.capacity());
    }
}

TEST(vector, construct_dctor) {
    {
        wvector<std::string> vec(16, default_construct);
        for_each_n(vec.begin(), 16, [](auto &x) { WJR_CHECK(x.empty()); });
    }
}

TEST(vector, clear) {
    {
        wvector<std::string> vec(16, default_construct);
        vec.clear();

        WJR_CHECK(vec.empty());
    }
}

TEST(vector, front) {
    {
        wvector<std::string> vec(16, default_construct);

        WJR_CHECK(vec.front().empty());
        WJR_CHECK(vec.front() == vec[0]);
        WJR_CHECK(vec.front() == vec.begin()[0]);
        WJR_CHECK(vec.front() == *vec.begin());
        WJR_CHECK(vec.front() == *vec.data());
    }
}