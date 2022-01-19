#include <algorithm>
#include <vector>
#include <deque>
#include "src/mtool.h"
using namespace wjr;
using namespace std;
int main(){
	String path;
	cin >> path;
	auto vec = get_all_files(path);
	auto s = mtime();
	for (auto& i : vec) {
		i = i.trim();
		if (i.ends_with(".h")) {
			cout<<i<<'\n';
		}
	}
	auto t = mtime();
	cout<<"done in " << t - s <<'\n';
	return 0;
} 