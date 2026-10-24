#ifndef WJR_NP_STATISTICS_HPP__
#define WJR_NP_STATISTICS_HPP__

#include <cmath>

#include <wjr/np/mean.hpp>

namespace wjr::np {
namespace detail {

template <class Acc, class X, class XE, class XL, class Y, class YE, class YL>
concept statistic_vectors =
    kernel_element<X> && kernel_element<Y> && kernel_layout<XL> && kernel_layout<YL> &&
    XE::rank() == 1 && YE::rank() == 1 && compatible_kernel_shape<XE, YE> &&
    (std::is_void_v<Acc> || mean_accumulator_type<Acc>);

template <class View>
constexpr auto statistic_reader(View view) noexcept {
    const auto *data = view.data_handle();
    if constexpr (packed_layout<typename View::layout_type> ||
                  padded_layout<typename View::layout_type>)
        return [data](size_t i) constexpr noexcept { return data[i]; };
    else
        return
            [data, step = view.stride(0)](size_t i) constexpr noexcept { return data[i * step]; };
}

template <bool SkipNaN, class X, class Y>
constexpr bool statistic_pair(X x, Y y) noexcept {
    if constexpr (SkipNaN && std::is_floating_point_v<X>)
        if (x != x)
            return false;
    if constexpr (SkipNaN && std::is_floating_point_v<Y>)
        if (y != y)
            return false;
    return true;
}

template <class Acc>
Acc normalized_correlation(Acc cross, Acc xx, Acc yy) noexcept {
    const auto xnorm = std::sqrt(xx);
    const auto ynorm = std::sqrt(yy);
    // Divide by the smaller norm first: cross/(large*small) can overflow
    // the product, and cross/large can underflow before division by small.
    const auto value = xnorm < ynorm ? (cross / xnorm) / ynorm : (cross / ynorm) / xnorm;
    return value < Acc{-1} ? Acc{-1} : value > Acc{1} ? Acc{1} : value;
}

// Extreme magnitudes are uncommon. Keep scaling and finite classification out
// of the ordinary two-pass kernel, and reread stable borrowed input only here.
template <class Acc, bool SkipNaN, class X, class Y>
WJR_NOINLINE WJR_COLD Acc scaled_correlation(X x, Y y, size_t count, size_t valid) noexcept {
    Acc xmin = std::numeric_limits<Acc>::infinity();
    Acc xmax = -xmin, ymin = xmin, ymax = xmax;
    for (size_t i = 0; i != count; ++i) {
        const auto raw_x = x(i);
        const auto raw_y = y(i);
        if (!statistic_pair<SkipNaN>(raw_x, raw_y))
            continue;
        const auto a = static_cast<Acc>(raw_x);
        const auto b = static_cast<Acc>(raw_y);
        if (!std::isfinite(a) || !std::isfinite(b))
            return std::numeric_limits<Acc>::quiet_NaN();
        xmin = a < xmin ? a : xmin;
        xmax = a > xmax ? a : xmax;
        ymin = b < ymin ? b : ymin;
        ymax = b > ymax ? b : ymax;
    }
    if (xmin == xmax || ymin == ymax)
        return std::numeric_limits<Acc>::quiet_NaN();
    const auto xscale = std::abs(xmin) > std::abs(xmax) ? std::abs(xmin) : std::abs(xmax);
    const auto yscale = std::abs(ymin) > std::abs(ymax) ? std::abs(ymin) : std::abs(ymax);
    Acc xsum{}, ysum{};
    for (size_t i = 0; i != count; ++i) {
        const auto a = x(i);
        const auto b = y(i);
        if (!statistic_pair<SkipNaN>(a, b))
            continue;
        xsum += static_cast<Acc>(a) / xscale;
        ysum += static_cast<Acc>(b) / yscale;
    }
    const auto xmean = xsum / static_cast<Acc>(valid);
    const auto ymean = ysum / static_cast<Acc>(valid);
    Acc cross{}, xx{}, yy{};
    for (size_t i = 0; i != count; ++i) {
        const auto a = x(i);
        const auto b = y(i);
        if (!statistic_pair<SkipNaN>(a, b))
            continue;
        const auto dx = static_cast<Acc>(a) / xscale - xmean;
        const auto dy = static_cast<Acc>(b) / yscale - ymean;
        cross += dx * dy;
        xx += dx * dx;
        yy += dy * dy;
    }
    return xx == 0 || yy == 0 ? std::numeric_limits<Acc>::quiet_NaN()
                              : normalized_correlation(cross, xx, yy);
}

template <class Acc, bool Correlation, bool SkipNaN, class X, class Y>
constexpr Acc paired_statistic(X x, Y y, size_t count, size_t ddof) noexcept {
    if (count <= ddof)
        return std::numeric_limits<Acc>::quiet_NaN();
    size_t first = 0;
    if constexpr (SkipNaN) {
        while (first != count && !statistic_pair<true>(x(first), y(first)))
            ++first;
        if (first == count)
            return std::numeric_limits<Acc>::quiet_NaN();
    }
    const auto xanchor = static_cast<Acc>(x(first));
    const auto yanchor = static_cast<Acc>(y(first));
    size_t valid = SkipNaN ? 0 : count;
    Acc xsum{}, ysum{};
    for (size_t i = first; i != count; ++i) {
        const auto a = x(i);
        const auto b = y(i);
        if (!statistic_pair<SkipNaN>(a, b))
            continue;
        xsum += static_cast<Acc>(a) - xanchor;
        ysum += static_cast<Acc>(b) - yanchor;
        if constexpr (SkipNaN)
            ++valid;
    }
    if constexpr (SkipNaN)
        if (valid <= ddof)
            return std::numeric_limits<Acc>::quiet_NaN();
    const auto xmean = xanchor + xsum / static_cast<Acc>(valid);
    const auto ymean = yanchor + ysum / static_cast<Acc>(valid);
    Acc cross{};
    [[maybe_unused]] Acc xx{}, yy{};
    for (size_t i = first; i != count; ++i) {
        const auto a = x(i);
        const auto b = y(i);
        // Select deviations before the dependent fold. Selecting the updated
        // accumulator instead can lengthen its dependency chain on every pair.
        const bool retained = statistic_pair<SkipNaN>(a, b);
        const Acc dx = retained ? static_cast<Acc>(a) - xmean : Acc{0};
        const Acc dy = retained ? static_cast<Acc>(b) - ymean : Acc{0};
        cross += dx * dy;
        if constexpr (Correlation) {
            xx += dx * dx;
            yy += dy * dy;
        }
    }
    if constexpr (Correlation) {
        using limits = std::numeric_limits<Acc>;
        if (xx >= limits::min() && xx <= limits::max() && yy >= limits::min() &&
            yy <= limits::max() && std::isfinite(cross))
            return normalized_correlation(cross, xx, yy);
        return scaled_correlation<Acc, SkipNaN>(x, y, count, valid);
    } else {
        return cross / static_cast<Acc>(valid - ddof);
    }
}

template <class Acc, bool Correlation, bool SkipNaN, class X, class Y>
constexpr auto statistic_views(X x, Y y, size_t ddof) noexcept {
    assert_kernel_shape(x, y);
    using accumulator =
        mean_accumulator<Acc,
                         std::common_type_t<typename X::element_type, typename Y::element_type>>;
    return paired_statistic<accumulator, Correlation, SkipNaN>(
        statistic_reader(x), statistic_reader(y), static_cast<size_t>(x.extent(0)), ddof);
}

} // namespace detail

// Scalar covariance of equal-length vectors. Ordered means followed by centered
// products, with no temporary arrays. Default ddof=1 is sample covariance;
// count <= ddof returns NaN. NaNs propagate and inputs may alias each other.
template <class Accumulator = void, class X, class XE, class XL, class Y, class YE, class YL>
requires detail::statistic_vectors<Accumulator, X, XE, XL, Y, YE, YL>
[[nodiscard]] constexpr auto covariance(tensor_view<X, XE, XL> x, tensor_view<Y, YE, YL> y,
                                        size_t ddof = 1) noexcept {
    return detail::statistic_views<Accumulator, false, false>(x, y, ddof);
}

// Pairwise NaN deletion: omit an observation if either input is NaN. Infinity
// is not a missing value. The divisor uses the retained pair count.
template <class Accumulator = void, class X, class XE, class XL, class Y, class YE, class YL>
requires detail::statistic_vectors<Accumulator, X, XE, XL, Y, YE, YL>
[[nodiscard]] constexpr auto nancovariance(tensor_view<X, XE, XL> x, tensor_view<Y, YE, YL> y,
                                           size_t ddof = 1) noexcept {
    constexpr bool skip = std::is_floating_point_v<X> || std::is_floating_point_v<Y>;
    return detail::statistic_views<Accumulator, false, skip>(x, y, ddof);
}

// Pearson coefficient only (not a matrix or hypothesis test). Fewer than two
// pairs, constant vectors, or nonfinite observations produce NaN. Rare scaling
// recovery can reread the input; borrowed values must remain stable.
template <class Accumulator = void, class X, class XE, class XL, class Y, class YE, class YL>
requires detail::statistic_vectors<Accumulator, X, XE, XL, Y, YE, YL>
[[nodiscard]] auto pearson_correlation(tensor_view<X, XE, XL> x,
                                       tensor_view<Y, YE, YL> y) noexcept {
    return detail::statistic_views<Accumulator, true, false>(x, y, 1);
}

template <class Accumulator = void, class X, class XE, class XL, class Y, class YE, class YL>
requires detail::statistic_vectors<Accumulator, X, XE, XL, Y, YE, YL>
[[nodiscard]] auto nanpearson_correlation(tensor_view<X, XE, XL> x,
                                          tensor_view<Y, YE, YL> y) noexcept {
    constexpr bool skip = std::is_floating_point_v<X> || std::is_floating_point_v<Y>;
    return detail::statistic_views<Accumulator, true, skip>(x, y, 1);
}

} // namespace wjr::np

#endif // WJR_NP_STATISTICS_HPP__
