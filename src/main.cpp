#include "biginteger/biginteger.h"
#include "generic/mtool.h"
#include "generic/mtag.h"
using namespace std;
using namespace wjr;

using namespace wjr::math::biginteger;

//void print(B) {
//    cout << "B\n";
//}
#include <set>
mt19937_64 mt_rand(time(NULL));

__uint128_t mulhi(__uint128_t a, uint64_t b){
    uint64_t hi = a >> 64;
    uint64_t lo = a;
    __uint128_t x = (__uint128_t)hi * b;
    uint64_t C = libdivide::libdivide_mullhi_u64(lo, b);
    return x + C;
}

__uint128_t div(__uint128_t a, libdivide::divider<uint64_t> b){
    uint8_t more = b.div.denom.more;
//    uint8_t more = denom->more;
    if(!b.div.denom.magic){
        return a >> more;
    }else {
        __uint128_t q = mulhi(a, b.div.denom.magic);
        if (more & libdivide::LIBDIVIDE_ADD_MARKER) {
            // uint32_t t = ((numer - q) >> 1) + q;
            // return t >> denom->shift;
            uint32_t shift = more & libdivide::LIBDIVIDE_64_SHIFT_MASK;
            __uint128_t t = ((a - q) >> 1) + q;
            return t >> shift;
        }
        else {
            return q >> more;
        }
    }

}

int main() {

//    0 5990815501116674527
//    0 5990815501116674526

    uint64_t a,b;
    a = 3463938419685202139ull;
    b = 10666057985446608553ull;
    uint64_t c;
    c = 10666057985446608553ull;

    __uint128_t x = (__uint128_t)a << 64 | b;
    libdivide::divider<uint64_t> g(c);
    auto ans = div(x,g);

    //int n, m;
    //cin >> n >> m;
    //cout << test_runtime(
    //    []() {
    //        calloc(100, sizeof(int));
    //    }
    //) << '\n';
    //return 0;
    //mt19937_64 mt_rand(time(NULL));
    //cout << test_runtime(
    //    [&]() {
    //        biginteger<2> a, b, c, d;
    //        for (int i = 0; i < 1e2; ++i) {
    //            a = random_biginteger<2>(1200 * 32);
    //            b = random_biginteger<2>(80 * 32);
    //            uint64_t B = mt_rand();
    //            for (int j = 0; j < 1e3; ++j) {
    //                //add(c, a, B);
    //                //c = a;
    //                //d = b;
    //                //divmod(c, d);
    //                mul(c, a, b);
    //                //mul(c, a, b);
    //            }
    //        }
    //    }
    //) << '\n';

    return 0;
}
