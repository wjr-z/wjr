/**
 * @file biginteger.hpp
 * @author wjr
 * @brief
 * @version 0.2
 * @date 2025-03-16
 * @todo
 * 1. Add more attributes to help the compiler optimize to zero overhead.
 * Such as unsigned biginteger, fixed size biginteger.
 * 2. Optimize reserve. For example, A = B + C, if A is restrict, then there's no need to memcpy if
 * A need to reserve.
 * 3. Optimization of constant value.
 * 4. [x] Use dispatch to optimize compile and binary size.
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef WJR_BIGINTEGER_BIGINTEGER_HPP__
#define WJR_BIGINTEGER_BIGINTEGER_HPP__

#include <istream>

#include <wjr/biginteger/detail.hpp>
#include <wjr/format/ostream_insert.hpp>
#include <wjr/math.hpp>
#include <wjr/optional.hpp>
#include <wjr/span.hpp>
#include <wjr/vector.hpp>

#include <fmt/format.h>

namespace wjr {

namespace biginteger_detail {

/**
 * @brief Remove leading zeros.
 *
 */
WJR_PURE inline WJR_CONSTEXPR20 uint32_t normalize(const uint64_t *ptr, uint32_t n) noexcept {
    return static_cast<uint32_t>(reverse_find_not_n(ptr, 0, n));
}

} // namespace biginteger_detail

class default_biginteger_size_reference {
public:
    default_biginteger_size_reference() = delete;
    default_biginteger_size_reference(const default_biginteger_size_reference &) = delete;
    default_biginteger_size_reference(default_biginteger_size_reference &&) = default;
    default_biginteger_size_reference &
    operator=(const default_biginteger_size_reference &) = delete;
    default_biginteger_size_reference &operator=(default_biginteger_size_reference &&) = default;
    ~default_biginteger_size_reference() = default;

    constexpr explicit default_biginteger_size_reference(int32_t &size) noexcept : m_size(&size) {}

    constexpr default_biginteger_size_reference &operator=(uint32_t size) noexcept {
        *m_size = __fast_negate_with<int32_t>(*m_size, to_signed(size));
        return *this;
    }

    constexpr operator uint32_t() const noexcept { return __fast_abs(*m_size); }

    constexpr default_biginteger_size_reference &operator++() noexcept {
        *m_size = __fast_increment(*m_size);
        return *this;
    }

    constexpr default_biginteger_size_reference &operator--() noexcept {
        *m_size = __fast_decrement(*m_size);
        return *this;
    }

    constexpr default_biginteger_size_reference &operator+=(uint32_t size) noexcept {
        *m_size = __fast_add(*m_size, size);
        return *this;
    }

    constexpr default_biginteger_size_reference &operator-=(uint32_t size) noexcept {
        *m_size = __fast_sub(*m_size, size);
        return *this;
    }

private:
    int32_t *m_size;
};

/// @private
template <>
struct __unref_wrapper_helper<default_biginteger_size_reference> {
    using type = uint32_t &;
};

/**
 * @brief data_type of biginteger
 *
 * @details View the data of biginteger. Used for type erasure. Manage memory
 * allocation and release on your own.
 *
 */
struct biginteger_data {
    WJR_PURE constexpr uint64_t *data() noexcept { return m_data; }
    WJR_PURE constexpr const uint64_t *data() const noexcept { return m_data; }

    WJR_PURE constexpr uint32_t size() const noexcept { return __fast_abs(m_size); }
    WJR_PURE constexpr uint32_t capacity() const noexcept { return m_capacity; }

    WJR_PURE constexpr bool empty() const noexcept { return m_size == 0; }

    WJR_PURE constexpr int32_t get_ssize() const noexcept { return m_size; }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    void set_ssize(T new_size) noexcept {
        m_size = truncate_cast<int32_t>(new_size);
    }

    void conditional_negate(bool condition) noexcept {
        set_ssize(__fast_conditional_negate<int32_t>(condition, get_ssize()));
    }

    void negate() noexcept { conditional_negate(true); }
    bool is_negate() const noexcept { return get_ssize() < 0; }
    void absolute() noexcept { set_ssize(__fast_abs(get_ssize())); }

    uint64_t *m_data = nullptr;
    int32_t m_size = 0;
    uint32_t m_capacity = 0;
};

template <typename Alloc, typename Mybase, typename IsReallocatable>
class __default_biginteger_vector_storage : private biginteger_data {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<uint64_t>;
    using _Alty_traits = std::allocator_traits<_Alty>;

    template <typename _Mybase, typename _Tag, typename _Enable>
    friend struct container_of_fn;

public:
    using value_type = uint64_t;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using size_type = uint32_t;
    using difference_type = int32_t;
    using allocator_type = Alloc;
    using storage_traits_type = vector_storage_traits<uint64_t, Alloc>;
    using is_reallocatable = IsReallocatable;

    __default_biginteger_vector_storage() = default;

    __default_biginteger_vector_storage(const __default_biginteger_vector_storage &) = delete;
    __default_biginteger_vector_storage(__default_biginteger_vector_storage &&) noexcept = delete;
    __default_biginteger_vector_storage &
    operator=(const __default_biginteger_vector_storage &) = delete;
    __default_biginteger_vector_storage &operator=(__default_biginteger_vector_storage &&) = delete;

    ~__default_biginteger_vector_storage() = default;

    void deallocate_nonnull(_Alty &al) noexcept {
        WJR_ASSERT_ASSUME_L2(data() != nullptr);
        WJR_ASSERT_ASSUME_L2(capacity() != 0);
        al.deallocate(data(), capacity());
    }

    void deallocate(_Alty &al) noexcept {
        if (data() != nullptr) {
            deallocate_nonnull(al);
        }
    }

    void destroy_and_deallocate(_Alty &al) noexcept {
        if (data() != nullptr) {
            destroy_n_using_allocator(data(), (size_type)size(), al);
            deallocate_nonnull(al);
        }
    }

    void uninitialized_construct(Mybase &other, size_type size, size_type capacity,
                                 _Alty &al) noexcept(noexcept(allocate_at_least(al, capacity))) {
        const auto result = allocate_at_least(al, capacity);

        other.m_data = result.ptr;
        other.m_size = __fast_negate_with<int32_t>(this->m_size, size);
        other.m_capacity = static_cast<size_type>(result.count);
    }

    void take_storage(Mybase &other, _Alty &) noexcept {
        *__get_data() = *other.__get_data();
        other.m_data = nullptr;
    }

    void swap_storage(Mybase &other, _Alty &) noexcept {
        std::swap(*__get_data(), *other.__get_data());
    }

    WJR_PURE default_biginteger_size_reference size() noexcept {
        return default_biginteger_size_reference(this->m_size);
    }

    using biginteger_data::capacity;
    using biginteger_data::data;
    using biginteger_data::size;

    constexpr biginteger_data *operator->() noexcept {
        return static_cast<biginteger_data *>(this);
    }

    constexpr const biginteger_data *operator->() const noexcept {
        return static_cast<const biginteger_data *>(this);
    }

    constexpr biginteger_data *__get_data() noexcept {
        return static_cast<biginteger_data *>(this);
    }

    constexpr const biginteger_data *__get_data() const noexcept {
        return static_cast<const biginteger_data *>(this);
    }
};

/**
 * @struct default_biginteger_data
 * @brief The data structure for biginteger
 *
 */
template <typename Alloc>
class default_biginteger_vector_storage
    : public __default_biginteger_vector_storage<Alloc, default_biginteger_vector_storage<Alloc>,
                                                 std::true_type> {};

template <typename Alloc>
struct get_relocate_mode<default_biginteger_vector_storage<Alloc>> {
    static constexpr relocate_t value = relocate_t::trivial;
};

template <typename Alloc>
class fixed_biginteger_vector_storage
    : public __default_biginteger_vector_storage<Alloc, fixed_biginteger_vector_storage<Alloc>,
                                                 std::false_type> {};

template <typename Alloc>
struct get_relocate_mode<fixed_biginteger_vector_storage<Alloc>> {
    static constexpr relocate_t value = relocate_t::trivial;
};

template <size_t Capacity>
class inplace_biginteger_vector_storage {
    using Alloc = std::allocator<uint64_t>;
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<uint64_t>;
    using _Alty_traits = std::allocator_traits<_Alty>;

    template <typename _Mybase, typename _Tag, typename _Enable>
    friend struct container_of_fn;

    static constexpr bool __use_memcpy = sizeof(uint64_t) * Capacity <= 32;

public:
    static constexpr bool is_trivially_relocate_v = false;

    using value_type = uint64_t;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using size_type = uint32_t;
    using difference_type = int32_t;
    using allocator_type = Alloc;
    using storage_traits_type = vector_storage_traits<uint64_t, Alloc>;
    using is_reallocatable = std::false_type;

    inplace_biginteger_vector_storage() noexcept : m_bd({m_storage, 0, Capacity}) {}

    inplace_biginteger_vector_storage(const inplace_biginteger_vector_storage &) = delete;
    inplace_biginteger_vector_storage(inplace_biginteger_vector_storage &&) noexcept = delete;
    inplace_biginteger_vector_storage &
    operator=(const inplace_biginteger_vector_storage &) = delete;
    inplace_biginteger_vector_storage &operator=(inplace_biginteger_vector_storage &&) = delete;

    ~inplace_biginteger_vector_storage() = default;

    void deallocate(_Alty &) noexcept {}

    void uninitialized_construct(inplace_biginteger_vector_storage &other, size_type size,
                                 size_type capacity, _Alty &) noexcept {
        WJR_ASSERT_ASSUME(capacity <= Capacity, "capacity must be less than or equal to Capacity");
        other.m_bd.m_data = m_storage;
        other.m_bd.m_size = size;
    }

    void take_storage(inplace_biginteger_vector_storage &other, _Alty &al) noexcept {
        const auto lhs = data();
        const auto rhs = other.data();

        m_bd.m_size = other.m_bd.m_size;

        if constexpr (__use_memcpy) {
            if (other.size()) {
                builtin_memcpy(lhs, rhs, Capacity);
            }
        } else {
            wjr::uninitialized_move_n_restrict_using_allocator(rhs, other.m_bd.m_size, lhs, al);
        }

        other.m_bd.m_size = 0;
    }

    void swap_storage(inplace_biginteger_vector_storage &other, _Alty &al) noexcept {
        auto *lhs = data();
        auto lsize = size();
        auto *rhs = other.data();
        auto rsize = other.size();

        if (lsize && rsize) {
            m_bd.m_size = rsize;
            other.m_bd.m_size = lsize;

            uint64_t tmp[Capacity];
            if constexpr (__use_memcpy) {
                builtin_memcpy(tmp, lhs, Capacity);
                builtin_memcpy(lhs, rhs, Capacity);
                builtin_memcpy(rhs, tmp, Capacity);
            } else {
                if (lsize > rsize) {
                    std::swap(lhs, rhs);
                    std::swap(lsize, rsize);
                }

                wjr::uninitialized_move_n_restrict_using_allocator(lhs, lsize, tmp, al);
                wjr::uninitialized_move_n_restrict_using_allocator(rhs, rsize, lhs, al);
                wjr::uninitialized_move_n_restrict_using_allocator(tmp, lsize, rhs, al);
            }
        } else if (rsize) {
            if constexpr (__use_memcpy) {
                builtin_memcpy(lhs, rhs, Capacity);
            } else {
                wjr::uninitialized_move_n_restrict_using_allocator(rhs, rsize, lhs, al);
            }
            m_bd.m_size = rsize;
            other.m_bd.m_size = 0;
        } else if (lsize) {
            if constexpr (__use_memcpy) {
                builtin_memcpy(rhs, lhs, Capacity);
            } else {
                wjr::uninitialized_move_n_restrict_using_allocator(lhs, lsize, rhs, al);
            }

            other.m_bd.m_size = lsize;
            m_bd.m_size = 0;
        }
    }

    WJR_PURE default_biginteger_size_reference size() noexcept {
        return default_biginteger_size_reference(m_bd.m_size);
    }

    WJR_PURE constexpr uint64_t *data() noexcept { return m_bd.data(); }
    WJR_PURE constexpr const uint64_t *data() const noexcept { return m_bd.data(); }

    WJR_PURE constexpr uint32_t size() const noexcept { return m_bd.size(); }
    WJR_PURE constexpr uint32_t capacity() const noexcept { return m_bd.capacity(); }

    constexpr biginteger_data *operator->() noexcept { return std::addressof(m_bd); }
    constexpr const biginteger_data *operator->() const noexcept { return std::addressof(m_bd); }

    constexpr biginteger_data *__get_data() noexcept { return std::addressof(m_bd); }
    constexpr const biginteger_data *__get_data() const noexcept { return std::addressof(m_bd); }

private:
    biginteger_data m_bd;
    uint64_t m_storage[Capacity];
};

template <size_t Capacity>
struct get_relocate_mode<inplace_biginteger_vector_storage<Capacity>> {
    static constexpr relocate_t value =
        inplace_biginteger_vector_storage<Capacity>::is_trivially_relocate_v
            ? relocate_t::trivial
            : relocate_t::maybe_trivial;
};

template <typename Alloc>
struct container_of_fn<default_biginteger_vector_storage<Alloc>, void> {
    using base_type = default_biginteger_vector_storage<Alloc>;
    using value_type = biginteger_data;

    base_type &operator()(value_type &ref) const noexcept { return static_cast<base_type &>(ref); }
    const base_type &operator()(const value_type &ref) const noexcept {
        return static_cast<base_type &>(ref);
    }
};

template <typename Alloc>
struct container_of_fn<fixed_biginteger_vector_storage<Alloc>, void> {
    using base_type = fixed_biginteger_vector_storage<Alloc>;
    using value_type = biginteger_data;

    base_type &operator()(value_type &ref) const noexcept { return static_cast<base_type &>(ref); }
    const base_type &operator()(const value_type &ref) const noexcept {
        return static_cast<base_type &>(ref);
    }
};

template <size_t Capacity>
struct container_of_fn<inplace_biginteger_vector_storage<Capacity>, void> {
    using base_type = inplace_biginteger_vector_storage<Capacity>;
    using value_type = biginteger_data;

    base_type &operator()(value_type &ref) const noexcept {
        return *reinterpret_cast<base_type *>(reinterpret_cast<std::byte *>(std::addressof(ref)) -
                                              offsetof(base_type, m_bd));
    }
    const base_type &operator()(const value_type &ref) const noexcept {
        return *reinterpret_cast<const base_type *>(
            reinterpret_cast<const std::byte *>(std::addressof(ref)) - offsetof(base_type, m_bd));
    }
};

template <typename Storage>
class basic_biginteger;

template <typename Alloc = std::allocator<uint64_t>>
using default_biginteger = basic_biginteger<default_biginteger_vector_storage<Alloc>>;

using biginteger = default_biginteger<>;
using weak_stack_biginteger = default_biginteger<weak_stack_allocator<uint64_t>>;

template <typename Alloc = std::allocator<uint64_t>>
using default_fixed_biginteger = basic_biginteger<fixed_biginteger_vector_storage<Alloc>>;

using fixed_biginteger = default_fixed_biginteger<>;
using fixed_weak_stack_biginteger = default_fixed_biginteger<weak_stack_allocator<uint64_t>>;

template <size_t Capacity>
using inplace_biginteger = basic_biginteger<inplace_biginteger_vector_storage<Capacity>>;

WJR_INTRINSIC_CONSTEXPR biginteger_data make_biginteger_data(span<const uint64_t> sp) noexcept {
    return biginteger_data{const_cast<uint64_t *>(sp.data()), static_cast<int32_t>(sp.size()), 0};
}

struct biginteger_dispatch_table;

template <typename T>
struct biginteger_dispatch_static_table;

class biginteger_dispatcher {
public:
    template <typename T>
    constexpr biginteger_dispatcher(T *p) noexcept
        : ptr(p->__get_data()), v_table(&biginteger_dispatch_static_table<T>::table) {}

    WJR_CONSTEXPR20 biginteger_dispatcher(enable_default_constructor_t) noexcept : ptr(nullptr) {}

    biginteger_dispatcher() = delete;
    biginteger_dispatcher(const biginteger_dispatcher &) = default;
    biginteger_dispatcher(biginteger_dispatcher &&) = default;
    biginteger_dispatcher &operator=(const biginteger_dispatcher &) = default;
    biginteger_dispatcher &operator=(biginteger_dispatcher &&) = default;

    WJR_PURE constexpr biginteger_data *raw() const noexcept { return ptr; }
    constexpr bool has_value() const noexcept { return ptr != nullptr; }

    constexpr uint64_t *data() noexcept { return ptr->data(); }
    constexpr const uint64_t *data() const noexcept { return ptr->data(); }

    constexpr uint32_t size() const noexcept { return ptr->size(); }
    constexpr uint32_t capacity() const noexcept { return ptr->capacity(); }

    constexpr bool empty() const noexcept { return ptr->empty(); }

    constexpr int32_t get_ssize() const noexcept { return ptr->get_ssize(); }

    void set_ssize(int32_t size) const noexcept { ptr->set_ssize(size); }

    void conditional_negate(bool condition) noexcept { ptr->conditional_negate(condition); }

    void negate() noexcept { ptr->negate(); }
    bool is_negate() const noexcept { return ptr->is_negate(); }
    void absolute() noexcept { ptr->absolute(); }

    template <typename UnsignedValue, WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    biginteger_dispatcher &operator=(UnsignedValue value) noexcept {
        clear();
        if (value != 0) {
            reserve(1);
            data()[0] = value;
            set_ssize(1);
        }

        return *this;
    }

    template <typename SignedValue, WJR_REQUIRES(is_nonbool_signed_integral_v<SignedValue>)>
    biginteger_dispatcher &operator=(SignedValue value) noexcept {
        clear();
        if (value != 0) {
            reserve(1);
            data()[0] = value < 0 ? -to_unsigned(value) : to_unsigned(value);
            set_ssize(__fast_conditional_negate<int32_t>(value < 0, 1));
        }

        return *this;
    }

    inline void reserve(uint32_t size) const;
    inline void clear_if_reserved(uint32_t size) const;
    inline void clear() const { set_ssize(0); }

    inline biginteger_dispatcher construct_reserve(uint32_t n, unique_stack_allocator *al) const;

    inline void destroy() const;
    inline void copy_assign(const biginteger_data *rhs) const;
    inline void move_assign(biginteger_data *rhs) const;

private:
    biginteger_data *ptr;
    const biginteger_dispatch_table *v_table;
};

struct biginteger_dispatch_table {
    void (*__reserve_unchecked)(biginteger_data *, uint32_t);
    void (*clear_if_reserved)(biginteger_data *, uint32_t);
    biginteger_dispatcher (*construct_reserve)(biginteger_data *, uint32_t,
                                               unique_stack_allocator *);
    void (*destroy)(biginteger_data *);
    void (*copy_assign)(biginteger_data *, const biginteger_data *);
    void (*move_assign)(biginteger_data *, biginteger_data *);
};

void biginteger_dispatcher::reserve(uint32_t size) const {
    // fast-path
    if (WJR_UNLIKELY(ptr->capacity() < size))
        v_table->__reserve_unchecked(ptr, size);
}

void biginteger_dispatcher::clear_if_reserved(uint32_t size) const {
    v_table->clear_if_reserved(ptr, size);
}

biginteger_dispatcher biginteger_dispatcher::construct_reserve(uint32_t n,
                                                               unique_stack_allocator *al) const {
    return v_table->construct_reserve(ptr, n, al);
}

void biginteger_dispatcher::destroy() const { v_table->destroy(ptr); }

void biginteger_dispatcher::copy_assign(const biginteger_data *rhs) const {
    v_table->copy_assign(ptr, rhs);
}

void biginteger_dispatcher::move_assign(biginteger_data *rhs) const {
    v_table->move_assign(ptr, rhs);
}

template <typename T>
struct biginteger_dispatch_static_table {
    static constexpr biginteger_dispatch_table table = {
        [](biginteger_data *data, uint32_t n) { container_of<T>(*data).__reserve_unchecked(n); },
        [](biginteger_data *data, uint32_t n) { container_of<T>(*data).clear_if_reserved(n); },
        [](biginteger_data *data, uint32_t n, unique_stack_allocator *al) -> biginteger_dispatcher {
            auto &dst = container_of<T>(*data);
            T *tmp = static_cast<T *>(al->allocate(sizeof(T)));
            wjr::construct_at(tmp, dst.get_growth_capacity(dst.capacity(), n), in_place_reserve,
                              dst.get_allocator());
            return biginteger_dispatcher(tmp);
        },
        [](biginteger_data *data) { std::destroy_at(std::addressof(container_of<T>(*data))); },
        [](biginteger_data *lhs, const biginteger_data *rhs) { container_of<T>(*lhs) = *rhs; },
        [](biginteger_data *lhs, biginteger_data *rhs) {
            container_of<T>(*lhs) = std::move(container_of<T>(*rhs));
        }};
};

namespace biginteger_detail {

// const basic_biginteger<Storage>* don't need to get allocator
// use const Storage* instead of const basic_biginteger<Storage>*

/// @private
template <typename S>
WJR_CONST bool __equal_pointer(const basic_biginteger<S> *lhs,
                               const basic_biginteger<S> *rhs) noexcept {
    return lhs == rhs;
}

/// @private
template <typename S0, typename S1>
WJR_CONST bool __equal_pointer(const basic_biginteger<S0> *,
                               const basic_biginteger<S1> *) noexcept {
    return false;
}

/// @private
template <typename S>
WJR_PURE bool __equal_pointer(const basic_biginteger<S> *lhs, const biginteger_data *rhs) noexcept {
    return lhs->__get_data() == rhs;
}

/// @private
template <typename S>
WJR_PURE bool __equal_pointer(const biginteger_data *lhs, const basic_biginteger<S> *rhs) noexcept {
    return lhs == rhs->__get_data();
}

/// @private
WJR_CONST inline bool __equal_pointer(const biginteger_data *lhs,
                                      const biginteger_data *rhs) noexcept {
    return lhs == rhs;
}

template <bool Check>
WJR_ALL_NONNULL from_chars_result<const char *>
__from_chars_impl(const char *first, const char *last, biginteger_dispatcher dst,
                  unsigned int base) noexcept;

extern template from_chars_result<const char *>
__from_chars_impl<false>(const char *first, const char *last, biginteger_dispatcher dst,
                         unsigned int base) noexcept;

extern template from_chars_result<const char *> __from_chars_impl<true>(const char *first,
                                                                        const char *last,
                                                                        biginteger_dispatcher dst,
                                                                        unsigned int base) noexcept;

/// @private
template <bool Check, typename S>
WJR_ALL_NONNULL from_chars_result<const char *>
__from_chars_impl(const char *first, const char *last, basic_biginteger<S> *dst,
                  unsigned int base) noexcept {
    return __from_chars_impl<Check>(first, last, biginteger_dispatcher(dst), base);
}

/// @private
WJR_PURE inline int32_t __compare_impl(const biginteger_data *lhs,
                                       const biginteger_data *rhs) noexcept;

/// @private
WJR_PURE inline int32_t __compare_ui_impl(const biginteger_data *lhs, uint64_t rhs) noexcept;

/// @private
WJR_PURE inline int32_t __compare_si_impl(const biginteger_data *lhs, int64_t rhs) noexcept;

/// @private
template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
WJR_PURE int32_t __compare_impl(const biginteger_data *lhs, T rhs) noexcept {
    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(rhs == 0)) {
        const int32_t ssize = lhs->get_ssize();
        return ssize == 0 ? 0 : ssize < 0 ? -1 : 1;
    }

    if constexpr (std::is_unsigned_v<T>) {
        return __compare_ui_impl(lhs, rhs);
    } else {
        if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(rhs >= 0)) {
            return __compare_ui_impl(lhs, to_unsigned(rhs));
        }

        return __compare_si_impl(lhs, rhs);
    }
}

/// @private
template <bool xsign>
WJR_ALL_NONNULL void __addsub_impl(biginteger_dispatcher dst, const biginteger_data *lhs,
                                   uint64_t rhs) noexcept;

extern template void __addsub_impl<false>(biginteger_dispatcher dst, const biginteger_data *lhs,
                                          uint64_t rhs) noexcept;
extern template void __addsub_impl<true>(biginteger_dispatcher dst, const biginteger_data *lhs,
                                         uint64_t rhs) noexcept;

/// @private
template <bool xsign, typename S>
void __addsub_impl(basic_biginteger<S> *dst, const biginteger_data *lhs, uint64_t rhs) noexcept {
    __addsub_impl<xsign>(biginteger_dispatcher(dst), lhs, rhs);
}

/// @private
template <typename S>
void __ui_sub_impl(basic_biginteger<S> *dst, uint64_t lhs, const biginteger_data *rhs) noexcept;

/// @private
extern WJR_ALL_NONNULL void __addsub_impl(biginteger_dispatcher dst, const biginteger_data *lhs,
                                          const biginteger_data *rhs, bool xsign) noexcept;

/// @private
template <bool xsign, typename S>
WJR_ALL_NONNULL void __addsub_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                                   const biginteger_data *rhs) noexcept {
    __addsub_impl(biginteger_dispatcher(dst), lhs, rhs, xsign);
}

/// @private
template <typename S>
void __add_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                const biginteger_data *rhs) noexcept {
    __addsub_impl<false>(dst, lhs, rhs);
}

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void __add_impl(basic_biginteger<S> *dst, const biginteger_data *lhs, T rhs) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        __addsub_impl<false>(dst, lhs, rhs);
    } else {
        if (rhs < 0) {
            __addsub_impl<true>(dst, lhs, -to_unsigned(rhs));
        } else {
            __addsub_impl<false>(dst, lhs, to_unsigned(rhs));
        }
    }
}

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void __add_impl(basic_biginteger<S> *dst, T lhs, const biginteger_data *rhs) noexcept {
    __add_impl(dst, rhs, lhs);
}

/// @private
template <typename S>
void __sub_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                const biginteger_data *rhs) noexcept {
    __addsub_impl<true>(dst, lhs, rhs);
}

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void __sub_impl(basic_biginteger<S> *dst, const biginteger_data *lhs, T rhs) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        __addsub_impl<true>(dst, lhs, rhs);
    } else {
        if (rhs < 0) {
            __addsub_impl<false>(dst, lhs, -to_unsigned(rhs));
        } else {
            __addsub_impl<true>(dst, lhs, to_unsigned(rhs));
        }
    }
}

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void __sub_impl(basic_biginteger<S> *dst, T lhs, const biginteger_data *rhs) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        __ui_sub_impl(dst, lhs, rhs);
    } else {
        if (lhs < 0) {
            __addsub_impl<false>(dst, rhs, -to_unsigned(lhs));
            dst->negate();
        } else {
            __ui_sub_impl(dst, to_unsigned(lhs), rhs);
        }
    }
}

/// @private
template <typename S>
void __mul_ui_impl(basic_biginteger<S> *dst, const biginteger_data *lhs, uint64_t rhs) noexcept;

/// @private
extern WJR_ALL_NONNULL void __mul_impl(biginteger_dispatcher dst, const biginteger_data *lhs,
                                       const biginteger_data *rhs) noexcept;

/// @private
template <typename S>
WJR_ALL_NONNULL void __mul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                                const biginteger_data *rhs) noexcept {
    __mul_impl(biginteger_dispatcher(dst), lhs, rhs);
}

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void __mul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs, T rhs) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        __mul_ui_impl(dst, lhs, rhs);
    } else {
        uint64_t value = to_unsigned(rhs);
        bool cond = false;

        if (rhs < 0) {
            value = -value;
            cond = true;
        }

        __mul_ui_impl(dst, lhs, value);
        dst->conditional_negate(cond);
    }
}

/// @private
extern WJR_ALL_NONNULL void __sqr_impl(biginteger_dispatcher dst,
                                       const biginteger_data *src) noexcept;

/// @private
template <typename S>
WJR_ALL_NONNULL void __sqr_impl(basic_biginteger<S> *dst, const biginteger_data *src) noexcept {
    __sqr_impl(biginteger_dispatcher(dst), src);
}

/// @private
extern WJR_ALL_NONNULL void __addsubmul_impl(biginteger_dispatcher dst, const biginteger_data *lhs,
                                             uint64_t rhs, int32_t xmask) noexcept;

/// @private
template <typename S>
void __addsubmul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs, uint64_t rhs,
                      int32_t xmask) noexcept {
    __addsubmul_impl(biginteger_dispatcher(dst), lhs, rhs, xmask);
}

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void __addmul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs, T rhs) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        __addsubmul_impl(dst, lhs, rhs, 0);
    } else {
        uint64_t rvalue = to_unsigned(rhs);
        int32_t xsign = 0;

        if (rhs < 0) {
            rvalue = -rvalue;
            xsign = -1;
        }

        __addsubmul_impl(dst, lhs, rvalue, xsign);
    }
}

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void __submul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs, T rhs) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        __addsubmul_impl(dst, lhs, rhs, -1);
    } else {
        uint64_t rvalue = to_unsigned(rhs);
        int32_t xsign = -1;

        if (rhs < 0) {
            rvalue = -rvalue;
            xsign = 0;
        }

        __addsubmul_impl(dst, lhs, rvalue, xsign);
    }
}

extern WJR_ALL_NONNULL void __addsubmul_impl(biginteger_dispatcher dst, const biginteger_data *lhs,
                                             const biginteger_data *rhs, int32_t xmask) noexcept;

/// @private
template <typename S>
WJR_ALL_NONNULL void __addsubmul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                                      const biginteger_data *rhs, int32_t xmask) noexcept {
    __addsubmul_impl(biginteger_dispatcher(dst), lhs, rhs, xmask);
}

/// @private
template <typename S>
void __addmul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                   const biginteger_data *rhs) noexcept {
    __addsubmul_impl(dst, lhs, rhs, 0);
}

/// @private
template <typename S>
void __submul_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                   const biginteger_data *rhs) noexcept {
    __addsubmul_impl(dst, lhs, rhs, -1);
}

/// @private
template <typename S>
void __mul_2exp_impl(basic_biginteger<S> *dst, const biginteger_data *lhs, uint32_t shift) noexcept;

/// @private
extern WJR_ALL_NONNULL void __tdiv_qr_impl(biginteger_dispatcher quot, biginteger_dispatcher rem,
                                           const biginteger_data *num,
                                           const biginteger_data *div) noexcept;

/// @private
template <typename S0, typename S1>
void __tdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                    const biginteger_data *num, const biginteger_data *div) noexcept {
    __tdiv_qr_impl(biginteger_dispatcher(quot), biginteger_dispatcher(rem), num, div);
}

/// @private
extern WJR_ALL_NONNULL void __tdiv_q_impl(biginteger_dispatcher quot, const biginteger_data *num,
                                          const biginteger_data *div) noexcept;

/// @private
template <typename S>
void __tdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                   const biginteger_data *div) noexcept {
    __tdiv_q_impl(biginteger_dispatcher(quot), num, div);
}

/// @private
extern WJR_ALL_NONNULL void __tdiv_r_impl(biginteger_dispatcher rem, const biginteger_data *num,
                                          const biginteger_data *div) noexcept;

/// @private
template <typename S>
void __tdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                   const biginteger_data *div) noexcept {
    __tdiv_r_impl(biginteger_dispatcher(rem), num, div);
}

/// @private
template <typename S0, typename S1>
uint64_t __tdiv_qr_ui_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                           const biginteger_data *num, uint64_t div) noexcept;

/// @private
template <typename S>
uint64_t __tdiv_q_ui_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                          uint64_t div) noexcept;

/// @private
template <typename S>
uint64_t __tdiv_r_ui_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                          uint64_t div) noexcept;

/// @private
template <typename S0, typename S1, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __tdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                        const biginteger_data *num, T div) noexcept;

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __tdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num, T div) noexcept;

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __tdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num, uint64_t div) noexcept;

/// @private
template <typename S0, typename S1>
void __fdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                    const biginteger_data *num, const biginteger_data *div) noexcept;

/// @private
template <typename S>
void __fdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                   const biginteger_data *div) noexcept;

/// @private
template <typename S>
void __fdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                   const biginteger_data *div) noexcept;

/// @private
template <typename S0, typename S1, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __fdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                        const biginteger_data *num, T div) noexcept;

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __fdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num, T div) noexcept;

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __fdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num, uint64_t div) noexcept;

/// @private
template <typename S0, typename S1>
void __cdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                    const biginteger_data *num, const biginteger_data *div) noexcept;

/// @private
template <typename S>
void __cdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                   const biginteger_data *div) noexcept;

/// @private
template <typename S>
void __cdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                   const biginteger_data *div) noexcept;

/// @private
template <typename S0, typename S1, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __cdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                        const biginteger_data *num, T div) noexcept;

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __cdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num, T div) noexcept;

/// @private
template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t __cdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num, uint64_t div) noexcept;

/// @private
template <typename S>
void __tdiv_q_2exp_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                        uint32_t shift) noexcept;

/// @private
template <typename S>
void __tdiv_r_2exp_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                        uint32_t shift) noexcept;

/// @private
template <typename S>
void __cfdiv_q_2exp_impl(basic_biginteger<S> *quot, const biginteger_data *num, uint32_t shift,
                         int32_t xdir) noexcept;

/// @private
template <typename S>
void __cdiv_q_2exp_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                        uint32_t shift) noexcept {
    __cfdiv_q_2exp_impl(quot, num, shift, 1);
}

/// @private
template <typename S>
void __fdiv_q_2exp_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                        uint32_t shift) noexcept {
    __cfdiv_q_2exp_impl(quot, num, shift, -1);
}

/// @private
extern WJR_ALL_NONNULL void __cfdiv_r_2exp_impl(biginteger_dispatcher rem,
                                                const biginteger_data *num, uint32_t shift,
                                                int32_t xdir) noexcept;

/// @private
template <typename S>
void __cfdiv_r_2exp_impl(basic_biginteger<S> *rem, const biginteger_data *num, uint32_t shift,
                         int32_t xdir) noexcept {
    __cfdiv_r_2exp_impl(biginteger_dispatcher(rem), num, shift, xdir);
}

/// @private
template <typename S>
void __cdiv_r_2exp_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                        uint32_t shift) noexcept {
    __cfdiv_r_2exp_impl(rem, num, shift, 1);
}

/// @private
template <typename S>
void __fdiv_r_2exp_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                        uint32_t shift) noexcept {
    __cfdiv_r_2exp_impl(rem, num, shift, -1);
}

/// @private
template <typename S, typename Engine,
          WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_bit_impl(basic_biginteger<S> *dst, uint32_t size, Engine &engine) noexcept;

/// @private
template <typename S, typename Engine,
          WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_exact_bit_impl(basic_biginteger<S> *dst, uint32_t size, Engine &engine) noexcept;

/// @private
template <typename Engine, WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_impl(biginteger_dispatcher dst, const biginteger_data *limit,
                    Engine &engine) noexcept;

/// @private
template <typename S, typename Engine,
          WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_impl(basic_biginteger<S> *dst, const biginteger_data *limit,
                    Engine &engine) noexcept;

/// @private
template <typename Engine, WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_exact_impl(biginteger_dispatcher dst, const biginteger_data *limit,
                          Engine &engine) noexcept;

/// @private
template <typename S, typename Engine,
          WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_exact_impl(basic_biginteger<S> *dst, const biginteger_data *limit,
                          Engine &engine) noexcept {
    __urandom_exact_impl(biginteger_dispatcher(dst), limit, engine);
}

/// @private
WJR_PURE inline uint32_t __bit_width_impl(const biginteger_data *num) noexcept;

/// @private
WJR_PURE inline uint32_t __ctz_impl(const biginteger_data *num) noexcept;

/// @private
extern WJR_ALL_NONNULL void __pow_impl(biginteger_dispatcher dst, const biginteger_data *num,
                                       uint32_t exp) noexcept;

/// @private
template <typename S>
void __pow_impl(basic_biginteger<S> *dst, const biginteger_data *num, uint32_t exp) noexcept {
    __pow_impl(biginteger_dispatcher(dst), num, exp);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void __pow_impl(basic_biginteger<S> *dst, T num, uint32_t exp) noexcept;

/// @private
struct __powmod_iterator {
    __powmod_iterator(const uint64_t *ptr_, uint32_t size_) noexcept
        : ptr(ptr_), cache(ptr[0]), size(size_) {}

    constexpr void next() noexcept {
        if (++offset == 64) {
            offset = 0;
            ++pos;
            WJR_ASSERT(pos != size);
            cache = ptr[pos];
        }
    }

    constexpr uint64_t get() const noexcept { return (cache >> offset); }
    constexpr bool end() const noexcept { return pos == size - 1 && get() == 1; }

    const uint64_t *ptr;
    uint64_t cache;
    uint32_t offset = 0;
    uint32_t pos = 0;
    uint32_t size;
};

/// @todo optimize
template <typename S>
void __powmod_impl(basic_biginteger<S> *dst, const biginteger_data *num, __powmod_iterator *iter,
                   const biginteger_data *mod) noexcept;

/// @todo optimize
template <typename S>
void __powmod_impl(basic_biginteger<S> *dst, const biginteger_data *num, const biginteger_data *exp,
                   const biginteger_data *mod) noexcept;

/// @todo optimize
template <typename S>
void __powmod_impl(basic_biginteger<S> *dst, const biginteger_data *num, uint64_t exp,
                   const biginteger_data *mod) noexcept;

} // namespace biginteger_detail

template <typename S>
from_chars_result<const char *> from_chars(const char *first, const char *last,
                                           basic_biginteger<S> &dst,
                                           unsigned int base = 10) noexcept;

template <typename Iter>
Iter to_chars_unchecked(Iter ptr, const biginteger_data &src, unsigned int base = 10) noexcept {
    if (src.empty()) {
        *ptr++ = '0';
        return ptr;
    }

    if (src.is_negate()) {
        *ptr++ = '-';
    }

    return biginteger_to_chars(ptr, src.data(), src.size(), base);
}

template <typename Traits, typename S>
std::basic_istream<char, Traits> &operator>>(std::basic_istream<char, Traits> &is,
                                             basic_biginteger<S> &dst) noexcept;

template <typename Traits>
std::basic_ostream<char, Traits> &operator<<(std::basic_ostream<char, Traits> &os,
                                             const biginteger_data &src) noexcept;

WJR_NODISCARD WJR_PURE inline int32_t compare(const biginteger_data &lhs,
                                              const biginteger_data &rhs) noexcept {
    return biginteger_detail::__compare_impl(&lhs, &rhs);
}

template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
WJR_NODISCARD WJR_PURE int32_t compare(const biginteger_data &lhs, T rhs) noexcept {
    return biginteger_detail::__compare_impl(&lhs, rhs);
}

template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
WJR_NODISCARD WJR_PURE int32_t compare(T lhs, const biginteger_data &rhs) noexcept {
    return -compare(rhs, lhs);
}

#define WJR_REGISTER_BIGINTEGER_COMPARE(op)                                                        \
    WJR_NODISCARD WJR_PURE inline bool operator op(const biginteger_data &lhs,                     \
                                                   const biginteger_data &rhs) noexcept {          \
        return compare(lhs, rhs) op 0;                                                             \
    }                                                                                              \
    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>                                  \
    WJR_NODISCARD WJR_PURE bool operator op(const biginteger_data &lhs, T rhs) noexcept {          \
        return compare(lhs, rhs) op 0;                                                             \
    }                                                                                              \
    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>                                  \
    WJR_NODISCARD WJR_PURE bool operator op(T lhs, const biginteger_data &rhs) noexcept {          \
        return compare(lhs, rhs) op 0;                                                             \
    }

WJR_REGISTER_BIGINTEGER_COMPARE(==)
WJR_REGISTER_BIGINTEGER_COMPARE(!=)
WJR_REGISTER_BIGINTEGER_COMPARE(<)
WJR_REGISTER_BIGINTEGER_COMPARE(>)
WJR_REGISTER_BIGINTEGER_COMPARE(<=)
WJR_REGISTER_BIGINTEGER_COMPARE(>=)

#undef WJR_REGISTER_BIGINTEGER_COMPARE

#define WJR_REGISTER_BIGINTEGER_ADDSUB(ADDSUB)                                                     \
    template <typename S>                                                                          \
    void ADDSUB(basic_biginteger<S> &dst, const biginteger_data &lhs,                              \
                const biginteger_data &rhs) noexcept {                                             \
        biginteger_detail::WJR_PP_CONCAT(__, WJR_PP_CONCAT(ADDSUB, _impl))(&dst, &lhs, &rhs);      \
    }                                                                                              \
    template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>                      \
    void ADDSUB(basic_biginteger<S> &dst, const biginteger_data &lhs, T rhs) noexcept {            \
        biginteger_detail::WJR_PP_CONCAT(__, WJR_PP_CONCAT(ADDSUB, _impl))(&dst, &lhs, rhs);       \
    }                                                                                              \
    template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>                      \
    void ADDSUB(basic_biginteger<S> &dst, T lhs, const biginteger_data &rhs) noexcept {            \
        biginteger_detail::WJR_PP_CONCAT(__, WJR_PP_CONCAT(ADDSUB, _impl))(&dst, lhs, &rhs);       \
    }

WJR_REGISTER_BIGINTEGER_ADDSUB(add)
WJR_REGISTER_BIGINTEGER_ADDSUB(sub)

#undef WJR_REGISTER_BIGINTEGER_ADDSUB

template <typename S>
void increment(basic_biginteger<S> &dst) noexcept {
    add(dst, dst, 1u);
}

template <typename S>
void decrement(basic_biginteger<S> &dst) noexcept {
    sub(dst, dst, 1u);
}

template <typename S>
void negate(basic_biginteger<S> &dst) noexcept;

template <typename S>
void absolute(basic_biginteger<S> &dst) noexcept;

template <typename S>
void sqr(basic_biginteger<S> &dst, const biginteger_data &src) noexcept {
    biginteger_detail::__sqr_impl(&dst, &src);
}

template <typename S>
void mul(basic_biginteger<S> &dst, const biginteger_data &lhs,
         const biginteger_data &rhs) noexcept {
    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(std::addressof(lhs) ==
                                                                   std::addressof(rhs))) {
        sqr(dst, lhs);
        return;
    }

    biginteger_detail::__mul_impl(&dst, &lhs, &rhs);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void mul(basic_biginteger<S> &dst, const biginteger_data &lhs, T rhs) noexcept {
    biginteger_detail::__mul_impl(&dst, &lhs, rhs);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void mul(basic_biginteger<S> &dst, T lhs, const biginteger_data &rhs) noexcept {
    biginteger_detail::__mul_impl(&dst, &rhs, lhs);
}

template <typename S>
void addmul(basic_biginteger<S> &dst, const biginteger_data &lhs,
            const biginteger_data &rhs) noexcept {
    biginteger_detail::__addmul_impl(&dst, &lhs, &rhs);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void addmul(basic_biginteger<S> &dst, const biginteger_data &lhs, T rhs) noexcept {
    biginteger_detail::__addmul_impl(&dst, &lhs, rhs);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void addmul(basic_biginteger<S> &dst, T lhs, const biginteger_data &rhs) noexcept {
    biginteger_detail::__addmul_impl(&dst, &rhs, lhs);
}

template <typename S>
void submul(basic_biginteger<S> &dst, const biginteger_data &lhs,
            const biginteger_data &rhs) noexcept {
    biginteger_detail::__submul_impl(&dst, &lhs, &rhs);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void submul(basic_biginteger<S> &dst, const biginteger_data &lhs, T rhs) noexcept {
    biginteger_detail::__submul_impl(&dst, &lhs, rhs);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void submul(basic_biginteger<S> &dst, T lhs, const biginteger_data &rhs) noexcept {
    biginteger_detail::__submul_impl(&dst, &rhs, lhs);
}

template <typename S>
void mul_2exp(basic_biginteger<S> &dst, const biginteger_data &lhs, uint32_t shift) noexcept {
    biginteger_detail::__mul_2exp_impl(&dst, &lhs, shift);
}

template <typename S0, typename S1>
void tdiv_qr(basic_biginteger<S0> &quot, basic_biginteger<S1> &rem, const biginteger_data &num,
             const biginteger_data &div) noexcept {
    biginteger_detail::__tdiv_qr_impl(&quot, &rem, &num, &div);
}

template <typename S>
void tdiv_q(basic_biginteger<S> &quot, const biginteger_data &num,
            const biginteger_data &div) noexcept {
    biginteger_detail::__tdiv_q_impl(&quot, &num, &div);
}

template <typename S0>
void tdiv_r(basic_biginteger<S0> &rem, const biginteger_data &num,
            const biginteger_data &div) noexcept {
    biginteger_detail::__tdiv_r_impl(&rem, &num, &div);
}

template <typename S0, typename S1, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t tdiv_qr(basic_biginteger<S0> &quot, basic_biginteger<S1> &rem, const biginteger_data &num,
                 T div) noexcept {
    return biginteger_detail::__tdiv_qr_impl(&quot, &rem, &num, div);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t tdiv_q(basic_biginteger<S> &quot, const biginteger_data &num, T div) noexcept {
    return biginteger_detail::__tdiv_q_impl(&quot, &num, div);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t tdiv_r(basic_biginteger<S> &rem, const biginteger_data &num, T div) noexcept {
    return biginteger_detail::__tdiv_r_impl(&rem, &num, div);
}

template <typename S0, typename S1>
void fdiv_qr(basic_biginteger<S0> &quot, basic_biginteger<S1> &rem, const biginteger_data &num,
             const biginteger_data &div) noexcept {
    biginteger_detail::__fdiv_qr_impl(&quot, &rem, &num, &div);
}

template <typename S>
void fdiv_q(basic_biginteger<S> &quot, const biginteger_data &num,
            const biginteger_data &div) noexcept {
    biginteger_detail::__fdiv_q_impl(&quot, &num, &div);
}

template <typename S0>
void fdiv_r(basic_biginteger<S0> &rem, const biginteger_data &num,
            const biginteger_data &div) noexcept {
    biginteger_detail::__fdiv_r_impl(&rem, &num, &div);
}

template <typename S0, typename S1, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t fdiv_qr(basic_biginteger<S0> &quot, basic_biginteger<S1> &rem, const biginteger_data &num,
                 T div) noexcept {
    return biginteger_detail::__fdiv_qr_impl(&quot, &rem, &num, div);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t fdiv_q(basic_biginteger<S> &quot, const biginteger_data &num, T div) noexcept {
    return biginteger_detail::__fdiv_q_impl(&quot, &num, div);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t fdiv_r(basic_biginteger<S> &rem, const biginteger_data &num, T div) noexcept {
    return biginteger_detail::__fdiv_r_impl(&rem, &num, div);
}

template <typename S0, typename S1>
void cdiv_qr(basic_biginteger<S0> &quot, basic_biginteger<S1> &rem, const biginteger_data &num,
             const biginteger_data &div) noexcept {
    biginteger_detail::__cdiv_qr_impl(&quot, &rem, &num, &div);
}

template <typename S>
void cdiv_q(basic_biginteger<S> &quot, const biginteger_data &num,
            const biginteger_data &div) noexcept {
    biginteger_detail::__cdiv_q_impl(&quot, &num, &div);
}

template <typename S0>
void cdiv_r(basic_biginteger<S0> &rem, const biginteger_data &num,
            const biginteger_data &div) noexcept {
    biginteger_detail::__cdiv_r_impl(&rem, &num, &div);
}

template <typename S0, typename S1, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t cdiv_qr(basic_biginteger<S0> &quot, basic_biginteger<S1> &rem, const biginteger_data &num,
                 T div) noexcept {
    return biginteger_detail::__cdiv_qr_impl(&quot, &rem, &num, div);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t cdiv_q(basic_biginteger<S> &quot, const biginteger_data &num, T div) noexcept {
    return biginteger_detail::__cdiv_q_impl(&quot, &num, div);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
uint64_t cdiv_r(basic_biginteger<S> &rem, const biginteger_data &num, T div) noexcept {
    return biginteger_detail::__cdiv_r_impl(&rem, &num, div);
}

template <typename S>
void tdiv_q_2exp(basic_biginteger<S> &quot, const biginteger_data &num, uint32_t shift) noexcept {
    biginteger_detail::__tdiv_q_2exp_impl(&quot, &num, shift);
}

template <typename S>
void tdiv_r_2exp(basic_biginteger<S> &rem, const biginteger_data &num, uint32_t shift) noexcept {
    biginteger_detail::__tdiv_r_2exp_impl(&rem, &num, shift);
}

template <typename S>
void fdiv_q_2exp(basic_biginteger<S> &quot, const biginteger_data &num, uint32_t shift) noexcept {
    biginteger_detail::__fdiv_q_2exp_impl(&quot, &num, shift);
}

template <typename S>
void cdiv_q_2exp(basic_biginteger<S> &quot, const biginteger_data &num, uint32_t shift) noexcept {
    biginteger_detail::__cdiv_q_2exp_impl(&quot, &num, shift);
}

template <typename S>
void cdiv_r_2exp(basic_biginteger<S> &rem, const biginteger_data &num, uint32_t shift) noexcept {
    biginteger_detail::__cdiv_r_2exp_impl(&rem, &num, shift);
}

template <typename S>
void fdiv_r_2exp(basic_biginteger<S> &rem, const biginteger_data &num, uint32_t shift) noexcept {
    biginteger_detail::__fdiv_r_2exp_impl(&rem, &num, shift);
}

template <typename S, typename Engine,
          WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void urandom_bit(basic_biginteger<S> &dst, uint32_t size, Engine &engine) noexcept {
    biginteger_detail::__urandom_bit_impl(&dst, size, engine);
}

template <typename S, typename Engine,
          WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void urandom_exact_bit(basic_biginteger<S> &dst, uint32_t size, Engine &engine) noexcept {
    biginteger_detail::__urandom_exact_bit_impl(&dst, size, engine);
}

template <typename S, typename Engine,
          WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void urandom(basic_biginteger<S> &dst, const biginteger_data &limit, Engine &engine) noexcept {
    biginteger_detail::__urandom_impl(&dst, &limit, engine);
}

template <typename S, typename Engine,
          WJR_REQUIRES(biginteger_uniform_random_bit_generator_v<Engine>)>
void urandom_exact(basic_biginteger<S> &dst, const biginteger_data &limit,
                   Engine &engine) noexcept {
    biginteger_detail::__urandom_exact_impl(&dst, &limit, engine);
}

inline uint32_t bit_width(const biginteger_data &num) noexcept {
    return biginteger_detail::__bit_width_impl(&num);
}

inline uint32_t ctz(const biginteger_data &num) noexcept {
    return biginteger_detail::__ctz_impl(&num);
}

inline uint32_t countr_zero(const biginteger_data &num) noexcept { return ctz(num); }

template <typename S>
void pow(basic_biginteger<S> &dst, const biginteger_data &num, uint32_t exp) noexcept {
    biginteger_detail::__pow_impl(&dst, &num, exp);
}

template <typename S, typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
void pow(basic_biginteger<S> &dst, T num, uint32_t exp) noexcept {
    biginteger_detail::__pow_impl(&dst, num, exp);
}

/**
 * @todo Need to optimize
 */
template <typename S>
void powmod(basic_biginteger<S> &dst, const biginteger_data &num, const biginteger_data &exp,
            const biginteger_data &mod) noexcept {
    biginteger_detail::__powmod_impl(&dst, &num, &exp, &mod);
}

/**
 * @todo Need to optimize
 */
template <typename S>
void powmod(basic_biginteger<S> &dst, const biginteger_data &num, uint64_t exp,
            const biginteger_data &mod) noexcept {
    biginteger_detail::__powmod_impl(&dst, &num, exp, &mod);
}

template <typename Storage>
class basic_biginteger {
    template <typename _Mybase, typename _Tag, typename _Enable>
    friend struct container_of_fn;

public:
    using storage_type = Storage;
    using vector_type = basic_vector<storage_type>;

    using value_type = typename vector_type::value_type;
    using size_type = typename vector_type::size_type;
    using difference_type = typename vector_type::difference_type;
    using reference = typename vector_type::reference;
    using const_reference = typename vector_type::const_reference;
    using pointer = typename vector_type::pointer;
    using const_pointer = typename vector_type::const_pointer;
    using iterator = typename vector_type::iterator;
    using const_iterator = typename vector_type::const_iterator;
    using reverse_iterator = typename vector_type::reverse_iterator;
    using const_reverse_iterator = typename vector_type::const_reverse_iterator;
    using allocator_type = typename vector_type::allocator_type;
    using is_reallocatable = typename storage_type::is_reallocatable;

    static_assert(std::is_same_v<value_type, uint64_t>, "value_type must be uint64_t");
    static_assert(std::is_same_v<pointer, uint64_t *>, "pointer must be uint64_t *");
    static_assert(std::is_same_v<const_pointer, const uint64_t *>,
                  "const_pointer must be const uint64_t *");
    static_assert(std::is_same_v<size_type, uint32_t>, "size_type must be uint32_t");
    static_assert(std::is_same_v<difference_type, int32_t>, "difference_type must be int32_t");

    basic_biginteger() = default;
    basic_biginteger(basic_biginteger &&other) = default;
    basic_biginteger &operator=(basic_biginteger &&other) = default;
    ~basic_biginteger() = default;

    basic_biginteger(const basic_biginteger &other, const allocator_type &al = allocator_type())
        : m_vec(other.m_vec, al) {
        set_ssize(other.get_ssize());
    }

    basic_biginteger(size_type n, in_place_reserve_t, const allocator_type &al = allocator_type())
        : m_vec(n, in_place_reserve, al) {}

    explicit basic_biginteger(const allocator_type &al) : m_vec(al) {}

    basic_biginteger(basic_biginteger &&other, const allocator_type &al)
        : m_vec(std::move(other.m_vec), al) {}

    template <typename UnsignedValue, WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    explicit basic_biginteger(UnsignedValue value, const allocator_type &al = allocator_type())
        : m_vec(value != 0, value, al) {}

    template <typename SignedValue, WJR_REQUIRES(is_nonbool_signed_integral_v<SignedValue>)>
    explicit basic_biginteger(SignedValue value, const allocator_type &al = allocator_type())
        : m_vec(al) {
        if (value != 0) {
            m_vec.emplace_back(value < 0 ? -to_unsigned(value) : to_unsigned(value));
            set_ssize(__fast_conditional_negate<int32_t>(value < 0, 1));
        }
    }

    explicit basic_biginteger(std::string_view str, unsigned int base = 10,
                              const allocator_type &al = allocator_type())
        : m_vec(al) {
        from_string(str, base);
    }

    template <typename OthterStorage>
    explicit basic_biginteger(const basic_biginteger<OthterStorage> &other,
                              const allocator_type &al = allocator_type())
        : m_vec(other.begin(), other.end(), al) {
        set_ssize(other.get_ssize());
    }

    explicit basic_biginteger(const biginteger_data &data,
                              const allocator_type &al = allocator_type())
        : m_vec(data.data(), data.data() + data.size(), al) {
        set_ssize(data.get_ssize());
    }

    basic_biginteger &operator=(const basic_biginteger &other) {
        m_vec = other.m_vec;
        set_ssize(other.get_ssize());
        return *this;
    }

    template <typename UnsignedValue, WJR_REQUIRES(is_nonbool_unsigned_integral_v<UnsignedValue>)>
    basic_biginteger &operator=(UnsignedValue value) {
        clear();
        if (value != 0) {
            m_vec.emplace_back(value);
        }
        return *this;
    }

    template <typename SignedValue, WJR_REQUIRES(is_nonbool_signed_integral_v<SignedValue>)>
    basic_biginteger &operator=(SignedValue value) {
        clear();
        if (value != 0) {
            m_vec.emplace_back(value < 0 ? -to_unsigned(value) : to_unsigned(value));
            set_ssize(__fast_conditional_negate<int32_t>(value < 0, 1));
        }
        return *this;
    }

    basic_biginteger &operator=(std::string_view str) { return from_string(str); }

    template <typename OthterStorage>
    basic_biginteger &operator=(const basic_biginteger<OthterStorage> &other) {
        m_vec.assign(other.begin(), other.end());
        set_ssize(other.get_ssize());
        return *this;
    }

    basic_biginteger &operator=(const biginteger_data &data) {
        if (WJR_UNLIKELY(__get_data() == std::addressof(data))) {
            return *this;
        }

        m_vec.assign(data.data(), data.data() + data.size());
        set_ssize(data.get_ssize());
        return *this;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    explicit operator T() const noexcept {
        if (empty()) {
            return static_cast<T>(0);
        }

        if constexpr (std::is_unsigned_v<T>) {
            return static_cast<T>(front());
        } else {
            const auto ret = front();
            return is_negate() ? -ret : ret;
        }
    }

    basic_biginteger &from_string(std::string_view str, unsigned int base = 10) noexcept {
        (void)from_chars(str.data(), str.data() + str.size(), *this, base);
        return *this;
    }

    allocator_type &get_allocator() noexcept { return m_vec.get_allocator(); }
    const allocator_type &get_allocator() const noexcept { return m_vec.get_allocator(); }

    pointer data() noexcept { return m_vec.data(); }
    const_pointer data() const noexcept { return m_vec.data(); }

    bool empty() const noexcept { return m_vec.empty(); }
    size_type size() const noexcept { return m_vec.size(); }
    size_type capacity() const noexcept { return m_vec.capacity(); }
    bool zero() const noexcept { return empty(); }

    size_type bit_width() const noexcept { return wjr::bit_width(*this); }
    size_type ctz() const noexcept { return wjr::ctz(*this); }
    size_type countr_zero() const noexcept { return ctz(); }

    void reserve(size_type new_capacity) noexcept { m_vec.reserve(new_capacity); }
    void clear_if_reserved(size_type new_capacity) noexcept {
        m_vec.clear_if_reserved(new_capacity);
    }
    void __reserve_unchecked(size_type new_capacity) noexcept {
        m_vec.__reserve_unchecked(new_capacity);
    }

    void shrink_to_fit() { m_vec.shrink_to_fit(); }

    /// equal to set_ssize(0)
    void clear() { set_ssize(0); }

    void swap(basic_biginteger &other) noexcept { m_vec.swap(other.m_vec); }

    void conditional_negate(bool condition) noexcept {
        get_storage()->conditional_negate(condition);
    }

    void negate() noexcept { get_storage()->negate(); }
    bool is_negate() const noexcept { return get_storage()->is_negate(); }
    void absolute() noexcept { get_storage()->absolute(); }

    basic_biginteger &operator++() {
        increment(*this);
        return *this;
    }

    basic_biginteger &operator--() {
        decrement(*this);
        return *this;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    basic_biginteger &operator+=(T rhs) {
        add(*this, *this, rhs);
        return *this;
    }

    basic_biginteger &operator+=(const biginteger_data &rhs) {
        add(*this, *this, rhs);
        return *this;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    basic_biginteger &operator-=(T rhs) {
        sub(*this, *this, rhs);
        return *this;
    }

    basic_biginteger &operator-=(const biginteger_data &rhs) {
        sub(*this, *this, rhs);
        return *this;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    basic_biginteger &operator*=(T rhs) {
        mul(*this, *this, rhs);
        return *this;
    }

    basic_biginteger &operator*=(const biginteger_data &rhs) {
        mul(*this, *this, rhs);
        return *this;
    }

    basic_biginteger &operator<<=(uint32_t shift) {
        mul_2exp(*this, *this, shift);
        return *this;
    }

    // extension

    reference at(size_type pos) { return m_vec.at(pos); }
    const_reference at(size_type pos) const { return m_vec.at(pos); }

    reference operator[](size_type pos) noexcept { return m_vec[pos]; }
    const_reference operator[](size_type pos) const noexcept { return m_vec[pos]; }

    reference front() { return m_vec.front(); }
    const_reference front() const { return m_vec.front(); }

    reference back() { return m_vec.back(); }
    const_reference back() const { return m_vec.back(); }

    iterator begin() noexcept { return m_vec.begin(); }
    const_iterator begin() const noexcept { return m_vec.begin(); }

    iterator end() noexcept { return m_vec.end(); }
    const_iterator end() const noexcept { return m_vec.end(); }

    const_iterator cbegin() const noexcept { return m_vec.cbegin(); }

    const_iterator cend() const noexcept { return m_vec.cend(); }

    reverse_iterator rbegin() noexcept { return m_vec.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return m_vec.rbegin(); }

    reverse_iterator rend() noexcept { return m_vec.rend(); }
    const_reverse_iterator rend() const noexcept { return m_vec.rend(); }

    const_reverse_iterator crbegin() const noexcept { return m_vec.crbegin(); }
    const_reverse_iterator crend() const noexcept { return m_vec.crend(); }

    int32_t get_ssize() const { return get_storage()->get_ssize(); }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    void set_ssize(T new_size) noexcept {
        get_storage()->set_ssize(new_size);
    }

    WJR_CONST static size_type get_growth_capacity(size_type old_capacity,
                                                   size_type new_size) noexcept {
        return vector_type::get_growth_capacity(old_capacity, new_size);
    }

    void take_storage(storage_type &other) noexcept { m_vec.take_storage(other); }

    void uninitialized_construct(storage_type &other, size_type siz, size_type cap) noexcept {
        m_vec.uninitialized_construct(other, siz, cap);
    }

    storage_type &get_storage() noexcept { return m_vec.get_storage(); }
    const storage_type &get_storage() const noexcept { return m_vec.get_storage(); }

    biginteger_data *__get_data() noexcept { return get_storage().__get_data(); }
    const biginteger_data *__get_data() const noexcept { return get_storage().__get_data(); }

    biginteger_data &__get_ref() noexcept { return *__get_data(); }
    const biginteger_data &__get_ref() const noexcept { return *__get_data(); }

    operator biginteger_data &() noexcept { return __get_ref(); }
    operator const biginteger_data &() const noexcept { return __get_ref(); }

private:
    void __check_high_bit() const {
        WJR_ASSERT(size() == 0 || back() != 0, "biginteger should not have leading zero");
    }

    vector_type m_vec;
};

template <typename S>
struct get_relocate_mode<basic_biginteger<S>> {
    static constexpr relocate_t value =
        get_relocate_mode_v<typename basic_biginteger<S>::vector_type>;
};

template <typename S>
struct container_of_fn<basic_biginteger<S>, void,
                       std::enable_if_t<std::is_standard_layout_v<basic_biginteger<S>>>> {
    using base_type = basic_biginteger<S>;
    using value_type = biginteger_data;

    base_type &operator()(value_type &ref) const noexcept {
        auto &__vecctor = container_of<typename base_type::vector_type>(container_of<S>(ref));
        return *reinterpret_cast<base_type *>(
            reinterpret_cast<std::byte *>(std::addressof(__vecctor)) - offsetof(base_type, m_vec));
    }

    const base_type &operator()(const value_type &ref) const noexcept {
        const auto &__vecctor = container_of<typename base_type::vector_type>(container_of<S>(ref));
        return *reinterpret_cast<const base_type *>(
            reinterpret_cast<const std::byte *>(std::addressof(__vecctor)) -
            offsetof(base_type, m_vec));
    }
};

template <typename Storage>
void swap(basic_biginteger<Storage> &lhs, basic_biginteger<Storage> &rhs) noexcept {
    lhs.swap(rhs);
}

namespace biginteger_detail {

inline int32_t __compare_impl(const biginteger_data *lhs, const biginteger_data *rhs) noexcept {
    const auto lssize = lhs->get_ssize();
    const auto rssize = rhs->get_ssize();

    if (lssize != rssize) {
        return lssize < rssize ? -1 : 1;
    }

    const int32_t ans = reverse_compare_n(lhs->data(), rhs->data(), __fast_abs(lssize));
    return lssize < 0 ? -ans : ans;
}

inline int32_t __compare_ui_impl(const biginteger_data *lhs, uint64_t rhs) noexcept {
    const auto lssize = lhs->get_ssize();

    if (WJR_UNLIKELY(lssize == 0)) {
        return -(rhs != 0);
    }

    if (lssize == 1) {
        const uint64_t lvalue = lhs->data()[0];
        return (lvalue != rhs ? (lvalue < rhs ? -1 : 1) : 0);
    }

    return lssize;
}

inline int32_t __compare_si_impl(const biginteger_data *lhs, int64_t rhs) noexcept {
    const auto lssize = lhs->get_ssize();
    const int32_t rssize = rhs == 0 ? 0 : __fast_conditional_negate<int32_t>(rhs < 0, 1);

    if (lssize != rssize) {
        return lssize - rssize;
    }

    if (lssize == 0) {
        return 0;
    }

    const uint64_t lvalue = lhs->data()[0];
    const uint64_t rvalue = rhs >= 0 ? to_unsigned(rhs) : -to_unsigned(rhs);

    if (lvalue == rvalue) {
        return 0;
    }

    if (lvalue > rvalue) {
        return lssize;
    }

    return -lssize;
}

template <typename S>
void __ui_sub_impl(basic_biginteger<S> *dst, uint64_t lhs, const biginteger_data *rhs) noexcept {
    const auto rssize = rhs->get_ssize();
    if (rssize == 0) {
        dst->clear_if_reserved(1);

        if (lhs == 0) {
            dst->set_ssize(0);
        } else {
            dst->set_ssize(1);
            dst->front() = lhs;
        }

        return;
    }

    const uint32_t rusize = __fast_abs(rssize);
    dst->clear_if_reserved(rusize);

    auto *const dp = dst->data();
    const auto *const rp = rhs->data();
    int32_t dssize;

    if (rssize < 0) {
        const auto cf = addc_1(dp, rp, rusize, lhs);
        dssize = rusize + cf;
        if (cf) {
            dp[rusize] = 1;
        }
    } else {
        // lhs >= rhs
        if (rusize == 1 && lhs >= rp[0]) {
            dp[0] = lhs - rp[0];
            dssize = dp[0] != 0;
        }
        // lhs < rhs
        else {
            (void)subc_1(dp, rp, rusize, lhs);
            dssize = __fast_negate<int32_t>(rusize - (dp[rusize - 1] == 0));
        }
    }

    dst->set_ssize(dssize);
}

template <typename S>
void __mul_ui_impl(basic_biginteger<S> *dst, const biginteger_data *lhs, uint64_t rhs) noexcept {
    const auto lssize = lhs->get_ssize();
    const uint32_t lusize = __fast_abs(lssize);

    if (lusize == 0 || rhs == 0) {
        dst->set_ssize(0);
        return;
    }

    dst->reserve(lusize + 1);

    auto *const dp = dst->data();
    const auto *const lp = lhs->data();

    const auto cf = mul_1(dp, lp, lusize, rhs);
    const uint32_t dusize = lusize + (cf != 0);
    if (cf != 0) {
        dp[lusize] = cf;
    }

    dst->set_ssize(__fast_conditional_negate<int32_t>(lssize < 0, dusize));
}

template <typename S>
void __mul_2exp_impl(basic_biginteger<S> *dst, const biginteger_data *lhs,
                     uint32_t shift) noexcept {
    const int32_t lssize = lhs->get_ssize();

    if (lssize == 0) {
        dst->set_ssize(0);
        return;
    }

    const uint32_t lusize = __fast_abs(lssize);
    const uint32_t offset = shift / 64;
    shift %= 64;
    uint32_t dusize = lusize + offset;

    dst->reserve(dusize + 1);
    auto *const dp = dst->data();
    const auto *const lp = lhs->data();

    const auto cf = lshift_n(dp + offset, lp, lusize, shift);
    set_n(dp, 0, offset);

    dp[dusize] = cf;
    dusize += (cf != 0);

    dst->set_ssize(__fast_conditional_negate<int32_t>(lssize < 0, dusize));
}

template <typename S0, typename S1>
uint64_t __tdiv_qr_ui_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                           const biginteger_data *num, uint64_t div) noexcept {
    const auto nssize = num->get_ssize();

    WJR_ASSERT(div != 0, "division by zero");

    if (nssize == 0) {
        quot->set_ssize(0);
        rem->set_ssize(0);
        return 0;
    }

    const auto nusize = __fast_abs(nssize);
    quot->reserve(nusize);
    const auto qp = quot->data();
    const auto np = num->data();

    uint64_t remv;
    div_qr_1(qp, remv, np, nusize, div);

    if (remv == 0) {
        rem->set_ssize(0);
    } else {
        rem->reserve(1);
        rem->set_ssize(__fast_conditional_negate<int32_t>(nssize < 0, 1));
        rem->front() = remv;
    }

    const auto qusize = nusize - (qp[nusize - 1] == 0);

    quot->set_ssize(__fast_conditional_negate<int32_t>(nssize < 0, qusize));
    return remv;
}

template <typename S>
uint64_t __tdiv_q_ui_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                          uint64_t div) noexcept {
    const auto nssize = num->get_ssize();

    WJR_ASSERT(div != 0, "division by zero");

    if (nssize == 0) {
        quot->set_ssize(0);
        return 0;
    }

    const auto nusize = __fast_abs(nssize);
    quot->reserve(nusize);
    const auto qp = quot->data();
    const auto np = num->data();

    uint64_t remv;
    div_qr_1(qp, remv, np, nusize, div);

    const auto qusize = nusize - (qp[nusize - 1] == 0);

    quot->set_ssize(__fast_conditional_negate<int32_t>(nssize < 0, qusize));
    return remv;
}

template <typename S>
uint64_t __tdiv_r_ui_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                          uint64_t div) noexcept {
    const auto nssize = num->get_ssize();

    WJR_ASSERT(div != 0, "division by zero");

    if (nssize == 0) {
        rem->set_ssize(0);
        return 0;
    }

    const auto nusize = __fast_abs(nssize);
    const auto np = num->data();

    uint64_t remv;
    remv = mod_1(np, nusize, div);

    if (remv == 0) {
        rem->set_ssize(0);
    } else {
        rem->reserve(1);
        rem->set_ssize(__fast_conditional_negate<int32_t>(nssize < 0, 1));
        rem->front() = remv;
    }

    return remv;
}

template <typename S0, typename S1, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __tdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                        const biginteger_data *num, T div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        return __tdiv_qr_ui_impl(quot, rem, num, div);
    } else {
        uint64_t udiv = to_unsigned(div);
        bool xsign = false;
        if (div < 0) {
            udiv = -udiv;
            xsign = true;
        }

        uint64_t remv = __tdiv_qr_ui_impl(quot, rem, num, udiv);

        quot->conditional_negate(xsign);
        return remv;
    }
}

template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __tdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num, T div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        return __tdiv_q_ui_impl(quot, num, div);
    } else {
        uint64_t udiv = to_unsigned(div);
        bool xsign = false;
        if (div < 0) {
            udiv = -udiv;
            xsign = true;
        }

        uint64_t remv = __tdiv_q_ui_impl(quot, num, udiv);

        quot->conditional_negate(xsign);
        return remv;
    }
}

template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __tdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                       uint64_t div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        return __tdiv_q_ui_impl(rem, num, div);
    } else {
        uint64_t udiv = to_unsigned(div);
        if (div < 0) {
            udiv = -udiv;
        }

        return __tdiv_r_ui_impl(rem, num, udiv);
    }
}

template <typename S0, typename S1>
void __fdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                    const biginteger_data *num, const biginteger_data *div) noexcept {

    WJR_ASSERT_ASSUME(!__equal_pointer(quot, rem), "quot should not be the same as rem");

    unique_stack_allocator stkal;
    biginteger_data tmp_div;

    const auto dssize = div->get_ssize();

    if (__equal_pointer(div, quot) || __equal_pointer(div, rem)) {
        const auto dusize = __fast_abs(dssize);
        auto *const ptr = stkal.template allocate<uint64_t>(dusize);
        tmp_div = {ptr, dssize, 0};
        copy_n_restrict(div->data(), dusize, ptr);
        div = &tmp_div;
    }

    const auto xsize = num->get_ssize() ^ dssize;

    __tdiv_qr_impl(quot, rem, num, div);

    if (xsize < 0 && !rem->empty()) {
        __sub_impl(quot, quot->__get_data(), 1u);
        __add_impl(rem, rem->__get_data(), div);
    }
}

template <typename S>
void __fdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                   const biginteger_data *div) noexcept {
    unique_stack_allocator stkal;
    weak_stack_biginteger rem(stkal);

    const auto xsize = num->get_ssize() ^ div->get_ssize();

    __tdiv_qr_impl(quot, &rem, num, div);

    if (xsize < 0 && !rem.empty()) {
        __sub_impl(quot, quot->__get_data(), 1u);
    }
}

template <typename S>
void __fdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                   const biginteger_data *div) noexcept {
    unique_stack_allocator stkal;
    biginteger_data tmp_div;

    const auto dssize = div->get_ssize();

    if (__equal_pointer(div, rem)) {
        const auto dusize = __fast_abs(dssize);
        auto *const ptr = stkal.template allocate<uint64_t>(dusize);
        tmp_div = {ptr, dssize, 0};
        copy_n_restrict(div->data(), dusize, ptr);
        div = &tmp_div;
    }

    const auto xsize = num->get_ssize() ^ dssize;

    __tdiv_r_impl(rem, num, div);

    if (xsize < 0 && !rem->empty()) {
        __add_impl(rem, rem->__get_data(), div);
    }
}

template <typename S0, typename S1, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __fdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                        const biginteger_data *num, T div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        const auto xssize = num->get_ssize();

        uint64_t remv = __tdiv_qr_ui_impl(quot, rem, num, div);

        if (xssize < 0 && remv != 0) {
            WJR_ASSERT(rem->is_negate());

            __sub_impl(quot, quot->__get_data(), 1u);
            rem->set_ssize(1);
            remv = div - remv;
            rem->front() = remv;
        }

        return remv;
    } else {
        uint64_t udiv = to_unsigned(div);
        int32_t xsign = 0;

        if (div < 0) {
            udiv = -udiv;
            xsign = -1;
        }

        const int32_t nssize = num->get_ssize();

        uint64_t remv = __tdiv_qr_ui_impl(quot, rem, num, udiv);

        quot->conditional_negate(xsign);

        if ((nssize ^ xsign) < 0 && remv != 0) {
            __sub_impl(quot, quot->__get_data(), 1u);
            rem->set_ssize(__fast_conditional_negate(xsign < 0, 1));
            remv = div - remv;
            rem->front() = remv;
        }

        return remv;
    }
}

template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __fdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num, T div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        const auto xssize = num->get_ssize();

        uint64_t remv = __tdiv_q_ui_impl(quot, num, div);

        if (xssize < 0 && remv != 0) {
            __sub_impl(quot, quot->__get_data(), 1u);
            remv = div - remv;
        }

        return remv;
    } else {
        uint64_t udiv = to_unsigned(div);
        int32_t xsign = 0;

        if (div < 0) {
            udiv = -udiv;
            xsign = -1;
        }

        const int32_t nssize = num->get_ssize();

        uint64_t remv = __tdiv_q_ui_impl(quot, num, udiv);

        quot->conditional_negate(xsign);

        if ((nssize ^ xsign) < 0 && remv != 0) {
            __sub_impl(quot, quot->__get_data(), 1u);
            remv = div - remv;
        }

        return remv;
    }
}

template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __fdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                       uint64_t div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        const auto xssize = num->get_ssize();

        uint64_t remv = __tdiv_r_ui_impl(rem, num, div);

        if (xssize < 0 && remv != 0) {
            WJR_ASSERT(rem->is_negate());

            rem->set_ssize(1);
            remv = div - remv;
            rem->front() = remv;
        }

        return remv;
    } else {
        uint64_t udiv = to_unsigned(div);
        int32_t xsign = 0;

        if (div < 0) {
            udiv = -udiv;
            xsign = -1;
        }

        const int32_t nssize = num->get_ssize();

        uint64_t remv = __tdiv_r_ui_impl(rem, num, udiv);

        if ((nssize ^ xsign) < 0 && remv != 0) {
            rem->set_ssize(__fast_conditional_negate(xsign < 0, 1));
            remv = div - remv;
            rem->front() = remv;
        }

        return remv;
    }
}

template <typename S0, typename S1>
void __cdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                    const biginteger_data *num, const biginteger_data *div) noexcept {
    WJR_ASSERT_ASSUME(!__equal_pointer(quot, rem), "quot should not be the same as rem");

    unique_stack_allocator stkal;
    biginteger_data tmp_div;

    const auto dssize = div->get_ssize();

    if (__equal_pointer(div, quot) || __equal_pointer(div, rem)) {
        const auto dusize = __fast_abs(dssize);
        auto *const ptr = stkal.template allocate<uint64_t>(dusize);
        tmp_div = {ptr, dssize, 0};
        copy_n_restrict(div->data(), dusize, ptr);
        div = &tmp_div;
    }

    const auto xsize = num->get_ssize() ^ dssize;

    __tdiv_qr_impl(quot, rem, num, div);

    if (xsize >= 0 && !rem->empty()) {
        __add_impl(quot, quot->__get_data(), 1u);
        __sub_impl(rem, rem->__get_data(), div);
    }
}

template <typename S>
void __cdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                   const biginteger_data *div) noexcept {
    unique_stack_allocator stkal;
    weak_stack_biginteger rem(stkal);

    const auto xsize = num->get_ssize() ^ div->get_ssize();

    __tdiv_qr_impl(quot, &rem, num, div);

    if (xsize >= 0 && !rem.empty()) {
        __add_impl(quot, quot->__get_data(), 1u);
    }
}

template <typename S>
void __cdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                   const biginteger_data *div) noexcept {
    unique_stack_allocator stkal;
    biginteger_data tmp_div;

    const auto dssize = div->get_ssize();

    if (__equal_pointer(div, rem)) {
        const auto dusize = __fast_abs(dssize);
        auto *const ptr = stkal.template allocate<uint64_t>(dusize);
        tmp_div = {ptr, dssize, 0};
        copy_n_restrict(div->data(), dusize, ptr);

        div = &tmp_div;
    }

    const auto xsize = num->get_ssize() ^ dssize;

    __tdiv_r_impl(rem, num, div);

    if (xsize >= 0 && !rem->empty()) {
        __sub_impl(rem, rem->__get_data(), div);
    }
}

template <typename S0, typename S1, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __cdiv_qr_impl(basic_biginteger<S0> *quot, basic_biginteger<S1> *rem,
                        const biginteger_data *num, T div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        const auto xssize = num->get_ssize();

        uint64_t remv = __tdiv_qr_ui_impl(quot, rem, num, div);

        if (xssize >= 0 && remv != 0) {
            WJR_ASSERT(rem->is_negate());

            __add_impl(quot, quot->__get_data(), 1u);
            rem->set_ssize(-1);
            remv = div - remv;
            rem->front() = remv;
        }

        return remv;
    } else {
        uint64_t udiv = to_unsigned(div);
        int32_t xsign = 0;

        if (div < 0) {
            udiv = -udiv;
            xsign = -1;
        }

        const int32_t nssize = num->get_ssize();

        uint64_t remv = __tdiv_qr_ui_impl(quot, rem, num, udiv);

        quot->conditional_negate(xsign);

        if ((nssize ^ xsign) >= 0 && remv != 0) {
            __add_impl(quot, quot->__get_data(), 1u);
            rem->set_ssize(__fast_conditional_negate(xsign >= 0, 1));
            remv = div - remv;
            rem->front() = remv;
        }

        return remv;
    }
}

template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __cdiv_q_impl(basic_biginteger<S> *quot, const biginteger_data *num, T div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        const auto xssize = num->get_ssize();

        uint64_t remv = __tdiv_q_ui_impl(quot, num, div);

        if (xssize >= 0 && remv != 0) {
            __add_impl(quot, quot->__get_data(), 1u);
            remv = div - remv;
        }

        return remv;
    } else {
        uint64_t udiv = to_unsigned(div);
        int32_t xsign = 0;

        if (div < 0) {
            udiv = -udiv;
            xsign = -1;
        }

        const int32_t nssize = num->get_ssize();

        uint64_t remv = __tdiv_q_ui_impl(quot, num, udiv);

        quot->conditional_negate(xsign);

        if ((nssize ^ xsign) >= 0 && remv != 0) {
            __add_impl(quot, quot->__get_data(), 1u);
            remv = div - remv;
        }

        return remv;
    }
}

template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
uint64_t __cdiv_r_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                       uint64_t div) noexcept {
    if constexpr (std::is_unsigned_v<T>) {
        const auto xssize = num->get_ssize();

        uint64_t remv = __tdiv_r_ui_impl(rem, num, div);

        if (xssize >= 0 && remv != 0) {
            WJR_ASSERT(rem->is_negate());

            rem->set_ssize(-1);
            remv = div - remv;
            rem->front() = remv;
        }

        return remv;
    } else {
        uint64_t udiv = to_unsigned(div);
        int32_t xsign = 0;

        if (div < 0) {
            udiv = -udiv;
            xsign = -1;
        }

        const int32_t nssize = num->get_ssize();

        uint64_t remv = __tdiv_r_ui_impl(rem, num, udiv);

        if ((nssize ^ xsign) >= 0 && remv != 0) {
            rem->set_ssize(__fast_conditional_negate(xsign >= 0, 1));
            remv = div - remv;
            rem->front() = remv;
        }

        return remv;
    }
}

template <typename S>
void __tdiv_q_2exp_impl(basic_biginteger<S> *quot, const biginteger_data *num,
                        uint32_t shift) noexcept {
    int32_t nssize = num->get_ssize();
    uint32_t nusize = __fast_abs(nssize);
    uint32_t offset = shift / 64;

    int32_t qssize = nusize - offset;

    if (qssize <= 0) {
        quot->set_ssize(0);
        return;
    }

    quot->reserve(qssize);
    const auto qp = quot->data();
    const auto np = num->data();

    (void)rshift_n(qp, np + offset, qssize, shift % 64);
    qssize -= qp[qssize - 1] == 0;

    quot->set_ssize(__fast_conditional_negate<int32_t>(nssize < 0, qssize));
}

template <typename S>
void __tdiv_r_2exp_impl(basic_biginteger<S> *rem, const biginteger_data *num,
                        uint32_t shift) noexcept {
    const int32_t nssize = num->get_ssize();
    uint32_t nusize = __fast_abs(nssize);
    uint32_t offset = shift / 64;

    uint32_t rusize;

    if (nusize <= offset) {
        rusize = nusize;
        offset = nusize;
        rem->reserve(rusize);
    } else {
        uint64_t high = num->data()[offset] & (((uint64_t)(1) << (shift % 64)) - 1);
        if (high != 0) {
            rusize = offset + 1;
            rem->reserve(rusize);
            rem->data()[offset] = high;
        } else {
            rusize = normalize(num->data(), offset);
            offset = rusize;
            rem->reserve(rusize);
        }
    }

    if (!__equal_pointer(rem, num)) {
        std::copy_n(num->data(), offset, rem->data());
    }

    rem->set_ssize(__fast_conditional_negate<int32_t>(nssize < 0, rusize));
}

template <typename S>
void __cfdiv_q_2exp_impl(basic_biginteger<S> *quot, const biginteger_data *num, uint32_t shift,
                         int32_t xdir) noexcept {
    int32_t nssize = num->get_ssize();
    uint32_t nusize = __fast_abs(nssize);
    uint32_t offset = shift / 64;

    int32_t qssize = nusize - offset;

    if (qssize <= 0) {
        if (nssize == 0) {
            quot->set_ssize(0);
            return;
        }

        quot->clear_if_reserved(1);
        quot->data()[0] = 1;

        quot->set_ssize((nssize ^ xdir) < 0 ? 0 : xdir);
        return;
    }

    quot->reserve(qssize + 1);
    const auto qp = quot->data();
    const auto np = num->data();

    uint64_t xmask = (nssize ^ xdir) < 0 ? 0 : UINT64_MAX;
    uint64_t round = 0;

    if (xmask) {
        // all is zero, then round is zero
        round = find_not_n(np, 0, offset) == offset ? 0 : 1;
    }

    round |= xmask & rshift_n(qp, np + offset, qssize, shift % 64);
    qssize -= qp[qssize - 1] == 0;

    if (WJR_LIKELY(round != 0)) {
        if (WJR_LIKELY(qssize != 0)) {
            const auto cf = addc_1(qp, qp, qssize, 1u);
            if (cf != 0) {
                qp[qssize] = cf;
                ++qssize;
            }
        } else {
            qp[0] = 1;
            qssize = 1;
        }
    }

    quot->set_ssize(__fast_conditional_negate<int32_t>(nssize < 0, qssize));
}

template <typename S, typename Engine,
          WJR_REQUIRES_I(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_bit_impl(basic_biginteger<S> *dst, uint32_t size, Engine &engine) noexcept {
    const uint32_t dusize = size / 64;
    size %= 64;
    uint32_t dssize = dusize + (size != 0);

    dst->reserve(dssize);
    const auto dp = dst->data();

    do {
        for (uint64_t i = 0; i < dusize; ++i) {
            dp[i] = engine();
        }
    } while (0);

    if (size != 0) {
        dp[dusize] = engine() >> (64 - size);
    }

    dssize = normalize(dp, dssize);
    dst->set_ssize(dssize);
}

template <typename S, typename Engine,
          WJR_REQUIRES_I(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_exact_bit_impl(basic_biginteger<S> *dst, uint32_t size, Engine &engine) noexcept {
    if (WJR_UNLIKELY(size == 0)) {
        dst->set_ssize(0);
        return;
    }

    const uint32_t dusize = (size - 1) / 64;
    size = (size - 1) % 64;
    const uint32_t dssize = dusize + 1;

    dst->reserve(dssize);
    const auto dp = dst->data();

    do {
        for (uint64_t i = 0; i < dusize; ++i) {
            dp[i] = engine();
        }
    } while (0);

    do {
        uint64_t high = (uint64_t)(1) << size;

        if (size != 0) {
            high |= engine() >> (64 - size);
        }

        dp[dusize] = high;
    } while (0);

    dst->set_ssize(dssize);
}

/// @private
template <typename Engine, WJR_REQUIRES_I(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_impl(biginteger_dispatcher dst, const biginteger_data *limit,
                    Engine &engine) noexcept {
    uint32_t size = limit->size();

    WJR_ASSERT(size != 0);

    if (WJR_UNLIKELY(size == 1)) {
        std::uniform_int_distribution<uint64_t> head(0, limit->data()[0] - 1);
        const uint64_t gen = head(engine);

        dst.clear_if_reserved(1);
        dst.data()[0] = gen;
        dst.set_ssize(gen == 0 ? 0 : 1);
        return;
    }

    dst.reserve(size);

    const auto dp = dst.data();
    const uint64_t *lp = limit->data();

    unique_stack_allocator stkal;

    if (__equal_pointer(dst.raw(), limit)) {
        auto *const tp = stkal.template allocate<uint64_t>(size);
        copy_n_restrict(lp, size, tp);
        lp = tp;
    }

    const uint32_t lst_pos = size - 1;
    const uint64_t lst_val = lp[lst_pos];
    std::uniform_int_distribution<uint64_t> head(0, lst_val);

    while (true) {
        uint64_t gen = head(engine);
        uint32_t pos = lst_pos;

        if (WJR_UNLIKELY(gen == lst_val)) {
            uint64_t now;
            do {
                dp[pos--] = gen;
                gen = engine();
                now = lp[pos];

                if (WJR_LIKELY(gen != now)) {
                    goto NEXT;
                }

            } while (pos);

            continue;

        NEXT:

            if (gen > now) {
                continue;
            }
        }

        dp[pos] = gen;
        while (pos) {
            dp[--pos] = engine();
        }

        break;
    }

    size = normalize(dp, size);
    dst.set_ssize(size);
}

template <typename S, typename Engine,
          WJR_REQUIRES_I(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_impl(basic_biginteger<S> *dst, const biginteger_data *limit,
                    Engine &engine) noexcept {
    __urandom_impl(biginteger_dispatcher(dst), limit, engine);
}

template <typename Engine, WJR_REQUIRES_I(biginteger_uniform_random_bit_generator_v<Engine>)>
void __urandom_exact_impl(biginteger_dispatcher dst, const biginteger_data *limit,
                          Engine &engine) noexcept {
    uint32_t size = limit->size();

    if (WJR_UNLIKELY(size <= 1)) {
        if (size == 0) {
            dst.set_ssize(0);
            return;
        }

        std::uniform_int_distribution<uint64_t> head(0, limit->data()[0]);
        const uint64_t gen = head(engine);

        dst.clear_if_reserved(1);
        dst.data()[0] = gen;
        dst.set_ssize(gen == 0 ? 0 : 1);
        return;
    }

    dst.reserve(size);

    const auto dp = dst.data();
    const uint64_t *lp = limit->data();

    unique_stack_allocator stkal;

    if (__equal_pointer(dst.raw(), limit)) {
        auto *const tp = stkal.template allocate<uint64_t>(size);
        copy_n_restrict(lp, size, tp);
        lp = tp;
    }

    const uint32_t lst_pos = size - 1;
    const uint64_t lst_val = lp[lst_pos];
    std::uniform_int_distribution<uint64_t> head(0, lst_val);

    while (true) {
        uint64_t gen = head(engine);
        uint32_t pos = lst_pos;

        if (WJR_UNLIKELY(gen == lst_val)) {
            uint64_t now;
            do {
                dp[pos--] = gen;
                gen = engine();
                now = lp[pos];
            } while (gen == now && pos);

            if (gen > now) {
                continue;
            }
        }

        dp[pos] = gen;

        while (pos) {
            dp[--pos] = engine();
        }

        break;
    }

    size = normalize(dp, size);
    dst.set_ssize(size);
}

/// @private
inline uint32_t __bit_width_impl(const biginteger_data *num) noexcept {
    const uint32_t size = num->size();
    if (WJR_UNLIKELY(size == 0)) {
        return 0;
    }

    return 64 * size - clz(num->data()[size - 1]);
}

/// @private
inline uint32_t __ctz_impl(const biginteger_data *num) noexcept {
    if (num->empty()) {
        return 0;
    }

    // can be optimize by using SIMD

    const auto *const ptr = num->data();
    const uint32_t size = num->size();
    const uint32_t idx = static_cast<uint32_t>(find_not_n(ptr, 0, size));
    WJR_ASSERT(idx != size);
    return idx * 64 + ctz(ptr[idx]);
}

template <typename S, typename T, WJR_REQUIRES_I(is_nonbool_integral_v<T>)>
void __pow_impl(basic_biginteger<S> *dst, T num, uint32_t exp) noexcept {
    inplace_biginteger<1> __num(num);
    __pow_impl(dst, __num.__get_data(), exp);
}

/// @private
template <typename S>
void __powmod_impl(basic_biginteger<S> *dst, const biginteger_data *num, __powmod_iterator *iter,
                   const biginteger_data *mod) noexcept {
    WJR_ASSERT(mod->size() != 0);

    const auto size = iter->size;

    if (size == 0) {
        uint64_t val;
        if (WJR_UNLIKELY(mod->size() == 1 && mod->data()[0] == 1)) {
            val = 0u;
        } else {
            val = 1u;
        }

        *dst = val;
        return;
    }

    tdiv_r(*dst, *num, *mod);

    while (!(iter->get() & 1)) {
        sqr(*dst, *dst);
        tdiv_r(*dst, *dst, *mod);
        iter->next();
    }

    if (iter->end()) {
        return;
    }

    basic_biginteger<S> tmp;
    iter->next();
    sqr(tmp, *dst);
    tdiv_r(tmp, tmp, *mod);

    if (iter->get() & 1) {
        mul(*dst, *dst, tmp);
        tdiv_r(*dst, *dst, *mod);
    }

    if (!iter->end()) {
        do {
            iter->next();
            sqr(tmp, tmp);
            tdiv_r(tmp, tmp, *mod);

            if (iter->get() & 1) {
                mul(*dst, *dst, tmp);
                tdiv_r(*dst, *dst, *mod);
            }
        } while (!iter->end());
    }
}

template <typename S>
void __powmod_impl(basic_biginteger<S> *dst, const biginteger_data *num, const biginteger_data *exp,
                   const biginteger_data *mod) noexcept {
    __powmod_iterator iter(exp->data(), exp->size());
    __powmod_impl(dst, num, &iter, mod);
}

template <typename S>
void __powmod_impl(basic_biginteger<S> *dst, const biginteger_data *num, uint64_t exp,
                   const biginteger_data *mod) noexcept {
    uint64_t tmp[] = {exp};
    __powmod_iterator iter(tmp, exp == 0 ? 0 : 1);
    __powmod_impl(dst, num, &iter, mod);
}

} // namespace biginteger_detail

template <typename S>
from_chars_result<const char *> from_chars(const char *first, const char *last,
                                           basic_biginteger<S> &dst, unsigned int base) noexcept {
    return biginteger_detail::__from_chars_impl<true>(first, last, &dst, base);
}

template <typename S>
from_chars_result<const char *> from_chars_characters_unchecked(const char *first, const char *last,
                                                                basic_biginteger<S> &dst,
                                                                unsigned int base) noexcept {
    return biginteger_detail::__from_chars_impl<false>(first, last, &dst, base);
}

template <typename S>
void negate(basic_biginteger<S> &dst) noexcept {
    dst.negate();
}

template <typename S>
void absolute(basic_biginteger<S> &dst) noexcept {
    dst.absolute();
}

/// @todo Need to check when read string. But that's so slow!
template <typename Traits, typename S>
std::basic_istream<char, Traits> &operator>>(std::basic_istream<char, Traits> &is,
                                             basic_biginteger<S> &dst) noexcept {
    std::basic_string<char, Traits, std::allocator<char>> str;
    is >> str;
    from_chars(str.data(), str.data() + str.size(), dst, 0);
    return is;
}

template <typename Traits>
std::basic_ostream<char, Traits> &operator<<(std::basic_ostream<char, Traits> &os,
                                             const biginteger_data &src) noexcept {
    if (const std::ostream::sentry ok(os); ok) {
        unique_stack_allocator stkal;
        std::basic_string<char, Traits, weak_stack_allocator<char>> buffer(stkal);
        buffer.reserve(256);

        const std::ios_base::fmtflags flags = os.flags();

        if ((flags & std::ios::showpos) && !src.is_negate()) {
            buffer.push_back('+');
        }

        int base = 10;

        if (const auto basefield = flags & std::ios::basefield; basefield != 0) {
            if (basefield == std::ios::oct) {
                base = 8;
                if (flags & std::ios::showbase) {
                    buffer.append({'0'});
                }
            } else if (basefield == std::ios::hex) {
                base = 16;
                if (flags & std::ios::showbase) {
                    buffer.append({'0', 'x'});
                }
            }
        }

        (void)to_chars_unchecked(std::back_inserter(buffer), src, base);
        // seems won't be empty
        WJR_ASSERT(!buffer.empty());
        __ostream_insert_unchecked(os, buffer.data(), buffer.size());
    } else {
        os.setstate(std::ios::badbit);
    }

    return os;
}

extern std::ostream &operator<<(std::ostream &os, const biginteger_data &src) noexcept;

} // namespace wjr

namespace fmt {
template <>
struct formatter<wjr::biginteger_data> {
private:
    detail::dynamic_format_specs<char> m_specs;

public:
    FMT_CONSTEXPR auto parse(parse_context<char> &ctx) -> const char * {
        if (ctx.begin() == ctx.end() || *ctx.begin() == '}')
            return ctx.begin();
        auto end = detail::parse_format_specs(ctx.begin(), ctx.end(), m_specs, ctx,
                                              detail::type::int128_type);
        if (m_specs.upper()) {
            report_error("Upper letters are temporarily not supported");
        }
        return end;
    }

private:
    void do_format(const wjr::biginteger_data &value,
                   wjr::vector<char, wjr::weak_stack_allocator<char>> &buffer) const;

public:
    template <typename Context>
    auto format(const wjr::biginteger_data &value, Context &ctx) const -> decltype(ctx.out()) {
        wjr::unique_stack_allocator stkal;
        wjr::vector<char, wjr::weak_stack_allocator<char>> buffer(stkal);
        do_format(value, buffer);
        std::string_view data(buffer.data(), buffer.size());
        auto specs = format_specs(m_specs);
        if (m_specs.dynamic()) {
            detail::handle_dynamic_spec(specs.dynamic_width(), specs.width, m_specs.width_ref, ctx);
        }
        return detail::write_bytes<char>(ctx.out(), data, specs);
    }
};

template <typename S>
struct formatter<wjr::basic_biginteger<S>> : formatter<wjr::biginteger_data> {};
} // namespace fmt

namespace std {

template <typename Storage>
constexpr void swap(wjr::basic_biginteger<Storage> &lhs,
                    wjr::basic_biginteger<Storage> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

} // namespace std

#endif