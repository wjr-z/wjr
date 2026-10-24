#ifndef WJR_NP_SEGMENTED_REDUCE_HPP__
#define WJR_NP_SEGMENTED_REDUCE_HPP__

#include <span>

#include <wjr/np/nan_reduce.hpp>

namespace wjr::np {
namespace detail {

template <class D, class DE, class DL, class T, class SE, class SL, class Index>
concept segmented_vectors =
    kernel_element<D> && !std::is_const_v<D> && kernel_element<T> && DE::rank() == 1 &&
    SE::rank() == 1 && kernel_layout<DL> && kernel_layout<SL> && std::integral<Index> &&
    !std::same_as<std::remove_cv_t<Index>, bool> && !std::is_volatile_v<Index>;

template <class Dst, class Src, class Index, size_t Extent, class Function>
constexpr void for_each_segment(
    Dst dst, [[maybe_unused]] Src src, std::span<Index, Extent> offsets,
    Function &function) noexcept(std::is_nothrow_invocable_v<Function &, size_t, size_t, size_t>) {
    const auto groups = static_cast<size_t>(dst.extent(0));
    if (groups == 0)
        return;
    WJR_ASSERT_L2(!offsets.empty() && offsets.size() - 1 == groups);
    auto begin = static_cast<size_t>(offsets[0]);
    for (size_t group = 0; group != groups; ++group) {
        const auto end = static_cast<size_t>(offsets[group + 1]);
        WJR_ASSERT_L2(begin <= end && end <= static_cast<size_t>(src.extent(0)));
        function(group, begin, end);
        begin = end;
    }
}

template <class Operation, class Acc, class T>
concept segmented_operation =
    kernel_element<Acc> && std::same_as<Acc, std::remove_cv_t<Acc>> &&
    std::invocable<Operation &, Acc, std::remove_cv_t<T>> &&
    std::is_assignable_v<Acc &, std::invoke_result_t<Operation &, Acc, std::remove_cv_t<T>>>;

template <class Operation, class Acc, class T>
inline constexpr bool segmented_nothrow =
    std::is_nothrow_invocable_v<Operation &, Acc, std::remove_cv_t<T>> &&
    std::is_nothrow_assignable_v<Acc &,
                                 std::invoke_result_t<Operation &, Acc, std::remove_cv_t<T>>>;

template <class Acc, bool Mean, bool SkipNaN, class Dst, class Src, class Index, size_t Extent>
constexpr void segmented_sum(Dst dst, Src src, std::span<Index, Extent> offsets) noexcept {
    const auto *input = src.data_handle();
    auto *output = dst.data_handle();
    const auto input_step = axis_stride<0>(src);
    const auto output_step = axis_stride<0>(dst);
    auto reduce = [&](size_t group, size_t begin, size_t end) constexpr noexcept {
        Acc value{};
        [[maybe_unused]] std::conditional_t<Mean && SkipNaN, size_t, no_nan_count> valid{};
        for (size_t i = begin; i != end; ++i) {
            const auto element = input[i * input_step];
            if constexpr (SkipNaN)
                if (element != element)
                    continue;
            value += static_cast<Acc>(element);
            if constexpr (Mean && SkipNaN)
                ++valid;
        }
        if constexpr (Mean) {
            const auto count = [&] {
                if constexpr (SkipNaN)
                    return valid;
                else
                    return end - begin;
            }();
            output[group * output_step] = count == 0 ? std::numeric_limits<Acc>::quiet_NaN()
                                                     : value / static_cast<Acc>(count);
        } else {
            output[group * output_step] = value;
        }
    };
    for_each_segment(dst, src, offsets, reduce);
}

} // namespace detail

// Ordered left folds over [offsets[g], offsets[g+1]). Offsets are read-only,
// nondecreasing, nonnegative and within src; their count is dst.size()+1.
// Empty output ignores offsets. Empty segments return initial. Destination
// storage is disjoint from source and offsets. No storage is allocated; the
// callable is borrowed and initial is copied into each segment's numeric fold.
template <class D, class DE, class DL, class T, class SE, class SL, class Index, size_t Extent,
          class Acc, class Operation>
requires(detail::segmented_vectors<D, DE, DL, T, SE, SL, Index> &&
         detail::segmented_operation<Operation, Acc, T>)
constexpr void
segmented_reduce_to(tensor_view<D, DE, DL> dst, tensor_view<T, SE, SL> src,
                    std::span<Index, Extent> offsets, Acc initial,
                    Operation &&operation) noexcept(detail::segmented_nothrow<Operation, Acc, T>) {
    const auto *input = src.data_handle();
    auto *output = dst.data_handle();
    const auto input_step = detail::axis_stride<0>(src);
    const auto output_step = detail::axis_stride<0>(dst);
    auto reduce = [&](size_t group, size_t begin,
                      size_t end) constexpr noexcept(detail::segmented_nothrow<Operation, Acc, T>) {
        Acc value = initial;
        for (size_t i = begin; i != end; ++i)
            value = std::invoke(operation, std::move(value),
                                static_cast<std::remove_cv_t<T>>(input[i * input_step]));
        output[group * output_step] = value;
    };
    detail::for_each_segment(dst, src, offsets, reduce);
}

// Ordered sums from zero; source conversion precedes each addition. Integer
// overflow and unrepresentable destination conversions violate preconditions.
template <class Accumulator = void, class D, class DE, class DL, class T, class SE, class SL,
          class Index, size_t Extent>
requires(detail::segmented_vectors<D, DE, DL, T, SE, SL, Index> &&
         (std::is_void_v<Accumulator> || detail::sum_accumulator_type<Accumulator>))
constexpr void segmented_sum_to(tensor_view<D, DE, DL> dst, tensor_view<T, SE, SL> src,
                                std::span<Index, Extent> offsets) noexcept {
    detail::segmented_sum<detail::sum_accumulator<Accumulator, T>, false, false>(dst, src, offsets);
}

template <class Accumulator = void, class D, class DE, class DL, class T, class SE, class SL,
          class Index, size_t Extent>
requires(detail::segmented_vectors<D, DE, DL, T, SE, SL, Index> &&
         (std::is_void_v<Accumulator> || detail::sum_accumulator_type<Accumulator>))
constexpr void segmented_nansum_to(tensor_view<D, DE, DL> dst, tensor_view<T, SE, SL> src,
                                   std::span<Index, Extent> offsets) noexcept {
    detail::segmented_sum<detail::sum_accumulator<Accumulator, T>, false,
                          std::is_floating_point_v<T>>(dst, src, offsets);
}

// Floating accumulators; integer/bool input defaults to double. Empty segments
// return quiet NaN, and means divide once after their ordered sum.
template <class Accumulator = void, class D, class DE, class DL, class T, class SE, class SL,
          class Index, size_t Extent>
requires(detail::segmented_vectors<D, DE, DL, T, SE, SL, Index> &&
         detail::mean_accumulator_type<D> &&
         (std::is_void_v<Accumulator> || detail::mean_accumulator_type<Accumulator>))
constexpr void segmented_mean_to(tensor_view<D, DE, DL> dst, tensor_view<T, SE, SL> src,
                                 std::span<Index, Extent> offsets) noexcept {
    detail::segmented_sum<detail::mean_accumulator<Accumulator, T>, true, false>(dst, src, offsets);
}

template <class Accumulator = void, class D, class DE, class DL, class T, class SE, class SL,
          class Index, size_t Extent>
requires(detail::segmented_vectors<D, DE, DL, T, SE, SL, Index> &&
         detail::mean_accumulator_type<D> &&
         (std::is_void_v<Accumulator> || detail::mean_accumulator_type<Accumulator>))
constexpr void segmented_nanmean_to(tensor_view<D, DE, DL> dst, tensor_view<T, SE, SL> src,
                                    std::span<Index, Extent> offsets) noexcept {
    detail::segmented_sum<detail::mean_accumulator<Accumulator, T>, true,
                          std::is_floating_point_v<T>>(dst, src, offsets);
}

} // namespace wjr::np

#endif // WJR_NP_SEGMENTED_REDUCE_HPP__
