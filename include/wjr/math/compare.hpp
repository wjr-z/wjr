#ifndef WJR_MATH_CMP_HPP__
#define WJR_MATH_CMP_HPP__

#include <wjr/math/find.hpp>

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR int compare_n(const T *src0, const T *src1, size_t n) {
    if constexpr (sizeof(T) == 1) {
        return ::memcmp(src0, src1, n);
    } else {
        return find_p(src0, src1, n,
                      [src0_copy = src0, src1_copy = src1](auto &&...args) -> int {
                          if constexpr (sizeof...(args) == 0) {
                              return 0;
                          } else {
                              return [=](auto idx) {
                                  return src0_copy[idx] < src1_copy[idx] ? -1 : 1;
                              }(std::forward<decltype(args)>(args)...);
                          }
                          WJR_UNREACHABLE;
                      });
    }
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR int reverse_compare_n(const T *src0, const T *src1, size_t n) {
    return reverse_find_p(
        src0, src1, n,
        [src0_copy = src0, src1_copy = src1, n_copy = n](auto &&...args) -> int {
            if constexpr (sizeof...(args) == 0) {
                return 0;
            } else {
                return [=](auto idx) {
                    return src0_copy[n_copy - idx - 1] < src1_copy[n_copy - idx - 1] ? -1
                                                                                     : 1;
                }(std::forward<decltype(args)>(args)...);
            }
            WJR_UNREACHABLE;
        });
}

} // namespace wjr

#endif // WJR_MATH_CMP_HPP__