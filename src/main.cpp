#include <functional>
#include <any>
//#define TEST_SHARED_STRING
#include "../include/mtool.h"
#include "../include/json.h"
#include "../include/String_helper.h"
#include "../include/thread_pool.h"
#include "../include/huffman.h"
using namespace wjr;
using namespace std;

int main() {
    shared_String it(1024,'a');
    shared_String it2(it);
    auto c = it2.mutable_data();
    c[3] = 'b';
    cout << it << '\n';
    cout << it2 << '\n';
    return 0;
}
