#ifndef WJR_NP_QUANTILE_HPP__
#define WJR_NP_QUANTILE_HPP__

#include <cmath>
#include <limits>

#include <wjr/np/partition.hpp>
#include <wjr/np/reduce.hpp>

namespace wjr::np {

enum class quantile_nan_policy { propagate, omit };

namespace detail {

template <class D, class DE, class DL, class T, class SE, class SL, class Q, class QE, class QL>
concept quantile_vectors =
    std::floating_point<D> && std::same_as<D, std::remove_cv_t<D>> && kernel_element<T> &&
    std::floating_point<Q> && !std::is_volatile_v<Q> && DE::rank() == 1 && SE::rank() == 1 &&
    QE::rank() == 1 && kernel_layout<DL> && kernel_layout<SL> && kernel_layout<QL> &&
    compatible_kernel_shape<DE, QE>;

template <class Calc>
struct quantile_position {
    size_t lower;
    Calc fraction;
};

template <class Calc, class Q>
constexpr quantile_position<Calc> locate_quantile(Q probability, size_t last) noexcept {
    const auto position = static_cast<Calc>(probability) * static_cast<Calc>(last);
    // Floating conversion can round last upward. Never convert an endpoint
    // into an out-of-range rank, including for wide counts and q == 1.
    if (position >= static_cast<Calc>(last))
        return {last, Calc{0}};
    const auto lower = static_cast<size_t>(position);
    return {lower, position - static_cast<Calc>(lower)};
}

template <class Calc>
constexpr Calc interpolate_quantile(Calc left, Calc right, Calc fraction) noexcept {
    if (fraction == 0 || left == right)
        return left;
    // std::lerp's finite guarantees do not define an extended-real policy.
    // A single infinite endpoint dominates an interior interpolation;
    // opposite infinities have no defined result.
    const auto infinite = [](Calc value) constexpr noexcept {
        if (std::is_constant_evaluated())
            return value == std::numeric_limits<Calc>::infinity() ||
                   value == -std::numeric_limits<Calc>::infinity();
        return std::isinf(value);
    };
    const bool left_inf = infinite(left), right_inf = infinite(right);
    if (left_inf || right_inf)
        return left_inf && right_inf ? std::numeric_limits<Calc>::quiet_NaN()
               : left_inf            ? left
                                     : right;
    return std::lerp(left, right, fraction);
}

template <class Dst>
constexpr void fill_quantile_nan(Dst dst) noexcept {
    apply_kernel(dst, [](typename Dst::reference value) noexcept {
        value = std::numeric_limits<typename Dst::value_type>::quiet_NaN();
    });
}

// Values is a mutable, NaN-free prefix. Its dtype is preserved through selection;
// only the selected endpoints are converted for interpolation.
template <class Dst, class T, class Queries>
constexpr void quantiles_to(Dst dst, T *values, size_t count, Queries probabilities,
                            std::span<size_t> rank_workspace) noexcept {
    if (count == 0) {
        fill_quantile_nan(dst);
        return;
    }
    using calc =
        std::common_type_t<T, typename Queries::value_type, typename Dst::value_type, double>;
    const auto queries = static_cast<size_t>(dst.extent(0));
    WJR_ASSERT_L2(rank_workspace.size() / 2 >= queries);
    auto ranks = rank_workspace.first(queries * 2);
    const auto *query = probabilities.data_handle();
    const auto query_step = axis_stride<0>(probabilities);
    for (size_t i = 0; i != queries; ++i) {
        const auto q = query[i * query_step];
        WJR_ASSERT_L2(q >= 0 && q <= 1);
        const auto [lower, fraction] = locate_quantile<calc>(q, count - 1);
        ranks[i * 2] = lower;
        ranks[i * 2 + 1] = lower + (fraction != 0);
    }
    std::sort(ranks.begin(), ranks.end());
    select_ranks<std::less<>>(values, 0, count, ranks,
                              partition_depth_factor * std::bit_width(count));
    auto *output = dst.data_handle();
    const auto output_step = axis_stride<0>(dst);
    for (size_t i = 0; i != queries; ++i) {
        const auto [lower, fraction] = locate_quantile<calc>(query[i * query_step], count - 1);
        output[i * output_step] =
            interpolate_quantile(static_cast<calc>(values[lower]),
                                 static_cast<calc>(values[lower + (fraction != 0)]), fraction);
    }
}

} // namespace detail

// Linear quantiles at probabilities in [0,1], in original query order.
// Values may be reordered; omit also compacts non-NaNs into the prefix and
// leaves the discarded tail unspecified. Rank workspace contains at least 2Q
// live size_t objects, disjoint from all views. Values, output, and probabilities
// have pairwise disjoint logical storage.
template <quantile_nan_policy NaN = quantile_nan_policy::propagate, class D, class DE, class DL,
          class T, class SE, class SL, class Q, class QE, class QL, size_t Extent>
requires((NaN == quantile_nan_policy::propagate || NaN == quantile_nan_policy::omit) &&
         detail::quantile_vectors<D, DE, DL, T, SE, SL, Q, QE, QL> && !std::is_const_v<T> &&
         detail::packed_layout<SL>)
constexpr void quantile_inplace_to(tensor_view<D, DE, DL> dst, tensor_view<T, SE, SL> values,
                                   tensor_view<Q, QE, QL> probabilities,
                                   std::span<size_t, Extent> rank_workspace) noexcept {
    detail::assert_kernel_shape(dst, probabilities);
    if (dst.empty())
        return;
    auto *data = values.data_handle();
    auto count = static_cast<size_t>(values.extent(0));
    if constexpr (std::is_floating_point_v<T>) {
        if (count != 0) {
            const auto missing = [](T value) constexpr noexcept { return value != value; };
            if constexpr (NaN == quantile_nan_policy::omit)
                count = static_cast<size_t>(std::remove_if(data, data + count, missing) - data);
            else if (std::any_of(data, data + count, missing)) {
                detail::fill_quantile_nan(dst);
                return;
            }
        }
    }
    detail::quantiles_to(dst, data, count, probabilities, std::span<size_t>(rank_workspace));
}

// Preserve the source using caller-owned live workspace of the source dtype.
// Data workspace has at least N elements, disjoint from source, probabilities,
// destination and rank workspace. All numeric conversions must be representable.
// Source and probabilities remain stable. No allocation or implicit copy-back.
template <quantile_nan_policy NaN = quantile_nan_policy::propagate, class D, class DE, class DL,
          class T, class SE, class SL, class Q, class QE, class QL, size_t DataExtent,
          size_t RankExtent>
requires((NaN == quantile_nan_policy::propagate || NaN == quantile_nan_policy::omit) &&
         detail::quantile_vectors<D, DE, DL, T, SE, SL, Q, QE, QL>)
constexpr void quantile_to(tensor_view<D, DE, DL> dst, tensor_view<T, SE, SL> source,
                           tensor_view<Q, QE, QL> probabilities,
                           std::span<std::remove_cv_t<T>, DataExtent> data_workspace,
                           std::span<size_t, RankExtent> rank_workspace) noexcept {
    detail::assert_kernel_shape(dst, probabilities);
    if (dst.empty())
        return;
    const auto count = static_cast<size_t>(source.extent(0));
    WJR_ASSERT_L2(data_workspace.size() >= count);
    const auto *input = source.data_handle();
    const auto step = detail::axis_stride<0>(source);
    size_t valid = 0;
    for (size_t i = 0; i != count; ++i) {
        const auto value = input[i * step];
        if constexpr (std::is_floating_point_v<T>) {
            if (value != value) {
                if constexpr (NaN == quantile_nan_policy::propagate) {
                    detail::fill_quantile_nan(dst);
                    return;
                } else {
                    continue;
                }
            }
        }
        data_workspace[valid++] = value;
    }
    detail::quantiles_to(dst, data_workspace.data(), valid, probabilities,
                         std::span<size_t>(rank_workspace));
}

} // namespace wjr::np

#endif // WJR_NP_QUANTILE_HPP__
