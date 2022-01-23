#include <algorithm>
#include <vector>
#include <deque>
#include "../include/mtool.h"
#include "../include/json.h"
#include <Windows.h>
#include <wchar.h>
using namespace wjr;
using namespace std;
int X;
#include <mutex>
int add() { return ++X; }
#include <codecvt>

int main() {
    //vector<int> vec;
    //string s(vec.begin(),vec.end());
    //wstring_conver;
    wString str = L"ºº×Ö";
    String s(str.convert_to<u16String>());
    for (auto& i : s) {
        printf("%x ",(uint8_t)i);
    }
    printf("\n");

    for (auto& i : str) {
        printf("%x ", (uint16_t)i);
    }
    printf("\n");

    return 0;
}