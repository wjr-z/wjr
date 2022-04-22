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

int main() {
	int cnt = 0;
	while (true) {
		biginteger<2> a = random_biginteger<2>(1000);
		biginteger<2> b = random_biginteger<2>(1000);
		int c = rand() % 4;
		if (c & 1) {
			a.change_signal();
		}
		if (c >> 1) {
			b.change_signal();
		}
		if ((a - b) + b != a) {
			printf("error\n");
			return 0;
		}
		if ((++cnt) % 10000 == 0) {
			cout << cnt << '\n';
		}
	}
	return 0;
	//freopen("data.out", "w", stdout);
	size_t T = (1 << 16) ;
	for (size_t i = 4; i < (1 << 18); i <<= 1) {
		size_t n = i * 32;
		cout << "n : " << n << "\n\n";
		auto a = random_biginteger<2>(n);
		for (double alpha = 0.05; alpha <= 1 + 1e-10; alpha += 0.05) {
			auto b = random_biginteger<2>(n * alpha);
			auto rd = max(1ull, T / i);
			double p = 0;
			biginteger<2> c, d;
			for (size_t j = 0; j < rd; ++j) {
				c = a;
				auto s = mtime();
				c *= b;
				p += mtime() - s;
			}
			double q = 0;
			for (size_t j = 0; j < rd;++ j) {
				c = a, d = b;
				auto s = mtime();
				c / d;
				q += mtime() - s;
			}
			cout << "alpha : " << alpha << " mul : " << p << " divide : " << q << " ratio : " << q / p << '\n';
		}
		cout << "\n\n";
	}
	return 0;
}
