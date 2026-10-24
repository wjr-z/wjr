#ifndef WJR_NP_DETAIL_INDEX_ORDER_HPP__
#define WJR_NP_DETAIL_INDEX_ORDER_HPP__

#include <wjr/np/detail/elementwise.hpp>

namespace wjr::np {

enum class sort_order { ascending, descending };
enum class sort_stability { unstable, stable };

namespace detail {

template <sort_order Order, sort_stability Stability>
concept supported_index_order =
    (Order == sort_order::ascending || Order == sort_order::descending) &&
    (Stability == sort_stability::unstable || Stability == sort_stability::stable);

// A packed reader carries only a pointer; the runtime stride exists only when
// the source mapping needs it. Keep that state identical for sort and selection.
template <class Index, class T, class E, class L>
constexpr auto index_reader(tensor_view<T, E, L> source) noexcept {
    const auto *input = source.data_handle();
    if constexpr (packed_layout<L> || padded_layout<L>)
        return [input](Index i) noexcept { return input[static_cast<size_t>(i)]; };
    else
        return [input, step = source.stride(0)](Index i) noexcept {
            return input[static_cast<size_t>(i) * step];
        };
}

template <sort_order Order, sort_stability Stability, class Reader>
struct index_order {
    Reader read;

    template <class Index>
    constexpr bool operator()(Index a, Index b) const noexcept {
        const auto left = read(a);
        const auto right = read(b);
        if constexpr (std::is_floating_point_v<decltype(left)>) {
            if (left != left)
                return Stability == sort_stability::stable && right != right && a < b;
            if (right != right)
                return true;
        }
        if constexpr (Stability == sort_stability::stable)
            if (left == right)
                return a < b;
        if constexpr (Order == sort_order::ascending)
            return left < right;
        else
            return left > right;
    }
};

} // namespace detail
} // namespace wjr::np

#endif // WJR_NP_DETAIL_INDEX_ORDER_HPP__
