#include <functional>
#include <any>
#include "../include/mtool.h"
#include "../include/json.h"
#include "../include/String_helper.h"
#include "../include/thread_pool.h"
#include "../include/huffman.h"
#include "../include/deflate.h"
using namespace wjr;
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    String str = read_file("canada.json");
    String ans(str);
    String str2(str.size() * 1.05, Reserved{});
    cout << str.size() << '\n';
    auto s = mtime();
    auto len = deflate_compress(str.data(),str.size(),str2.data(),str2.capacity());
    str2.set_size(len);
    auto t = mtime();
    cout << t - s << '\n';
    cout << len / 1024<< '\n';
    s = mtime();
    len = deflate_decompress(str2.data(),str2.size(),str.data(),str.capacity());
    t = mtime();
    str.set_size(len);
    cout << t - s << '\n';
    cout << str.size() << '\n';

    cout <<(str == ans) << '\n';

    return 0;
}
