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
        ::new (static_cast<void *>(to_address(iter)))
            value_type(std::forward<Args>(args)...);
    } else {
        std::allocator_traits<Alloc>::construct(alloc, to_address(iter),
                                                std::forward<Args>(args)...);
    }
}

template <typename InputIter, typename OutputIter, typename Alloc>
WJR_CONSTEXPR20 OutputIter uninitialized_copy_using_allocator(InputIter first,
                                                              InputIter last,
                                                              OutputIter result,
                                                              Alloc &alloc) {
    if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
        return std::uninitialized_copy(first, last, result);
    } else {
        for (; first != last; ++first, ++result) {
            std::allocator_traits<Alloc>::construct(alloc, to_address(result), *first);
        }
        return result;
    }
}

template <typename InputIter, typename Size, typename OutputIter, typename Alloc>
WJR_CONSTEXPR20 OutputIter uninitialized_copy_n_using_allocator(InputIter first, Size n,
                                                                OutputIter result,
                                                                Alloc &alloc) {
    if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
        return std::uninitialized_copy_n(first, n, result);
    } else {
        for (; n > 0; ++first, ++result, --n) {
            std::allocator_traits<Alloc>::construct(alloc, to_address(result), *first);
        }
        return result;
    }
}

template <typename InputIter, typename OutputIter, typename Alloc>
WJR_CONSTEXPR20 OutputIter uninitialized_move_using_allocator(InputIter first,
                                                              InputIter last,
                                                              OutputIter result,
                                                              Alloc &alloc) {
    return uninitialized_copy_using_allocator(
        std::make_move_iterator(first), std::make_move_iterator(last), result, alloc);
}

template <typename InputIter, typename Size, typename OutputIter, typename Alloc>
WJR_CONSTEXPR20 OutputIter uninitialized_move_n_using_allocator(InputIter first, Size n,
                                                                OutputIter result,
                                                                Alloc &alloc) {
    return uninitialized_copy_n_using_allocator(std::make_move_iterator(first), n, result,
                                                alloc);
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
                std::allocator_traits<Alloc>::construct(alloc, to_address(first),
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
                std::allocator_traits<Alloc>::construct(alloc, to_address(first),
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
            std::allocator_traits<Alloc>::construct(alloc, to_address(first),
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
            std::allocator_traits<Alloc>::construct(alloc, to_address(first),
                                                    value_type());
        }
    }
}

/**
 * @brief Fill the range [first, last) with value using allocator.
 *
 * @tparam T The value type. Use `in_place_default_construct_t` to default construct the
 * elements. Use `in_place_value_construct_t` to value construct the elements.
 */
template <typename Iter, typename Alloc, typename T>
WJR_CONSTEXPR20 void uninitialized_fill_using_allocator(Iter first, Iter last,
                                                        Alloc &alloc, const T &value) {
    if constexpr (std::is_same_v<T, in_place_default_construct_t>) {
        uninitialized_default_construct_using_allocator(first, last, alloc);
    } else if constexpr (std::is_same_v<T, in_place_value_construct_t>) {
        uninitialized_value_construct_using_allocator(first, last, alloc);
    } else {
        if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
            std::uninitialized_fill(first, last, value);
        } else {
            for (; first != last; ++first) {
                std::allocator_traits<Alloc>::construct(alloc, to_address(first), value);
            }
        }
    }
}

/**
 * @brief Fill the range [first, first + n) with value using allocator.
 *
 * @tparam T The value type. Use `in_place_default_construct_t` to default construct the
 * elements. Use `in_place_value_construct_t` to value construct the elements.
 */
template <typename Iter, typename Size, typename Alloc, typename T>
WJR_CONSTEXPR20 void uninitialized_fill_n_using_allocator(Iter first, Size n,
                                                          Alloc &alloc, const T &value) {
    if constexpr (std::is_same_v<T, in_place_default_construct_t>) {
        uninitialized_default_construct_n_using_allocator(first, n, alloc);
    } else if constexpr (std::is_same_v<T, in_place_value_construct_t>) {
        uninitialized_value_construct_n_using_allocator(first, n, alloc);
    } else {
        if constexpr (is_trivially_allocator_constructible_v<Alloc>) {
            std::uninitialized_fill_n(first, n, value);
        } else {
            for (; n > 0; ++first, --n) {
                std::allocator_traits<Alloc>::construct(alloc, to_address(first), value);
            }
        }
    }
}

template <typename Iter, typename Alloc>
WJR_CONSTEXPR20 void destroy_at_using_allocator(Iter iter, Alloc &alloc) {
    if constexpr (is_trivially_allocator_destructible_v<Alloc>) {
        std::destroy_at(to_address(iter));
    } else {
        std::allocator_traits<Alloc>::destroy(alloc, to_address(iter));
    }
}

template <typename Iter, typename Alloc>
WJR_CONSTEXPR20 void destroy_using_allocator(Iter first, Iter last, Alloc &alloc) {
    if constexpr (is_trivially_allocator_destructible_v<Alloc>) {
        std::destroy(first, last);
    } else {
        for (; first != last; ++first) {
            std::allocator_traits<Alloc>::destroy(alloc, to_address(first));
        }
    }
}

template <typename Iter, typename Size, typename Alloc>
WJR_CONSTEXPR20 void destroy_n_using_allocator(Iter first, Size n, Alloc &alloc) {
    if constexpr (is_trivially_allocator_v<Alloc>) {
        std::destroy_n(first, n);
    } else {
        for (; n > 0; ++first, --n) {
            std::allocator_traits<Alloc>::destroy(alloc, to_address(first));
        }
    }
}

} // namespace wjr

#endif // WJR_MEMORY_UNINITIALIZED_HPP__