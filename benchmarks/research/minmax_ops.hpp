#ifndef WJR_RESEARCH_MINMAX_OPS_HPP
#define WJR_RESEARCH_MINMAX_OPS_HPP

#include <array>
#include <type_traits>

#include <wjr/np/expression_math.hpp>

namespace research {
namespace np = wjr::np;
enum class form { current, split, tie, bit_or, inclusive };

template <form Form, bool Minimum>
struct operation {
    template <class L, class R>
    using result_type = std::common_type_t<L, R>;

    template <class L, class R>
    static constexpr auto apply(L left, R right) noexcept -> result_type<L, R> {
        using T = result_type<L, R>;
        const T lhs = static_cast<T>(left), rhs = static_cast<T>(right);
        if constexpr (Form == form::current) {
            using Op = std::conditional_t<Minimum, np::detail::minimum_operation,
                                          np::detail::maximum_operation>;
            return Op::apply(lhs, rhs);
        } else if constexpr (Form == form::bit_or) {
            const bool keep_left = (lhs != lhs) | (Minimum ? rhs >= lhs : lhs >= rhs);
            return keep_left ? lhs : rhs;
        } else {
            if (lhs != lhs)
                return lhs;
            if constexpr (Form == form::split) {
                if (rhs != rhs)
                    return rhs;
                if constexpr (Minimum)
                    return rhs < lhs ? rhs : lhs;
                else
                    return lhs < rhs ? rhs : lhs;
            } else {
                if constexpr (Form == form::tie)
                    if (lhs == rhs)
                        return lhs;
                if constexpr (Minimum)
                    return rhs >= lhs ? lhs : rhs;
                else
                    return lhs >= rhs ? lhs : rhs;
            }
        }
    }
};

template <class T, class Op>
WJR_NOINLINE void api(T *out, const T *left, const T *right, size_t count) noexcept {
    using view = np::tensor_view<const T, np::dextents<size_t, 1>>;
    const auto expression = [&] {
        if constexpr (std::same_as<Op, np::detail::minimum_operation>)
            return np::minimum(view(left, count), view(right, count));
        else if constexpr (std::same_as<Op, np::detail::maximum_operation>)
            return np::maximum(view(left, count), view(right, count));
        else
            return np::detail::make_extremum_expression<Op>(view(left, count), view(right, count));
    }();
    np::eval_to(np::tensor_view<T, np::dextents<size_t, 1>>(out, count), expression);
}

template <class T, class Op>
WJR_NOINLINE void hand(T *out, const T *left, const T *right, size_t count) noexcept {
    for (size_t i = 0; i != count; ++i)
        out[i] = Op::apply(left[i], right[i]);
}

template <class T>
using kernel = void (*)(T *, const T *, const T *, size_t) noexcept;

template <class T>
struct variant {
    const char *name;
    kernel<T> function;
};

template <class T, bool Minimum>
inline constexpr std::array variants{
    variant<T>{"api_current", api<T, std::conditional_t<Minimum, np::detail::minimum_operation,
                                                        np::detail::maximum_operation>>},
    variant<T>{"hand_current", hand<T, std::conditional_t<Minimum, np::detail::minimum_operation,
                                                          np::detail::maximum_operation>>},
    variant<T>{"api_split", api<T, operation<form::split, Minimum>>},
    variant<T>{"hand_split", hand<T, operation<form::split, Minimum>>},
    variant<T>{"api_tie", api<T, operation<form::tie, Minimum>>},
    variant<T>{"hand_tie", hand<T, operation<form::tie, Minimum>>},
    variant<T>{"api_bit_or", api<T, operation<form::bit_or, Minimum>>},
    variant<T>{"hand_bit_or", hand<T, operation<form::bit_or, Minimum>>},
    variant<T>{"api_inclusive", api<T, operation<form::inclusive, Minimum>>},
    variant<T>{"hand_inclusive", hand<T, operation<form::inclusive, Minimum>>},
};
} // namespace research

#endif
