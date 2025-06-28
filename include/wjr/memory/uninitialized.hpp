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
construct_at(T *ptr, Args &&...args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
    ::new (static_cast<void *>(ptr)) T(std::forward<Args>(args)...);
}

template <typename T, WJR_REQUIRES(!std::is_constructible_v<T, default_construct_t>)>
WJR_CONSTEXPR20 void
construct_at(T *ptr, default_construct_t) noexcept(std::is_nothrow_default_constructible_v<T>) {
    ::new (static_cast<void *>(ptr)) T;
}

template <typename Iter, typename Alloc, typename... Args>
WJR_CONSTEXPR20 void uninitialized_construct_using_allocator(Iter iter, Alloc &alloc,
                                                             Args &&...args) {
    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<Iter>, Args...>) {
        wjr::construct_at(wjr::to_address(iter), std::forward<Args>(args)...);
    } else {
        std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(iter),
                                                std::forward<Args>(args)...);
    }
}

template <typename Iter, typename Alloc>
WJR_CONSTEXPR20 void uninitialized_construct_using_allocator(Iter iter, Alloc &alloc,
                                                             default_construct_t) {
    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<Iter>>) {
        wjr::construct_at(wjr::to_address(iter), default_construct);
    } else {
        std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(iter));
    }
}

template <typename InputItInit, typename InputItSe, typename OutputIt>
WJR_CONSTEXPR20 OutputIt uninitialized_copy(InputItInit first, InputItSe last, OutputIt dest) {
    auto __first = wjr::__iter_base(first);
    auto __last = wjr::__iter_base(last);
    auto __dest = wjr::__iter_base(dest);
    if constexpr (std::is_same_v<InputItInit, InputItSe>) {
        return wjr::__iter_wrap(dest, std::uninitialized_copy(__first, __last, __dest));
    } else {
        for (; __first != __last; ++__dest, (void)++__first) {
            wjr::construct_at(wjr::to_address(__dest), *__first);
        }

        return wjr::__iter_wrap(dest, __dest);
    }
}

template <typename InputItInit, typename InputItSe, typename OutputIt>
WJR_CONSTEXPR20 OutputIt uninitialized_copy_restrict(InputItInit first, InputItSe last,
                                                     OutputIt dest) {
    return wjr::__iter_wrap(dest, wjr::uninitialized_copy(__add_restrict(wjr::__iter_base(first)),
                                                          __add_restrict(wjr::__iter_base(last)),
                                                          __add_restrict(wjr::__iter_base(dest))));
}

template <typename InputIt, typename Size, typename OutputIt>
WJR_CONSTEXPR20 OutputIt uninitialized_copy_n(InputIt first, Size n, OutputIt dest) {
    return wjr::__iter_wrap(
        dest, std::uninitialized_copy_n(wjr::__iter_base(first), n, wjr::__iter_base(dest)));
}

template <typename InputIt, typename Size, typename OutputIt>
WJR_CONSTEXPR20 OutputIt uninitialized_copy_n_restrict(InputIt first, Size n, OutputIt dest) {
    return wjr::__iter_wrap(dest,
                            std::uninitialized_copy_n(__add_restrict(wjr::__iter_base(first)), n,
                                                      __add_restrict(wjr::__iter_base(dest))));
}

template <typename InputItInit, typename InputItSe, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt uninitialized_copy_using_allocator(InputItInit first, InputItSe last,
                                                            OutputIt dest, Alloc &alloc) {
    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<OutputIt>,
                                                     iterator_reference_t<InputItInit>>) {
        return wjr::uninitialized_copy(first, last, dest);
    } else {
        auto __first = wjr::__iter_base(first);
        auto __last = wjr::__iter_base(last);
        auto __dest = wjr::__iter_base(dest);
        for (; __first != __last; ++__dest, (void)++__first) {
            std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(__dest), *first);
        }
        return wjr::__iter_wrap(dest, __dest);
    }
}

template <typename InputItInit, typename InputItSe, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt uninitialized_copy_restrict_using_allocator(InputItInit first,
                                                                     InputItSe last, OutputIt dest,
                                                                     Alloc &alloc) {
    return wjr::__iter_wrap(
        dest, uninitialized_copy_using_allocator(__add_restrict(wjr::__iter_base(first)),
                                                 __add_restrict(wjr::__iter_base(last)),
                                                 __add_restrict(wjr::__iter_base(dest)), alloc));
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt uninitialized_copy_n_using_allocator(InputIt first, Size n, OutputIt dest,
                                                              Alloc &alloc) {
    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<OutputIt>,
                                                     iterator_reference_t<InputIt>>) {
        return wjr::uninitialized_copy_n(first, n, dest);
    } else {
        auto __first = wjr::__iter_base(first);
        auto __dest = wjr::__iter_base(dest);
        for (; n > 0; --n, (void)++__dest, (void)++__first) {
            std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(__dest), *__first);
        }
        return wjr::__iter_wrap(dest, __dest);
    }
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt uninitialized_copy_n_restrict_using_allocator(InputIt first, Size n,
                                                                       OutputIt dest,
                                                                       Alloc &alloc) {
    return wjr::__iter_wrap(
        dest, uninitialized_copy_n_using_allocator(__add_restrict(wjr::__iter_base(first)), n,
                                                   __add_restrict(wjr::__iter_base(dest)), alloc));
}

template <typename InputItInit, typename InputItSe, typename OutputIt>
WJR_CONSTEXPR20 OutputIt uninitialized_move(InputItInit first, InputItSe last, OutputIt dest) {
    return wjr::uninitialized_copy(std::make_move_iterator(first), std::make_move_iterator(last),
                                   dest);
}

template <typename InputItInit, typename InputItSe, typename OutputIt>
WJR_CONSTEXPR20 OutputIt uninitialized_move_restrict(InputItInit first, InputItSe last,
                                                     OutputIt dest) {
    return uninitialized_copy_restrict(std::make_move_iterator(first),
                                       std::make_move_iterator(last), dest);
}

template <typename InputIt, typename Size, typename OutputIt>
WJR_CONSTEXPR20 OutputIt uninitialized_move_n(InputIt first, Size n, OutputIt dest) {
    return wjr::uninitialized_copy_n(std::make_move_iterator(first), n, dest);
}

template <typename InputIt, typename Size, typename OutputIt>
WJR_CONSTEXPR20 OutputIt uninitialized_move_n_restrict(InputIt first, Size n, OutputIt dest) {
    return uninitialized_copy_n_restrict(std::make_move_iterator(first), n, dest);
}

template <typename InputItnit, typename InputItSe, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt uninitialized_move_using_allocator(InputItnit first, InputItSe last,
                                                            OutputIt dest, Alloc &alloc) {
    return uninitialized_copy_using_allocator(std::make_move_iterator(first),
                                              std::make_move_iterator(last), dest, alloc);
}

template <typename InputItInit, typename InputItSe, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt uninitialized_move_restrict_using_allocator(InputItInit first,
                                                                     InputItSe last, OutputIt dest,
                                                                     Alloc &alloc) {
    return uninitialized_copy_restrict_using_allocator(std::make_move_iterator(first),
                                                       std::make_move_iterator(last), dest, alloc);
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt uninitialized_move_n_using_allocator(InputIt first, Size n, OutputIt dest,
                                                              Alloc &alloc) {
    return uninitialized_copy_n_using_allocator(std::make_move_iterator(first), n, dest, alloc);
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
WJR_CONSTEXPR20 OutputIt uninitialized_move_n_restrict_using_allocator(InputIt first, Size n,
                                                                       OutputIt dest,
                                                                       Alloc &alloc) {
    return uninitialized_copy_n_restrict_using_allocator(std::make_move_iterator(first), n, dest,
                                                         alloc);
}

template <typename IterInit, typename IterSe>
WJR_CONSTEXPR20 void uninitialized_default_construct(IterInit first, IterSe last) {
    auto __first = wjr::__iter_base(first);
    auto __last = wjr::__iter_base(last);
    if constexpr (std::is_same_v<IterInit, IterSe>) {
        std::uninitialized_default_construct(__first, __last);
    } else {
        for (; __first != __last; ++__first) {
            wjr::construct_at(wjr::to_address(__first), default_construct);
        }
    }
}

template <typename IterInit, typename IterSe, typename Alloc>
WJR_CONSTEXPR20 void uninitialized_default_construct_using_allocator(IterInit first, IterSe last,
                                                                     Alloc &alloc) {
    using value_type = iterator_value_t<IterInit>;
    WJR_MAYBE_UNUSED auto __first = wjr::__iter_base(first);
    WJR_MAYBE_UNUSED auto __last = wjr::__iter_base(last);
    if constexpr (is_trivially_allocator_construct_v<Alloc, value_type>) {
        wjr::uninitialized_default_construct(__first, __last);
    } else {
        if constexpr (!std::is_trivially_default_constructible_v<value_type>) {
            for (; __first != __last; ++__first) {
                std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(__first));
            }
        }
    }
}

template <typename Iter, typename Size, typename Alloc>
WJR_CONSTEXPR20 void uninitialized_default_construct_n_using_allocator(Iter first, Size n,
                                                                       Alloc &alloc) {
    using value_type = iterator_value_t<Iter>;
    auto __first = wjr::__iter_base(first);
    if constexpr (is_trivially_allocator_construct_v<Alloc, value_type>) {
        std::uninitialized_default_construct_n(__first, n);
    } else {
        if constexpr (!std::is_trivially_default_constructible_v<value_type>) {
            for (; n > 0; ++__first, --n) {
                std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(__first),
                                                        value_type());
            }
        }
    }
}

template <typename IterInit, typename InitSe>
WJR_CONSTEXPR20 void uninitialized_value_construct(IterInit first, InitSe last) {
    auto __first = wjr::__iter_base(first);
    auto __last = wjr::__iter_base(last);
    if constexpr (std::is_same_v<IterInit, InitSe>) {
        std::uninitialized_value_construct(__first, __last);
    } else {
        for (; __first != __last; ++__first) {
            wjr::construct_at(wjr::to_address(__first));
        }
    }
}

template <typename IterInit, typename InitSe, typename Alloc>
WJR_CONSTEXPR20 void uninitialized_value_construct_using_allocator(IterInit first, InitSe last,
                                                                   Alloc &alloc) {
    auto __first = wjr::__iter_base(first);
    auto __last = wjr::__iter_base(last);
    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<IterInit>>) {
        wjr::uninitialized_value_construct(__first, __last);
    } else {
        for (; __first != __last; ++__first) {
            std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(__first));
        }
    }
}

template <typename Iter, typename Size, typename Alloc>
WJR_CONSTEXPR20 void uninitialized_value_construct_n_using_allocator(Iter first, Size n,
                                                                     Alloc &alloc) {
    auto __first = wjr::__iter_base(first);
    if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<Iter>>) {
        std::uninitialized_value_construct_n(__first, n);
    } else {
        for (; n > 0; --n, (void)++__first) {
            std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(__first));
        }
    }
}

template <typename IterInit, typename InitSe, typename T>
WJR_CONSTEXPR20 void uninitialized_fill(IterInit first, InitSe last, const T &value) {
    auto __first = wjr::__iter_base(first);
    auto __last = wjr::__iter_base(last);
    if constexpr (std::is_same_v<IterInit, InitSe>) {
        std::uninitialized_fill(__first, __last, value);
    } else {
        for (; __first != __last; ++__first) {
            wjr::construct_at(wjr::to_address(__first), value);
        }
    }
}

/**
 * @brief Fill the range [first, last) with value using allocator.
 *
 * @tparam T The value type. Use `default_construct_t` to default construct the
 * elements. Use `value_construct_t` to value construct the elements.
 */
template <typename IterInit, typename InitSe, typename Alloc, typename T>
WJR_CONSTEXPR20 void uninitialized_fill_using_allocator(IterInit first, InitSe last, Alloc &alloc,
                                                        const T &value) {
    if constexpr (std::is_same_v<T, default_construct_t>) {
        uninitialized_default_construct_using_allocator(first, last, alloc);
    } else if constexpr (std::is_same_v<T, value_construct_t>) {
        uninitialized_value_construct_using_allocator(first, last, alloc);
    } else {
        auto __first = wjr::__iter_base(first);
        auto __last = wjr::__iter_base(last);
        if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<IterInit>,
                                                         const T &>) {
            wjr::uninitialized_fill(__first, __last, value);
        } else {
            for (; __first != __last; ++__first) {
                std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(__first), value);
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
WJR_CONSTEXPR20 void uninitialized_fill_n_using_allocator(Iter first, Size n, Alloc &alloc,
                                                          const T &value) {
    if constexpr (std::is_same_v<T, default_construct_t>) {
        uninitialized_default_construct_n_using_allocator(first, n, alloc);
    } else if constexpr (std::is_same_v<T, value_construct_t>) {
        uninitialized_value_construct_n_using_allocator(first, n, alloc);
    } else {
        auto __first = wjr::__iter_base(first);
        if constexpr (is_trivially_allocator_construct_v<Alloc, iterator_value_t<Iter>,
                                                         const T &>) {
            std::uninitialized_fill_n(__first, n, value);
        } else {
            for (; n > 0; --n, (void)__first) {
                std::allocator_traits<Alloc>::construct(alloc, wjr::to_address(__first), value);
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

template <typename IterInit, typename InitSe>
WJR_CONSTEXPR20 void destroy(IterInit first, InitSe last) {
    auto __first = wjr::__iter_base(first);
    auto __last = wjr::__iter_base(last);
    if constexpr (std::is_same_v<IterInit, InitSe>) {
        std::destroy(__first, __last);
    } else {
        for (; __first != __last; ++__first) {
            std::destroy_at(wjr::to_address(__first));
        }
    }
}

template <typename IterInit, typename InitSe, typename Alloc>
WJR_CONSTEXPR20 void destroy_using_allocator(IterInit first, InitSe last, Alloc &alloc) {
    auto __first = wjr::__iter_base(first);
    auto __last = wjr::__iter_base(last);
    if constexpr (is_trivially_allocator_destroy_v<Alloc, iterator_value_t<IterInit>>) {
        wjr::destroy(__first, __last);
    } else {
        for (; __first != __last; ++__first) {
            std::allocator_traits<Alloc>::destroy(alloc, wjr::to_address(__first));
        }
    }
}

template <typename Iter, typename Size, typename Alloc>
WJR_CONSTEXPR20 void destroy_n_using_allocator(Iter first, Size n, Alloc &alloc) {
    auto __first = wjr::__iter_base(first);
    if constexpr (is_trivially_allocator_destroy_v<Alloc, iterator_value_t<Iter>>) {
        std::destroy_n(__first, n);
    } else {
        for (; n > 0; --n, (void)++__first) {
            std::allocator_traits<Alloc>::destroy(alloc, wjr::to_address(__first));
        }
    }
}

template <typename T>
T *relocate_at(T *src, T *dst) noexcept(std::is_nothrow_move_constructible_v<T> &&
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
T *uninitialized_relocate_at_using_allocator(T *src, T *dst, Alloc &alloc) noexcept(
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
OutputIt uninitialized_relocate(InputItInit first, InputItSe last, OutputIt dest) {
    using InputValue = iterator_value_t<InputItInit>;
    using OutputValue = iterator_value_t<OutputIt>;

    auto __first = wjr::__iter_base(first);
    auto __last = wjr::__iter_base(last);
    auto __dest = wjr::__iter_base(dest);

    if constexpr (std::is_same_v<InputItInit, InputItSe> &&
                  std::is_same_v<InputValue, OutputValue> &&
                  is_contiguous_iterator_v<InputItInit> && is_contiguous_iterator_v<OutputIt> &&
                  get_relocate_mode_v<InputValue> == relocate_t::trivial) {
        const auto n = __last - __first;
        maybe_null_memcpy(__dest, __first, n * sizeof(InputValue));
        return wjr::__iter_wrap(dest, __dest);
    } else if constexpr (std::is_same_v<InputValue, OutputValue> &&
                         get_relocate_mode_v<InputValue> == relocate_t::maybe_trivial) {
        for (; __first != __last; ++__dest, (void)++__first) {
            wjr::construct_at(wjr::to_address(__dest), std::move(*__first));
            std::destroy_at(wjr::to_address(__first));
        }

        return wjr::__iter_wrap(dest, __dest);
    } else {
        __dest = wjr::uninitialized_move(__first, __last, __dest);
        std::destroy(__first, __last);
        return wjr::__iter_wrap(dest, __dest);
    }
}

template <typename InputIt, typename OutputIt>
OutputIt uninitialized_relocate_restrict(InputIt first, InputIt last, OutputIt dest) {
    return wjr::__iter_wrap(dest, uninitialized_relocate(__add_restrict(wjr::__iter_base(first)),
                                                         __add_restrict(wjr::__iter_base(last)),
                                                         __add_restrict(wjr::__iter_base(dest))));
}

template <typename InputItInit, typename InputItSe, typename OutputIt, typename Alloc>
OutputIt uninitialized_relocate_using_allocator(InputItInit first, InputItSe last, OutputIt dest,
                                                Alloc &alloc) {
    using InputValue = iterator_value_t<InputItInit>;
    using OutputValue = iterator_value_t<OutputIt>;

    auto __first = wjr::__iter_base(first);
    auto __last = wjr::__iter_base(last);
    auto __dest = wjr::__iter_base(dest);

    if constexpr (is_trivially_allocator_construct_v<
                      Alloc, OutputValue,
                      std::remove_reference_t<iterator_reference_t<InputItInit>> &&> &&
                  is_trivially_allocator_destroy_v<Alloc, iterator_category_t<InputItInit>>) {
        return wjr::__iter_wrap(dest, uninitialized_relocate(__first, __last, __dest));
    } else if constexpr (std::is_same_v<InputValue, OutputValue> &&
                         get_relocate_mode_v<InputValue> == relocate_t::maybe_trivial) {
        for (; __first != __last; ++__dest, (void)++__first) {
            uninitialized_construct_using_allocator(__dest, alloc, std::move(*__first));
            destroy_at_using_allocator(__first, alloc);
        }

        return wjr::__iter_wrap(dest, __dest);
    } else {
        __dest = uninitialized_move_using_allocator(__first, __last, __dest, alloc);
        destroy_using_allocator(__first, __last, alloc);
        return wjr::__iter_wrap(dest, __dest);
    }
}

template <typename InputIt, typename OutputIt, typename Alloc>
OutputIt uninitialized_relocate_restrict_using_allocator(InputIt first, InputIt last, OutputIt dest,
                                                         Alloc &alloc) {
    return wjr::__iter_wrap(dest, uninitialized_relocate_using_allocator(
                                      __add_restrict(wjr::__iter_base(first)),
                                      __add_restrict(wjr::__iter_base(last)),
                                      __add_restrict(wjr::__iter_base(dest)), alloc));
}

template <typename InputIt, typename Size, typename OutputIt>
OutputIt uninitialized_relocate_n(InputIt first, Size n, OutputIt dest) {
    using InputValue = iterator_value_t<InputIt>;
    using OutputValue = iterator_value_t<OutputIt>;

    auto __first = wjr::__iter_base(first);
    auto __dest = wjr::__iter_base(dest);

    if constexpr (std::is_same_v<InputValue, OutputValue> && is_contiguous_iterator_v<InputIt> &&
                  is_contiguous_iterator_v<OutputIt> &&
                  get_relocate_mode_v<InputValue> == relocate_t::trivial) {
        maybe_null_memcpy(__dest, __first, n * sizeof(InputValue));
        return wjr::__iter_wrap(dest, __dest);
    } else if constexpr (std::is_same_v<InputValue, OutputValue> &&
                         get_relocate_mode_v<InputValue> == relocate_t::maybe_trivial) {
        for (; n > 0; --n, (void)++__dest, (void)++__first) {
            wjr::construct_at(wjr::to_address(__dest), std::move(*__first));
            std::destroy_at(wjr::to_address(__first));
        }

        return wjr::__iter_wrap(dest, __dest);
    } else {
        __dest = wjr::uninitialized_move_n(__first, n, __dest);
        std::destroy_n(__first, n);
        return wjr::__iter_wrap(dest, __dest);
    }
}

template <typename InputIt, typename Size, typename OutputIt>
OutputIt uninitialized_relocate_n_restrict(InputIt first, Size n, OutputIt dest) {
    return wjr::__iter_wrap(dest, uninitialized_relocate(__add_restrict(wjr::__iter_base(first)), n,
                                                         __add_restrict(wjr::__iter_base(dest))));
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
OutputIt uninitialized_relocate_n_using_allocator(InputIt first, Size n, OutputIt dest,
                                                  Alloc &alloc) {
    using InputValue = iterator_value_t<InputIt>;
    using OutputValue = iterator_value_t<OutputIt>;

    auto __first = wjr::__iter_base(first);
    auto __dest = wjr::__iter_base(dest);

    if constexpr (is_trivially_allocator_construct_v<
                      Alloc, OutputValue,
                      std::remove_reference_t<iterator_reference_t<InputIt>> &&> &&
                  is_trivially_allocator_destroy_v<Alloc, iterator_category_t<InputIt>>) {
        return wjr::__iter_wrap(dest, uninitialized_relocate_n(__first, n, __dest));
    } else if constexpr (std::is_same_v<InputValue, OutputValue> &&
                         get_relocate_mode_v<InputValue> == relocate_t::maybe_trivial) {
        for (; n > 0; --n, (void)++__first, (void)++__dest) {
            uninitialized_construct_using_allocator(__dest, alloc, std::move(*__first));
            destroy_at_using_allocator(__first, alloc);
        }

        return wjr::__iter_wrap(dest, __dest);
    } else {
        __dest = uninitialized_move_n_using_allocator(__first, n, __dest, alloc);
        destroy_n_using_allocator(__first, n, alloc);
        return wjr::__iter_wrap(dest, __dest);
    }
}

template <typename InputIt, typename Size, typename OutputIt, typename Alloc>
OutputIt uninitialized_relocate_n_restrict_using_allocator(InputIt first, Size n, OutputIt dest,
                                                           Alloc &alloc) {
    return wjr::__iter_wrap(dest, uninitialized_relocate_n_using_allocator(
                                      __add_restrict(wjr::__iter_base(first)), n,
                                      __add_restrict(wjr::__iter_base(dest)), alloc));
}

template <typename T>
struct __aligned_storage_t {
    T *operator&() noexcept { return static_cast<T *>(static_cast<void *>(this)); }
    const T *operator&() const noexcept { return static_cast<T *>(static_cast<void *>(this)); }

    alignas(T) std::byte buf[sizeof(T)];
};

template <typename T, bool Constructor, bool Destructor>
struct __uninitialized_base;

#define WJR_REGISTER_UNION_BASE(CON, DES)                                                          \
    template <typename T>                                                                          \
    struct __uninitialized_base<T, CON, DES> {                                                     \
        __uninitialized_base(const __uninitialized_base &) = default;                              \
        __uninitialized_base(__uninitialized_base &&) = default;                                   \
        __uninitialized_base &operator=(const __uninitialized_base &) = default;                   \
        __uninitialized_base &operator=(__uninitialized_base &&) = default;                        \
                                                                                                   \
        constexpr __uninitialized_base() WJR_PP_BOOL_IF(CON, = default, noexcept {});              \
                                                                                                   \
        template <typename... Args>                                                                \
        constexpr __uninitialized_base(Args &&...args) noexcept(                                   \
            std::is_nothrow_constructible_v<T, Args...>)                                           \
            : m_value(std::forward<Args>(args)...) {}                                              \
                                                                                                   \
        constexpr __uninitialized_base(enable_default_constructor_t) noexcept {}                   \
                                                                                                   \
        ~__uninitialized_base() WJR_PP_BOOL_IF(DES, = default, noexcept {});                       \
                                                                                                   \
        union {                                                                                    \
            T m_value;                                                                             \
            __aligned_storage_t<T> m_storage;                                                      \
        };                                                                                         \
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
private:
    using Mybase = __uninitialized_base_selector<T>;

public:
    using Mybase::Mybase;

    template <typename U = T, WJR_REQUIRES(std::is_copy_constructible_v<U>)>
    WJR_CONSTEXPR20 void __copy_construct(const __uninitialized_control_base &other) noexcept(
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
    std::is_trivially_move_constructible_v<T> || !std::is_move_constructible_v<T>, true, true>;

template <typename T>
using __uninitialized_enabler =
    enable_special_members_base<true, true, std::is_copy_constructible_v<T>,
                                std::is_move_constructible_v<T>, false, false>;

/**
 * @class uninitialized
 *
 * @details Uninitialized object. Make trivially constructible and destructible
 * of any type. At present, the detector for detecting the legality of states
 * under DEBUG has been removed.
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

    constexpr uninitialized(default_construct_t) noexcept : Mybase() {}

    template <typename... Args, WJR_REQUIRES(std::is_constructible_v<T, Args...>)>
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