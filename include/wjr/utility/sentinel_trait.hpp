#ifndef WJR_UTILITY_SENTINEL_TRAIT_HPP__
#define WJR_UTILITY_SENTINEL_TRAIT_HPP__

#include <wjr/assert.hpp>

namespace wjr {
struct sentinel_trait : std::false_type {
    using sentinel_type = bool;

    static constexpr void set_sentinel(sentinel_type &has_value) noexcept { has_value = false; }
    static constexpr void unset_sentinel(sentinel_type &has_value) noexcept { has_value = true; }
    static constexpr bool is_sentinel(const sentinel_type &has_value) noexcept {
        return !has_value;
    }
};

template <typename T>
struct typed_sentinel_trait : sentinel_trait {};

template <typename Mybase, typename S>
struct compressed_sentinel_trait_base : std::true_type {
    using sentinel_type = S;

    static constexpr void unset_sentinel(WJR_MAYBE_UNUSED sentinel_type &val) noexcept {
        WJR_ASSERT_ASSUME_L2(!Mybase::is_sentinel(val));
    }
};

namespace detail {
template <typename T, T uniq_sentinel>
struct __compressed_trivial_sentinel_trait_base
    : compressed_sentinel_trait_base<__compressed_trivial_sentinel_trait_base<T, uniq_sentinel>,
                                     T> {
    using sentinel_type = T;

    static constexpr void set_sentinel(sentinel_type &val) noexcept { val = uniq_sentinel; }
    static constexpr bool is_sentinel(const sentinel_type &val) noexcept {
        return val == uniq_sentinel;
    }
};
} // namespace detail

template <typename T, typename = void>
struct compressed_sentinel_trait : sentinel_trait {};

template <typename T>
struct compressed_sentinel_trait<T *, void>
    : detail::__compressed_trivial_sentinel_trait_base<T *, nullptr> {};

} // namespace wjr

#endif // WJR_UTILITY_SENTINEL_TRAIT_HPP__