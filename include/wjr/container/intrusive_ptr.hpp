#ifndef WJR_CONTAINER_INTRUSIVE_PTR_HPP__
#define WJR_CONTAINER_INTRUSIVE_PTR_HPP__

#include <wjr/type_traits.hpp>

#include <functional>
#include <utility>

namespace wjr {

template <typename T>
class intrusive_ptr {
    static constexpr bool is_noexcept_reset =
        noexcept(std::declval<T *>()->add_ref()) && noexcept(std::declval<T *>()->drop_ref());

public:
    using element_type = T;
    using pointer = T *;

    intrusive_ptr() = default;
    intrusive_ptr(const intrusive_ptr &other) noexcept(is_noexcept_reset) {
        reset(other.m_handle, /*add_ref=*/true);
    }
    intrusive_ptr(intrusive_ptr &&other) noexcept { *this = std::move(other); }

    intrusive_ptr(T *h, bool add_ref) noexcept(is_noexcept_reset) { reset(h, add_ref); }

    intrusive_ptr &operator=(const intrusive_ptr &o) noexcept(is_noexcept_reset) {
        reset(o.m_handle, /*add_ref=*/true);
        return *this;
    }

    intrusive_ptr &operator=(intrusive_ptr &&o) noexcept {
        if (m_handle != o.m_handle) {
            reset(o.detach(), /*add_ref=*/false);
        }
        return *this;
    }

    ~intrusive_ptr() noexcept(is_noexcept_reset) {
        if (m_handle)
            m_handle->drop_ref();
    }

    void reset(T *h, bool add_ref) noexcept(is_noexcept_reset) {
        if (h != m_handle) {
            if (add_ref && h)
                h->add_ref();
            if (m_handle)
                m_handle->drop_ref();
            m_handle = h;
        }
    }

    T *operator->() const { return m_handle; }
    T &operator*() const { return *m_handle; }
    explicit operator bool() const noexcept { return get(); }
    T *get() const { return m_handle; }
    T *detach() {
        T *handle = m_handle;
        m_handle = nullptr;
        return handle;
    }

private:
    pointer m_handle = nullptr;
};

template <typename T>
bool operator==(const intrusive_ptr<T> &lhs, const intrusive_ptr<T> &rhs) noexcept {
    return lhs.get() == rhs.get();
}

template <typename T>
bool operator!=(const intrusive_ptr<T> &lhs, const intrusive_ptr<T> &rhs) noexcept {
    return !(lhs == rhs);
}

} // namespace wjr

#endif // WJR_CONTAINER_INTRUSIVE_PTR_HPP__