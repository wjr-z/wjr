#ifndef WJR_CONTAINER_GENERIC_DETAILS_HPP__
#define WJR_CONTAINER_GENERIC_DETAILS_HPP__

#include <wjr/type_traits.hpp>

namespace wjr::container_details {

WJR_REGISTER_HAS_TYPE(container_begin, std::declval<Container>().begin(), Container);
WJR_REGISTER_HAS_TYPE(container_cbegin, std::declval<Container>().cbegin(), Container);
WJR_REGISTER_HAS_TYPE(container_end, std::declval<Container>().end(), Container);
WJR_REGISTER_HAS_TYPE(container_cend, std::declval<Container>().cend(), Container);
WJR_REGISTER_HAS_TYPE(container_size, std::declval<Container>().size(), Container);

WJR_REGISTER_HAS_TYPE(container_resize,
                      std::declval<Container>().resize(std::declval<Size>(),
                                                       std::declval<Args>()...),
                      Container, Size);
WJR_REGISTER_HAS_TYPE(container_reserve,
                      std::declval<Container>().reserve(std::declval<Size>()), Container,
                      Size);
WJR_REGISTER_HAS_TYPE(container_append,
                      std::declval<Container>().append(std::declval<Args>()...),
                      Container);
WJR_REGISTER_HAS_TYPE(container_insert,
                      (std::declval<Container>().insert(
                           std::declval<Container>().cbegin(), std::declval<Args>()...),
                       std::declval<Container>().insert(std::declval<Container>().cend(),
                                                        std::declval<Args>()...)),
                      Container);

} // namespace wjr::container_details

#endif // WJR_CONTAINER_GENERIC_DETAILS_HPP__