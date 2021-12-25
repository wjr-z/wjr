#include <algorithm>
#include <vector>
#include <deque>
#include "src/string_tool.h"
#include "src/mtool.h"
#include "src/json/json.h"
#include "src/mySTL.h"
#include "src/String.h"
using namespace wjr;
using namespace std;
#include <string>
int main(){	
	{
		json w[3];
		auto vec = get_all_files("test");
		auto s = mtime();
		int x = 0;
		for (auto i : vec) {
			w[x++] = json::eval(read_file(i));
		}
		auto t = mtime();
		cout << t - s << '\n';
	}

	{
		json w[3];
		auto vec = get_all_files("test");
		auto s = mtime();
		int x = 0;
		for (auto i : vec) {
			w[x++] = json::eval(read_file(i));
		}
		auto t = mtime();
		cout << t - s << '\n';
	}

	return 0;
}