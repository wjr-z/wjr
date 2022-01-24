#include <algorithm>
#include <vector>
#include <deque>
#include "../include/mtool.h"
#include "../include/json.h"
#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <iomanip>
#include <codecvt>
#include <cstdint>

using namespace wjr;
using namespace std;

int main() {
	std::locale::global(std::locale(""));
	String str = "汉字";
	wString s(str);
	std::wcout.imbue(std::locale());
	wcout<<s;
    return 0;
}