#include <functional>
#include "../include/exist_ptr.h"
#include "../include/graphic.h"
#include "../include/json.h"
#include "../include/mtool.h"
#include "../include/thread_pool.h"
#include "../include/biginteger.h"
#include <random>
using namespace wjr;
using namespace std;

int main() {
	biginteger<2> big = random_biginteger<2>(1e6 * 32);
	biginteger<2> small = random_biginteger<2>(5e5 * 32);
	auto s = mtime();
	big / small;
	auto t = mtime();
	cout << t - s << '\n';
    return 0;
}

