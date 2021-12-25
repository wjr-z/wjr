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

	cout<<is_reverse_iterator<int*>::value<<'\n'<<is_reverse_iterator<String::reverse_iterator>::value;

	reverse_iterator<int*> g;
	int*h = g.base();

	return 0;
}