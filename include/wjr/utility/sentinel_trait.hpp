/**
 * @file utility/sentinel_trait.hpp
 * @brief Sentinel value traits for optional-like types
 * @author wjr
 *
 * Provides traits for managing sentinel values that indicate empty/invalid state.
 * Supports both boolean sentinels and compressed sentinels using unique values.
 */

#ifndef WJR_UTILITY_SENTINEL_TRAIT_HPP__
#define WJR_UTILITY_SENTINEL_TRAIT_HPP__

#include <wjr/assert.hpp>

namespace wjr {
struct sentinel_trait {
    using self_type = sentinel_trait;
    static constexpr bool is_compressed = false;
    using sentinel_type = bool;

    static constexpr void construct_sentinel(sentinel_type *has_value) noexcept {
        set_sentinel(*has_value);
    }
    static constexpr void set_sentinel(sentinel_type &has_value) noexcept { has_value = false; }
    static constexpr void unset_sentinel(sentinel_type &has_value) noexcept { has_value = true; }
    WJR_PURE static constexpr bool is_sentinel(const sentinel_type &has_value) noexcept {
        return !has_value;
    }
};

template <typename T>
struct typed_sentinel_trait : sentinel_trait {};

template <typename Mybase, typename S>
struct compressed_sentinel_trait_base {
    using self_type = Mybase;
    static constexpr bool is_compressed = true;
    using sentinel_type = S;

    static constexpr void unset_sentinel(WJR_MAYBE_UNUSED sentinel_type &val) noexcept {
        WJR_ASSERT_ASSUME_L2(!Mybase::is_sentinel(val));
    }
};

/// @private Base implementation for compressed trivial sentinel trait
namespace detail {
template <typename T, T uniq_sentinel>
struct _compressed_trivial_sentinel_trait_base
    : compressed_sentinel_trait_base<_compressed_trivial_sentinel_trait_base<T, uniq_sentinel>, T> {
    using sentinel_type = T;

    static constexpr void construct_sentinel(sentinel_type *has_value) noexcept {
        set_sentinel(*has_value);
    }
    static constexpr void set_sentinel(sentinel_type &val) noexcept { val = uniq_sentinel; }
    WJR_PURE static constexpr bool is_sentinel(const sentinel_type &val) noexcept {
        return val == uniq_sentinel;
    }
};
} // namespace detail

template <typename T, typename = void>
struct compressed_sentinel_trait : sentinel_trait {};

template <typename T>
struct compressed_sentinel_trait<T *, void>
    : detail::_compressed_trivial_sentinel_trait_base<T *, nullptr> {};

template <typename T, typename SentinelTrait = sentinel_trait>
struct sentinel_tag {};

template <typename T>
struct sentinel_tag_tarit {
    using value_type = T;
    using trait_type = sentinel_trait;
};

template <typename T, typename SentinelTrait>
struct sentinel_tag_tarit<sentinel_tag<T, SentinelTrait>> {
    using value_type = T;
    using trait_type = typename SentinelTrait::self_type;
};

template <typename T>
using sentinel_tag_value_t = typename sentinel_tag_tarit<T>::value_type;

template <typename T>
using sentinel_tag_trait_t = typename sentinel_tag_tarit<T>::trait_type;

} // namespace wjr

#endif // WJR_UTILITY_SENTINEL_TRAIT_HPP__