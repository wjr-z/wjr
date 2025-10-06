/**
 * @file container/detail/storage_traits.hpp
 * @brief Storage type traits for containers
 * @author wjr
 *
 * Provides type traits extraction from container storage types.
 */

#ifndef WJR_CONTAINER_DETAIL_STORAGE_TRAITS_HPP__
#define WJR_CONTAINER_DETAIL_STORAGE_TRAITS_HPP__

namespace wjr {
/**
 * @brief Extract type traits from storage
 * @tparam Storage Container storage type
 */
template <typename Storage>
struct storage_traits {
    using value_type = typename Storage::value_type;
    using difference_type = typename Storage::difference_type;
    using pointer = typename Storage::pointer;
    using const_pointer = typename Storage::const_pointer;
    using reference = value_type &;
    using const_reference = const value_type &;
};
} // namespace wjr

#endif // WJR_CONTAINER_DETAIL_STORAGE_TRAITS_HPP__