#include <functional>
#include <any>
#include "../include/mtool.h"
#include "../include/json.h"
#include "../include/String_helper.h"
#include "../include/thread_pool.h"
#include "../include/huffman.h"
using namespace wjr;
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    uint8_t x = 127;
    cout<<(x>>8)<<'\n';

    String str = read_file("canada.json");
    String str2(str.size(), Reserved{});
    cout << str.size() << '\n';
    auto s = mtime();
    size_t l = huffman_compress(str.data(),str.length(),str2.data());
    auto t = mtime();
    cout << t - s << '\n';
    str2.set_size(l);
    cout << l << '\n';
    s = mtime();
    l = huffman_decompress(str2.data(),str2.size(),str.data(),str.capacity());
    t = mtime();
    cout << t - s << '\n';
    str.set_size(l);
    cout << l << '\n';
    cout <<(str == read_file("canada.json"))<<'\n';
    return 0;
}
