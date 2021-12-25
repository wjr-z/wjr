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

	String s = "wjrwjr3";

	String g = "wjr";

	skmp_searcher it(g.rbegin(),g.rend());

	cout<<(it(s.rbegin(),s.rend()).first-s.rbegin())<<'\n';

	return 0;
}