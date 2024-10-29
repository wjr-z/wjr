#ifndef WJR_CONTAINER_INTRUSIVE_DETAIL_HPP__
#define WJR_CONTAINER_INTRUSIVE_DETAIL_HPP__

#include <cstddef>

#define WJR_CONTAINER_OF(obj, container, member)                                                   \
    reinterpret_cast<container *>(reinterpret_cast<char *>(obj) - offsetof(container, member))

#endif // WJR_CONTAINER_INTRUSIVE_DETAIL_HPP__