#ifndef WJR_EXAMPLES_EXPRESSION_DESIGN_HPP__
#define WJR_EXAMPLES_EXPRESSION_DESIGN_HPP__

#include <cstddef>
#include <span>
#include <type_traits>
#include <utility>

#include <wjr/compressed_pair.hpp>

#if defined(WJR_EXPR_XSIMD)
    #include <wjr/simd/xsimd.hpp>
#endif

namespace expression_experiment {

template <class E>
concept expression = requires { typename std::remove_cvref_t<E>::expression_tag; };

template <std::size_t Extent = std::dynamic_extent>
struct leaf {
    using expression_tag = void;
    std::span<const double, Extent> values;

    WJR_INTRINSIC_INLINE double value_at(std::size_t i) const noexcept { return values.data()[i]; }
#if defined(WJR_EXPR_XSIMD)
    template <class Batch>
    WJR_INTRINSIC_INLINE Batch read_batch(std::size_t i) const noexcept {
        return Batch::load_unaligned(values.data() + i);
    }
#endif
    constexpr leaf bind_row(std::size_t) const noexcept { return *this; }
};

template <class T, std::size_t Extent>
requires std::is_same_v<std::remove_const_t<T>, double>
constexpr auto as_expr(std::span<T, Extent> values) noexcept {
    return leaf<Extent>{std::span<const double, Extent>(values)};
}

struct scalar {
    using expression_tag = void;
    double value;

    constexpr double value_at(std::size_t) const noexcept { return value; }
#if defined(WJR_EXPR_XSIMD)
    template <class Batch>
    WJR_INTRINSIC_INLINE Batch read_batch(std::size_t) const noexcept {
        return Batch(value);
    }
#endif
    constexpr scalar bind_row(std::size_t) const noexcept { return *this; }
};

struct matrix_leaf {
    using expression_tag = void;
    const double *data;
    std::size_t columns;

    constexpr leaf<> bind_row(std::size_t row) const noexcept {
        return {std::span<const double>(data + row * columns, columns)};
    }
};

struct add_op {
    template <class V>
    static WJR_INTRINSIC_INLINE auto apply(const V &left, const V &right) noexcept {
        return left + right;
    }
};
struct subtract_op {
    template <class V>
    static WJR_INTRINSIC_INLINE auto apply(const V &left, const V &right) noexcept {
        return left - right;
    }
};
struct multiply_op {
    template <class V>
    static WJR_INTRINSIC_INLINE auto apply(const V &left, const V &right) noexcept {
        return left * right;
    }
};

template <class Op, expression Left, expression Right>
struct binary {
    using expression_tag = void;
    wjr::compressed_pair<Left, Right> children;

    WJR_INTRINSIC_INLINE double value_at(std::size_t i) const noexcept {
        return Op::apply(children.first().value_at(i), children.second().value_at(i));
    }
#if defined(WJR_EXPR_XSIMD)
    template <class Batch>
    WJR_INTRINSIC_INLINE Batch read_batch(std::size_t i) const noexcept {
        return Op::apply(children.first().template read_batch<Batch>(i),
                         children.second().template read_batch<Batch>(i));
    }
#endif
    WJR_INTRINSIC_INLINE auto bind_row(std::size_t row) const noexcept {
        using result_type = binary<Op, decltype(children.first().bind_row(row)),
                                   decltype(children.second().bind_row(row))>;
        return result_type{{children.first().bind_row(row), children.second().bind_row(row)}};
    }
};

template <class E>
constexpr auto capture(E &&value) noexcept {
    if constexpr (expression<E>) {
        return std::forward<E>(value);
    } else {
        return scalar{static_cast<double>(value)};
    }
}

template <class Op, class L, class R>
constexpr auto make_binary(L &&left, R &&right) noexcept {
    using result_type = binary<Op, decltype(capture(std::forward<L>(left))),
                               decltype(capture(std::forward<R>(right)))>;
    return result_type{{capture(std::forward<L>(left)), capture(std::forward<R>(right))}};
}

template <class L, class R>
requires((expression<L> || expression<R>) &&
         (expression<L> || std::is_arithmetic_v<std::remove_cvref_t<L>>) &&
         (expression<R> || std::is_arithmetic_v<std::remove_cvref_t<R>>))
constexpr auto operator+(L &&left, R &&right) noexcept {
    return make_binary<add_op>(std::forward<L>(left), std::forward<R>(right));
}

template <class L, class R>
requires((expression<L> || expression<R>) &&
         (expression<L> || std::is_arithmetic_v<std::remove_cvref_t<L>>) &&
         (expression<R> || std::is_arithmetic_v<std::remove_cvref_t<R>>))
constexpr auto operator-(L &&left, R &&right) noexcept {
    return make_binary<subtract_op>(std::forward<L>(left), std::forward<R>(right));
}

template <class L, class R>
requires((expression<L> || expression<R>) &&
         (expression<L> || std::is_arithmetic_v<std::remove_cvref_t<L>>) &&
         (expression<R> || std::is_arithmetic_v<std::remove_cvref_t<R>>))
constexpr auto operator*(L &&left, R &&right) noexcept {
    return make_binary<multiply_op>(std::forward<L>(left), std::forward<R>(right));
}

// The experiment starts with an already valid, bound iteration domain.
// Output is disjoint from the inputs, or has exactly their pointwise mapping.
template <expression E>
WJR_INTRINSIC_INLINE void evaluate(double *dst, std::size_t count, const E &expr) noexcept {
    for (std::size_t i = 0; i != count; ++i) {
        dst[i] = expr.value_at(i);
    }
}

template <std::size_t Extent, expression E>
WJR_INTRINSIC_INLINE void eval_to(std::span<double, Extent> dst, const E &expr) noexcept {
    evaluate(dst.data(), dst.size(), expr);
}

template <expression E>
WJR_INTRINSIC_INLINE void evaluate_rows(double *dst, std::size_t rows, std::size_t columns,
                                        const E &expr) noexcept {
    // A nonzero row count with zero columns must not offset a null handle.
    if (columns == 0)
        return;
    for (std::size_t row = 0; row != rows; ++row) {
        const auto bound_row = expr.bind_row(row);
        evaluate(dst + row * columns, columns, bound_row);
    }
}

#if defined(WJR_EXPR_XSIMD)
template <expression E>
WJR_INTRINSIC_INLINE void evaluate_simd(double *dst, std::size_t count, const E &expr) noexcept {
    using batch = xsimd::batch<double>;
    std::size_t i = 0;
    for (; count - i >= batch::size; i += batch::size) {
        expr.template read_batch<batch>(i).store_unaligned(dst + i);
    }
    for (; i != count; ++i)
        dst[i] = expr.value_at(i);
}
#endif

constexpr auto returned_expression(const double *x, const double *y, std::size_t n) noexcept {
    const leaf<> a{std::span<const double>(x, n)};
    const leaf<> b{std::span<const double>(y, n)};
    const auto local = a + b;
    return local * local; // Value closure: no reference to a, b or local escapes.
}

} // namespace expression_experiment

#endif // WJR_EXAMPLES_EXPRESSION_DESIGN_HPP__
