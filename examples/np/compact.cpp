#include <iostream>

#include <wjr/np/compact.hpp>

int main() {
    namespace np = wjr::np;
    using shape = np::dextents<size_t, 1>;
    const double values[] = {3., 0., 5., 0., 7.};
    const bool mask[] = {true, false, false, true, true};
    double selected[5];
    size_t positions[5];
    const auto kept = np::compress_to(np::tensor_view<double, shape>(selected, 5),
                                      np::tensor_view<const double, shape>(values, 5),
                                      np::tensor_view<const bool, shape>(mask, 5));
    const auto found = np::flatnonzero_to(np::tensor_view<size_t, shape>(positions, 5),
                                          np::tensor_view<const double, shape>(values, 5));
    std::cout << "Selected:";
    for (size_t i = 0; i != kept; ++i)
        std::cout << ' ' << selected[i];
    std::cout << "\nNonzero positions:";
    for (size_t i = 0; i != found; ++i)
        std::cout << ' ' << positions[i];
    std::cout << '\n';
    return kept == 3 && found == 3 && selected[0] == 3 && selected[1] == 0 && selected[2] == 7 &&
                   positions[0] == 0 && positions[1] == 2 && positions[2] == 4
               ? 0
               : 1;
}
