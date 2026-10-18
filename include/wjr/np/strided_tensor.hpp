#ifndef WJR_NP_STRIDED_TENSOR_HPP__
#define WJR_NP_STRIDED_TENSOR_HPP__

#include <wjr/np/checked_strided_view.hpp>
#include <wjr/np/tensor.hpp>

namespace wjr::np {

namespace detail {

template <class Owner, class Integer, class StrideInteger>
concept strided_tensor_shape =
    tensor_shape<Owner, Integer> && std::same_as<typename Owner::layout_type, layout_stride> &&
    std::integral<StrideInteger> && !std::same_as<std::remove_cv_t<StrideInteger>, bool>;

template <class Owner, class Integer, class StrideInteger, class... Initialization>
constexpr shape_result<Owner>
make_allocated_strided_tensor(const std::array<Integer, Owner::rank()> &shape,
                              const std::array<StrideInteger, Owner::rank()> &strides,
                              const typename Owner::container_type::allocator_type &allocator,
                              Initialization... initialization) {
    using C = typename Owner::container_type;
    using allocator_traits = std::allocator_traits<typename C::allocator_type>;
    return with_strided_mapping<typename Owner::extents_type, typename Owner::element_type>(
        shape, strides, [&](const auto &mapping, size_t count) -> shape_result<Owner> {
            if (!wjr::in_range<typename C::size_type>(count))
                return unexpected(shape_error::size_overflow);
            if (count > C(allocator).max_size() ||
                wjr::cmp_greater(count, allocator_traits::max_size(allocator)))
                return unexpected(shape_error::container_size_limit);
            return tensor_access::make_mapping<Owner>(
                mapping, static_cast<typename C::size_type>(count), initialization..., allocator);
        });
}

} // namespace detail

// Allocate the addressed span, including padding. All stored objects are live;
// default_construct starts their lifetimes without value-initializing scalars.
template <class Owner, class Integer, class StrideInteger>
requires(detail::strided_tensor_shape<Owner, Integer, StrideInteger> &&
         std::default_initializable<typename Owner::element_type>)
[[nodiscard]] constexpr shape_result<Owner>
make_tensor(const std::array<Integer, Owner::rank()> &shape,
            const std::array<StrideInteger, Owner::rank()> &strides, std::allocator_arg_t,
            const typename Owner::container_type::allocator_type &allocator) {
    return detail::make_allocated_strided_tensor<Owner>(shape, strides, allocator);
}

template <class Owner, class Integer, class StrideInteger>
requires(detail::strided_tensor_shape<Owner, Integer, StrideInteger> &&
         detail::default_initialized_tensor_container<typename Owner::container_type> &&
         std::default_initializable<typename Owner::element_type>)
[[nodiscard]] constexpr shape_result<Owner>
make_tensor(const std::array<Integer, Owner::rank()> &shape,
            const std::array<StrideInteger, Owner::rank()> &strides, default_construct_t,
            std::allocator_arg_t, const typename Owner::container_type::allocator_type &allocator) {
    return detail::make_allocated_strided_tensor<Owner>(shape, strides, allocator,
                                                        default_construct);
}

template <class Owner, class Integer, class StrideInteger>
requires(detail::strided_tensor_shape<Owner, Integer, StrideInteger> &&
         detail::default_initialized_tensor_container<typename Owner::container_type> &&
         std::default_initializable<typename Owner::container_type::allocator_type> &&
         std::default_initializable<typename Owner::element_type>)
[[nodiscard]] constexpr shape_result<Owner>
make_tensor(const std::array<Integer, Owner::rank()> &shape,
            const std::array<StrideInteger, Owner::rank()> &strides, default_construct_t) {
    return make_tensor<Owner>(shape, strides, default_construct, std::allocator_arg,
                              typename Owner::container_type::allocator_type{});
}

template <class Owner, class Integer, class StrideInteger>
requires(detail::strided_tensor_shape<Owner, Integer, StrideInteger> &&
         std::default_initializable<typename Owner::container_type::allocator_type> &&
         std::default_initializable<typename Owner::element_type>)
[[nodiscard]] constexpr shape_result<Owner>
make_tensor(const std::array<Integer, Owner::rank()> &shape,
            const std::array<StrideInteger, Owner::rank()> &strides) {
    return make_tensor<Owner>(shape, strides, std::allocator_arg,
                              typename Owner::container_type::allocator_type{});
}

// Adoption transfers an exact-span container only after validating metadata.
template <class Owner, class Integer, class StrideInteger>
requires(detail::strided_tensor_shape<Owner, Integer, StrideInteger> &&
         std::is_move_constructible_v<typename Owner::container_type>)
[[nodiscard]] constexpr shape_result<Owner>
adopt_tensor(const std::array<Integer, Owner::rank()> &shape,
             const std::array<StrideInteger, Owner::rank()> &strides,
             typename Owner::container_type &&storage) {
    using C = typename Owner::container_type;
    return detail::with_strided_mapping<typename Owner::extents_type, typename Owner::element_type>(
        shape, strides, [&](const auto &mapping, size_t count) -> shape_result<Owner> {
            if (!wjr::in_range<typename C::size_type>(count))
                return unexpected(shape_error::size_overflow);
            if (count > storage.max_size())
                return unexpected(shape_error::container_size_limit);
            if (count != storage.size())
                return unexpected(shape_error::storage_size_mismatch);
            return detail::tensor_access::make_mapping<Owner>(mapping, std::move(storage));
        });
}

} // namespace wjr::np

#endif // WJR_NP_STRIDED_TENSOR_HPP__
