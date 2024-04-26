#ifndef WJR_MATH_RANDOM_HPP
#define WJR_MATH_RANDOM_HPP

#include <random>

namespace wjr {

template <typename Iter, typename Rand>
void random(Iter First, Iter Last, Rand &&rd) {
    for (; First != Last; ++First) {
        *First = rd();
    }
}

} // namespace wjr

#endif // WJR_MATH_RANDOM_HPP