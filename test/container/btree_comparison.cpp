#include <concepts>
#include <map>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/container/btree_map.hpp>

namespace {
template <size_t NodeSize, typename Allocator, typename Compare>
void check_comparisons() {
    using tree = wjr::btree_map<int, int, Compare, Allocator, NodeSize>;
    static_assert(std::totally_ordered<tree>);
    std::vector<std::map<int, int, Compare>> cases;
    for (int count : {0, 1, 2, 3, 80}) {
        for (int variation : {0, 1, 2}) {
            auto &values = cases.emplace_back();
            for (int i = 0; i != count; ++i) {
                values.emplace(i * 2 + (variation == 1), i + (variation == 2 ? 100 : 0));
            }
        }
    }
    for (const auto &lhs_values : cases) {
        for (const auto &rhs_values : cases) {
            const tree lhs(lhs_values.begin(), lhs_values.end());
            const tree rhs(rhs_values.begin(), rhs_values.end());
            EXPECT_EQ(lhs == rhs, lhs_values == rhs_values);
            EXPECT_EQ(lhs != rhs, lhs_values != rhs_values);
            EXPECT_EQ(lhs < rhs, lhs_values < rhs_values);
            EXPECT_EQ(lhs > rhs, lhs_values > rhs_values);
            EXPECT_EQ(lhs <= rhs, lhs_values <= rhs_values);
            EXPECT_EQ(lhs >= rhs, lhs_values >= rhs_values);
        }
    }
}

TEST(BtreeComparison, SupportsAllocatorAndNodeSizeConfigurations) {
    check_comparisons<0, std::allocator<char>, std::less<int>>();
    check_comparisons<0, std::allocator<int>, std::less<int>>();
    check_comparisons<4, std::allocator<char>, std::less<int>>();
    check_comparisons<16, std::allocator<char>, std::greater<int>>();
    check_comparisons<32, std::allocator<int>, std::less<int>>();
    check_comparisons<64, std::allocator<char>, std::less<int>>();
}
} // namespace
