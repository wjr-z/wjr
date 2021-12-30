#include <algorithm>
#include <vector>
#include <deque>
#include "src/string_helper.h"
#include "src/mtool.h"
#include "src/json/json.h"
#include "src/mySTL.h"
#include "src/String.h"
using namespace wjr;
using namespace std;

int main(){

	String s1;
	string s2;
	int n = 1e6;
	auto s = mtime();
	for (auto i = 0; i < n; ++i) {
		s2 = std::move(s2) + "wjr";
	}
	auto t = mtime();
	cout<<t-s<<'\n';

	s = mtime();
	for (auto i = 0; i < n; ++i) {
		s1 = std::move(s1) + "wjr";
	}
	t = mtime();
	cout<<t-s<<'\n';
	
	return 0;
}