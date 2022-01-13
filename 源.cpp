#include <algorithm>
#include <vector>
#include <deque>
#include "src/mtool.h"
#include "src/json/json.h"
using namespace wjr;
using namespace std;
#include "src/string_helper.h"
#include <regex>
struct A {
	struct B {
		static set<int> X;
	};
};
set<int> A::B::X({});
int main(){

	int n = 1e7;
	String_view a("1234");
	int ans1 = 0,ans2 = 0;
	auto s = mtime();
	for (int i = 0; i < n; ++i) {
		bool ok = false;
		int v = basic_String_traits<char>::range_to_int(a.begin(),a.end(),&ok);
		if (ok == true) {
			ans1 += v;
		}
	}
	auto t = mtime();
	cout << t - s << '\n';
	s = mtime();
	for (int i = 0; i < n; ++i) {
		ans2 += atoi(a.begin());
	}
	t = mtime();
	cout << t - s << '\n';
	cout<<ans1<<' '<<ans2<<'\n';
	return 0;
} 