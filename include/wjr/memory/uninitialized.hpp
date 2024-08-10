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
WJR_CONSTEXPR20 void
construct_at(T *ptr,
             Args &&...args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
    ::new (static_cast<void *>(ptr)) T(std::forward<Args>(args)...);
}

template <typename T>
WJR_CONSTEXPR20 void
construct_at(T *ptr, dctor_t) noexcept(std::is_nothrow_default_constructible_v<T>) {
    ::new (static_cast<void *>(ptr)) T;
}

template <typename Iter, typename Alloc, typename... Args>
WJR_CONSTEXPR20 void uninitialized_construct_using_allocator(Iter iter, Alloc &alloc,
                                                             Args &&...args) {
    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<Iter>,
                                                     Args...>) {
        wjr::construct_at(wjr::to_address(iter), std::forward<Args>(args)...);
    } else {
        std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(iter),
                                                std::forward<Args>(args)...);
    }
}

template <typename Iter, typename Alloc>
WJR_CONSTEXPR20 void uninitialized_construct_using_allocator(Iter iter, Alloc &alloc,
                                                             dctor_t) {
    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<Iter>>) {
        wjr::construct_at(wjr::to_address(iter), dctor);
    } else {
        std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(iter));
    }
}

template <typename InputIt, typename OutputIt>
WJR_CONSTEXPR20 OutputIt uninitialized_copy(InputIt first, InputIt last,
                                            OutputIt d_first) {
    return std::uninitialized_copy(to_contiguous_address(first),
                                   to_contiguous_address(last),
                                   to_contiguous_address(d_first));
}

template <typename InputIt, typename OutputIt>
WJR_CONSTEXPR20 OutputIt __uninitialized_copy_restrict_aux(
    add_restrict_t<InputIt> first, add_restrict_t<InputIt> last,
    add_restrict_t<OutputIt> d_first) {
    return std::uninitialized_copy(first, last, d_first);
}

template <typename InputIt, typename OutputIt>
WJR_CONSTEXPR20 OutputIt __uninitialized_copy_restrict_impl(InputIt first, InputIt last,
                                                            OutputIt d_first) {
    return __uninitialized_copy_restrict_aux<InputIt, OutputIt>(first, last, d_first);
}

template <typename InputIt, typename OutputIt>
WJR_CONSTEXPR20 OutputIt uninitialized_copy_restrict(InputIt first, InputIt last,
                                                     OutputIt d_first) {
    const auto __first = to_contiguous_address(first);
    const auto __last = to_contiguous_address(last);
    if constexpr (is_contiguous_iterator_v<OutputIt>) {
        const auto __d_first = wjr::to_address(d_first);
        const auto __d_last =
            __uninitialized_copy_restrict_impl(__first, __last, __d_first);
        return std::next(d_first, std::distance(__d_first, __d_last));
    } else {
        return __uninitialized_copy_restrict_impl(__first, __last, d_first);
    }
}

template <typename InputIt, typename Size, typename OutputIt>
WJR_CONSTEXPR20 OutputIt uninitialized_copy_n(InputIt first, Size n, OutputIt d_first) {
    return std::uninitialized_copy_n(to_contiguous_address(first), n,
                                     to_contiguous_address(d_first));
}

template <typename InputIt, typename Size, typename OutputIt>
WJR_CONSTEXPR20 OutputIt __uninitialized_copy_n_restrict_aux(
    add_restrict_t<InputIt> first, Size n, add_restrict_t<OutputIt> d_first) {
    return std::uninitialized_copy_n(first, n, d_first);
}

template <typename InputIt, typename Size, typename OutputIt>
WJR_CONSTEXPR20 OutputIt __uninitialized_copy_n_restrict_impl(InputIt first, Size n,
                                                              OutputIt d_first) {
    return __uninitialized_copy_n_restrict_aux<InputIt, Size, OutputIt>(first, n,
                                                                        d_first);
}

template <typename InputIt, typename Size, typename OutputIt>
WJR_CONSTEXPR20 OutputIt uninitialized_copy_n_restrict(InputIt first, Size n,
                                                       OutputIt d_first) {
    const auto __first = to_contiguous_address(first);
    if constexpr (is_contiguous_iterator_v<OutputIt>) {
        const auto __d_first = wjr::to_address(d_first);
        const auto __d_last = __uninitialized_copy_n_restrict_impl(__first, n, __d_first);
        return std::next(d_first, std::distance(__d_first, __d_last));
    } else {
        return __uninitialized_copy_n_restrict_impl(__first, n, d_first);
    }
}

template <typename InputIt, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt uninitialized_copy_using_allocator(InputIt first, InputIt last,
                                                            OutputIt d_first,
                                                            Alloc &alloc) {
    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<OutputIt>,
                                                     iterator_reference_t<InputIt>>) {
        return wjr::uninitialized_copy(first, last, d_first);
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
WJR_CONSTEXPR20 OutputIt __uninitialized_copy_restrict_using_allocator_aux(
    add_restrict_t<InputIt> first, add_restrict_t<InputIt> last,
    add_restrict_t<OutputIt> d_first, Alloc &alloc) {
    return uninitialized_copy_using_allocator(first, last, d_first, alloc);
}

/// @private
template <typename InputIt, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt __uninitialized_copy_restrict_using_allocator_impl(
    InputIt first, InputIt last, OutputIt d_first, Alloc &alloc) {
    return __uninitialized_copy_restrict_using_allocator_aux<InputIt, OutputIt, Alloc>(
        first, last, d_first, alloc);
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
    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<OutputIt>,
                                                     iterator_reference_t<InputIt>>) {
        return wjr::uninitialized_copy_n(first, n, d_first);
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
WJR_CONSTEXPR20 OutputIt __uninitialized_copy_n_restrict_using_allocator_aux(
    add_restrict_t<InputIt> first, Size n, add_restrict_t<OutputIt> d_first,
    Alloc &alloc) {
    return uninitialized_copy_n_using_allocator(first, n, d_first, alloc);
}

/// @private
template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt __uninitialized_copy_n_restrict_using_allocator_impl(
    InputIt first, Size n, OutputIt d_first, Alloc &alloc) {
    return __uninitialized_copy_n_restrict_using_allocator_aux<InputIt, Size, OutputIt,
                                                               Alloc>(first, n, d_first,
                                                                      alloc);
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

template <typename InputIt, typename OutputIt>
WJR_CONSTEXPR20 OutputIt uninitialized_move(InputIt first, InputIt last,
                                            OutputIt d_first) {
    return wjr::uninitialized_copy(std::make_move_iterator(first),
                                   std::make_move_iterator(last), d_first);
}

template <typename InputIt, typename OutputIt>
WJR_CONSTEXPR20 OutputIt uninitialized_move_restrict(InputIt first, InputIt last,
                                                     OutputIt d_first) {
    return uninitialized_copy_restrict(std::make_move_iterator(first),
                                       std::make_move_iterator(last), d_first);
}

template <typename InputIt, typename Size, typename OutputIt>
WJR_CONSTEXPR20 OutputIt uninitialized_move_n(InputIt first, Size n, OutputIt d_first) {
    return wjr::uninitialized_copy_n(std::make_move_iterator(first), n, d_first);
}

template <typename InputIt, typename Size, typename OutputIt>
WJR_CONSTEXPR20 OutputIt uninitialized_move_n_restrict(InputIt first, Size n,
                                                       OutputIt d_first) {
    return uninitialized_copy_n_restrict(std::make_move_iterator(first), n, d_first);
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
    using value_type = iterator_value_t<Iter>;
    if constexpr (is_trivially_allocator_construct_v<Alloc, value_type>) {
        std::uninitialized_default_construct(to_contiguous_address(first),
                                             to_contiguous_address(last));
    } else {
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
    using value_type = iterator_value_t<Iter>;
    if constexpr (is_trivially_allocator_construct_v<Alloc, value_type>) {
        std::uninitialized_default_construct_n(to_contiguous_address(first), n);
    } else {
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
    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<Iter>>) {
        std::uninitialized_value_construct(to_contiguous_address(first),
                                           to_contiguous_address(last));
    } else {
        for (; first != last; ++first) {
            std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(first));
        }
    }
}

template <typename Iter, typename Size, typename Alloc>
WJR_CONSTEXPR20 void uninitialized_value_construct_n_using_allocator(Iter first, Size n,
                                                                     Alloc &alloc) {
    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<Iter>>) {
        std::uninitialized_value_construct_n(to_contiguous_address(first), n);
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
        if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<Iter>,
                                                         const T &>) {
            std::uninitialized_fill(to_contiguous_address(first),
                                    to_contiguous_address(last), value);
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
        if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<Iter>,
                                                         const T &>) {
            std::uninitialized_fill_n(to_contiguous_address(first), n, value);
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
    if constexpr (is_trivially_allocator_destroy_v<Alloc, iterator_value_t<Iter>>) {
        std::destroy_at(wjr::to_address(iter));
    } else {
        std::allocator_traits<Alloc>::destroy(alloc, wjr::to_address(iter));
    }
}

template <typename Iter, typename Alloc>
WJR_CONSTEXPR20 void destroy_using_allocator(Iter first, Iter last, Alloc &alloc) {
    if constexpr (is_trivially_allocator_destroy_v<Alloc, iterator_value_t<Iter>>) {
        std::destroy(first, last);
    } else {
        for (; first != last; ++first) {
            std::allocator_traits<Alloc>::destroy(alloc, wjr::to_address(first));
        }
    }
}

template <typename Iter, typename Size, typename Alloc>
WJR_CONSTEXPR20 void destroy_n_using_allocator(Iter first, Size n, Alloc &alloc) {
    if constexpr (is_trivially_allocator_destroy_v<Alloc, iterator_value_t<Iter>>) {
        std::destroy_n(first, n);
    } else {
        for (; n > 0; ++first, --n) {
            std::allocator_traits<Alloc>::destroy(alloc, wjr::to_address(first));
        }
    }
}

template <typename T>
T *relocate_at(T *src, T *dst) noexcept(
    std::is_nothrow_move_constructible_v<T> &&std::is_nothrow_destructible_v<T>) {
    if constexpr (get_relocate_mode_v<T> == relocate_t::trivial) {
        std::memcpy(dst, src, sizeof(T));
    } else {
        wjr::construct_at(dst, std::move(*src));
        std::destroy_at(src);
    }

    return dst;
}

template <typename T, typename Alloc>
T *uninitialized_relocate_at_using_allocator(T *src, T *dst, Alloc &alloc) noexcept(
    std::is_nothrow_move_constructible_v<T> &&std::is_nothrow_destructible_v<T>) {
    if constexpr (is_trivially_allocator_v<Alloc>) {
        return relocate_at(src, dst);
    } else {
        wjr::uninitialized_construct_using_allocator(dst, alloc, std::move(*src));
        std::destroy_at(src);
    }

    return dst;
}

template <typename InputIt, typename OutputIt>
OutputIt uninitialized_relocate(InputIt first, InputIt last, OutputIt d_first) {
    using InputValue = iterator_value_t<InputIt>;
    using OutputValue = iterator_value_t<OutputIt>;

    if constexpr (std::is_same_v<InputValue, OutputValue> &&
                  is_contiguous_iterator_v<InputIt> &&
                  is_contiguous_iterator_v<OutputIt> &&
                  get_relocate_mode_v<InputValue> == relocate_t::trivial) {
        const auto __first = wjr::to_address(first);
        const auto n = wjr::to_address(last) - __first;
        const auto __d_first = wjr::to_address(d_first);
        std::memcpy(__d_first, __first, n * sizeof(InputValue));
        return std::next(d_first, n);
    } else if constexpr (std::is_same_v<InputValue, OutputValue> &&
                         get_relocate_mode_v<InputValue> == relocate_t::maybe_trivial) {
        for (; first != last; ++first, ++d_first) {
            wjr::construct_at(wjr::to_address(d_first), std::move(*first));
            std::destroy_at(wjr::to_address(first));
        }

        return d_first;
    } else {
        d_first = wjr::uninitialized_move(first, last, d_first);
        std::destroy(first, last);
        return d_first;
    }
}

template <typename InputIt, typename OutputIt>
OutputIt __uninitialized_relocate_restrict_aux(add_restrict_t<InputIt> first,
                                               add_restrict_t<InputIt> last,
                                               add_restrict_t<OutputIt> d_first) {
    return uninitialized_relocate(first, last, d_first);
}

template <typename InputIt, typename OutputIt>
OutputIt __uninitialized_relocate_restrict_impl(InputIt first, InputIt last,
                                                OutputIt d_first) {
    return __uninitialized_relocate_restrict_aux<InputIt, OutputIt>(first, last, d_first);
}

template <typename InputIt, typename OutputIt>
OutputIt uninitialized_relocate_restrict(InputIt first, InputIt last, OutputIt d_first) {
    const auto __first = to_contiguous_address(first);
    const auto __last = to_contiguous_address(last);
    if constexpr (is_contiguous_iterator_v<OutputIt>) {
        const auto __d_first = wjr::to_address(d_first);
        const auto __d_last =
            __uninitialized_relocate_restrict_impl(__first, __last, __d_first);
        return std::next(d_first, std::distance(__d_first, __d_last));
    } else {
        return __uninitialized_relocate_restrict_impl(__first, __last, d_first);
    }
}

template <typename InputIt, typename OutputIt, typename Alloc>
OutputIt uninitialized_relocate_using_allocator(InputIt first, InputIt last,
                                                OutputIt d_first, Alloc &alloc) {
    using InputValue = iterator_value_t<InputIt>;
    using OutputValue = iterator_value_t<OutputIt>;

    if constexpr (is_trivially_allocator_construct_v<
                      Alloc, OutputValue,
                      std::remove_reference_t<iterator_reference_t<InputIt>> &&> &&
                  is_trivially_allocator_destroy_v<Alloc, iterator_category_t<InputIt>>) {
        return uninitialized_relocate(first, last, d_first);
    } else if constexpr (std::is_same_v<InputValue, OutputValue> &&
                         get_relocate_mode_v<InputValue> == relocate_t::maybe_trivial) {
        for (; first != last; ++first, ++d_first) {
            wjr::construct_at(wjr::to_address(d_first), std::move(*first));
            std::destroy_at(wjr::to_address(first));
        }

        return d_first;
    } else {
        d_first = uninitialized_move_using_allocator(first, last, d_first);
        std::destroy(first, last);
        return d_first;
    }
}

template <typename InputIt, typename OutputIt, typename Alloc>
OutputIt __uninitialized_relocate_restrict_using_allocator_aux(
    add_restrict_t<InputIt> first, add_restrict_t<InputIt> last,
    add_restrict_t<OutputIt> d_first, Alloc &alloc) {
    return uninitialized_relocate_using_allocator(first, last, d_first, alloc);
}

template <typename InputIt, typename OutputIt, typename Alloc>
OutputIt
__uninitialized_relocate_restrict_using_allocator_impl(InputIt first, InputIt last,
                                                       OutputIt d_first, Alloc &alloc) {
    return __uninitialized_relocate_restrict_using_allocator_aux<InputIt, OutputIt,
                                                                 Alloc>(first, last,
                                                                        d_first, alloc);
}

template <typename InputIt, typename OutputIt, typename Alloc>
OutputIt uninitialized_relocate_restrict_using_allocator(InputIt first, InputIt last,
                                                         OutputIt d_first, Alloc &alloc) {
    const auto __first = to_contiguous_address(first);
    const auto __last = to_contiguous_address(last);
    if constexpr (is_contiguous_iterator_v<OutputIt>) {
        const auto __d_first = wjr::to_address(d_first);
        const auto __d_last = __uninitialized_relocate_restrict_using_allocator_impl(
            __first, __last, __d_first, alloc);
        return std::next(d_first, std::distance(__d_first, __d_last));
    } else {
        return __uninitialized_relocate_restrict_using_allocator_impl(__first, __last,
                                                                      d_first, alloc);
    }
}

template <typename InputIt, typename Size, typename OutputIt>
OutputIt uninitialized_relocate_n(InputIt first, Size n, OutputIt d_first) {
    using InputValue = iterator_value_t<InputIt>;
    using OutputValue = iterator_value_t<OutputIt>;

    if constexpr (std::is_same_v<InputValue, OutputValue> &&
                  is_contiguous_iterator_v<InputIt> &&
                  is_contiguous_iterator_v<OutputIt> &&
                  get_relocate_mode_v<InputValue> == relocate_t::trivial) {
        const auto __first = wjr::to_address(first);
        const auto __d_first = wjr::to_address(d_first);
        std::memcpy(__d_first, __first, n * sizeof(InputValue));
        return std::next(d_first, n);
    } else if constexpr (std::is_same_v<InputValue, OutputValue> &&
                         get_relocate_mode_v<InputValue> == relocate_t::maybe_trivial) {
        for (; n > 0; ++first, ++d_first, --n) {
            wjr::construct_at(wjr::to_address(d_first), std::move(*first));
            std::destroy_at(wjr::to_address(first));
        }

        return d_first;
    } else {
        d_first = wjr::uninitialized_move_n(first, n, d_first);
        std::destroy_n(first, n);
        return d_first;
    }
}

template <typename InputIt, typename Size, typename OutputIt>
OutputIt __uninitialized_relocate_n_restrict_aux(add_restrict_t<InputIt> first, Size n,
                                                 add_restrict_t<OutputIt> d_first) {
    return uninitialized_relocate(first, n, d_first);
}

template <typename InputIt, typename Size, typename OutputIt>
OutputIt __uninitialized_relocate_n_restrict_impl(InputIt first, Size n,
                                                  OutputIt d_first) {
    return __uninitialized_relocate_n_restrict_aux<InputIt, Size, OutputIt>(first, n,
                                                                            d_first);
}

template <typename InputIt, typename Size, typename OutputIt>
OutputIt uninitialized_relocate_n_restrict(InputIt first, Size n, OutputIt d_first) {
    const auto __first = to_contiguous_address(first);
    if constexpr (is_contiguous_iterator_v<OutputIt>) {
        const auto __d_first = wjr::to_address(d_first);
        const auto __d_last =
            __uninitialized_relocate_n_restrict_impl(__first, n, __d_first);
        return std::next(d_first, std::distance(__d_first, __d_last));
    } else {
        return __uninitialized_relocate_n_restrict_impl(__first, n, d_first);
    }
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
OutputIt uninitialized_relocate_n_using_allocator(InputIt first, Size n, OutputIt d_first,
                                                  Alloc &alloc) {
    using InputValue = iterator_value_t<InputIt>;
    using OutputValue = iterator_value_t<OutputIt>;

    if constexpr (is_trivially_allocator_construct_v<
                      Alloc, OutputValue,
                      std::remove_reference_t<iterator_reference_t<InputIt>> &&> &&
                  is_trivially_allocator_destroy_v<Alloc, iterator_category_t<InputIt>>) {
        return uninitialized_relocate_n(first, n, d_first);
    } else if constexpr (std::is_same_v<InputValue, OutputValue> &&
                         get_relocate_mode_v<InputValue> == relocate_t::maybe_trivial) {
        for (; n > 0; ++first, ++d_first, --n) {
            wjr::construct_at(wjr::to_address(d_first), std::move(*first));
            std::destroy_at(wjr::to_address(first));
        }

        return d_first;
    } else {
        d_first = uninitialized_move_n_using_allocator(first, n, d_first);
        std::destroy_n(first, n);
        return d_first;
    }
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
OutputIt __uninitialized_relocate_n_restrict_using_allocator_aux(
    add_restrict_t<InputIt> first, Size n, add_restrict_t<OutputIt> d_first,
    Alloc &alloc) {
    return uninitialized_relocate_n_using_allocator(first, n, d_first, alloc);
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
OutputIt __uninitialized_relocate_n_restrict_using_allocator_impl(InputIt first, Size n,
                                                                  OutputIt d_first,
                                                                  Alloc &alloc) {
    return __uninitialized_relocate_n_restrict_using_allocator_aux<InputIt, Size,
                                                                   OutputIt, Alloc>(
        first, n, d_first, alloc);
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
OutputIt uninitialized_relocate_n_restrict_using_allocator(InputIt first, Size n,
                                                           OutputIt d_first,
                                                           Alloc &alloc) {
    const auto __first = to_contiguous_address(first);
    if constexpr (is_contiguous_iterator_v<OutputIt>) {
        const auto __d_first = wjr::to_address(d_first);
        const auto __d_last = __uninitialized_relocate_n_restrict_using_allocator_impl(
            __first, n, __d_first, alloc);
        return std::next(d_first, std::distance(__d_first, __d_last));
    } else {
        return __uninitialized_relocate_n_restrict_using_allocator_impl(__first, n,
                                                                        d_first, alloc);
    }
}

template <typename T, bool Constructor, bool Destructor>
struct __uninitialized_base;

template <typename T>
struct __aligned_storage_t {
    alignas(T) char buf[sizeof(T)];
};

#define WJR_REGISTER_UNION_BASE(CON, DES)                                                \
    template <typename T>                                                                \
    struct __uninitialized_base<T, CON, DES> {                                           \
        __uninitialized_base(const __uninitialized_base &) = default;                    \
        __uninitialized_base(__uninitialized_base &&) = default;                         \
        __uninitialized_base &operator=(const __uninitialized_base &) = default;         \
        __uninitialized_base &operator=(__uninitialized_base &&) = default;              \
                                                                                         \
        constexpr __uninitialized_base() WJR_PP_BOOL_IF(CON, = default, noexcept {});    \
                                                                                         \
        template <typename... Args>                                                      \
        constexpr __uninitialized_base(Args &&...args) noexcept(                         \
            std::is_nothrow_constructible_v<T, Args...>)                                 \
            : m_value(std::forward<Args>(args)...) {}                                    \
                                                                                         \
        constexpr __uninitialized_base(enable_default_constructor_t) noexcept {}         \
                                                                                         \
        ~__uninitialized_base() WJR_PP_BOOL_IF(DES, = default, noexcept {});             \
                                                                                         \
        union {                                                                          \
            T m_value;                                                                   \
            __aligned_storage_t<T> m_storage;                                            \
        };                                                                               \
    }

WJR_REGISTER_UNION_BASE(0, 0);
WJR_REGISTER_UNION_BASE(0, 1);
WJR_REGISTER_UNION_BASE(1, 0);
WJR_REGISTER_UNION_BASE(1, 1);

#undef WJR_REGISTER_UNION_BASE

template <typename T>
using __uninitialized_base_selector =
    __uninitialized_base<T, std::is_trivially_default_constructible_v<T>,
                         std::is_trivially_destructible_v<T>>;

template <typename T>
struct __uninitialized_control_base : __uninitialized_base_selector<T> {
    using Mybase = __uninitialized_base_selector<T>;
    using Mybase::Mybase;

    template <typename U = T, WJR_REQUIRES(std::is_copy_constructible_v<U>)>
    WJR_CONSTEXPR20 void
    __copy_construct(const __uninitialized_control_base &other) noexcept(
        std::is_nothrow_copy_constructible_v<T>) {
        wjr::construct_at(std::addressof(this->m_value), other.m_value);
    }

    template <typename U = T, WJR_REQUIRES(std::is_move_constructible_v<U>)>
    WJR_CONSTEXPR20 void __move_construct(__uninitialized_control_base &&other) noexcept(
        std::is_nothrow_move_constructible_v<T>) {
        wjr::construct_at(std::addressof(this->m_value), std::move(other.m_value));
    }
};

template <typename T>
using __uninitialized_control_selector = control_special_members_base<
    __uninitialized_control_base<T>,
    std::is_trivially_copy_constructible_v<T> || !std::is_copy_constructible_v<T>,
    std::is_trivially_move_constructible_v<T> || !std::is_move_constructible_v<T>, true,
    true>;

template <typename T>
using __uninitialized_enabler =
    enable_special_members_base<true, true, std::is_copy_constructible_v<T>,
                                std::is_move_constructible_v<T>, false, false>;

/**
 * @class uninitialized
 *
 * @details Uninitialized object. Make trivially constructible and destructible of
 * any type. At present, the detector for detecting the legality of states under DEBUG has
 * been removed.
 *
 */
template <typename T>
class WJR_EMPTY_BASES uninitialized : __uninitialized_control_selector<T>,
                                      __uninitialized_enabler<T> {
    using Mybase = __uninitialized_control_selector<T>;

public:
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(uninitialized);

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<Mybase, Args...>)>
    constexpr uninitialized(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<Mybase, Args...>)
        : Mybase(std::forward<Args>(args)...) {}

    constexpr uninitialized(dctor_t) noexcept : Mybase() {}

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<T, Args...>)>
    constexpr T &
    emplace(Args &&...args) noexcept(std::is_nothrow_constructible_v<Mybase, Args...>) {
        wjr::construct_at(get(), std::forward<Args>(args)...);
        return this->m_value;
    }

    constexpr void reset() noexcept(std::is_nothrow_destructible_v<T>) {
        std::destroy_at(get());
    }

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

/// @private
template <typename T, bool = true>
class __lazy_initialized_base : public uninitialized<T> {
    using Mybase = uninitialized<T>;

public:
    using Mybase::Mybase;
};

/// @private
template <typename T>
class __lazy_initialized_base<T, false> : public uninitialized<T> {
    using Mybase = uninitialized<T>;

public:
    using Mybase::Mybase;

    ~__lazy_initialized_base() noexcept(noexcept(Mybase::reset())) { Mybase::reset(); }
};

/// @private
template <typename T>
using lazy_initialized_base = __lazy_initialized_base<T, false>;

template <typename T>
class lazy_initialized : public lazy_initialized_base<T> {
    using Mybase = lazy_initialized_base<T>;

public:
    using Mybase::Mybase;
};

} // namespace wjr

#endif // WJR_MEMORY_UNINITIALIZED_HPP__