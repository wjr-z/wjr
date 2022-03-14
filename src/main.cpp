#include <functional>
#include <any>
#include <thread>
#include <source_location>
#include <bitset>
#include <conio.h>
#include <variant>
#include "../include/mtool.h"
#include "../include/deflate.h"
#include "../include/thread_pool.h"
#include "../include/json.h"
#include "../include/graphic.h"
#include "../include/exist_ptr.h"
using namespace wjr;
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    variant<int,double,String> x(3),y(3);
    cout << sizeof(x) << '\n';
    cout << x.index() << '\n';
    struct node {
        void operator()(int){cout <<"int\n"; }
        void operator()(double){cout << "double\n"; }
        void operator()(const String&){cout << "String\n"; }
    };
    return 0;
}
