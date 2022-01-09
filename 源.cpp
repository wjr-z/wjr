#include <algorithm>
#include <vector>
#include <deque>
#include "src/mtool.h"
#include "src/json/json.h"
using namespace wjr;
using namespace std;

inline bool cmp(int a, int b) {
	return a < b;
}
#include <codecvt>
int main(){
	auto vec = get_all_files("test");
	auto s = mtime();
	for (auto& i : vec) {
		json::eval(read_file(i));
	}
	auto t = mtime();
	cout<<t-s<<'\n';

	return 0;
}