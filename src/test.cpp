#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <iostream>
using namespace std;

int main() {
	uint64_t a, b;
	cin >> a >> b;
	auto val = (__uint128_t)a * b;
	a = (val >> 64);
	b = val;
	cout << a << ' ' << b << '\n';
	return 0;
}