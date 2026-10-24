#include <iostream>

#include <wjr/np/bincount.hpp>

int main() {
    namespace np = wjr::np;
    using shape = np::dextents<size_t, 1>;
    const unsigned categories[] = {2, 0, 2, 1, 0};
    const double values[] = {1.5, 2., 3.5, -1., 4.};
    unsigned counts[4];
    double totals[4];
    const np::tensor_view<const unsigned, shape> indices(categories, 5);
    np::bincount_to(np::tensor_view<unsigned, shape>(counts, 4), indices);
    np::bincount_to(np::tensor_view<double, shape>(totals, 4), indices,
                    np::tensor_view<const double, shape>(values, 5));
    for (size_t i = 0; i != 4; ++i)
        std::cout << i << ": count=" << counts[i] << ", total=" << totals[i] << '\n';
    return counts[0] == 2 && counts[1] == 1 && counts[2] == 2 && counts[3] == 0 && totals[0] == 6 &&
                   totals[1] == -1 && totals[2] == 5 && totals[3] == 0
               ? 0
               : 1;
}
