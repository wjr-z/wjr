#ifndef WJR_ITERATOR_CONTIGUOUS_ITERATOR_ADAPTER_HPP__
#define WJR_ITERATOR_CONTIGUOUS_ITERATOR_ADAPTER_HPP__

#include <wjr/assert.hpp>
#include <wjr/memory/detail.hpp>

namespace wjr {

template <typename Container, typename Traits>
class contiguous_const_iterator_adapter {
    using __pointer = typename Traits::pointer;

    template <typename Ptr>
    friend struct std::pointer_traits;

public:
#if defined(WJR_CPP_20)
    using iterator_concept = std::contiguous_iterator_tag;
#endif
    using iterator_category = std::random_access_iterator_tag;
    using value_type = typename Traits::value_type;
    using difference_type = typename Traits::difference_type;
    using pointer = typename Traits::const_pointer;
    using reference = typename Traits::const_reference;

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(contiguous_const_iterator_adapter);

    WJR_CONSTEXPR20
    contiguous_const_iterator_adapter(__pointer ptr, const Container *container) noexcept(
        std::is_nothrow_copy_constructible_v<__pointer>)
        : m_ptr(ptr) {
        __set_container(container);
    }

    WJR_NODISCARD WJR_CONSTEXPR20 pointer operator->() const noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_CHECK(m_container != nullptr, "Can't dereference an value-initialized iterator.");
        WJR_CHECK(m_ptr != nullptr, "Can't dereference an invalid iterator.");
        WJR_CHECK(m_ptr >= __begin() && m_ptr < __end(),
                  "Can't dereference an out-of-range iterator.");
#endif
        return const_cast<pointer>(m_ptr);
    }

    WJR_NODISCARD WJR_CONSTEXPR20 reference operator*() const noexcept { return *operator->(); }

    WJR_CONSTEXPR20 contiguous_const_iterator_adapter &operator++() noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_CHECK(m_container != nullptr, "Can't increment an value-initialized iterator.");
        WJR_CHECK(m_ptr != nullptr, "Can't increment an invalid iterator.");
        WJR_CHECK(m_ptr < __end(), "Can't increment an iterator that is already at/after the end.");
#endif
        ++m_ptr;
        return *this;
    }

    WJR_CONSTEXPR20 contiguous_const_iterator_adapter operator++(int) noexcept {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    WJR_CONSTEXPR20 contiguous_const_iterator_adapter &operator--() noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_CHECK(m_container != nullptr, "Can't decrement an value-initialized iterator.");
        WJR_CHECK(m_ptr != nullptr, "Can't decrement an invalid iterator.");
        WJR_CHECK(m_ptr > __begin(),
                  "Can't decrement an iterator that is already at/before the beginning.");
#endif
        --m_ptr;
        return *this;
    }

    WJR_CONSTEXPR20 contiguous_const_iterator_adapter operator--(int) noexcept {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    WJR_CONSTEXPR20 contiguous_const_iterator_adapter &operator+=(difference_type n) noexcept {
        __check_offset(n);
        m_ptr += n;
        return *this;
    }

    WJR_NODISCARD WJR_CONSTEXPR20 contiguous_const_iterator_adapter
    operator+(difference_type n) const noexcept {
        auto tmp = *this;
        return tmp += n;
    }

    WJR_NODISCARD friend WJR_CONSTEXPR20 contiguous_const_iterator_adapter
    operator+(difference_type n, const contiguous_const_iterator_adapter &rhs) noexcept {
        return rhs + n;
    }

    WJR_CONSTEXPR20 contiguous_const_iterator_adapter &operator-=(difference_type n) noexcept {
        __check_offset(-n);
        m_ptr -= n;
        return *this;
    }

    WJR_NODISCARD WJR_CONSTEXPR20 contiguous_const_iterator_adapter
    operator-(difference_type n) const noexcept {
        auto tmp = *this;
        return tmp -= n;
    }

    WJR_NODISCARD WJR_PURE WJR_CONSTEXPR20 difference_type
    operator-(const contiguous_const_iterator_adapter &rhs) const noexcept {
        __check_same_container(rhs);
        return m_ptr - rhs.m_ptr;
    }

    WJR_NODISCARD WJR_CONSTEXPR20 reference operator[](difference_type n) const noexcept {
        return *(*this + n);
    }

    WJR_NODISCARD WJR_PURE WJR_CONSTEXPR20 bool
    operator==(const contiguous_const_iterator_adapter &rhs) const noexcept {
        __check_same_container(rhs);
        return m_ptr == rhs.m_ptr;
    }

    WJR_NODISCARD WJR_PURE WJR_CONSTEXPR20 bool
    operator!=(const contiguous_const_iterator_adapter &rhs) const noexcept {
        return !(*this == rhs);
    }

    WJR_NODISCARD WJR_PURE WJR_CONSTEXPR20 bool
    operator<(const contiguous_const_iterator_adapter &rhs) const noexcept {
        __check_same_container(rhs);
        return m_ptr < rhs.m_ptr;
    }

    WJR_NODISCARD WJR_PURE WJR_CONSTEXPR20 bool
    operator>(const contiguous_const_iterator_adapter &rhs) const noexcept {
        return rhs < *this;
    }

    WJR_NODISCARD WJR_PURE WJR_CONSTEXPR20 bool
    operator<=(const contiguous_const_iterator_adapter &rhs) const noexcept {
        return !(rhs < *this);
    }

    WJR_NODISCARD WJR_PURE WJR_CONSTEXPR20 bool
    operator>=(const contiguous_const_iterator_adapter &rhs) const noexcept {
        return !(*this < rhs);
    }

    WJR_CONSTEXPR20 void
    check_same_container(WJR_MAYBE_UNUSED const Container *cont) const noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_CHECK(m_container == cont, "Can't compare iterators from different containers.");
#else
        (void)(cont);
#endif
    }

private:
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
    /// @private
    WJR_CONSTEXPR20 void __set_container(const Container *container) noexcept {
        m_container = container;
    }

    /// @private
    WJR_CONSTEXPR20 void __check_offset(difference_type offset) const noexcept {
        if (offset == 0) {
            return;
        }
        WJR_CHECK(m_container != nullptr, "Can't seek an value-initialized iterator.");
        WJR_CHECK(m_ptr != nullptr, "Can't seek an invalid iterator.");
        if (offset < 0) {
            WJR_CHECK(offset >= __begin() - m_ptr,
                      "Can't seek an iterator that before the beginning.");
        } else {
            WJR_CHECK(offset <= __end() - m_ptr, "Can't seek an iterator that after the end.");
        }
    }

    /// @private
    WJR_CONSTEXPR20 void
    __check_same_container(const contiguous_const_iterator_adapter &rhs) const noexcept {
        WJR_CHECK(m_container == rhs.m_container,
                  "Can't compare iterators from different containers.");
    }

    /// @private
    WJR_PURE WJR_CONSTEXPR20 pointer __begin() const noexcept { return m_container->data(); }

    /// @private
    WJR_PURE WJR_CONSTEXPR20 pointer __end() const noexcept {
        return m_container->data() + m_container->size();
    }
#else
    /// @private
    constexpr static void __set_container(const Container *) noexcept {}

    /// @private
    constexpr static void __check_offset(difference_type) noexcept {}

    /// @private
    constexpr static void
    __check_same_container(const contiguous_const_iterator_adapter &) noexcept {}

#endif
protected:
    __pointer m_ptr;
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
    const Container *m_container;
#endif
};

template <typename Container, typename Traits>
class contiguous_iterator_adapter : public contiguous_const_iterator_adapter<Container, Traits> {
    using Mybase = contiguous_const_iterator_adapter<Container, Traits>;
    using __pointer = typename Traits::pointer;

    template <typename Ptr>
    friend struct std::pointer_traits;

public:
#if defined(WJR_CPP_20)
    using iterator_concept = typename Mybase::iterator_concept;
#endif
    using iterator_category = typename Mybase::iterator_category;
    using value_type = typename Mybase::value_type;
    using difference_type = typename Mybase::difference_type;
    using pointer = typename Traits::pointer;
    using reference = typename Traits::reference;

    using Mybase::Mybase;

    WJR_NODISCARD WJR_CONSTEXPR20 pointer operator->() const noexcept {
        return const_cast<pointer>(Mybase::operator->());
    }

    WJR_NODISCARD WJR_CONSTEXPR20 reference operator*() const noexcept { return *operator->(); }

    WJR_CONSTEXPR20 contiguous_iterator_adapter &operator++() noexcept {
        Mybase::operator++();
        return *this;
    }

    WJR_CONSTEXPR20 contiguous_iterator_adapter operator++(int) noexcept {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    WJR_CONSTEXPR20 contiguous_iterator_adapter &operator--() noexcept {
        Mybase::operator--();
        return *this;
    }

    WJR_CONSTEXPR20 contiguous_iterator_adapter operator--(int) noexcept {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    WJR_CONSTEXPR20 contiguous_iterator_adapter &operator+=(difference_type n) noexcept {
        Mybase::operator+=(n);
        return *this;
    }

    WJR_NODISCARD WJR_CONSTEXPR20 contiguous_iterator_adapter
    operator+(difference_type n) const noexcept {
        auto tmp = *this;
        return tmp += n;
    }

    WJR_NODISCARD friend WJR_CONSTEXPR20 contiguous_iterator_adapter
    operator+(difference_type n, const contiguous_iterator_adapter &rhs) noexcept {
        return rhs + n;
    }

    WJR_CONSTEXPR20 contiguous_iterator_adapter &operator-=(difference_type n) noexcept {
        Mybase::operator-=(n);
        return *this;
    }

    WJR_NODISCARD WJR_CONSTEXPR20 contiguous_iterator_adapter
    operator-(difference_type n) const noexcept {
        auto tmp = *this;
        return tmp -= n;
    }

    using Mybase::operator-;

    WJR_NODISCARD WJR_CONSTEXPR20 reference operator[](difference_type n) const noexcept {
        return *(*this + n);
    }

    using Mybase::check_same_container;
};

/// @private
template <typename Container, typename Traits>
struct __is_contiguous_iterator_impl<contiguous_const_iterator_adapter<Container, Traits>>
    : std::true_type {};

/// @private
template <typename Container, typename Traits>
struct __is_contiguous_iterator_impl<contiguous_iterator_adapter<Container, Traits>>
    : std::true_type {};

} // namespace wjr

namespace std {

template <typename Container, typename Traits>
struct pointer_traits<wjr::contiguous_const_iterator_adapter<Container, Traits>> {
    using pointer = wjr::contiguous_const_iterator_adapter<Container, Traits>;
    using element_type = const typename pointer::value_type;
    using difference_type = typename pointer::difference_type;

    WJR_NODISCARD constexpr static element_type *to_address(const pointer &ptr) noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        const auto cont = ptr.m_container;
        if (cont) {
            WJR_CHECK(ptr.m_ptr >= ptr.__begin() && ptr.m_ptr <= ptr.__end(),
                      "can't convert out-of-range vector iterator to pointer");
        } else {
            WJR_CHECK(ptr.m_ptr == nullptr, "can't convert invalid vector iterator to pointer");
        }
#endif

        return const_cast<element_type *>(wjr::to_address(ptr.m_ptr));
    }
};

template <typename Container, typename Traits>
struct pointer_traits<wjr::contiguous_iterator_adapter<Container, Traits>> {
    using pointer = wjr::contiguous_iterator_adapter<Container, Traits>;
    using element_type = typename pointer::value_type;
    using difference_type = typename pointer::difference_type;

    WJR_NODISCARD constexpr static element_type *to_address(const pointer &ptr) noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        const auto cont = ptr.m_container;
        if (cont) {
            WJR_CHECK(ptr.m_ptr >= ptr.__begin() && ptr.m_ptr <= ptr.__end(),
                      "can't convert out-of-range vector iterator to pointer");
        } else {
            WJR_CHECK(ptr.m_ptr == nullptr, "can't convert invalid vector iterator to pointer");
        }
#endif

        return wjr::to_address(ptr.m_ptr);
    }
};

} // namespace std

#endif // WJR_ITERATOR_CONTIGUOUS_ITERATOR_ADAPTER_HPP__
