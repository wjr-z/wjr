#include <functional>
#include "../include/exist_ptr.h"
#include "../include/graphic.h"
#include "../include/json.h"
#include "../include/mtool.h"
#include "../include/thread_pool.h"
#include "../include/biginteger.h"
#include <random>
#include <intrin.h>

using namespace wjr;
using namespace std;

void Karatsuba(biginteger<2>& result,
	const virtual_biginteger<2, 0>& lhs, const virtual_biginteger<2, 0>& rhs) {
	size_t n = lhs.size(), m = rhs.size();
	if (n <= 100 || m <= 100) {
		mul(result, lhs, rhs);
		return;
	}
	size_t mid = n >> 1;
	virtual_biginteger<2, 0> l_high(lhs.data() + mid, n - mid);
	virtual_biginteger<2, 0> l_low(lhs.data(), mid);
	virtual_biginteger<2, 0> r_high(rhs.data() + mid, m - mid);
	virtual_biginteger<2, 0> r_low(rhs.data(), mid);
	l_low.maintain();
	r_low.maintain();
	biginteger<2> A, B, C;
	add(A, l_high, l_low);
	add(C, r_high, r_low);
	Karatsuba(B, get_virtual_biginteger<2>(A), get_virtual_biginteger<2>(C));
	Karatsuba(A, l_high, r_high);
	Karatsuba(C, l_low, r_low);
	B -= A;
	B -= C;
	result = std::move(A);
	result <<= (mid * 32);
	result += B;
	result <<= (mid * 32);
	result += C;
}


using vt = virtual_biginteger<2,0>;
void toom_cook(biginteger<2>& result,
	vt lhs,vt rhs) {
	if (lhs.size() < rhs.size()) {
		swap(lhs, rhs);
	}
	size_t n = lhs.size(), m = rhs.size();
	if (n <= 128 || m <= 128 /*|| n != m*/) {
		mul(result, lhs, rhs);
		return;
	}
	size_t m3 = n / 3;
	vt U0(lhs.signal(), lhs.data(), m3);
	vt U1(lhs.signal(), lhs.data() + m3, m3);
	vt U2(lhs.signal(), lhs.data() + 2 * m3, n - 2 * m3);
	size_t q = m;
	size_t l = min(q, m3);
	vt V0(rhs.signal(),rhs.data(), l);
	q -= l;
	l = min(q, m3);
	vt V1(rhs.signal(), rhs.data() + m3, l);
	q -= l;
	vt V2(rhs.signal(), rhs.data() + 2 * m3, q);
	U0.maintain();
	U1.maintain();
	U2.maintain();
	V0.maintain();
	V1.maintain();
	V2.maintain();
	biginteger<2> W0, W1, W2, W3, W4;

	add(W0, U0, U2);
	sub(W3, W0, U1);
	add(W0, W0, U1);

	add(W4, V0, V2);
	sub(W2, W4, V1);
	add(W4, W4, V1);

	toom_cook(W1, W3.get_virtual(), W2.get_virtual());

	toom_cook(W2, W0.get_virtual(), W4.get_virtual());

	W0 += U2;
	W0 <<= 1;
	W0 -= U0;
	W4 += V2;
	W4 <<= 1;
	W4 -= V0;
	
	toom_cook(W3, W0.get_virtual(), W4.get_virtual());
	toom_cook(W0, U0, V0);
	toom_cook(W4, U2, V2);
	
	W3 -= W1;
	W3 /= 3;
	W1 -= W2;
	W1.change_signal();
	W1 >>= 1;
	W2 -= W0;
	W3 -= W2;
	W3 >>= 1;
	W3 -= (W4 << 1);
	W2 -= W1;

	W3 += (W4 << (32 * m3)) ;
	W1 += (W2 << (32 * m3)) ;
	W1 -= W3;
	result = W3;
	result <<= 64 * m3;
	result += W1;
	result <<= 32 * m3;
	result += W0;
}

void test() {
	String str;
	struct Q {
		String* ptr;
		~Q() {
			write_file("data2.out", *ptr);
		}
	};
	Q it;
	it.ptr = &str;
	for (size_t n = 1 << 5; n <= (1 << 20); n *= 1.5) {
		size_t T = max((1 << 8) / n, 1ull);
		biginteger<2> a = random_biginteger<2>(n * 32);
		String w;
		w.multiple_append(
			"n : ",
			String::number(n).right_justified(8),
			" 迭代次数 : ",
			String::number(T).right_justified(8),
			"\n\n"
		);
		for (double alpha = 0.04; alpha <= 1.0 + 1e-10; alpha += 0.04) {
			biginteger<2> b = random_biginteger<2>(n * 32 * alpha);
			auto s = mtime();
			for (int i = 1; i <= T; ++i) {
				a* b;
			}
			double p = mtime() - s;
			s = mtime();
			for (int i = 1; i <= T; ++i) {
				a / b;
			}
			double q = mtime() - s;
			w.multiple_append(
				"alpha : ",
				String::number(alpha).right_justified(8),
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

biginteger<2> _Quick_Mod(const biginteger<2>& x, const biginteger<2>& mod, const biginteger<2>& mu) {
	if (x < mod)return x;
	const size_t k = mod.size();
	biginteger<2> r2(virtual_biginteger<2,0>(true,x.data() + k - 1,x.size() - (k - 1)));
	r2 *= mu;
	r2 >>= (k + 1) * 32;
	r2 *= mod;
	if (r2.size() > k + 1) {
		r2.resize((k + 1) * 32);
		r2.maintain();
	}
	biginteger<2> r(virtual_biginteger<2,0>(true, x.data(), min(x.size(),k + 1)).maintain());
	r -= r2;
	if (r >= mod)
		r -= mod;
	if (!r.signal()) {
		biginteger<2> G(1);
		G <<= (k + 1) * 32;
		r += G;
	}
	return r;
}

biginteger<2> pow(biginteger<2> a, biginteger<2> b, 
	const biginteger<2>& mod,const biginteger<2>& mu) {
	biginteger<2> ans(1);
	for (auto i : b) {
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
	biginteger<2> seed, const biginteger<2>& d, size_t r,const biginteger<2>& mu) {
	seed = pow(seed, d, n,mu);
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
	mu <<= (n.size() * 64);
	mu /= n;

	biginteger<2> S(n - 1);
	size_t r = 0;
	for (auto i : S) {
		if (i)break;
		++r;
	}
	biginteger<2> d(S);
	d >>= r;
	biginteger<2> seed(2);
	if (!witness(n, S, seed, d, r,mu))return false;
	seed = 3;
	if (!witness(n, S, seed, d, r,mu))return false;
	seed = 61;
	if (n > 61 && !witness(n, S, seed, d, r,mu))return false;
	biginteger<2> _Max(n - 6);
	for (size_t i = 0; i < k; ++i) {
		random_biginteger_max(seed,_Max);
		seed += 4;
		if (!witness(n, S, seed, d, r,mu))return false;
	}
	return true;
}

bool is_prime(const biginteger<2>& x) {
	return miller_robin(x);
}

int main() {
	list<int> li;
	
	return 0;
}
