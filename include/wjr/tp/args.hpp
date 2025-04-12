#ifndef WJR_TP_ARGS_HPP__
#define WJR_TP_ARGS_HPP__

#include <wjr/tuple.hpp>

namespace wjr {

/**
 * @brief Make functions can be called by `std::integral_constant'
 * @details
 * ```cpp
 * auto x = tp_make_functions(func0, func1);
 * x(std::integral_constant<size_t, 0>{}); // call func0
 * x(std::integral_constant<size_t, 1>{}); // call func1
 * ```
 */
template <typename... Args>
constexpr auto tp_make_functions(Args &&...args) {
    return [_tuple = forward_as_tuple(std::forward<Args>(args)...)](auto idx,
                                                                    auto &&..._args) mutable {
        return std::get<idx()>(_tuple)(std::forward<decltype(_args)>(_args)...);
    };
}

} // namespace wjr

#endif // WJR_TP_ARGS_HPP__