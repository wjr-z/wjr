#pragma once
#ifndef __WJR_SMART_POINTER_WRAPPER_H
#define __WJR_SMART_POINTER_WRAPPER_H

#include <memory>

#include <wjr/compressed_pair.h>
#include <wjr/algorithm.h>

_WJR_BEGIN

//template<typename T, typename Alloc>
//class smart_pointer_wrapper;
//
//template<typename T, typename D, typename Alloc>
//class smart_pointer_wrapper<std::unique_ptr<T, D>, Alloc> {
//    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
//    using _Alty_traits = std::allocator_traits<_Alty>;
//    using _Ptr = std::unique_ptr<T, D>;
//public:
//    using value_type = T;
//    using allocator_type = Alloc;
//    using pointer = T*;
//    using reference = T&;
//    using const_pointer = const T*;
//    using const_reference = const T&;
//
//    static_assert(std::is_same_v<typename std::allocator_traits<Alloc>::value_type, value_type>, "");
//
//    constexpr smart_pointer_wrapper() noexcept(std::is_nothrow_default_constructible_v<_Alty>
//        && std::is_nothrow_default_constructible_v<T>)
//        : m_value(std::piecewise_construct_t(),
//            std::forward_as_tuple(),
//            std::forward_as_tuple()) {
//        getPtr() = getAllocator().allocate(1);
//        construct_at(getAllocator(), getPtr().get(), default_construct_tag());
//    }
//
//    constexpr smart_pointer_wrapper(const allocator_type& al) noexcept(
//        std::is_nothrow_constructible_v<_Alty, const allocator_type&>
//        && std::is_nothrow_default_constructible_v<T>)
//        : m_value(std::piecewise_construct_t(),
//            std::forward_as_tuple(al),
//            std::forward_as_tuple()) {
//        getPtr() = getAllocator().allocate(1);
//        construct_at(getAllocator(), getPtr().get(), default_construct_tag());
//    }
//
//    constexpr smart_pointer_wrapper(const smart_pointer_wrapper& other, 
//        const allocator_type& al = allocator_type()) 
//        noexcept(std::is_nothrow_constructible_v<_Alty, const allocator_type&>
//            && std::is_nothrow_copy_constructible_v<T>) : m_value(std::piecewise_construct_t(),
//                std::forward_as_tuple(al),
//                std::forward_as_tuple()) {
//        getPtr() = getAllocator().allocate(1);
//        construct_at(getAllocator(), getPtr().get(), other.get());
//    }
//
//    constexpr smart_pointer_wrapper(smart_pointer_wrapper&& other) noexcept 
//        : m_value(std::piecewise_construct_t(),
//            std::forward_as_tuple(std::move(other.getAllocator())),
//            std::forward_as_tuple()) {
//        getPtr() = getAllocator().allocate(1);
//        construct_at(getAllocator(), getPtr().get(), std::move(other.get()));
//    }
//
//    constexpr sptr_wrapper(sptr_wrapper&&) noexcept = default;
//    template<typename...Args, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
//    constexpr sptr_wrapper(Args&&...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
//        : m_ptr(mallocate_unique<T>(mallocator<T>{}, std::forward<Args>(args)...)) {}
//
//    template<std::enable_if_t<std::is_copy_constructible_v<T>, int> = 0>
//    constexpr sptr_wrapper& operator=(const sptr_wrapper& other) {
//        if (unlikely(this == std::addressof(other))) {
//            return *this;
//        }
//        if constexpr (std::is_copy_assignable_v<T>) {
//            if (!m_ptr) {
//                m_ptr = mallocate_unique<T>(mallocator<T>{}, other.get());
//            }
//            else {
//                *m_ptr = *other.m_ptr;
//            }
//        }
//        else {
//            m_ptr = mallocate_unique<T>(mallocator<T>{}, other.get());
//        }
//        return *this;
//    }
//
//    constexpr sptr_wrapper& operator=(sptr_wrapper&& other) noexcept = default;
//    template<typename U, std::enable_if_t<std::is_constructible_v<T, U>, int> = 0>
//    constexpr sptr_wrapper& operator=(U&& other) {
//        if constexpr (std::is_assignable_v<T, U>) {
//            if (!m_ptr) {
//                m_ptr = mallocate_unique<T>(mallocator<T>{}, std::forward<U>(other));
//            }
//            else {
//                *m_ptr = std::forward<U>(other);
//            }
//        }
//        else {
//            m_ptr = mallocate_unique<T>(mallocator<T>{}, std::forward<U>(other));
//        }
//        return *this;
//    }
//
//    constexpr _Alty& getAllocator() { return m_vlaue.first(); }
//    constexpr const _Alty& getAllocator() const { return m_value.first(); }
//
//    constexpr _Ptr& getPtr() { return m_value.second(); }
//    constexpr const _Ptr& getPtr() const { return m_value.second(); }
//
//private:
//    compressed_pair<_Alty, _Ptr> m_value;
//};

_WJR_END

#endif // __WJR_SMART_POINTER_WRAPPER_H