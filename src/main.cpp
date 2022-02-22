#include <functional>
#include <any>
#include <thread>
#include "../include/mtool.h"
#include "../include/deflate.h"
#include "../include/thread_pool.h"
#include "../include/json.h"
using namespace wjr;
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    auto str = read_file("canada.json");
    auto s = mtime();
    auto str2 = deflate_compress(str,6);
    auto t = mtime();
    cout << t - s << '\n';
    cout << str2.size() / 1024 << '\n';
    cout << (str.size() / (1024.0) / 1024 / (t - s) * 1000) << " MB/s\n";
    s = mtime();
    String str3 = deflate_uncompress(str2);
    t = mtime();
    cout << t - s << '\n';
    cout << (str3 == str) << '\n';
    cout << (str2.size() / (1024.0) / 1024 / (t - s) * 1000) << " MB/s\n";
    return 0;
}
