#ifndef WJR_NP_TENSOR_VECTOR_HPP__
#define WJR_NP_TENSOR_VECTOR_HPP__

#include <wjr/container/vector.hpp>

namespace wjr::np::detail {

// Tensor owners allow querying and copying their moved-from representation.
// Keep that stronger contract local to their storage, with no additional state.
template <class T, class Allocator>
class tensor_vector_storage : public default_vector_storage<T, Allocator> {
    using base_type = default_vector_storage<T, Allocator>;
    using rebound_allocator = typename std::allocator_traits<Allocator>::template rebind_alloc<T>;

public:
    constexpr void take_storage(tensor_vector_storage &other,
                                rebound_allocator &allocator) noexcept {
        base_type::take_storage(other, allocator);
        other.m_storage.m_end = nullptr;
        other.m_storage.m_buffer = nullptr;
    }
};

} // namespace wjr::np::detail

namespace wjr {

template <class T, class Allocator>
struct get_relocate_mode<np::detail::tensor_vector_storage<T, Allocator>>
    : get_relocate_mode<default_vector_storage<T, Allocator>> {};

} // namespace wjr

namespace wjr::np {

// Reuses WJR vector while keeping moved-from storage valid for owner observers.
template <class T, class Allocator = std::allocator<T>>
using tensor_vector = wjr::basic_vector<detail::tensor_vector_storage<T, Allocator>>;

} // namespace wjr::np

#endif // WJR_NP_TENSOR_VECTOR_HPP__
