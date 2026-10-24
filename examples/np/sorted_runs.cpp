#include <array>
#include <iostream>
#include <span>

#include <wjr/np/difference.hpp>
#include <wjr/np/run_offsets.hpp>
#include <wjr/np/segmented_reduce.hpp>
#include <wjr/np/sort.hpp>

int main() {
    namespace np = wjr::np;
    const int input[] = {3, 1, 3, 2, 1, 3};
    constexpr size_t count = std::size(input);
    using shape = np::dextents<size_t, 1>;
    std::array<int, count> sorted, sums;
    std::array<size_t, count + 1> offsets;
    std::array<size_t, count> lengths;
    const np::tensor_view<int, shape> values(sorted.data(), count);
    np::sort_to(values, np::tensor_view<const int, shape>(input, count));
    const auto groups =
        np::run_offsets_to(np::tensor_view<size_t, shape>(offsets.data(), offsets.size()), values);
    np::segmented_sum_to(np::tensor_view<int, shape>(sums.data(), groups), values,
                         std::span<const size_t>(offsets.data(), groups + 1));
    np::diff_to(np::tensor_view<size_t, shape>(lengths.data(), groups),
                np::tensor_view<const size_t, shape>(offsets.data(), groups + 1), np::axes<0>);
    for (size_t group = 0; group != groups; ++group)
        std::cout << sorted[offsets[group]] << ": count=" << lengths[group]
                  << ", sum=" << sums[group] << '\n';
    return groups == 3 && lengths[0] == 2 && lengths[1] == 1 && lengths[2] == 3 && sums[0] == 2 &&
                   sums[1] == 2 && sums[2] == 9
               ? 0
               : 1;
}
