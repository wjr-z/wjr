#include <wjr/np/pairwise.hpp>
#ifdef PAIRWISE_EXPRESSIONS
    #include <wjr/np/expression_pairwise.hpp>
#endif

namespace np = wjr::np;
template <class T>
using line_view = np::tensor_view<T, np::dextents<size_t, 1>>;

extern "C" float view_f(const float *p, size_t n) {
    return np::pairwise_sum(line_view<const float>(p, n));
}
extern "C" double view_d(const double *p, size_t n) {
    return np::pairwise_sum(line_view<const double>(p, n));
}
extern "C" double view_wide(const float *p, size_t n) {
    return np::pairwise_sum<double>(line_view<const float>(p, n));
}
extern "C" double view_int(const int *p, size_t n) {
    return np::pairwise_sum<double>(line_view<const int>(p, n));
}
extern "C" float view_mutable_f(float *p, size_t n) {
    return np::pairwise_sum(line_view<float>(p, n));
}
extern "C" double view_mutable_d(double *p, size_t n) {
    return np::pairwise_sum(line_view<double>(p, n));
}
template <class T>
WJR_INTRINSIC_INLINE T strided(const T *p, size_t n, size_t step) {
    using shape = np::dextents<size_t, 1>;
    const auto mapping = np::layout_stride::mapping<shape>(shape(n), std::array<size_t, 1>{step});
    return np::pairwise_sum(np::tensor_view<const T, shape, np::layout_stride>(p, mapping));
}
extern "C" float view_stride_f(const float *p, size_t n, size_t step) {
    return strided(p, n, step);
}
extern "C" double view_stride_d(const double *p, size_t n, size_t step) {
    return strided(p, n, step);
}
extern "C" float view_static6(const float *p) {
    return np::pairwise_sum(np::tensor_view<const float, np::extents<size_t, 6>>(p));
}
extern "C" double view_static257(const double *p) {
    return np::pairwise_sum(np::tensor_view<const double, np::extents<size_t, 257>>(p));
}

#ifdef PAIRWISE_EXPRESSIONS
template <class T, int Kind>
WJR_INTRINSIC_INLINE T hand_value(const T *x, const T *y, size_t i, T alpha, T beta) {
    if constexpr (Kind == 0)
        return x[i] * x[i];
    else if constexpr (Kind == 1) {
        const T a = x[i] * alpha;
        const T b = y[i] * beta;
        return a + b;
    } else
        return x[i] * alpha;
}
// Competent direct fused loop: raw inputs and scalar snapshots, same grouping.
template <class Acc, class T, int Kind>
WJR_INTRINSIC_INLINE Acc hand_leaf(const T *x, const T *y, size_t n, T alpha, T beta) {
    auto read = [&](size_t i) {
        return static_cast<Acc>(hand_value<T, Kind>(x, y, i, alpha, beta));
    };
    if (n < 8) {
        Acc result = -Acc{0};
        for (size_t i = 0; i != n; ++i)
            result += read(i);
        return result;
    }
    Acc a = read(0), b = read(1), c = read(2), d = read(3), e = read(4), f = read(5), g = read(6),
        h = read(7);
    const auto end = n - n % 8;
    size_t i = 8;
    for (; i != end; i += 8) {
        a += read(i);
        b += read(i + 1);
        c += read(i + 2);
        d += read(i + 3);
        e += read(i + 4);
        f += read(i + 5);
        g += read(i + 6);
        h += read(i + 7);
    }
    Acc result = ((a + b) + (c + d)) + ((e + f) + (g + h));
    for (; i != n; ++i)
        result += read(i);
    return result;
}
template <class Acc, class T, int Kind>
WJR_NOINLINE Acc hand_tree(const T *x, const T *y, size_t n, T alpha, T beta) {
    if (n <= 128)
        return hand_leaf<Acc, T, Kind>(x, y, n, alpha, beta);
    const auto half = n / 2 - n / 2 % 8;
    const auto left = hand_tree<Acc, T, Kind>(x, y, half, alpha, beta);
    const T *next_y = y;
    if constexpr (Kind == 1)
        next_y += half;
    const auto right = hand_tree<Acc, T, Kind>(x + half, next_y, n - half, alpha, beta);
    return left + right;
}
template <class Acc, class T, int Kind>
WJR_INTRINSIC_INLINE Acc hand_kernel(const T *x, const T *y, size_t n, T alpha, T beta) {
    if (!n)
        return Acc{0};
    if (n <= 128)
        return hand_leaf<Acc, T, Kind>(x, y, n, alpha, beta);
    return hand_tree<Acc, T, Kind>(x, y, n, alpha, beta);
}
template <class Acc, class T, int Kind>
WJR_INTRINSIC_INLINE Acc expression(const T *x, const T *y, size_t n, T alpha, T beta) {
    const auto a = np::as_expr(line_view<const T>(x, n));
    if constexpr (Kind == 0)
        return np::pairwise_sum<Acc>(a * a);
    else if constexpr (Kind == 1) {
        const auto b = np::as_expr(line_view<const T>(y, n));
        return np::pairwise_sum<Acc>(a * alpha + b * beta);
    } else {
        struct scale {
            T value;
            T operator()(T x) const noexcept { return x * value; }
        };
        const auto mapped = np::map(scale{alpha}, a);
        return np::pairwise_sum<Acc>(mapped);
    }
}
    #define EXPORT(Name, Acc, T, Kind)                                                             \
        extern "C" Acc expr_##Name(const T *x, const T *y, size_t n, T a, T b) {                   \
            return expression<Acc, T, Kind>(x, y, n, a, b);                                        \
        }                                                                                          \
        extern "C" Acc hand_##Name(const T *x, const T *y, size_t n, T a, T b) {                   \
            return hand_kernel<Acc, T, Kind>(x, y, n, a, b);                                       \
        }
EXPORT(square_f, float, float, 0)
EXPORT(square_d, double, double, 0)
EXPORT(affine_f, float, float, 1)
EXPORT(affine_d, double, double, 1)
EXPORT(affine_wide, double, float, 1)
EXPORT(mapped_f, float, float, 2)
    #undef EXPORT
extern "C" float expr_static6(const float *p) {
    const auto a = np::as_expr(np::tensor_view<const float, np::extents<size_t, 6>>(p));
    return np::pairwise_sum(a * a);
}
extern "C" float hand_static6(const float *p) {
    return hand_kernel<float, float, 0>(p, nullptr, 6, 0.f, 0.f);
}

    #ifdef PAIRWISE_RUNTIME
        #include <bit>
        #include <cmath>
        #include <cstdio>
        #include <cstdlib>
        #include <vector>
template <class T>
bool equal(T a, T b) {
    return (std::isnan(a) && std::isnan(b)) || (a == b && std::signbit(a) == std::signbit(b));
}
template <class Acc, class T, int Kind>
size_t check() {
    size_t checks = 0;
    for (size_t n = 0; n != 520; ++n) {
        std::vector<T> a(n), b(n);
        for (size_t i = 0; i != n; ++i) {
            a[i] = T(int(i % 17) - 8) * T(.25);
            b[i] = T(int(i % 7) - 3) * T(.5);
        }
        auto actual = expression<Acc, T, Kind>(a.data(), b.data(), n, T(.5), T(.25));
        auto expected = hand_kernel<Acc, T, Kind>(a.data(), b.data(), n, T(.5), T(.25));
        if (!equal(actual, expected))
            std::abort();
        ++checks;
    }
    for (size_t n : {1024u, 4095u, 4096u, 4097u, 8193u}) {
        std::vector<T> a(n, T(1)), b(n, T(.25));
        a.front() = T(1e10);
        a.back() = -T(1e10);
        if (!equal(expression<Acc, T, Kind>(a.data(), b.data(), n, T(.5), T(.25)),
                   hand_kernel<Acc, T, Kind>(a.data(), b.data(), n, T(.5), T(.25))))
            std::abort();
        ++checks;
    }
    return checks;
}
int main() {
    auto count = check<float, float, 0>() + check<double, double, 0>() + check<float, float, 1>() +
                 check<double, double, 1>() + check<double, float, 1>() + check<float, float, 2>();
    int values[]{3, 5, 7, -3};
    auto expression = np::as_expr(np::tensor_view<const int, np::extents<size_t, 4>>(values)) / 2;
    if (np::pairwise_sum<double>(expression) != 5.)
        std::abort();
    std::printf("PASS %zu actual fused/hand grouping pairs plus integer-dtype oracle\n", count);
}
    #endif
#endif
