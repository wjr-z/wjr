#ifndef WJR_MATH_COPY_HPP__
#define WJR_MATH_COPY_HPP__

#include <wjr/iterator/details.hpp>

#if defined(WJR_X86)
#include <wjr/x86/math/copy.hpp>
#endif

namespace wjr {

/// @private
template <typename InputIt, typename OutputIt>
struct __is_builtin_copy
    : std::conjunction<
          std::is_trivially_copyable<iterator_value_t<InputIt>>,
          std::is_same<iterator_value_t<InputIt>, iterator_value_t<OutputIt>>,
          is_contiguous_iterator<InputIt>, is_contiguous_iterator<OutputIt>> {};

/// @private
template <typename InputIt, typename OutputIt>
inline constexpr bool __is_builtin_copy_v = __is_builtin_copy<InputIt, OutputIt>::value;

template <size_t N, typename InputIt, typename OutputIt>
OutputIt fallback_copy_c(InputIt first, OutputIt dst) {
    return std::copy(first, first + N, dst);
}

template <size_t N, typename InputIt, typename OutputIt>
OutputIt copy_c(InputIt first, OutputIt dst) {
#if WJR_HAS_BUILTIN(COPY_C)
    if constexpr (__is_builtin_copy_v<InputIt, OutputIt>) {
        builtin_copy_c<N>(to_address(first), to_address(dst));
        return dst + N;
    } else {
        return fallback_copy_c<N>(first, dst);
    }

#else
    return fallback_copy_c<N>(first, dst);
#endif
}

template <size_t N, typename InputIt, typename OutputIt>
OutputIt fallback_copy_backward_c(InputIt s_last, OutputIt d_last) {
    return std::copy_backward(s_last - N, s_last, d_last);
}

template <size_t N, typename InputIt, typename OutputIt>
OutputIt copy_backward_c(InputIt s_last, OutputIt d_last) {
#if WJR_HAS_BUILTIN(COPY_BACKWARD_C)
    if constexpr (__is_builtin_copy_v<InputIt, OutputIt>) {
        builtin_copy_backward_c<N>(to_address(s_last), to_address(d_last));
        return d_last - N;
    } else {
        return fallback_copy_backward_c<N>(s_last, d_last);
    }
#else
    return fallback_copy_backward_c<N>(s_last, d_last);
#endif
}

} // namespace wjr

#endif // WJR_MATH_COPY_HPP__