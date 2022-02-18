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
    
    auto s = mtime();
    auto j = json::parse(read_file("db.json"));
    auto t = mtime();
    cout << t - s << '\n';
    s = mtime();
    write_file("db2.json",j.minify());
    t = mtime();
    cout << t - s << '\n';
    return 0;
}
