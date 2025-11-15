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
#include <wjr/memory/detail.hpp>

namespace wjr {
template <typename T, typename... Args>
constexpr void construct_at(T *ptr,
                            Args &&...args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
    std::construct_at(ptr, std::forward<Args>(args)...);
}

template <typename T>
requires(!std::is_constructible_v<T, default_construct_t>)
constexpr void
construct_at(T *ptr, default_construct_t) noexcept(std::is_nothrow_default_constructible_v<T>) {
    // todo: Make this function constexpr if possible
    ::new (static_cast<void *>(ptr)) T;
}

namespace detail {
constexpr bool use_constexpr_memory_algorithms() {
#if __cpp_lib_raw_memory_algorithms >= 202411L
    // std has constexpr memory algorithms
    // Don't need to use custom constexpr algorithms
    return false;
#else
    return is_constant_evaluated();
#endif
}
} // namespace detail

template <typename Iter, typename Alloc, typename... Args>
constexpr void uninitialized_construct_using_allocator(Iter iter, Alloc &alloc, Args &&...args) {
    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<Iter>, Args...>) {
        wjr::construct_at(wjr::to_address(iter), std::forward<Args>(args)...);
    } else {
        std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(iter),
                                                std::forward<Args>(args)...);
    }
}

template <typename Iter, typename Alloc>
constexpr void uninitialized_construct_using_allocator(Iter iter, Alloc &alloc,
                                                       default_construct_t) {
    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<Iter>>) {
        wjr::construct_at(wjr::to_address(iter), default_construct);
    } else {
        std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(iter));
    }
}

template <typename InputItInit, typename InputItSe, typename OutputIt>
constexpr OutputIt uninitialized_copy(InputItInit first, InputItSe last, OutputIt dest) {
    auto _first = wjr::_iter_base(first);
    auto _last = wjr::_iter_base(last);
    auto _dest = wjr::_iter_base(dest);

    if constexpr (detail::_same_iterator<InputItInit, InputItSe>) {
        if (!detail::use_constexpr_memory_algorithms()) {
            return wjr::_iter_wrap(dest, std::uninitialized_copy(_first, _last, _dest));
        }
    }

    for (; _first != _last; ++_dest, (void)++_first) {
        wjr::construct_at(wjr::to_address(_dest), *_first);
    }

    return wjr::_iter_wrap(dest, _dest);
}

template <typename InputItInit, typename InputItSe, typename OutputIt>
constexpr OutputIt uninitialized_copy_restrict(InputItInit first, InputItSe last, OutputIt dest) {
    return wjr::_iter_wrap(dest, wjr::uninitialized_copy(_add_restrict(wjr::_iter_base(first)),
                                                         _add_restrict(wjr::_iter_base(last)),
                                                         _add_restrict(wjr::_iter_base(dest))));
}

template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt uninitialized_copy_n(InputIt first, Size n, OutputIt dest) {
    auto _first = wjr::_iter_base(first);
    auto _dest = wjr::_iter_base(dest);

    if (!detail::use_constexpr_memory_algorithms()) {
        return wjr::_iter_wrap(dest, std::uninitialized_copy_n(_first, n, _dest));
    }

    for (; n > 0; --n, (void)++_first, (void)++_dest) {
        wjr::construct_at(wjr::to_address(_dest), *_first);
    }

    return wjr::_iter_wrap(dest, _dest);
}

template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt uninitialized_copy_n_restrict(InputIt first, Size n, OutputIt dest) {
    return wjr::_iter_wrap(dest, wjr::uninitialized_copy_n(_add_restrict(wjr::_iter_base(first)), n,
                                                           _add_restrict(wjr::_iter_base(dest))));
}

template <typename InputItInit, typename InputItSe, typename OutputIt, typename Alloc>
constexpr OutputIt uninitialized_copy_using_allocator(InputItInit first, InputItSe last,
                                                      OutputIt dest, Alloc &alloc) {
    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<OutputIt>,
                                                     iterator_reference_t<InputItInit>>) {
        return wjr::uninitialized_copy(first, last, dest);
    } else {
        auto _first = wjr::_iter_base(first);
        auto _last = wjr::_iter_base(last);
        auto _dest = wjr::_iter_base(dest);
        for (; _first != _last; ++_dest, (void)++_first) {
            std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(_dest), *first);
        }
        return wjr::_iter_wrap(dest, _dest);
    }
}

template <typename InputItInit, typename InputItSe, typename OutputIt, typename Alloc>
constexpr OutputIt uninitialized_copy_restrict_using_allocator(InputItInit first, InputItSe last,
                                                               OutputIt dest, Alloc &alloc) {
    return wjr::_iter_wrap(
        dest, uninitialized_copy_using_allocator(_add_restrict(wjr::_iter_base(first)),
                                                 _add_restrict(wjr::_iter_base(last)),
                                                 _add_restrict(wjr::_iter_base(dest)), alloc));
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
constexpr OutputIt uninitialized_copy_n_using_allocator(InputIt first, Size n, OutputIt dest,
                                                        Alloc &alloc) {
    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<OutputIt>,
                                                     iterator_reference_t<InputIt>>) {
        return wjr::uninitialized_copy_n(first, n, dest);
    } else {
        auto _first = wjr::_iter_base(first);
        auto _dest = wjr::_iter_base(dest);
        for (; n > 0; --n, (void)++_dest, (void)++_first) {
            std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(_dest), *_first);
        }
        return wjr::_iter_wrap(dest, _dest);
    }
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
constexpr OutputIt uninitialized_copy_n_restrict_using_allocator(InputIt first, Size n,
                                                                 OutputIt dest, Alloc &alloc) {
    return wjr::_iter_wrap(
        dest, uninitialized_copy_n_using_allocator(_add_restrict(wjr::_iter_base(first)), n,
                                                   _add_restrict(wjr::_iter_base(dest)), alloc));
}

template <typename InputItInit, typename InputItSe, typename OutputIt>
constexpr OutputIt uninitialized_move(InputItInit first, InputItSe last, OutputIt dest) {
    return wjr::uninitialized_copy(std::make_move_iterator(first), std::make_move_iterator(last),
                                   dest);
}

template <typename InputItInit, typename InputItSe, typename OutputIt>
constexpr OutputIt uninitialized_move_restrict(InputItInit first, InputItSe last, OutputIt dest) {
    return uninitialized_copy_restrict(std::make_move_iterator(first),
                                       std::make_move_iterator(last), dest);
}

template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt uninitialized_move_n(InputIt first, Size n, OutputIt dest) {
    return wjr::uninitialized_copy_n(std::make_move_iterator(first), n, dest);
}

template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt uninitialized_move_n_restrict(InputIt first, Size n, OutputIt dest) {
    return uninitialized_copy_n_restrict(std::make_move_iterator(first), n, dest);
}

template <typename InputItnit, typename InputItSe, typename OutputIt, typename Alloc>
constexpr OutputIt uninitialized_move_using_allocator(InputItnit first, InputItSe last,
                                                      OutputIt dest, Alloc &alloc) {
    return uninitialized_copy_using_allocator(std::make_move_iterator(first),
                                              std::make_move_iterator(last), dest, alloc);
}

template <typename InputItInit, typename InputItSe, typename OutputIt, typename Alloc>
constexpr OutputIt uninitialized_move_restrict_using_allocator(InputItInit first, InputItSe last,
                                                               OutputIt dest, Alloc &alloc) {
    return uninitialized_copy_restrict_using_allocator(std::make_move_iterator(first),
                                                       std::make_move_iterator(last), dest, alloc);
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
constexpr OutputIt uninitialized_move_n_using_allocator(InputIt first, Size n, OutputIt dest,
                                                        Alloc &alloc) {
    return uninitialized_copy_n_using_allocator(std::make_move_iterator(first), n, dest, alloc);
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
constexpr OutputIt uninitialized_move_n_restrict_using_allocator(InputIt first, Size n,
                                                                 OutputIt dest, Alloc &alloc) {
    return uninitialized_copy_n_restrict_using_allocator(std::make_move_iterator(first), n, dest,
                                                         alloc);
}

template <typename IterInit, typename IterSe>
constexpr void uninitialized_default_construct(IterInit first, IterSe last) {
    auto _first = wjr::_iter_base(first);
    auto _last = wjr::_iter_base(last);

    if constexpr (detail::_same_iterator<IterInit, IterSe>) {
        if (!detail::use_constexpr_memory_algorithms()) {
            std::uninitialized_default_construct(_first, _last);
            return;
        }
    }

    for (; _first != _last; ++_first) {
        wjr::construct_at(wjr::to_address(_first), default_construct);
    }
}

template <typename IterInit, typename IterSe, typename Alloc>
constexpr void uninitialized_default_construct_using_allocator(IterInit first, IterSe last,
                                                               Alloc &alloc) {
    using value_type = iterator_value_t<IterInit>;
    WJR_MAYBE_UNUSED auto _first = wjr::_iter_base(first);
    WJR_MAYBE_UNUSED auto _last = wjr::_iter_base(last);
    if constexpr (is_trivially_allocator_construct_v<Alloc, value_type>) {
        wjr::uninitialized_default_construct(_first, _last);
    } else {
        if constexpr (!std::is_trivially_default_constructible_v<value_type>) {
            for (; _first != _last; ++_first) {
                std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(_first));
            }
        }
    }
}

template <typename Iter, typename Size, typename Alloc>
constexpr void uninitialized_default_construct_n_using_allocator(Iter first, Size n, Alloc &alloc) {
    using value_type = iterator_value_t<Iter>;
    auto _first = wjr::_iter_base(first);

    if constexpr (is_trivially_allocator_construct_v<Alloc, value_type>) {
        if (!detail::use_constexpr_memory_algorithms()) {
            std::uninitialized_default_construct_n(_first, n);
            return;
        }
    }

    if constexpr (!std::is_trivially_default_constructible_v<value_type>) {
        for (; n > 0; ++_first, --n) {
            std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(_first), value_type());
        }
    }
}

template <typename IterInit, typename InitSe>
constexpr void uninitialized_value_construct(IterInit first, InitSe last) {
    auto _first = wjr::_iter_base(first);
    auto _last = wjr::_iter_base(last);

    if constexpr (detail::_same_iterator<IterInit, InitSe>) {
        if (!detail::use_constexpr_memory_algorithms()) {
            std::uninitialized_value_construct(_first, _last);
            return;
        }
    }

    for (; _first != _last; ++_first) {
        wjr::construct_at(wjr::to_address(_first));
    }
}

template <typename IterInit, typename InitSe, typename Alloc>
constexpr void uninitialized_value_construct_using_allocator(IterInit first, InitSe last,
                                                             Alloc &alloc) {
    auto _first = wjr::_iter_base(first);
    auto _last = wjr::_iter_base(last);
    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<IterInit>>) {
        wjr::uninitialized_value_construct(_first, _last);
    } else {
        for (; _first != _last; ++_first) {
            std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(_first));
        }
    }
}

template <typename Iter, typename Size, typename Alloc>
constexpr void uninitialized_value_construct_n_using_allocator(Iter first, Size n, Alloc &alloc) {
    auto _first = wjr::_iter_base(first);

    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<Iter>>) {
        if (!detail::use_constexpr_memory_algorithms()) {
            std::uninitialized_value_construct_n(_first, n);
            return;
        }
    }

    for (; n > 0; --n, (void)++_first) {
        std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(_first));
    }
}

template <typename IterInit, typename InitSe, typename T>
constexpr void uninitialized_fill(IterInit first, InitSe last, const T &value) {
    auto _first = wjr::_iter_base(first);
    auto _last = wjr::_iter_base(last);

    if constexpr (detail::_same_iterator<IterInit, InitSe>) {
        if (!detail::use_constexpr_memory_algorithms()) {
            std::uninitialized_fill(_first, _last, value);
            return;
        }
    }

    for (; _first != _last; ++_first) {
        wjr::construct_at(wjr::to_address(_first), value);
    }
}

/**
 * @brief Fill the range [first, last) with value using allocator.
 *
 * @tparam T The value type. Use `default_construct_t` to default construct the
 * elements. Use `value_construct_t` to value construct the elements.
 */
template <typename IterInit, typename InitSe, typename Alloc, typename T>
constexpr void uninitialized_fill_using_allocator(IterInit first, InitSe last, Alloc &alloc,
                                                  const T &value) {
    if constexpr (std::is_same_v<T, default_construct_t>) {
        uninitialized_default_construct_using_allocator(first, last, alloc);
    } else if constexpr (std::is_same_v<T, value_construct_t>) {
        uninitialized_value_construct_using_allocator(first, last, alloc);
    } else {
        auto _first = wjr::_iter_base(first);
        auto _last = wjr::_iter_base(last);
        if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<IterInit>,
                                                         const T &>) {
            wjr::uninitialized_fill(_first, _last, value);
        } else {
            for (; _first != _last; ++_first) {
                std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(_first), value);
            }
        }
    }
}

/**
 * @brief Fill the range [first, first + n) with value using allocator.
 *
 * @tparam T The value type. Use `default_construct_t` to default construct the
 * elements. Use `value_construct_t` to value construct the elements.
 */
template <typename Iter, typename Size, typename Alloc, typename T>
constexpr void uninitialized_fill_n_using_allocator(Iter first, Size n, Alloc &alloc,
                                                    const T &value) {
    if constexpr (std::is_same_v<T, default_construct_t>) {
        uninitialized_default_construct_n_using_allocator(first, n, alloc);
    } else if constexpr (std::is_same_v<T, value_construct_t>) {
        uninitialized_value_construct_n_using_allocator(first, n, alloc);
    } else {
        auto _first = wjr::_iter_base(first);
        if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<Iter>,
                                                         const T &>) {
            if (!detail::use_constexpr_memory_algorithms()) {
                std::uninitialized_fill_n(_first, n, value);
                return;
            }
        }

        for (; n > 0; --n, (void)_first) {
            std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(_first), value);
        }
    }
}

template <typename Iter, typename Alloc>
constexpr void destroy_at_using_allocator(Iter iter, Alloc &alloc) {
    if constexpr (is_trivially_allocator_destroy_v<Alloc, iterator_value_t<Iter>>) {
        std::destroy_at(wjr::to_address(iter));
    } else {
        std::allocator_traits<Alloc>::destroy(alloc, wjr::to_address(iter));
    }
}

template <typename IterInit, typename InitSe>
constexpr void destroy(IterInit first, InitSe last) {
    auto _first = wjr::_iter_base(first);
    auto _last = wjr::_iter_base(last);
    if constexpr (detail::_same_iterator<IterInit, InitSe>) {
        std::destroy(_first, _last);
    } else {
        for (; _first != _last; ++_first) {
            std::destroy_at(wjr::to_address(_first));
        }
    }
}

template <typename IterInit, typename InitSe, typename Alloc>
constexpr void destroy_using_allocator(IterInit first, InitSe last, Alloc &alloc) {
    auto _first = wjr::_iter_base(first);
    auto _last = wjr::_iter_base(last);
    if constexpr (is_trivially_allocator_destroy_v<Alloc, iterator_value_t<IterInit>>) {
        wjr::destroy(_first, _last);
    } else {
        for (; _first != _last; ++_first) {
            std::allocator_traits<Alloc>::destroy(alloc, wjr::to_address(_first));
        }
    }
}

template <typename Iter, typename Size, typename Alloc>
constexpr void destroy_n_using_allocator(Iter first, Size n, Alloc &alloc) {
    auto _first = wjr::_iter_base(first);
    if constexpr (is_trivially_allocator_destroy_v<Alloc, iterator_value_t<Iter>>) {
        std::destroy_n(_first, n);
    } else {
        for (; n > 0; --n, (void)++_first) {
            std::allocator_traits<Alloc>::destroy(alloc, wjr::to_address(_first));
        }
    }
}

template <typename T>
constexpr T *relocate_at(T *src, T *dst) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                                  std::is_nothrow_destructible_v<T>) {
    if constexpr (get_relocate_mode_v<T> == relocate_t::trivial) {
        builtin_memcpy(dst, src, sizeof(T));
    } else {
        wjr::construct_at(dst, std::move(*src));
        std::destroy_at(src);
    }

    return dst;
}

template <typename T, typename Alloc>
constexpr T *uninitialized_relocate_at_using_allocator(T *src, T *dst, Alloc &alloc) noexcept(
    std::is_nothrow_move_constructible_v<T> && std::is_nothrow_destructible_v<T>) {
    if constexpr (is_trivially_allocator_v<Alloc>) {
        return relocate_at(src, dst);
    } else {
        wjr::uninitialized_construct_using_allocator(dst, alloc, std::move(*src));
        destroy_at_using_allocator(src, alloc);
    }

    return dst;
}

template <typename InputItInit, typename InputItSe, typename OutputIt>
constexpr OutputIt uninitialized_relocate(InputItInit first, InputItSe last, OutputIt dest) {
    using InputValue = iterator_value_t<InputItInit>;
    using OutputValue = iterator_value_t<OutputIt>;

    auto _first = wjr::_iter_base(first);
    auto _last = wjr::_iter_base(last);
    auto _dest = wjr::_iter_base(dest);

    if constexpr (detail::_same_iterator<InputItInit, InputItSe> &&
                  std::is_same_v<InputValue, OutputValue> &&
                  is_contiguous_iterator_v<InputItInit> && is_contiguous_iterator_v<OutputIt> &&
                  get_relocate_mode_v<InputValue> == relocate_t::trivial) {
        if (!is_constant_evaluated()) {
            const auto n = _last - _first;
            maybe_null_memcpy(_dest, _first, n * sizeof(InputValue));
            return wjr::_iter_wrap(dest, _dest);
        }
    } else if constexpr (std::is_same_v<InputValue, OutputValue> &&
                         get_relocate_mode_v<InputValue> == relocate_t::maybe_trivial) {
        for (; _first != _last; ++_dest, (void)++_first) {
            wjr::construct_at(wjr::to_address(_dest), std::move(*_first));
            std::destroy_at(wjr::to_address(_first));
        }

        return wjr::_iter_wrap(dest, _dest);
    }

    _dest = wjr::uninitialized_move(_first, _last, _dest);
    std::destroy(_first, _last);
    return wjr::_iter_wrap(dest, _dest);
}

template <typename InputIt, typename OutputIt>
constexpr OutputIt uninitialized_relocate_restrict(InputIt first, InputIt last, OutputIt dest) {
    return wjr::_iter_wrap(dest, uninitialized_relocate(_add_restrict(wjr::_iter_base(first)),
                                                        _add_restrict(wjr::_iter_base(last)),
                                                        _add_restrict(wjr::_iter_base(dest))));
}

template <typename InputItInit, typename InputItSe, typename OutputIt, typename Alloc>
constexpr OutputIt uninitialized_relocate_using_allocator(InputItInit first, InputItSe last,
                                                          OutputIt dest, Alloc &alloc) {
    using InputValue = iterator_value_t<InputItInit>;
    using OutputValue = iterator_value_t<OutputIt>;

    auto _first = wjr::_iter_base(first);
    auto _last = wjr::_iter_base(last);
    auto _dest = wjr::_iter_base(dest);

    if constexpr (is_trivially_allocator_construct_v<
                      Alloc, OutputValue,
                      std::remove_reference_t<iterator_reference_t<InputItInit>> &&> &&
                  is_trivially_allocator_destroy_v<Alloc, iterator_category_t<InputItInit>>) {
        return wjr::_iter_wrap(dest, uninitialized_relocate(_first, _last, _dest));
    } else if constexpr (std::is_same_v<InputValue, OutputValue> &&
                         get_relocate_mode_v<InputValue> == relocate_t::maybe_trivial) {
        for (; _first != _last; ++_dest, (void)++_first) {
            uninitialized_construct_using_allocator(_dest, alloc, std::move(*_first));
            destroy_at_using_allocator(_first, alloc);
        }

        return wjr::_iter_wrap(dest, _dest);
    } else {
        _dest = uninitialized_move_using_allocator(_first, _last, _dest, alloc);
        destroy_using_allocator(_first, _last, alloc);
        return wjr::_iter_wrap(dest, _dest);
    }
}

template <typename InputIt, typename OutputIt, typename Alloc>
constexpr OutputIt uninitialized_relocate_restrict_using_allocator(InputIt first, InputIt last,
                                                                   OutputIt dest, Alloc &alloc) {
    return wjr::_iter_wrap(
        dest, uninitialized_relocate_using_allocator(_add_restrict(wjr::_iter_base(first)),
                                                     _add_restrict(wjr::_iter_base(last)),
                                                     _add_restrict(wjr::_iter_base(dest)), alloc));
}

template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt uninitialized_relocate_n(InputIt first, Size n, OutputIt dest) {
    using InputValue = iterator_value_t<InputIt>;
    using OutputValue = iterator_value_t<OutputIt>;

    auto _first = wjr::_iter_base(first);
    auto _dest = wjr::_iter_base(dest);

    if constexpr (std::is_same_v<InputValue, OutputValue> && is_contiguous_iterator_v<InputIt> &&
                  is_contiguous_iterator_v<OutputIt> &&
                  get_relocate_mode_v<InputValue> == relocate_t::trivial) {
        if (!is_constant_evaluated()) {
            maybe_null_memcpy(_dest, _first, n * sizeof(InputValue));
            return wjr::_iter_wrap(dest, _dest);
        }
    } else if constexpr (std::is_same_v<InputValue, OutputValue> &&
                         get_relocate_mode_v<InputValue> == relocate_t::maybe_trivial) {
        for (; n > 0; --n, (void)++_dest, (void)++_first) {
            wjr::construct_at(wjr::to_address(_dest), std::move(*_first));
            std::destroy_at(wjr::to_address(_first));
        }

        return wjr::_iter_wrap(dest, _dest);
    }

    _dest = wjr::uninitialized_move_n(_first, n, _dest);
    std::destroy_n(_first, n);
    return wjr::_iter_wrap(dest, _dest);
}

template <typename InputIt, typename Size, typename OutputIt>
constexpr OutputIt uninitialized_relocate_n_restrict(InputIt first, Size n, OutputIt dest) {
    return wjr::_iter_wrap(dest, uninitialized_relocate(_add_restrict(wjr::_iter_base(first)), n,
                                                        _add_restrict(wjr::_iter_base(dest))));
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
constexpr OutputIt uninitialized_relocate_n_using_allocator(InputIt first, Size n, OutputIt dest,
                                                            Alloc &alloc) {
    using InputValue = iterator_value_t<InputIt>;
    using OutputValue = iterator_value_t<OutputIt>;

    auto _first = wjr::_iter_base(first);
    auto _dest = wjr::_iter_base(dest);

    if constexpr (is_trivially_allocator_construct_v<
                      Alloc, OutputValue,
                      std::remove_reference_t<iterator_reference_t<InputIt>> &&> &&
                  is_trivially_allocator_destroy_v<Alloc, iterator_category_t<InputIt>>) {
        return wjr::_iter_wrap(dest, uninitialized_relocate_n(_first, n, _dest));
    } else if constexpr (std::is_same_v<InputValue, OutputValue> &&
                         get_relocate_mode_v<InputValue> == relocate_t::maybe_trivial) {
        for (; n > 0; --n, (void)++_first, (void)++_dest) {
            uninitialized_construct_using_allocator(_dest, alloc, std::move(*_first));
            destroy_at_using_allocator(_first, alloc);
        }

        return wjr::_iter_wrap(dest, _dest);
    } else {
        _dest = uninitialized_move_n_using_allocator(_first, n, _dest, alloc);
        destroy_n_using_allocator(_first, n, alloc);
        return wjr::_iter_wrap(dest, _dest);
    }
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
constexpr OutputIt uninitialized_relocate_n_restrict_using_allocator(InputIt first, Size n,
                                                                     OutputIt dest, Alloc &alloc) {
    return wjr::_iter_wrap(dest, uninitialized_relocate_n_using_allocator(
                                     _add_restrict(wjr::_iter_base(first)), n,
                                     _add_restrict(wjr::_iter_base(dest)), alloc));
}

template <typename T>
struct _aligned_storage_t {
    T *operator&() noexcept { return static_cast<T *>(static_cast<void *>(this)); }
    const T *operator&() const noexcept { return static_cast<T *>(static_cast<void *>(this)); }

    alignas(T) std::byte buf[sizeof(T)];
};

template <typename T, bool CON, bool DES>
struct _uninitialized_base {
    _uninitialized_base(const _uninitialized_base &) = default;
    _uninitialized_base(_uninitialized_base &&) = default;
    _uninitialized_base &operator=(const _uninitialized_base &) = default;
    _uninitialized_base &operator=(_uninitialized_base &&) = default;

    _uninitialized_base()
    requires(CON)
    = default;

    constexpr _uninitialized_base() noexcept
    requires(!CON)
    {}

    template <typename... Args>
    constexpr _uninitialized_base(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : m_value(std::forward<Args>(args)...) {}

    constexpr _uninitialized_base(enable_default_constructor_t) noexcept {}

    ~_uninitialized_base()
    requires(DES)
    = default;

    constexpr ~_uninitialized_base() noexcept
    requires(!DES)
    {}

    union {
        T m_value;
        _aligned_storage_t<T> m_storage;
    };
};

template <typename T>
using _uninitialized_base_selector =
    _uninitialized_base<T, std::is_trivially_default_constructible_v<T>,
                        std::is_trivially_destructible_v<T>>;

template <typename T>
struct _uninitialized_control_base : _uninitialized_base_selector<T> {
private:
    using Mybase = _uninitialized_base_selector<T>;

public:
    using Mybase::Mybase;

    constexpr void _copy_construct(const _uninitialized_control_base &other) noexcept(
        std::is_nothrow_copy_constructible_v<T>)
    requires(std::is_copy_constructible_v<T>)
    {
        wjr::construct_at(std::addressof(this->m_value), other.m_value);
    }

    constexpr void _move_construct(_uninitialized_control_base &&other) noexcept(
        std::is_nothrow_move_constructible_v<T>)
    requires(std::is_move_constructible_v<T>)
    {
        wjr::construct_at(std::addressof(this->m_value), std::move(other.m_value));
    }

    constexpr void _copy_assign(const _uninitialized_control_base &other) noexcept(
        std::is_nothrow_copy_assignable_v<T>)
    requires(std::is_copy_assignable_v<T>)
    {
        this->m_value = other.m_value;
    }

    constexpr void
    _move_assign(_uninitialized_control_base &&other) noexcept(std::is_nothrow_move_assignable_v<T>)
    requires(std::is_move_assignable_v<T>)
    {
        this->m_value = std::move(other.m_value);
    }
};

template <typename T>
using _uninitialized_control_selector = control_special_members_base<
    _uninitialized_control_base<T>,
    std::is_trivially_copy_constructible_v<T> || !std::is_copy_constructible_v<T>,
    std::is_trivially_move_constructible_v<T> || !std::is_move_constructible_v<T>,
    std::is_trivially_copy_assignable_v<T> || !std::is_copy_assignable_v<T>,
    std::is_trivially_move_assignable_v<T> || !std::is_move_assignable_v<T>>;

template <typename T, bool Enable>
struct _uninitialized_enabler {
    using type = enable_special_members_base<true, true, false, false, false, false>;
};

template <typename T>
struct _uninitialized_enabler<T, true> {
    using type =
        enable_special_members_base<true, true, std::is_copy_constructible_v<T>,
                                    std::is_move_constructible_v<T>, std::is_copy_assignable_v<T>,
                                    std::is_move_assignable_v<T>>;
};

template <typename T, bool Enable>
using _uninitialized_enabler_t = typename _uninitialized_enabler<T, Enable>::type;

template <typename T, bool Enable>
class WJR_EMPTY_BASES _uninitialized : _uninitialized_control_selector<T>,
                                       _uninitialized_enabler_t<T, Enable> {
    using Mybase = _uninitialized_control_selector<T>;

public:
    using value_type = T;

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(_uninitialized);

    template <typename U>
    requires(!std::is_same_v<remove_cvref_t<U>, _uninitialized> && std::is_constructible_v<T, U>)
    constexpr _uninitialized(U &&u) noexcept(std::is_nothrow_constructible_v<T, U>)
        : Mybase(std::forward<U>(u)) {}

    template <typename... Args>
    requires(sizeof...(Args) > 1 && std::is_constructible_v<Mybase, Args...>)
    constexpr _uninitialized(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<Mybase, Args...>)
        : Mybase(std::forward<Args>(args)...) {}

    constexpr _uninitialized(default_construct_t) noexcept : Mybase() {}

    template <typename... Args>
    requires(std::is_constructible_v<T, Args...>)
    constexpr T &
    emplace(Args &&...args) noexcept(std::is_nothrow_constructible_v<Mybase, Args...>) {
        wjr::construct_at(get(), std::forward<Args>(args)...);
        return this->m_value;
    }

    constexpr void reset() noexcept(std::is_nothrow_destructible_v<T>) { std::destroy_at(get()); }

    constexpr T &operator*() & noexcept { return this->m_value; }
    constexpr const T &operator*() const & noexcept { return this->m_value; }
    constexpr T &&operator*() && noexcept { return static_cast<T &&>(this->operator*()); }
    constexpr const T &&operator*() const && noexcept {
        return static_cast<const T &&>(this->operator*());
    }

    constexpr T *get() noexcept { return std::addressof(this->m_value); }
    constexpr const T *get() const noexcept { return std::addressof(this->m_value); }

    constexpr T *operator->() noexcept { return get(); }
    constexpr const T *operator->() const noexcept { return get(); }
};

/**
 * @class uninitialized
 *
 * @details Uninitialized object. Make trivially constructible and destructible
 * of any type. At present, the detector for detecting the legality of states
 * under DEBUG has been removed.
 *
 */
template <typename T>
class uninitialized : public _uninitialized<T, false> {
    using Mybase = _uninitialized<T, false>;

public:
    using Mybase::Mybase;
};

/// @private
template <typename T, bool = true>
class _lazy_initialized_base : public _uninitialized<T, true> {
    using Mybase = _uninitialized<T, true>;

public:
    using Mybase::Mybase;
};

/// @private
template <typename T>
class _lazy_initialized_base<T, false> : public _uninitialized<T, true> {
    using Mybase = _uninitialized<T, true>;

public:
    using Mybase::Mybase;

    ~_lazy_initialized_base() noexcept(noexcept(this->reset())) { this->reset(); }
};

/// @private
template <typename T>
using lazy_initialized_base = _lazy_initialized_base<T, std::is_trivially_destructible_v<T>>;

template <typename T>
class lazy_initialized : public lazy_initialized_base<T> {
    using Mybase = lazy_initialized_base<T>;

public:
    using Mybase::Mybase;
};

} // namespace wjr

#endif // WJR_MEMORY_UNINITIALIZED_HPP__