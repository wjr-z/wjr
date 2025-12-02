#include "detail.hpp"

#include <wjr/tuple.hpp>

using namespace wjr;

TEST_CASE("tuple - constructor") {
    static_assert(std::is_constructible_v<tuple<int>, std::tuple<int>>, "");
}