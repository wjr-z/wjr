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
#include <wjr/crtp/trivial_allocator_base.hpp>
#include <wjr/memory/details.hpp>

namespace wjr {

template <typename Iter, typename... Args>
WJR_CONSTEXPR20 void construct_at(Iter iter, Args &&...args) {
    ::new (static_cast<void *>((to_address)(iter)))
        iterator_value_t<Iter>(std::forward<Args>(args)...);
}

template <typename Iter>
WJR_CONSTEXPR20 void construct_at(Iter iter, dctor_t) {
    ::new (static_cast<void *>((to_address)(iter))) iterator_value_t<Iter>;
}

template <typename Iter, typename Alloc, typename... Args>
WJR_CONSTEXPR20 void uninitialized_construct_using_allocator(Iter iter, Alloc &alloc,
                                                             Args &&...args) {
    if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
        construct_at(iter, std::forward<Args>(args)...);
    } else {
        std::allocator_traits<Alloc>::construct(alloc, (to_address)(iter),
                                                std::forward<Args>(args)...);
    }
}

template <typename Iter, typename Alloc>
WJR_CONSTEXPR20 void uninitialized_construct_using_allocator(Iter iter, Alloc &alloc,
                                                             dctor_t) {
    if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
        construct_at(iter, dctor_t{});
    } else {
        std::allocator_traits<Alloc>::construct(alloc, (to_address)(iter));
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
            std::allocator_traits<Alloc>::construct(alloc, (to_address)(d_first), *first);
        }
        return d_first;
    }
}

template <typename InputIt, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt __uninitialized_copy_restrict_using_allocator_impl_aux(
    add_restrict_t<InputIt> first, add_restrict_t<InputIt> last,
    add_restrict_t<OutputIt> d_first, Alloc &alloc) {
    return uninitialized_copy_using_allocator(first, last, d_first, alloc);
}

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
    const auto __first = try_to_address(first);
    const auto __last = try_to_address(last);
    if constexpr (is_contiguous_iterator_v<OutputIt>) {
        const auto __d_first = (to_address)(d_first);
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
            std::allocator_traits<Alloc>::construct(alloc, (to_address)(d_first), *first);
        }
        return d_first;
    }
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt __uninitialized_copy_n_restrict_using_allocator_impl_aux(
    add_restrict_t<InputIt> first, Size n, add_restrict_t<OutputIt> d_first,
    Alloc &alloc) {
    return uninitialized_copy_n_using_allocator(first, n, d_first, alloc);
}

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
    const auto __first = try_to_address(first);
    if constexpr (is_contiguous_iterator_v<OutputIt>) {
        const auto __d_first = (to_address)(d_first);
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
                std::allocator_traits<Alloc>::construct(alloc, (to_address)(first),
                                                        value_type());
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
                std::allocator_traits<Alloc>::construct(alloc, (to_address)(first),
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
        using value_type = iterator_value_t<Iter>;
        for (; first != last; ++first) {
            std::allocator_traits<Alloc>::construct(alloc, (to_address)(first),
                                                    value_type());
        }
    }
}

template <typename Iter, typename Size, typename Alloc>
WJR_CONSTEXPR20 void uninitialized_value_construct_n_using_allocator(Iter first, Size n,
                                                                     Alloc &alloc) {
    if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
        std::uninitialized_value_construct_n(first, n);
    } else {
        using value_type = iterator_value_t<Iter>;
        for (; n > 0; ++first, --n) {
            std::allocator_traits<Alloc>::construct(alloc, (to_address)(first),
                                                    value_type());
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
                std::allocator_traits<Alloc>::construct(alloc, (to_address)(first),
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
                std::allocator_traits<Alloc>::construct(alloc, (to_address)(first),
                                                        value);
            }
        }
    }
}

template <typename Iter, typename Alloc>
WJR_CONSTEXPR20 void destroy_at_using_allocator(Iter iter, Alloc &alloc) {
    if constexpr (is_trivially_allocator_destructible_v<Alloc>) {
        std::destroy_at((to_address)(iter));
    } else {
        std::allocator_traits<Alloc>::destroy(alloc, (to_address)(iter));
    }
}

template <typename Iter, typename Alloc>
WJR_CONSTEXPR20 void destroy_using_allocator(Iter first, Iter last, Alloc &alloc) {
    if constexpr (is_trivially_allocator_destructible_v<Alloc>) {
        std::destroy(first, last);
    } else {
        for (; first != last; ++first) {
            std::allocator_traits<Alloc>::destroy(alloc, (to_address)(first));
        }
    }
}

template <typename Iter, typename Size, typename Alloc>
WJR_CONSTEXPR20 void destroy_n_using_allocator(Iter first, Size n, Alloc &alloc) {
    if constexpr (is_trivially_allocator_v<Alloc>) {
        std::destroy_n(first, n);
    } else {
        for (; n > 0; ++first, --n) {
            std::allocator_traits<Alloc>::destroy(alloc, (to_address)(first));
        }
    }
}

template <typename T, typename Tag>
using __uninitilized_checker_base_enabler_select = enable_special_members_base<
    std::is_trivially_default_constructible_v<T>, true,
    std::is_trivially_copy_constructible_v<T>, std::is_trivially_move_constructible_v<T>,
    std::is_trivially_copy_assignable_v<T>, std::is_trivially_move_assignable_v<T>, Tag>;

#if WJR_DEBUG_LEVEL > 2

template <typename T, typename Tag>
struct __uninitialized_checker : __uninitilized_checker_base_enabler_select<T, Tag> {
    using Mybase = __uninitilized_checker_base_enabler_select<T, Tag>;
    using Mybase::Mybase;
    static constexpr bool __is_noexcept = false;

    __uninitialized_checker(bool value) noexcept
        : Mybase(enable_default_constructor), m_initialized(value) {}

protected:
    constexpr void check(bool value) const {
        WJR_ASSERT_L2(m_initialized == value, "Expected ",
                      (value ? "initialized" : "uninitialized"),
                      " value when using an uninitialized object.");
    }

    constexpr void set(bool value) noexcept { m_initialized = value; }

    bool m_initialized = false;
};

#else

template <typename T, typename Tag>
struct __uninitialized_checker : __uninitilized_checker_base_enabler_select<T, Tag> {
    using Mybase = __uninitilized_checker_base_enabler_select<T, Tag>;
    using Mybase::Mybase;
    static constexpr bool __is_noexcept = true;

    constexpr __uninitialized_checker(bool) noexcept
        : Mybase(enable_default_constructor) {}

protected:
    constexpr void check(bool) const noexcept {}
    constexpr void set(bool) noexcept {}
};

#endif

template <bool Trivial, typename T>
struct __uninitilized_dtor_base
    : __uninitialized_checker<T, __uninitilized_dtor_base<Trivial, T>> {
    using Mybase = __uninitialized_checker<T, __uninitilized_dtor_base<Trivial, T>>;
    using Mybase::Mybase;

    template <typename... Args>
    constexpr __uninitilized_dtor_base(Args &&...args) noexcept(
        Mybase::__is_noexcept &&std::is_nothrow_constructible_v<T, Args...>)
        : Mybase(true), m_value(std::forward<Args>(args)...) {}

    constexpr __uninitilized_dtor_base(enable_default_constructor_t) noexcept(
        Mybase::__is_noexcept)
        : Mybase(enable_default_constructor), m_storage() {}

    // don't need to check for trivially destructible types
    ~__uninitilized_dtor_base() noexcept = default;

    union {
        T m_value;
        std::aligned_storage_t<sizeof(T), alignof(T)> m_storage;
    };
};

template <typename T>
struct __uninitilized_dtor_base<false, T>
    : __uninitialized_checker<T, __uninitilized_dtor_base<false, T>> {
    using Mybase = __uninitialized_checker<T, __uninitilized_dtor_base<false, T>>;
    using Mybase::Mybase;

    template <typename... Args>
    constexpr __uninitilized_dtor_base(Args &&...args) noexcept(
        Mybase::__is_noexcept &&std::is_nothrow_constructible_v<T, Args...>)
        : Mybase(true), m_value(std::forward<Args>(args)...) {}

    constexpr __uninitilized_dtor_base(enable_default_constructor_t) noexcept(
        Mybase::__is_noexcept)
        : Mybase(enable_default_constructor), m_storage() {}

    ~__uninitilized_dtor_base() noexcept(Mybase::__is_noexcept) { Mybase::check(false); }

    union {
        T m_value;
        std::aligned_storage_t<sizeof(T), alignof(T)> m_storage;
    };
};

template <typename T>
using __uninitilized_dtor_base_select =
    __uninitilized_dtor_base<std::is_trivially_destructible_v<T>, T>;

template <bool Trivial, typename T>
struct __uninitialized_ctor_base : __uninitilized_dtor_base_select<T> {
    using Mybase = __uninitilized_dtor_base_select<T>;

    constexpr __uninitialized_ctor_base() noexcept = default;

    template <typename... Args>
    constexpr __uninitialized_ctor_base(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<Mybase, Args...>)
        : Mybase(std::forward<Args>(args)...) {}
};

template <typename T>
struct __uninitialized_ctor_base<false, T> : __uninitilized_dtor_base_select<T> {
    using Mybase = __uninitilized_dtor_base_select<T>;

    constexpr __uninitialized_ctor_base() noexcept : Mybase(enable_default_constructor) {}

    template <typename... Args>
    constexpr __uninitialized_ctor_base(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<Mybase, Args...>)
        : Mybase(std::forward<Args>(args)...) {}
};

template <typename T>
using __uninitialized_ctor_base_select =
    __uninitialized_ctor_base<std::is_trivially_default_constructible_v<T>, T>;

/**
 * @class uninitialized
 *
 * @details Trivially constructible and destructible uninitialized object. Copy/move
 * constructor and assignment operators are deleted if the type is not trivially
 * copy/move constructible/assignable.
 *
 */
template <typename T>
class uninitialized : __uninitialized_ctor_base_select<T> {
    using Mybase = __uninitialized_ctor_base_select<T>;

public:
    constexpr uninitialized() noexcept = default;
    ~uninitialized() noexcept(std::is_nothrow_destructible_v<Mybase>) = default;

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<T, Args...>)>
    constexpr uninitialized(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<Mybase, Args...>)
        : Mybase(std::forward<Args>(args)...) {}

    constexpr uninitialized(dctor_t) noexcept : Mybase() {}

    constexpr T &get() & noexcept(Mybase::__is_noexcept) {
        Mybase::check(true);
        return Mybase::m_value;
    }

    constexpr const T &get() const & noexcept(Mybase::__is_noexcept) {
        Mybase::check(true);
        return Mybase::m_value;
    }

    constexpr T &&get() && noexcept(Mybase::__is_noexcept) {
        Mybase::check(true);
        return std::move(Mybase::m_value);
    }

    constexpr const T &&get() const && noexcept(Mybase::__is_noexcept) {
        Mybase::check(true);
        return std::move(Mybase::m_value);
    }

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<T, Args...>)>
    constexpr T &emplace(Args &&...args) noexcept(
        Mybase::__is_noexcept &&std::is_nothrow_constructible_v<Mybase, Args...>) {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            Mybase::check(false);
        }

        construct_at(this, std::forward<Args>(args)...);
        return get();
    }

    constexpr void
    reset() noexcept(Mybase::__is_noexcept &&std::is_nothrow_destructible_v<T>) {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            Mybase::check(true);
            get().~T();
        }

        Mybase::set(false);
    }

    constexpr T *operator->() noexcept(Mybase::__is_noexcept) {
        return std::addressof(get());
    }

    constexpr const T *operator->() const noexcept(Mybase::__is_noexcept) {
        return std::addressof(get());
    }

    constexpr T &operator*() & noexcept(Mybase::__is_noexcept) { return get(); }

    constexpr const T &operator*() const & noexcept(Mybase::__is_noexcept) {
        return get();
    }

    constexpr T &&operator*() && noexcept(Mybase::__is_noexcept) {
        return std::move(get());
    }

    constexpr const T &&operator*() const && noexcept(Mybase::__is_noexcept) {
        return std::move(get());
    }
};

} // namespace wjr

#endif // WJR_MEMORY_UNINITIALIZED_HPP__