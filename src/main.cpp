#include "biginteger/biginteger.h"
#include "generic/mtool.h"
using namespace std;
using namespace wjr;

using namespace wjr::math::biginteger;
using type = limb_t;
using btype = double_limb_t;
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
    const int N = max((size_t)1e9 / (n + 4), size_t(1));
    biginteger<2> a, b, c;
    a = random_biginteger<2>(limb_bits * n);
    b = random_biginteger<2>(limb_bits * n);

    //d.resize_bit(limb_bits * (n + 1 ));

    auto time1 = test_runtime(
        std::in_place_type_t<std::chrono::nanoseconds>{},
        [&]() {
            for (int i = 0; i < N; ++i) {
                add(c, a, b);
                //                c.data()[0] = math::adc(0, a.data(), b.data(), c.data(), n);
            }
        }
    );


    cout << n << '\n';
    cout << time1 << ' ';
    cout << ((double)time1.count() / N) << ' ';
    cout << ((double)time1.count() / N / n) << ' ';
    auto time2 = test_runtime(
        std::in_place_type_t<std::chrono::nanoseconds>{},
        [&]() {
            for (int i = 0; i < N; ++i) {
                //                add(c,a,b);
                //                c.data()[0] = math::adc(0, a.data(), b.data(), c.data(), n);
                //d.data()[n] = math::adc(0, a.data(), b.data(), d.data(), n);
            }
        }
    );


    cout << time2 << ' ';
    cout << ((double)time2.count() / N) << ' ';
    cout << ((double)time2.count() / N / n) << '\n';

    //cout << boolalpha << equal(c.data(), c.data() + c.size(), d.data()) << '\n';

}

mt19937_64 mt_rand(time(NULL));

void test(size_t n) {
    mt19937 mt_rand(time(NULL));
    cout << n << endl;
    size_t N = (1 << 20) / (n + 16);
    if (N < 1)N = 1;
    for (size_t i = 0; i < N; ++i) {
        auto a = random_biginteger<2>(n * limb_bits);
        auto b = random_biginteger<2>(n * limb_bits);
        auto c(a);
        auto d(b);
        auto e = mt_rand();
        auto f = mt_rand() % (128 * limb_bits);
        WASSERT_LEVEL_1(a - b + b == a);
        WASSERT_LEVEL_1(b - a + a == b);
        WASSERT_LEVEL_1(--(++a) == c);
        WASSERT_LEVEL_1((a * e) / e == a);
        cout << f << endl;
        a.mul_base_power(f).div_base_power(f);
        WASSERT_LEVEL_1(a == c);
    }
}

struct node {
    uint32_t* ptr;
    uint32_t* get()const { return ptr; }
};

void print(uint32_t* ptr) {
    printf("%p\n", ptr);
}
int main() {

    return 0;
}
