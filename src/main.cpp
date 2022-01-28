#define STRING_SEARCHER_DISABLE_MEMCHR_TEST
#include "../include/mtool.h"
#include "../include/json.h"
#include "../json/single_include/nlohmann/json.hpp"
#include <functional>
using namespace wjr;
using namespace std;
int main() {
    String str = read_file("canada.json");
    //str.resize(1e6);
    //generate(str.begin(),str.end(),[](){return 'a';});
    String g = "01234376728917asdhjkkjhj{A}{}:,aoznvknxciuwqejlkfsdjlkdfsjklfds";
    //memrchr()
    //g.resize(128,'a');
    //g.push_back('b');
    //g.push_back('a');
    int n = 1e1;
    int ans1 = 0,ans2 = 0;
    auto it1 = String::get_find_helper(g);
    auto s = mtime();
    for (int i = 0; i < n; ++i) {
        ans1 += str.find(it1);
    }
    auto t = mtime();
    cout << t - s << '\n';
    cout << ans1 << '\n';

    boyer_moore_horspool_searcher it2(g.data(), g.data() + g.size());
    s = mtime();
    for (int i = 0; i < n; ++i) {
        ans2 += search(str.data(), str.data() + str.size(), it2) - str.data();
    }
    t = mtime();
    cout << t - s << '\n';
    cout << ans2 << '\n';
    return 0;
}
