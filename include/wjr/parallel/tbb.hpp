#ifndef WJR_PARALLEL_TBB_HPP__
#define WJR_PARALLEL_TBB_HPP__

#include <concepts>
#include <cstddef>
#include <functional>

#include <oneapi/tbb/blocked_range.h>
#include <oneapi/tbb/parallel_for.h>
#include <oneapi/tbb/partitioner.h>

#include <wjr/assert.hpp>

namespace wjr::tbb {

// Synchronously borrow one const callable. Without cancellation, chunks cover
// [first,last) exactly once; grain is a splitting hint, not a maximum size.
// The callable and its referenced resources must support concurrent calls.
template <class F>
requires std::invocable<const F &, std::size_t, std::size_t>
void parallel_for_chunks(std::size_t first, std::size_t last, std::size_t grain,
                         const F &function) {
    WJR_ASSERT_L2(first <= last);
    WJR_ASSERT_L2(grain > 0);
    if (first == last)
        return;
    oneapi::tbb::parallel_for(
        oneapi::tbb::blocked_range<std::size_t>{first, last, grain},
        [&function](const oneapi::tbb::blocked_range<std::size_t> &range) {
            (void)std::invoke(function, range.begin(), range.end());
        },
        oneapi::tbb::auto_partitioner{});
}

} // namespace wjr::tbb

#endif // WJR_PARALLEL_TBB_HPP__
