#ifndef WJR_MATH_RANDOM_HPP__
#define WJR_MATH_RANDOM_HPP__

#include <random>

namespace wjr {

template <typename Iter, typename Rand>
void random(Iter First, Iter Last, Rand &&rd) {
    for (; First != Last; ++First) {
        *First = rd();
    }
}

// TODO

} // namespace wjr

#endif // WJR_MATH_RANDOM_HPP__