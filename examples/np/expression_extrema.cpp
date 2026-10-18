#include <array>

#include <wjr/np/expression_extrema.hpp>

int main() {
    namespace np = wjr::np;
    using matrix = np::extents<size_t, 2, 3>;
    const std::array<int, 6> values{3, 1, 7, 4, 9, 2};
    const auto input = np::as_expr(np::tensor_view<const int, matrix>(values.data()));
    const auto adjusted = input * 2 + 1;

    const auto minimum = np::min(adjusted);
    const auto maximum = np::max(adjusted);
    std::array<int, 2> row_minima{};
    std::array<int, 3> column_maxima{};
    np::min_to(np::tensor_view<int, np::extents<size_t, 2>>(row_minima.data()), adjusted,
               np::axes<1>);
    np::max_to(np::tensor_view<int, np::extents<size_t, 3>>(column_maxima.data()), adjusted,
               np::axes<0>);

    return minimum == 3 && maximum == 19 && row_minima == std::array{3, 5} &&
                   column_maxima == std::array{9, 19, 15}
               ? 0
               : 1;
}
