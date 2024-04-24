#ifndef WJR_CONTAINER_INTRUSIVE_FORWARD_LIST_HPP__
#define WJR_CONTAINER_INTRUSIVE_FORWARD_LIST_HPP__

#include <wjr/container/intrusive/details.hpp>

namespace wjr {

template <typename Tag = void>
class foward_list_node {
    constexpr foward_list_node() = default;
    foward_list_node(const foward_list_node &) = delete;
    foward_list_node(foward_list_node &&) = delete;
    foward_list_node &operator=(const foward_list_node &) = delete;
    foward_list_node &operator=(foward_list_node &&) = delete;

protected:
    foward_list_node *m_next = this;
};

template <typename Mybase>
class forward_list_root {
protected:
private:
    foward_list_node *m_root;
};

} // namespace wjr

#endif // WJR_CONTAINER_INTRUSIVE_FORWARD_LIST_HPP__