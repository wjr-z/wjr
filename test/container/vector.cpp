#include <algorithm>
#include <list>
#include <string>
#include <string_view>
#include <vector>

#include "../random.hpp"

#include <wjr/math.hpp>
#include <wjr/vector.hpp>

inline const int __int = 5082;
inline const int __int2 = 123;
inline const std::string __string = std::string("wjr");
inline const std::string __string2 = std::string("abc");

using namespace wjr;

static_assert(is_trivially_allocator_v<std::allocator<int>>);

template <typename Vec>
constexpr bool test_vector_type() {
    using T = typename Vec::value_type;
    using allocator_type = typename Vec::allocator_type;
    using storage_type = typename Vec::storage_type;

    static_assert(std::is_nothrow_default_constructible_v<storage_type>);

    static_assert(std::is_nothrow_default_constructible_v<Vec> ==
                  std::is_nothrow_default_constructible_v<allocator_type>);
    static_assert(std::is_nothrow_move_constructible_v<Vec>);
    static_assert(std::is_nothrow_destructible_v<Vec>);

    static_assert(is_contiguous_iterator_v<typename Vec::iterator>);
    static_assert(is_contiguous_iterator_v<typename Vec::const_iterator>);

    return true;
}

static_assert(test_vector_type<vector<int>>());
static_assert(test_vector_type<vector<std::string>>());

static_assert(test_vector_type<inplace_vector<int, 8>>());
static_assert(test_vector_type<inplace_vector<std::string, 8>>());

static_assert(test_vector_type<small_vector<int, 8>>());
static_assert(test_vector_type<small_vector<std::string, 8>>());

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

// Container templates
template <typename T>
using wvector = wjr::vector<T>;

template <typename T>
using wsmall_vector = wjr::small_vector<T, 4>;

namespace wjr {
template class basic_vector<default_vector_storage<int, std::allocator<int>>>;
template class basic_vector<default_vector_storage<std::string, std::allocator<std::string>>>;
} // namespace wjr

// Wrapper for container template
template <template <typename> class Container>
struct ContainerWrapper {
    template <typename T>
    using type = Container<T>;
};

// Test fixture for typed tests
template <typename TypeParam>
class VectorTest : public ::testing::Test {};

// Define the types to test - just the two container templates
using VectorTypes = ::testing::Types<ContainerWrapper<wvector>, ContainerWrapper<wsmall_vector>>;

// Custom name generator for better test output
struct VectorTestNames {
    template <typename T>
    static std::string GetName(int i) {
        if constexpr (std::is_same_v<T, ContainerWrapper<wvector>>) {
            return "vector";
        } else if constexpr (std::is_same_v<T, ContainerWrapper<wsmall_vector>>) {
            return "small_vector";
        } else {
            static_assert(false, "Unknown container type");
        }
    }
};

TYPED_TEST_SUITE(VectorTest, VectorTypes, VectorTestNames);

TYPED_TEST(VectorTest, construct_default) {
    auto test = [](auto _Val) {
        using T = decltype(_Val);
        using Vec = typename TypeParam::template type<T>;
        // default
        {
            Vec v;
            EXPECT_EQ(v.size(), 0);
            EXPECT_GE(v.capacity(), 0);
        }
        {
            std::allocator<T> al;
            Vec v(al);
            EXPECT_EQ(v.size(), 0);
            EXPECT_GE(v.capacity(), 0);
        }
    };

    test(__int);
    test(__string);
}

TYPED_TEST(VectorTest, construct_with_size) {
    {
        auto test = [](auto _Val, size_t n) {
            using T = decltype(_Val);
            using Vec = typename TypeParam::template type<T>;
            std::allocator<T> al;
            Vec v(n, al);
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

TYPED_TEST(VectorTest, assignment) {
    // vector& operator=(const vector&)
    {
        auto test = [](auto _Val, size_t n, size_t s, size_t c) {
            using T = decltype(_Val);
            using Vec = typename TypeParam::template type<T>;
            std::allocator<T> al;
            Vec V(n, _Val, al);
            Vec v(c);
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
            using Vec = typename TypeParam::template type<T>;
            std::allocator<T> al;
            Vec V(n, _Val, al);
            Vec v(c);
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
            using Vec = typename TypeParam::template type<T>;
            auto n = il.size();
            Vec v;
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

TYPED_TEST(VectorTest, assign) {
    // assign(size_t _Count, const vlaue_type& _Val)
    {
        auto test = [](auto _Val, size_t n, size_t s, size_t c) {
            using T = decltype(_Val);
            using Vec = typename TypeParam::template type<T>;
            Vec v(c);
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
            using Vec = typename TypeParam::template type<T>;
            Vec v;
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
            using Vec = typename TypeParam::template type<T>;
            auto n = il.size();
            Vec v;
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

TYPED_TEST(VectorTest, resize) {
    {
        auto test = [](auto _Val, size_t n, size_t s, size_t c) {
            using T = decltype(_Val);
            using Vec = typename TypeParam::template type<T>;
            Vec v(c);
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
            using Vec = typename TypeParam::template type<T>;
            Vec v(c);
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

TYPED_TEST(VectorTest, reserve) {
    {
        auto test = [](auto _Val, size_t n, size_t s, size_t c) {
            using T = decltype(_Val);
            using Vec = typename TypeParam::template type<T>;
            Vec v(c, _Val);
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

TYPED_TEST(VectorTest, shrink_to_fit) {
    {
        auto test = [](auto _Val, size_t s, size_t c) {
            using T = decltype(_Val);
            using Vec = typename TypeParam::template type<T>;
            Vec v(c, _Val);
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

TYPED_TEST(VectorTest, emplace_back) {
    {
        auto test = [](auto _Val, size_t n) {
            using T = decltype(_Val);
            using Vec = typename TypeParam::template type<T>;
            Vec v;
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
        using Vec = typename TypeParam::template type<int>;
        Vec a;
        a.emplace_back(default_construct);
        EXPECT_TRUE(a.size() == 1);
    }

    {
        using Vec = typename TypeParam::template type<std::string>;
        Vec a;
        a.emplace_back(default_construct);
        EXPECT_TRUE(a.size() == 1);
        EXPECT_TRUE(a[0].empty());
    }
}

TYPED_TEST(VectorTest, pop_back) {
    {
        auto test = [](auto _Val, size_t n) {
            if (!n) {
                return;
            }
            using T = decltype(_Val);
            using Vec = typename TypeParam::template type<T>;
            Vec v(n, _Val);
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

TYPED_TEST(VectorTest, empty) {
    auto test = [](auto _Val) {
        using T = decltype(_Val);
        using Vec = typename TypeParam::template type<T>;
        Vec v;
        EXPECT_EQ(v.size(), 0);
        EXPECT_GE(v.capacity(), 0);
        EXPECT_TRUE(v.empty());
    };
    test(__int);
    test(__string);
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
TYPED_TEST(VectorTest, at) {
    {
        auto test = [](auto _Val, size_t n, size_t i) {
            using T = decltype(_Val);
            using Vec = typename TypeParam::template type<T>;
            Vec v(n, _Val);
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

TYPED_TEST(VectorTest, emplace) {
    {
        auto test = [](auto _Val, auto _Val2, size_t n, size_t s, size_t c) {
            if (n > s) {
                return;
            }
            using T = decltype(_Val);
            using Vec = typename TypeParam::template type<T>;
            Vec v(c, _Val);
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

TYPED_TEST(VectorTest, insert) {
    {
        auto test = [](auto _Val, auto _Val2, size_t n, size_t s, size_t c) {
            using T = decltype(_Val);
            using Vec = typename TypeParam::template type<T>;
            auto __test = [&](size_t pos) {
                if (_has_high_bit(pos) || pos > s) {
                    return;
                }
                Vec v(c, _Val);
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

TYPED_TEST(VectorTest, erase) {
    {
        auto test = [](auto _Val, size_t n, size_t s, size_t c) {
            if (n >= s || !s) {
                return;
            }
            using T = decltype(_Val);
            using Vec = typename TypeParam::template type<T>;
            Vec v(c, _Val);
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

TYPED_TEST(VectorTest, swap) {
    using Vec = typename TypeParam::template type<int>;

    {
        Vec v1(32, 1);
        Vec v2(64, 2);
        v1.swap(v2);
        EXPECT_EQ(v1.size(), 64);
        EXPECT_EQ(v2.size(), 32);
        for_each_n(v1.begin(), 64, [](auto &x) { EXPECT_EQ(x, 2); });
        for_each_n(v2.begin(), 32, [](auto &x) { EXPECT_EQ(x, 1); });
    }

    for (int n = 0; n < 32; ++n) {
        for (int m = 0; m < 32; ++m) {
            Vec v1(n, 1);
            Vec v2(m, 2);
            v1.swap(v2);
            EXPECT_EQ(v1.size(), m);
            EXPECT_EQ(v2.size(), n);
            for_each_n(v1.begin(), m, [](auto &x) { EXPECT_EQ(x, 2); });
            for_each_n(v2.begin(), n, [](auto &x) { EXPECT_EQ(x, 1); });
        }
    }
}

// Keep inplace_vector tests separate as they have fixed capacity
TEST(vector, swap_inplace) {
    for (int n = 0; n < 16; ++n) {
        for (int m = 0; m < 16; ++m) {
            inplace_vector<int, 16> v1(n, 1);
            inplace_vector<int, 16> v2(m, 2);
            v1.swap(v2);
            EXPECT_EQ(v1.size(), m);
            EXPECT_EQ(v2.size(), n);
            for_each_n(v1.begin(), m, [](auto &x) { EXPECT_EQ(x, 2); });
            for_each_n(v2.begin(), n, [](auto &x) { EXPECT_EQ(x, 1); });
        }
    }

    for (int n = 0; n < 32; ++n) {
        for (int m = 0; m < 32; ++m) {
            inplace_vector<int, 32> v1(n, 1);
            inplace_vector<int, 32> v2(m, 2);
            v1.swap(v2);
            EXPECT_EQ(v1.size(), m);
            EXPECT_EQ(v2.size(), n);
            for_each_n(v1.begin(), m, [](auto &x) { EXPECT_EQ(x, 2); });
            for_each_n(v2.begin(), n, [](auto &x) { EXPECT_EQ(x, 1); });
        }
    }
}

TYPED_TEST(VectorTest, ptr_unsafe) {
    using Vec = typename TypeParam::template type<int>;
    Vec a(16);
    EXPECT_TRUE(a.begin_unsafe() == wjr::to_address(a.begin()));
    EXPECT_TRUE(a.end_unsafe() == wjr::to_address(a.end()));
    EXPECT_TRUE(a.buf_end_unsafe() == a.begin_unsafe() + a.capacity());
}

TYPED_TEST(VectorTest, construct_dctor) {
    using Vec = typename TypeParam::template type<std::string>;
    Vec vec(16, default_construct);
    for_each_n(vec.begin(), 16, [](auto &x) { EXPECT_TRUE(x.empty()); });
}

TYPED_TEST(VectorTest, clear) {
    using Vec = typename TypeParam::template type<std::string>;
    Vec vec(16, default_construct);
    vec.clear();
    EXPECT_TRUE(vec.empty());
}

TYPED_TEST(VectorTest, front) {
    using Vec = typename TypeParam::template type<std::string>;
    Vec vec(16, default_construct);
    EXPECT_TRUE(vec.front().empty());
    EXPECT_TRUE(vec.front() == vec[0]);
    EXPECT_TRUE(vec.front() == vec.begin()[0]);
    EXPECT_TRUE(vec.front() == *vec.begin());
    EXPECT_TRUE(vec.front() == *vec.data());
}

// Test memcpy byte size correctness for inplace_vector and small_vector
// These tests verify that when _use_memcpy condition is satisfied,
// the correct number of bytes (count * sizeof(T)) is copied
TEST(vector, memcpy_bytes_correctness) {
    // Test with different sized trivially copyable types
    // uint8_t: 1 byte
    // uint16_t: 2 bytes
    // uint32_t: 4 bytes
    // uint64_t: 8 bytes

    // Test inplace_vector with uint16_t (2 bytes)
    {
        inplace_vector<uint16_t, 8> v1;
        for (uint16_t i = 0; i < 8; ++i) {
            v1.push_back(0x1000 + i);
        }

        inplace_vector<uint16_t, 8> v2;
        v2 = v1; // copy assignment

        EXPECT_EQ(v2.size(), 8);
        for (size_t i = 0; i < 8; ++i) {
            EXPECT_EQ(v2[i], 0x1000 + i);
        }

        // Test move/swap
        inplace_vector<uint16_t, 8> v3;
        for (uint16_t i = 0; i < 8; ++i) {
            v3.push_back(0x2000 + i);
        }

        v1.swap(v3);
        EXPECT_EQ(v1.size(), 8);
        EXPECT_EQ(v3.size(), 8);
        for (size_t i = 0; i < 8; ++i) {
            EXPECT_EQ(v1[i], 0x2000 + i);
            EXPECT_EQ(v3[i], 0x1000 + i);
        }
    }

    // Test inplace_vector with uint32_t (4 bytes)
    {
        inplace_vector<uint32_t, 4> v1;
        for (uint32_t i = 0; i < 4; ++i) {
            v1.push_back(0x10000000 + i);
        }

        inplace_vector<uint32_t, 4> v2;
        v2 = v1;

        EXPECT_EQ(v2.size(), 4);
        for (size_t i = 0; i < 4; ++i) {
            EXPECT_EQ(v2[i], 0x10000000 + i);
        }
    }

    // Test inplace_vector with uint64_t (8 bytes)
    {
        inplace_vector<uint64_t, 2> v1;
        v1.push_back(0x1000000000000000ULL);
        v1.push_back(0x2000000000000000ULL);

        inplace_vector<uint64_t, 2> v2;
        v2 = v1;

        EXPECT_EQ(v2.size(), 2);
        EXPECT_EQ(v2[0], 0x1000000000000000ULL);
        EXPECT_EQ(v2[1], 0x2000000000000000ULL);
    }

    // Test small_vector with uint16_t
    {
        small_vector<uint16_t, 4> v1;
        for (uint16_t i = 0; i < 4; ++i) {
            v1.push_back(0x3000 + i);
        }

        small_vector<uint16_t, 4> v2;
        v2 = v1;

        EXPECT_EQ(v2.size(), 4);
        for (size_t i = 0; i < 4; ++i) {
            EXPECT_EQ(v2[i], 0x3000 + i);
        }

        // Test swap when small
        small_vector<uint16_t, 4> v3;
        for (uint16_t i = 0; i < 4; ++i) {
            v3.push_back(0x4000 + i);
        }

        v1.swap(v3);
        EXPECT_EQ(v1.size(), 4);
        EXPECT_EQ(v3.size(), 4);
        for (size_t i = 0; i < 4; ++i) {
            EXPECT_EQ(v1[i], 0x4000 + i);
            EXPECT_EQ(v3[i], 0x3000 + i);
        }
    }

    // Test small_vector with uint32_t
    {
        small_vector<uint32_t, 4> v1;
        for (uint32_t i = 0; i < 4; ++i) {
            v1.push_back(0x20000000 + i);
        }

        small_vector<uint32_t, 4> v2;
        v2 = v1;

        EXPECT_EQ(v2.size(), 4);
        for (size_t i = 0; i < 4; ++i) {
            EXPECT_EQ(v2[i], 0x20000000 + i);
        }
    }

    // Test with struct that is 12 bytes
    struct Data12 {
        uint32_t a;
        uint32_t b;
        uint32_t c;

        bool operator==(const Data12 &other) const {
            return a == other.a && b == other.b && c == other.c;
        }
    };
    static_assert(sizeof(Data12) == 12, "Data12 should be 12 bytes");
    static_assert(std::is_trivially_copyable_v<Data12>, "Data12 should be trivially copyable");

    {
        inplace_vector<Data12, 2> v1;
        v1.push_back({0x11111111, 0x22222222, 0x33333333});
        v1.push_back({0x44444444, 0x55555555, 0x66666666});

        inplace_vector<Data12, 2> v2;
        v2 = v1;

        EXPECT_EQ(v2.size(), 2);
        EXPECT_EQ(v2[0].a, 0x11111111);
        EXPECT_EQ(v2[0].b, 0x22222222);
        EXPECT_EQ(v2[0].c, 0x33333333);
        EXPECT_EQ(v2[1].a, 0x44444444);
        EXPECT_EQ(v2[1].b, 0x55555555);
        EXPECT_EQ(v2[1].c, 0x66666666);
    }

    // Test partial fill (size < capacity) to ensure we don't read uninitialized memory
    {
        inplace_vector<uint32_t, 8> v1;
        v1.push_back(0xAAAAAAAA);
        v1.push_back(0xBBBBBBBB);
        v1.push_back(0xCCCCCCCC);
        // Only 3 elements, capacity is 8

        inplace_vector<uint32_t, 8> v2;
        v2 = v1;

        EXPECT_EQ(v2.size(), 3);
        EXPECT_EQ(v2[0], 0xAAAAAAAA);
        EXPECT_EQ(v2[1], 0xBBBBBBBB);
        EXPECT_EQ(v2[2], 0xCCCCCCCC);
    }

    {
        small_vector<uint32_t, 4> v1;
        v1.push_back(0xDDDDDDDD);
        v1.push_back(0xEEEEEEEE);
        // Only 2 elements, capacity is 4

        small_vector<uint32_t, 4> v2;
        v2 = v1;

        EXPECT_EQ(v2.size(), 2);
        EXPECT_EQ(v2[0], 0xDDDDDDDD);
        EXPECT_EQ(v2[1], 0xEEEEEEEE);
    }
}