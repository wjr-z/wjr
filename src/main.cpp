#include "generic/mtool.h"
#include "biginteger/biginteger.h"
#include <variant>
using namespace std;
using namespace wjr;

using type = biginteger_basic_value_type;
using btype = biginteger_basic_twice_value_type;

WJR_FORCEINLINE type mul_1(const type* a, type b, type* c, size_t n) {
    type cf = 0;

    for (size_t i = 0; i < n; ++i) {
        auto val = a[i] * (btype)b + cf;
        cf = val >> 32;
        c[i] = val;
    }

    return cf;
}

static void mymul(size_t n) {
    const int N = max((size_t)2e9 / (n * n), size_t(1));
    biginteger<2> a, b, c;
    a = random_biginteger<2>(biginteger_basic_bit * n);
    b = random_biginteger<2>(biginteger_basic_bit * n);

    auto time1 = test_runtime(
        std::in_place_type_t<std::chrono::nanoseconds>{},
        [&]() {
            for (int i = 0; i < N; ++i) {
                add(c, a, b);
            }
        }
    );

    cout << n << '\n';
    cout << time1 << ' ';
    cout << (time1.count() / N) << '\n';
   /* vector<uint32_t> d(n + 1);


    auto time2 = test_runtime(
        std::in_place_type_t<std::chrono::nanoseconds>{},
        [&]() {
            for (int i = 0; i < N; ++i) {
                d.data()[n] = mul_1(a.data(), b.data()[0], d.data(), n);
            }
        }
    );

    cout << time2 << ' ';
    cout << (time2.count() / N) << '\n';

    for (size_t i = 0; i < c.size(); ++i) {
        if (c.data()[i] != d.data()[i]) {
            cout << "error at " << i << endl;
            cout << a.data()[i] << ' ' << b.data()[i] << ' ' << c.data()[i] << ' ' << d.data()[i] << endl;

            return;
        }
    }
    cout << boolalpha << std::equal(c.data(), c.data() + c.size(), d.data()) << '\n';*/
}


int main() {
    mymul(128);
    //    for(int i=1;i<=(1<<20);i<<=1){
    //        mymul(i);
    //    }

    return 0;
}
