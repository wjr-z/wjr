#include "biginteger/biginteger.h"
#include "generic/mtool.h"
#include "generic/mtag.h"
//#include <numbers>
using namespace std;
using namespace wjr;
using namespace wjr::math::biginteger;
const double pi = acos(-1);


biginteger<2> _Quick_Mod(const biginteger<2>& x, const biginteger<2>& mod, const biginteger<2>& mu) {
    if (x < mod)return x;
    const size_t k = mod.size();
    biginteger<2> r2(signed_view<2>(x, k - 1, x.size() - (k - 1)).abs());
    r2 *= mu;
    r2.div_base_power((k + 1) * 32);
    r2 *= mod;
    r2.mod_base_power((k + 1) * 32);
    biginteger<2> r(signed_view<2>(x, 0, min(x.size(), k + 1)).abs().maintain());
    r -= r2;
    if (r >= mod)
        r -= mod;
    if (!r.signal()) {
        biginteger<2> G(1);
        G.mul_base_power((k + 1) * 32);
        r += G;
    }
    return r;
}

biginteger<2> pow(biginteger<2> a, const biginteger<2>& b,
    const biginteger<2>& mod, const biginteger<2>& mu) {
    biginteger<2> ans(1);
    auto view = unsigned_iterator<2>(b);
    for (; view.vaild(); ++view) {
        if (*view) {
            ans *= a;
            ans = _Quick_Mod(ans, mod, mu);
        }
        a *= a;
        a = _Quick_Mod(a, mod, mu);
    }
    return ans;
}

bool witness(const biginteger<2>& n, const biginteger<2>& S,
    biginteger<2> seed, const biginteger<2>& d, size_t r, const biginteger<2>& mu) {
    seed = pow(seed, d, n, mu);
    if (seed == 1)return true;
    for (size_t i = 0; i < r; ++i) {
        if (seed == S)return true;
        seed *= seed;
        _Quick_Mod(seed, n, mu);
        if (seed == 1)return false;
    }
    return false;
}

template<int k>
uint32_t gtk(uint32_t val) {
    return val >> k;
}

uint32_t gk(uint32_t val, int k) {
    using func = uint32_t(*)(uint32_t);
    constexpr func table[32] = { gtk<0>,gtk<1>,gtk<2>,gtk<3>, };
}
bool miller_robin(const biginteger<2>& n, const size_t k = 5) {
    if (n.size() == 1) {
        uint32_t val(n[0]);
        if (val == 2 || val == 3 || val == 5)return true;
        if (val == 1 || val == 27509653 || val == 74927161)return false;
    }
    if (n.data()[0] % 2 == 0) {
        return false;
    }

    biginteger<2> mu(1);
    mu.mul_base_power(n.size() * 32 * 2);
    mu /= n;

    biginteger<2> S(n);
    sub(S, S, 1u);
    size_t r = 0;
    auto view = unsigned_iterator<2>(S, 1, 0);
    while (view.vaild()) {
        if (*view)break;
        ++r;
        ++view;
    }
    biginteger<2> d(S);
    d.div_base_power(r);
    biginteger<2> seed(2);
    if (!witness(n, S, seed, d, r, mu))return false;
    seed = 3;
    if (!witness(n, S, seed, d, r, mu))return false;
    seed = 61;
    if (n > 61 && !witness(n, S, seed, d, r, mu))return false;
    biginteger<2> _Max(n);
    sub(_Max, _Max, 6u);
    for (size_t i = 0; i < k; ++i) {
        interval_random(seed, _Max);
        seed += 4u;
        if (!witness(n, S, seed, d, r, mu))return false;
    }
    return true;
}

bool is_prime(const biginteger<2>& x) {
    return miller_robin(x);
}
void test() {
    while (true) {
        biginteger<2> a(1u);
        a.mul_base_power(1024 * 64);
        auto b = random<2>(512 * 64);
        a /= b;
        cout << "done\n";
    }
}

int main() {
    test();
    int n = 12;
    while (n--) {
        auto a = random<2>(128 * 64);
        cout << test_runtime([&]() {is_prime(a); }) << '\n';
    }

	
    return 0;
}
