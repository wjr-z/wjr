#include <wjr/np/expression_math.hpp>
#include <wjr/np/expression_reduce.hpp>
#include <wjr/np/expression_xsimd.hpp>

namespace probe {
namespace np = wjr::np;
template <class T>
using vector = np::tensor_view<T, np::dextents<size_t, 1>>;
template <class T>
using matrix = np::tensor_view<T, np::extents<size_t, np::dynamic_extent, 300>>;
template <class T>
using row = np::tensor_view<T, np::extents<size_t, 300>>;

template <class T, class Function>
WJR_INTRINSIC_INLINE void vector_expression(T *out, const T *left, const T *right, size_t count,
                                            Function function) {
    const auto a = np::as_expr(vector<const T>(left, count));
    const auto b = np::as_expr(vector<const T>(right, count));
    np::eval_to(vector<T>(out, count), function(a, b), np::xsimd_policy<>{});
}

template <class T>
WJR_INTRINSIC_INLINE void arithmetic(T *out, const T *left, const T *right, size_t count) {
    vector_expression(out, left, right, count,
                      [](auto a, auto b) { return (+a + b) * (a - b) / (b + T(2)); });
}
template <class T>
WJR_INTRINSIC_INLINE void negative(T *out, const T *left, const T *right, size_t count) {
    vector_expression(out, left, right, count, [](auto a, auto) { return -a; });
}

#define PROBE_COMPARISON(Name, Operator)                                                           \
    template <class T>                                                                             \
    WJR_INTRINSIC_INLINE void Name(T *out, const T *left, const T *right, size_t count) {          \
        vector_expression(out, left, right, count,                                                 \
                          [](auto a, auto b) { return np::select(a Operator b, a, b); });          \
    }
PROBE_COMPARISON(less, <)
PROBE_COMPARISON(less_equal, <=)
PROBE_COMPARISON(greater, >)
PROBE_COMPARISON(greater_equal, >=)
PROBE_COMPARISON(equal, ==)
PROBE_COMPARISON(not_equal, !=)
#undef PROBE_COMPARISON

template <class T>
WJR_INTRINSIC_INLINE void logic(T *out, const T *left, const T *right, size_t count) {
    vector_expression(out, left, right, count, [](auto a, auto b) {
        const auto mask =
            np::logical_or(np::logical_and(a < b, np::logical_not(a == T(0))), b > T(1));
        return np::select(mask, a, b);
    });
}
template <class T>
WJR_INTRINSIC_INLINE void predicate_select(T *out, const T *left, const T *right, size_t count) {
    vector_expression(out, left, right, count, [](auto a, auto b) {
        const auto mask = np::select(a < b, a <= T(0), b >= T(0));
        return np::select(mask, a, b);
    });
}
template <class T>
WJR_INTRINSIC_INLINE void minimum(T *out, const T *left, const T *right, size_t count) {
    vector_expression(out, left, right, count, [](auto a, auto b) { return np::minimum(a, b); });
}
template <class T>
WJR_INTRINSIC_INLINE void maximum(T *out, const T *left, const T *right, size_t count) {
    vector_expression(out, left, right, count, [](auto a, auto b) { return np::maximum(a, b); });
}
template <class T>
WJR_INTRINSIC_INLINE void clip(T *out, const T *left, const T *right, size_t count) {
    vector_expression(out, left, right, count, [](auto a, auto b) { return np::clip(a, b, T(2)); });
}
template <class T>
WJR_INTRINSIC_INLINE void broadcast_row(T *out, const T *left, const T *right, size_t rows) {
    const auto a = np::as_expr(matrix<const T>(left, rows));
    const auto b = np::as_expr(row<const T>(right));
    np::eval_to(matrix<T>(out, rows), np::select(a >= b, np::clip(a + b, T(-1), T(1)), -a),
                np::xsimd_policy<>{});
}
template <class T>
WJR_INTRINSIC_INLINE void scalar_sum(T *out, const T *left, const T *right, size_t count) {
    const auto a = np::as_expr(vector<const T>(left, count));
    const auto b = np::as_expr(vector<const T>(right, count));
    *out = np::sum(np::minimum(+a * a, b) - -b / T(2));
}
template <class T>
WJR_INTRINSIC_INLINE void column_sum(T *out, const T *left, const T *right, size_t rows) {
    const auto a = np::as_expr(matrix<const T>(left, rows));
    const auto b = np::as_expr(row<const T>(right));
    const auto expression = np::maximum(+a * a, b) + -b / T(2) - T(1);
    static_assert(np::detail::interleavable_sum_expression<decltype(expression)>);
    np::sum_to(row<T>(out), expression, np::axes<0>);
}
template <class T>
WJR_INTRINSIC_INLINE void stride_fallback(T *out, const T *left, const T *, size_t count) {
    using shape = np::dextents<size_t, 1>;
    const auto mapping = np::layout_stride::mapping<shape>(shape(count), std::array<size_t, 1>{2});
    const auto a = np::as_expr(np::tensor_view<const T, shape, np::layout_stride>(left, mapping));
    const auto expression = np::minimum(a, T(0));
    static_assert(!np::detail::homogeneous_xsimd_expression<decltype(expression), T>);
    np::eval_to(vector<T>(out, count), expression, np::xsimd_policy<>{});
}
template <class T>
WJR_INTRINSIC_INLINE void map_fallback(T *out, const T *left, const T *right, size_t count) {
    const auto a = np::as_expr(vector<const T>(left, count));
    const auto b = np::as_expr(vector<const T>(right, count));
    const auto expression = np::map([](T value) noexcept { return value * value; }, a) + b;
    static_assert(!np::detail::homogeneous_xsimd_expression<decltype(expression), T>);
    np::eval_to(vector<T>(out, count), expression, np::xsimd_policy<>{});
}
template <class T>
WJR_INTRINSIC_INLINE void mixed_fallback(T *out, const T *left, const T *right, size_t count) {
    const auto a = np::as_expr(vector<const T>(left, count));
    const auto b = np::as_expr(vector<const T>(right, count));
    const auto expression = np::minimum(a, 1) + b;
    static_assert(!np::detail::homogeneous_xsimd_expression<decltype(expression), T>);
    np::eval_to(vector<T>(out, count), expression, np::xsimd_policy<>{});
}
} // namespace probe

#define EXPORT(Name, Type, Suffix)                                                                 \
    extern "C" void Name##Suffix(Type *out, const Type *left, const Type *right, size_t count) {   \
        probe::Name<Type>(out, left, right, count);                                                \
    }
#define EXPORT_TYPE(Type, Suffix)                                                                  \
    EXPORT(arithmetic, Type, Suffix)                                                               \
    EXPORT(negative, Type, Suffix)                                                                 \
    EXPORT(less, Type, Suffix)                                                                     \
    EXPORT(less_equal, Type, Suffix)                                                               \
    EXPORT(greater, Type, Suffix)                                                                  \
    EXPORT(greater_equal, Type, Suffix)                                                            \
    EXPORT(equal, Type, Suffix)                                                                    \
    EXPORT(not_equal, Type, Suffix)                                                                \
    EXPORT(logic, Type, Suffix)                                                                    \
    EXPORT(predicate_select, Type, Suffix)                                                         \
    EXPORT(minimum, Type, Suffix)                                                                  \
    EXPORT(maximum, Type, Suffix)                                                                  \
    EXPORT(clip, Type, Suffix)                                                                     \
    EXPORT(broadcast_row, Type, Suffix)                                                            \
    EXPORT(scalar_sum, Type, Suffix)                                                               \
    EXPORT(column_sum, Type, Suffix)                                                               \
    EXPORT(stride_fallback, Type, Suffix)                                                          \
    EXPORT(map_fallback, Type, Suffix)                                                             \
    EXPORT(mixed_fallback, Type, Suffix)
EXPORT_TYPE(float, _f)
EXPORT_TYPE(double, _d)
#undef EXPORT_TYPE
#undef EXPORT
