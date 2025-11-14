#ifndef WJR_CONCURRENCY_RCREFERENCE_HPP__
#define WJR_CONCURRENCY_RCREFERENCE_HPP__

#include <wjr/type_traits.hpp>

#include <functional>
#include <utility>

namespace wjr {

template <typename T>
class RCReference {
    static constexpr bool is_noexcept_reset =
        noexcept(std::declval<T *>()->add_ref()) && noexcept(std::declval<T *>()->drop_ref());

public:
    using element_type = T;
    using pointer = T *;

    RCReference() = default;
    RCReference(const RCReference &other) noexcept(is_noexcept_reset) {
        reset(other.m_handle, /*add_ref=*/true);
    }
    RCReference(RCReference &&other) noexcept { *this = std::move(other); }

    RCReference(T *h, bool add_ref) noexcept(is_noexcept_reset) { reset(h, add_ref); }

    RCReference &operator=(const RCReference &o) noexcept(is_noexcept_reset) {
        reset(o.m_handle, /*add_ref=*/true);
        return *this;
    }

    RCReference &operator=(RCReference &&o) noexcept {
        if (m_handle != o.m_handle) {
            reset(o.detach(), /*add_ref=*/false);
        }
        return *this;
    }

    ~RCReference() noexcept(is_noexcept_reset) {
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
bool operator==(const RCReference<T> &lhs, const RCReference<T> &rhs) noexcept {
    return lhs.get() == rhs.get();
}

template <typename T>
bool operator!=(const RCReference<T> &lhs, const RCReference<T> &rhs) noexcept {
    return !(lhs == rhs);
}

} // namespace wjr

#endif // WJR_CONCURRENCY_RCREFERENCE_HPP__