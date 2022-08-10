#include "biginteger/biginteger.h"
#include "generic/mtool.h"
#include "generic/mtag.h"
using namespace std;
using namespace wjr;

using namespace wjr::math::biginteger;

mt19937_64 mt_rand(time(NULL));
int main() {

    auto x = [&]() {
        for (int i = 0; i < 1e3; ++i) {
            auto a = random_biginteger<2>(32 * 32);
            auto b = random_biginteger<2>(1 * 32);
            biginteger<2> c;
            for (int j = 0; j < 5e3; ++j) {
                c = a;
                c /= b;
            }
        }
    };

    cout << test_runtime(x) << '\n';
    return 0;
}
