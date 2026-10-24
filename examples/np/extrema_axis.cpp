#include <array>
#include <iostream>

#include <wjr/np/extrema_axis.hpp>

int main() {
    namespace np = wjr::np;
    const double values[] = {3., 1., 1., 2., 7., 4., 8., 6., 5.};
    const np::tensor_view<const double, np::extents<size_t, 3, 3>> matrix(values);
    std::array<unsigned, 3> row_minima, column_maxima;
    np::argmin_to(np::tensor_view<unsigned, np::extents<size_t, 3>>(row_minima.data()), matrix,
                  np::axes<1>);
    np::argmax_to(np::tensor_view<unsigned, np::extents<size_t, 1, 3>>(column_maxima.data()),
                  matrix, np::axes<0>, np::keepdims);
    for (size_t row = 0; row != row_minima.size(); ++row)
        std::cout << "row " << row << ": minimum at column " << row_minima[row] << '\n';
    for (size_t column = 0; column != column_maxima.size(); ++column)
        std::cout << "column " << column << ": maximum at row " << column_maxima[column] << '\n';
    return row_minima == std::array{1u, 0u, 2u} && column_maxima == std::array{2u, 1u, 2u} ? 0 : 1;
}
