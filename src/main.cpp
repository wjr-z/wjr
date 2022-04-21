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

void print(const biginteger<2>& x) {
	cout << base_conversion<10>(x).tostr() << '\n';
}
int l = 22;
biginteger<2> inv(const biginteger<2> &a,int g) {
	using traits = biginteger_traits<2>;
	size_t n = a.size();
	biginteger<2> b, c;
	b.resize(1 * 32);
	b.data()[0] = traits::max_value / a.data()[n-1];
	biginteger<2> d;  
	for (int i = 0; i <= l; ++i) {
		c.resize(0);
		size_t m = std::min((size_t)1 << i, n);
		c.resize(m * 32);
		//b <<= ((1 << i) - b.size()) * 32;
		for (int j = 0; j < m; ++j) {
			c.data()[m - j - 1] = a.data()[n - j - 1];
		}
		c *= b;
		d.resize(0);
		d.resize((b.size() + m) * 32);
		d.data()[b.size() + m - 1] = 2;
		c = d - c;
		b *= c;
		int C = min(1 << (i + 1), g);
		if (b.size() >= C) {
			b >>= (b.size() - C) * 32;
		}
	}

	return b;
}

const char* get_mode(size_t n,size_t m) {
	size_t _Min = n < m ? n : m;
	size_t _Max = n > m ? n : m;
	if (_Min <= 32 ||
		((uint64_t)(1) << (std::min(size_t(60), ((_Min - 32) / 2)))) <= _Max) {
		return "slow mul";
	}
	else {
		return "fft mul";
	}
}

int T = 1 << 16;
int main() {
	freopen("data.out", "w", stdout);
	using bint = biginteger<2>;
	for (size_t n = 4; n <= (1 << 10); n <<= 1) {
		auto a = random_biginteger<2>(n * 32);
		cout << '\n';
		cout << "n : " << n << '\n';
		cout << '\n' << '\n';
		for (double alpha = 0.1; alpha <= 1; alpha += 0.05) {
			auto b = random_biginteger<2>(n * 32 * alpha);
			bint c;
			double p = 0, q = 0;
			auto s = mtime();
			for (int j = 0; j < T / n; ++j) {
				c = a;
				s = mtime();
				c.ubint.slow_mul<0>(get_virtual_biginteger<2>(b.ubint));
				p += mtime() - s;
			}
			cout << alpha << ' ';
			cout << p << ' ';
			for (int j = 0; j < T / n; ++j) {
				c = a;
				s = mtime();
				c.ubint.fft_mul<0>(get_virtual_biginteger<2>(b.ubint));
				q += mtime() - s;
			}
			auto A = get_mode(a.size(), b.size());
			auto B = (p < q ? "slow mul" : "fft mul");
			cout << q << ' ' << (p / q) << ' '
				<< "auto : " << A
				<< " best : " << B;
			cout <<" " << (A == B ? "OK" : "FAIL") << '\n';
		}
	}
	return 0;
}
