#ifndef WJR_NP_RUN_OFFSETS_HPP__
#define WJR_NP_RUN_OFFSETS_HPP__

#include <functional>
#include <limits>
#include <utility>

#include <wjr/np/reduce.hpp>

namespace wjr::np {
namespace detail {

template <class Equal, class T>
concept run_equality = requires(Equal &equal, const T &left, const T &right) {
    static_cast<bool>(std::invoke(equal, left, right));
};

template <class Equal, class T>
inline constexpr bool run_equality_nothrow = noexcept(static_cast<bool>(
    std::invoke(std::declval<Equal &>(), std::declval<const T &>(), std::declval<const T &>())));

template <bool Contiguous, class D, class T, class Step, class Equal>
constexpr size_t run_offsets_line(D *WJR_RESTRICT output, [[maybe_unused]] size_t capacity,
                                  const T *input, size_t count, Step step,
                                  Equal &equal) noexcept(run_equality_nothrow<Equal, T>) {
    WJR_ASSERT_L2(capacity != 0);
    output[0] = 0;
    if (count == 0)
        return 0;
    size_t runs = 1;
    if constexpr (Contiguous) {
        for (size_t i = 1; i != count; ++i) {
            if (!static_cast<bool>(std::invoke(equal, input[(i - 1) * step], input[i * step]))) {
                WJR_ASSERT_L2(runs < capacity);
                output[runs++] = static_cast<D>(i);
            }
        }
    } else {
        const T *previous = input;
        for (size_t i = 1; i != count; ++i) {
            const T *current = previous + step;
            if (!static_cast<bool>(std::invoke(equal, *previous, *current))) {
                WJR_ASSERT_L2(runs < capacity);
                output[runs++] = static_cast<D>(i);
            }
            previous = current;
        }
    }
    WJR_ASSERT_L2(runs < capacity);
    output[runs] = static_cast<D>(count);
    return runs;
}

} // namespace detail

// Emit [0, adjacent run starts..., keys.size()] and return the number of runs.
// Empty keys emit one zero. Destination must have room for the actual runs+1
// offsets, represent keys.size(), and be disjoint from keys. Its suffix is
// untouched. The borrowed callable is invoked once per adjacent pair in order;
// it must not access destination storage or modify key storage.
// a throwing comparison leaves the written prefix without a final sentinel.
template <class D, class DE, class DL, class T, class TE, class TL, class Equal = std::equal_to<>>
requires(std::integral<D> && !std::same_as<D, bool> && !std::is_const_v<D> &&
         !std::is_volatile_v<D> && detail::kernel_element<T> && DE::rank() == 1 &&
         TE::rank() == 1 && detail::packed_layout<DL> && detail::kernel_layout<TL> &&
         detail::run_equality<Equal, T>)
constexpr size_t
run_offsets_to(tensor_view<D, DE, DL> dst, tensor_view<T, TE, TL> keys,
               Equal &&equal = {}) noexcept(detail::run_equality_nothrow<Equal, T>) {
    const auto count = static_cast<size_t>(keys.extent(0));
    WJR_ASSERT_L2(count <= static_cast<std::make_unsigned_t<D>>(std::numeric_limits<D>::max()));
    return detail::run_offsets_line < detail::packed_layout<TL> ||
           detail::padded_layout < TL >> (dst.data_handle(), static_cast<size_t>(dst.extent(0)),
                                          keys.data_handle(), count, detail::axis_stride<0>(keys),
                                          equal);
}

} // namespace wjr::np

#endif // WJR_NP_RUN_OFFSETS_HPP__
