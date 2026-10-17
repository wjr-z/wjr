#include <wjr/np/view.hpp>

int main() {
    int values[] = {0, 1, 2, 3, 4, 5};
    using shape = wjr::np::extents<size_t, wjr::np::dynamic_extent, 3>;
    wjr::np::tensor_view<int, shape> view(values, 2);
    view(1, 2) = 42;
    return values[5] == 42 && view.size() == 6 ? 0 : 1;
}
