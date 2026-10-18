/*
 * Scaling constants and bucket algorithm adapted from Reference BLAS xNRM2,
 * LAPACK v3.12.1. Original license:
 *
 * Copyright (c) 1992-2023 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2000-2023 The University of California Berkeley. All
 *                         rights reserved.
 * Copyright (c) 2006-2023 The University of Colorado Denver.  All rights
 *                         reserved.
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer listed
 *   in this license in the documentation and/or other materials
 *   provided with the distribution.
 *
 * - Neither the name of the copyright holders nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * The copyright holders provide no reassurances that the source code
 * provided does not infringe any patent, copyright, or any other
 * intellectual property rights of third parties.  The copyright holders
 * disclaim any liability to any recipient for claims brought against
 * recipient by any third party for infringement of that parties
 * intellectual property rights.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WJR_NP_NORM_HPP__
#define WJR_NP_NORM_HPP__

#include <cmath>
#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>

#include <wjr/config/attribute.hpp>
#include <wjr/np/detail/layout.hpp>

namespace wjr::np {
namespace detail {

template <class Accumulator, class T>
using norm_accumulator =
    std::conditional_t<std::is_void_v<Accumulator>,
                       std::conditional_t<std::is_floating_point_v<T>, std::remove_cv_t<T>, double>,
                       Accumulator>;

template <class T>
constexpr T norm_power_of_two(int exponent) noexcept {
    T value = 1;
    for (; exponent > 0; --exponent)
        value *= T{2};
    for (; exponent < 0; ++exponent)
        value *= T{0.5};
    return value;
}

constexpr int norm_floor_half(int value) noexcept {
    return value / 2 - (value < 0 && value % 2 != 0);
}

constexpr int norm_ceil_half(int value) noexcept {
    return value / 2 + (value > 0 && value % 2 != 0);
}

// Rare fallback when a fixed-scale bucket overflows, including input infinity.
// The scale/sumsq representation avoids squaring the original magnitude.
template <class Acc, class T>
WJR_NOINLINE WJR_COLD Acc scaled_norm(const T *input, size_t count, size_t step) noexcept {
    Acc scale = 0;
    Acc sumsq = 1;
    bool infinite = false;
    for (size_t i = 0; i != count; ++i) {
        const auto value = std::abs(static_cast<Acc>(input[i * step]));
        if (value > std::numeric_limits<Acc>::max()) {
            infinite = true;
        } else if (value != value) {
            return value;
        } else if (value != 0) {
            if (scale < value) {
                const auto ratio = scale / value;
                sumsq = 1 + (sumsq * ratio) * ratio;
                scale = value;
            } else {
                const auto ratio = value / scale;
                sumsq += ratio * ratio;
            }
        }
    }
    return infinite ? std::numeric_limits<Acc>::infinity() : scale * std::sqrt(sumsq);
}

// Blue's three fixed scaling buckets, as used by Reference BLAS xNRM2.
// Powers of two are exact and determined entirely by the accumulator type.
template <class Acc, class T>
Acc blue_norm(const T *input, size_t count, size_t step) noexcept {
    using limits = std::numeric_limits<Acc>;
    constexpr auto small_threshold =
        norm_power_of_two<Acc>(norm_ceil_half(limits::min_exponent - 1));
    constexpr auto big_threshold =
        norm_power_of_two<Acc>(norm_floor_half(limits::max_exponent - limits::digits + 1));
    constexpr auto scale_small =
        norm_power_of_two<Acc>(-norm_floor_half(limits::min_exponent - limits::digits));
    constexpr auto scale_big =
        norm_power_of_two<Acc>(-norm_ceil_half(limits::max_exponent + limits::digits - 1));
    Acc small = 0;
    Acc medium = 0;
    Acc big = 0;
    bool no_big = true;
    for (size_t i = 0; i != count; ++i) {
        const auto value = std::abs(static_cast<Acc>(input[i * step]));
        if (value > big_threshold) {
            const auto scaled = value * scale_big;
            big += scaled * scaled;
            no_big = false;
        } else if (value < small_threshold) {
            if (no_big) {
                const auto scaled = value * scale_small;
                small += scaled * scaled;
            }
        } else {
            medium += value * value;
        }
    }
    // NaNs enter the medium bucket. They take precedence over infinity.
    if (medium != medium)
        return medium;
    if (big > limits::max() || medium > limits::max() || small > limits::max())
        return scaled_norm<Acc>(input, count, step);
    if (big > 0) {
        big += (medium * scale_big) * scale_big;
        if (big > limits::max())
            return scaled_norm<Acc>(input, count, step);
        return std::sqrt(big) / scale_big;
    }
    if (small > 0) {
        if (medium > 0) {
            const auto a = std::sqrt(medium);
            const auto b = std::sqrt(small) / scale_small;
            const auto high = a > b ? a : b;
            const auto low = a > b ? b : a;
            const auto ratio = low / high;
            return high * std::sqrt(1 + ratio * ratio);
        }
        return std::sqrt(small) / scale_small;
    }
    return std::sqrt(medium);
}

} // namespace detail

// Stable Euclidean norm of a valid borrowed view. Packed layouts use physical
// storage order; rank-one strided views use increasing logical index order.
// Finite input conversions must be representable in the selected accumulator.
template <class Accumulator = void, class T, class Extents, class Layout>
requires(std::is_arithmetic_v<T> && !std::is_volatile_v<T> &&
         (detail::packed_layout<Layout> ||
          (Extents::rank() <= 1 && std::same_as<Layout, layout_stride>)) &&
         (std::is_void_v<Accumulator> ||
          (std::floating_point<Accumulator> &&
           std::same_as<Accumulator, std::remove_cv_t<Accumulator>>)) &&
         std::numeric_limits<detail::norm_accumulator<Accumulator, T>>::radix == 2)
[[nodiscard]] auto norm_l2(tensor_view<T, Extents, Layout> source) noexcept {
    using accumulator_type = detail::norm_accumulator<Accumulator, T>;
    const auto count = static_cast<size_t>(source.size());
    if (count == 0)
        return accumulator_type{0};
    if constexpr (detail::packed_layout<Layout> || Extents::rank() == 0) {
        return detail::blue_norm<accumulator_type>(source.data_handle(), count, 1);
    } else {
        return detail::blue_norm<accumulator_type>(source.data_handle(), count,
                                                   static_cast<size_t>(source.stride(0)));
    }
}

} // namespace wjr::np

#endif // WJR_NP_NORM_HPP__
