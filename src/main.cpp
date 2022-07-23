#include "json/json.h"
#include "network/thread_pool.h"
#include "generic/mtool.h"
#include "generic/masm.h"
#include "biginteger/biginteger.h"
using namespace std;
using namespace wjr;

using type = biginteger_basic_value_type;
using btype = biginteger_basic_twice_value_type;

WJR_FORCEINLINE type mul_1(const type* a, type b, type* c, size_t n) {
    type cf = 0;

    for (size_t i = 0; i < n; ++i) {
        auto val = a[i] * (uint64_t)b + cf;
        cf = val >> 32;
        c[i] = val;
    }

    return cf;
}

static void mymul() {
    const int N = 1e6;
    int n = 128;
    biginteger<2> a, b, c;
    vector<type> d(1e4);
    a = random_biginteger<2>(biginteger_basic_bit * n);
    b = random_biginteger<2>(biginteger_basic_bit);

    auto time1 = test_runtime(
        std::in_place_type_t<std::chrono::nanoseconds>{},
        [&]() {
            for (int i = 0; i < N; ++i)
                mul(c, a, b);
        }
    );

    cout << time1 << '\n';

    cout << (time1.count() / N) << '\n';

    auto time2 = test_runtime(
        std::in_place_type_t<std::chrono::nanoseconds>{},
        [&]() {
            for (int i = 0; i < N; ++i)
                d.data()[n] = mul_1(a.data(), b.data()[0], d.data(), n);
        }
    );
    cout << time2 << '\n';
    cout << (time2.count() / N) << '\n';

    for (size_t i = 0; i < c.size(); ++i) {
        if (c.data()[i] != d.data()[i]) {
            cout << "error at " << i << '\n';
            exit(0);
        }
    }

    cout << boolalpha << equal(c.data(), c.data() + c.size(), d.data()) << '\n';
}

int main() {

    mymul();

    return 0;
}
