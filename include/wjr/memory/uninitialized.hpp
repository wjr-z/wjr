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

#include <wjr/crtp/trivial_allocator_base.hpp>
#include <wjr/memory/details.hpp>

namespace wjr {

template <typename Iter, typename Alloc, typename... Args>
WJR_CONSTEXPR20 void uninitialized_construct_using_allocator(Iter iter, Alloc &alloc,
                                                             Args &&...args) {
    if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
        using value_type = typename std::iterator_traits<Iter>::value_type;
        ::new (static_cast<void *>((to_address)(iter)))
            value_type(std::forward<Args>(args)...);
    } else {
        std::allocator_traits<Alloc>::construct(alloc, (to_address)(iter),
                                                std::forward<Args>(args)...);
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
        using value_type = typename std::iterator_traits<Iter>::value_type;
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
        using value_type = typename std::iterator_traits<Iter>::value_type;
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
        using value_type = typename std::iterator_traits<Iter>::value_type;
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
        using value_type = typename std::iterator_traits<Iter>::value_type;
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
                std::allocator_traits<Alloc>::construct(alloc, (to_address)(first), value);
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
                std::allocator_traits<Alloc>::construct(alloc, (to_address)(first), value);
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

} // namespace wjr

#endif // WJR_MEMORY_UNINITIALIZED_HPP__