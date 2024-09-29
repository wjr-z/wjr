#ifndef WJR_CONTAINER_INTRUSIVE_DETAIL_HPP__
#define WJR_CONTAINER_INTRUSIVE_DETAIL_HPP__

#include <cstddef>

namespace wjr {

#define WJR_CONTAINER_OF(obj, container, member)                                         \
    reinterpret_cast<container *>(reinterpret_cast<char *>(obj) -                        \
                                  offsetof(container, member))

} // namespace wjr

#endif // WJR_CONTAINER_INTRUSIVE_DETAIL_HPP__