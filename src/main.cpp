#include <fstream>
#include "../include/biginteger.h"
#include "../include/json.h"
#include "../include/mtool.h"

using namespace wjr;
using namespace std;


biginteger<2> _Quick_Mod(const biginteger<2>& x, const biginteger<2>& mod, const biginteger<2>& mu) {
    if (x < mod)return x;
    const size_t k = mod.size();
    biginteger<2> r2(virtual_biginteger<2, 0>(true, x.data() + k - 1, x.size() - (k - 1)));
    r2 *= mu;
    r2.div_base_power((k + 1) * 32);
    r2 *= mod;
    r2.mod_base_power((k + 1) * 32);
    biginteger<2> r(virtual_biginteger<2, 0>(true, x.data(), min(x.size(), k + 1)).maintain());
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
    for (const auto& i : b) {
        if (i) {
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

bool miller_robin(const biginteger<2>& n, const size_t k = 5) {
    if (n.size() == 1) {
        uint32_t val(n);
        if (val == 2 || val == 3 || val == 5)return true;
        if (val == 1 || val == 27509653 || val == 74927161)return false;
    }
    if (n.data()[0] % 2 == 0) {
        return false;
    }

    biginteger<2> mu(1);
    mu.mul_base_power(n.size() * 64);
    mu /= n;

    biginteger<2> S(n - 1);
    size_t r = 0;
    for (auto i : S) {
        if (i)break;
        ++r;
    }
    biginteger<2> d(S);
    d.div_base_power(r);
    biginteger<2> seed(2);
    if (!witness(n, S, seed, d, r, mu))return false;
    seed = 3;
    if (!witness(n, S, seed, d, r, mu))return false;
    seed = 61;
    if (n > 61 && !witness(n, S, seed, d, r, mu))return false;
    biginteger<2> _Max(n - 6);
    for (size_t i = 0; i < k; ++i) {
        random_biginteger_max(seed, _Max);
        seed += 4;
        if (!witness(n, S, seed, d, r, mu))return false;
    }
    return true;
}

bool is_prime(const biginteger<2>& x) {
    return miller_robin(x);
}

int main() {
    auto vec = get_all_files("test");
    for (auto& i : vec) {
        i = read_file(i);
    }
    using cjson = basic_json<default_json_traits>;
    vector<cjson> jc;
    auto s = mtime();
    for (auto i : vec) {
        jc.push_back(cjson::parse(i.c_str(), i.c_str() + i.size()));
    }
    auto t = mtime();
    cout << t - s << '\n';

    return 0;
}
												