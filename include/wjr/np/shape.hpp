#ifndef WJR_NP_SHAPE_HPP__
#define WJR_NP_SHAPE_HPP__

#include <array>
#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>

#include <wjr/expected.hpp>
#include <wjr/np/view.hpp>

namespace wjr::np {

enum class shape_error {
    negative_extent,
    extent_out_of_range,
    static_extent_mismatch,
    index_overflow,
    size_overflow,
    insufficient_storage,
    null_storage,
    storage_size_mismatch,
    container_size_limit,
    element_count_mismatch,
    nonpositive_stride,
    stride_out_of_range,
    unsupported_stride_order,
    expression_shape_mismatch,
    ok, // Reserved for shape_result success; never construct unexpected(ok).
};

// The reserved success code encodes state without a separate boolean.
template <class T>
using shape_result = compressed_expected<T, shape_error, shape_error::ok>;

namespace detail {

template <class T>
inline constexpr bool is_extents = false;

template <class Index, size_t... Values>
inline constexpr bool is_extents<extents<Index, Values...>> = true;

template <class Extents>
struct shape_metadata {
    std::array<typename Extents::index_type, Extents::rank()> dimensions;
    size_t size;
};

// Shared boundary validation; no backend mapping or allocation exists yet.
template <class T, class Extents, std::integral Integer>
constexpr shape_result<shape_metadata<Extents>>
check_metadata(const std::array<Integer, Extents::rank()> &shape) noexcept {
    using index_type = typename Extents::index_type;
    std::array<index_type, Extents::rank()> dimensions{};
    bool empty = false;

    // Validate every dimension before multiplying, including dimensions after a
    // zero. Do not narrow an external integer before checking representability.
    for (size_t r = 0; r != Extents::rank(); ++r) {
        if constexpr (std::is_signed_v<Integer>) {
            if (shape[r] < 0) {
                return unexpected(shape_error::negative_extent);
            }
        }
        if (!wjr::in_range<index_type>(shape[r])) {
            return unexpected(shape_error::extent_out_of_range);
        }
        if (Extents::static_extent(r) != dynamic_extent &&
            !wjr::cmp_equal(shape[r], Extents::static_extent(r))) {
            return unexpected(shape_error::static_extent_mismatch);
        }
        dimensions[r] = static_cast<index_type>(shape[r]);
        empty |= dimensions[r] == 0;
    }

    // Even empty mappings can compute overflowing prefix/suffix strides. Every
    // zero-free run must fit index_type before invoking either backend layout.
    constexpr index_type max_index = std::numeric_limits<index_type>::max();
    index_type run = 1;
    for (index_type dimension : dimensions) {
        if (dimension == 0) {
            run = 1;
        } else {
            if (run > max_index / dimension) {
                return unexpected(shape_error::index_overflow);
            }
            run *= dimension;
        }
    }
    const index_type count = empty ? 0 : run;
    if (!wjr::in_range<size_t>(count)) {
        return unexpected(shape_error::size_overflow);
    }
    const auto elements = static_cast<size_t>(count);
    // Conservatively keep the addressed byte span within PTRDIFF_MAX, which
    // also makes its multiplication by sizeof(T) representable in size_t.
    constexpr auto max_bytes = static_cast<size_t>(std::numeric_limits<ptrdiff_t>::max());
    if (elements > max_bytes / sizeof(T)) {
        return unexpected(shape_error::size_overflow);
    }
    return shape_metadata<Extents>{dimensions, elements};
}

} // namespace detail

} // namespace wjr::np

#endif // WJR_NP_SHAPE_HPP__
