#ifndef WJR_CONTAINER_CONTAINER_OF_HPP__
#define WJR_CONTAINER_CONTAINER_OF_HPP__

#include <cstddef>

namespace wjr {

template <typename Mybase, typename Tag = void, typename Enable = void>
struct container_of_fn;

template <typename Mybase, typename Tag = void, typename Enable = void>
inline container_of_fn<Mybase, Tag, Enable> container_of;

} // namespace wjr

#endif // WJR_CONTAINER_CONTAINER_OF_HPP__