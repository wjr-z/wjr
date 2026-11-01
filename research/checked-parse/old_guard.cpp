#include "old_api.hpp"
#include <memory>
int main() {
    auto input = std::make_unique<char[]>(3);
    input[0] = '1';
    input[1] = '2';
    input[2] = '3';
    wjr::biginteger value;
    auto result = wjr::biginteger_detail::_probe_old_from_chars_impl<true>(
        input.get(), input.get() + 3, wjr::biginteger_dispatcher(&value), 10);
    return result.ptr == input.get() + 3 && value == 123 ? 0 : 2;
}
