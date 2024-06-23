#ifndef WJR_MEMORY_UNINITIALIZED_HPP__
#define WJR_MEMORY_UNINITIALIZED_HPP__

/**
 * @file uninitialized.hpp
 * @brief The header file for uninitialized memory operations using allocator.
 *
 * @version 0.0.1
 * @date 2024-03-18
 *
 */

#include <wjr/crtp/class_base.hpp>
#include <wjr/crtp/trivially_allocator_base.hpp>
#include <wjr/memory/details.hpp>

namespace wjr {

template <typename Iter, typename... Args>
WJR_CONSTEXPR20 void construct_at(Iter iter, Args &&...args) {
    ::new (static_cast<void *>(wjr::to_address(iter)))
        iterator_value_t<Iter>(std::forward<Args>(args)...);
}

template <typename Iter>
WJR_CONSTEXPR20 void construct_at(Iter iter, dctor_t) {
    ::new (static_cast<void *>(wjr::to_address(iter))) iterator_value_t<Iter>;
}

template <typename Iter, typename Alloc, typename... Args>
WJR_CONSTEXPR20 void uninitialized_construct_using_allocator(Iter iter, Alloc &alloc,
                                                             Args &&...args) {
    if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
        (construct_at)(iter, std::forward<Args>(args)...);
    } else {
        std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(iter),
                                                std::forward<Args>(args)...);
    }
}

template <typename Iter, typename Alloc>
WJR_CONSTEXPR20 void uninitialized_construct_using_allocator(Iter iter, Alloc &alloc,
                                                             dctor_t) {
    if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
        (construct_at)(iter, dctor_t{});
    } else {
        std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(iter));
    }
}

template <typename InputIt, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt uninitialized_copy_using_allocator(InputIt first, InputIt last,
                                                            OutputIt d_first,
                                                            Alloc &alloc) {
    if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
        return std::uninitialized_copy(first, last, d_first);
    } else {
        for (; first != last; ++first, ++d_first) {
            std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(d_first),
                                                    *first);
        }
        return d_first;
    }
}

/// @private
template <typename InputIt, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt __uninitialized_copy_restrict_using_allocator_impl_aux(
    add_restrict_t<InputIt> first, add_restrict_t<InputIt> last,
    add_restrict_t<OutputIt> d_first, Alloc &alloc) {
    return uninitialized_copy_using_allocator(first, last, d_first, alloc);
}

/// @private
template <typename InputIt, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt __uninitialized_copy_restrict_using_allocator_impl(
    InputIt first, InputIt last, OutputIt d_first, Alloc &alloc) {
    return __uninitialized_copy_restrict_using_allocator_impl_aux<InputIt, OutputIt,
                                                                  Alloc>(first, last,
                                                                         d_first, alloc);
}

template <typename InputIt, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt uninitialized_copy_restrict_using_allocator(InputIt first,
                                                                     InputIt last,
                                                                     OutputIt d_first,
                                                                     Alloc &alloc) {
    const auto __first = to_contiguous_address(first);
    const auto __last = to_contiguous_address(last);
    if constexpr (is_contiguous_iterator_v<OutputIt>) {
        const auto __d_first = wjr::to_address(d_first);
        const auto __d_last = __uninitialized_copy_restrict_using_allocator_impl(
            __first, __last, __d_first, alloc);
        return std::next(d_first, std::distance(__d_first, __d_last));
    } else {
        return __uninitialized_copy_restrict_using_allocator_impl(__first, __last,
                                                                  d_first, alloc);
    }
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt uninitialized_copy_n_using_allocator(InputIt first, Size n,
                                                              OutputIt d_first,
                                                              Alloc &alloc) {
    if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
        return std::uninitialized_copy_n(first, n, d_first);
    } else {
        for (; n > 0; ++first, ++d_first, --n) {
            std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(d_first),
                                                    *first);
        }
        return d_first;
    }
}

/// @private
template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt __uninitialized_copy_n_restrict_using_allocator_impl_aux(
    add_restrict_t<InputIt> first, Size n, add_restrict_t<OutputIt> d_first,
    Alloc &alloc) {
    return uninitialized_copy_n_using_allocator(first, n, d_first, alloc);
}

/// @private
template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt __uninitialized_copy_n_restrict_using_allocator_impl(
    InputIt first, Size n, OutputIt d_first, Alloc &alloc) {
    return __uninitialized_copy_n_restrict_using_allocator_impl_aux<InputIt, Size,
                                                                    OutputIt, Alloc>(
        first, n, d_first, alloc);
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt uninitialized_copy_n_restrict_using_allocator(InputIt first,
                                                                       Size n,
                                                                       OutputIt d_first,
                                                                       Alloc &alloc) {
    const auto __first = to_contiguous_address(first);
    if constexpr (is_contiguous_iterator_v<OutputIt>) {
        const auto __d_first = wjr::to_address(d_first);
        const auto __d_last = __uninitialized_copy_n_restrict_using_allocator_impl(
            __first, n, __d_first, alloc);
        return std::next(d_first, std::distance(__d_first, __d_last));
    } else {
        return __uninitialized_copy_n_restrict_using_allocator_impl(__first, n, d_first,
                                                                    alloc);
    }
}

template <typename InputIt, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt uninitialized_move_using_allocator(InputIt first, InputIt last,
                                                            OutputIt d_first,
                                                            Alloc &alloc) {
    return uninitialized_copy_using_allocator(
        std::make_move_iterator(first), std::make_move_iterator(last), d_first, alloc);
}

template <typename InputIt, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt uninitialized_move_restrict_using_allocator(InputIt first,
                                                                     InputIt last,
                                                                     OutputIt d_first,
                                                                     Alloc &alloc) {
    return uninitialized_copy_restrict_using_allocator(
        std::make_move_iterator(first), std::make_move_iterator(last), d_first, alloc);
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt uninitialized_move_n_using_allocator(InputIt first, Size n,
                                                              OutputIt d_first,
                                                              Alloc &alloc) {
    return uninitialized_copy_n_using_allocator(std::make_move_iterator(first), n,
                                                d_first, alloc);
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt uninitialized_move_n_restrict_using_allocator(InputIt first,
                                                                       Size n,
                                                                       OutputIt d_first,
                                                                       Alloc &alloc) {
    return uninitialized_copy_n_restrict_using_allocator(std::make_move_iterator(first),
                                                         n, d_first, alloc);
}

template <typename Iter, typename Alloc>
WJR_CONSTEXPR20 void
uninitialized_default_construct_using_allocator(Iter first, Iter last, Alloc &alloc) {
    if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
        std::uninitialized_default_construct(first, last);
    } else {
        using value_type = iterator_value_t<Iter>;
        if constexpr (!std::is_trivially_default_constructible_v<value_type>) {
            for (; first != last; ++first) {
                std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(first));
            }
        }
    }
}

template <typename Iter, typename Size, typename Alloc>
WJR_CONSTEXPR20 void uninitialized_default_construct_n_using_allocator(Iter first, Size n,
                                                                       Alloc &alloc) {
    if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
        std::uninitialized_default_construct_n(first, n);
    } else {
        using value_type = iterator_value_t<Iter>;
        if constexpr (!std::is_trivially_default_constructible_v<value_type>) {
            for (; n > 0; ++first, --n) {
                std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(first),
                                                        value_type());
            }
        }
    }
}

template <typename Iter, typename Alloc>
WJR_CONSTEXPR20 void uninitialized_value_construct_using_allocator(Iter first, Iter last,
                                                                   Alloc &alloc) {
    if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
        std::uninitialized_value_construct(first, last);
    } else {
        for (; first != last; ++first) {
            std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(first));
        }
    }
}

template <typename Iter, typename Size, typename Alloc>
WJR_CONSTEXPR20 void uninitialized_value_construct_n_using_allocator(Iter first, Size n,
                                                                     Alloc &alloc) {
    if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
        std::uninitialized_value_construct_n(first, n);
    } else {
        for (; n > 0; ++first, --n) {
            std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(first));
        }
    }
}

/**
 * @brief Fill the range [first, last) with value using allocator.
 *
 * @tparam T The value type. Use `dctor_t` to default construct the
 * elements. Use `vctor_t` to value construct the elements.
 */
template <typename Iter, typename Alloc, typename T>
WJR_CONSTEXPR20 void uninitialized_fill_using_allocator(Iter first, Iter last,
                                                        Alloc &alloc, const T &value) {
    if constexpr (std::is_same_v<T, dctor_t>) {
        uninitialized_default_construct_using_allocator(first, last, alloc);
    } else if constexpr (std::is_same_v<T, vctor_t>) {
        uninitialized_value_construct_using_allocator(first, last, alloc);
    } else {
        if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
            std::uninitialized_fill(first, last, value);
        } else {
            for (; first != last; ++first) {
                std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(first),
                                                        value);
            }
        }
    }
}

/**
 * @brief Fill the range [first, first + n) with value using allocator.
 *
 * @tparam T The value type. Use `dctor_t` to default construct the
 * elements. Use `vctor_t` to value construct the elements.
 */
template <typename Iter, typename Size, typename Alloc, typename T>
WJR_CONSTEXPR20 void uninitialized_fill_n_using_allocator(Iter first, Size n,
                                                          Alloc &alloc, const T &value) {
    if constexpr (std::is_same_v<T, dctor_t>) {
        uninitialized_default_construct_n_using_allocator(first, n, alloc);
    } else if constexpr (std::is_same_v<T, vctor_t>) {
        uninitialized_value_construct_n_using_allocator(first, n, alloc);
    } else {
        if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
            std::uninitialized_fill_n(first, n, value);
        } else {
            for (; n > 0; ++first, --n) {
                std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(first),
                                                        value);
            }
        }
    }
}

template <typename Iter, typename Alloc>
WJR_CONSTEXPR20 void destroy_at_using_allocator(Iter iter, Alloc &alloc) {
    if constexpr (is_trivially_allocator_destructible_v<Alloc>) {
        std::destroy_at(wjr::to_address(iter));
    } else {
        std::allocator_traits<Alloc>::destroy(alloc, wjr::to_address(iter));
    }
}

template <typename Iter, typename Alloc>
WJR_CONSTEXPR20 void destroy_using_allocator(Iter first, Iter last, Alloc &alloc) {
    if constexpr (is_trivially_allocator_destructible_v<Alloc>) {
        std::destroy(first, last);
    } else {
        for (; first != last; ++first) {
            std::allocator_traits<Alloc>::destroy(alloc, wjr::to_address(first));
        }
    }
}

template <typename Iter, typename Size, typename Alloc>
WJR_CONSTEXPR20 void destroy_n_using_allocator(Iter first, Size n, Alloc &alloc) {
    if constexpr (is_trivially_allocator_v<Alloc>) {
        std::destroy_n(first, n);
    } else {
        for (; n > 0; ++first, --n) {
            std::allocator_traits<Alloc>::destroy(alloc, wjr::to_address(first));
        }
    }
}

/// @private
template <typename T, typename Tag>
using __uninitialized_checker_base_enabler_select =
    enable_special_members_base<true, true, std::is_trivially_copy_constructible_v<T>,
                                std::is_trivially_move_constructible_v<T>,
                                std::is_trivially_copy_assignable_v<T>,
                                std::is_trivially_move_assignable_v<T>, Tag>;

/// @private
template <bool Default, bool Destructor, typename T>
struct __uninitialized_base;

#define WJR_REGISTER_UNINITIALIZED_BASE(DEF, DES)                                        \
    template <typename T>                                                                \
    struct __uninitialized_base<DEF, DES, T>                                             \
        : __uninitialized_checker_base_enabler_select<                                   \
              T, __uninitialized_base<DEF, DES, T>> {                                    \
        using Mybase = __uninitialized_checker_base_enabler_select<                      \
            T, __uninitialized_base<DEF, DES, T>>;                                       \
                                                                                         \
        constexpr __uninitialized_base() noexcept WJR_PP_BOOL_IF(DEF, = default,         \
                                                                 : m_storage(){});       \
                                                                                         \
        template <typename... Args,                                                      \
                  WJR_REQUIRES(std::is_constructible_v<T, Args &&...>)>                  \
        constexpr __uninitialized_base(Args &&...args) noexcept(                         \
            std::is_nothrow_constructible_v<T, Args &&...>)                              \
            : m_value(std::forward<Args>(args)...) {}                                    \
                                                                                         \
        ~__uninitialized_base() noexcept WJR_PP_BOOL_IF(DES, = default, {});             \
                                                                                         \
        union {                                                                          \
            T m_value;                                                                   \
            std::aligned_storage_t<sizeof(T), alignof(T)> m_storage;                     \
        };                                                                               \
    }

WJR_REGISTER_UNINITIALIZED_BASE(1, 1);
WJR_REGISTER_UNINITIALIZED_BASE(1, 0);
WJR_REGISTER_UNINITIALIZED_BASE(0, 1);
WJR_REGISTER_UNINITIALIZED_BASE(0, 0);

#undef WJR_REGISTER_UNINITIALIZED_BASE

#if WJR_DEBUG_LEVEL > 1
#define WJR_HAS_DEBUG_UNINITIALIZED_CHECKER WJR_HAS_DEF
#endif

/// @private
template <typename T>
using __uninitialized_base_select =
    __uninitialized_base<std::is_trivially_default_constructible_v<T>,
                         std::is_trivially_destructible_v<T>, T>;

/**
 * @class uninitialized
 *
 * @details Uninitialized object. Make trivially constructible and destructible of
 * any type.+
 *
 * @details Trivially constructible and destructible uninitialized object. Copy/move
 * constructor and assignment operators are deleted if the type is not trivially
 * copy/move constructible/assignable.
 *
 */
template <typename T>
class uninitialized : __uninitialized_base_select<T> {
    using Mybase = __uninitialized_base_select<T>;

public:
    using Mybase::Mybase;

    constexpr uninitialized() noexcept = default;

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<Mybase, Args &&...>)>
    constexpr uninitialized(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<Mybase, Args...>)
        : Mybase(std::forward<Args>(args)...) {
        checker_set(true);
    }

    constexpr uninitialized(dctor_t) noexcept : Mybase() {}

    constexpr T &get() & noexcept {
        check(true);
        return Mybase::m_value;
    }

    constexpr const T &get() const & noexcept {
        check(true);
        return Mybase::m_value;
    }

    constexpr T &&get() && noexcept { return static_cast<T &&>(get()); }

    constexpr const T &&get() const && noexcept { return static_cast<const T &&>(get()); }

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<T, Args &&...>)>
    constexpr T &
    emplace(Args &&...args) noexcept(std::is_nothrow_constructible_v<Mybase, Args...>) {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            check(false);
        }

        (construct_at)(ptr_unsafe(), std::forward<Args>(args)...);
        checker_set(true);
        return get();
    }

    constexpr void reset() noexcept(std::is_nothrow_destructible_v<T>) {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            check(true);
        }

        std::destroy_at(ptr_unsafe());
        checker_set(false);
    }

    constexpr T *operator->() noexcept { return std::addressof(get()); }
    constexpr const T *operator->() const noexcept { return std::addressof(get()); }

    constexpr T &operator*() & noexcept { return get(); }
    constexpr const T &operator*() const & noexcept { return get(); }
    constexpr T &&operator*() && noexcept { return static_cast<T &&>(get()); }
    constexpr const T &&operator*() const && noexcept {
        return static_cast<const T &&>(get());
    }

    constexpr T *ptr_unsafe() noexcept { return std::addressof(Mybase::m_value); }
    constexpr const T *ptr_unsafe() const noexcept {
        return std::addressof(Mybase::m_value);
    }

private:
#if WJR_HAS_DEBUG(UNINITIALIZED_CHECKER)
    struct __checker {
        constexpr void set(bool value) noexcept { m_initialized = value; }
        constexpr void check(bool value) const noexcept {
            WJR_ASSERT_L0(m_initialized == value, "Expected ",
                          (value ? "initialized" : "uninitialized"),
                          " value when using an uninitialized object.");
        }

        ~__checker() noexcept {
            if constexpr (!std::is_trivially_destructible_v<T>) {
                check(false);
            }
        }

        bool m_initialized = false;
    };

    __checker m_checker;

    constexpr void checker_set(bool value) noexcept { m_checker.set(value); }
    constexpr void check(bool value) const noexcept { m_checker.check(value); }
#else
    constexpr static void checker_set(bool) noexcept {}
    constexpr static void check(bool) noexcept {}
#endif
};

/// @private
template <typename T, bool = true>
class __lazy_base : public uninitialized<T> {
    using Mybase = uninitialized<T>;

public:
    using Mybase::Mybase;
};

/// @private
template <typename T>
class __lazy_base<T, false> : public uninitialized<T> {
    using Mybase = uninitialized<T>;

public:
    using Mybase::Mybase;

    ~__lazy_base() noexcept(noexcept(Mybase::reset())) { Mybase::reset(); }
};

/// @private
template <typename T>
using lazy_base = __lazy_base<T,
#if WJR_HAS_DEBUG(UNINITIALIZED_CHECKER)
                              false
#else
                              std::is_trivially_destructible_v<T>
#endif
                              >;

template <typename T>
class lazy : public lazy_base<T> {
    using Mybase = lazy_base<T>;

public:
    using Mybase::Mybase;
};

} // namespace wjr

#endif // WJR_MEMORY_UNINITIALIZED_HPP__