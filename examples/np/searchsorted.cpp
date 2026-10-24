#include <array>
#include <iostream>

#include <wjr/np/searchsorted.hpp>

int main() {
    namespace np = wjr::np;
    // Search an existing ordered grid without sorting or allocating an index.
    const int grid[] = {10, 20, 20, 40};
    const int values[] = {5, 10, 20, 21, 30, 40, 50, 20};
    std::array<size_t, 8> left, right;
    using shape = np::extents<size_t, 8>;
    const np::tensor_view<const int, np::extents<size_t, 4>> sorted(grid);
    const np::tensor_view<const int, shape> queries(values);
    np::searchsorted_to(np::tensor_view<size_t, shape>(left.data()), sorted, queries);
    // The explicit batch path interleaves fixed blocks of rank-one queries.
    np::searchsorted_batch_to<np::search_side::right>(np::tensor_view<size_t, shape>(right.data()),
                                                      sorted, queries);
    for (size_t i = 0; i != left.size(); ++i)
        std::cout << values[i] << ": left=" << left[i] << " right=" << right[i] << '\n';
    return left == std::array<size_t, 8>{0, 0, 1, 3, 3, 3, 4, 1} &&
                   right == std::array<size_t, 8>{0, 1, 3, 3, 3, 4, 4, 3}
               ? 0
               : 1;
}
