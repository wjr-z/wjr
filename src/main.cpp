#include "../include/biginteger.h"
#include "../include/json.h"

using namespace wjr;
using namespace std;

void test() {
    String str;
    struct Q {
        String* ptr;
        ~Q() {
            write_file("test.out", *ptr);
        }
    };
    Q it;
    it.ptr = &str;
    for (size_t n = 32; n <= (1 << 20); n *= 1.2) {
        size_t T = max((1 << 18) / n, 1ull);
        biginteger<2> c, d;
        biginteger<2> a = random_biginteger<2>(n * 32);
        String w;
        w.multiple_append(
            "n : ",
            String::number(n).right_justified(8),
            " 迭代次数 : ",
            String::number(T).right_justified(8),
            "\n\n"
        );
        for (size_t m = 4; m <= 256; m <<= 1) {
            biginteger<2> b = random_biginteger<2>(m * 32);
            auto s = mtime();
            for (size_t i = 0; i < T; ++i) {
                mul(c, a, b);
            }
            double p = mtime() - s;
            s = mtime();
            for (size_t i = 0; i < T; ++i) {
                //a / b;
                /*Karatsuba(d, a.get_virtual(), b.get_virtual());*/
            }
            double q = mtime() - s;
            w.multiple_append(
                "alpha : ",
                String::number(m * 1.0 / n).right_justified(8),
                " ",
                String::number(p).right_justified(8),
                " ",
                String::number(q).right_justified(8),
                " ",
                String::number(q / p).right_justified(8),
                " ",
                String::number(100 * p / T).right_justified(8),
                " ",
                String::number(100 * q / T).right_justified(8),
                "\n"
            );
        }
        w.multiple_append(
            "\n\n"
        );
        cout << w;
        str.append(w);
    }
}

void test2() {
    String str;
    struct Q {
        String* ptr;
        ~Q() {
            write_file("test_5.out", *ptr);
        }
    };
    Q it;
    it.ptr = &str;
    for (size_t n = 32; n <= (1 << 20); n = ceil(n * 1.2)) {
        size_t T = max((1 << 18) / n, 1ull);
        biginteger<2> c, d;
        biginteger<2> a = random_biginteger<2>(n * 32);
        String w;
        w.multiple_append(
            "n : ",
            String::number(n).right_justified(8),
            " 迭代次数 : ",
            String::number(T).right_justified(8),
            "\n\n"
        );
        for (size_t m = 2; m <= n; m = ceil(m * 1.2)) {
            biginteger<2> b = random_biginteger<2>(m * 32);
            auto s = mtime();
            for (size_t i = 0; i < T; ++i) {
                //qmul(c, a.get_virtual(), b.get_virtual());
                //toom_cook(c, a.get_virtual(), b.get_virtual());
                //mul(c, a, b);
            }
            double p = mtime() - s;
            w.multiple_append(
                "m : ",
                String::number(m).right_justified(8),
                " ",
                String::number(p).right_justified(8),
                "\n"
            );
        }
        w.multiple_append(
            "\n\n"
        );
        cout << w;
        str.append(w);
    }
}

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

biginteger<2> gcd2(biginteger<2> a, biginteger<2> b) {
    while (!b.zero()) {
        a %= b;
        swap(a, b);
    }
    return a;
}

int main() {
    while (true) {
        auto a = random_biginteger<2>(24 * 32);
        auto b = random_biginteger<2>(24 * 32);
        biginteger<2> c;
        auto s = mtime();
        mul(c, a, b);
        auto t = mtime();
        cout << t - s << '\n';
    }
    return 0;
}