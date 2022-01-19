#include <algorithm>
#include <vector>
#include <deque>
#include "src/mtool.h"
using namespace wjr;
using namespace std;
#include <regex>
int main(){
	String str = "D:/wjrblog2";
	auto vec = get_all_files(str);
	vec.resize(remove_if(vec.begin(), vec.end(), [](auto& x) {
		return x.contains(".deploy_git") || x.contains("wjrblog2/public")
			|| x.contains("node_modules") || x.contains("¸±±¾") || x.contains(".md"); }) - vec.begin());
	auto s = mtime();
	for (auto& i : vec) {
		auto s = read_file(i);
		//cout<<i<<'\n';
		if (s.contains(".js")) {
			cout<<i<<'\n';
		}
	}
	auto t = mtime();
	cout << "done at " << t - s << '\n';
	return 0;
} 