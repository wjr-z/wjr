#include <cstdint>
#include <wjr/container/btree_map.hpp>
using tree = wjr::btree_map<uint64_t, uint64_t, std::less<>, std::allocator<char>, 16>;
extern "C" void copy_tree(tree &target, const tree &source) { target = source; }
