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
WJR_NOINLINE void for_each_n(Iter first, size_t n, Func fn) {
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
using VectorTypes = ::testing::Types<ContainerWrapper<wsmall_vector>>;

// Custom name generator for better test output
struct VectorTestNames {
    template <typename T>
    static std::string GetName(int i) {
        if constexpr (std::is_same_v<T, ContainerWrapper<wvector>>) {
            return "vector";
        } else if constexpr (std::is_same_v<T, ContainerWrapper<wsmall_vector>>) {
            return "small_vector";
        } else {
            return ::testing::internal::StreamableToString(i);
        }
    }
};

TYPED_TEST_SUITE(VectorTest, VectorTypes, VectorTestNames);

TYPED_TEST(VectorTest, assign) {
#ifdef __clang__
    std::cout << "clang:" << __clang_major__ << "." << __clang_minor__ << "."
              << __clang_patchlevel__ << std::endl;
#elif defined(__GNUC__)
    std::cout << "gcc:" << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__
              << std::endl;
#elif defined(_MSC_VER)
    std::cout << "msvc:" << _MSC_VER << std::endl;
#endif

    // assign(size_t _Count, const vlaue_type& _Val)
    {
        auto test = [](auto _Val, size_t n, size_t s, size_t c) {
            using T = decltype(_Val);
            using Vec = typename TypeParam::template type<T>;
            Vec v(c);
            v.resize(s);
            v.assign(n, _Val);
            for_each_n(v.begin(), n, [_Val, n, s, c](auto &x) {
                EXPECT_EQ(x, _Val) << n << ' ' << s << ' ' << c;
            });
        };
        auto fn = [&](int n, int s, int c) {
            test(__int, n, s, c);
            test(__string, n, s, c);
        };

        fn(0, 0, 1);
    }

    std::cout << "assign(range)" << std::endl;
}