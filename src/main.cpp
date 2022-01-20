#include <algorithm>
#include <vector>
#include <deque>
#include "../include/mtool.h"
#include "../include/json.h"
using namespace wjr;
using namespace std;
int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    String path;
    cin >> path;
    auto s = mtime();
    auto vec = get_all_files(path);
    //      auto s = mtime();
    auto t = mtime();
    auto X = vec.size();
    auto z = t - s;
    s = mtime();
    vec.resize(remove_if(vec.begin(), vec.end(),
        [](auto& x) {x = x.trim(); return !x.ends_with(".json"); })
        - vec.begin());
    t = mtime();
    cout<<"search done\n";
    auto h = t - s ;
    size_t total = 0, total_accept = 0;
    s = mtime();
    for (auto& i : vec) {
        String str = read_file(i);
        total += str.size();
        if (json::accept(str)) {
            json::eval(str);
            //cout << "done\n";
            total_accept += str.size();
        }
    }
    t = mtime();
    cout << "find " << X << " files at " << z <<"ms\n";
    cout << "find " << vec.size()<< " json at " << h << "ms\n";
    cout << "total size of json is " << total << '\n';
    cout << "total size of accepted json is " << total_accept << '\n';
    cout << "parse all json at " << t - s << '\n';
    return 0;
} 